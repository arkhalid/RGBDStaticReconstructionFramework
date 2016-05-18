#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <glm/glm.hpp>

#include "Mesh.h"

class ObjLoader
{
	public:
		static bool loadOBJ(const char* dataPath, const char * objFileName, Mesh* mesh);
		static bool loadWeightedOBJ(const char* dataPath, const char * objFileName, UnGroupedMesh& mesh, bool rgb = false);
};
#endif