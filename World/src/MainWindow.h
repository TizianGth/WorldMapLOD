#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <math.h>

#include "Shader.h"
#include "Input.h"
#include "Camera.h";
#include "Map.h"

class MainWindow
{

public:
	void Start(int width, int height);

private:
	bool GLInit();
	void Init();

	void Update();
	void Render();

	void OnMouseUpdate();
	void OnKeyboardUpdate();
	void UpdateWater();

	void GuiRender();

private:
	int windowWidth, windowHeight;

	GLFWwindow* window;

	unsigned int vao, vbo;
	bool wireFrame = false;
	std::vector<float> vertices;
	bool shouldClose = false;
	bool vertsUpdated = false;
	Shader* shader = nullptr;
	Camera camera;
	bool mouseLocked = true;
	Map map;
	glm::vec2 mapSize = glm::vec2(200, 100);
	glm::mat4 projCam;


	float totalTime = 0.0f;
	float updateTime = 0.0f;
	double deltaTime = 0.0f;

	unsigned int lod = 11;
	const int maxLod = 13;
	const int minLod = 0;
};

