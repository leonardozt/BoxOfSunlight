#pragma once

#include "..\utils\Config.h"
#include "..\utils\BoxOfSunlightError.h"

#include <vector>

namespace BOSL
{
	// Class that wraps an OpenGL Cubemap Texture Object
	class Cubemap
	{
	public:
		Cubemap();
		~Cubemap();

		// Delete the copy constructor/assignment
		Cubemap(const Cubemap&) = delete;
		Cubemap& operator=(const Cubemap&) = delete;

		// move constructor
		Cubemap(Cubemap&& other) noexcept;
		// move assignment
		Cubemap& operator=(Cubemap&& other) noexcept;

		// Loads image data into OpenGL
		// (assumes SRGB format)
		void load() const;
	private:
		GLuint textureObj;

		void release();
	};
}