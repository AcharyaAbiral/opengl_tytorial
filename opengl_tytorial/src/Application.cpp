#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

 #include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>
 #include <glm/gtc/type_ptr.hpp>

class Vertex
{
public:
    float x;
    float y;
    float z;
    Vertex() {}
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void GenerateTranslation(float m[], int x, int y);
void initMatrix(float translation[]);


// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 transformation1;\n"
"uniform mat4 transformation2;\n"
"void main()\n"
"{\n"
"   gl_Position = transformation2*transformation1*vec4(aPos, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

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

    glewInit();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentshader);

    // link shader
    unsigned int shaderprogram = glCreateProgram();
    glAttachShader(shaderprogram, vertexShader);
    glAttachShader(shaderprogram, fragmentshader);
    glLinkProgram(shaderprogram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentshader);

    float scl = 60;
    /*
    terrain[i].x = ((i)*scl) / 600;
    terrain[i].z = (i * scl) / 600;
    terrain[i].y = (rand() % 400) / 600.0;
    std::cout << terrain[i].x << std::endl;
    std::cout << terrain[i].y << std::endl;
    std::cout << terrain[i].z << std::endl;*/

    Vertex* terrain = new Vertex[30 * 30];
    for (int i = 0; i < 30; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            terrain[i * 30 + j].x = (scl * i) / 600;
            terrain[i * 30 + j].y = (rand() % 100) / 600.0;
            terrain[i * 30 + j].z = (scl * j) / 600;
        }
    }

    unsigned int* indices = new unsigned int[29 * 29 * 6];
    int index = 0;

    // for(int i=0;i<)

    for (int i = 0; i < 29; i++)
    {
        for (int j = 0; j < 29; j++)
        {
            indices[index] = 30 * i + j;
            index++;
            indices[index] = 30 * i + j + 1;
            index++;
            indices[index] = 30 * (i + 1) + j;
            index++;
            indices[index] = 30 * i + j + 1;
            index++;
            indices[index] = 30 * (i + 1) + j;
            index++;
            indices[index] = 30 * (i + 1) + j + 1;
            index++;
        }
    }

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 900 * sizeof(Vertex), terrain, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 29 * 29 * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // glfwSwapBuffers(window);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUseProgram(shaderprogram);
    int transformation1Loc = glGetUniformLocation(shaderprogram, "transformation1");
    std::cout << transformation1Loc<<std::endl;

    float translationMatrix[16];
    initMatrix(translationMatrix);
    GenerateTranslation(translationMatrix, -600, 200);
    glUniformMatrix4fv(transformation1Loc, 1, GL_TRUE, translationMatrix);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(60.0f), glm::vec3(1.0, 0.0, 0.0));

    int transformationLoc = glGetUniformLocation(shaderprogram, "transformation2");
    std::cout << transformationLoc;
    glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(trans));

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        // glPointSize(10);
        // glDrawArrays(GL_TRIANGLES, 0, 900);
        glDrawElements(GL_TRIANGLES, 5046, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    return 0;
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
        // rotation[4 * i + i] = 1;
        // scaling[4 * i + i] = 1;
        // reflection[4 * i + i] = 1;
    }
}