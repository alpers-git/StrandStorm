#include <Mesh.hpp>
#include <Logging.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

uint64_t cantor(uint32_t x, uint32_t y) {
    return ((x + y) * (x + y + 1u)) / 2u + y;
}

// --- HairMesh --------------------------------------------------------------

void HairMesh::build(const OpenGLProgram &prog)
{
    spdlog::assrt(!this->vaoInitialized, "HairMesh already built");
    this->vaoInitialized = true;
    glGenVertexArrays(1, &this->vao) $gl_chk;
    glBindVertexArray(vao) $gl_chk;
    this->vboInterp = gl::buffer(GL_ARRAY_BUFFER, numInterpVertices() * sizeof(glm::vec4));
    this->eboInterp = gl::buffer(GL_ELEMENT_ARRAY_BUFFER, numInterpElements() * sizeof(GLuint));
    this->vboControl = gl::buffer(GL_ARRAY_BUFFER, this->controlVerts, GL_DYNAMIC_DRAW);
    
    prog.SetAttribPointer(vboInterp, "vPos", 4, GL_FLOAT);
}

void HairMesh::loadFromFile(const std::string &modelPath, bool compNormals)
{
    spdlog::assrt(fs::exists(modelPath), "model '{}' not found", modelPath);

    // Load the mesh
    cyTriMesh mesh;
    if (!mesh.LoadFromFileObj(modelPath.c_str())) {
        spdlog::error("Failed to load mesh from file: {}", modelPath);
    }

    // Grow the control hairs from the stored vertices
    for (int i = 0; i < mesh.NV(); i++) {
        growControlHair(glm::make_vec3(mesh.V(i)), glm::make_vec3(mesh.VN(i)));
    }

    spdlog::debug("{}: {} control hairs, {} output vertices, {} output elems, {} bezier control points",
        modelPath, numHairs(), numInterpVertices(), numInterpElements(), numControlPoints());
}

void HairMesh::draw(const OpenGLProgram &prog)
{
    glBindVertexArray(this->vao) $gl_chk;
    glBindBuffer(GL_ARRAY_BUFFER, this->vboInterp) $gl_chk;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboInterp) $gl_chk;
    glDrawElements(GL_LINES, numInterpElements(), GL_UNSIGNED_INT, nullptr) $gl_chk;
}

void HairMesh::bindToComputeShader(ComputeShader &cs) const
{
    assert(this->vaoInitialized);
    cs.assocBuffer(0u, this->vboControl);
    cs.createBuffer(1u, (3 * controlHairLen - 2) * numHairs() * sizeof(glm::vec4));
    cs.assocBuffer(2u, this->vboInterp);
    cs.assocBuffer(3u, this->eboInterp);
    cs.setUniform("N", controlHairLen);
    cs.setUniform("M", subdivide);
}

void HairMesh::growControlHair(const glm::vec3 &root, const glm::vec3 &dir)
{
    glm::vec3 v = root;
    for (int i = 0; i < controlHairLen; i++) {
        this->controlVerts.push_back({v, 1.0f});
        v += dir * 0.1f + rng.vec(glm::vec3(-1.0f), glm::vec3(1.0f)) * 0.1f;
    }
}

// --- SurfaceMesh -----------------------------------------------------------

void SurfaceMesh::loadFromFile(const std::string &modelPath, bool compNormals)
{
    spdlog::assrt(fs::exists(modelPath), "model '{}' not found", modelPath);

    // Load the mesh
    cyTriMesh mesh;
    if (!mesh.LoadFromFileObj(modelPath.c_str())) {
        spdlog::error("Failed to load mesh from file: {}", modelPath);
    }
    if (compNormals || mesh.NVN() == 0) {
        mesh.ComputeNormals();
    }

    // Mapping from vertex index to (normalIdx, texIdx)
    std::vector<std::tuple<int, int>> indices(mesh.NV(), {-1, -1});
    // Remapping (normalIdx, texIdx) -> new vertex index
    std::unordered_map<size_t, size_t> vertRemap({});
    // Map mixed indices to flat indices
    this->indices.resize(mesh.NF() * 3);
    this->vertices.resize(mesh.NV());
    this->normals.resize(mesh.NV());
    this->textureCoords.resize(mesh.NV());
    for (size_t i = 0; i < mesh.NF(); i++) {
        const uint32_t* normalIndices = mesh.FN(i).v;
        const uint32_t* texIndices = mesh.FT(i).v;
        const uint32_t* vertIndices = mesh.F(i).v;
        for (size_t j = 0; j < 3; j++) {
            uint32_t vertIdx = vertIndices[j];
            const uint32_t inNormalIdx = normalIndices[j];
            const uint32_t inTexIdx = texIndices[j];
            const uint32_t combinedAttrIdx = cantor(inNormalIdx, inTexIdx);
            if (vertRemap.count(combinedAttrIdx)) {
                // Vertex has already been remapped, so use the stored remapped vertex index
                vertIdx = vertRemap.at(combinedAttrIdx);
            } else {
                const int storedNormalIdx = std::get<0>(indices[vertIdx]);
                const int storedTexIdx = std::get<1>(indices[vertIdx]);
                if (storedNormalIdx != (int)inNormalIdx || storedTexIdx != (int)inTexIdx) {
                    const uint32_t oldVertIdx = vertIdx;
                    // Vertex either hasn't been stored, or hasn't been remapped
                    if (storedNormalIdx == -1 || storedTexIdx == -1) {
                        // First encounter with this vertex, store it for the first time
                        indices[vertIdx] = {inNormalIdx, inTexIdx};
                    } else {
                        // Stored index set doesn't match, so we dupe and remap
                        vertIdx = vertRemap.emplace(combinedAttrIdx, this->vertices.size()).first->second;
                        this->vertices.push_back({});
                        this->normals.push_back({});
                        this->textureCoords.push_back({});
                    }
                    this->vertices[vertIdx] = glm::make_vec3(&mesh.V(oldVertIdx)[0]);
                    this->normals[vertIdx] = glm::make_vec3(&mesh.VN(inNormalIdx)[0]);
                    this->textureCoords[vertIdx] = glm::make_vec2(&mesh.VT(inTexIdx)[0]);
                }
            }
            this->indices[i*3 + j] = vertIdx;
        }
    }
}

void SurfaceMesh::build(const OpenGLProgram &prog)
{
    spdlog::assrt(!this->vaoInitialized, "SurfaceMesh already built");
    this->vaoInitialized = true;
    glGenVertexArrays(1, &this->vao) $gl_chk;
    glBindVertexArray(vao) $gl_chk;
    this->vboVertices = gl::buffer(GL_ARRAY_BUFFER, this->vertices);
    this->vboNormals = gl::buffer(GL_ARRAY_BUFFER, this->normals);
    this->vboTexCoords = gl::buffer(GL_ARRAY_BUFFER, this->textureCoords);
    this->ebo = gl::buffer(GL_ELEMENT_ARRAY_BUFFER, this->indices);
    
    prog.SetAttribPointer(vboVertices, "vPos", 3, GL_FLOAT);
    prog.SetAttribPointer(vboNormals, "vNormal", 3, GL_FLOAT);
}

void SurfaceMesh::draw(const OpenGLProgram &prog)
{
    glBindVertexArray(this->vao) $gl_chk;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo) $gl_chk;
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices) $gl_chk;
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, nullptr) $gl_chk;
}
