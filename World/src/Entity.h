#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/dual_quaternion.hpp>

class Entity {
public:
	glm::vec3 position = glm::vec3(0.0f,0.0f,0);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 direction = glm::normalize(glm::vec3() - glm::vec3(0.0f, 0, 1.0f));
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction));
	glm::vec3 up = glm::cross(direction, right);

	inline virtual void Rotate(glm::vec3 rotation) {
		direction.x = sin(glm::radians(-rotation.x)) * cos(glm::radians(rotation.y));
		direction.y = -sin(glm::radians(rotation.y));
		direction.z = cos(glm::radians(-rotation.x)) * cos(glm::radians(rotation.y));

		direction = glm::normalize(direction);
		right = glm::normalize(glm::cross(direction, up));
		this->rotation = rotation;
		UpdateMat4();
	}
	inline virtual void MoveBy(glm::vec3 position) {
		this->position += position;
		UpdateMat4();
	}
	inline virtual void MoveTo(glm::vec3 position) {
		this->position = position;
		UpdateCollider(glm::vec3(0.0f));
		UpdateMat4();
	}
	inline virtual void UpdateMat4() {}
	inline virtual void UpdateCollider(glm::vec3 velocity) {}

};