#pragma once

#include <OpenGLProgram.hpp>
#include <cyTriMesh.h>
#include <Renderer.hpp>

class HairMesh
{
private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    // Grow control hair from a root position and direction, adding to my vertices and indices
    void growControlHair(const glm::vec3& root, const glm::vec3& dir);
public:
    // Control hair vertex positions
    std::vector<glm::vec3> vertices;
    // Control hair vertex indices
    std::vector<GLuint> indices;

    HairMesh() = default;

    // Loads mesh from file
    void loadFromFile(const std::string& modelPath);
    // Builds the buffers
    void build(const OpenGLProgram& prog);
    // Draws the hair mesh
    void draw(const OpenGLProgram& prog);
};