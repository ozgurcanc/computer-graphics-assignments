#include "defs.h"
#include "Scene.h"
#include <iostream>

Scene *pScene; // definition of the global scene variable (declared in defs.h)


int main(int argc, char *argv[])
{
	if (argc != 2)
    {
        std::cout << "Please run the ray tracer as:" << std::endl
             << "\t./raytracer inputs/<input_file_name>" << std::endl;
        return 1;
    }
    else
    {
    	const char *xmlPath = argv[1];

    	pScene = new Scene(xmlPath);

    	pScene->renderScene();	
	}
	
	return 0;
}
