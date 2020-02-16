#pragma once
#include "BaseApp.h"


class SphericalApp : public BaseApp
{
public:
	SphericalApp(const char * tMap, const char * hMap);
	~SphericalApp();

	void InitShadersAndTextures();
	void InitCamera();
	void InitTextureShaderValues();
	void BuildLayout();
	void Render();

private:
	float radius;
	int verticalCount;
	int horizontalCount;

	int verticesSize;
	int indicesSize;

	glm::vec2* sphereVertices;
	GLuint *sphereIndices;
};

