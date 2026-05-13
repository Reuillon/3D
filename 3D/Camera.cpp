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

void camera::camRot(glm::vec2 cameraTurn)
{
	yaw += cameraTurn.x;
	pitch += cameraTurn.y;
}
void camera::update()
{
	//PREVENTS CAMERA FROM FLIPPING UPSIDE DOWN
	pitch = glm::clamp(pitch, -89.0f, 89.0f);
	//update view matrix
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
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
