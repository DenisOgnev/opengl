#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>
#include <vector>

#include <glm/vec3.hpp>

struct Vertex
{
    glm::vec3 positions {};
    glm::vec3 colors {};
    glm::vec3 normals {};
};

struct Mesh
{
    std::vector<Vertex> vertices {};
};

#endif