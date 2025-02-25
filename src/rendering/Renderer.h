#pragma once

#include "..\utils\BoxOfSunlightError.h"
#include "..\scene\Scene.h"
#include "..\glwrappers\Program.h"
#include "ScreenQuad.h"

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
		// Scene to be rendered
		Scene scene;

		// Shader program used for ray tracing
		Program rtShader;
		// Shader program used to draw screen quad
		Program quadShader;

		// Quad that will be rendered on screen in final drawing pass
		ScreenQuad quad;
		// Texture that will be mapped on quad
		GLuint outputTex;

		// Texture Image Units
		struct TexImgUnits {
			static const int cubemap = 0;
			static const int outputTex = 1;
			static const int albedoMap = 2;
			static const int normalMap = 3;
		};

		// Prepares shader programs for rendering
		void initShaders();

		// Creates and adds the output texture (where the path
		// tracers saves the output image) to the OpenGL context
		void initOutputTexture();

		// Checks system's limitations for Compute Shader Workgroups
		bool checkComputeLimits();

		// Updates uniform variables related to camera.
		// The corresponding shader program needs to be active.
		void updateCameraUniforms();

		void release();
	};
}