#pragma once

#include "Camera.h"
#include "Cubemap.h"
#include "Triangle.h"

namespace BOSL
{
	class Scene
	{
	public:
		Scene();
		
		Camera camera;
		Cubemap cubemap;

		// Wall object (for testing)
		Triangle t1;
		Triangle t2;
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