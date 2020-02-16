#pragma once
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <sstream>
#include <fstream>
#include <string>

# define PI 3.14159265358979323846

class BaseApp
{
public:
	static GLFWwindow * win;

	static GLuint programShader;

	static glm::vec3 cameraPosition;
	static glm::vec3 cameraUp;
	static glm::vec3 cameraGaze;
	static glm::vec3 cameraRight;

	static glm::vec3 lightOffSet;

	static GLfloat cameraSpeed;
	static GLfloat heightFactor;

	static GLFWmonitor* PrimaryMonitor;
	static const GLFWvidmode* Monitor;
	static bool isFullScreen;

	static float AspectRatio;
	static int moveTexture;

	int widthTexture, heightTexture;

	BaseApp(const char * tMap, const char * hMap);

	static void KeyPressed(GLFWwindow *win, int key, int scode, int act, int mods);
	static void WindowSizeChanged(GLFWwindow* win, int width, int height);
	static void ErrorCallback(int error, const char * description);

		
	void InitShaders(const char * vertexPath, const char * fragmentPath);

	void UpdateScene();

	void Run();

	virtual void InitShadersAndTextures() = 0;
	virtual void InitCamera() = 0;
	virtual void InitTextureShaderValues() = 0;
	virtual void BuildLayout() = 0;
	virtual void Render() = 0;
	

	void InitTexture(const char * textureMap, const char * heightMap);


	static glm::vec3 baseCameraPosition;
	static glm::vec3 baseCameraUp;
	static glm::vec3 baseCameraGaze;
	static glm::vec3 baseCameraRight;

	static void BaseCameraSettings();

	const char * textureMap;
	const char * heightMap;
	
};

