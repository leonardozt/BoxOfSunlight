#pragma once

#include "..\glwrappers\Texture.h"
#include "..\glwrappers\Cubemap.h"
#include "Camera.h"
#include "Triangle.h"
#include "Sphere.h"

namespace BOSL
{
	// for testing
	class PointLight
	{
	public:
		PointLight()
			: PointLight(glm::vec3(0.0f),glm::vec3(0.0f))
		{
			// (intentionally empty)
		}
		PointLight(glm::vec3 position, glm::vec3 emission)
			: position(position)
			, emission(emission)
		{
			// (intentionally empty)
		}
		glm::vec3 getPosition() {
			return position;
		}
		glm::vec3 getEmission() {
			return emission;
		}
	private:
		glm::vec3 position;
		glm::vec3 emission;
	};

	class Scene
	{
	public:
		Scene();
		
		Camera camera;
		Cubemap cubemap;

		// Wall object (for testing)
		Triangle t1;
		Triangle t2;
		Texture normalMap;
		Texture albedoMap;

		// Point light (for testing)
		PointLight pLight;

		// Sphere object (for testing)
		Sphere sphere;
	private:
		// Data used for initialization of camera
		struct camAtStart
		{
			static const glm::vec3 worldUp;
			static const glm::vec3 position;
			static const glm::vec3 lookAt;
			static const float focalLength;
			static const float vfov;
		};
	};
}