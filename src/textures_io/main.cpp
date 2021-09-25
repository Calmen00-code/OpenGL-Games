/*
* Control the texture using the arrow up and down button
*/


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include<fstream>
#include<sstream>
#include<streambuf>
#include<string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <stb/stb_image.h>


#define NUM_VAL_TRIANGLE 3
#define NUM_VAL_TRIANGLE_WITH_COLOR 6

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

float mixVal = 0.5;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader shader("assets/texture_vertex_io_core.glsl", "assets/texture_fragment_io_core.glsl");

    float triangleVertices[] = {
        // first triangle           // colors           // texture coordinates
        -0.5f, -0.5f, 0.0f,         1.0f, 1.0f, 0.5f,   0.0f, 0.0f,     // bottom left
        -0.5f, 0.5f, 0.0f,          0.5f, 1.0f, 0.75f,  0.0f, 1.0f,     // top left
        0.5f, -0.5f, 0.0f,          0.6f, 1.0f, 0.2f,   1.0f, 0.0f,     // bottom right
        0.5f, 0.5f, 0.0f,           1.0f, 0.2f, 1.0f,   1.0f, 1.0f      // top right
    };
    unsigned int indices[] = {
        0, 1, 2,    // first triangle
        3, 1, 2     // second triangle
    };

    // VAO, VBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind VAO
    glBindVertexArray(VAO);

    // bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    // setup EBO
    // we dont need glVertexAttribPointer as it is elementary buffer not vertex array buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // positions attributes
    glVertexAttribPointer(0, NUM_VAL_TRIANGLE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attributes
    // @firstparam is 1 to tell OpenGL that it is color not position (which means is 0 for position)
    glVertexAttribPointer(1, NUM_VAL_TRIANGLE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attributes
    // @lastparam   6 * sizeof(float) because first, we have position attrib takes up 3 columns, 
    //              then color attrib takes up 3 columns, therefore 3 columns + 3 columns = 6
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load and create texture
    // -------------------------
    unsigned int texture1, texture2;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("assets/textures/obama10.jpg", &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    data = stbi_load("assets/textures/americanflag.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    shader.activate();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader.activate();
    shader.setMat4("transform", trans);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // moving rotation
        float timeVal = glfwGetTime()/10000.0f;
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, glm::radians(timeVal), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("transform", trans);

        shader.setFloat("mixVal", mixVal);

        // draw shapes
        glBindVertexArray(VAO);
        shader.activate();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // change mix value
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        mixVal += 0.5f;
        if (mixVal > 1) {
            mixVal = 1.0f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mixVal -= 0.5f;
        if (mixVal < 0) {
            mixVal = 0.0f;
        }

    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
