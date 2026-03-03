#include "Camera.h"
#include <iostream>


camera::camera(const unsigned int width, const unsigned int height, const unsigned int cFov)
{
	scrWidth = width;
	scrHeight = height;
	fov = cFov;
	projection = glm::perspective(glm::radians(fov), (float)scrWidth / (float)scrHeight, cameraNear, cameraFar);
}
camera::camera()
{
}
void camera::init(const unsigned int width, const unsigned int height, const unsigned int cFov)
{
	scrWidth = width;
	scrHeight = height;
	fov = cFov;
	projection = glm::perspective(glm::radians(fov), (float)scrWidth / (float)scrHeight, cameraNear, cameraFar);
}

void camera::camRot(float xOffset, float yOffset)
{
	yaw += xOffset;
	pitch += yOffset;
}
void camera::update(float deltaTime)
{
	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}
	if (fov < 45.0f)
	{
		fov = 45.0f;
	}
	if (fov > 120.0f)
	{
		fov = 120.0f;
	}
	//update view matrix
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); // Note that we convert the angle to radians first
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(glm::radians(fov), (float)scrWidth / (float)scrHeight, cameraNear, cameraFar);
}
void camera::fovMod(float val)
{
	fov -= val;
}
