#include "Camera.h"

using glm::vec3;
using glm::normalize;
using glm::cross;
using glm::radians;

#include <iostream>


namespace BOSL
{
	Camera::Camera()
		: worldUp(config::worldUp)
		, position(config::cameraStartPos)
		, lookAt(config::cameraLookAt)
		, vfov(config::cameraVFOV)
		, focalLength(config::cameraFocalLen)
	{
		calculateDirVecs();
		calculateViewport();
	}

	vec3 Camera::getPosition() const
	{
		return position;
	}

	void Camera::setPosition(glm::vec3 position)
	{
		this->position = position;
		calculateDirVecs();
		calculateViewport();
	}

	Viewport Camera::getViewport() const
	{
		return viewport;
	}

	void Camera::calculateDirVecs()
	{
		forward = normalize(lookAt - position);
		up = worldUp;
		right = normalize(cross(forward, up));
	}

	void Camera::calculateViewport()
	{
		// Calculate dimensions of viewport
		float theta = radians(vfov);
		float h = tan(theta/2) * focalLength;
		float viewportHeight = 2 * h;
		float aspectRatio = float(config::windowWidth) / float(config::windowHeight);
		float viewportWidth = viewportHeight * aspectRatio;
		// axes of viewport
		vec3 horiz = viewportWidth * right;
		vec3 vert = viewportHeight * (-up);
		// distances between pixels
		vec3 deltaHoriz = horiz / float(config::windowWidth);
		vec3 deltaVert = vert / float(config::windowHeight);
		vec3 viewportUpperLeft = position
			+ forward * focalLength
			- horiz * 0.5f
			- vert * 0.5f;
		// position of pixel (0,0)
		vec3 pixel00 = viewportUpperLeft + 0.5f * (deltaHoriz + deltaVert);
		viewport =
		{
			horiz,
			vert,
			pixel00,
			deltaHoriz,
			deltaVert
		};
	}
}