#pragma once

#include "..\utils\Config.h"

namespace BOSL
{
	// Quad that fills the entire screen
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

		void draw() const;
	private:
		GLuint VAO;
		GLuint VBO;

		void init();

		void release();
	};
}