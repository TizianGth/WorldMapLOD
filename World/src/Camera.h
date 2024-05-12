#pragma once

#include "Entity.h"
#include <iostream>
#include <math.h>
#include <numbers>

class Camera : public Entity {
public:

	// "World" properties
	float rotationSpeed = 0.03f;
	float movementSpeed = 0.009f;
	float movementSpeedMaxMulti = 10;
	float colliderHeight = 2.0f;
	float cameraHeight = 1.5f;

	// "Rendering" properties
	glm::mat4 mat4 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 proj = glm::mat4(1.0f);
	glm::vec2 planeScale;
	glm::vec2 nearPlane;
	glm::vec2 farPlane;
	unsigned int fov = 90.0f;
	unsigned int fovDefault = 90.0f;
	unsigned int fovZoom = 10.0f;
	float nearPlaneDist = 0.03f;
	float farPlaneDist = 10000.0f;
	float aspectRatio = 16.0f / 9.0f;

	Camera() {};

	inline Camera(int width, int height)
	{ 
		SetFov(fovDefault, width, height); 
	};

	inline void SetFov(unsigned int fov, int windowWidth, int windowHeight) {
		this->fov = fov;
		aspectRatio = (float)windowWidth / (float)windowHeight;
		proj = glm::perspective(glm::radians((float)fov), aspectRatio, nearPlaneDist, farPlaneDist);

		float deg = (fov / 2.0f);
		planeScale.y = 1.0f / ( (float)deg != 0 ? std::tan((deg / 180.0f) * std::numbers::pi) : 0.0f);
		planeScale.x = planeScale.y * aspectRatio;

		nearPlane.x = nearPlaneDist * planeScale.x;
		nearPlane.y = nearPlaneDist * planeScale.y;

		farPlane.x = farPlaneDist * planeScale.x;
		farPlane.y = farPlaneDist * planeScale.y;
	};

	inline void UpdateCollider(glm::vec3 velocity) override {

	}

	void Zoom(int windowWidth, int windowHeight) {
		SetFov(fovZoom, windowWidth, windowHeight);
	}
	// moved mat4 here so not every gameobj has a mat4
	inline void UpdateMat4() override {
	mat4 = glm::lookAt(position, position + direction, up);
	}

};
