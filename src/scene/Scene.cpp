#include "Scene.h"

namespace BOSL
{
	void calculateTBN(Triangle& t)
	{
		// Edges of the triangle : position delta
		glm::vec3 edge1 = t.v1.pos - t.v0.pos;
		glm::vec3 edge2 = t.v2.pos - t.v0.pos;

		// UV delta
		glm::vec2 deltaUV1 = t.v1.uv - t.v0.uv;
		glm::vec2 deltaUV2 = t.v2.uv - t.v0.uv;

		// Fractional part of equation (determinant)
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		glm::vec3 tangent = f * (edge1 * deltaUV2.y - edge2 * deltaUV1.y);
		glm::vec3 bitangent = f * (edge2 * deltaUV1.x - edge1 * deltaUV2.x);
		glm::vec3 normal = glm::cross(edge1, edge2);

		t.T = glm::vec4(glm::normalize(tangent), 0.0f);
		t.B = glm::vec4(glm::normalize(bitangent), 0.0f);
		t.N = glm::vec4(glm::normalize(normal), 0.0f);
	}

	// Static constants
	const glm::vec3 Scene::camAtStart::position = glm::vec3(0.0f, 0.0f, 5.0f);
	const glm::vec3 Scene::camAtStart::lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	const float Scene::camAtStart::focalLength = 1.0f;
	const float Scene::camAtStart::vfov = 30.0f;

	Scene::Scene()
		: camera(
			camAtStart::position,
			camAtStart::lookAt,
			camAtStart::vfov,
			camAtStart::focalLength)
		, albedoMap(config::imagesDir + "learnopengl\\albedo.png", GL_SRGB)
		, normalMap(config::imagesDir + "learnopengl\\normal.png", GL_RGB)
		, metallicMap(config::imagesDir + "learnopengl\\metallic.png", GL_RED)
		, roughnessMap(config::imagesDir + "learnopengl\\roughness.png", GL_RED)
	{
		// for testing
		pLight.position = glm::vec3(0.0f);
		pLight.emission = glm::vec3(0.0f);
	}

}

