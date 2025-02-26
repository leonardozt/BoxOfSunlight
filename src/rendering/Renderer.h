#pragma once

#include "..\utils\BoxOfSunlightError.h"
#include "..\utils\Debug.h"
#include "..\scene\Scene.h"
#include "..\glwrappers\Program.h"
#include "ScreenQuad.h"

namespace BOSL
{
	// Initializes GLEW and sets the needed OpenGL settings
	void initGL();

	class Renderer
	{
	public:
		// Before creating a Renderer object, call initGL()
		Renderer(Scene scene);
		~Renderer();

		// Renders a single frame using Ray Tracing
		void render();

		// Delete the copy constructor/assignment
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		// move constructor
		Renderer(Renderer&& other) noexcept;
		// move assignment
		Renderer& operator=(Renderer&& other) noexcept;

	private:
		// Scene to be rendered
		Scene scene;

		// Compute Shader program, based on ray tracing
		Program compShader;
		// Shader program used to draw final quad on screen
		Program quadShader;

		// ---------------- compute shader data ----------------
		// SSBO for triangles
		GLuint trianglesBuf; 
		// SSBO for spheres
		GLuint spheresBuf; 

		// Texture Image Units used by Compute Shader
		enum CompShaderTexImgUnits {
			cubemapImgUnit, // cubemap = 0
			albedoMapImgUnit,
			normalMapImgUnit,
			metallicMapImgUnit,
			roughnessMapImgUnit,
			NUM_COMP_IMG_UNITS,
		};
		// Names of Texture used by Compute Shader
		const std::string CompShaderTexNames[NUM_COMP_IMG_UNITS] =
		{
			"cubemap",
			"albedoMap",
			"normalMap",
			"metallicMap",
			"roughnessMap"
		};
		// -----------------------------------------------------

		// ---------------- output shader data -----------------
		// Quad that will be rendered on screen in final drawing pass
		ScreenQuad quad;
		// Texture that will be mapped on quad (OpenGL object)
		GLuint quadTexObj;
		// Texture Image Unit of quad texture
		static const unsigned int quadTexImgUnit = NUM_COMP_IMG_UNITS; // (next available unit)
		// Name of quad texture in GLSL Shader
		static const std::string quadTexName;
		//----------------------------------------------------------

		// Prepares shader programs for rendering
		void initShaders();

		void initCameraUniforms();

		// Prepares all the textures needed by the Compute Shader
		void initAllCompShaderTextures();
		// Prepares a single 2D texture in the compute shader for rendering.
		// compShader needs to be in use at time of call
		void initCompShader2DTex(const Texture& tex, CompShaderTexImgUnits imgUnit);

		void passDataToSSBO(GLuint buffer, GLuint index, GLsizeiptr size, const void* data);

		// Creates and adds the quad texture (where the compute
		// shader saves the output image) to the OpenGL context
		void initQuadTexture();

		// Checks system's limitations for Compute Shader Workgroups
		bool checkComputeLimits();

		void release();
	};
}