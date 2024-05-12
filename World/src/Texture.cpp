#include "Texture.h"
#include <iostream>

Texture::Texture()
{
}

Texture::~Texture()
{
	stbi_image_free(image);
}

void Texture::Load(const std::string& path, int channels)
{
	if (image != nullptr)
		stbi_image_free(image);

	unsigned char* temp = stbi_load(path.c_str(), &width, &height, &this->channels, channels);

	if (temp == nullptr) {
		std::cout << "Failed to load texture: \"" << path.c_str() << "\"" << std::endl;
	}

	if (channels < this->channels) this->channels = channels;


	unsigned int length = width * height * this->channels;
	image = new unsigned char[length];
	std::copy(temp, temp + length, image);
	stbi_image_free(temp);

}

void Texture::GlGenerate()
{
	glGenTextures(1, &glId);
	glBindTexture(GL_TEXTURE_2D, glId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	int channel = GL_RED;
	if (channels == 3) {
		channel = GL_RGB;
	}
	else if (channels == 4) {
		channel = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, channel, width, height, 0, channel, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::GlBind(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	GlGenerate();
	glBindTexture(GL_TEXTURE_2D, glId);
}

unsigned char Texture::GetChannelValueAt(float x, float y, int channel)
{
	x = std::clamp(x, 0.0f, width-1.0f);
	y = std::clamp(y, 0.0f, height-1.0f);
	int pixel = (int)y * width + (int)x;
	int index = pixel * channels + channel;
	return image[index];
}

unsigned char Texture::GetChannelValueAtResized(float x, float y, int channel)
{
	x *= width / 200.0f;
	y *= width / 200.0f;
	return GetChannelValueAt(x, y, channel);
}

glm::vec3 Texture::GetColorAt(float x, float y)
{
	x = std::clamp(x, 0.0f, width - 1.0f);
	y = std::clamp(y, 0.0f, height - 1.0f);
	int pixel = (int)y * width + (int)x;
	int index = pixel * channels;

	return glm::vec3(image[index], image[index + 1], image[index + 2]);
}

glm::vec3 Texture::GetColorAtResized(float x, float y)
{
	x *= width / 200.0f;
	y *= width / 200.0f;
	return GetColorAt(x, y);
}
