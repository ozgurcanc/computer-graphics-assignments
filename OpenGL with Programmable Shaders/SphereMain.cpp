﻿
#include "SphericalApp.h"
#include <iostream>

int main(int argc, char * argv[]) 
{
	
	if (argc != 3)
    {
        std::cout << "Please run the program as:" << std::endl
             << "\t./hw3_sphere inputs/<height_map_file_name> inputs/<texture_map_file_name>" << std::endl;
        return 1;
    }
	
	SphericalApp earth(argv[2], argv[1]);

	earth.Run();

	return 0;
}
