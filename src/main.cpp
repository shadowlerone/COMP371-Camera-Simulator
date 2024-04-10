#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "learnopengl/shader.h"
#include "learnopengl/camera.h"
#include "learnopengl/stb_image.h"
// #include <learnopengl/filesystem.h>
#include "learnopengl/shader_m.h"
#include "learnopengl/camera.h"
#include "learnopengl/model.h"
// #include "Camera.h"

#include "Shape.h"
#include "Square.h"
#include "Cube.h"

#include "camera_source.h"

#ifdef WINDOW_WIDTH
const int width = WINDOW_WIDTH;
#else
const int width = 1920;
#endif
#ifdef WINDOW_HEIGHT
const int height = WINDOW_HEIGHT;
#else
const int height = 1080;
#endif
const float pi = std::acos(-1.0f);

// const std::string phongVertex = "phong.vert.glsl";
// const std::string phongFragment = "phong.frag.glsl";

// const std::string blinnPhongVertex = "blinnPhong.vert.glsl";
// const std::string blinnPhongFragment = "blinnPhong.frag.glsl";

// const std::string toonVertex = "toon.vert.glsl";
// const std::string toonFragment = "toon.frag.glsl";

/* phong.build(phongVertex, phongFragment);
blinnPhong.build(blinnPhongVertex, blinnPhongFragment);
toon.build(toonVertex, toonFragment); */
// Shader phong = Shader(phongVertex.c_str(), phongFragment.c_str());
// Shader toon = Shader(toonVertex.c_str(), toonFragment.c_str());

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
	{
		return;
	}
	key_callback(window, key, scancode, action, mods);
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
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

int main(int argc, char **argv)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow *window = glfwCreateWindow(width, height, "Capsule 04 - Toon Shader", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, scroll_callback);
	std::cout << "reading 'blinnphong shaders' shader" << endl;
	Shader blinnPhong = Shader(blinnPhongVertex.c_str(), blinnPhongFragment.c_str());
	Shader *currentShader = &blinnPhong;
	camera = Camera();
	// DEBUG_RIGHT()
	// currentShader = &blinnPhong;

	// todo: figure out

	std::vector<Shape *> shapes;

	float lastTime = glfwGetTime();

	float rot = 0.0f;

	glm::mat4 viewProjection;

	Square floor;
	floor.translate(glm::vec3(0.0f, -0.5f, 0.0f));
	floor.rotate(-pi / 2, glm::vec3(1.0f, 0.0f, 0.0f));
	floor.scale(glm::vec3(10.0f));

	shapes.push_back(&floor);
	// DEBUG_RIGHT()
	// Cube red;
	// red.setColor(1.0f, 0.0f, 0.0f);

	// Cube green;
	// green.setColor(0.0f, 1.0f, 0.0f);
	// green.translate(glm::vec3(2.0f, 1.0f, 0.5f));
	// green.rotate(pi / 2, glm::vec3(std::sqrt(2.0f) / 2, std::sqrt(2.0f) / 2, 0.0f));
	// green.scale(glm::vec3(1.5f));

	// Cube blue;
	// blue.setColor(0.0f, 0.0f, 1.0f);
	// blue.translate(glm::vec3(-1.5f, 1.0f, -1.5f));
	// blue.rotate(-pi / 4, glm::vec3(1.0f, 0.0f, 0.0f));

	// Cube cyan;
	// cyan.setColor(0.0f, 1.0f, 1.0f);
	// cyan.translate(glm::vec3(-0.5f, 2.0f, 0.0f));
	// cyan.rotate(pi / 4, glm::vec3(std::sqrt(2.0f) / 2, std::sqrt(2.0f) / 2, 0.0f));

	// shapes.push_back(&red);
	// shapes.push_back(&green);
	// shapes.push_back(&blue);
	// shapes.push_back(&cyan);

	Model ourModel("assets/models/chess_set_1k.dae");
	// Model ourModel("assets/models/chess_set_4k.fbx_Scene/chess_set_4k.fbx_Scene.fbx");
	// Model ourModel("assets/models/chess_set_4k.obj");

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	std::vector<std::string> faces = {
		"assets/skybox/right.jpg",
		"assets/skybox/left.jpg",
		"assets/skybox/top.jpg",
		"assets/skybox/bottom.jpg",
		"assets/skybox/front.jpg",
		"assets/skybox/back.jpg"};
	unsigned int cubemapTexture = loadCubemap(faces);
	std::cout << "Loading skybox shader" << std::endl;
	Shader skyboxShader = Shader("shaders/skybox.vert.glsl", "shaders/skybox.frag.glsl");

	// DEBUG_RIGHT()

	camera.setAspectRatio((float)width / height);

	// DEBUG_RIGHT()

	// camera.Right = glm::normalize(glm::cross(camera.Front, {0.f,1.f,0.f}));
	// camera.Position = (glm::vec3(0.0f, 1.0f, -5.0f));
	camera.MouseSensitivity = 0.01f;
	// DEBUG_RIGHT()

	std::vector<glm::vec3> lightPoss;
	std::vector<glm::vec3> lightCol;

	lightPoss.push_back({5.0f, 5.0f, 0.0f});
	lightPoss.push_back({5.0f, 5.0f, 0.0f});
	lightCol.push_back({1.f, 0.f, 0.f});
	lightCol.push_back({0.f, 0.f, 1.f});
	// DEBUG_RIGHT()

	// glm::vec3 lightPos{;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// DEBUG_RIGHT()

	// glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	camera.setFov(FOV(focal_length)); // hijacked
	// DEBUG_RIGHT()

	// camera.
	// glm::vec3
	for (int i = 0; i < lightPoss.size(); i++)
	{
		currentShader->setVec3("pointLights[" + std::to_string(i) + "].position", lightPoss[i]);

		// currentShader->setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.5f * lightCol[i]);
		// currentShader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f * lightCol[i]);
		// currentShader->setVec3("pointLights[" + std::to_string(i) + "].specular", 1.f * lightCol[i]);
		currentShader->setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.5f, .5f, .5f);
		currentShader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, .8f, .8f);
		currentShader->setVec3("pointLights[" + std::to_string(i) + "].specular", 1.f, 1.f, 1.f);
		currentShader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
		currentShader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
		currentShader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
	}
	// DEBUG_RIGHT()

	for (int i = 0; i < ourModel.meshes.size(); i++)
	{
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3((i % 8) / 10., glm::floor(i / 8) / 10., 0.0f));
		ourModel.transforms.push_back(trans);
	}

	ourModel.transform = glm::rotate(ourModel.transform, glm::radians(-90.f), glm::vec3(1.0, 0.0, .0));
	// DEBUG_RIGHT()

	while (!glfwWindowShouldClose(window))
	{
		// DEBUG_RIGHT()
		perTick(window);
		currentShader->use();

		deltaTime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();
		update(deltaTime);
		// DEBUG_RIGHT()

		rot += deltaTime * pi / 2;	  // pi/2 per second
		rot = std::fmod(rot, 2 * pi); // overflow prevention

		lightPoss[0].x = 15 * std::cos(rot);
		lightPoss[1].y = 10 * std::cos(rot/11.);
		// DEBUG_RIGHT()

		// int fragmentFocusDistance = glGetUniformLocation(, "focusDistance");
		// glUniform1f(fragmentFocusDistance, FOCUS_DISTANCE);
		// camera.setLookAt(cameraFront);
		//  camera.Front = glm::vec3(0,0,-1);
		viewProjection = camera.getProjMatrix() * camera.getViewMatrix();
		// DEBUG_RIGHT()

		currentShader->use();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// DEBUG_RIGHT()

		// currentShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		// currentShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		// currentShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		// currentShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// currentShader->setFloat("material.shininess", 32.0f);

		// for (int i = 0; i < lightPoss.size(); i++)
		// {
		// 	currentShader->setVec3("pointLights[" + std::to_string(i) + "].position", lightPoss[i]);
		// }
		for (int i = 0; i < lightPoss.size(); i++)
		{
			currentShader->setVec3("pointLights[" + std::to_string(i) + "].position", lightPoss[i]);

			currentShader->setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.5f * lightCol[i]);
			currentShader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f * lightCol[i]);
			currentShader->setVec3("pointLights[" + std::to_string(i) + "].specular", 1.f * lightCol[i]);
			// currentShader->setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.5f, .5f, .5f);
			// currentShader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, .8f, .8f);
			// currentShader->setVec3("pointLights[" + std::to_string(i) + "].specular", 1.f, 1.f, 1.f);
			currentShader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
			currentShader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
			currentShader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
		}
		// DEBUG_RIGHT()

		// currentShader->setVec3("lightPos", lightPos);
		currentShader->setFloat("focusDistance", FOCUS_DISTANCE);
		currentShader->setVec3("viewPos", camera.Position);
		// DEBUG_RIGHT()

		for (auto shape : shapes)
		{
			currentShader->setMat4("mvp", viewProjection * shape->getTransform());
			currentShader->setMat4("m", shape->getTransform());
			currentShader->setVec3("objectColor", shape->getColor());
			shape->draw();
		}
		// DEBUG_RIGHT()

		for (int i = 0; i < ourModel.meshes.size(); i++)
		{
			currentShader->setMat4("mvp", viewProjection * ourModel.transforms[i] * ourModel.transform);

			currentShader->setMat4("m", ourModel.transforms[i] * ourModel.transform);
			ourModel.meshes[i].Draw(*currentShader);
		}
		// DEBUG_RIGHT()

		// ourModel.Draw(*currentShader);

		//

		// draw skybox as last
		glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		// view = ; // remove translation from the view matrix
		skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
		skyboxShader.setMat4("projection", camera.getProjMatrix());
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default


		// blur
		// DEBUG_RIGHT()

		if (!CAPTURING)
		{
			// glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
			// bloom(10);

			glfwSwapBuffers(window);
		}
		else
		{
			// glClearColor(0.f, 0.f, 0.f, 1.0f);

			// glClear(GL_COLOR_BUFFER_BIT);
			glfwSwapBuffers(window);
		}

		glfwPollEvents();
	}

	currentShader = nullptr;

	glfwTerminate();

	return 0;
}
