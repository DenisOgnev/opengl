#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <array>


const std::string WINDOW_NAME = "OpenGL";
const int32_t WIDTH = 800;
const int32_t HEIGHT = 600;

const std::array vertices = {
     0.5f,  0.5f,  0.0f,
     0.5f, -0.5f,  0.0f, 
    -0.5f, -0.5f,  0.0f,
    -0.5f,  0.5f,  0.0f
};

const std::array indices = {
    0, 1, 3,
    1, 2, 3
};


void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);


class OpenGlApp
{
public:
    OpenGlApp(std::string _window_name, int32_t _width, int32_t _height) : window_name(_window_name), width(_width), height(_height) { }


    void run()
    {
        init();
        create_objects();
        main_loop();
    }


    ~OpenGlApp()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }


private:
    const std::string window_name;
    const int32_t width;
    const int32_t height;

    GLFWwindow* window;

    uint32_t VAO;
    uint32_t VBO;
    uint32_t EBO;
    uint32_t vert_shader;
    uint32_t frag_shader;
    uint32_t shader_program;
    

    void main_loop()
    {
        while (!glfwWindowShouldClose(window))
        {
            process_input();

            render();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }


    void init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
        if (window == NULL)
        {
            throw std::runtime_error("Failed to create window");
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }


    int clamp(int32_t value, int32_t lowest, int32_t highest)
    {
        if (value > highest)
        {
            return highest;
        }
        if (value < lowest)
        {
            return lowest;
        }
        return value;
    }


    void process_input()
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }


    void create_objects()
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

        std::ifstream vert("../../src/vert_shader.vert");
        std::ifstream frag("../../src/frag_shader.frag");
        std::stringstream vert_buffer;
        std::stringstream frag_buffer;
        vert_buffer << vert.rdbuf();
        frag_buffer << frag.rdbuf();
        vert.close();
        frag.close();
        std::string vert_buffer_string = vert_buffer.str();
        std::string frag_buffer_string = frag_buffer.str();
        const char* vert_buffer_source = vert_buffer_string.c_str();
        const char* frag_buffer_source = frag_buffer_string.c_str();

        int success;
        char info_log[512];

        vert_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_shader, 1, &vert_buffer_source, nullptr);
        glCompileShader(vert_shader);
        
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vert_shader, 512, nullptr, info_log);
            std::cout << info_log << "\n";
            throw std::runtime_error("Vertex shader didn't compile");
        }

        frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_shader, 1, &frag_buffer_source, nullptr);
        glCompileShader(frag_shader);

        glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(frag_shader, 512, nullptr, info_log);
            std::cout << info_log << "\n";
            throw std::runtime_error("Fragment shader didn't compile");
        }

        shader_program = glCreateProgram();
        glAttachShader(shader_program, vert_shader);
        glAttachShader(shader_program, frag_shader);
        glLinkProgram(shader_program);

        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
            std::cout << info_log << "\n";
            throw std::runtime_error("Shader program didn't link");
        }
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

        glUseProgram(shader_program);
    }


    void render()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }
};


int main()
{
    OpenGlApp app(WINDOW_NAME, WIDTH, HEIGHT);
    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height)
{
    glViewport(0, 0, width, height);
}