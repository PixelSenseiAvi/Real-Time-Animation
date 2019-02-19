#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	count = 0;

	distance = 25.f;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
		distance += (GLfloat)(20 * velocity);
	}

	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
		distance -= (GLfloat)(20 * velocity);
	}

	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	//Later remember to set limits [0 - 2*pi]
	verticalAngle = yChange;
	horizontalAngle = xChange;

	//TO-DO: add the horzontal and vertical angles occured by rolling // consider angles w,a, s,d
	offset.x = distance * cos(glm::radians(verticalAngle))*(-sin(glm::radians(horizontalAngle)));
	offset.y = distance * sin(glm::radians(verticalAngle));
	offset.z = distance * cos(glm::radians(verticalAngle)) * cos(glm::radians(horizontalAngle));
	offset.x = -offset.x;
	offset.z = -offset.z;

	update();
}


glm::mat4 Camera::calculateViewMatrix()
{
	 return glm::lookAt(position, position + front, up);
}
glm::vec3 Camera::getCameraPosition()
{
	return position;
}
glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}


void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
	
}


Camera::~Camera()
{
}
