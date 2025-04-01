#include "Scene.h"

namespace BOSL
{
	Triangle::Triangle(Vertex v0, Vertex v1, Vertex v2)
		: v0(v0), v1(v1), v2(v2)	
	{
		// Calculate T,B,N vectors

		// Edges of the triangle
		glm::vec3 edge1 = v1.pos - v0.pos;
		glm::vec3 edge2 = v2.pos - v0.pos;

		// U delta
		float deltaU1 = v1.uv.x - v0.uv.x;
		float deltaU2 = v2.uv.x - v0.uv.x;

		// V delta
		float deltaV1 = v1.uv.y - v0.uv.y;
		float deltaV2 = v2.uv.y - v0.uv.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));		
		glm::vec3 tangent = glm::normalize(f * (deltaV2 * edge1 - deltaV1 * edge2));
		glm::vec3 bitangent = glm::cross(normal, tangent);

		T = glm::vec4(tangent, 0.0f);
		B = glm::vec4(bitangent, 0.0f);
		N = glm::vec4(normal, 0.0f);
	}

	PointLight::PointLight()
		: position(glm::vec3(3.0f))
		, emission(glm::vec3(1.0f))
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
		, cubemap("DefaultCubemap")
		, albedoMap(config::texturesDir + "\\gold\\albedo.jpg", GL_SRGB)
		, normalMap(config::texturesDir + "\\gold\\normalDX.jpg", GL_RGB)
		, metallicMap(config::texturesDir + "\\gold\\metallic.jpg", GL_RED)
		, roughnessMap(config::texturesDir + "\\gold\\roughness.jpg", GL_RED)
		, useCubemap(false)
		, useAlbedoMap(false)
		, useNormalMap(false)
		, useMetallicMap(false)
		, useRoughnessMap(false)
		, hemisphereSamples(10)
		, exposure(3.0f)
	{
		// Left intentionally empty
	}
}

