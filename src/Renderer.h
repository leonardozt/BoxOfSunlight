#pragma once

#include "BoxOfSunlightError.h"
#include "Scene.h"
#include "Program.h"
#include "ScreenQuad.h"

#include <vector>

// for testing
#include <glm/gtc/matrix_transform.hpp>

namespace BOSL
{
	class Renderer
	{
	public:
		Renderer(Scene scene);

		// Renders a single frame using Ray Tracing
		void render();
	
	private:
		Scene scene;

		// contains the output image
		GLuint outputTex;
		GLuint outputTexImgUnit;
		// Shader program used for ray tracing
		Program rtShader;
		// Shader program used to draw screen quad
		Program quadShader;

		GLuint cubemapImgUnit;

		ScreenQuad quad;

		// Initializes and links shader programs
		void initShaders();
		// Creates and adds the output texture (where the path
		// tracers saves the output image) to the OpenGL context
		void initOutputTexture();
		// Checks system's limitations for Compute Shader Workgroups
		bool checkComputeLimits();

		// Updates uniform variables related to camera.
		// The corresponding shader program needs to be active.
		void updateCameraUniforms();

		// for testing
		float cameraDegree = 0.0f;
	};
}