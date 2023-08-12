
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




void addfault(float** heightmap, int num, float m,int randomness, int c, int x, int y);
void applyFIRfilter(float** heightmap);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void findMaxMin(float** heightmap, float& max, float& min);


float X1, Y1, X2, Y2, slope;
void setEqn(float x1, float y1, float x2, float y2);
bool inside(float x, float y, int randomness, float m, int c);




// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 830;
const int scl = 300;
const int TOTAL_itr = 200;
int mesh_dim = 50;
const float filter = 0.4;
float max_height = 750;
float min_height = 00;
float del_height = max_height - min_height;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 30.0f, 0.0f);
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

class FaultLines
{
public:
	float* m;
	int* c;
	int* randomness;
	FaultLines()
	{
		m = new float[TOTAL_itr];
		c = new int[TOTAL_itr];
		randomness = new int[TOTAL_itr];
	}
	void generate()
	{
		int number_of_itr = TOTAL_itr;
		srand(0);

		int i = 0;
		while (i < TOTAL_itr)
		{
			int angle = rand() % 181;
			while (angle == 90)
			{
				angle = rand() % 181;
			}

			m[i] = tanf(angle);
			c[i] = pow(-1, rand() % 2) * (rand() % 200);
			randomness[i] = rand() % 2;
			std::cout << m[i] << "   " << c[i] << std::endl;
			i++;


		}


	}

	~FaultLines()
	{
		std::cout << "no error" << std::endl;
		delete[] m;
		delete[] c;
		delete[] randomness;
		std::cout << "after delting" << std::endl;
	}


};

class Patch
{
public:
	int m_x, m_y, m_prev_x, m_prev_y;
	Vertex* terrain;
	unsigned int VAO, VBO;
	bool m_buffer_set;

	Patch(int x=0, int y=0, int prev_x=0, int prev_y=0)
	{
		m_x = x;
		m_y = y;
		m_prev_x = prev_x;
		m_prev_y = prev_y;
		m_buffer_set = false;
	}

	void setPos(int x, int y)
	{
		m_x = x;
		m_y = y;
	}
	void generateHeightMap(FaultLines &faultline)
	{
		float** heightmap = new float* [mesh_dim];
		for (int i = 0; i < mesh_dim; i++)
		{
			heightmap[i] = new float[mesh_dim] { 0 };
		}
		int number_of_itr = TOTAL_itr;
		while (number_of_itr > 0)
		{
			addfault(heightmap, number_of_itr, faultline.m[number_of_itr], faultline.c[number_of_itr],faultline.randomness[number_of_itr], m_x, m_y);
			number_of_itr--;
		}
		applyFIRfilter(heightmap);


		terrain = new Vertex[mesh_dim * mesh_dim];
		for (int i = 0; i < mesh_dim; i++)
		{
			for (int j = 0; j < mesh_dim; j++)
			{
				terrain[i * mesh_dim + j].x = ((m_x) * (mesh_dim - 1) * scl + (scl * i)) / (float)SCR_WIDTH;
				terrain[i * mesh_dim + j].y = heightmap[i][j] / SCR_HEIGHT;
				terrain[i * mesh_dim + j].z = -((m_y) * (mesh_dim - 1) * scl + (scl * j)) / (float)SCR_WIDTH;
				terrain[i * mesh_dim + j].text_u = i / (float)mesh_dim;
				terrain[i * mesh_dim + j].text_v = j / (float)mesh_dim;
			}

		}

		for (int i = 0; i < mesh_dim; i++)
		{
			delete[] heightmap[i];
		}
		delete[] heightmap;
		std::cout << "before calling" << std::endl;
	}

	void setupBuffers(unsigned int EBO)
	{
		m_buffer_set = true;
		glGenVertexArrays(1, &VAO);

		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh_dim * mesh_dim * sizeof(Vertex), terrain, GL_STATIC_DRAW);
		delete[]terrain;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	void render()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (mesh_dim - 1) * (mesh_dim - 1) * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	void cleanupBuffers()
	{
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}

	~Patch()
	{


	}


};











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



	
	int x1, y1, x2, y2;
	int number_of_itr = TOTAL_itr;


	//for (int i = 0; i < mesh_dim; i++)
	//{
		//heightmap2[0][i] = heightmap[mesh_dim - 1][i];

//	}



	float max, min;
	//findMaxMin(heightmap2, max, min);
	max = 38500 / SCR_HEIGHT;
	min = 0 / SCR_HEIGHT;
	shader.setFloat("max_y", max);
	//shader.setFloat("min_y", min);
//	cameraPos = glm::vec3(5, 3 + (int)heightmap[5][5] / SCR_HEIGHT, -5);
	cameraPos = glm::vec3(5,  max, 0);


	std::cout << std::endl << "maximum height is " << max * SCR_HEIGHT << std::endl;


	unsigned int* indices = new unsigned int[(mesh_dim - 1) * (mesh_dim - 1) * 6];
	int index = 0;


	for (int i = 0; i < (mesh_dim - 1); i++)
	{
		for (int j = 0; j < (mesh_dim - 1); j++)
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



	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (mesh_dim - 1) * (mesh_dim - 1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);



	unsigned int texture1, texture2, texture3;
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
	data = stbi_load("C:\\Users\\Abiral Acharya\\Desktop\\gmass.png", &width, &height, &nrChannels, 0);
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




	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);
	shader.setInt("texture3", 2);





	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



	glm::mat4 translationMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(translationMatrix, glm::vec3(-1.0f, 0.33f, 0.00f));

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(60.0f), glm::vec3(1.0, 0.0, 0.0));


	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", projection);


	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);


	FaultLines* faultlines = new FaultLines;
	faultlines->generate();
	/*
	Patch* patch1 = new Patch(0, 0, 0, 0);
	patch1->generateHeightMap(*faultlines);
	patch1->setupBuffers(EBO);
	Patch* patch2 = new Patch(-1, 0, 0, 0);
	patch2->generateHeightMap(*faultlines);
	patch2->setupBuffers(EBO);
	Patch* patch3 = new Patch(1, 0, 0, 0);
	patch3->generateHeightMap(*faultlines);
	patch3->setupBuffers(EBO);
	Patch* patch4 = new Patch(0, 1, 0, 0);
	patch4->generateHeightMap(*faultlines);
	patch4->setupBuffers(EBO);
	Patch* patch5 = new Patch(0, -1, 0, 0);
	patch5->generateHeightMap(*faultlines);
	patch5->setupBuffers(EBO);
	Patch* patch6 = new Patch(1, -1, 0, 0);
	patch6->generateHeightMap(*faultlines);
	patch6->setupBuffers(EBO);
	*/


	//Patch* patches = new Patch[5];

	Patch** patch = new Patch*[5];
	patch[0] = new Patch(-1, 0);

	patch[1] = new Patch(1, 0);
	patch[2] = new Patch(0, 0);


	patch[3] = new Patch(0, -1);
	patch[4] = new Patch(0, 1);

	while (!glfwWindowShouldClose(window))
	{
		//std::cout << "FO" << std::endl;
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glBindVertexArray(VAO);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", view);
		
		//std::cout << "camera" << std::endl;
		//std::cout << cameraPos.x << "                       " << cameraPos.z << std::endl;
		
		

		int cur_x = ((int)cameraPos.x / 10 );
		if (cameraPos.x<0 )
		{
			cur_x--;
		}
		
		int cur_y = -((int)cameraPos.z /10 );
		if (cameraPos.z > 0)
		{
			cur_y--;
		}
		
		std::cout << "current" << std::endl;
		std::cout << cur_x << "      " << cur_y << std::endl;
		
		
		if ((cur_x != patch[2]->m_x) || (cur_y != patch[2]->m_y))
		{
			for (int m = 0; m < 5; m++)
			{
				delete patch[m];
			}
			
			patch[0] = new Patch(cur_x-1, cur_y+0);

			patch[1] = new Patch(cur_x+1, cur_y+0);
			patch[2] = new Patch(cur_x, cur_y);


			patch[3] = new Patch(cur_x+0, cur_y-1);
			patch[4] = new Patch(cur_x+0, cur_y+1);
			//cameraPos = glm::vec3(cameraPos.x,cameraPos.y,cameraPos.z+10);
		}




		for (int i = 0; i < 5; i++)
		{
		


			if (!(patch[i]->m_buffer_set))
			{
				patch[i]->generateHeightMap(*faultlines);
				patch[i]->setupBuffers(EBO);
				
			}
			patch[i]->render();
		}

		/*patch1->render();
		patch2->render();
		patch3->render();
		patch4->render();
		patch5->render();
		patch6->render();
		*/

		// glPointSize(10);
		// glDrawArrays(GL_TRIANGLES, 0, 900);
		//glDrawElements(GL_TRIANGLES, (mesh_dim - 1) * (mesh_dim - 1) * 6, GL_UNSIGNED_INT, 0);



		//glBindVertexArray(VAO2);
		//glDrawElements(GL_TRIANGLES, (mesh_dim - 1) * (mesh_dim - 1) * 6, GL_UNSIGNED_INT, 0);



		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	delete faultlines;
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

void addfault(float** heightmap, int cur_itr, float m, int c,int randomness, int x, int y)
{
	float height = max_height - ((float)cur_itr / TOTAL_itr) * del_height;
	std::cout << randomness;
	for (int i = 0; i < mesh_dim; i++)
	{
		for (int j = 0; j < mesh_dim; j++)
		{



			if (inside( i + x * (mesh_dim - 1), j + y * (mesh_dim - 1), randomness, m, c))
			{

				heightmap[i][j] += height;


			}



		}
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
bool inside(float x, float y, int randomness, float m, int c)
{
	bool value = true;
	if (randomness == 1)
	{
		value = false;
	}
	if (y - m * x - c >= 0)
	{
		return value;
	}
	else
	{
		return !value;
	}
}
void findMaxMin(float** heightmap, float& max, float& min)
{
	max = 0;
	min = 0;
	for (int i = 0; i < mesh_dim; i++)
	{
		for (int j = 0; j < mesh_dim; j++)
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

void applyFIRfilter(float** heightmap)
{
	
	
	//left to right
	for (int i = 0; i < mesh_dim; i++)
	{
		for (int j = 1; j < (mesh_dim - 1); j++)
		{
			heightmap[i][j] = filter * heightmap[i][j - 1] + (1 - filter) * heightmap[i][j];
		}
	}

	//right to left
	for (int i = 0; i < mesh_dim; i++)
	{
		for (int j = (mesh_dim - 2); j > 0; j--)
		{
			heightmap[i][j] = filter * heightmap[i][j + 1] + (1 - filter) * heightmap[i][j];
		}
	}

	// top to bottom
	for (int i = 1; i < (mesh_dim - 1); i++)
	{
		for (int j = 0; j < mesh_dim; j++)
		{
			heightmap[i][j] = filter * heightmap[i - 1][j] + (1 - filter) * heightmap[i][j];
		}
	}

	//bottom to top
	for (int i = (mesh_dim - 2); i > 0; i--)
	{
		for (int j = 0; j < mesh_dim; j++)
		{
			heightmap[i][j] = filter * heightmap[i + 1][j] + (1 - filter) * heightmap[i][j];
		}
	}



	
}
