#include <stdlib.h>

#include "Visualizer.h"

int main()
{
	
	char dataPath[256], shaderPath[256];
	sprintf_s(dataPath, "..\\..\\Models\\");
	sprintf_s(shaderPath, "..\\..\\Shaders\\");

	Visualizer visualizer;
	visualizer.setDataPath(dataPath);
	visualizer.setShaderPath(shaderPath);
	visualizer.initializeGL(1920, 1080);
	visualizer.loadMesh(OBJ,"nurse");
	//visualizer.loadMesh(PLY,"doctor");
	visualizer.initialize();
	visualizer.visualize();
	
}