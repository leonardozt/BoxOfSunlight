#include "Scene.h"

namespace BOSL
{
	// Static constants
	const glm::vec3 Scene::camAtStart::worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 Scene::camAtStart::position = glm::vec3(-3.0f, 3.0f, 5.0f);
	const glm::vec3 Scene::camAtStart::lookAt = glm::vec3(1.0f, 0.0f, 0.0f);
	const float Scene::camAtStart::focalLength = 1.0f;
	const float Scene::camAtStart::vfov = 30.0f;

	Scene::Scene()
		: camera(
			camAtStart::worldUp,
			camAtStart::position,
			camAtStart::lookAt,
			camAtStart::vfov,
			camAtStart::focalLength
		)
	{
		// for testing --------------
		glm::vec3 p0(-1.0, 0.0, 1.0);
		glm::vec3 p1(1.0, 0.0, 1.0);
		glm::vec3 p2(1.0, 0.0, -1.0);
		glm::vec3 p3(-1.0, 0.0, -1.0);

		glm::vec2 uv0(0.0, 0.0);
		glm::vec2 uv1(1.0, 0.0);
		glm::vec2 uv2(1.0, 1.0);
		glm::vec2 uv3(0.0, 1.0);

		Vertex v0(p0, uv0);
		Vertex v1(p1, uv1);
		Vertex v2(p2, uv2);
		Vertex v3(p3, uv3);

		t1 = Triangle(v0, v1, v2);
		t2 = Triangle(v0, v2, v3);

		pLight = PointLight(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		// --------------------------
	}

}

