#include "Texture.h"

namespace BOSL
{
	Texture::Texture()
	{
		glGenTextures(1, &object);
	}

	void Texture::load(const std::string& filePath, bool SRGB) const
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
		if (!data)
		{
			throw BoxOfSunlightError("Texture load failure at path " + filePath);
		}

		GLint internalFormat;
		SRGB ? internalFormat = GL_SRGB : internalFormat = GL_RGB;
		GLenum format = GL_RGB;
	
		glBindTexture(GL_TEXTURE_2D, object);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	Texture::Texture(Texture&& other) noexcept
		: object(other.object)
	{
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