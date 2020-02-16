#include "BaseApp.h"

#include <jpeglib.h>

GLFWwindow * BaseApp::win;

GLuint BaseApp::programShader;

glm::vec3 BaseApp::cameraPosition;
glm::vec3 BaseApp::cameraUp;
glm::vec3 BaseApp::cameraGaze;
glm::vec3 BaseApp::cameraRight;

glm::vec3 BaseApp::lightOffSet;

GLfloat BaseApp::cameraSpeed;
GLfloat BaseApp::heightFactor;

GLFWmonitor* BaseApp::PrimaryMonitor;
const GLFWvidmode* BaseApp::Monitor;
bool BaseApp::isFullScreen;

float BaseApp::AspectRatio;
int BaseApp::moveTexture;

glm::vec3 BaseApp::baseCameraPosition;
glm::vec3 BaseApp::baseCameraUp;
glm::vec3 BaseApp::baseCameraGaze;
glm::vec3 BaseApp::baseCameraRight;


BaseApp::BaseApp(const char * tMap, const char * hMap)
	:heightMap(hMap),
	textureMap(tMap)
{
	win = NULL;
	cameraSpeed = 0.0f;
	heightFactor = 0.0f;
	isFullScreen = false;

	AspectRatio = 1.0f;
	moveTexture = 0;

	lightOffSet = glm::vec3(0.0, 0.0, 0.0);

}

void BaseApp::KeyPressed(GLFWwindow *win, int key, int scode, int act, int mods) {

	if (act == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_P:
			if (!isFullScreen)
			{
				glfwSetWindowMonitor(win, PrimaryMonitor, 0, 0, Monitor->width, Monitor->height, Monitor->refreshRate);
				AspectRatio = (float)Monitor->width / Monitor->height;
			}
			else
			{
				glfwSetWindowMonitor(win, NULL, 100, 100, 1000, 1000, 0);
				AspectRatio = 1.0f;
			}
			isFullScreen = !isFullScreen;
			break;

		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(win, GLFW_TRUE);
			break;

		case GLFW_KEY_I:
			cameraSpeed = 0.0;
			lightOffSet = glm::vec3(0, 0, 0);
			BaseCameraSettings();
			break;

		case GLFW_KEY_X:
			cameraSpeed = 0.0;
		}
	}

	if (act == GLFW_PRESS || act == GLFW_REPEAT) {
		switch (key) {

		case GLFW_KEY_R:
		
			heightFactor += 0.5f;
			glUniform1f(glGetUniformLocation(programShader, "heightFactor"), heightFactor);
			break;
		
		case GLFW_KEY_F:
		
			heightFactor -= 0.5f;
			glUniform1f(glGetUniformLocation(programShader, "heightFactor"), heightFactor);
			break;
		
		case GLFW_KEY_A:
		
			cameraRight = glm::rotate(cameraRight, glm::radians(0.5f), cameraUp);
			cameraGaze = glm::rotate(cameraGaze, glm::radians(0.5f), cameraUp);
			break;
		
		case GLFW_KEY_D:
		
			cameraRight = glm::rotate(cameraRight, -glm::radians(0.5f), cameraUp);
			cameraGaze = glm::rotate(cameraGaze, -glm::radians(0.5f), cameraUp);
			break;
		
		case GLFW_KEY_S:
		
			cameraUp = glm::rotate(cameraUp, glm::radians(0.5f), cameraRight);
			cameraGaze = glm::rotate(cameraGaze, glm::radians(0.5f), cameraRight);
			break;
		
		case GLFW_KEY_W:
		
			cameraUp = glm::rotate(cameraUp, -glm::radians(0.5f), cameraRight);
			cameraGaze = glm::rotate(cameraGaze, -glm::radians(0.5f), cameraRight);
			break;
		
		case GLFW_KEY_Q:
		
			moveTexture--;
			glUniform1i(glGetUniformLocation(programShader, "moveTex"), moveTexture);
			break;
		

		case GLFW_KEY_E:
		
			moveTexture++;
			glUniform1i(glGetUniformLocation(programShader, "moveTex"), moveTexture);
			break;
		
		case GLFW_KEY_UP:
		
			lightOffSet += baseCameraGaze * 5.0f;
			break;
		
		case GLFW_KEY_DOWN:
		
			lightOffSet += baseCameraGaze * -5.0f;
			break;
		
		case GLFW_KEY_RIGHT:
		
			lightOffSet += baseCameraRight * -5.0f;
			break;
		
		case GLFW_KEY_LEFT:
		
			lightOffSet += baseCameraRight * 5.0f;
			break;
		
		case GLFW_KEY_T:
		
			lightOffSet += baseCameraUp * 5.0f;
			break;
		
		case GLFW_KEY_G:
		
			lightOffSet += baseCameraUp * -5.0f;
			break;
		
		case GLFW_KEY_Y:
			cameraSpeed += 0.01;
			break;

		case GLFW_KEY_H:
			cameraSpeed -= 0.01;
			break;

		}
	}
}

void BaseApp::WindowSizeChanged(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
	AspectRatio = (float)width / (float)height;
}

void BaseApp::ErrorCallback(int error, const char * description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void BaseApp::InitShaders(const char * vertexPath, const char * fragmentPath)
{
	std::string vertexShaderCode;
	std::string fragmentShaderCode;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	vertexShaderFile.open(vertexPath);
	fragmentShaderFile.open(fragmentPath);
	std::stringstream vertexShaderStream, fragmentShaderStream;
	
	vertexShaderStream << vertexShaderFile.rdbuf();
	fragmentShaderStream << fragmentShaderFile.rdbuf();
	
	vertexShaderFile.close();
	fragmentShaderFile.close();
	
	vertexShaderCode = vertexShaderStream.str();
	fragmentShaderCode = fragmentShaderStream.str();

	const char* vertexShaderCode_ = vertexShaderCode.c_str();
	const char* fragmentShaderCode_ = fragmentShaderCode.c_str();

	GLuint fragmentShader;
	GLuint vertexShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode_, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode_, NULL);
	glCompileShader(fragmentShader);

	programShader = glCreateProgram();
	glAttachShader(programShader, vertexShader);
	glAttachShader(programShader, fragmentShader);
	glLinkProgram(programShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void BaseApp::UpdateScene() {

	cameraPosition += cameraGaze * cameraSpeed;

	glm::mat4 mProj = glm::perspective(45.0f, AspectRatio, 0.1f, 1000.0f);

	glm::vec3 target = cameraPosition + cameraGaze * 0.1f;

	glm::mat4 mView = glm::lookAt(cameraPosition, target, cameraUp);

	glm::mat4 mModelViewProj = mProj * mView;

	glUniformMatrix4fv(glGetUniformLocation(programShader, "ModelViewProjection"), 1, GL_FALSE, glm::value_ptr(mModelViewProj));

	glUniform3fv(glGetUniformLocation(programShader, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

	glUniform3fv(glGetUniformLocation(programShader, "lightOffSet"), 1, glm::value_ptr(lightOffSet));
}

void BaseApp::Run()
{
	glfwSetErrorCallback(ErrorCallback);

	if (!glfwInit()) {
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	win = glfwCreateWindow(1000, 1000, "HW3 - OpenGL", NULL, NULL);
	glViewport(0, 0, 1000, 1000);
	
	if (!win) {
		glfwTerminate();
		exit(-1);
	}

	PrimaryMonitor = glfwGetPrimaryMonitor();
	Monitor = glfwGetVideoMode(PrimaryMonitor);

	glfwMakeContextCurrent(win);

	if (GLEW_OK != glewInit()) {
		glfwTerminate();
		exit(-1);
	}

	InitShadersAndTextures();
	InitTextureShaderValues();

	glfwSetKeyCallback(win, KeyPressed);
	glfwSetWindowSizeCallback(win, WindowSizeChanged);

	InitCamera();
	BuildLayout();

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(win)) {
		UpdateScene();

		Render();

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glfwDestroyWindow(win);
	glfwTerminate();
}

void BaseApp::InitTexture(const char * textureMap, const char * heightMap)
{
	for(int t = 0; t<2 ; t++)
	{	
		unsigned int texture;
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0 + t);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		int width, height;

	    unsigned char *imgData = NULL;
	    int bytes_per_pixel = 3;   
	    int color_space = JCS_RGB; 

	
	    struct jpeg_decompress_struct cinfo;
	    struct jpeg_error_mgr jerr;

	    JSAMPROW row_pointer[1];

	    FILE *infile;
	    if(t==0) infile = fopen( textureMap, "rb" );
	    else infile = fopen( heightMap, "rb" );
	    unsigned long location = 0;
	    int i = 0, j = 0;

	    cinfo.err = jpeg_std_error( &jerr );
	    
	    jpeg_create_decompress( &cinfo );
	    
	    jpeg_stdio_src( &cinfo, infile );
	    
	    jpeg_read_header( &cinfo, TRUE );
	    
	    jpeg_start_decompress( &cinfo );

	    
	    imgData = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
	    
	    row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );
	    
	    while( cinfo.output_scanline < cinfo.image_height )
	    {
	        jpeg_read_scanlines( &cinfo, row_pointer, 1 );
	        for( i=0; i<cinfo.image_width*cinfo.num_components;i++)
	            imgData[location++] = row_pointer[0][i];
	    }

	    height = cinfo.image_height;
	    width = cinfo.image_width;

		if (imgData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		jpeg_finish_decompress( &cinfo );
	    jpeg_destroy_decompress( &cinfo );
		free( row_pointer[0] );
	    free( imgData );
	    fclose( infile );

		widthTexture = width;
		heightTexture = height;
	}

}


void BaseApp::BaseCameraSettings()
{
	cameraPosition = baseCameraPosition;
	cameraUp = baseCameraUp;
	cameraGaze = baseCameraGaze;
	cameraRight = baseCameraRight;
}