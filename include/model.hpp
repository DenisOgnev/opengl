#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstdint>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "shader.hpp"
#include "mesh.hpp"
#include "transform.hpp"


class Model
{
private:
    Mesh mesh {};

    uint32_t VAO {};
    uint32_t VBO {};

    Shader shader {};

    Transform transform {};

    void init_shader()
    {
        shader.init();
        shader.use();
    };

    void create_buffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    };

    void bind_buffers()
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(sizeof(float) * 6));
        glEnableVertexAttribArray(2);
    };

    void set_transform()
    {
        shader.set_mat4("model", transform.model);
        shader.set_mat4("view", transform.view);
        shader.set_mat4("projection", transform.projection);
    };
public:
    Model() = default;
    Model(const Mesh& _mesh, const Transform& _transform, const std::string& vertex_shader_path, const std::string& fragment_shader_path) : mesh{_mesh}, shader{vertex_shader_path, fragment_shader_path}, transform{_transform}
    {
        init_shader();
        create_buffers();
        bind_buffers();
        set_transform();
    };
    ~Model()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    };

    void update_transform(const Transform& _transform)
    {
        transform = _transform;
        set_transform();
    };

    void draw()
    {
        shader.use();
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
    };
};

#endif