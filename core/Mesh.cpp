#include <Mesh.hpp>
#include <Logging.hpp>

void HairMesh::loadFromFile(const std::string &modelPath)
{
    assert(fs::exists(modelPath));

    // Load the mesh
    cyTriMesh mesh;
    if (!mesh.LoadFromFileObj(modelPath.c_str())) {
        spdlog::error("Failed to load mesh from file: {}", modelPath);
        return;
    }
    mesh.ComputeNormals();

    // Grow the control hairs from the stored vertices
    this->vertices.reserve(mesh.NV());
    for (int i = 0; i < mesh.NV(); i++) {
        this->growControlHair(
            {mesh.V(i).x, mesh.V(i).y, mesh.V(i).z},
            {mesh.VN(i).x, mesh.VN(i).y, mesh.VN(i).z});
    }

    GL_CALL(glGenVertexArrays(1, &this->vao));
    GL_CALL(glGenBuffers(1, &this->vbo));
    GL_CALL(glGenBuffers(1, &this->ebo));
}

void HairMesh::build(const OpenGLProgram& prog)
{
    GL_CALL(glBindVertexArray(vao));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER,
        this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW));

    GLuint attrib_vPos = prog.AttribLocation("vPos");
    GL_CALL(glEnableVertexAttribArray(attrib_vPos));
    GL_CALL(glVertexAttribPointer(attrib_vPos, 3, GL_FLOAT, GL_FALSE, 0u, (void*)0u));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, GL_NONE));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE));
}

void HairMesh::draw(const OpenGLProgram& prog)
{
    GL_CALL(glBindVertexArray(this->vao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo));
    GL_CALL(glDrawElements(GL_LINE_STRIP, this->indices.size(), GL_UNSIGNED_INT, nullptr));
}

void HairMesh::growControlHair(const glm::vec3& root, const glm::vec3& dir)
{
    glm::vec3 v = root;
    this->indices.push_back(this->vertices.size());
    for (int i = 0; i < 10; i++) {
        this->indices.push_back(this->vertices.size());
        this->vertices.push_back(v);
        v += dir * 0.01f;
    }
}