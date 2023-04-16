#pragma once

#include <OpenGLProgram.hpp>
#include <cyTriMesh.h>
#include <Renderer.hpp>
#include <unordered_map>
#include <string>

class Mesh
{
protected:
    RNG rng = {0};
    bool vaoInitialized = false;
public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;

    Mesh() = default;

    virtual void loadFromFile(const std::string& modelPath, bool compNormals = true) = 0;
    void build(const OpenGLProgram& prog);
    virtual void draw(const OpenGLProgram& prog) = 0;

    //setters
    inline void setVAO(GLuint vao) { this->vao = vao; vaoInitialized = true; }
    //getters
    inline bool isVAOInitialized() const { return vaoInitialized; }
    inline GLuint getVAO() const { return vao; }
};

class HairMesh : public Mesh
{
private:
    // Grow control hair from a root position and direction, adding to my vertices and indices
    void growControlHair(const glm::vec3& root, const glm::vec3& dir);
public:
    static constexpr uint32_t controlHairLen = 21;
    static constexpr uint32_t subdivide = 7;

    HairMesh() = default;

    // Loads mesh from file
    void loadFromFile(const std::string& modelPath, bool compNormals = true);
    // Draws the hair mesh
    void draw(const OpenGLProgram& prog) override;
    // Returns number of hairs
    inline size_t numHairs() const { return vertices.size() / controlHairLen; }

    glm::vec4 color = glm::vec4(0.57f, 0.48f, 0.0f, 1.0f);
    bool show = true;
};

class SurfaceMesh : public Mesh
{
private:
    GLuint normalVbo;
    GLuint textureVbo;
public:
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    struct Material {
        glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
        float shininess = 32.0f;
    } material;

    SurfaceMesh() = default;

    // Loads mesh from file
    void loadFromFile(const std::string& modelPath, bool compNormals = true);
    // Builds the buffers
    void build(const OpenGLProgram& prog);
    // Draws the hair mesh
    void draw(const OpenGLProgram& prog);
};
