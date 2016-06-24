#pragma once
#include <GL\glew.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>


// camera movement options
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 0.03f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;
const int offsetLimit = 1;

class Camera
{
public:
	int mouseX = 0;
	int mouseY = 0;
	int lastMouseX = 0;
	int lastMouseY = 0;

	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH)
		:	position(pos), worldUp(up), yaw(yaw), pitch(pitch),
			front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(SPEED),
			mouseSensitivity(SENSITIVTY), zoom(ZOOM)
		{ updateCameraVectors(); }

	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
		:	position(glm::vec3(posX, posY, posZ)), worldUp(glm::vec3(upX, upY, upZ)),
			yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(SPEED),
			mouseSensitivity(SENSITIVTY), zoom(ZOOM)
		{ updateCameraVectors(); }

	//TODO: needs destructor ~Camera();

	glm::mat4 getViewMatrix();
	void processKeyboard(Camera_Movement direction);
	void processMouseMovement();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat speed;
	GLfloat mouseSensitivity;
	GLfloat zoom;

	void updateCameraVectors();
};