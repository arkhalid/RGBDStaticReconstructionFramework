#ifndef MESH_H
#define MESH_H

// Include GLM
#include <glm\\glm.hpp>

#include <vector>

using namespace glm;
using namespace std;

struct Material
{
	float Ks;
	float Ka;
	float Kd;
	vec3 color;
	char map_Kd[256];
	char tag[256];
	bool texture;
};


struct Group
{
	char tag[256];
	Material mat;
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> uvs;
	vector<unsigned int> indices;
};

struct UnGroupedMesh
{
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> uvs;
	vector<unsigned int> indices;
	vector<unsigned int> FaceType;
	int numTri;
};

struct Mesh
{
	vector<Group> groups;
	int numGroups;
	int numMaterials;
	vector<Material> materials;
	UnGroupedMesh unGroupedMesh;
};

struct PlyMesh
{
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec3> colors;
	vector<unsigned int> indices;
};

#endif

