#include "Camera.h"

using glm::vec3;
using glm::normalize;
using glm::cross;
using glm::radians;

#include <iostream>


namespace BOSL
{
	Camera::Camera()
		: Camera(
			glm::vec3(0.0f,1.0f,0.0f),
			glm::vec3(0.0f,0.0f,1.0f),
			glm::vec3(0.0f,0.0f,0.0f),
			30.0f,
			1.0f
			)
	{
		// (left intentionally empty)
	}

	Camera::Camera(glm::vec3 worldUp, glm::vec3 position, glm::vec3 lookAt,
		float vfov, float focalLength)
		: worldUp(worldUp)
		, position(position)
		, lookAt(lookAt)
		, vfov(vfov)
		, focalLength(focalLength)
	{
		calculateDirVecs();
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

	void Camera::calculateDirVecs()
	{
		forward = normalize(lookAt - position);
		up = worldUp;
		right = normalize(cross(forward, up));
	}

	Viewport Camera::calculateViewport()
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
		Viewport vp =
		{
			horiz,
			vert,
			pixel00,
			deltaHoriz,
			deltaVert
		};
		return vp;
	}
}