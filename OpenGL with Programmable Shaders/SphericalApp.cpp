#include "SphericalApp.h"

 
SphericalApp::SphericalApp(const char * tMap, const char * hMap)
	:BaseApp(tMap, hMap)
{
	radius = 350.0f;
	verticalCount = 125;
	horizontalCount = 250;

}


SphericalApp::~SphericalApp()
{
	delete[] sphereVertices;
	delete[] sphereIndices;
}



void SphericalApp::InitShadersAndTextures()
{
	InitShaders("./spherical.vert","./spherical.frag");
	glUseProgram(programShader);
	InitTexture(textureMap, heightMap);
}

void SphericalApp::InitCamera()
{
	
	cameraPosition = glm::vec3(0.f, 600.f + radius, 0.f);;
	cameraUp = glm::vec3(0.0, 0.0, 1.0);
	cameraGaze = glm::vec3(0.0, -1.0, 0.0);
	cameraRight = cross(cameraUp, cameraGaze);

	baseCameraPosition = cameraPosition;
	baseCameraUp = cameraUp;
	baseCameraGaze = cameraGaze;
	baseCameraRight = cameraRight;
	
}
void SphericalApp::InitTextureShaderValues()
{
	glUniform1i(glGetUniformLocation(programShader, "textureMap"), 0);
	glUniform1i(glGetUniformLocation(programShader, "heightMap"), 1);
	glUniform1i(glGetUniformLocation(programShader, "horizontal"), 250);
	glUniform1i(glGetUniformLocation(programShader, "vertical"), 125);
	glUniform1f(glGetUniformLocation(programShader, "heightFactor"), heightFactor);
	glUniform1f(glGetUniformLocation(programShader, "radius"), radius);
	glUniform1f(glGetUniformLocation(programShader, "hAngle"), (2.0f * PI) / horizontalCount);
	glUniform1f(glGetUniformLocation(programShader, "vAngle"), PI / verticalCount);
	glUniform1i(glGetUniformLocation(programShader, "moveTex"), moveTexture);
}
void SphericalApp::BuildLayout()
{
	verticesSize = (verticalCount - 1) * (horizontalCount + 1) + 2;
	indicesSize = (horizontalCount * 2 * 3) + horizontalCount * (verticalCount - 2) * 6;

	sphereVertices = new glm::vec2[verticesSize];


	glm::vec2 topVertex = glm::vec2(0, 0);
	glm::vec2 bottomVertex = glm::vec2(0, verticalCount);


	unsigned int index = 0;

	sphereVertices[index++] = topVertex;

	for (int i = 1; i <= verticalCount - 1; i++)
	{
		for (int j = 0; j <= horizontalCount; j++)
		{
			sphereVertices[index++] = glm::vec2(j, i);
		}
	}

	sphereVertices[index++] = bottomVertex;

	sphereIndices = new GLuint[indicesSize];

	index = 0;

	for (int i = 1; i <= horizontalCount; i++)
	{
		sphereIndices[index++] = 0;
		sphereIndices[index++] = i + 1;
		sphereIndices[index++] = i;
	}

	int startIndex = 1;
	int ringVertexCount = horizontalCount + 1;
	for (int i = 0; i < verticalCount - 2; ++i)
	{
		for (int j = 0; j < horizontalCount; ++j)
		{
			sphereIndices[index++] = startIndex + i * ringVertexCount + j;
			sphereIndices[index++] = startIndex + i * ringVertexCount + j + 1;
			sphereIndices[index++] = startIndex + (i + 1)*ringVertexCount + j;

			sphereIndices[index++] = startIndex + (i + 1)*ringVertexCount + j;
			sphereIndices[index++] = startIndex + i * ringVertexCount + j + 1;
			sphereIndices[index++] = startIndex + (i + 1)*ringVertexCount + j + 1;
		}
	}

	int bottomPoleIndex = verticesSize - 1;
	startIndex = bottomPoleIndex - ringVertexCount;

	for (int i = 0; i < horizontalCount; i++)
	{
		sphereIndices[index++] = bottomPoleIndex;
		sphereIndices[index++] = startIndex + i;
		sphereIndices[index++] = startIndex + i + 1;
	}
}

void SphericalApp::Render()
{
	glClearStencil(0);
	glClearDepth(1.0f);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, sphereVertices);
	glDrawElements(GL_TRIANGLES, (unsigned int)indicesSize, GL_UNSIGNED_INT, sphereIndices);
	glDisableVertexAttribArray(0);
}

