#ifndef CAMERA_SOURCE_H
#define CAMERA_SOURCE_H

#include <cmath>
#include "learnopengl/stb_image_write.h"
#include "learnopengl/shader.h"
#include "learnopengl/camera.h"

#pragma region includes

// #include <GL
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
// IMPORTANT 1
#include <GL/glew.h>	// Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
						// initializing OpenGL and binding inputs

#include <glm/glm.hpp> // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/ext.hpp>
#ifdef FREEGLUT
#include <GL/freeglut.h>
#else
// #include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
// #include <stdio.h>
#include <sstream>

#pragma endregion

#pragma region constants
#define MAX_ISO 3200
#define MIN_ISO 100
#define MAX_APERTURE 32
#define MAX_APERTURE_INDEX 8
#define MIN_APERTURE 1
#define MAX_SHUTTER_SPEED_INDEX 12
#define MIN_SHUTTER_SPEED_INDEX -26

#define MAX_FOCUS_DISTANCE 200.
#define MIN_FOCUS_DISTANCE 1e-10

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920

#define INITIAL_APERTURE 6
#define INITIAL_SHUTTERSPEED -12
#define INITIAL_ISO 400

#define QUALITY 100

const std::string phongVertex = "phong.vert.glsl";
const std::string phongFragment = "phong.frag.glsl";

const std::string blinnPhongVertex = "shaders/blinnPhong.vert.glsl";
const std::string blinnPhongFragment = "shaders/blinnPhong.frag.glsl";

const std::string toonVertex = "toon.vert.glsl";
const std::string toonFragment = "toon.frag.glsl";

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

using namespace std;

// #define DEBUG_RIGHT() {std::cout <<  "DEBUG: " << __FILE__ << ":" << __func__ << ":" << __LINE__ << " RIGHT: " << glm::to_string(camera.Right) << std::endl;}

#pragma region global variables

/* Global variables */

float APERTURE;
int APERTURE_INDEX = INITIAL_APERTURE; // should map to F/8
bool PINHOLE = false;
int ISO = INITIAL_ISO;
float SHUTTER_SPEED;
int SHUTTER_INDEX = INITIAL_SHUTTERSPEED;
bool AUTO_ISO = true;
float FOCUS_DISTANCE = 0;
bool CAPTURING = false;
float capture_time = 0;
float focal_length = 35;
float BRIGHTNESS = 1;
int frame = 0;
float velocity = 0;
float acceleration = 0.1f;

#pragma endregion

// Buffers

#pragma region buffers

GLFWwindow *window;

unsigned int cameraFBO;
unsigned int hdrFBO;

unsigned int pingpongFBO[2];
unsigned int pingpongBuffer[2];

unsigned int colorBuffers[2];
Shader *currentShader;
#pragma endregion
// base settings:
//  ISO 400
//  F/8.0
//  1/60th of a second

Camera camera;
float deltaTime; // Delta Time

string APERTURE_STRING;
string SHUTTER_SPEED_STRING;
string FOCAL_LENGTH_STRING;

bool firstMouse = false;
float yaw = -90.0f;
float pitch = 0;
// glm::vec3 cameraFront;

float lastX = WINDOW_WIDTH / 2, lastY = WINDOW_HEIGHT / 2;

#pragma region helper functions

float FOV(float f)
{
	return glm::degrees(2 * atan(35 / (2 * f)));
}

void calc_aperture()
{
	APERTURE = pow(2, APERTURE_INDEX / 2.);
}

void calc_shutter()
{
	SHUTTER_SPEED = pow(2, SHUTTER_INDEX / 2);
}

float calc_shutter(int x)
{
	return pow(2, x / 2.);
}

void incr_aperture()
{
	PINHOLE = false;
	APERTURE_INDEX = min(APERTURE_INDEX + 1, MAX_APERTURE_INDEX);
}
void decr_aperture()
{
	APERTURE_INDEX = max(-1, APERTURE_INDEX - 1);
	if (APERTURE_INDEX == 0)
	{
		PINHOLE = true;
	}
}

void incr_shutter_speed()
{
	SHUTTER_INDEX = min(MAX_SHUTTER_SPEED_INDEX, SHUTTER_INDEX + 1);
}
void decr_shutter_speed()
{
	SHUTTER_INDEX = max(MIN_SHUTTER_SPEED_INDEX, SHUTTER_INDEX - 1);
}

void incr_ISO()
{
	ISO = min(ISO * 2, MAX_ISO);
}

void decr_ISO()
{
	if (ISO <= MIN_ISO)
	{
		ISO = MIN_ISO;
		AUTO_ISO = true;
	}
	else
	{
		ISO /= 2;
	}
}

void set_aperture_string()
{
	std::stringstream stream;
	if (APERTURE >= 10)
	{
		stream << std::fixed << std::setprecision(0) << APERTURE;
		// sprintf(APERTURE_STRING, );
	}
	else
	{
		stream << std::fixed << std::setprecision(1) << APERTURE;
	}
	APERTURE_STRING = stream.str();
}

void set_shutter_string()
{
	std::stringstream stream;
	stream << calc_shutter(abs(SHUTTER_INDEX));
	if (SHUTTER_INDEX >= 0)
	{
		stream << "\"";
	}
	SHUTTER_SPEED_STRING = stream.str();
}

void capture_image()
{
	cout << "saving image" << endl;
	CAPTURING = true;
}

void set_brightness()
{
	BRIGHTNESS = pow(2, (INITIAL_SHUTTERSPEED - SHUTTER_INDEX) + (INITIAL_APERTURE - APERTURE_INDEX) + log2(ISO / INITIAL_ISO));
}

#pragma endregion

void setup_shader(Shader *currentShader)
{
	currentShader->setFloat("focusDistance", FOCUS_DISTANCE);
}

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

void perTick(GLFWwindow *window)
{
	// processInput(window);
	// std::cout << "ticking" << std::endl;
	glm::vec3 front;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.Yaw -= velocity;
		front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		front.y = sin(glm::radians(camera.Pitch));
		front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		camera.Front = glm::normalize(front);
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.Yaw += velocity;
		// glm::vec3 front;
		front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		front.y = sin(glm::radians(camera.Pitch));
		front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		camera.Front = glm::normalize(front);
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		camera.Pitch += velocity;
		// Pitch += yoffset;
		front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		front.y = sin(glm::radians(camera.Pitch));
		front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		camera.Front = glm::normalize(front);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{

		camera.Pitch -= velocity;
		// glm::vec3 front;
		front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		front.y = sin(glm::radians(camera.Pitch));
		front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		camera.Front = glm::normalize(front);
	}
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// DEBUG_RIGHT()

	glm::vec3 front;
	std::cout << "woop" << std::endl;

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

	front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	front.y = sin(glm::radians(camera.Pitch));
	front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	camera.Front = glm::normalize(front);
	// DEBUG_RIGHT()
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	focal_length = max(yoffset + focal_length, 1.0);
	FOCUS_DISTANCE = min(max(0., FOCUS_DISTANCE + xoffset / 100.), 1.);
	cout << focal_length << "mm" << endl;
	cout << FOCUS_DISTANCE * 100 << "%" << endl;
	camera.setFov(FOV(focal_length)); // hijacked
									  // camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
}

#pragma endregion

void update(float dt)
{
	if (!CAPTURING)
	{
		frame = 0;
		return;
	}
	frame++;
	capture_time += dt;
	if (capture_time >= SHUTTER_SPEED)
	{
		int pixels[WINDOW_HEIGHT * WINDOW_WIDTH];
		stbi_flip_vertically_on_write(true);
		glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		stbi_write_jpg(("test" + std::to_string(frame) + ".jpg").c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, 3, pixels, QUALITY);
		CAPTURING = false;
		capture_time = 0;
	}
}

void setup()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Capsule 04 - Toon Shader", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		// exit(-1);
		exit(-1);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	std::cout << "reading shader" << endl;
	Shader blinnPhong = Shader(blinnPhongVertex.c_str(), blinnPhongFragment.c_str());
	currentShader = &blinnPhong;
	glGenFramebuffers(1, &cameraFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, cameraFBO);
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
}

void bloom(int amount)
{
	bool horizontal = true, first_iteration = true;
	cout << "REading blur shader" << endl;
	Shader shaderBlur = Shader("viewing.vert.glsl", "blur.frag.glsl");

	// int amount = 10;
	shaderBlur.use();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		shaderBlur.setInt("horizontal", horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);
		// RenderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#endif