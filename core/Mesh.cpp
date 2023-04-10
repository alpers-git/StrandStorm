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

    glGenVertexArrays(1, &this->vao); $gl_chk
    glGenBuffers(1, &this->vbo); $gl_chk
    glGenBuffers(1, &this->ebo); $gl_chk
}

void HairMesh::build(const OpenGLProgram& prog)
{
    glBindVertexArray(vao); $gl_chk

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo); $gl_chk
    glBufferData(GL_ARRAY_BUFFER,
        this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW); $gl_chk

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo); $gl_chk
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW); $gl_chk

    GLuint attrib_vPos = prog.AttribLocation("vPos");
    glEnableVertexAttribArray(attrib_vPos); $gl_chk
    glVertexAttribPointer(attrib_vPos, 3, GL_FLOAT, GL_FALSE, 0u, (void*)0u); $gl_chk

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); $gl_chk
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); $gl_chk
}

void HairMesh::draw(const OpenGLProgram& prog)
{
    glBindVertexArray(this->vao); $gl_chk
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo); $gl_chk
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo); $gl_chk
    glDrawElements(GL_LINES, this->indices.size(), GL_UNSIGNED_INT, nullptr); $gl_chk
}

void HairMesh::growControlHair(const glm::vec3& root, const glm::vec3& dir)
{
    glm::vec3 v = root;
    for (int i = 0; i < 10; i++) {
        this->indices.push_back(this->vertices.size());
        this->vertices.push_back(v);
        this->indices.push_back(this->vertices.size());
        v += dir * 0.01f;
    }
    this->vertices.push_back(v);
}

//----------------------------------------------------------------------

void SurfaceMesh::loadFromFile(const std::string &modelPath)
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
    this->vertices.reserve(mesh.NF() * 3);
    this->indices.reserve(mesh.NF() * 3);
    for (int i = 0; i < mesh.NF(); i++) {
        this->indices.push_back(this->vertices.size());
        this->vertices.push_back({mesh.V(mesh.F(i).v[0]).x, mesh.V(mesh.F(i).v[0]).y, mesh.V(mesh.F(i).v[0]).z});
        this->indices.push_back(this->vertices.size());
        this->vertices.push_back({mesh.V(mesh.F(i).v[1]).x, mesh.V(mesh.F(i).v[1]).y, mesh.V(mesh.F(i).v[1]).z});
        this->indices.push_back(this->vertices.size());
        this->vertices.push_back({mesh.V(mesh.F(i).v[2]).x, mesh.V(mesh.F(i).v[2]).y, mesh.V(mesh.F(i).v[2]).z});
    }

    glGenVertexArrays(1, &this->vao); $gl_chk
    glGenBuffers(1, &this->vbo); $gl_chk
    glGenBuffers(1, &this->ebo); $gl_chk
}

void SurfaceMesh::build(const OpenGLProgram& prog)
{
    glBindVertexArray(vao); $gl_chk

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo); $gl_chk
    glBufferData(GL_ARRAY_BUFFER,
        this->vertices.size() * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW); $gl_chk

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo); $gl_chk
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW); $gl_chk

    GLuint attrib_vPos = prog.AttribLocation("vPos");
    glEnableVertexAttribArray(attrib_vPos); $gl_chk
    glVertexAttribPointer(attrib_vPos, 3, GL_FLOAT, GL_FALSE, 0u, (void*)0u); $gl_chk

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); $gl_chk
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); $gl_chk
}

void SurfaceMesh::draw(const OpenGLProgram& prog)
{
    glBindVertexArray(this->vao); $gl_chk
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo); $gl_chk
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo); $gl_chk
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, nullptr); $gl_chk
}