// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include "Visualizer.h"
#include "ShaderUtils.h"
#include "TextureLoader.h"

#define StartPointOffset 2.5f
#define WalkThroughSpeedScale 1.4f

//#define VSYNC

Controls controls;
GLFWwindow* window;

Visualizer::Visualizer()
{
	sceneMesh = NULL;
}

Visualizer::~Visualizer()
{
}

bool Visualizer::initializeGL(const int width, const int height)
{
	winWidth = width;
	winHeight = height;

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return false;
	}

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "Visualizer", NULL, NULL);

	// Set mouse position to the center
	glfwSetCursorPos(window, winWidth / 2, winHeight / 2);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	
	#ifndef VSYNC
		glfwSwapInterval(0);
	#endif

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetScrollCallback(window, scroll_callback);

	char vShader[256], fShader[256];

	sprintf_s(vShader, "%sSimpleShader.vs", shaderPath);
	sprintf_s(fShader, "%sSimpleShader.fs", shaderPath);

	objProgramId = ShaderUtils::loadShaders(vShader, fShader);

	sprintf_s(vShader, "%sPlyShader.vs", shaderPath);
	sprintf_s(fShader, "%sPlyShader.fs", shaderPath);

	plyProgramId = ShaderUtils::loadShaders(vShader, fShader);

	if (objProgramId != 0 && plyProgramId != 0)
		printf("Shaders Compiled\n");
	
	controls.init(width, height);
}

void Visualizer::destroyGL()
{
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void Visualizer::initialize()
{
	mvpMatrixId = glGetUniformLocation(objProgramId, "MVP");
	mvpMatrixPlyId = glGetUniformLocation(plyProgramId, "MVP");
	samplerId = glGetUniformLocation(objProgramId, "textureSampler");

	mvpMatrixId = glGetUniformLocation(objProgramId, "MVP");
	mvpMatrixPlyId = glGetUniformLocation(plyProgramId, "MVP");
	samplerId = glGetUniformLocation(objProgramId, "textureSampler");

	
	sceneMesh->loadBuffers();
	if (sceneMesh->getMeshType() == OBJ)
		sceneMesh->setMVPMatrixId(mvpMatrixId);
	else if (sceneMesh->getMeshType() == PLY)
		sceneMesh->setMVPMatrixId(mvpMatrixPlyId);

	controls.setPosition(findStartPos());
	controls.setMeshSize(meshSize.z);
	controls.setWalkThroughSpeedScale(WalkThroughSpeedScale);
}

void Visualizer::setPrespectiveMatrix()
{
	prespectiveMatrix = glm::perspective(60.0f, (float)winWidth / (float)winHeight, 1.0f, 80000.0f);
}

void Visualizer::setViewMatrix()
{
	viewMatrix = controls.getViewMatrix();
}

void Visualizer::setModelMatrix()
{
	modelMatrix = controls.getModelMatrix();
}

void Visualizer::visualize()
{
	glEnable(GL_DEPTH_TEST);
	do
	{
		render();
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	glDisable(GL_DEPTH_TEST);

	delete sceneMesh;

	destroyGL();
}

void Visualizer::loadMesh(const MeshType & meshType, char * prefix)
{
	sceneMesh = new SceneMesh();

	sceneMesh->initialize(meshType);
	char fileName[256];
	if (meshType == OBJ)
	{
		if (prefix != NULL)
			sprintf(fileName, "%s.obj", prefix);
		
		sceneMesh->setDataPath(dataPath);
		bool objLoadResult = ObjLoader::loadOBJ(dataPath, fileName, sceneMesh->getMesh());

		if (objLoadResult)
			printf("Mesh %s loaded\n", fileName);
		else
			printf("Mesh %s not loaded\n", fileName);
	}
	else if (meshType == PLY)
	{
		if (prefix != NULL)
			sprintf(fileName, "%s.ply", prefix);

		sceneMesh->setDataPath(dataPath);
		bool plyLoadResult = PlyLoader::loadPly(dataPath, fileName, sceneMesh->getPlyMesh());

		if (plyLoadResult)
			printf("Mesh %s loaded\n", fileName);
		else
			printf("Mesh %s not loaded\n", fileName);
	}
}



void Visualizer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	controls.computeMatricesFromInputs(window);

	setPrespectiveMatrix();
	setViewMatrix();
	setModelMatrix();

	mvpMatrix = prespectiveMatrix * viewMatrix * modelMatrix;

	
	glPushMatrix();
	// Use our shader
	if (sceneMesh->getMeshType() == OBJ)
	{
		glActiveTexture(GL_TEXTURE0);
		// Use our shader
		glUseProgram(objProgramId);
		glUniform1i(samplerId, 0);
	}
	else if (sceneMesh->getMeshType() == PLY)
	{
		// Use our shader
		glUseProgram(plyProgramId);
	}
	sceneMesh->render(mvpMatrix);

	glPopMatrix();

}



vec3 Visualizer::findStartPos()
{

	vec3 startPos = vec3(0.0f, 0.0f, 0.0f);

	vec3 max = -vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 min = -max;

	if (sceneMesh->getMeshType() == OBJ)
	{
		Mesh* mesh = sceneMesh->getMesh();
		const int numGroups = mesh->numGroups;
		for (int i = 0; i < numGroups; i++)
		{
			const Group g = mesh->groups[i];

			const int numVertex = g.vertices.size();
			for (int j = 0; j < numVertex; j++)
			{
				if (max.x < g.vertices[j].x)
					max.x = g.vertices[j].x;
				if (max.y < g.vertices[j].y)
					max.y = g.vertices[j].y;
				if (max.z < g.vertices[j].z)
					max.z = g.vertices[j].z;
				if (min.x > g.vertices[j].x)
					min.x = g.vertices[j].x;
				if (min.y > g.vertices[j].y)
					min.y = g.vertices[j].y;
				if (min.z > g.vertices[j].z)
					min.z = g.vertices[j].z;
			}
		}
	}
	else if (sceneMesh->getMeshType() == PLY)
	{
		PlyMesh* mesh = sceneMesh->getPlyMesh();
		const int numVertex = mesh->vertices.size();
		for (int j = 0; j < numVertex; j++)
		{
			if (max.x < mesh->vertices[j].x)
				max.x = mesh->vertices[j].x;
			if (max.y < mesh->vertices[j].y)
				max.y = mesh->vertices[j].y;
			if (max.z < mesh->vertices[j].z)
				max.z = mesh->vertices[j].z;
			if (min.x > mesh->vertices[j].x)
				min.x = mesh->vertices[j].x;
			if (min.y > mesh->vertices[j].y)
				min.y = mesh->vertices[j].y;
			if (min.z > mesh->vertices[j].z)
				min.z = mesh->vertices[j].z;
		}
	}
	vec3 center = (max + min) / 2.0f;

	startPos = center;
	startPos.z = max.z * StartPointOffset;
	meshSize = max - min;
	meshCenter = center;
	printf("MeshCenter Pos is %f , %f , %f\n",meshCenter.x , meshCenter.y ,meshCenter.z);
	printf("Start Pos is %f , %f , %f\n",startPos.x , startPos.y ,startPos.z);
	return startPos;
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	controls.scroll_callback(window,yoffset);
}

