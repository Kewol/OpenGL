#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <direct.h>
#include <list>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ShaderUtils.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void recursiveTriangleFunction(std::vector<float>& container, glm::vec3 A, glm::vec3 B, glm::vec3 C, int layer);

void recursiveTestFunction(int& incrementInteger, int level);

// constants
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixVal = 0.2f;

int main()
{
	std::cout << "starting program..." << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader shader("shaders/vertex.vs", "shaders/fragment.fs");
	//Shader shader("shaders/SierpinskiVert.vs", "shaders/SierpinskiFrag.fs");

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	float vertices[] =
	{
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	unsigned int indices[] =
	{
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	//std::vector<float> vec;

	//call recursive function here to load in the triangles in the array
	//recursiveTriangleFunction(vec, glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), 7);

	// vertex buffer, element buffer and vertex array
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(float), &vec[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	////////////////////////
	// texture generation //
	////////////////////////
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glGenTextures(1, &texture2);

	// first texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR::Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// second texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR::Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// rendering
	shader.use();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);

	glBindVertexArray(VAO);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.setFloat("mixVal", mixVal);

		float time = glfwGetTime();

		glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(0.5f, 0.0f, 0.0f));
		trans = glm::rotate(trans, time, glm::vec3(0.0f, 1.0f, 0.0f));

		unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glDrawElements(GL_TRIANGLES, 2, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, vec.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixVal += 0.01f;
		if (mixVal >= 1.0f)
			mixVal = 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixVal -= 0.01f;
		if (mixVal <= 0.0f)
			mixVal = 0.0f;
	}
}

void recursiveTriangleFunction(std::vector<float>& container, glm::vec3 A, glm::vec3 B, glm::vec3 C, int layer)
{
	if (layer == 0)
	{
		return;
	}

	glm::vec3 D = ((A + B) / 2.0f);
	glm::vec3 E = ((A + C) / 2.0f);
	glm::vec3 F = ((B + C) / 2.0f);

	// ugly! but will improve this later on once i have a better understanding
	container.push_back(D.x);
	container.push_back(D.y);
	container.push_back(D.z);

	container.push_back(E.x);
	container.push_back(E.y);
	container.push_back(E.z);

	container.push_back(F.x);
	container.push_back(F.y);
	container.push_back(F.z);

	layer--;

	recursiveTriangleFunction(container, A, D, E, layer);
	recursiveTriangleFunction(container, D, B, F, layer);
	recursiveTriangleFunction(container, E, F, C, layer);
}

void recursiveTestFunction(int& incrementInteger, int level)
{
	if (level == 0)
	{
		return;
	}

	incrementInteger++;
	level--;

	recursiveTestFunction(incrementInteger, level);
}

