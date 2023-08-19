//
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//
//#include <iostream>
//#include <cstdlib>
//#include<shader.h>
//#include<stb_image.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <cmath>
//
//
//
//
//void addfault(float** heightmap, int num, float m,int randomness, int c, int x, int y);
//void applyFIRfilter(float** heightmap);
//
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void findMaxMin(float** heightmap, float& max, float& min);
//
//
//float X1, Y1, X2, Y2, slope;
//void setEqn(float x1, float y1, float x2, float y2);
//bool inside(float x, float y, int randomness, float m, int c);
//
//
//
//
//// settings
//const unsigned int SCR_WIDTH = 1600;
//const unsigned int SCR_HEIGHT = 830;
//const int scl = 400;
//const int TOTAL_itr =200;
//int mesh_dim = 50;
//const float filter = 0.4;
//float max_height = 750;
//float min_height = 00;
//float del_height = max_height - min_height;
//
//// camera
//glm::vec3 cameraPos = glm::vec3(0.0f, 30.0f, 0.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//
//
//// timing
//float deltaTime = 0.0f;	// time between current frame and last frame
//float lastFrame = 0.0f;
//
//bool firstMouse = true;
//float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
//float pitch = 0.0f;
//float lastX = 800.0f / 2.0;
//float lastY = 600.0 / 2.0;
//float fov = 45.0f;
//bool fly = true;
//
//
//
//
//
//class Vertex
//{
//public:
//	float x;
//	float y;
//	float z;
//	float text_u;
//	float text_v;
//	Vertex() {}
//
//	void loadHeight()
//	{
//
//	}
//};
//
//class FaultLines
//{
//public:
//	float* m;
//	int* c;
//	int* randomness;
//	FaultLines()
//	{
//		m = new float[TOTAL_itr];
//		c = new int[TOTAL_itr];
//		randomness = new int[TOTAL_itr];
//	}
//	void generate()
//	{
//		int number_of_itr = TOTAL_itr;
//		srand(0);
//
//		int i = 0;
//		while (i < TOTAL_itr)
//		{
//			int angle = rand() % 181;
//			while (angle == 90)
//			{
//				angle = rand() % 181;
//			}
//
//			m[i] = tanf(angle);
//			c[i] = pow(-1, rand() % 2) * (rand() % 200);
//			randomness[i] = rand() % 2;
//			
//			i++;
//
//
//		}
//
//
//	}
//
//	~FaultLines()
//	{
//		std::cout << "no error" << std::endl;
//		delete[] m;
//		delete[] c;
//		delete[] randomness;
//		std::cout << "after delting" << std::endl;
//	}
//
//
//};
//
//class Patch
//{
//public:
//	int m_x, m_y, m_prev_x, m_prev_y;
//	Vertex* terrain;
//	unsigned int VAO, VBO;
//	bool m_buffer_set;
//
//	Patch(int x=0, int y=0, int prev_x=0, int prev_y=0)
//	{
//		m_x = x;
//		m_y = y;
//		m_prev_x = prev_x;
//		m_prev_y = prev_y;
//		m_buffer_set = false;
//	}
//
//	void setPos(int x, int y)
//	{
//		m_x = x;
//		m_y = y;
//	}
//	void generateHeightMap(FaultLines &faultline)
//	{
//		float** heightmap = new float* [mesh_dim];
//		for (int i = 0; i < mesh_dim; i++)
//		{
//			heightmap[i] = new float[mesh_dim] { 0 };
//		}
//		int number_of_itr = TOTAL_itr;
//		while (number_of_itr > 0)
//		{
//			addfault(heightmap, number_of_itr, faultline.m[number_of_itr], faultline.c[number_of_itr],faultline.randomness[number_of_itr], m_x, m_y);
//			number_of_itr--;
//		}
//		applyFIRfilter(heightmap);
//
//
//		terrain = new Vertex[mesh_dim * mesh_dim];
//		for (int i = 0; i < mesh_dim; i++)
//		{
//			for (int j = 0; j < mesh_dim; j++)
//			{
//				terrain[i * mesh_dim + j].x = ((m_x) * (mesh_dim - 1) * scl + (scl * i)) / (float)SCR_WIDTH;
//				terrain[i * mesh_dim + j].y = heightmap[i][j] / SCR_HEIGHT;
//				terrain[i * mesh_dim + j].z = -((m_y) * (mesh_dim - 1) * scl + (scl * j)) / (float)SCR_WIDTH;
//				terrain[i * mesh_dim + j].text_u = i / (float)mesh_dim;
//				terrain[i * mesh_dim + j].text_v = j / (float)mesh_dim;
//			}
//
//		}
//
//		for (int i = 0; i < mesh_dim; i++)
//		{
//			delete[] heightmap[i];
//		}
//		delete[] heightmap;
//		
//	}
//
//	void setupBuffers(unsigned int EBO)
//	{
//		m_buffer_set = true;
//		glGenVertexArrays(1, &VAO);
//
//		glGenBuffers(1, &VBO);
//
//		glBindVertexArray(VAO);
//
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, mesh_dim * mesh_dim * sizeof(Vertex), terrain, GL_STATIC_DRAW);
//		//delete[]terrain;
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
//		glEnableVertexAttribArray(0);
//
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//
//		glBindVertexArray(0);
//	}
//
//	void render()
//	{
//		glBindVertexArray(VAO);
//		glDrawElements(GL_TRIANGLES, (mesh_dim - 1) * (mesh_dim - 1) * 6, GL_UNSIGNED_INT, 0);
//		glBindVertexArray(0);
//
//	}
//
//	float humanHeight()
//	{
//		std::cout << "camera:" << cameraPos.x << "  " << cameraPos.z << std::endl;
//		float x_pos = fabs( cameraPos.x) * (float)SCR_WIDTH / scl;
//		float z_pos =fabs( cameraPos.z )* (float)SCR_WIDTH / scl;
//		
//		int arr_x = (int)x_pos / mesh_dim;
//		int arr_z = (int)z_pos/  mesh_dim;
//		x_pos = x_pos - arr_x * mesh_dim;
//		z_pos = z_pos - arr_z * mesh_dim;
//		std::cout << x_pos << "   " << z_pos << std::endl;
//		float factor = x_pos - (int)x_pos;
//		float h_bottom = (terrain[(int)z_pos * mesh_dim + (int)x_pos + 1].y - terrain[(int)z_pos * mesh_dim + (int)x_pos].y) * factor + terrain[(int)z_pos * mesh_dim + (int)x_pos].y;
//		float h_top= (terrain[((int)z_pos+1) * mesh_dim + (int)x_pos + 1].y - terrain[((int)z_pos +1)* mesh_dim + (int)x_pos].y) * factor + terrain[((int)z_pos+1) * mesh_dim + (int)x_pos].y;
//		float factorZ = z_pos - (int)z_pos;
//		float h_final = (h_top - h_bottom) * factorZ + h_bottom;
//		h_final = h_final+2;
//		return h_final;
//	}
//
//	void cleanupBuffers()
//	{
//		glDeleteBuffers(1, &VBO);
//		glDeleteVertexArrays(1, &VAO);
//	}
//
//	~Patch()
//	{
//
//		delete[]terrain;
//	}
//
//
//};
//
//
//
//
//
//
//
//
//
//
//
//int main()
//{
//	// glfw: initialize and configure
//	// ------------------------------
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//	// glfw window creation
//	// --------------------
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	glewInit();
//	Shader shader("vshader.vs", "fshader.fs");
//	shader.use();
//
//	glEnable(GL_DEPTH_TEST);
//
//
//
//	
//	int x1, y1, x2, y2;
//	int number_of_itr = TOTAL_itr;
//
//
//
//	float max, min;
//	//findMaxMin(heightmap2, max, min);
//	max = 40000 / SCR_HEIGHT;
//	min = 0 / SCR_HEIGHT;
//	shader.setFloat("max_y", max);
//	
//	cameraPos = glm::vec3(0,  max, 0);
//
//
//	
//
//	unsigned int* indices = new unsigned int[(mesh_dim - 1) * (mesh_dim - 1) * 6];
//	int index = 0;
//
//
//	for (int i = 0; i < (mesh_dim - 1); i++)
//	{
//		for (int j = 0; j < (mesh_dim - 1); j++)
//		{
//			indices[index] = mesh_dim * i + j;
//			index++;
//			indices[index] = mesh_dim * i + j + 1;
//			index++;
//			indices[index] = mesh_dim * (i + 1) + j;
//			index++;
//			indices[index] = mesh_dim * i + j + 1;
//			index++;
//			indices[index] = mesh_dim * (i + 1) + j;
//			index++;
//			indices[index] = mesh_dim * (i + 1) + j + 1;
//			index++;
//		}
//	}
//
//
//
//	unsigned int EBO;
//	glGenBuffers(1, &EBO);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (mesh_dim - 1) * (mesh_dim - 1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
//
//
//
//	unsigned int texture1, texture2, texture3;
//	// texture 1
//	// ---------
//	glGenTextures(1, &texture1);
//	glBindTexture(GL_TEXTURE_2D, texture1);
//	// set the texture wrapping parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// load image, create texture and generate mipmaps
//	int width, height, nrChannels;
//	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
//	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
//	unsigned char* data = stbi_load("C:\\Users\\Abiral Acharya\\Desktop\\bhir.jpg", &width, &height, &nrChannels, 0);
//	if (data)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		std::cout << "Failed to load texture" << std::endl;
//	}
//	stbi_image_free(data);
//	// texture 2
//	// ---------
//	glGenTextures(1, &texture2);
//	glBindTexture(GL_TEXTURE_2D, texture2);
//	// set the texture wrapping parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// load image, create texture and generate mipmaps
//	data = stbi_load("C:\\Users\\Abiral Acharya\\Desktop\\gmass.png", &width, &height, &nrChannels, 0);
//	if (data)
//	{
//		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		std::cout << "Failed to load texture" << std::endl;
//	}
//	stbi_image_free(data);
//
//
//	// texture 3
//// ---------
//	glGenTextures(1, &texture3);
//	glBindTexture(GL_TEXTURE_2D, texture3);
//	// set the texture wrapping parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// load image, create texture and generate mipmaps
//	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
//	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
//	data = stbi_load("C:\\Users\\Abiral Acharya\\Desktop\\snow.jpg", &width, &height, &nrChannels, 0);
//	if (data)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		std::cout << "Failed to load texture" << std::endl;
//	}
//	stbi_image_free(data);
//
//
//
//
//	shader.setInt("texture1", 0);
//	shader.setInt("texture2", 1);
//	shader.setInt("texture3", 2);
//
//
//
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//
//
//	glm::mat4 translationMatrix = glm::mat4(1.0f);
//	translationMatrix = glm::translate(translationMatrix, glm::vec3(-1.0f, 0.33f, 0.00f));
//
//	glm::mat4 trans = glm::mat4(1.0f);
//	trans = glm::rotate(trans, glm::radians(60.0f), glm::vec3(1.0, 0.0, 0.0));
//
//
//	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//	shader.setMat4("projection", projection);
//
//
//	glm::mat4 model = glm::mat4(1.0f);
//	shader.setMat4("model", model);
//
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, texture1);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, texture2);
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, texture3);
//
//
//	FaultLines* faultlines = new FaultLines;
//	faultlines->generate();
//
//	Patch** patch = new Patch * [9];
//	patch[0] = new Patch(-1, 1);
//
//	patch[1] = new Patch(0, 1);
//	patch[2] = new Patch(1, 1);
//
//
//	patch[3] = new Patch(-1, 0);
//	patch[4] = new Patch(0, 0);
//
//
//	patch[5] = new Patch(1, 0);
//	patch[6] = new Patch(-1, -1);
//
//
//	patch[7] = new Patch(0, -1);
//	patch[8] = new Patch(1, -1);
//
//
//	while (!glfwWindowShouldClose(window))
//	{
//		float currentFrame = static_cast<float>(glfwGetTime());
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//
//		processInput(window);
//		
//		glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		//glBindVertexArray(VAO);
//		/*if (!fly)
//		{
//			//cameraPos = glm::vec3(cameraPos.x, patch[4]->humanHeight(), cameraPos.z);
//			cameraPos.y = patch[4]->humanHeight();
//		}
//		*/
//
//		if (!fly)
//		{
//			// Get the current camera position
//			glm::vec3 adjustedCameraPos = cameraPos;
//
//			// Calculate the height of the terrain at the camera's current x and z positions
//			float terrainHeight = patch[4]->humanHeight();
//
//			// Update the camera's y-coordinate based on the terrain height
//			adjustedCameraPos.y = terrainHeight;
//
//			// Update the view matrix with the adjusted camera position
//			glm::mat4 view = glm::lookAt(adjustedCameraPos, adjustedCameraPos + cameraFront, cameraUp);
//			shader.setMat4("view", view);
//		}
//		else
//		{
//			// Fly mode is enabled, update the view matrix using the original camera position
//			glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
//			shader.setMat4("view", view);
//		}
//
//
//
//		/*
//		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
//		shader.setMat4("view", view);
//		*/
//		
//		int cur_x = ((int)cameraPos.x / (mesh_dim*scl/(float)SCR_WIDTH) );
//		if (cameraPos.x<0 )
//		{
//			cur_x--;
//		}
//		
//		int cur_y = -((int)cameraPos.z /(mesh_dim*scl/(float)SCR_WIDTH) );
//		if (cameraPos.z > 0)
//		{
//			cur_y--;
//		}
//		
//		
//		
//		if ((cur_x != patch[4]->m_x) || (cur_y != patch[4]->m_y))
//		{
//			for (int m = 0; m < 9; m++)
//			{
//				delete patch[m];
//			}
//			
//			patch[0] = new Patch(cur_x-1, cur_y+1);
//
//			patch[1] = new Patch(cur_x, cur_y+1);
//			patch[2] = new Patch(cur_x+1, cur_y+1);
//
//
//			patch[3] = new Patch(cur_x-1, cur_y);
//			patch[4] = new Patch(cur_x,cur_y);
//
//
//			patch[5] = new Patch(cur_x+1, cur_y);
//			patch[6] = new Patch(cur_x-1, cur_y-1);
//
//
//			patch[7] = new Patch(cur_x, cur_y-1);
//			patch[8] = new Patch(cur_x+1, cur_y-1);
//
//
//
//
//		}
//
//
//
//
//		for (int i = 0; i < 9; i++)
//		{
//		
//
//
//			if (!(patch[i]->m_buffer_set))
//			{
//				patch[i]->generateHeightMap(*faultlines);
//				patch[i]->setupBuffers(EBO);
//				
//			}
//			patch[i]->render();
//		}
//
//		
//
//
//		glfwSwapBuffers(window);
//
//		glfwPollEvents();
//	}
//	delete faultlines;
//	return 0;
//}
//
//void processInput(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//	float cameraSpeed = static_cast<float>(5 * deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
//		fly = !fly;
//	if (!fly)
//	{
//		cameraSpeed = static_cast<float>(2 * deltaTime);
//	}
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		cameraPos += cameraSpeed * cameraFront;
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		cameraPos -= cameraSpeed * cameraFront;
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//	
//}
//
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//}
//
//
//// ---------------------------------------------------------------------------------------------
//
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//	float xpos = static_cast<float>(xposIn);
//	float ypos = static_cast<float>(yposIn);
//
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//	lastX = xpos;
//	lastY = ypos;
//
//	float sensitivity = 0.1f; // change this value to your liking
//	xoffset *= sensitivity;
//	yoffset *= sensitivity;
//
//	yaw += xoffset;
//	pitch += yoffset;
//
//	// make sure that when pitch is out of bounds, screen doesn't get flipped
//	if (pitch > 89.0f)
//		pitch = 89.0f;
//	if (pitch < -89.0f)
//		pitch = -89.0f;
//
//	glm::vec3 front;
//	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//	front.y = sin(glm::radians(pitch));
//	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//	cameraFront = glm::normalize(front);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	fov -= (float)yoffset;
//	if (fov < 1.0f)
//		fov = 1.0f;
//	if (fov > 45.0f)
//		fov = 45.0f;
//}
//
//void addfault(float** heightmap, int cur_itr, float m, int c,int randomness, int x, int y)
//{
//	float height = max_height - ((float)cur_itr / TOTAL_itr) * del_height;
//	for (int i = 0; i < mesh_dim; i++)
//	{
//		for (int j = 0; j < mesh_dim; j++)
//		{
//
//
//
//			if (inside( i + x * (mesh_dim - 1), j + y * (mesh_dim - 1), randomness, m, c))
//			{
//
//				heightmap[i][j] += height;
//
//
//			}
//
//
//
//		}
//	}
//}
//void setEqn(float x1, float y1, float x2, float y2)
//{
//	X1 = x1;
//	Y1 = y1;
//	X2 = x2;
//	Y2 = y2;
//	slope = (Y2 - Y1) / (X2 - X1);
//}
//bool inside(float x, float y, int randomness, float m, int c)
//{
//	bool value = true;
//	if (randomness == 1)
//	{
//		value = false;
//	}
//	if (y - m * x - c >= 0)
//	{
//		return value;
//	}
//	else
//	{
//		return !value;
//	}
//}
//void findMaxMin(float** heightmap, float& max, float& min)
//{
//	max = 0;
//	min = 0;
//	for (int i = 0; i < mesh_dim; i++)
//	{
//		for (int j = 0; j < mesh_dim; j++)
//		{
//			if (heightmap[i][j] > max)
//			{
//				max = heightmap[i][j];
//			}
//			else if (heightmap[i][j] < min)
//			{
//				min = heightmap[i][j];
//			}
//		}
//	}
//}
//
//void applyFIRfilter(float** heightmap)
//{
//	
//	
//	//left to right
//	for (int i = 0; i < mesh_dim; i++)
//	{
//		for (int j = 1; j < (mesh_dim - 1); j++)
//		{
//			heightmap[i][j] = filter * heightmap[i][j - 1] + (1 - filter) * heightmap[i][j];
//		}
//	}
//
//	//right to left
//	for (int i = 0; i < mesh_dim; i++)
//	{
//		for (int j = (mesh_dim - 2); j > 0; j--)
//		{
//			heightmap[i][j] = filter * heightmap[i][j + 1] + (1 - filter) * heightmap[i][j];
//		}
//	}
//
//	// top to bottom
//	for (int i = 1; i < (mesh_dim - 1); i++)
//	{
//		for (int j = 0; j < mesh_dim; j++)
//		{
//			heightmap[i][j] = filter * heightmap[i - 1][j] + (1 - filter) * heightmap[i][j];
//		}
//	}
//
//	//bottom to top
//	for (int i = (mesh_dim - 2); i > 0; i--)
//	{
//		for (int j = 0; j < mesh_dim; j++)
//		{
//			heightmap[i][j] = filter * heightmap[i + 1][j] + (1 - filter) * heightmap[i][j];
//		}
//	}
//
//
//
//	
//}
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include<shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
    glfwInit();

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("6.1.cubemaps.vs", "6.1.cubemaps.fs");
    Shader skyboxShader("6.1.skybox.vs", "6.1.skybox.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // texture Coords
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
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/container.jpg").c_str());

    vector<std::string> faces
    {
        FileSystem::getPath("resources/textures/skybox/right.jpg"),
        FileSystem::getPath("resources/textures/skybox/left.jpg"),
        FileSystem::getPath("resources/textures/skybox/top.jpg"),
        FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
        FileSystem::getPath("resources/textures/skybox/front.jpg"),
        FileSystem::getPath("resources/textures/skybox/back.jpg")
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene as normal
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
