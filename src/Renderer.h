#pragma once

#include "Debug.h"
#include "BoxOfSunlightError.h"
#include "ScreenQuad.h"
#include "Program.h"
#include "Camera.h"
#include "Cubemap.h"

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

		// contains the output image
		GLuint outputTex;
		GLuint outputTexImgUnit;
		// Shader program used for path tracing
		Program ptShader;
		// Shader program used to draw screen quad
		Program quadShader;

		Camera camera;

		Cubemap cubemap;
		GLuint cubemapImgUnit;

		ScreenQuad quad;

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