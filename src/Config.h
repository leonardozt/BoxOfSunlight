#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace BOSL
{
	namespace config
	{
		// Configuration Values
		const int windowWidth = 800;
		const int windowHeight = 600;
		const std::string windowTitle = "BoxOfSunlight";
		const std::string shadersDir = "..\\..\\..\\src\\shaders\\";
		const std::string imagesDir = "..\\..\\..\\images\\";
		const glm::vec3 cameraStartPos(0.0f, 2.5f, 20.0f);
		const glm::vec3 cameraLookAt(0.0f, 0.0f, 0.0f);
		const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
		const float cameraFocalLen = 1.0f;
		const float cameraVFOV = 30.0f;
	}
}