#pragma once

#include "Config.h"
#include "Program.h"

namespace BOSL
{
	// Quad that fills the entire screen,
	// in Normalized Device Coordinates
	class ScreenQuad
	{
	public:
		ScreenQuad();
		~ScreenQuad();

		// Delete the copy constructor/assignment
		ScreenQuad(const ScreenQuad&) = delete;
		ScreenQuad& operator=(const ScreenQuad&) = delete;

		// move constructor
		ScreenQuad(ScreenQuad&& other) noexcept;
		// move assignment
		ScreenQuad& operator=(ScreenQuad&& other) noexcept;

		void init();
		void draw(Program& shaderProgram) const;
	private:
		bool initialized;

		GLuint VAO;
		GLuint VBO;

		void release();
	};
}