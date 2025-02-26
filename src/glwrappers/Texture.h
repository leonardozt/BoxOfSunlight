#pragma once

#include "..\utils\Config.h"
#include "..\utils\BoxOfSunlightError.h"

#include <stb/stb_image.h>

namespace BOSL
{
	// Class that wraps an OpenGL 2D Texture Object
	class Texture
	{
	public:
		// Parameter SRGB will be ignored when loading image file
		// for images with a number of channels different from 3.
		Texture(std::string imgFilePath, bool SRGB = false);

		// Loads image data into OpenGL context.
		void load() const;

		void setImgFilePath(std::string path);
		void setSRGB(bool SRGB);

		const std::string& getImgFilePath();

		// Delete the copy constructor/assignment
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// move constructor
		Texture(Texture&& other) noexcept;
		// move assignment
		Texture& operator=(Texture&& other) noexcept;
		
		~Texture();
	private:
		GLuint object;

		std::string imgFilePath;
		bool SRGB;

		void release();
	};
}