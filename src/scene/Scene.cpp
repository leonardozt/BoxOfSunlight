#include "Scene.h"

namespace BOSL
{
	glm::mat3 triangleTBN(Vertex v0, Vertex v1, Vertex v2) {
		glm::vec3 p0 = v0.pos;
		glm::vec3 p1 = v1.pos;
		glm::vec3 p2 = v2.pos;

		glm::vec2 uv0 = v0.uv;
		glm::vec2 uv1 = v1.uv;
		glm::vec2 uv2 = v2.uv;

		// Edges of the triangle : position delta
		glm::vec3 edge1 = p1 - p0;
		glm::vec3 edge2 = p2 - p0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		// Fractional part of equation (determinant)
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		glm::vec3 tangent = f * (edge1 * deltaUV2.y - edge2 * deltaUV1.y);
		glm::vec3 bitangent = f * (edge2 * deltaUV1.x - edge1 * deltaUV2.x);
		glm::vec3 normal = glm::cross(edge1, edge2);

		tangent = glm::normalize(tangent);
		bitangent = glm::normalize(bitangent);
		normal = glm::normalize(normal);

		return glm::mat3(tangent, bitangent, normal);
	}

	// Static constants
	const glm::vec3 Scene::camAtStart::worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 Scene::camAtStart::position = glm::vec3(0.0f, 0.0f, 5.0f);
	const glm::vec3 Scene::camAtStart::lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	const float Scene::camAtStart::focalLength = 1.0f;
	const float Scene::camAtStart::vfov = 30.0f;

	Scene::Scene()
		: camera(
			camAtStart::worldUp,
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

