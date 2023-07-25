

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include<shader.h>
#include<stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>




class Vertex
{
public:
	float x;
	float y;
	float z;
	float text_u;
	float text_v;
	Vertex() {}

	void loadHeight()
	{

	}
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void GenerateTranslation(float m[], int x, int y);
void initMatrix(float translation[]);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void findMaxMin(float** heightmap, float& max, float& min);


float X1, Y1, X2, Y2, slope;
void setEqn(float x1, float y1, float x2, float y2);
bool inside(float x, float y, int randomness);
void addfault(float** heightmap, int num);
int patch(int i, int j);
void applyFIRfilter(float** heightmap);
void makeValley(float** heightmap);


// settings
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 830;
const int scl = 300;
const int TOTAL_itr = 46;
const int mesh_dim = 200;
const float filter = 0.4;
float max_height = 1300;
float min_height = 50;
float del_height = max_height - min_height;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 15.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;



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


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();
	Shader shader("vshader.vs", "fshader.fs");
	shader.use();

	glEnable(GL_DEPTH_TEST);



	float** heightmap = new float* [mesh_dim];
	for (int i = 0; i < mesh_dim; i++)
	{
		heightmap[i] = new float[mesh_dim]{ 0 };
	}
	int x1, y1, x2, y2;
	int number_of_itr = TOTAL_itr;
	

	//makeValley(heightmap);
	while (number_of_itr > 0)
	{
		x1 = rand() % mesh_dim;
		y1 = rand() % mesh_dim;
		x2 = rand() % mesh_dim;
		while (x2 == x1)
		{
			x2 = rand() % mesh_dim;
		}
		y2 = rand() % mesh_dim;
		while (y2 == y1)
		{
			y2 = rand() % mesh_dim;
		}
		setEqn(x1, y1, x2, y2);
		addfault(heightmap, number_of_itr);
		number_of_itr--;
	}


	applyFIRfilter(heightmap);

	


	float max, min;
	findMaxMin(heightmap, max, min);
	max = max / SCR_HEIGHT;
	min = min / SCR_HEIGHT;
	shader.setFloat("max_y", max);
	//shader.setFloat("min_y", min);
	cameraPos = glm::vec3(5 , (int)heightmap[5][5]/SCR_HEIGHT+2, -5);
	std::cout<<std::endl << "maximum height is " << max*SCR_HEIGHT<<std::endl;

	Vertex* terrain = new Vertex[mesh_dim * mesh_dim];
	for (int i = 0; i < mesh_dim; i++)
	{
		for (int j = 0; j < mesh_dim; j++)
		{
			terrain[i * mesh_dim + j].x = (scl * i) /(float) SCR_WIDTH;
			terrain[i * mesh_dim + j].y = heightmap[i][j] / SCR_HEIGHT;
			terrain[i * mesh_dim + j].z = -(scl * j) /(float) SCR_WIDTH;
			terrain[i * mesh_dim + j].text_u = i /(float) mesh_dim;
			terrain[i * mesh_dim + j].text_v = j /(float) mesh_dim;
		}

	}

	unsigned int* indices = new unsigned int[(mesh_dim-1) * (mesh_dim-1) * 6];
	int index = 0;

	// for(int i=0;i<)

	for (int i = 0; i < (mesh_dim-1); i++)
	{
		for (int j = 0; j < (mesh_dim-1); j++)
		{
			indices[index] = mesh_dim * i + j;
			index++;
			indices[index] = mesh_dim * i + j + 1;
			index++;
			indices[index] = mesh_dim * (i + 1) + j;
			index++;
			indices[index] = mesh_dim * i + j + 1;
			index++;
			indices[index] = mesh_dim * (i + 1) + j;
			index++;
			indices[index] = mesh_dim * (i + 1) + j + 1;
			index++;
		}
	}

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh_dim * mesh_dim * sizeof(Vertex), terrain, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (mesh_dim-1) * (mesh_dim-1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	unsigned int texture1, texture2,texture3;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("C:\\Users\\Abiral Acharya\\Desktop\\bhir.jpg", &width, &height, &nrChannels, 0);
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
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("C:\\Users\\Abiral Acharya\\Desktop\\grass3.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	// texture 3
// ---------
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	data = stbi_load("C:\\Users\\Abiral Acharya\\Desktop\\snow.jpg", &width, &height, &nrChannels, 0);
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




	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	//shader.use(); // don't forget to activate/use the shader before setting uniforms!
	// either set it manually like so:
	//glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
	// or set it via the texture class
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);
	shader.setInt("texture3", 2);





	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// glfwSwapBuffers(window);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//float translationMatrix[16];
	//initMatrix(translationMatrix);
	//GenerateTranslation(translationMatrix, -600, 200);
	//glUniformMatrix4fv(transformation1Loc, 1, GL_TRUE, translationMatrix);

	glm::mat4 translationMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(translationMatrix, glm::vec3(-1.0f, 0.33f, 0.00f));
	// shader.setMat4("transformation1", translationMatrix);

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(60.0f), glm::vec3(1.0, 0.0, 0.0));

	//int transformationLoc = glGetUniformLocation(shaderprogram, "transformation2");
	//std::cout << transformationLoc;
	//glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(trans));
 //   shader.setMat4("transformation2", trans);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", projection);

	// glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	 //view = glm::lookAt(glm::vec3(0.0, 10.0, 0.0), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	 //shader.setMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);


	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(VAO);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", view);



		// glPointSize(10);
		// glDrawArrays(GL_TRIANGLES, 0, 900);
		glDrawElements(GL_TRIANGLES, (mesh_dim-1) * (mesh_dim-1) * 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void GenerateTranslation(float m[], int x, int y)
{

	m[3] = (float)x / SCR_WIDTH;
	m[7] = (float)y / SCR_HEIGHT;
}

void initMatrix(float translation[])
{
	for (int i = 0; i < 16; i++)
	{
		translation[i] = 0;
		// rotation[i] = 0;
		// scaling[i] = 0;
		// reflection[i] = 0;
	}

	for (int i = 0; i < 4; i++)
	{
		translation[4 * i + i] = 1;
	}
}
// ---------------------------------------------------------------------------------------------

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

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
	cameraFront = glm::normalize(front);
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

void addfault(float** heightmap, int num)
{
	//float height = (rand() % 100) * pow(2, -num);
	//float height = ((float)(rand() % 200)) * ((float)num / TOTAL_itr);
	float height = max_height - ((float)num/TOTAL_itr) * del_height;
	//float height = ((float)(rand() % 90));
	//std::cout << height << " ";
	int randomness = rand() % 2;
	std::cout << randomness;
	//std::cout << randomness << std::endl;
	for (int i = 0; i < mesh_dim; i++)
	{
		for (int j = 0; j < mesh_dim; j++)
		{

			if (inside(j, i, randomness))
			{
				//heightmap[i][j] += height;
				//if ((i > 20 && i < 40) && (j > 20 && j < 40))
				//{
					//heightmap[i][j] += 10;
				//}
				//if (patch(j, i) == 1)
				//{
					//heightmap[i][j] += (height /4250.0)* ((i - 100.5) * (i - 100.5) + (j - 100.5)*(j - 100.5));
				//}
				//else

				//{
					heightmap[i][j] += height;
				//}

			}
			//		std::cout << heightmap[i][j] << " ";
		}
		//std::cout << std::endl;
	}
}
void setEqn(float x1, float y1, float x2, float y2)
{
	X1 = x1;
	Y1 = y1;
	X2 = x2;
	Y2 = y2;
	slope = (Y2 - Y1) / (X2 - X1);
}
bool inside(float x, float y, int randomness)
{
	bool value = true;
	if (randomness == 1)
	{
		value = false;
	}
	if ((y - Y1 - (slope) * (x - X1)) >= 0)
	{
		return value;
	}
	else
	{
		return !value;
	}
}
void findMaxMin(float** heightmap,float &max,float& min)
{
	max = 0;
	min = 0;
	for (int i = 0; i <mesh_dim; i++)
	{
		for (int j = 0; j <mesh_dim; j++)
		{
			if (heightmap[i][j] > max)
			{
				max = heightmap[i][j];
			}
			else if (heightmap[i][j] < min)
			{
				min = heightmap[i][j];
			}
		}
	}
}

void makeValley(float** heightmap)
{
	for (int i = mesh_dim * 0.3; i < 0.7*mesh_dim; i++)
	{
		for (int j = mesh_dim * 0.3; j < 0.7 * mesh_dim; j++)
		{
			float dist = (i - 105.5) * (i - 105.5) + (j - 100.5) * (j - 100.5);
			dist = sqrtf(dist);
			//std::cout << dist << std::endl;
			heightmap[i][j] -= (-dist*20);
		}
	}
}

int patch(int i, int j)
{
	/*if (i < 100 && j < 100)
	{
		return 1;
	}
	else if ((i < 200 && i >= 100) && (j < 200 && j >= 100))
	{
		return 2;
	}
	else if (i < 100 && j >= 100)
	{
		return 3;
	}
	*/
	if ((i >= 75 && i <= 125) && (j >= 75 && j <= 125))
	{
		return 1;
	}
	else
	{
		return 0;

	}
}

void applyFIRfilter(float** heightmap)
{
	for (int i = 1; i < (mesh_dim - 1); i++)
	{
		for (int j = 0; j < mesh_dim; j++)
		{
			heightmap[i][j] = filter * heightmap[i - 1][j] + (1 - filter) * heightmap[i][j];
		}
	}


	for (int i = (mesh_dim - 2); i > 0; i--)
	{
		for (int j = 0; j < mesh_dim; j++)
		{
			heightmap[i][j] = filter * heightmap[i + 1][j] + (1 - filter) * heightmap[i][j];
		}
	}

	for (int i = 0; i < mesh_dim; i++)
	{
		for (int j = (mesh_dim - 2); j > 0; j--)
		{
			heightmap[i][j] = filter * heightmap[i][j + 1] + (1 - filter) * heightmap[i][j];
		}
	}

	for (int i = 0; i < mesh_dim; i++)
	{
		for (int j = 1; j < (mesh_dim - 1); j++)
		{
			heightmap[i][j] = filter * heightmap[i][j - 1] + (1 - filter) * heightmap[i][j];
		}
	}


}



//----------------------------------------------------------------main---------------------------------------------------------------------------

/*
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include<shader.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

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
	glewInit();

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("shader.vs", "shader.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.55f, 0.55f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.55f, 0.45f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.45f, 0.45f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.45f, 0.55f  // top left
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// load and create a texture
	// -------------------------
	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // note that we set the container wrapping method to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // set texture filtering to nearest neighbor to clearly see the texels/pixels
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("C:/Users/Abiral Acharya/Pictures/Screenshots/screenshot.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // set texture filtering to nearest neighbor to clearly see the texels/pixels
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	data = stbi_load("C:/Users/Abiral Acharya/Pictures/Screenshots/screenshot.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
	// either set it manually like so:
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	// or set it via the texture class
	ourShader.setInt("texture2", 1);


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

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// render container
		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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
*/