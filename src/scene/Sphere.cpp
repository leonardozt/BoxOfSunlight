#include "Sphere.h"

namespace BOSL
{
	Sphere::Sphere(glm::vec3 center, float radius)
		: center(center), radius(radius)
	{
		// (intentionally empty)
	}

	Sphere::Sphere()
		: Sphere(glm::vec3(0.0f,0.0f,0.0f), 1.0f)
	{
		// (intentionally empty)
	}

	glm::vec3 Sphere::getCenter() const
	{
		return center;
	}

	float Sphere::getRadius() const
	{
		return radius;
	}
}