#include <iostream>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <fstream>
#include "Camera.h"

// vertex data
float vertices[] = {
	// positions     textures
	// front edge
	-0.5,  0.5,  0.0,  0.0,  0.0,
	-0.5, -0.5,  0.0,  0.0,  1.0,
	 0.5, -0.5,  0.0,  1.0,  1.0,
	 0.5,  0.5,  0.0,  0.0,  1.0,
	// back edge
	 0.5,  0.5, -1.0,  0.0,  0.0,
	 0.5, -0.5, -1.0,  0.0,  1.0,
	-0.5, -0.5, -1.0,  1.0,  1.0,
	-0.5,  0.5, -1.0,  0.0,  1.0,
	// left edge
    -0.5,  0.5, -1.0,  0.0,  0.0,
    -0.5, -0.5, -1.0,  0.0,  1.0,
    -0.5, -0.5,  0.0,  1.0,  1.0,
    -0.5,  0.5,  0.0,  0.0,  1.0,
	// right edge
	 0.5,  0.5,  0.0,  0.0,  0.0,
	 0.5, -0.5,  0.0,  0.0,  1.0,
	 0.5, -0.5, -1.0,  1.0,  1.0,
	 0.5,  0.5, -1.0,  0.0,  1.0,
	// upper edge
    -0.5,  0.5, -1.0,  0.0,  0.0,
	-0.5,  0.5,  0.0,  0.0,  1.0,
	 0.5,  0.5,  0.0,  1.0,  1.0,
	 0.5,  0.5, -1.0,  0.0,  1.0,
	 // bottom edge
	-0.5, -0.5,  0.0,  0.0,  0.0,
	-0.5, -0.5, -1.0,  0.0,  1.0,
	 0.5, -0.5, -1.0,  1.0,  1.0,
	 0.5, -0.5,  0.0,  0.0,  1.0,
};

unsigned int indices[] = {
	0,  1,  2, // front edge
	2,  3,  0,
	4,  5,  6, // back edge
	6,  7,  4,
	8,  9, 10, // left edge
   10, 11,  8,
   12, 13, 14, // right edge
   14, 15, 12,
   16, 17, 18, // upper edge
   18, 19, 16,
   20, 21, 22, // bottom edge
   22, 23, 20,
};

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -3.0f);
float cameraSpeed = 5.0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float deltaTime, Camera& camera);

struct ShaderSources {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderSources ParseShaders(const std::string& filepath)
{
	std::ifstream stream(filepath);
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	ShaderType type = ShaderType::NONE;
	std::stringstream ss[2];

	std::string line;
	while (getline(stream, line))
	{
		if (line.find("shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int)type] << line << std::endl;
		}
	}
	return {
		ss[0].str(), ss[1].str()
	};
}

static GLuint CompileShader(const std::string& shader, const std::string& shaderType, GLenum shaderTypeEnum)
{
	GLuint shaderID = glCreateShader(shaderTypeEnum);
	const char* shaderSource = shader.c_str();
	glShaderSource(shaderID, 1, &shaderSource, NULL);
	glCompileShader(shaderID);

	GLint shader_compiled;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shader_compiled);
	if (shader_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(shaderID, 1024, &log_length, message);
		std::cout << "ERROR: COMPILING " << shaderType << " SHADER: " << message << std::endl;
	}
	return shaderID;
}

static GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLuint vShader = CompileShader(vertexShader, "VERTEX", GL_VERTEX_SHADER);
	GLuint fShader = CompileShader(fragmentShader, "FRAGMENT", GL_FRAGMENT_SHADER);
	GLuint program = glCreateProgram();
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);
	GLint program_linked;
	glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
	if (program_linked != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetProgramInfoLog(program, 1024, &log_length, message);
	}
	glDeleteShader(vShader);
	glDeleteShader(fShader);
	return program;
}

GLenum mode = GL_TRIANGLES;

int main()
{
	// INIT GLFW (window, context)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Faield to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}
	std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << std::endl;

	// Texture
	GLuint rock_texture = SOIL_load_OGL_texture(
		"res/textures/rock.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	if (rock_texture == 0)
	{
		std::cout << "SOIL loading error: " << SOIL_last_result() << std::endl;
	}
	
	// Viewport
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);

	// Buffers
	unsigned int vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	unsigned int indices_buffer;
	glGenBuffers(1, &indices_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	ShaderSources shaders = ParseShaders("res/shaders/Basic.shader");

	GLuint program = CreateShader(shaders.VertexSource, shaders.FragmentSource);
	glUseProgram(program);
	glBindTexture(GL_TEXTURE_2D, rock_texture);

	Camera camera;
	

	// position
	glm::vec3 position = glm::vec3(0.0f, 0.0f, -5.0f);

	// projection transform
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	// uniform locations
	int projectionLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	float deltaTime = 0.0f;
	float currentTime = (float) glfwGetTime();
	float prevTime = (float) glfwGetTime();
	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;
		currentTime = (float)glfwGetTime();
		// INPUT
		processInput(window, deltaTime, camera);
		// RENDERING
		
		glm::mat4 model = glm::mat4(1.0);
		
		// dynamic transforms
		// model
		model = glm::translate(model, position);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(55.0f), glm::vec3(1.0, 0.0, 0.0));
		int modelLoc = glGetUniformLocation(program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// view
		glm::mat4 view;
		view = camera.getViewMatrix();
		int viewLoc = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(mode, 36, GL_UNSIGNED_INT, nullptr);
		// OPEN GL BASE LOOP FUNCTIONS
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(program);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, float deltaTime, Camera& camera)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	float forwardDelta = 0.0f;
	float rightDelta = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		forwardDelta += deltaTime * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		forwardDelta -= deltaTime * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		rightDelta   -= deltaTime * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		rightDelta   += deltaTime * cameraSpeed;
	camera.updatePosition(glm::vec3(rightDelta, 0.0f, forwardDelta));
}
