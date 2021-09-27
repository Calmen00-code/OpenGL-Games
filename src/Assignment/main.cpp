#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <stb/stb_image.h>

// Box coordinate with 36 vertices.
// Every 3 coordinates will form 1 triangle.
// The last 2 columns represent texture coordinate for mapping.
float box[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void register_texture(unsigned int * tex, const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
glm::vec3 camera_pos   = glm::vec3(0.0f, 0.9f,  3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

//Toggle (Animation or states)
bool BUTTON_PRESSED = false;
int BUTTON_DELAY = 0;
bool BUTTON_CLOSE_ENOUGH = false;

bool SHOW_COORDINATE = false;
int SHOW_DELAY = 0;

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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader skyShader("assets/assignment/assignment_vertex_core.glsl", "assets/assignment/assignment_fragment_core.glsl");


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	unsigned int VBO_box, VAO_box;

	glGenVertexArrays(1, &VAO_box);
	glGenBuffers(1, &VBO_box);

	glBindVertexArray(VAO_box);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_box);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


    // load and create a texture 
    // -------------------------
    unsigned int textureSun, textureSky, textureCloud;

    register_texture(&textureSun, "assets/textures/sun.jpg");
    register_texture(&textureSky, "assets/textures/blue_sky.jpg");
    register_texture(&textureCloud, "assets/textures/cloud.jpg");

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    skyShader.use();

	// pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
	// -----------------------------------------------------------------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 300.0f);
	skyShader.setMat4("projection", projection);


    /*
    skyShader.setInt("textureSky", 0);
    skyShader.setInt("textureSun", 1);
    skyShader.setInt("textureCloud", 2);
    */
    // set transformation
    /*
    glm::mat4 transformSky = glm::mat4(1.0f);
    glm::mat4 transformSun = glm::mat4(1.0f);
    glm::mat4 transformCloud = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.5f, 0.5f, 0.0f));  // sun on the top left
	model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));   // sky on middle
    model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0.0f));  // cloud on top right
    skyShader.use();
    skyShader.setMat4("transform", transformSky);
    skyShader.setMat4("transform", transformSun);
    skyShader.setMat4("transform", transformCloud);
    */

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		delta_time = currentFrame - last_frame;
		last_frame = currentFrame;


		// input
		// -----
		process_input(window);

		// render
		// ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 


		// activate shader
		skyShader.use();


		// camera/view transformation
		glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
		skyShader.setMat4("view", view);

		//declare transformation matrix
		glm::mat4 model = glm::mat4();

		// Render objects
		//------------------------------------------------------------------------------------------
		
		//Sky
		glBindVertexArray(VAO_box);//This does not have to binded every time after first call, but just for consistency.

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureSky);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));   // sky on middle

		skyShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

        // Sun
        glBindVertexArray(VAO_box);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureSun);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));  // sun on the top left

        skyShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);


        // Cloud
        /*
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureCloud);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0.0f));  // cloud on top right

        skyShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        */


        // create transformation for shaders
        // ---------------------------------
        // float timeVal = glfwGetTime() / 10000.0f;

        // create transformation for skyShader
        /*
        glm::mat4 transformSky = glm::mat4(1.0f);
        glm::mat4 transformSun = glm::mat4(1.0f);
        glm::mat4 transformCloud = glm::mat4(1.0f);

        transformSun = glm::translate(transformSun, glm::vec3(-0.5f, 0.5f, 0.0f));  // sun on the top left
        transformSky = glm::translate(transformSky, glm::vec3(0.0f, 0.5f, 0.0f));   // sky on middle
        transformCloud = glm::translate(transformCloud, glm::vec3(0.5f, 0.5f, 0.0f));  // cloud on top right
        transformSky = glm::rotate(transformSky, glm::radians(timeVal), glm::vec3(0.0f, 0.0f, 1.0f));
        skyShader.setMat4("transform", transformSky);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        skyShader.setMat4("transform", transformSun);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        skyShader.setMat4("transform", transformCloud);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        */

        // get matrix's uniform location and set matrix
        /*
        unsigned int transformSkyLoc = glGetUniformLocation(skyShader.ID, "transform");
        glUniformMatrix4fv(transformSkyLoc, 1, GL_FALSE, glm::value_ptr(transformSky));
        unsigned int transformSunLoc = glGetUniformLocation(skyShader.ID, "transform");
        glUniformMatrix4fv(transformSunLoc, 1, GL_FALSE, glm::value_ptr(transformSun));
        unsigned int transformCloudLoc = glGetUniformLocation(skyShader.ID, "transform");
        glUniformMatrix4fv(transformCloudLoc, 1, GL_FALSE, glm::value_ptr(transformCloud));
        */

        // render the texture


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO_box);
    glDeleteBuffers(1, &VBO_box);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void process_input(GLFWwindow *window)
{
    	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        	glfwSetWindowShouldClose(window, true);

	float cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		cameraSpeed = 2.5 * delta_time; 
	else
		cameraSpeed = 2.5 * delta_time * 2;	// double speed with "Shift" pressed


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera_pos += cameraSpeed * camera_front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera_pos -= cameraSpeed * camera_front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * cameraSpeed;

	//toggle red button
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && BUTTON_DELAY == 0 && BUTTON_CLOSE_ENOUGH == true)
	{
		BUTTON_DELAY = 20;
		if(BUTTON_PRESSED == false) 		
			BUTTON_PRESSED = true;
		else
			BUTTON_PRESSED = false;
	}

	//toggle coordinate visibility
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && SHOW_DELAY == 0)
	{
		SHOW_DELAY = 20;
		if(SHOW_COORDINATE == false) 		
			SHOW_COORDINATE = true;
		else
			SHOW_COORDINATE = false;
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

// registering a new texture. You can modify this function to allow for more configuration options.
void register_texture(unsigned int * tex, const char *path)
{
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor [] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

