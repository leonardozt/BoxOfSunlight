#pragma once

#include "Config.h"

namespace BOSL
{
	class Sphere
	{
	public:
		Sphere(glm::vec3 center, float radius);
		Sphere();
		glm::vec3 getCenter() const;
		float getRadius() const;
	private:
		glm::vec3 center;
		float radius;
	};
}
