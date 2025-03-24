#pragma once

#include "..\glwrappers\Texture.h"
#include "..\glwrappers\Cubemap.h"
#include "Camera.h"

#include <vector>

namespace BOSL
{
	struct PointLight
	{
		glm::vec3 position;
		glm::vec3 emission;

		PointLight();
	};

	struct Vertex {
		alignas(16) glm::vec4 pos;
		glm::vec2 uv;
		// Implicit padding of 8 bites here
	};

	struct Triangle {
		Vertex v0;     // offset:  0, size: 32
		Vertex v1;     // offset: 32, size: 32
		Vertex v2;     // offset: 64, size: 32
		
		glm::vec4 T;
		glm::vec4 B;
		glm::vec4 N;

		Triangle(Vertex v0, Vertex v1, Vertex v2);
	};

	struct Sphere {
		alignas(16) glm::vec4 center;
		float radius;
	};

	struct Material
	{
		glm::vec3 baseColor;
		float subsurface;
		float metallic;
		float specular;
		float specularTint;
		float roughness;
		float anisotropic;
		float sheen;
		float sheenTint;
		float clearCoat;
		float clearCoatGloss;

		Material();
	};

	// Objects of this class are used to store descriptions
	// of scenes to be rendered.
	// The OpenGL context needs to be initialized before Scene object creation.
	class Scene
	{
	public:
		Scene();

		Camera camera;

		Cubemap cubemap;

		std::vector<Triangle> triangles;
		std::vector<Sphere> spheres;

		Material material;

		Texture albedoMap;
		Texture normalMap;
		Texture metallicMap;
		Texture roughnessMap;

		PointLight pLight;

		bool useCubemap;
		bool useAlbedoMap;
		bool useNormalMap;
		bool useMetallicMap;
		bool useRoughnessMap;

		unsigned int hemisphereSamples;

		float exposure;

	private:
		// Data used for initialization of camera
		struct CamAtStart
		{
			static const glm::vec3 position;
			static const glm::vec3 lookAt;
			static const float focalLength;
			static const float vfov;
		};
	};
}