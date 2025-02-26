#include "Texture.h"

namespace BOSL
{
	Texture::Texture(std::string imgFilePath, bool SRGB)
		: imgFilePath(imgFilePath)
		, SRGB(SRGB)
	{
		glGenTextures(1, &object);
	}

	void Texture::load() const
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(imgFilePath.c_str(), &width, &height, &nrChannels, 0);
		if (!data)
		{
			throw BoxOfSunlightError("Texture load failure at path " + imgFilePath);
		}

		// default RGB
		GLint internalFormat = GL_RGB;
		GLenum format = internalFormat;
		
		if (nrChannels == 1) {
			internalFormat = GL_RED;
			format = internalFormat;
		}
		else if (nrChannels == 3) {
			SRGB ? internalFormat = GL_SRGB : internalFormat = GL_RGB;
			format = GL_RGB;
		} else if (nrChannels == 4) {
			internalFormat = GL_RGBA;
			format = internalFormat;
		} else{
			throw BoxOfSunlightError("Texture::load(const std::string& filePath, bool SRGB) -"
				" Image " + imgFilePath +
				" uses a format that isn't recognised.");
		}
	
		glBindTexture(GL_TEXTURE_2D, object);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	void Texture::setImgFilePath(std::string path) {
		imgFilePath = path;
	}

	void Texture::setSRGB(bool SRGB) {
		this->SRGB = SRGB;
	}

	const std::string& Texture::getImgFilePath()
	{
		return imgFilePath;
	}

	Texture::Texture(Texture&& other) noexcept
		: object(other.object)
		, imgFilePath(other.imgFilePath)
		, SRGB(other.SRGB)
	{
		other.imgFilePath = "";
		other.object = 0;
	}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		// check for self-assignment
		if (this != &other)
		{
			release();
			// object is now 0
			std::swap(object, other.object);

			imgFilePath = other.imgFilePath;
			SRGB = other.SRGB;

			other.imgFilePath = "";
		}
		return *this;
	}

	Texture::~Texture()
	{
		release();
	}

	void Texture::release()
	{
		glDeleteTextures(1, &object);
		object = 0;
	}
}