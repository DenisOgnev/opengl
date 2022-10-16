#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <array>



const std::string WINDOW_NAME = "OpenGL";
const int32_t WIDTH = 800;
const int32_t HEIGHT = 600;

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

const std::array indices = {
    0, 1, 2,
    2, 3, 0
};


void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);


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
            throw std::runtime_error("Failed to create window.");
        }
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

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

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
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

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


        // glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, 36);
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