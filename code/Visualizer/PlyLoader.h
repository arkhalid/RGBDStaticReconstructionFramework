#ifndef PLYLOADER_H
#define PLYLOADER_H

#include <vector>
#include <glm/glm.hpp>

#include "Mesh.h"

class PlyLoader
{
public:
	static bool loadPly(const char* dataPath, const char * plyFileName, PlyMesh* mesh);

};
#endif