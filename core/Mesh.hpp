#pragma once

#include <OpenGLProgram.hpp>
#include <cyTriMesh.h>
#include <Renderer.hpp>
#include <unordered_map>
#include <string>

class ComputeShader;

class Mesh
{
protected:
    bool vaoInitialized = false;
    GLuint vao = GL_INVALID_INDEX;
public:
    Mesh() = default;

    virtual void loadFromFile(const std::string& modelPath, bool compNormals = true) = 0;
    virtual void build(const OpenGLProgram& prog) = 0;
    virtual void draw(const OpenGLProgram& prog) = 0;

    inline bool isVaoInitialized() const { return vaoInitialized; }
};

class HairMesh : public Mesh
{
private:
    // Random number gen for making control hairs
    RNG rng = {0};
    // Vertices for control hairs
    std::vector<glm::vec4> controlVerts;
    // Triangles for interpolating hairs
    std::vector<GLuint> tris;
    // VBO for control hairs
    GLuint vboControl = GL_INVALID_INDEX;
    // VBO for interpolated hairs
    GLuint vboInterp = GL_INVALID_INDEX;
    // EBO for interpolated hairs
    GLuint eboInterp = GL_INVALID_INDEX;
    // EBO for triangles
    GLuint eboTris = GL_INVALID_INDEX;

    // Grow control hair from a root position and direction, adding to my vertices and indices
    void growControlHair(const glm::vec3& root, const glm::vec3& dir);
public:
    // Number of vertices in each control hair (N)
    static constexpr uint32_t controlHairLen = 5;
    // Number of subdivisions between each control hair vertex (M)
    //  Includes start vertex, so needs to be >=1
    static constexpr uint32_t subdivide = 3;
    // Number of additional hairs to interpolate across each face
    static constexpr uint32_t interpDensity = 6;
    // Hair growth amount per control vert
    static constexpr float hairGrowth = 0.05f;
    // Debug control hair duplication
    static constexpr uint32_t controlHairDensity = 0;

    bool drawControlHairs = false;

    HairMesh() = default;

    void build(const OpenGLProgram& prog) override;
    void loadFromFile(const std::string& modelPath, bool compNormals = true) override;
    void draw(const OpenGLProgram& prog) override;

    void bindToComputeShader(ComputeShader& cs) const;

    // Returns number of control hairs (H)
    inline size_t numControlHairs() const {
        return controlVerts.size() / controlHairLen;
    }
    // Returns total number of triangles
    inline size_t numTris() const {
        return tris.size() / 3;
    }
    // Returns total number of interpolated hairs
    inline size_t numInterpHairs() const {
        return interpDensity * numTris() + numControlHairs();
    }
    // Number of elements in the interpolated hair buffer
    inline size_t numInterpElements() const {
        return 2 * subdivide * (controlHairLen - 1) * numInterpHairs();
    }
    // Number of interpolated vertices (elements in vboInterp)
    inline size_t numInterpVertices() const {
        return (subdivide * (controlHairLen - 1) + 1) * numInterpHairs();
    }
    // Number of bezier control points
    inline size_t numControlPoints() const {
        return (3 * controlHairLen - 2) * numControlHairs();
    }

    glm::vec4 color = glm::vec4(0.57f, 0.48f, 0.0f, 1.0f);
    bool show = true;
    bool shadowsEnable = true;
};

class SurfaceMesh : public Mesh
{
private:
    GLuint vboNormals = GL_INVALID_INDEX;
    GLuint vboTexCoords = GL_INVALID_INDEX;
    GLuint vboVertices = GL_INVALID_INDEX;
    GLuint ebo = GL_INVALID_INDEX;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
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
    void loadFromFile(const std::string& modelPath, bool compNormals = true) override;
    // Builds the buffers
    void build(const OpenGLProgram& prog) override;
    // Draws the hair mesh
    void draw(const OpenGLProgram& prog) override;
};
