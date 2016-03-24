#include "Camera.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::processKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
	GLfloat velocity = this->speed * deltaTime;
	if (direction == FORWARD)
		this->position += this->front * velocity;
	if (direction == BACKWARD)
		this->position -= this->front * velocity;
	if (direction == LEFT)
		this->position -= this->right * velocity;
	if (direction == RIGHT)
		this->position += this->right * velocity;
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

	this->yaw += (mouseX - lastMouseX) * mouseSensitivity;
	this->pitch += (lastMouseY - mouseY) * mouseSensitivity;

	// don't flip screen when pitch is out of bounds
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	this->updateCameraVectors();
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