#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include<fstream>
#include<sstream>
#include<streambuf>
#include<string>
#include <iostream>

#define NUM_VAL_TRIANGLE 3
#define NUM_VAL_TRIANGLE_WITH_COLOR 6

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::string loadShaderSrc(const char* filename);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    } 

    /* shaders */
    // compile vertex shader
    int success;
    char infoLog[512];
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertShaderSrc = loadShaderSrc("assets/shader_vertex_core.glsl");
    const GLchar* vertShader = vertShaderSrc.c_str();
    glShaderSource(vertexShader, 1, &vertShader, NULL);
    glCompileShader(vertexShader);

    // catch error
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Error with vertex shader comp.:" << std::endl << infoLog << std::endl;
    }

    // compile fragment shader
    unsigned int fragmentShaders[2];

    fragmentShaders[0] = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragShaderSrc = loadShaderSrc("assets/shader_fragment_core.glsl");
    const GLchar* fragShader = fragShaderSrc.c_str();
    glShaderSource(fragmentShaders[0], 1, &fragShader, NULL);
    glCompileShader(fragmentShaders[0]);
   
    // catch error
    glGetShaderiv(fragmentShaders[0], GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(fragmentShaders[0], 512, NULL, infoLog);
        std::cout << "Error with fragment shader comp.:" << std::endl << infoLog << std::endl;
    }

    fragmentShaders[1] = glCreateShader(GL_FRAGMENT_SHADER);
    fragShaderSrc = loadShaderSrc("assets/shader_fragment_core2.glsl");
    fragShader = fragShaderSrc.c_str();
    glShaderSource(fragmentShaders[1], 1, &fragShader, NULL);
    glCompileShader(fragmentShaders[1]);
   
    // catch error
    glGetShaderiv(fragmentShaders[1], GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(fragmentShaders[1], 512, NULL, infoLog);
        std::cout << "Error with fragment shader comp.:" << std::endl << infoLog << std::endl;
    }

    unsigned int shaderPrograms[2];
    shaderPrograms[0] = glCreateProgram();

    glAttachShader(shaderPrograms[0], vertexShader);
    glAttachShader(shaderPrograms[0], fragmentShaders[0]);
    glLinkProgram(shaderPrograms[0]);

    // catch errors
    glGetProgramiv(shaderPrograms[0], GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderPrograms[0], 512, NULL, infoLog);
        std::cout << "Linking Error" << std::endl << infoLog << std::endl;
    }

    shaderPrograms[1] = glCreateProgram();

    glAttachShader(shaderPrograms[1], vertexShader);
    glAttachShader(shaderPrograms[1], fragmentShaders[1]);
    glLinkProgram(shaderPrograms[1]);

    // catch errors
    glGetProgramiv(shaderPrograms[1], GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderPrograms[1], 512, NULL, infoLog);
        std::cout << "Linking Error" << std::endl << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaders[0]);
    glDeleteShader(fragmentShaders[1]);

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

        // draw shapes
        glBindVertexArray(VAO);

        // first triangle
        glUseProgram(shaderPrograms[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // second triangle
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

std::string loadShaderSrc(const char* filename) {
    std::ifstream file;
    std::stringstream buf;

    std::string ret = "";
    
    file.open(filename);

    if (file.is_open()) {
        buf << file.rdbuf();
        ret = buf.str();
    }
    else {
        std::cout << "Could not open " << filename << std::endl;
    }

    file.close();

    return ret;
}
