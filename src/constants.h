#ifndef CAMERA_CONSTANTS_H
#define CAMERA_CONSTANTS_H


#pragma region constants
#define MAX_ISO 6400
#define MIN_ISO 100
#define MAX_APERTURE 32
#define MAX_APERTURE_INDEX 32
#define MIN_APERTURE 1
#define MAX_SHUTTER_SPEED_INDEX 12
#define MIN_SHUTTER_SPEED_INDEX -26

#define MAX_FOCUS_DISTANCE 200.
#define MIN_FOCUS_DISTANCE 1e-10

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920

#define INITIAL_APERTURE 6
#define INITIAL_SHUTTERSPEED -12
#define INITIAL_ISO 1600

#define QUALITY 100


// Shader Paths
#define phongVertex "phong.vert.glsl";
#define phongFragment "phong.frag.glsl";
#define blinnPhongVertex "shaders/blinnPhong.vert.glsl"
#define blinnPhongFragment "shaders/blinnPhong.frag.glsl"
#define toonVertex "toon.vert.glsl"
#define toonFragment "toon.frag.glsl"
#pragma endregion

#endif