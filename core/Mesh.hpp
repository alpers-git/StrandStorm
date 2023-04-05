#pragma once

#include <OpenGLProgram.h>
#include <cyTriMesh.h>

using namespace ssCore;

class HairMesh
{
private:
    GLuint vbo;

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
    // Builds the VBO
    void build();
    // Draws the hair mesh
    void draw(const OpenGLProgram& prog);
};