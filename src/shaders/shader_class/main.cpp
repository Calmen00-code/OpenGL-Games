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

#include "shader.h";

#define NUM_VAL_TRIANGLE 3
#define NUM_VAL_TRIANGLE_WITH_COLOR 6

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

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

    Shader shader("assets/shader_class_vertex_core.glsl", "assets/shader_class_fragment_core.glsl");
    Shader shaderTwo("assets/shader_class_vertex_core.glsl", "assets/shader_class_fragment_core2.glsl");

    float triangleVertices[] = {
        // first triangle           // colors
        -0.25f, -0.5f, 0.0f,        1.0f, 1.0f, 0.5f,
        0.15f, 0.0f, 0.0f,          0.5f, 1.0f, 0.75f,
        0.0f, 0.5f, 0.0f,           0.6f, 1.0f, 0.2f,
        0.5f, -0.4f, 0.0f,          1.0f, 0.2f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        3, 1, 2
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

    // set attribute pointer

    // positions attributes
    glVertexAttribPointer(0, NUM_VAL_TRIANGLE, GL_FLOAT, GL_FALSE, NUM_VAL_TRIANGLE_WITH_COLOR * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attributes
    // @firstparam is 1 to tell OpenGL that it is color not position (which means is 0 for position)
    glVertexAttribPointer(1, NUM_VAL_TRIANGLE, GL_FLOAT, GL_FALSE, 
        NUM_VAL_TRIANGLE_WITH_COLOR * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader.activate();
    shader.setMat4("transform", trans);

    glm::mat4 transTwo = glm::mat4(1.0f);
    transTwo = glm::scale(transTwo, glm::vec3(1.5f));
    transTwo = glm::rotate(transTwo, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shaderTwo.activate();
    shaderTwo.setMat4("transform", transTwo);

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

        // moving rotation
        trans = glm::rotate(trans, glm::radians((float)glfwGetTime() / 1000.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.activate();
        shader.setMat4("transform", trans);

        // draw shapes
        glBindVertexArray(VAO);
        shader.activate();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        transTwo = glm::rotate(transTwo, glm::radians((float)glfwGetTime() / -1000.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shaderTwo.activate();
        shaderTwo.setMat4("transform", transTwo);

        shaderTwo.activate();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(3 * sizeof(GLuint)));


        // glUseProgram(shaderPrograms[1]);
        // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(3 * sizeof(unsigned int)));
        glBindVertexArray(0);

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
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
