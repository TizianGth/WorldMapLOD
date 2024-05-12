#pragma once

#include <GLFW/glfw3.h>
#include <tuple>
#include <chrono>
#include <unordered_map>

class Input
{
public:

	static bool GetKeyPressed(GLFWwindow* window, int keycode);
	static bool GetKeyDown(GLFWwindow* window, int keycode);
	static bool GetKeyUp(GLFWwindow* window, int keycode);
	static std::pair<double, double> GetMousePosition(GLFWwindow* window);
private:
	static bool keyDown[65536];
};

