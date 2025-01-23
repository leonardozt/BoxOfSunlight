#pragma once

#include "Debug.h"
#include "BoxOfSunlightError.h"
#include "ScreenQuad.h"
#include "Program.h"

#include <vector>

namespace BOSL
{
	class Renderer
	{
	public:
		Renderer();
		void init();
		// Renders a single frame using Path Tracing
		void render();
	private:
		bool initialized;

		ScreenQuad quad;
		GLuint outputTexture;
		Program pathTracerShader;
		Program screenQuadShader;

		// Initializes GLEW and sets the needed OpenGL settings
		static void initGL();
		// Initializes and links shader programs
		void initShaders();
		// Creates and adds the output texture (where the path
		// tracers saves the output image) to the OpenGL context
		void initOutputTexture();
		// Checks system's limitations for Compute Shader Workgroups
		bool checkComputeLimits();
	};
}