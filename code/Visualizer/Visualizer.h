#ifndef VISUALIZER
#define VISUALIZER

// Include GLEW
#include <glew.h>

// Include GLFW
#include "glfw3.h"

// Include GLM
#include <glm\\glm.hpp>
#include "glm\\gtc\\matrix_transform.hpp"

// Include Mesh
#include "Mesh.h"

// Include Control
#include "Controls.h"

// Include SceneMesh
#include "SceneMesh.h"

// Include Mesh Loaders
#include "ObjLoader.h"
#include "PlyLoader.h"

using namespace glm;



class Visualizer
{
	public:
		Visualizer();
		~Visualizer();

		void setDataPath(const char* path) { dataPath = path; }
		void setShaderPath(const char* path){ shaderPath = path; }

		bool initializeGL(const int width, const int height);
	

		void destroyGL();

		void initialize();

		void visualize();

		void loadMesh(const MeshType& meshType, char* prefix);
		
		friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	private:
		int winWidth;
		int winHeight;

		mat4 prespectiveMatrix;
		mat4 viewMatrix;
		mat4 modelMatrix;
		mat4 mvpMatrix;

		SceneMesh* sceneMesh;

		GLuint mvpMatrixId;
		GLuint samplerId;
		GLuint mvpMatrixPlyId;

		GLuint objProgramId;
		GLuint plyProgramId;

		

		const char* dataPath;
		const char* shaderPath;

		
		vec3 meshCenter;

		vec3 meshSize;

		
		void render();

		void setPrespectiveMatrix();
		void setViewMatrix();
		void setModelMatrix();

		vec3 findStartPos();


};

#endif