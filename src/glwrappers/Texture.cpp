#include "Texture.h"

namespace BOSL
{
	const std::vector<GLint> Texture::supportedFormats =
	{
		GL_RED,
		GL_RGB,
		GL_SRGB,
		GL_RGBA,
		GL_RGB16F
	};

	Texture::Texture(std::string imgFilePath, GLint internalFormat)
	{
		if (!isSupported(internalFormat)) {
			throw BoxOfSunlightError("Error when creating Texture object: "
					"the specified internal format is not supported.");
		}
		this->imgFilePath = imgFilePath;
		this->internalFormat = internalFormat;
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

		GLenum format;
		bool error = false;
		int requiredChannels;
		switch (internalFormat)
		{
			case GL_RED:
				format = internalFormat;
				requiredChannels = 1;
				break;
			case GL_RGB:
				format = internalFormat;
				requiredChannels = 3;
				break;
			case GL_SRGB:
				format = GL_RGB;
				requiredChannels = 3;
				break;
			case GL_RGBA:
				format = internalFormat;
				requiredChannels = 4;
			case GL_RGB16F:
				format = GL_RGB;
				requiredChannels = 3;
			default:
				throw BoxOfSunlightError(
					"Texture::load() - received an unknown format "
					" for image " + imgFilePath);
		}
		if (requiredChannels != nrChannels) {
			throw BoxOfSunlightError("Texture::load() - the requestested internalFormat "
				"is incompatible with the number of channels in " + imgFilePath);
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

	void Texture::setInternalFormat(GLint internalFormat) {
		this->internalFormat = internalFormat;
	}

	const std::string& Texture::getImgFilePath() {
		return imgFilePath;
	}

	bool Texture::isSupported(GLint internalFormat)
	{
		bool supported = false;
		for (unsigned int i = 0; i < supportedFormats.size(); i++) {
			if (internalFormat == supportedFormats.at(i)) {
				supported = true;
				break;
			}
		}
		return supported;
	}

	Texture::Texture(Texture&& other) noexcept
		: object(other.object)
		, imgFilePath(other.imgFilePath)
		, internalFormat(other.internalFormat)
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

			imgFilePath = other.imgFilePath;
			internalFormat = other.internalFormat;
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