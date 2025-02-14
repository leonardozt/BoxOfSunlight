#include "Triangle.h"

namespace BOSL
{
	Triangle::Triangle(Vertex v0, Vertex v1, Vertex v2)
		: v0(v0), v1(v1), v2(v2)
	{
		// (left intentionally empty)
	}

	// Calls default constuctors for the 3 vertices
	Triangle::Triangle()
	{
		// (left intentionally empty)
	}

	glm::mat3 Triangle::calculateTBN() const
	{
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
}

