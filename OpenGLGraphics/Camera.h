#pragma once
#include <glm.hpp>

using glm::vec3;
using glm::mat4;
using glm::vec2;

class Camera
{
	vec3 position;
	vec3 viewDirection;
	vec3 Left;
	const vec3 UP;
	vec2 oldMousePosition;

public:
	Camera();
	void mouseUpdate(const vec2& newMousePosition);
	mat4 getWorldToViewMatrix() const;
	const vec3 getPosition() { return position; }

	void moveForward(float value);
	void strafeLeft(float value);
};

