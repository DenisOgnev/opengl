#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "model.hpp"

const std::string WINDOW_NAME{"OpenGL"};
constexpr int32_t WIDTH{1280};
constexpr int32_t HEIGHT{720};

class OpenGlApp
{
public:
    OpenGlApp(const std::string &_window_name, int32_t _width, int32_t _height) : window_name{_window_name}, width{_width}, height{_height} {};

    void run()
    {
        init();
        create_shaders();
        create_mvp_matrices();
        create_objects();
        main_loop();
    };

    ~OpenGlApp()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    };

private:
    const std::string window_name;
    int32_t width;
    int32_t height;

    GLFWwindow *window;

    Shader sphere_shader;
    Shader light_shader;

    std::vector<std::pair<const std::string, const std::string>> shaders_paths{
        {"../../src/shaders/vert_shader.vert", "../../src/shaders/frag_shader.frag"},
        {"../../src/shaders/light_shader.vert", "../../src/shaders/light_shader.frag"},
    };

    glm::vec3 camera_pos{0.0f, 0.0f, -1.0f};
    glm::mat4 view;
    glm::mat4 projection;

    float rotate_angle = 30.0f;
    float delta_time = 0.0f;
    float last_time = 0.0f;

    std::unique_ptr<Drawable> sphere;
    std::unique_ptr<Drawable> sphere2;

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
    };

    void init()
    {
        init_glfw();
        init_window();
        load_glad();
        set_opengl_parameters();
    };

    void init_glfw()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    };

    void init_window()
    {
        window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
        if (window == nullptr)
        {
            throw std::runtime_error("Failed to create window.");
        }
        glfwSetWindowUserPointer(window, this);
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    };

    void load_glad()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD.");
        }
    };

    void set_opengl_parameters()
    {
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);
    };

    void create_shaders()
    {
        sphere_shader = Shader(shaders_paths[0].first, shaders_paths[0].second);
        sphere_shader.init();

        light_shader = Shader(shaders_paths[1].first, shaders_paths[1].second);
        light_shader.init();
    };

    void create_mvp_matrices()
    {
        view = glm::translate(glm::mat4(1.0f), camera_pos);
        projection = glm::perspective(glm::radians(45.0f), width / static_cast<float>(height), 0.1f, 100.0f);
    }

    void create_objects()
    {
        uint32_t segments = 16;
        uint32_t ring_segments = 16;
        float radius = 0.5f;

        Mesh sphere_mesh = get_sphere_mesh(segments, ring_segments, radius, glm::vec3{0.5f, 0.1f, 0.2f});
        Transform transform{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), 0.0f, glm::vec3(0.3f)};
        sphere = std::make_unique<ModelIndexed>(sphere_mesh, transform, view, projection);

        transform.translate = glm::vec3(-0.5f, 0.0f, 0.0f);
        sphere2 = std::make_unique<ModelIndexed>(sphere_mesh, transform, view, projection);
    };

    void render()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sphere_shader.use();
        sphere_shader.set_vec3("light_color", glm::vec3(1.0f));
        sphere_shader.set_vec3("input_color", sphere->get_color());
        sphere_shader.set_vec3("light_position", sphere2->get_transform().translate);
        sphere_shader.set_vec3("view_position", -camera_pos);

        sphere->draw(sphere_shader);

        float x{static_cast<float>(cos(glfwGetTime())) / 2.0f};
        float z{static_cast<float>(sin(glfwGetTime())) / 2.0f};
        std::cout << cos(glfwGetTime()) << " " << sin(glfwGetTime()) << "\n";

        auto transform{sphere2->get_transform()};
        transform.translate = glm::vec3(x, 0.0f, z);

        sphere2->update_transform(transform);

        sphere2->draw(light_shader);
    };

    void update_variables()
    {
        delta_time = static_cast<float>(glfwGetTime()) - last_time;
        last_time = static_cast<float>(glfwGetTime());
    };

    void process_input()
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            sphere->rotate(glm::vec3(0.0f, 1.0f, 0.0f), rotate_angle * delta_time);
            sphere2->rotate(glm::vec3(0.0f, 1.0f, 0.0f), rotate_angle * delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            sphere->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -rotate_angle * delta_time);
            sphere2->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -rotate_angle * delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            sphere->rotate(glm::vec3(1.0f, 0.0f, 0.0f), rotate_angle * delta_time);
            sphere2->rotate(glm::vec3(1.0f, 0.0f, 0.0f), rotate_angle * delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            sphere->rotate(glm::vec3(1.0f, 0.0f, 0.0f), -rotate_angle * delta_time);
            sphere2->rotate(glm::vec3(1.0f, 0.0f, 0.0f), -rotate_angle * delta_time);
        }
    };

    static void framebuffer_size_callback(GLFWwindow *window, int32_t _width, int32_t _height)
    {
        auto app = reinterpret_cast<OpenGlApp *>(glfwGetWindowUserPointer(window));
        app->framebuffer_size_resize(_width, _height);
    };
    void framebuffer_size_resize(int32_t _width, int32_t _height)
    {
        width = _width;
        height = _height;
        glViewport(0, 0, width, height);

        projection = glm::perspective(glm::radians(45.0f), width / static_cast<float>(height), 0.1f, 100.0f);
        sphere_shader.set_mat4("projection", projection);
    };

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
    };
};

int main()
{
    OpenGlApp app{WINDOW_NAME, WIDTH, HEIGHT};
    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
