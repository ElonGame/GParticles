#include "Camera.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
glm::vec4 Camera::getPosition()
{
	return glm::vec4(position, 1);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
glm::vec4 Camera::getFront()
{
	return glm::vec4(front, 0);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
glm::vec4 Camera::getUp()
{
	return glm::vec4(up, 0);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::processKeyboard(Camera_Movement direction)
{
	GLfloat velocity = speed;

	switch (direction)
	{
		case FORWARD:
			position += front * velocity;
			break;

		case BACKWARD:
			position -= front * velocity;
			break;

		case LEFT:
			position -= right * velocity;
			break;

		case RIGHT:
			position += right * velocity;
			break;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::processMouseMovement()
{
	//// accumulate this frame offset to target offset
	//targetOffsetX += mouseX - lastMouseX;
	//targetOffsetY += mouseY - lastMouseY;

	//std::cout << ">> TargetOffsetX: " << targetOffsetX << std::endl;

	//GLfloat xoffset = glm::clamp(targetOffsetX, -offsetLimit, offsetLimit);
	//GLfloat yoffset = glm::clamp(targetOffsetY, -offsetLimit, offsetLimit);

	//std::cout << ">> Xoffset: " << xoffset << std::endl;

	//targetOffsetX -= xoffset;
	//targetOffsetY -= yoffset;

	//targetOffsetX += mouseX - lastMouseX;
	//targetOffsetY += mouseY - lastMouseY;

	//std::cout << ">> TargetOffsetX: " << targetOffsetX << std::endl;

	//GLfloat xoffset = glm::clamp(targetOffsetX, -offsetLimit, offsetLimit);
	//GLfloat yoffset = glm::clamp(targetOffsetY, -offsetLimit, offsetLimit);

	yaw += (mouseX - lastMouseX) * mouseSensitivity;
	pitch += (lastMouseY - mouseY) * mouseSensitivity;

	// don't flip screen when pitch is out of bounds
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	updateCameraVectors();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::updateCameraVectors()
{
	// calculates the new front, right and up vectors
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(newFront);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}