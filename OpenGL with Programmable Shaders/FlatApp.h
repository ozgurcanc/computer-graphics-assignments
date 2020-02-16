#pragma once
#include "BaseApp.h"

class FlatApp : public BaseApp
{
public:
	FlatApp(const char * tMap, const char * hMap);
	~FlatApp();

	void InitShadersAndTextures();
	void InitCamera();
	void InitTextureShaderValues();
	void BuildLayout();
	void Render();

private:
	
	int verticesSize;
	int indicesSize;

	glm::vec3* flatVertices;
	GLuint *flatIndices;
};

