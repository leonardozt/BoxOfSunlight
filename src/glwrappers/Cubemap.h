#pragma once

#include "..\utils\Config.h"
#include "..\utils\BoxOfSunlightError.h"

#include <vector>

namespace BOSL
{
	// Wrapper class for cubemap textures
	class Cubemap
	{
	public:
		Cubemap(const std::string& dirName);
		~Cubemap();

		// Changes the name of the directory from where
		// the faces are loaded.
		void setDirName(const std::string& name);

		// Delete the copy constructor/assignment
		Cubemap(const Cubemap&) = delete;
		Cubemap& operator=(const Cubemap&) = delete;

		// move constructor
		Cubemap(Cubemap&& other) noexcept;
		// move assignment
		Cubemap& operator=(Cubemap&& other) noexcept;

		// Loads image data into OpenGL		
		void load() const;
	private:
		GLuint textureObj;

		// Name of directory from where
		// the faces are loaded
		std::string dirName;

		void release();
	};
}