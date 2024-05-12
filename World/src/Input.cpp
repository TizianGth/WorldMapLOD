#include "Input.h"


bool Input::keyDown[65536];

bool Input::GetKeyPressed(GLFWwindow* window, int keycode)
{
	auto state = glfwGetKey(window, keycode);
	Input::keyDown[keycode] = state == GLFW_PRESS || state == GLFW_REPEAT;
	return Input::keyDown[keycode];
}

bool Input::GetKeyDown(GLFWwindow* window, int keycode)
{
	auto state = glfwGetKey(window, keycode);

	if (state != GLFW_PRESS) {
		Input::keyDown[keycode] = false;
		return false;
	}

	if (Input::keyDown[keycode]) {
		return false;
	}

	Input::keyDown[keycode] = true;

	return true;
}

bool Input::GetKeyUp(GLFWwindow* window, int keycode)
{
	return false;
}


std::pair<double, double> Input::GetMousePosition(GLFWwindow* window)
{
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

	return std::pair<double, double> {xPos, yPos};
}
