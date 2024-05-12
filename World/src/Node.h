#pragma once

#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <mutex>

#include <glm/mat4x4.hpp>

#include "Mesh.h"
#include "Texture.h"

typedef unsigned int uint;

enum Sides { E = 0, W = 1, N = 2, S = 3 };

class Box {
public:
	glm::vec3 min;
	glm::vec3 max;

	Box();
	Box(glm::vec3 min, glm::vec3 max);

	glm::vec3 GetCenter();
};

struct LodParams {
public:
	static const std::vector<float> distances;
};

class Node
{

public:
	Node(Texture& heightmap);
	Node(Texture& heightmap, unsigned int lod, Node* parent, Node* start, glm::vec2 min, glm::vec2 max, bool isLeaf, unsigned int roomNumber, unsigned char child);

	~Node();

	void Delete();

	void GenerateChildren(unsigned int& depth, Box target);
	unsigned int GetChildrenCount();
	float GetDistance2(glm::vec3 a, glm::vec3 b);
	void GenerateCorners();

	void GetVertices(std::vector<float>& vertices);
	void GetIndices(std::vector<unsigned int>& indices, int& indicesCount);

	// Returns true if neighbours lod didnt change
	bool UpdateNeighbours(glm::vec4& c);

	uint GetNeighbourDetailLevel(uint queryHash, int dl);

public:
	Node* children[4];
	Node* parent = nullptr;
	Node* start = nullptr;
	bool newData = true;
	unsigned int lod = 0;
	unsigned char child = 0;
	bool isLeaf = false;
	glm::vec2 min;
	glm::vec2 max;

	uint roomHash = 0;
	float corners[9];

private:
	uint CheckNeighbourLOD(uint side, uint hash);
private:
	std::vector<float> verts;
	char neighbours[4] = { -1,-1,-1,-1 };
	float localCorners[4] = { 0,0,0,0 };
	bool vertsGenerated = false;
	bool cornersGenerated = false;
	float heightDivisor = 170.0;
	Texture& heightmap;
};

