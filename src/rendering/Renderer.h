#pragma once

#include "..\utils\BoxOfSunlightError.h"
#include "..\utils\Config.h"
#include "..\utils\Debug.h"
#include "..\scene\Scene.h"
#include "..\glwrappers\Program.h"
#include "ScreenQuad.h"

#include <array>

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
		// (based on rasterization)
		Program quadShader;

		unsigned int frameNumber;

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
			NUM_COMP_TEX_IMG_UNITS,
		};
		// Names of Texture used by Compute Shader
		static const std::array<std::string, NUM_COMP_TEX_IMG_UNITS> compShaderTexNames;
		
		// Image Units used by Compute Shader.
		// Not to be confused with "Texture Image Units"!
		// Image Units correspond to GLSL image variables,
		// Texture Image Units correspont to GLSL samplers.
		enum CompShaderImgUnits {
			srcImgUnit, // srcImgUnit = 0
			dstImgUnit,
			NUM_COMP_IMG_UNITS
		};
		static const std::array<std::string, NUM_COMP_IMG_UNITS> compShaderImgNames;

		// Indexes of SSBOs used in Compute Shader
		enum CompShaderSSBOIndexes {
			trianglesBufIdx,
			spheresBufIdx,
			rngStateBufIdx
		};
		// -----------------------------------------------------

		// ---------------- output shader data -----------------
		// Quad that will be rendered on screen in final drawing pass
		ScreenQuad quad;
		// Texture that will be mapped on quad (OpenGL object)
		GLuint quadTexObj;
		// Texture Image Unit of quad texture
		static const unsigned int quadTexImgUnit = NUM_COMP_TEX_IMG_UNITS; // (next available unit)
		// Name of quad texture in GLSL Shader
		static const std::string quadTexName;
		//----------------------------------------------------------

		// Prepares all shader programs for rendering
		void initAllShaders();
		// Prepares everything that's needed by the compute shader program
		void initCompShader();
		// Prepares everything that's needed by the quad shader program
		void initQuadShader();

		void initCameraUniforms();

		// Prepares all the textures needed by the Compute Shader
		void initAllCompShaderTextures();
		// Prepares a single 2D texture in the compute shader for rendering.
		// compShader needs to be in use at time of call
		void initCompShader2DTex(const Texture& tex, CompShaderTexImgUnits imgUnit);

		void passDataToSSBO(GLuint buffer, GLuint index,
			GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW);

		// Creates and adds the quad texture to the OpenGL context,
		// connecting it to the output image of the compute shader
		void initQuadTexture();

		// Checks system's limitations for Compute Shader Workgroups,
		// Returns false if limitations are too low.
		bool checkComputeLimits();

		void release();
	};
}