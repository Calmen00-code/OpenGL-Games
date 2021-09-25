#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include<fstream>
#include<sstream>
#include<streambuf>
#include<string>
#include <iostream>

#define NUM_VAL_TRIANGLE 3

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
    std::string vertShaderSrc = loadShaderSrc("assets/triangle_vertex_core.glsl");
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
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragShaderSrc = loadShaderSrc("assets/triangle_fragment_core.glsl");
    const GLchar* fragShader = fragShaderSrc.c_str();
    glShaderSource(fragmentShader, 1, &fragShader, NULL);
    glCompileShader(fragmentShader);

    // catch error
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Error with fragment shader comp.:" << std::endl << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // catch errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Linking Error" << std::endl << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // vertex array
    float triangleVertices[] = {
        // first triangle
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,

        // second triangle
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    // VAO, VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind VAO
    glBindVertexArray(VAO);

    // bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    // set attribute pointer
    glVertexAttribPointer(0, NUM_VAL_TRIANGLE, GL_FLOAT, GL_FALSE, NUM_VAL_TRIANGLE * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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