/* // IMPORTANT 1
#include <GL/glew.h>	// Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
						// initializing OpenGL and binding inputs
#include "learnopengl/stb_image.h"

#include <glm/glm.hpp> // GLM is an optimized math library with syntax to similar to OpenGL Shading Language'

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl/shader_m.h"
#include "learnopengl/camera.h"
#include "learnopengl/model.h"

glfwMakeContextCurrent(window);
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
glfwSetCursorPosCallback(window, mouse_callback);
glfwSetScrollCallback(window, scroll_callback);

// tell GLFW to capture our mouse
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

// Initialize GLEW
glewExperimental = true; // Needed for core profile
if (glewInit() != GLEW_OK)
{
	std::cerr << "Failed to create GLEW" << std::endl;
	glfwTerminate();
	return -1;
} */