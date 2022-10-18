#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.hpp>
#include <camera.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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


const std::array vertices = {
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f
};


const std::array cubePositions = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};



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
        glDeleteTextures(1, &textures[0].texture_id);
        glDeleteTextures(1, &textures[1].texture_id);
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
    Shader shader;

    float mix_value = 0.5f;

    struct Texture
    {
        int32_t texture_width, texture_height, texture_channels;
        unsigned char *data;

        uint32_t texture_id;
    };

    std::string texture_paths[2] = {"../../textures/container.jpg", "../../textures/awesomeface.png"};
    Texture textures[2];

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    Camera camera;

    float delta_time = 0.0f;
    float last_frame = 0.0f;

    float last_x = WIDTH / 2.0f;
    float last_y = HEIGHT / 2.0f;
    bool first_mouse = true;
    

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
        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        shader.init();
        shader.use();
        shader.set_float("mix_value", mix_value);
    }


    void create_objects()
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);


        textures[0].data = stbi_load(texture_paths[0].c_str(), &textures[0].texture_width, &textures[0].texture_height, &textures[0].texture_channels, 0);

        if (!textures[0].data)
        {
            throw std::runtime_error("Failed to load texture.");
        }

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &textures[0].texture_id);
        glBindTexture(GL_TEXTURE_2D, textures[0].texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textures[0].texture_width, textures[0].texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, textures[0].data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(textures[0].data);

        stbi_set_flip_vertically_on_load(true);
        textures[1].data = stbi_load(texture_paths[1].c_str(), &textures[1].texture_width, &textures[1].texture_height, &textures[1].texture_channels, 0);

        if (!textures[1].data)
        {
            throw std::runtime_error("Failed to load texture.");
        }

        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &textures[1].texture_id);
        glBindTexture(GL_TEXTURE_2D, textures[1].texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textures[1].texture_width, textures[1].texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures[1].data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(textures[1].data);
        shader.set_int("input_texture1", 0);
        shader.set_int("input_texture2", 1);

        model = glm::mat4(1.0f);

        view = glm::mat4(1.0f);
        view = camera.get_view_matrix();

        projection = glm::perspective(glm::radians(45.0f), width / static_cast<float>(height), 0.1f, 100.0f);

        shader.set_mat4("model", model);
        shader.set_mat4("view", view);
        shader.set_mat4("projection", projection);
    }


    void render()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        view = camera.get_view_matrix();
        shader.set_mat4("view", view);
        projection = glm::perspective(glm::radians(camera.zoom), width / static_cast<float>(height), 0.1f, 100.0f);
        shader.set_mat4("projection", projection);

        for (size_t i = 0; i < cubePositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            if (i % 3 == 0)
            {
                model = glm::rotate(model, glm::radians(10 * static_cast<float>(glfwGetTime())), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            shader.set_mat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }


    void update_variables()
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
    }


    void process_input()
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            mix_value += 0.01f;
            if (mix_value >= 1.0f) mix_value = 1.0f;
            shader.set_float("mix_value", mix_value);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            mix_value -= 0.01f;
            if (mix_value <= 0.0f) mix_value = 0.0f;
            shader.set_float("mix_value", mix_value);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera.process_keyboard_input(Camera_Movement::FORWARD, delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera.process_keyboard_input(Camera_Movement::BACKWARD, delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.process_keyboard_input(Camera_Movement::LEFT, delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.process_keyboard_input(Camera_Movement::RIGHT, delta_time);
        }
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

    
    static void mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
    {
        auto app = reinterpret_cast<OpenGlApp*>(glfwGetWindowUserPointer(window));
		app->mouse_callback_fn(x_pos, y_pos);
    }
    void mouse_callback_fn(double x_pos, double y_pos)
    {
        if (first_mouse)
        {
            last_x = static_cast<float>(x_pos);
            last_y = static_cast<float>(y_pos);
            first_mouse = false;
        }

        float x_offset = static_cast<float>(x_pos) - last_x;
        float y_offset = last_y - static_cast<float>(y_pos);

        last_x = static_cast<float>(x_pos);
        last_y = static_cast<float>(y_pos);

        camera.process_mouse_input(x_offset, y_offset);
    }


    static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
    {
        auto app = reinterpret_cast<OpenGlApp*>(glfwGetWindowUserPointer(window));
        app->scroll_callback_fn(x_offset, y_offset);
    }
    void scroll_callback_fn(double x_offset, double y_offset)
    {
        camera.process_mouse_scroll(static_cast<float>(y_offset));
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


