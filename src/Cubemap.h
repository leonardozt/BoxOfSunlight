#pragma once

#include "Config.h"
#include "BoxOfSunlightError.h"

#include <vector>
#include <string>

namespace BOSL
{
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

		void load() const;

	private:
		GLuint textureObj;

		void release();
	};
}