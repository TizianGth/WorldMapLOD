#pragma once

#include <vector>
#include <string>
#include "stb_image/stb_image.h"

#include "Node.h"
#include "Shader.h"

class Map
{
public:
	Map();
	~Map();

	void Init(unsigned int& depth, std::string heightmap, std::string mask, std::string tempmap, glm::vec2 min, glm::vec2 max, glm::vec3& targetPos);
	void UpdateThread(unsigned int& depth, glm::vec3& targetPos, glm::vec2 offset);
	void Update();
	void Render();

public:
	unsigned char* heightmapBuffer = nullptr;
	unsigned char* maskBuffer = nullptr;

	bool verticesUpdated = true;

private:
	unsigned int vao, vbo, ibo, maskTexture;
	unsigned int iboCount = 0;

	Mesh mesh;

	Texture heightMap;
	Texture maskMap;
	Texture tempMap;
	Texture waterNormalBig;
	Texture waterNormalSmall;
	Texture waterColorMask;

	Node start = Node(heightMap);

	std::vector<float> vertices {0,0,0,0,0,1,1,0,0};
	std::thread sideThread;
	std::mutex mutex;
	glm::vec2 offset;

};

