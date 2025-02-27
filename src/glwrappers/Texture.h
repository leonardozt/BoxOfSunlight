#pragma once

#include "..\utils\Config.h"
#include "..\utils\BoxOfSunlightError.h"

#include <vector>

#include <stb/stb_image.h>

namespace BOSL
{
	// Class that wraps an OpenGL 2D Texture Object
	class Texture
	{
	public:
		// Parameter SRGB will be ignored when loading image file
		// for images with a number of channels different from 3.
		Texture(std::string imgFilePath, GLint internalFormat = GL_RGB);

		// Loads image data into OpenGL context.
		void load() const;

		void setImgFilePath(std::string path);
		void setInternalFormat(GLint internalFormat);

		const std::string& getImgFilePath();

		static bool isSupported(GLint internalFormat);

		// Delete the copy constructor/assignment
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// move constructor
		Texture(Texture&& other) noexcept;
		// move assignment
		Texture& operator=(Texture&& other) noexcept;
		
		~Texture();
	private:
		// OpenGL texture object
		GLuint object;
		
		// internal format of image
		GLuint internalFormat;
		// supported internal formats for images
		static const std::vector<GLint> supportedFormats;

		std::string imgFilePath;

		void release();
	};
}