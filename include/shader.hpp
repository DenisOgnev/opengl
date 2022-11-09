#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
private:
    std::string vertex_path{};
    std::string fragment_path{};
    unsigned int ID{};

public:

    Shader() = default;
    Shader(const std::string &_vertex_path, const std::string &_fragment_path) : vertex_path{_vertex_path}, fragment_path{_fragment_path}{};

    Shader& operator=(Shader&& shader)
    {
        if (this == &shader)
        {
            return *this;
        }

        std::swap(vertex_path, shader.vertex_path);
        std::swap(fragment_path, shader.fragment_path);
        std::swap(ID, shader.ID);

        return *this;
    }

    void init()
    {
        std::string vertex_shader_code{};
        std::string fragment_shader_code{};
        std::ifstream vert_shader_file{};
        std::ifstream frag_shader_file{};
        vert_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        frag_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vert_shader_file.open(vertex_path);
            frag_shader_file.open(fragment_path);
            std::stringstream vert_shader_stream{};
            std::stringstream frag_shader_stream{};
            vert_shader_stream << vert_shader_file.rdbuf();
            frag_shader_stream << frag_shader_file.rdbuf();
            vert_shader_file.close();
            frag_shader_file.close();

            vertex_shader_code = vert_shader_stream.str();
            fragment_shader_code = frag_shader_stream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "File not succefully read\n";
        }

        const char *vertex_shader_source = vertex_shader_code.c_str();
        const char *fragment_shader_source = fragment_shader_code.c_str();

        uint32_t vertex{glCreateShader(GL_VERTEX_SHADER)};
        uint32_t fragment{glCreateShader(GL_FRAGMENT_SHADER)};

        glShaderSource(vertex, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex);

        glShaderSource(fragment, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment);

        int32_t error_status;
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &error_status);

        char error_log[512];
        if (!error_status)
        {
            glGetShaderInfoLog(vertex, 512, nullptr, error_log);
            std::cout << error_log << "\n";
            throw std::runtime_error("Vertex shader compilation failed.");
        }

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &error_status);
        if (!error_status)
        {
            glGetShaderInfoLog(fragment, 512, nullptr, error_log);
            std::cout << error_log << "\n";
            throw std::runtime_error("Fragment shader compilation failed.");
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &error_status);
        if (!error_status)
        {
            glGetProgramInfoLog(ID, 512, nullptr, error_log);
            std::cout << error_log << "\n";
            throw std::runtime_error("Shader link failed.");
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    };

    void use() { glUseProgram(ID); };
    void set_bool(const std::string &name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); };
    void set_int(const std::string &name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); };
    void set_float(const std::string &name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); };
    void set_vec2(const std::string &name, const glm::vec2 &value) const { glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); };
    void set_vec2(const std::string &name, float x, float y) const { glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); };
    void set_vec3(const std::string &name, const glm::vec3 &value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); };
    void set_vec3(const std::string &name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); };
    void set_vec4(const std::string &name, const glm::vec4 &value) const { glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); };
    void set_vec4(const std::string &name, float x, float y, float z, float w) const { glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); };
    void set_mat2(const std::string &name, const glm::mat2 &value) const { glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]); };
    void set_mat3(const std::string &name, const glm::mat3 &value) const { glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]); };
    void set_mat4(const std::string &name, const glm::mat4 &value) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]); };

    ~Shader()
    {
        if (ID)
        {
            glDeleteProgram(ID);
        }
    };
};

#endif