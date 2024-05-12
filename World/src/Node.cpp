#include "Node.h"

#include <bitset>

const std::vector<float> LodParams::distances = {
	std::numeric_limits<float>::infinity(),
		70000.0f,
		50000.0f,
		30000.0f,
		17900.0f,
		10800.0f,
		3700.4f,
		2470.9f,
		1384.24f,
		390.24f,
		100.24f,
		32.24f,
		4.7f,
		1.1f,
		0.3f,
};

Box::Box()
{
}
Box::Box(glm::vec3 min, glm::vec3 max)
{
	this->min = min;
	this->max = max;
}


glm::vec3 Box::GetCenter()
{
	return glm::vec3((max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f);
}

Node::~Node()
{
	Delete();
}

void Node::Delete()
{
	for (int i = 0; i < 4; i++) {
		if (children[i] != nullptr) {
			if (!children[i]->isLeaf) {
				children[i]->Delete();
			}
			delete children[i];
			children[i] = nullptr;
		}

	}
	isLeaf = true;
}

Node::Node(Texture& heightmap) : heightmap(heightmap)
{
	this->start = this;
	for (int i = 0; i < 4; i++) {
		children[i] = nullptr;
	}
}

Node::Node(Texture& heightmap, unsigned int lod, Node* parent, Node* start, glm::vec2 min, glm::vec2 max, bool isLeaf, unsigned int roomNumber, unsigned char child) : heightmap(heightmap)
{
	this->lod = lod;
	this->parent = parent;
	this->start = start;
	this->isLeaf = isLeaf;
	this->min = min;
	this->max = max;

	this->child = child;
	this->roomHash = roomNumber * 4 + child;

	for (int i = 0; i < 4; i++) {
		children[i] = nullptr;
	}

	
}

void Node::GenerateChildren(unsigned int& depth, Box target)
{
	if (lod >= depth) {
		Delete();
		return;
	}

	auto targetCenter = target.GetCenter();
	if (lod == 0 ){
		glm::vec3 boundsCeneter((min.x + max.x) / 2, 0, (min.y + max.y) / 2);
		boundsCeneter.y = heightmap.GetChannelValueAtResized((min.x + max.x) / 2, (min.y + max.y) / 2, 0) / heightDivisor;
		float distance = GetDistance2(glm::vec3(targetCenter.x, targetCenter.y, targetCenter.z), boundsCeneter);
		if (distance > LodParams::distances[1]) {
			Delete();
			return;
		}
	}


	isLeaf = false;
	cornersGenerated = false;

	glm::vec2 center((min.x + max.x) / 2, (min.y + max.y) / 2);

	// Top left
	if (!children[0]) children[0] = new Node(heightmap, lod + 1, this, start, glm::vec2(min.x, center.y), glm::vec2(center.x, max.y), true, roomHash, 0);
	// Top right
	if (!children[1]) children[1] = new Node(heightmap, lod + 1, this, start, glm::vec2(center.x, center.y), glm::vec2(max.x, max.y), true, roomHash, 1);
	// Botoom right
	if (!children[2]) children[2] = new Node(heightmap, lod + 1, this, start, glm::vec2(center.x, min.y), glm::vec2(max.x, center.y), true, roomHash, 2);
	// Bottom left
	if (!children[3]) children[3] = new Node(heightmap, lod + 1, this, start, glm::vec2(min.x, min.y), glm::vec2(center.x, center.y), true, roomHash, 3);


	for (Node* node : children) {
		if (!node)
			continue;

		glm::vec3 boundsCeneter((node->min.x + node->max.x) / 2, 0, (node->min.y + node->max.y) / 2);
		boundsCeneter.y = heightmap.GetChannelValueAtResized((min.x + max.x) / 2, (min.y + max.y) / 2, 0) / heightDivisor;
		float distance = GetDistance2(glm::vec3(targetCenter.x, targetCenter.y, targetCenter.z), boundsCeneter);
		if (node->lod < LodParams::distances.size() && distance <= LodParams::distances[node->lod]) {
			node->GenerateChildren(depth, target);
		}
		else if (!node->isLeaf) {
			node->Delete();
		}
	}
}

unsigned int Node::GetChildrenCount()
{
	unsigned int count = 0;
	for (Node* node : children) {
		if (node != nullptr) {
			if (!node->isLeaf) {
				count += node->GetChildrenCount();
			}
			else {
				count++;
			}
		}
	}
	return count;
}


float Node::GetDistance2(glm::vec3 a, glm::vec3 b)
{
	return (std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2) + std::pow(b.z - a.z, 2));
}

void Node::GenerateCorners()
{
	corners[0] = heightmap.GetChannelValueAtResized(min.x, min.y, 0) / heightDivisor;
	corners[2] = heightmap.GetChannelValueAtResized(min.x, max.y, 0) / heightDivisor;
	corners[4] = heightmap.GetChannelValueAtResized(max.x, max.y, 0) / heightDivisor;
	corners[6] = heightmap.GetChannelValueAtResized(max.x, min.y, 0) / heightDivisor;

	corners[8] = heightmap.GetChannelValueAtResized((min.x + max.x) / 2, (min.y + max.y) / 2, 0) / heightDivisor;

	// get averages of the points in between 
	corners[1] = (corners[0] + corners[2]) / 2;
	corners[3] = (corners[2] + corners[4]) / 2;
	corners[5] = (corners[4] + corners[6]) / 2;
	corners[7] = (corners[0] + corners[6]) / 2;

	cornersGenerated = true;
}

void Node::GetVertices(std::vector<float>& vertices)
{

	if (isLeaf) {
		if (parent && !parent->cornersGenerated) {
			parent->GenerateCorners();
		}

		if (!cornersGenerated) {
			GenerateCorners();
		}
		glm::vec4 c(corners[0], corners[6], corners[2], corners[4]);

		bool update = parent ? !UpdateNeighbours(c) : false;
		if (!vertsGenerated || update) {
				verts.clear();
				verts.reserve(3 * 4);

				// Bottom left
				verts.emplace_back(min.x);
				verts.emplace_back(c[0]);
				verts.emplace_back(min.y);


				// Bottom right
				verts.emplace_back(max.x);
				verts.emplace_back(c[1]);
				verts.emplace_back(min.y);

				// Top left
				verts.emplace_back(min.x);
				verts.emplace_back(c[2]);
				verts.emplace_back(max.y);

				// Top right
				verts.emplace_back(max.x);
				verts.emplace_back(c[3]);
				verts.emplace_back(max.y);
			
		}
		vertices.insert(vertices.end(), verts.begin(), verts.end());

	}
	else {
		if(parent)
			vertsGenerated = false;
		for (Node* node : children) {
			if (node) {
				node->GetVertices(vertices);
			}
		}
	}

}

void Node::GetIndices(std::vector<unsigned int>& indices, int& indicesCount)
{

	if (isLeaf) {
		indices.reserve(6);
		indices.emplace_back(indicesCount + 1);
		indices.emplace_back(indicesCount + 2);
		indices.emplace_back(indicesCount + 0);

		indices.emplace_back(indicesCount + 1);
		indices.emplace_back(indicesCount + 3);
		indices.emplace_back(indicesCount + 2);

		indicesCount += 4;

	}
	else {
		for (Node* node : children) {
			if (node)
				node->GetIndices(indices, indicesCount);
		}
	}
}

bool Node::UpdateNeighbours(glm::vec4& c)
{
	unsigned char n[4] = { 0,0,0,0 };

	if (child == 0) {
		n[Sides::W] = CheckNeighbourLOD(Sides::W, roomHash);
		n[Sides::N] = CheckNeighbourLOD(Sides::N, roomHash);

		if (n[Sides::W]) {
			c[0] = parent->corners[1];
		}

		if (n[Sides::N]) {
			c[3] = parent->corners[3];
		}

		c[1] = parent->corners[8];
		c[2] = parent->corners[2];
	}
	else if (child == 1) {
		n[Sides::N] = CheckNeighbourLOD(Sides::N, roomHash);
		n[Sides::E] = CheckNeighbourLOD(Sides::E, roomHash);

		if (n[Sides::N]) {
			c[2] = parent->corners[3];
		}

		if (n[Sides::E]) {
			c[1] = parent->corners[5];
		}

		c[0] = parent->corners[8];
		c[3] = parent->corners[4];
	}
	else if (child == 2) {
		n[Sides::E] = CheckNeighbourLOD(Sides::E, roomHash);
		n[Sides::S] = CheckNeighbourLOD(Sides::S, roomHash);

		if (n[Sides::E]) {
			c[3] = parent->corners[5];
		}

		if (n[Sides::S]) {
			c[0] = parent->corners[7];
		}

		c[1] = parent->corners[6];
		c[2] = parent->corners[8];
	}
	else {
		n[Sides::W] = CheckNeighbourLOD(Sides::W, roomHash);
		n[Sides::S] = CheckNeighbourLOD(Sides::S, roomHash);

		if (n[Sides::W]) {
			c[2] = parent->corners[1];
		}

		if (n[Sides::S]) {
			c[1] = parent->corners[7];
		}

		c[0] = parent->corners[0];
		c[3] = parent->corners[8];
	}
	
	bool res = true;
	for (int i = 0; i < 4; i++) {
		if (n[i] != neighbours[i]) {
			res = false;
			neighbours[i] = n[i];
		}
	}

	return res;
}


uint Node::GetNeighbourDetailLevel(uint queryHash, int dl)
{
	int dlResult = 0; // dl = detail level

	if (roomHash == queryHash)
	{
		dlResult = lod;
	}
	else
	{
		//uint h = queryHash >> (30 - (dl * 2));
		//h &= 3;
		uint h = (queryHash >> ((dl - 1) * 2)) & 3;
		Node* node = children[h];
		if (node)
		{
			dlResult += node->GetNeighbourDetailLevel(queryHash, dl - 1);
		}
	}

	return dlResult; // Returns 0 if no quad with the given hash is found
}

uint Node::CheckNeighbourLOD(uint side, uint hash)
{
	uint bitmask = 0;
	uint count = 0;
	uint twoLast;

	while (count < lod * 2) // 0 through 3 can be represented as a two bit number
	{
		count += 2;
		twoLast = (hash & 3); // Get the two last bits of the hash. 0b_10011 --> 0b_11

		bitmask = bitmask * 4; // Add zeroes to the end of the bitmask. 0b_10011 --> 0b_1001100

		// Create mask to get the quad on the opposite side. 2 = 0b_10 and generates the mask 0b_11 which flips it to 1 = 0b_01
		if (side == 2 || side == 3)
		{
			bitmask += 3; // Add 0b_11 to the bitmask
		}
		else
		{
			bitmask += 1; // Add 0b_01 to the bitmask
		}

		// Break if the hash goes in the opposite direction
		if ((side == 0 && (twoLast == 0 || twoLast == 3)) ||
			(side == 1 && (twoLast == 1 || twoLast == 2)) ||
			(side == 2 && (twoLast == 3 || twoLast == 2)) ||
			(side == 3 && (twoLast == 0 || twoLast == 1)))
		{
			break;
		}

		hash = hash >> 2;
	}
	if (start->GetNeighbourDetailLevel(roomHash ^ bitmask, lod) < lod) 
		return 1;
	return 0;
}



