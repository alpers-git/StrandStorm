#include <Mesh.hpp>
#include <Logging.hpp>
#include <glm/gtc/type_ptr.hpp>

// void Mesh::loadFromFile(const std::string& modelPath, bool compNormals)
// {
//     if (modelPath.empty() || !fs::exists(modelPath)) {
//         spdlog::error("Model path is incorrect");
//         return;
//     }

//     // Load the mesh
//     cyTriMesh mesh;
//     if (!mesh.LoadFromFileObj(modelPath.c_str())) {
//         spdlog::error("Failed to load mesh from file: {}", modelPath);
//         return;
//     }

//     this->vertices.resize(mesh.NV());
//     for (int i = 0; i < mesh.NV(); i++) {
//         this->vertices[i] = glm::vec3(mesh.V(i).x, mesh.V(i).y, mesh.V(i).z);
//     }

//     this->indices.resize(mesh.NF() * 3);
//     for (int i = 0; i < mesh.NF(); i++) {
//         this->indices[i + 0] = (mesh.F(i).v[0]);
//         this->indices[i + 1] = (mesh.F(i).v[1]);
//         this->indices[i + 2] = (mesh.F(i).v[2]);
//     }

//     if (!isVAOInitialized())
//         glGenVertexArrays(1, &this->vao)$gl_chk;

//     glGenBuffers(1, &this->vbo)$gl_chk;

//     glGenBuffers(1, &this->ebo)$gl_chk;

// }

uint64_t cantor(uint32_t x, uint32_t y) {
    return ((x + y) * (x + y + 1u)) / 2u + y;
}

void Mesh::build(const OpenGLProgram &prog)
{
    if (!isVAOInitialized())
        throw std::runtime_error("VAO is not initalized. Did you forget to call build before initializing VAO?");
    glBindVertexArray(vao) $gl_chk;
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo) $gl_chk;
    glBufferData(GL_ARRAY_BUFFER,
                 this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW) $gl_chk;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo) $gl_chk;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW) $gl_chk;
    GLuint attrib_vPos = prog.AttribLocation("vPos");
    glEnableVertexAttribArray(attrib_vPos) $gl_chk;
    glVertexAttribPointer(attrib_vPos, 3, GL_FLOAT, GL_FALSE, 0, 0) $gl_chk;
}

void HairMesh::loadFromFile(const std::string &modelPath, bool compNormals)
{
    if (modelPath.empty() || !fs::exists(modelPath))
    {
        spdlog::error("Model path is incorrect");
        return;
    }

    // Load the mesh
    cyTriMesh mesh;
    if (!mesh.LoadFromFileObj(modelPath.c_str()))
    {
        spdlog::error("Failed to load mesh from file: {}", modelPath);
        return;
    }
    if (compNormals || mesh.NVN() == 0)
        mesh.ComputeNormals();

    // Grow the control hairs from the stored vertices
    this->vertices.reserve(mesh.NV());
    for (int i = 0; i < mesh.NV(); i++) {
        glm::vec3 dir(mesh.VN(i).x, mesh.VN(i).y, mesh.VN(i).z);
        if (glm::dot(dir, {0.0f, 1.0f, 0.0f}) > 0.0f)
            this->growControlHair(
                {mesh.V(i).x, mesh.V(i).y, mesh.V(i).z},
                dir);
    }

    glGenVertexArrays(1, &this->vao);
    $gl_chk;
    this->vaoInitialized = true;
    glGenBuffers(1, &this->vbo);
    $gl_chk;

    glGenBuffers(1, &this->ebo);
    $gl_chk;
}

// void HairMesh::build(const OpenGLProgram& prog)
// {
//     glBindVertexArray(vao)$gl_ch;k

//     glBindBuffer(GL_ARRAY_BUFFER, this->vbo)$gl_chk;

//     glBufferData(GL_ARRAY_BUFFER,
//         this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW)$gl_ch;k

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo)$gl_chk;

//     glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//         this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW)$gl_ch;k

//     GLuint attrib_vPos = prog.AttribLocation("vPos");
//     glEnableVertexAttribArray(attrib_vPos)$gl_chk;

//     glVertexAttribPointer(attrib_vPos, 3, GL_FLOAT, GL_FALSE, 0u, (void*)0u)$gl_ch;k

//     glBindBuffer(GL_ARRAY_BUFFER, GL_NONE)$gl_chk;

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE)$gl_chk;

// }

void HairMesh::draw(const OpenGLProgram &prog)
{
    glBindVertexArray(this->vao);
    $gl_chk;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    $gl_chk;

    glDrawElements(GL_LINES, this->indices.size(), GL_UNSIGNED_INT, nullptr);
    $gl_chk;
}

void HairMesh::growControlHair(const glm::vec3 &root, const glm::vec3 &dir)
{
    glm::vec3 v = root;
    for (int i = 0; i < 5; i++) {
        this->indices.push_back(this->vertices.size());
        this->vertices.push_back(v);
        this->indices.push_back(this->vertices.size());
        v += dir * 0.1f + rng.vec(glm::vec3(-1.0f), glm::vec3(1.0f)) * 0.05f;
    }
    this->vertices.push_back(v);
}

//----------------------------------------------------------------------

void SurfaceMesh::loadFromFile(const std::string &modelPath, bool compNormals)
{
    if (modelPath.empty() || !fs::exists(modelPath))
    {
        spdlog::error("Model path is incorrect");
        return;
    }

    // Load the mesh
    cyTriMesh mesh;
    if (!mesh.LoadFromFileObj(modelPath.c_str()))
    {
        spdlog::error("Failed to load mesh from file: {}", modelPath);
        return;
    }
    if (compNormals || mesh.NVN() == 0)
        mesh.ComputeNormals();

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
    // create a triangle mesh indexed into indices list
    glGenVertexArrays(1, &this->vao) $gl_chk;
    this->vaoInitialized = true;
    glGenBuffers(1, &this->vbo) $gl_chk;
    glGenBuffers(1, &this->normalVbo) $gl_chk;
    glGenBuffers(1, &this->textureVbo) $gl_chk;
    glGenBuffers(1, &this->ebo) $gl_chk;
}

void SurfaceMesh::build(const OpenGLProgram &prog)
{
    this->Mesh::build(prog);
    glBindVertexArray(this->vao) $gl_chk;
    // Bind normal buffer and set attribute pointer
    glBindBuffer(GL_ARRAY_BUFFER, this->normalVbo) $gl_chk;
    glBufferData(GL_ARRAY_BUFFER,
                 this->normals.size() * sizeof(glm::vec3), this->normals.data(), GL_STATIC_DRAW) $gl_chk;
    GLuint attrib_vNormal = prog.AttribLocation("vNormal");
    glEnableVertexAttribArray(attrib_vNormal) $gl_chk;
    glVertexAttribPointer(attrib_vNormal, 3, GL_FLOAT, GL_FALSE, 0u, (void *)0u) $gl_chk;

    // Bind normal buffer and set attribute pointer
    // glBindBuffer(GL_ARRAY_BUFFER, this->textureVbo) $gl_chk;
    // glBufferData(GL_ARRAY_BUFFER,
    //              this->textureCoords.size() * sizeof(glm::vec2), this->textureCoords.data(), GL_STATIC_DRAW) $gl_chk;
    // GLuint attrib_vTexCoord = prog.AttribLocation("vTexCoord");
    // glEnableVertexAttribArray(attrib_vTexCoord) $gl_chk;
    // glVertexAttribPointer(attrib_vTexCoord, 2, GL_FLOAT, GL_FALSE, 0u, (void *)0u) $gl_chk;

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE) $gl_chk;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE) $gl_chk;
}

void SurfaceMesh::draw(const OpenGLProgram &prog)
{
    glBindVertexArray(this->vao) $gl_chk;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo) $gl_chk;

    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, nullptr) $gl_chk;
}
