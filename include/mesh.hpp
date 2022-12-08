#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>
#include <vector>

#include <glm/vec3.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<int32_t> indices;
    glm::vec3 color;
};

Mesh get_sphere_mesh(uint32_t segments, uint32_t ring_segments, float radius, const glm::vec3 &color)
{
    if (segments < 2 || ring_segments < 3 || radius <= 0.0f)
    {
        throw std::runtime_error("Wrong parameters");
    }
    Mesh result_mesh;
    result_mesh.vertices.reserve(ring_segments * (segments - 1) + 2);
    result_mesh.indices.reserve(6 * ring_segments * (segments - 1));
    result_mesh.color = color;

    float alpha_step = 180 / static_cast<float>(segments);
    float beta_step = 360 / static_cast<float>(ring_segments);

    for (uint32_t i = 0; i <= segments; i++)
    {
        float alpha = glm::radians(i * alpha_step);
        float sin_alpha = std::sinf(alpha);
        float cos_alpha = std::cosf(alpha);
        float radius_dot_sin_alpha = radius * sin_alpha;
        float y = radius * cos_alpha;

        for (uint32_t j = 0; j < ring_segments; j++)
        {
            float beta = glm::radians(j * beta_step);
            float sin_beta = std::sinf(beta);
            float cos_beta = std::cosf(beta);
            float x = radius_dot_sin_alpha * sin_beta;
            float z = radius_dot_sin_alpha * cos_beta;

            result_mesh.vertices.push_back(Vertex{glm::vec3(x, y, z), glm::normalize(glm::vec3(x, y, z))});

            if (i == 0 || i == segments)
            {
                break;
            }
        }
    }

    for (uint32_t i = 0; i < segments; i++)
    {
        for (uint32_t j = 0; j < ring_segments; j++)
        {
            if (i == 0)
            {
                result_mesh.indices.push_back(i);
                result_mesh.indices.push_back(i + j + 1);
                if (j == ring_segments - 1)
                    result_mesh.indices.push_back((i + j + 2) % ring_segments);
                else
                    result_mesh.indices.push_back(i + j + 2);
            }
            else if (i == segments - 1)
            {
                result_mesh.indices.push_back(ring_segments * i + 1);
                result_mesh.indices.push_back(ring_segments * (i - 1) + j + 1);
                if (j == ring_segments - 1)
                    result_mesh.indices.push_back(ring_segments * (i - 1) + (j + 2) % ring_segments);
                else
                    result_mesh.indices.push_back(ring_segments * (i - 1) + j + 2);
            }
            else
            {
                result_mesh.indices.push_back(ring_segments * (i - 1) + j + 1);
                if (j == ring_segments - 1)
                {
                    result_mesh.indices.push_back(ring_segments * (i - 1) + (j + 2) % ring_segments);
                    result_mesh.indices.push_back(ring_segments * (i - 1) + (j + 2));
                }
                else
                {
                    result_mesh.indices.push_back(ring_segments * (i - 1) + (j + 2));
                    result_mesh.indices.push_back(ring_segments * (i - 1) + j + 2 + ring_segments);
                }

                result_mesh.indices.push_back(ring_segments * (i - 1) + j + 1);
                if (j == ring_segments - 1)
                {
                    result_mesh.indices.push_back(ring_segments * (i - 1) + (j + 2 + (ring_segments - 1)));
                    result_mesh.indices.push_back(ring_segments * (i - 1) + (j + 2));
                }
                else
                {
                    result_mesh.indices.push_back(ring_segments * (i - 1) + (j + 2 + (ring_segments - 1)));
                    result_mesh.indices.push_back(ring_segments * (i - 1) + j + 2 + ring_segments);
                }
            }
        }
    }

    return result_mesh;
};

#endif