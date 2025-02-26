#pragma once

#include "..\glwrappers\Texture.h"
#include "..\glwrappers\Cubemap.h"
#include "Camera.h"

#include <vector>

namespace BOSL
{
	// for testing
	struct PointLight
	{
		glm::vec3 position;
		glm::vec3 emission;
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
		glm::mat4 TBN; // offset: 96, size: 64
	};
	glm::mat3 triangleTBN(Vertex v0, Vertex v1, Vertex v2);

	struct Sphere {
		alignas(16) glm::vec4 center;
		float radius;
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

		Texture normalMap;
		Texture albedoMap;
		Texture metallicMap;
		Texture roughnessMap;

		// Point light (for testing)
		PointLight pLight;

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