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
			glm::vec3(0.0f,0.0f,1.0f),
			glm::vec3(0.0f,0.0f,0.0f),
			60.0f,
			1.0f
			)
	{
		// Left intentionally empty
	}

	Camera::Camera(glm::vec3 position, glm::vec3 lookAt,
		float vfov, float focalLength)
		: position(position)
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
	}

	glm::vec3 Camera::getLookAt() const
	{
		return lookAt;
	}

	void Camera::setLookAt(glm::vec3 lookAt)
	{
		this->lookAt = lookAt;
		calculateDirVecs();
	}

	void Camera::setVFOV(float vfov)
	{
		this->vfov = vfov;
	}

	void Camera::calculateDirVecs()
	{
		forward = normalize(lookAt - position);
		right = normalize(cross(forward, glm::vec3(0.0f,1.0f,0.0f)));
		up = normalize(cross(right,forward));
	}

	Viewport Camera::calculateViewport() const
	{
		// Calculate dimensions of viewport
		float theta = radians(vfov);
		float h = tan(theta/2) * focalLength;
		float viewportHeight = 2 * h;
		float aspectRatio = float(config::imageWidth) / float(config::imageHeight);
		float viewportWidth = viewportHeight * aspectRatio;
		// axes of viewport
		vec3 horiz = viewportWidth * right;
		vec3 vert = viewportHeight * (-up);
		// distances between pixels
		vec3 deltaHoriz = horiz / float(config::imageWidth);
		vec3 deltaVert = vert / float(config::imageHeight);
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