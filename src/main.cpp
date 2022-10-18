#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.hpp>

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <array>



const std::string WINDOW_NAME = "OpenGL";
int32_t WIDTH = 800;
int32_t HEIGHT = 600;

const std::string vertex_shader_path = "../../src/vert_shader.vert";
const std::string fragment_shader_path = "../../src/frag_shader.frag";


class OpenGlApp
{
public:
    OpenGlApp(std::string _window_name, int32_t _width, int32_t _height) : window_name(_window_name), width(_width), height(_height), shader(Shader(vertex_shader_path, fragment_shader_path)) { }


    void run()
    {
        init();
        create_objects();
        main_loop();
    }


    ~OpenGlApp()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glfwDestroyWindow(window);
        glfwTerminate();
    }


private:
    const std::string window_name;
    int32_t width;
    int32_t height;

    GLFWwindow* window;

    uint32_t VAO;
    uint32_t VBO;
    uint32_t EBO;
    Shader shader;

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    struct Mesh
    {
        uint32_t vertices_size;
        uint32_t indices_size;
        std::unique_ptr<Vertex[]> vertices;
        std::unique_ptr<uint32_t[]> indices;
    };
    
    Mesh mesh;
    

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
        if (window == nullptr)
        {
            throw std::runtime_error("Failed to create window.");
        }
        glfwSetWindowUserPointer(window, this);
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD.");
        }
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        shader.init();
        shader.use();
    }


    void create_objects()
    {
        Vertex vertex {glm::vec3(0.0f), glm::vec3(0.0f)};

        std::array<Vertex, 3> vertices =  {
            Vertex{glm::vec3( 0.0f,  0.5f,  0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
            Vertex{glm::vec3(-0.5f, -0.5f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
            Vertex{glm::vec3( 0.5f, -0.5f,  0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
        };

        std::array<uint32_t, 3> indices = {0, 1, 2};

        mesh = get_circle_mesh(8, 0.5f);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices_size, mesh.vertices.get(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh.indices_size, mesh.indices.get(), GL_STATIC_DRAW);
    }


    void render()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
    }


    void process_input()
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }


    Mesh get_circle_mesh(uint32_t vertices_count, float radius)
    {
        if (vertices_count < 3)
        {
            throw std::runtime_error("Wrong vertices count");
        }
        Mesh result_mesh;
        result_mesh.vertices_size = vertices_count + 1;
        result_mesh.indices_size = vertices_count * 3;
        result_mesh.vertices = std::make_unique<Vertex[]>(result_mesh.vertices_size);
        result_mesh.indices = std::make_unique<uint32_t[]>(result_mesh.indices_size);

        float angle = std::ceilf(360 / static_cast<float>(vertices_count));

        result_mesh.vertices[0] = Vertex{glm::vec3(0.0f), glm::vec3((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f)};
        
        for (uint32_t i = 0; i < vertices_count; i++)
        {
            float current_angle = glm::radians(static_cast<float>(i) * angle);
            
            float x = radius * cosf(current_angle);
            float y = radius * sinf(current_angle);

            result_mesh.vertices[i + 1] = Vertex{glm::vec3(x, y, 0.0f), glm::vec3((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f)};

            result_mesh.indices[i * 3] = 0;
            result_mesh.indices[i * 3 + 1] = i + 1;
            result_mesh.indices[i * 3 + 2] = (i + 1) % vertices_count + 1;
        }
        
        return result_mesh;
    }


    static void framebuffer_size_callback(GLFWwindow* window, int32_t _width, int32_t _height)
    {
        auto app = reinterpret_cast<OpenGlApp*>(glfwGetWindowUserPointer(window));
		app->framebuffer_size_resize(_width, _height);
    }
    void framebuffer_size_resize(int32_t _width, int32_t _height)
    {
        width = _width;
        height = _height;
        glViewport(0, 0, width, height);
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


