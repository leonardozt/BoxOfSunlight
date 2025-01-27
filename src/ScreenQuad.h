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

		void init();
		void draw(const Program& shaderProgram) const;
	private:
		bool initialized;

		GLuint VAO;
		GLuint VBO;
	};
}