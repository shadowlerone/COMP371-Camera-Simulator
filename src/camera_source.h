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

#include "constants.h"

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

// base settings:
//  ISO 400
//  F/8.0
//  1/60th of a second

// Camera camera;

string APERTURE_STRING;
string SHUTTER_SPEED_STRING;
string FOCAL_LENGTH_STRING;

bool firstMouse = false;
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
	if (PINHOLE){
		APERTURE_STRING = "Pinhole";
		return;
	}
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
	if(SHUTTER_INDEX < 0) {
		stream << "1/";
	}
	stream << floor(calc_shutter(abs(SHUTTER_INDEX)));
	if (SHUTTER_INDEX >= 0)
	{
		stream << "\"";
	}
	SHUTTER_SPEED_STRING = stream.str();
}

void set_strings()
{
	set_aperture_string();
	set_shutter_string();
}

void capture_image()
{
	cout << "saving image" << endl;
	glClear(GL_ACCUM_BUFFER_BIT);
	glAccum(GL_LOAD, 1);
	// glClear(GL_ACCUM_BUFFER_BIT);
	CAPTURING = true;
}

void calc_exposure() {
	calc_shutter();
	calc_aperture();
}
void set_brightness()
{
	// std::cout << BRIGHTNESS << std::endl;
	calc_exposure();
	BRIGHTNESS = pow(2, (SHUTTER_INDEX - INITIAL_SHUTTERSPEED) + (INITIAL_APERTURE - APERTURE_INDEX) + log2((float)ISO / (float)INITIAL_ISO));
}

#pragma endregion

void setup_shader(Shader *currentShader)
{
	currentShader->setFloat("focusDistance", FOCUS_DISTANCE);
}

#pragma endregion

#endif