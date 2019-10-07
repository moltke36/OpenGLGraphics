#include "Camera.h"
#include <gtx\transform.hpp>

const float ROTATIONAL_SPEED = 0.0025f;
const float MOVEMENT_SPEED = 0.1f;

Camera::Camera() : 
	viewDirection(-0.0672f,-0.636f,-0.768f), 
	UP(0.0f,1.0f,0.0f),
	position(0.201f,3.636f,4.530f)
{

}

mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + viewDirection, UP);
}

void Camera::moveForward(float value)
{
	position += MOVEMENT_SPEED * viewDirection * value;
}

void Camera::strafeLeft(float value)
{
	position += MOVEMENT_SPEED * Left * value;
}

void Camera::mouseUpdate(const vec2 & newMousePosition)
{
	vec2 mouseDelta = newMousePosition - oldMousePosition;
	if (glm::length(mouseDelta) > 50.0f)
	{
		oldMousePosition = newMousePosition;
		return;
	}
	Left = glm::cross(UP,viewDirection);
	mat4 rotator = glm::rotate(-mouseDelta.y * ROTATIONAL_SPEED, Left) *
		glm::rotate(-mouseDelta.x * ROTATIONAL_SPEED, UP);

	viewDirection = (glm::mat3)rotator * viewDirection;


	oldMousePosition = newMousePosition;
}
