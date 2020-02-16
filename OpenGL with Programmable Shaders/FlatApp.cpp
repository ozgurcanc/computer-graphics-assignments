#include "FlatApp.h"



FlatApp::FlatApp(const char * tMap, const char * hMap)
	:BaseApp(tMap,hMap)
{
	heightFactor = 10.0f;
	
}


FlatApp::~FlatApp()
{
	delete[] flatVertices;
	delete[] flatIndices;
}

void FlatApp::InitShadersAndTextures()
{
	
	InitShaders("./flat.vert", "./flat.frag");
	glUseProgram(programShader);
	InitTexture(textureMap, heightMap);
}

void FlatApp::InitCamera()
{
	
	cameraPosition = glm::vec3(widthTexture / 2, widthTexture / 10, -widthTexture / 4);
	cameraUp = glm::vec3(0.0, 1.0, 0.0);
	cameraGaze = glm::vec3(0.0, 0.0, 1.0);
	cameraRight = cross(cameraUp, cameraGaze);

	baseCameraPosition = cameraPosition;
	baseCameraUp = cameraUp;
	baseCameraGaze = cameraGaze;
	baseCameraRight = cameraRight;
}
void FlatApp::InitTextureShaderValues()
{
	glUniform1i(glGetUniformLocation(programShader, "textureMap"), 0);
	glUniform1i(glGetUniformLocation(programShader, "heightMap"), 1);
	glUniform1i(glGetUniformLocation(programShader, "widthTexture"), widthTexture);
	glUniform1i(glGetUniformLocation(programShader, "heightTexture"), heightTexture);
	glUniform1f(glGetUniformLocation(programShader, "heightFactor"), heightFactor);
	glUniform1i(glGetUniformLocation(programShader, "moveTex"), moveTexture);
}
void FlatApp::BuildLayout()
{
	int w = widthTexture;
	int h = heightTexture;

	verticesSize = (w + 1) * (h + 1);
	indicesSize = 4 * w * h;

	flatVertices = new glm::vec3[verticesSize];

	int index = 0;
	for (int i = 0; i <= w; i += 1)
	{
		for (int j = 0; j <= h; j += 1)
		{
			flatVertices[index++] = glm::vec3(i, 0, j);
		}
	}

	flatIndices = new GLuint[indicesSize];

	index = 0;
	for (int i = 0; i < w; i += 1)
	{
		for (int j = 0; j < h; j += 1)
		{
			flatIndices[index++] = i * (h + 1) + j + 1;
			flatIndices[index++] = (i + 1) * (h + 1) + j + 1;
			flatIndices[index++] = (i + 1) * (h + 1) + j;
			flatIndices[index++] = i * (h + 1) + j;
		}
	}
}

void FlatApp::Render()
{
	glClearStencil(0);
	glClearDepth(1.0f);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, flatVertices);
	glDrawElements(GL_QUADS, (unsigned int)indicesSize, GL_UNSIGNED_INT, flatIndices);
	glDisableVertexAttribArray(0);
}

