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
		Texture();

		// Loads image data into OpenGL context
		void load(const std::string& fileName, bool SRGB) const;

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

		void release();
	};
}