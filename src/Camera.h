#pragma once

#include "Config.h"

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
		Camera(glm::vec3 position, glm::vec3 lookAt, float vfov);
		
		glm::vec3 getPosition();
		
		// Returns viewport associated with camera
		const Viewport& getViewport();

	private:
		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;

		glm::vec3 worldUp;

		float vfov;
		float focalLength;

		Viewport viewport;
		void setUpViewport();
	};
}