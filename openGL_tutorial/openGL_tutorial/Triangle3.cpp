
#include <iostream>
#include <sstream>

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

int gWindowWidth = 800;
int gWindowHeight = 800;
const char* TITLE = "Hello Triangle with color rotation";

GLFWwindow* gWindow = nullptr;

// shader
const char* vertexShaderSrc = R"(
	#version 330 core
	layout (location = 0) in vec3 pos;
	layout (location = 1) in vec3 color;
	out vec3 vert_color;
	void main()
	{
		vert_color = color;
		gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	}
)";


const char* fragmentShaderSrc =R"(
	#version 330 core
	in vec3 vert_color;
	out vec4 frag_color;
	void main()
	{
		frag_color = vec4(vert_color, 1.0f);
	}
)";
bool initOpenGL()
{
	if (!glfwInit())
	{
		std::cerr << "Error ! GLFW initialization failed " << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, TITLE, nullptr, nullptr);
	if (gWindow == nullptr)
	{
		std::cerr << "Error ! window is not created for opengl context" << std::endl;
		return false;
	}

	glfwMakeContextCurrent(gWindow);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Error ! GLEW initialization failed " << std::endl;
		return false;
	}
	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	return true;
}
int main()
{
	if (!initOpenGL())
	{
		std::cerr << "Error ! OpenGL initialization failed " << std::endl;
		return -1;
	}

	float elapseTime = 0.0f;

	float vertices[] =
	{
		 0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,	// Top vertex
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,	// Bottom right vertex
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,	// Bottom left vertex
	};

	unsigned int vbo, vao;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, nullptr);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// create vertex shader
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShaderSrc, nullptr);
	glCompileShader(vs);

	int result;
	char infoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vs, sizeof(infoLog), nullptr, infoLog);
		std::cerr << "Error ! unable to compile vertex shader " << infoLog << std::endl;
	}

	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShaderSrc, nullptr);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fs, sizeof(infoLog), nullptr, infoLog);
		std::cerr << "Error ! unable to compile fragment shader " << infoLog << std::endl;
	}

	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);
	int colorLocation = glGetAttribLocation(shaderProgram, "vertexShaderSrc");
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
		std::cerr << "Error ! unable to link shader program " << infoLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	// rendering loop
	while (!glfwWindowShouldClose(gWindow))
	{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		double currentTime = glfwGetTime();
		double deltatime = currentTime - elapseTime;
		if (deltatime > 0.2)
		{
			elapseTime = currentTime;
			float temp_vertices[] =
			{
				vertices[0],  vertices[1],  vertices[2],  vertices[9],  vertices[10], vertices[11],
						vertices[6],  vertices[7],  vertices[8],  vertices[15], vertices[16], vertices[17],
						vertices[12], vertices[13], vertices[14], vertices[3],  vertices[4],  vertices[5]
			};
			std::memcpy(vertices, temp_vertices, sizeof(vertices));
			deltatime = 0.0;
		}
		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(vertices), (void*)(sizeof(float) * 3));
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(gWindow);
	}

	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();
	return 0;
}