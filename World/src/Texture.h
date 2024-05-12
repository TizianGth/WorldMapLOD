#pragma once

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>

#include <string>
#include <algorithm>

#include <glm/vec3.hpp>

#include "stb_image/stb_image.h"

class Texture
{
public:
	Texture();
	~Texture();

	void Load(const std::string& path, int channels);
	void GlGenerate();
	void GlBind(unsigned int slot);
	unsigned char GetChannelValueAt(float x, float y, int channel);
	unsigned char GetChannelValueAtResized(float x, float y, int channel);

	glm::vec3 GetColorAt(float x, float y);
	glm::vec3 GetColorAtResized(float x, float y);

private:
	int width, height, channels;

	unsigned char* image = nullptr; 
	unsigned int glId;
};

