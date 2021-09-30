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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void renderZombie(glm::mat4 model, Shader shader, unsigned int VAO_box);
void renderVillager(glm::mat4 model, Shader shader, unsigned int VAO_box);
void toggle_button_distance(glm::vec3 button_pos);

// settings
const unsigned int SCR_WIDTH = 1900;
const unsigned int SCR_HEIGHT = 1000;

// camera
glm::vec3 camera_pos   = glm::vec3(0.0f, 0.9f,  120.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

//Toggle (Animation or states)
bool BUTTON_PRESSED = false;
bool KEY_R_PRESSED = false;
int BUTTON_DELAY = 0;
bool BUTTON_CLOSE_ENOUGH = false;

bool SHOW_COORDINATE = false;
int SHOW_DELAY = 0;

// Countdown until the button trigger can be pressed again.
// This prevents accidental burst repeat clicking of the key.
void update_delay()
{
	if(BUTTON_DELAY > 0) BUTTON_DELAY -= 1;
	if(SHOW_DELAY > 0) SHOW_DELAY -= 1;
}

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader shader("assets/assignment/assignment_vertex_core.glsl", "assets/assignment/assignment_fragment_core.glsl");


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
    unsigned int textureSky;
	unsigned int textureStreetVertical, textureStreetHorizontal;
	unsigned int textureLandVertical, textureLandHorizontal, textureBox;
	unsigned int textureAWP, textureKnife, textureGrenade;
	unsigned int texWood, texMarble;
	unsigned int texRedDark, texRedBright;

    register_texture(&textureSky, "assets/textures/horror_night.jpg");
    register_texture(&textureStreetVertical, "assets/textures/street_vertical.png");
    register_texture(&textureStreetHorizontal, "assets/textures/street_horizontal.png");
	register_texture(&textureLandVertical, "assets/textures/brickwall_vertical.jpg");
	register_texture(&textureLandHorizontal, "assets/textures/brickwall_horizontal.jpg");
	register_texture(&textureBox, "assets/textures/container.jpg");
	register_texture(&textureAWP, "assets/textures/awp_skin1.jpeg");
	register_texture(&textureKnife, "assets/textures/knife.jfif");
	register_texture(&textureGrenade, "assets/textures/grenade.jfif");
	register_texture(&texWood,"assets/textures/wood2.jpg");
	register_texture(&texMarble,"assets/textures/marble.jpg");
	register_texture(&texRedDark,"assets/textures/red_dark.jpg");
	register_texture(&texRedBright,"assets/textures/red_bright.jpg");

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    shader.use();

	// pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
	// -----------------------------------------------------------------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 300.0f);
	shader.setMat4("projection", projection);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		delta_time = currentFrame - last_frame;
		last_frame = currentFrame;

		// update delay countdown
		update_delay();

		// input
		// -----
		process_input(window);

		// render
		// ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 


		// activate shader
		shader.use();

		// camera/view transformation
		glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
		shader.setMat4("view", view);

		//declare transformation matrix
		glm::mat4 model = glm::mat4();

		// Render objects
		//------------------------------------------------------------------------------------------

		// Sky
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureSky);

		model = glm::mat4();
		model = translate(model, glm::vec3(0.0f, 0.9f, 80.0f));
		model = glm::scale(model, glm::vec3(200.0f, 200.0f, 200.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Coordinate System
		/*
		if(SHOW_COORDINATE == true)
		{
			
			glm::vec3 coord_scales[] = {
				glm::vec3( 100.0f,  0.02f,  0.02f),	//X
				glm::vec3( 0.02f,  100.0f,  0.02f),	//Y
				glm::vec3( 0.02f,  0.02f,  100.0f),	//Z
			};

			glBindVertexArray(VAO_box);

			glActiveTexture(GL_TEXTURE0);

			for(int tab = 0; tab < 3; tab++)
			{	
				if(tab == 0) glBindTexture(GL_TEXTURE_2D, texRed); 	//X
				if(tab == 1) glBindTexture(GL_TEXTURE_2D, texGreen);	//Y
				if(tab == 2) glBindTexture(GL_TEXTURE_2D, texBlue);	//Z

				model = glm::mat4();
				model = glm::scale(model, coord_scales[tab]);

				shader.setMat4("model", model);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		*/


		// Street 1 (Vertical)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureStreetVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 70.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.0f, -100.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Side land 1, left (Vertical)
		// First half
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-3.0f, -0.5f, 90.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.0f, -57.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Second half
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-3.0f, -0.5f, 36.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.0f, -45.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Item 1
		//Table (4 tall boxes for legs & 1 thin box as table top)
		glm::vec3 table_scales[] = {
			glm::vec3( 1.0f,  0.1f,  1.0f),	//top
			glm::vec3( 0.1f,  0.5f,  0.1f),	//near left
			glm::vec3( 0.1f,  0.5f,  0.1f),	//near right
			glm::vec3( 0.1f,  0.5f,  0.1f),	//far left
			glm::vec3( 0.1f,  0.5f,  0.1f),	//far right
		};
		glm::vec3 table_positions[] = {
			glm::vec3(0.0f,  0.1f,  0.0f),		//top
			glm::vec3(-0.80f, -0.1f,  27.45f),	//near left
			glm::vec3(0.10f, -0.1f,  27.45f),	//near right
			glm::vec3(-0.80f, -0.1f, 26.55f),	//far left
			glm::vec3(0.10f, -0.1f, 26.55f),	//far right
		};

		glBindVertexArray(VAO_box);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texWood);

		for(int tab = 0; tab < 5; tab++)
		{	
			model = glm::mat4();
			model = glm::translate(model, table_positions[tab]);
			model = glm::scale(model, table_scales[tab]);
			model = glm::translate(model, glm::vec3(-0.4f, -0.2f, 30.0f));

			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Button on table (1 big box & 1 small box as button)
		glm::vec3 button_scales[] = {
			glm::vec3( 0.2f,  0.12f,  0.2f),		//case
			glm::vec3( 0.12f,  0.12f,  0.12f),		//button
		};

		float red_button_height = -0.45f;
		if(BUTTON_PRESSED == true) {red_button_height -= 1.02f;}

		glm::vec3 button_positions[] = {
			glm::vec3( -0.50f,  -0.5f,  29.88f),			//case
			glm::vec3( -0.50f,  red_button_height,  29.88f),	//button
		};

		glm::vec3 button_final_location = glm::vec3(0.0f, 0.56f, 0.25f);
		toggle_button_distance(button_final_location); 

		glBindVertexArray(VAO_box);

		for(int tab = 0; tab < 2; tab++)
		{	
			glActiveTexture(GL_TEXTURE0);
			if(tab == 0)
			{	
				glBindTexture(GL_TEXTURE_2D, texMarble);
			}
			else
			{
				if(BUTTON_PRESSED == false)
				{
					glBindTexture(GL_TEXTURE_2D, texRedDark); 	// Not pressed
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, texRedBright);	// Pressed
				}
			}

			model = glm::mat4();
			model = glm::translate(model, button_final_location);
			model = glm::translate(model, button_positions[tab]);
			model = glm::scale(model, button_scales[tab]);
			model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));

			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		/* AWP */
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureAWP);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-0.4f, 0.5f, 30.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.2f, 0.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Side land 1, Right (Vertical)
		// First half
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, -0.5f, 90.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.0f, -57.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Second half
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, -0.5f, 36.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.0f, -45.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Street 2 (Left Horizontal)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureStreetHorizontal);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-3.5f, -0.5f, 60.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.0f, -3.0f));

		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Side land 2 (Left Horizontal)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandHorizontal);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-7.5f, -0.5f, 62.8f));
		model = glm::scale(model, glm::vec3(11.0f, 0.0f, -3.0f));

		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Side land 2 (Left Vertical)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-13.5f, -0.5f, 35.8f));
		model = glm::scale(model, glm::vec3(2.0f, 0.0f, -57.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Street 2 (Left Vertical)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureStreetVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-11.0f, -0.5f, 35.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.0f, -50.0f));

		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Item 2
		//Table (4 tall boxes for legs & 1 thin box as table top)
		glm::vec3 table_scales_2[] = {
			glm::vec3( 1.0f,  0.1f,  1.0f),	//top
			glm::vec3( 0.1f,  0.5f,  0.1f),//near left
			glm::vec3( 0.1f,  0.5f,  0.1f),	//near right
			glm::vec3( 0.1f,  0.5f,  0.1f),//far left
			glm::vec3( 0.1f,  0.5f,  0.1f),	//far right
		};
		glm::vec3 table_positions_2[] = {
			glm::vec3(-11.0f, 0.0f, 25.0f),		//top
			glm::vec3(-10.55f, -0.4f, 25.45f),	//near left
			glm::vec3(-11.45f, -0.4f, 25.45f),	//near right
			glm::vec3(-10.55f, -0.4f, 24.55f),	//far left
			glm::vec3(-11.45f, -0.4f, 24.45f),	//far right
		};

		glBindVertexArray(VAO_box);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texWood);

		for(int tab = 0; tab < 5; tab++)
		{	
			model = glm::mat4();
			model = glm::translate(model, table_positions_2[tab]);
			model = glm::scale(model, table_scales_2[tab]);
			model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));

			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		//Button on table (1 big box & 1 small box as button)
		glm::vec3 button_scales_2[] = {
			glm::vec3( 0.2f,  0.12f,  0.2f),		//case
			glm::vec3( 0.12f,  0.12f,  0.12f),		//button
		};

		red_button_height = -0.45f;
		if(BUTTON_PRESSED == true) {red_button_height -= 1.02f;}

		glm::vec3 button_positions_2[] = {
			glm::vec3( -11.0f,  -0.5f,  25.0f),			//case
			glm::vec3( -11.0f,  red_button_height,  25.0f),	//button
		};

		glm::vec3 button_final_location_2 = glm::vec3(0.0f, 0.56f, 0.25f);
		toggle_button_distance(button_final_location_2); 

		glBindVertexArray(VAO_box);
		
		for(int tab = 0; tab < 2; tab++)
		{	
			glActiveTexture(GL_TEXTURE0);
			if(tab == 0)
			{	
				glBindTexture(GL_TEXTURE_2D, texMarble);
			}
			else
			{
				if(BUTTON_PRESSED == false)
				{
					glBindTexture(GL_TEXTURE_2D, texRedDark); 	// Not pressed
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, texRedBright);	// Pressed
				}
			}

			model = glm::mat4();
			model = glm::translate(model, button_final_location);
			model = glm::translate(model, button_positions_2[tab]);
			model = glm::scale(model, button_scales_2[tab]);
			model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));

			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		/*
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureBox);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-11.0f, -0.2f, 25.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/

		/* Knife */
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureKnife);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-11.0f, 0.5f, 25.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.2f, 0.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);


		// Street 3 (Right Horizontal)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureStreetHorizontal);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.5f, -0.5f, 60.0f));
		model = glm::scale(model, glm::vec3(30.0f, 0.0f, -3.0f));

		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Side land 3 (Right Horizontal)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandHorizontal);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(7.5f, -0.5f, 62.8f));
		model = glm::scale(model, glm::vec3(11.0f, 0.0f, -3.0f));

		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Street 3 (Right Vertical)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureStreetVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(11.0f, -0.5f, 35.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.0f, -50.0f));

		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Side land 3 (Right Vertical)
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandVertical);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(13.5f, -0.5f, 35.8f));
		model = glm::scale(model, glm::vec3(2.0f, 0.0f, -57.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Item 3
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureBox);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(11.0f, -0.2f, 30.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		/* Grenade */
		glBindVertexArray(VAO_box);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureGrenade);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(11.0f, 0.5f, 30.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.0f));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Display villager if R is pressed
		// else display zombie
		if (KEY_R_PRESSED == true) {
			// Display villager
			renderVillager(model, shader, VAO_box);
		}
		else {
			// renderZombie(model, shader, VAO_box);
		}
		
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
		cameraSpeed = 2.5 * delta_time * 2; 
	else
		cameraSpeed = 2.5 * delta_time * 50;	// double speed with "Shift" pressed


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera_pos += cameraSpeed * camera_front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera_pos -= cameraSpeed * camera_front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		KEY_R_PRESSED = !KEY_R_PRESSED;

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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void renderVillager(glm::mat4 model, Shader shader, unsigned int VAO_box) {
	unsigned int villagerTexture, villagerFace;
	unsigned int greenTexture;

	register_texture(&villagerTexture, "assets/textures/green_specular.jpg");
	register_texture(&villagerFace, "assets/textures/villager_face.png");
	register_texture(&greenTexture, "assets/textures/green.jpg");

	// head
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-0.5f, 20.5f, 5.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// face
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerFace);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-0.5f, 20.5f, 7.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 1.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// body
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-0.5f, 15.5f, 5.0f));
	model = glm::scale(model, glm::vec3(4.0f, 7.0f, 4.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// left hand
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, greenTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.5f, 15.5f, 5.0f));
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 6.0f, 3.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// right hand
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, greenTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.5f, 15.5f, 5.0f));
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 6.0f, 3.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// left leg
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, greenTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.5f, 9.5f, 5.0f));
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 7.5f, 2.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// right leg
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, greenTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.5f, 9.5f, 5.0f));
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 7.5f, 2.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void renderZombie(glm::mat4 model, Shader shader, unsigned int VAO_box) {
	unsigned int villagerTexture, zombieFace;

	register_texture(&villagerTexture, "assets/textures/zombie_color.png");
	register_texture(&zombieFace, "assets/textures/zombie_face.png");

	// head
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-0.5f, 20.5f, 5.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// face
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, zombieFace);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-0.5f, 20.5f, 7.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 1.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// body
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-0.5f, 15.5f, 5.0f));
	model = glm::scale(model, glm::vec3(4.0f, 7.0f, 4.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// left hand
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.5f, 17.5f, 7.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 7.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// right hand
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.5f, 17.5f, 7.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 7.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// left leg
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.5f, 9.5f, 5.0f));
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 7.5f, 2.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// right leg
	glBindVertexArray(VAO_box);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, villagerTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.5f, 9.5f, 5.0f));
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 7.5f, 2.0f));
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Toggle button pressing only if the camera is close enough.
void toggle_button_distance(glm::vec3 button_pos)
{
	if(glm::length(camera_pos - button_pos) <= 1.6f)
		BUTTON_CLOSE_ENOUGH = true;
	else
		BUTTON_CLOSE_ENOUGH = false;
}
