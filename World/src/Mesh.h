#pragma once
#include <vector>
class Mesh {
public:
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	void AddVertices(std::vector<float> vertices) {
		this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());
	}

	void AddIndices(std::vector<unsigned int> indices) {
		this->indices.insert(this->indices.end(), indices.begin(), indices.end());
	}

	void AddMeshData(Mesh& mesh) {
		AddVertices(mesh.vertices);
		AddIndices(mesh.indices);
	}
};