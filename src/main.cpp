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
// #include "learnopengl/shader_m.h"
// #include "learnopengl/camera.h"
#include "learnopengl/model.h"
// #include "Camera.h"

#include "Shape.h"
#include "Square.h"
#include "Cube.h"

#include "utils.h"

#include "constants.h"
#include "camera_source.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

void renderCube();
void renderQuad();

unsigned int loadCubemap(vector<std::string> faces);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void perTick(GLFWwindow *window, Camera &camera, float deltaTime);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

int main()
{
	// return 0;
	cout << "Greetings!" << endl
		 << "Initializing glwf!" << endl;

#pragma region GLFW INIT
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// cout << "Creating window" << endl;
	DEBUGLN("Creating window")
	GLFWwindow *window = glfwCreateWindow(width, height, "Camera Simulator", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	DEBUGLN("making context current")
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	DEBUGLN("Setting up GLEW")
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

#pragma endregion

#pragma region IMGUI INIT
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	  // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
#pragma endregion

	DEBUGLN("creating hdr framebuffer object and depth object")
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	unsigned int colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma region shadows
	DEBUGLN("Dealing with shadows")

	DEBUGLN("Shadows skipped")
	/* 	Shader simpleDepthShader("shaders/3.1.2.shadow_mapping_depth.vs", "shaders/3.1.2.shadow_mapping_depth.fs");

		unsigned int depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);

		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

		unsigned int depthMap;
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 */

	/* #pragma region SHADOW MAP

		unsigned int depthCubemap;
		glGenTextures(1, &depthCubemap);
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
						 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	#pragma endregion */
#pragma endregion

	DEBUGLN("Creating camera Framebuffer object")
	unsigned int cameraFBO;
	DEBUGLN("Creating blur Framebuffer object")

	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];

	// unsigned int motionBlurTex[2];
	// unsigned int motionBlurFBO[2];
	DEBUGLN("Creating color Framebuffer object")

	unsigned int colorBuffers[2];

	DEBUGLN("setting callbacks")
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	std::cout << "reading 'blinnphong shaders' shader" << endl;

	DEBUGLN("loading main lighting shader")
	Shader blinnPhong = Shader(blinnPhongVertex, blinnPhongFragment);
	DEBUGLN("Setting up HDR Shader")
	Shader hdrShader("shaders/6.hdr.vs", "shaders/6.hdr.fs");

	DEBUGLN("loading lighting shader as current shader")
	Shader *currentShader = &blinnPhong;

	DEBUGLN("initializing camera")
	Camera camera = Camera();
	// currentShader = &blinnPhong;

	// todo: figure out

	DEBUGLN("Setting up shapes vector")
	std::vector<Shape *> shapes;

	DEBUGLN("initializing delta time and last time")
	float deltaTime;
	float lastTime = glfwGetTime();

	DEBUGLN("Setting up rotation (used for movement)")
	float rot = 0.0f;

	glm::mat4 viewProjection;

	// return 0;
#pragma region Geometry
	// d("setting up geometry")
	Square floor;
	floor.translate(glm::vec3(0.0f, -0.5f, 0.0f));
	floor.rotate(-pi / 2, glm::vec3(1.0f, 0.0f, 0.0f));
	floor.scale(glm::vec3(10.0f));

	// shapes.push_back(&floor);
	// DEBUG_RIGHT()
	Cube red;
	red.setColor(50.0f, 0.0f, 0.0f);

	Cube green;
	green.setColor(0.0f, 1.0f, 0.0f);
	green.translate(glm::vec3(2.0f, 1.0f, 0.5f));
	green.rotate(pi / 2, glm::vec3(std::sqrt(2.0f) / 2, std::sqrt(2.0f) / 2, 0.0f));
	green.scale(glm::vec3(1.5f));

	Cube blue;
	blue.setColor(0.0f, 0.0f, 25.0f);
	blue.translate(glm::vec3(-1.5f, 1.0f, -1.5f));
	blue.rotate(-pi / 4, glm::vec3(1.0f, 0.0f, 0.0f));

	Cube cyan;
	cyan.setColor(0.0f, 1.0f, 1.0f);
	cyan.translate(glm::vec3(-0.5f, 2.0f, 0.0f));
	cyan.rotate(pi / 4, glm::vec3(std::sqrt(2.0f) / 2, std::sqrt(2.0f) / 2, 0.0f));

	shapes.push_back(&red);
	// shapes.push_back(&green);
	shapes.push_back(&blue);
	// shapes.push_back(&cyan);
	float skyboxVertices[] = {
		// positions
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f};

#pragma endregion
#pragma region motion blur
	// d("starting motion blur region")
	// unsigned int numberOfMotionBlurBuffers = 2;
	// unsigned int MotionBlurBuffers[numberOfMotionBlurBuffers];

	// glGenFramebuffers(numberOfMotionBlurBuffers, MotionBlurBuffers);
	// glBindFramebuffer(GL_FRAMEBUFFER, MotionBlurBuffers[0]);

	// // The texture we're going to render to
	// unsigned int previousFrame;

	// glGenTextures(1, &previousFrame);

	// // "Bind" the newly created texture : all future texture functions will modify this texture
	// glBindTexture(GL_TEXTURE_2D, previousFrame);

	// // Give an empty image to OpenGL ( the last "0" )
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA, GL_FLOAT, 0);
#pragma endregion

	// d("loading motion blur shader")
	Shader motionBlurShader("shaders/6.hdr.vs", "shaders/motionblur.fs");
	Model ourModel("assets/models/chess_set_1k.dae");
	int pixels[WINDOW_HEIGHT * WINDOW_WIDTH];
	stbi_flip_vertically_on_write(true);
	// d("Done loading motion blur shader")

	// Model ourModel("assets/models/chess_set_4k.fbx_Scene/chess_set_4k.fbx_Scene.fbx");
	// Model ourModel("assets/models/chess_set_4k.obj");

	// d("setting up skybox")
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

	// d("done setting up skybox")

	// DEBUG_RIGHT()

	// d("setting up camera")
	camera.setAspectRatio((float)width / height);
	camera.setFov(FOV(focal_length)); // hijacked

	// DEBUG_RIGHT()

	// camera.Right = glm::normalize(glm::cross(camera.Front, {0.f,1.f,0.f}));
	// camera.Position = (glm::vec3(0.0f, 1.0f, -5.0f));
	// camera.MouseSensitivity = 0.001f;
	// DEBUG_RIGHT()

	std::vector<glm::vec3> lightPoss;
	std::vector<glm::vec3> lightCol;
	// d("setting up lights")
	lightPoss.push_back({5.0f, 5.0f, 0.0f});
	lightPoss.push_back({5.0f, 5.0f, 0.0f});
	lightCol.push_back({50.f, 0.f, 0.f});
	lightCol.push_back({0.f, 0.f, 25.f});
	// DEBUG_RIGHT()

	// glm::vec3 lightPos{;

	// d("setting up mouse callbacks")
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// glfwSetCursorPosCallback(window, mouse_callback);

	// d("enabling opengl depth testing and face culling")
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// d("setting up light vector for shader")
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

	// d("transforming loaded model")
	for (int i = 0; i < ourModel.meshes.size(); i++)
	{
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3((i % 8), glm::floor(i / 8), 0.0f));
		ourModel.transforms.push_back(trans);
	}

	ourModel.transform = glm::rotate(ourModel.transform, glm::radians(-90.f), glm::vec3(1.0, 0.0, .0));
	ourModel.transform = glm::scale(ourModel.transform, glm::vec3(10.0));
	// DEBUG_RIGHT()
	// shader.use();
	// shader.setInt("diffuseTexture", 0);

	// d("setting up hdr shader")
	hdrShader.use();
	hdrShader.setInt("hdrBuffer", 0);
	/* 	motionBlurShader.use();
		motionBlurShader.setInt("scene", 1);
		motionBlurShader.setInt("prevFrame", 2); */

	// d("setting up motionoblur frame buffer object")
	unsigned int motionBlurFBO[2];
	unsigned int motionBlurTex[2];
	glGenFramebuffers(2, motionBlurFBO);
	glGenTextures(2, motionBlurTex);
	// d("setting up frame buffer textures")
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, motionBlurFBO[i]);
		glBindTexture(GL_TEXTURE_2D, motionBlurTex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, motionBlurTex[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}
	// d("hit the loop")

#pragma region camera setup

	set_aperture_string();
	set_shutter_string();
	// set_ISO_string();

#pragma endregion

#pragma region LOOP
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
#pragma region ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// ImGui::ShowDemoWindow(); // Show demo window! :)
		{
			ImGui::Begin("Camera Settings");
			// ImGui::Text("This is some useful text.");
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

			ImGui::Text("ISO: %d", ISO);
			ImGui::PushButtonRepeat(true);
			if (ImGui::ArrowButton("##left", ImGuiDir_Left))
			{
				decr_ISO();
			}
			ImGui::SameLine(0.0f, spacing);
			if (ImGui::ArrowButton("##right", ImGuiDir_Right))
			{
				incr_ISO();
			}
			ImGui::PopButtonRepeat();

			ImGui::Text("Aperture: f/%s", APERTURE_STRING.c_str());
			ImGui::PushButtonRepeat(true);
			if (ImGui::ArrowButton("##left", ImGuiDir_Left))
			{
				decr_aperture();
			}
			ImGui::SameLine(0.0f, spacing);
			if (ImGui::ArrowButton("##right", ImGuiDir_Right))
			{
				incr_aperture();
			}
			ImGui::PopButtonRepeat();

			ImGui::Text("Shutter Speed: %s", SHUTTER_SPEED_STRING.c_str());
			ImGui::PushButtonRepeat(true);
			if (ImGui::ArrowButton("##left", ImGuiDir_Left))
			{
				decr_shutter_speed();
			}
			ImGui::SameLine(0.0f, spacing);
			if (ImGui::ArrowButton("##right", ImGuiDir_Right))
			{
				incr_shutter_speed();
			}
			ImGui::PopButtonRepeat();

			set_strings();
			/* 	ImGui::SliderInt("ISO", &ISO, 100, 3200);
				ImGui::InputInt("Shutter Speed", &SHUTTER_INDEX, MIN_SHUTTER_SPEED_INDEX, MAX_SHUTTER_SPEED_INDEX);
				ImGui::End(); */
		}
#pragma endregion
		// DEBUG_RIGHT()
		// // d("===========================================================================")
		// lblock
		// d("TOP OF LOOP")
		// d("binding to hdr buffer")
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// d("resetting shader")
		currentShader->use();

		// d("calculating delta time")
		deltaTime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();
		// d("Calculated Delta time: " << deltaTime)
		// d("pre-tick")
		perTick(window, camera, deltaTime);
		// d("post tick");

// update(deltaTime);
#pragma region update
		// cout << "Updating values " << endl;
		// d("hit is capturing loop")
		if (!CAPTURING)
		{
			frame = 0;
			// d("not capturing")
		}
		else
		{
			capture_time += deltaTime;
			// d("current capture time: " << capture_image)
			// d("current shutter speed:" << SHUTTER_SPEED)
			if (capture_time >= SHUTTER_SPEED)
			{
				// d("shutter speed hit, saving image")
				int pixels[WINDOW_HEIGHT * WINDOW_WIDTH];
				glBindFramebuffer(GL_FRAMEBUFFER, motionBlurFBO[frame % 2]);

				stbi_flip_vertically_on_write(true);
				glAccum(GL_RETURN, 1.0);
				glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
				stbi_write_jpg("test.jpg", WINDOW_WIDTH, WINDOW_HEIGHT, 3, pixels, QUALITY);
				CAPTURING = false;
				std::cout << "image_saved" << std::endl;
				capture_time = 0;
			}
			else
			{
				// d("shutter speed not hit, incrementing frame")
				frame++;
			}
		}
		// frame++;

#pragma endregion
		// DEBUG_RIGHT()

#pragma region positioning elements
		// d("positioning objects in frame")
		rot += deltaTime * pi / 2;	  // pi/2 per second
		rot = std::fmod(rot, 2 * pi); // overflow prevention
		lightPoss[0].x = 15 * std::cos(rot);
		lightPoss[1] = glm::vec3(5 * sin(rot), 10 * sin(rot) * cos(rot), 5 * sin(rot));
		blue.resetTransform();
		red.resetTransform();
		blue.translate(lightPoss[1]);
		red.translate(lightPoss[0]);

#pragma endregion
		// DEBUG_RIGHT()

		// reset viewport
		// glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// int fragmentFocusDistance = glGetUniformLocation(, "focusDistance");
		// glUniform1f(fragmentFocusDistance, FOCUS_DISTANCE);
		// camera.setLookAt(cameraFront);
		//  camera.Front = glm::vec3(0,0,-1);

		// d("setting up camera view direction")
		glm::vec3 front;
		camera.setFov(FOV(focal_length)); // hijacked

		viewProjection = camera.getProjMatrix() * camera.getViewMatrix();
#pragma region RENDER

		// d("setting up lights")
		set_brightness();
		currentShader->setFloat("exposure", BRIGHTNESS);
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

		// d("setting up light shapes")
		for (auto shape : shapes)
		{
			currentShader->setMat4("mvp", viewProjection * shape->getTransform());
			currentShader->setMat4("m", shape->getTransform());
			currentShader->setVec3("objectColor", shape->getColor());
			currentShader->setInt("isLight", 1);
			shape->draw();
		}
		currentShader->setInt("isLight", 0);

		// DEBUG_RIGHT()
		// d("setting up models")
		for (int i = 0; i < ourModel.meshes.size(); i++)
		{
			currentShader->setMat4("mvp", viewProjection * ourModel.transforms[i] * ourModel.transform);

			currentShader->setMat4("m", ourModel.transforms[i] * ourModel.transform);
			currentShader->setVec3("objectColor", glm::vec3(0.f));

			ourModel.meshes[i].Draw(*currentShader);
		}

#pragma endregion

		// d("setting up skybox")
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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// d("done with skybox")

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// d("computing hdr image value")
		hdrShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		hdrShader.setInt("hdr", 1);

		hdrShader.setFloat("exposure", BRIGHTNESS);
		// std::cout<<BRIGHTNESS << std::endl;

		// d("rendering hdr output")
		renderQuad();

		// blur
		// DEBUG_RIGHT()
		// bloom(10);
		// d("computing motion blur if capturing")
		if (!CAPTURING)
		{
			// d("not capturing, no motion blur")

			// glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
			// bloom(10);
		}
		else
		{
// d("rendering motion blur")
#pragma region motion blur
			motionBlurShader.use();
			bool even = frame % 2;
			glBindFramebuffer(GL_FRAMEBUFFER, motionBlurFBO[even]);
			if (frame == 0)
			{
				glBindTexture(GL_TEXTURE_2D, colorBuffer);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (frame == 0) ? colorBuffer : motionBlurTex[!even]);
			}
			renderQuad();

			// glBindFramebuffer(GL_FRAMEBUFFER, motionBlurFBO[frame % 2]);
			// char c;
			// cin >> c;
			// glAccum(GL_RETURN, 1.0);
			// glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
			// stbi_write_jpg("test.jpg", WINDOW_WIDTH, WINDOW_HEIGHT, 3, pixels, QUALITY);

			// glAccum(GL_ACCUM, 0.1);
			// glClearColor(0.f, 0.f, 0.f, 1.0f);

			// glClear(GL_COLOR_BUFFER_BIT);
			// glBindTexture(GL_TEXTURE_2D, previousFrame);

			// // Give an empty image to OpenGL ( the last "0" )
			// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA, GL_FLOAT, 0);

			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			// glClear();
			// d("returning to main framebuffer")
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#pragma endregion
		}
		// glfwSwapBuffers(window);
		// d("swapping framebuffers")
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		// d("polling events")
	}
#pragma endregion
	currentShader = nullptr;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}

#pragma region fbo
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
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	// bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,	// top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
																// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-right
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-left
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,	// top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f	  // bottom-left
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,
			1.0f,
			0.0f,
			0.0f,
			1.0f,
			-1.0f,
			-1.0f,
			0.0f,
			0.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			1.0f,
			1.0f,
			-1.0f,
			0.0f,
			1.0f,
			0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
#pragma endregion
#pragma region callbacks

// shamelessly stolen from learnOpenGL
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
								  // camera.ProcessMouseMovement(xoffset, yoffset);
								  // lastX = xpos;
								  // lastY = ypos;
								  // const float sensitivity = 0.1f;
								  // xoffset *= sensitivity;
								  // yoffset *= sensitivity;
								  // yaw += xoffset;
								  // // pitch += yoffset;
								  // camera.ProcessMouseMovement(xoffset, yoffset);
								  // std::cout << "yaw: " << camera.Yaw << endl;
								  // std::cout << "Pitch: " << camera.Pitch << endl;
								  // std::cout << "xoffset: " << xoffset << endl;
								  // std::cout << "yoffset: " << yoffset << endl;

	// if (pitch > 89.0f)
	// 	pitch = 89.0f;
	// if (pitch < -89.0f)
	// 	pitch = -89.0f;
	// camera.ProcessMouseMovement(xoffset, yoffset);
	// cout << "pitch: " << pitch << endl;
	// cout << "yaw: " << yaw << endl;

	// glm::vec3 direction;
	// direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	// direction.y = sin(glm::radians(pitch));
	// direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	// cameraFront = glm::normalize(direction);
	// cout << glm::to_string(cameraFront) << endl;
}

void perTick(GLFWwindow *window, Camera &camera, float deltaTime)
{
	// processInput(window);
	// std::cout << "ticking" << std::endl;
	// d("ticking")
	glm::vec3 front;
	bool triggered = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		triggered = true;
		// d("moving camera")
		// d("movement direction: forward")
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		triggered = true;
		// d("moving camera")
		// d("movement direction: backward")
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		triggered = true;
		// d("moving camera")
		// d("movement direction: left")
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		triggered = true;
		// d("moving camera")
		// d("movement direction: right")
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		triggered = true;
		camera.Yaw += velocity;
		// d("camera.Yaw: " << camera.Yaw)
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		triggered = true;
		camera.Yaw -= velocity;
		// d("camera.Yaw: " << camera.Yaw)
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		triggered = true;
		// d("moving up")
		camera.Position += camera.MovementSpeed * deltaTime * camera.Up;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		triggered = true;
		// d("moving down")

		camera.Position -= camera.MovementSpeed * deltaTime * camera.Up;
	}

	front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	front.y = sin(glm::radians(camera.Pitch));
	front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	camera.Front = glm::normalize(front);
	camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.

	// d("Camera: " << camera)
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// DEBUG_RIGHT()

	// std::cout << "woop" << std::endl;

	if (action != GLFW_PRESS)
	{
		// velocity = 0;
		return;
	}
	if (CAPTURING)
	{
		return;
	}
	// if (action == GLFW_REPEAT){
	// velocity += acceleration * deltaTime;
	velocity = 0.5f;
	// }
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_UP:
		incr_aperture();
		calc_aperture();
		set_aperture_string();
		cout << "aperture index: " << APERTURE_INDEX << endl;
		cout << "aperture: f/" << APERTURE_STRING << endl;
		break;
	case GLFW_KEY_DOWN:
		decr_aperture();
		calc_aperture();
		set_aperture_string();
		cout << "aperture index: " << APERTURE_INDEX << endl;
		cout << "aperture: f/" << APERTURE_STRING << endl;
		break;
	case GLFW_KEY_LEFT:
		incr_shutter_speed();
		calc_shutter();
		set_shutter_string();
		cout << "shutter speed index: " << SHUTTER_INDEX << endl;
		cout << "shutter speed:" << SHUTTER_SPEED_STRING << endl;

		break;
	case GLFW_KEY_RIGHT:

		decr_shutter_speed();
		calc_shutter();
		set_shutter_string();
		cout << "shutter speed index: " << SHUTTER_INDEX << endl;
		cout << "shutter speed:" << SHUTTER_SPEED_STRING << endl;
		break;
	case GLFW_KEY_SPACE:
		capture_image();
		break;
	default:
		break;
	}
	// DEBUG_RIGHT()
	// DEBUG_RIGHT()
	set_brightness();
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	focal_length = max(yoffset + focal_length, 1.0);
	FOCUS_DISTANCE = min(max(0., FOCUS_DISTANCE + xoffset / 100.), 1.);
	cout << focal_length << "mm" << endl;
	cout << FOCUS_DISTANCE * 100 << "%" << endl;
}

#pragma endregion