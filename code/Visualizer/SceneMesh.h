#ifndef SCENE_MESH_H
#define SCENE_MESH_H

// Include GLEW
#include <glew.h>

// Include GLM
#include <glm\\glm.hpp>
#include "glm\\gtc\\matrix_transform.hpp"

// Include Mesh
#include "Mesh.h"

enum MeshType
{
	OBJ,
	PLY
};


using namespace glm;
class SceneMesh
{
	public:
		SceneMesh() { }
		~SceneMesh();

		void initialize(const MeshType& mType)
		{
			type = mType;
			if (type == OBJ)
			{
				mesh = new Mesh();
			}
			else if (type == PLY)
			{
				plyMesh = new PlyMesh();
			}
		}


		Mesh* getMesh() { return mesh; }
		PlyMesh* getPlyMesh() { return plyMesh;}

		MeshType getMeshType() { return type; }
		
		void loadBuffers();
		void render(glm::mat4& mvp);
		void setDataPath(const char* path) { dataPath = path; }
		void setMVPMatrixId(const GLuint& mvpMatrixId) { this->mvpMatrixId = mvpMatrixId; }
		void setSamplerId(const GLuint& sId) { samplerId = sId; }
	
	private:

		MeshType type;

		void unloadBuffers();
		void renderMeshGroup(const int index);
		void renderPlyMesh();

		GLuint mvpMatrixId;
		GLuint* vertexBuffers;
		GLuint* normalBuffers;
		GLuint* uvBuffers;
		GLuint* indexBuffers;
		GLuint vertexArrayId;
		GLuint samplerId;
		
		GLuint* textureIds;

		GLuint plyVertexBuffer;
		GLuint plyNormalBuffer;
		GLuint plyColorBuffer;
		GLuint plyIndexBuffer;

		unsigned int numPlyMeshIndices;

		Mesh* mesh;
		PlyMesh* plyMesh;

		const char* dataPath;
		
		vector<int> textureIdList;
		vector<int> numIndicesList;

		int numMeshGroups;
		int numMeshMaterials;
};
#endif