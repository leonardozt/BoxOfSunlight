#include "Scene.h"

namespace BOSL
{
	Triangle::Triangle(Vertex v0, Vertex v1, Vertex v2)
		: v0(v0), v1(v1), v2(v2)	
	{
		// Calculate T,B,N vectors

		// Edges of the triangle : position delta
		glm::vec3 edge1 = v1.pos - v0.pos;
		glm::vec3 edge2 = v2.pos - v0.pos;

		// UV delta
		glm::vec2 deltaUV1 = v1.uv - v0.uv;
		glm::vec2 deltaUV2 = v2.uv - v0.uv;

		// Fractional part of equation (determinant)
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		glm::vec3 tangent = f * (edge1 * deltaUV2.y - edge2 * deltaUV1.y);
		glm::vec3 bitangent = f * (edge2 * deltaUV1.x - edge1 * deltaUV2.x);
		glm::vec3 normal = glm::cross(edge1, edge2);

		T = glm::vec4(glm::normalize(tangent), 0.0f);
		B = glm::vec4(glm::normalize(bitangent), 0.0f);
		N = glm::vec4(glm::normalize(normal), 0.0f);
	}

	PointLight::PointLight()
		: position(glm::vec3(3.0f))
		, emission(glm::vec3(3.0f))
	{
		// Left intentionally empty
	}

	Material::Material()
		: baseColor(glm::vec3(1.0f))
		, subsurface(0.0f)
		, metallic(0.0f)
		, specular(0.0f)
		, specularTint(0.0f)
		, roughness(1.0f)
		, anisotropic(0.0f)
		, sheen(0.0f)
		, sheenTint(0.0f)
		, clearCoat(0.0f)
		, clearCoatGloss(1.0f)
	{
		// Left intentionally empty
	}

	// Static constants
	const glm::vec3 Scene::CamAtStart::position = glm::vec3(0.0f, 0.0f, 5.0f);
	const glm::vec3 Scene::CamAtStart::lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	const float Scene::CamAtStart::focalLength = 1.0f;
	const float Scene::CamAtStart::vfov = 60.0f;

	Scene::Scene()
		: camera(
			CamAtStart::position,
			CamAtStart::lookAt,
			CamAtStart::vfov,
			CamAtStart::focalLength)
		, albedoMap(config::texturesDir + "\\rock\\rock_face_03_diff_4k.jpg", GL_SRGB)
		, normalMap(config::texturesDir + "\\rock\\rock_face_03_nor_gl_4k.jpg", GL_RGB)
		, metallicMap(config::texturesDir + "\\rock\\rock_face_03_metal_4k.jpg", GL_RED)
		, roughnessMap(config::texturesDir + "\\rock\\rock_face_03_rough_4k.jpg", GL_RED)
		, useCubemap(false)
		, useAlbedoMap(true)
		, useNormalMap(true)
		, useMetallicMap(true)
		, useRoughnessMap(true)
		, hemisphereSamples(10)
	{
		// Left intentionally empty
	}
}

