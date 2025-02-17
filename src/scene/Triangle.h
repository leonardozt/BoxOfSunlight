#pragma once

#include "..\utils\Config.h"

namespace BOSL
{
	class Vertex
	{
	public:
		glm::vec3 pos;
		glm::vec2 uv;
		
		Vertex(glm::vec3 pos, glm::vec2 uv)
			: pos(pos)
			, uv(uv)
		{
			// (left intentionally empty)
		}

		Vertex()
			: pos(glm::vec3(0.0f, 0.0f, 0.0f))
			, uv(glm::vec2(0.0f, 0.0f))
		{
			// (left intentionally empty)
		}
	};

	class Triangle
	{
	public:
		Triangle(Vertex v0, Vertex v1, Vertex v2);
		Triangle();

		Vertex v0;
		Vertex v1;
		Vertex v2;

		glm::mat3 calculateTBN() const;
	};
}
