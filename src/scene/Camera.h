#pragma once

#include "..\utils\Config.h"

namespace BOSL
{
	struct Viewport {
		// Horizontal axis
		glm::vec3 horiz;
		// Vertical axis
		glm::vec3 vert;
		// Position of pixel (0,0) in 3D space
		glm::vec3 pixel00;
		// Horizontal distance between adjacent pixel centers
		glm::vec3 deltaHoriz;
		// Vertical distance between adjacent pixel centers
		glm::vec3 deltaVert;
	};

	class Camera
	{
	public:
		Camera();

		Camera(glm::vec3 worldUp, glm::vec3 position, glm::vec3 lookAt,
			float vfov, float focalLength);
		
		glm::vec3 getPosition() const;
		void setPosition(glm::vec3 position);

		glm::vec3 getLookAt() const;
		void setLookAt(glm::vec3 lookAt);
		
		// Returns Viewport (rectangle in 3D space)
		// associated with camera
		Viewport calculateViewport() const;

	private:
		glm::vec3 worldUp;

		glm::vec3 position;
		glm::vec3 lookAt;

		float vfov;
		float focalLength;

		// direction vectors
		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;

		// Calculates direction vectors,
		// using position, lookAt and worldUp
		void calculateDirVecs();
	};
}