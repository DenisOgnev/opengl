#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstdint>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "shader.hpp"
#include "mesh.hpp"
#include "transform.hpp"

class Drawable
{
protected:
    Mesh mesh;
    Transform transform;
    uint32_t VAO;
    uint32_t VBO;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    virtual void create_buffers()
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
    };
    void set_transform(const Shader& shader) const
    {
        shader.set_mat4("model", model);
        shader.set_mat4("view", view);
        shader.set_mat4("projection", projection);
    };
public:
    Drawable() = default;
    Drawable(const Mesh& _mesh, const Transform& _transform, const glm::mat4& _view, const glm::mat4& _projection) : mesh{_mesh}, transform{_transform}, view{_view}, projection{_projection}
    {
        model = glm::translate(glm::mat4(1.0f), transform.translate);
        model = glm::rotate(model, transform.angle, transform.rotate_axis);
        model = glm::scale(model, transform.scale);
    };
    virtual ~Drawable()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    };
    virtual void draw(Shader& shader) = 0;
    Transform get_transform() const
    {
        return transform;
    };
    void update_transform(const Transform& _transform)
    {
        transform = _transform;
        model = glm::translate(glm::mat4(1.0f), transform.translate);
        model = glm::rotate(model, transform.angle, transform.rotate_axis);
        model = glm::scale(model, transform.scale);
    };
    void update_view(const glm::mat4& _view)
    {
        view = _view;
    };
    void update_projection(const glm::mat4& _projection)
    {
        projection = _projection;
    };
    glm::vec3 get_color()
    {
        return mesh.color;
    };
    void rotate(const glm::vec3& axis, float angle)
    {
        model = glm::rotate(model, glm::radians(angle), axis);
    };
};

class Model : public Drawable
{
public:
    Model() = default;
    Model(const Mesh& mesh, const Transform& transform, const glm::mat4& view, const glm::mat4& projection) : Drawable{mesh, transform, view, projection}
    {
        create_buffers();
    };
    Model(const Model&) = delete;
    Model(Model&& model)
    {
        std::swap(mesh, model.mesh);
        std::swap(VAO, model.VAO);
        std::swap(VBO, model.VBO);
        std::swap(transform, model.transform);
    }
    ~Model() override {};

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&& model)
    {
        if (this == &model)
        {
            return *this;
        }

        std::swap(mesh, model.mesh);
        std::swap(VAO, model.VAO);
        std::swap(VBO, model.VBO);
        std::swap(transform, model.transform);

        return *this;
    };

    void draw(Shader& shader) override
    {
        set_transform(shader);
        glBindVertexArray(VAO);
        shader.use();
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
    };
};

class ModelIndexed : public Drawable
{
private:
    uint32_t EBO;

    void create_buffers() override
    {
        Drawable::create_buffers();

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
    };
public:
    ModelIndexed() = default;
    ModelIndexed(const Mesh& mesh, const Transform& transform, const glm::mat4& view, const glm::mat4& projection) : Drawable{mesh, transform, view, projection}
    {
        create_buffers();
    };
    ModelIndexed(const ModelIndexed&) = delete;
    ModelIndexed(ModelIndexed&& model)
    {
        std::swap(mesh, model.mesh);
        std::swap(VAO, model.VAO);
        std::swap(VBO, model.VBO);
        std::swap(EBO, model.EBO);
        std::swap(transform, model.transform);
    }
    ~ModelIndexed() override
    {
        glDeleteBuffers(1, &EBO);
    };

    ModelIndexed& operator=(const ModelIndexed&) = delete;
    ModelIndexed& operator=(ModelIndexed&& model)
    {
        if (this == &model)
        {
            return *this;
        }

        std::swap(mesh, model.mesh);
        std::swap(VAO, model.VAO);
        std::swap(VBO, model.VBO);
        std::swap(EBO, model.EBO);
        std::swap(transform, model.transform);

        return *this;
    };

    void draw(Shader& shader) override
    {
        glBindVertexArray(VAO);
        shader.use();
        set_transform(shader);
        glDrawElements(GL_TRIANGLES, static_cast<int32_t>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
    };
};

#endif