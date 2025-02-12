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
		~Renderer();

		// Delete the copy constructor/assignment
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		// move constructor
		Renderer(Renderer&& other) noexcept;
		// move assignment
		Renderer& operator=(Renderer&& other) noexcept;

		// Renders a single frame using Ray Tracing
		void render();
	
	private:
		Scene scene;

		// Shader program used for ray tracing
		Program rtShader;
		
		// Shader program used to draw screen quad
		Program quadShader;
		ScreenQuad quad;
		// Contains the output image
		GLuint outputTex;
		
		static const GLuint cubemapImgUnit = GL_TEXTURE0;
		static const GLuint outputTexImgUnit = GL_TEXTURE1;

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

		void release();
	};
}