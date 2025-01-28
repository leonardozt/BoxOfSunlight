#include "Camera.h"

using glm::vec3;
using glm::normalize;
using glm::cross;
using glm::radians;

#include <iostream>

namespace BOSL
{
	Camera::Camera(vec3 position, vec3 lookAt, float vfov)
		: position(position), vfov(vfov)
	{
		forward = normalize(lookAt - position);
		worldUp = vec3(0.0f, 1.0f, 0.0f);
		up = worldUp;
		right = normalize(cross(forward, up));
		focalLength = 1.0f;
		setUpViewport();
	}

	vec3 Camera::getPosition()
	{
		return position;
	}

	const Viewport& Camera::getViewport()
	{
		return viewport;
	}

	void Camera::setUpViewport()
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