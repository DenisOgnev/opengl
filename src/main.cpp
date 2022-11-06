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

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    float rotate_angle = 30.0f;
    float delta_time = 0.0f;
    

    void main_loop()
    {
        while (!glfwWindowShouldClose(window))
        {
            update_variables();
            
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

        shader.init();
        shader.use();

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }


    void create_objects()
    {
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.7f));
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        projection = glm::perspective(glm::radians(45.0f), width / static_cast<float>(height), 0.1f, 100.0f);

        shader.set_mat4("model", model);
        shader.set_mat4("view", view);
        shader.set_mat4("projection", projection);

        float radius = 0.5f;
        uint32_t segments = 3;
        uint32_t ring_segments = 4;
        
        mesh = get_sphere_mesh(segments, ring_segments, radius);


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

        glDrawElements(GL_TRIANGLES, mesh.indices_size, GL_UNSIGNED_INT, nullptr);
        // glDrawArrays(GL_POINTS, 0, vertices.size());
    }


    void update_variables()
    {
        delta_time = static_cast<float>(glfwGetTime());
        glfwSetTime(0.0);
    }


    void process_input()
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            model = glm::rotate(model, glm::radians(rotate_angle * delta_time), glm::vec3(0.0f, 1.0f, 0.0f));
            shader.set_mat4("model", model);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            model = glm::rotate(model, glm::radians(-rotate_angle * delta_time), glm::vec3(0.0f, 1.0f, 0.0f));
            shader.set_mat4("model", model);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            model = glm::rotate(model, glm::radians(rotate_angle * delta_time), glm::vec3(1.0f, 0.0f, 0.0f));
            shader.set_mat4("model", model);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            model = glm::rotate(model, glm::radians(-rotate_angle * delta_time), glm::vec3(1.0f, 0.0f, 0.0f));
            shader.set_mat4("model", model);
        }
    }


    Mesh get_circle_mesh(uint32_t vertices_count, float radius)
    {
        if (vertices_count < 3 || radius <= 0.0f)
        {
            throw std::runtime_error("Wrong parameters");
        }
        Mesh result_mesh;
        result_mesh.vertices_size = vertices_count + 1;
        result_mesh.indices_size = vertices_count * 3;
        result_mesh.vertices = std::make_unique<Vertex[]>(result_mesh.vertices_size);
        result_mesh.indices = std::make_unique<uint32_t[]>(result_mesh.indices_size);

        float angle = 360 / static_cast<float>(vertices_count);

        result_mesh.vertices[0] = Vertex{glm::vec3(0.0f), glm::vec3((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f)};
        
        for (uint32_t i = 0; i < vertices_count; i++)
        {
            float current_angle = glm::radians(static_cast<float>(i) * angle);
            
            float x = radius * std::cosf(current_angle);
            float y = radius * std::sinf(current_angle);

            result_mesh.vertices[i + 1] = Vertex{glm::vec3(x, y, 0.0f), glm::vec3((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f)};

            result_mesh.indices[i * 3] = 0;
            result_mesh.indices[i * 3 + 1] = i + 1;
            result_mesh.indices[i * 3 + 2] = (i + 1) % vertices_count + 1;
        }
        
        return result_mesh;
    }


    Mesh get_sphere_mesh(uint32_t segments, uint32_t ring_segments, float radius)
    {
        if (segments < 2 || ring_segments < 3 || radius <= 0.0f)
        {
            throw std::runtime_error("Wrong parameters");
        }
        Mesh result_mesh;
        result_mesh.vertices_size = ring_segments * (segments - 1) + 2;
        result_mesh.indices_size = 6 * ring_segments * (segments - 1);
        result_mesh.vertices = std::make_unique<Vertex[]>(result_mesh.vertices_size);
        result_mesh.indices = std::make_unique<uint32_t[]>(result_mesh.indices_size);

        float alpha_step = 180 / static_cast<float>(segments);
        float beta_step = 360 / static_cast<float>(ring_segments);

        uint32_t count = 0;
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
                
                result_mesh.vertices[count] = Vertex{glm::vec3(x, y, z), glm::vec3((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f)};
                count++;

                if (i == 0 || i == segments)
                {
                    break;
                }
            }
        }

        count = 0;
        for (uint32_t i = 0; i < segments; i++)
        {
            for (uint32_t j = 0; j < ring_segments; j++)
            {
                if (i == 0)
                {
                    indices.push_back(i);
                    indices.push_back(i + j + 1);
                    if (j == ring_segments - 1)
                        indices.push_back((i + j + 2) % ring_segments);
                    else
                        indices.push_back(i + j + 2);

                    result_mesh.indices[count] = i;
                    result_mesh.indices[count + 1] = i + j + 1;
                    if (j == ring_segments - 1)
                        result_mesh.indices[count + 2] = (i + j + 2) % ring_segments;
                    else
                        result_mesh.indices[count + 2] = i + j + 2;
                    
                    count += 3;
                }
                else if (i == segments - 1)
                {
                    result_mesh.indices[count] = ring_segments * i + 1;
                    result_mesh.indices[count + 1] = ring_segments * (i - 1) + j + 1;
                    if (j == ring_segments - 1)
                        result_mesh.indices[count + 2] = ring_segments * (i - 1) + (j + 2) % ring_segments;
                    else
                        result_mesh.indices[count + 2] = ring_segments * (i - 1) + j + 2;

                    count += 3;
                }
                else
                {
                    result_mesh.indices[count] = ring_segments * (i - 1) + j + 1;
                    if (j == ring_segments - 1)
                    {
                        result_mesh.indices[count + 1] = ring_segments * (i - 1) + (j + 2) % ring_segments;
                        result_mesh.indices[count + 2] = ring_segments * (i - 1) + (j + 2);
                    }
                    else
                    {
                        result_mesh.indices[count + 1] = ring_segments * (i - 1) + (j + 2);
                        result_mesh.indices[count + 2] = ring_segments * (i - 1) + j + 2 + ring_segments;
                    }

                    result_mesh.indices[count + 3] = ring_segments * (i - 1) + j + 1;
                    if (j == ring_segments - 1)
                    {
                        result_mesh.indices[count + 4] = ring_segments * (i - 1) + (j + 2 + (ring_segments - 1));
                        result_mesh.indices[count + 5] = ring_segments * (i - 1) + (j + 2);
                    }
                    else
                    {
                        result_mesh.indices[count + 4] = ring_segments * (i - 1) + (j + 2 + (ring_segments - 1));
                        result_mesh.indices[count + 5] = ring_segments * (i - 1) + j + 2 + ring_segments;
                    }

                    count += 6;
                }
            }
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

        projection = glm::perspective(glm::radians(45.0f), width / static_cast<float>(height), 0.1f, 100.0f);
        shader.set_mat4("projection", projection);
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


