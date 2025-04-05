#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace BOSL
{
	namespace config
	{
		const std::string windowTitle = "BoxOfSunlight";
		
		constexpr unsigned int windowWidth = 800;
		constexpr unsigned int windowHeight = 800;

		// Number of screen chunks used for rendering,
		// on X and Y axes respectively
		constexpr glm::uvec2 numChunks(40);

		static_assert(
			((numChunks.x > 0 && numChunks.y > 0)
				&& (numChunks.x <= windowWidth && numChunks.y <= windowHeight)),
			"Number of chunks needs to be in [1, windowWidth] for the x axis and "
			"[1, windowHeight] for the y axis.");
		static_assert((windowWidth % numChunks.x == 0) && (windowHeight % numChunks.y == 0),
			"Screen size must be a multiple of number of chunks");

		// Path relative to executable
		const std::string rootDir = "..\\..\\..";
		const std::string shadersDir = rootDir + "\\shaders";
		const std::string assetsDir = rootDir + "\\assets";
		const std::string cubemapsDir = assetsDir + "\\cubemaps";
		const std::string texturesDir = assetsDir + "\\textures";
		const std::string modelsDir = assetsDir + "\\models";

		const bool debugMode = false;
	}
}