#include "Camera.h"
#include <iostream>


camera::camera(const unsigned int width, const unsigned int height, const unsigned int cFov)
{
	scrWidth = width;
	scrHeight = height;
	fov = cFov;
	projection = glm::perspective(glm::radians(fov), (float)scrWidth / (float)scrHeight, 0.001f, 2000.0f);
}
void camera::forward()
{
	forw.y = 0.0;
	nextPosition += forw;
}
void camera::left()
{
	nextPosition -= glm::cross(forw, cameraUp);
}
void camera::right()
{
	nextPosition += glm::cross(forw, cameraUp);
}
void camera::back()
{
	forw.y = 0.0;
	nextPosition -= forw;
}
void camera::jump()
{
	nextPosition.y += cameraUp.y;
}
void camera::crouch()
{
	nextPosition.y -= cameraUp.y;
}
void camera::camRot(float xOffset, float yOffset)
{
	
	yaw += xOffset;
	pitch += yOffset;
	
}
void camera::update(float deltaTime)
{
	glm::vec2 normalized = glm::vec2(0.0f);
	if (nextPosition.x != 0 && nextPosition.z != 0)
	{
		normalized = glm::normalize(glm::vec2(nextPosition.x, nextPosition.z));
	}
	//CONSIDER SEPERATE VECTOR FOR VERTICALITY
	cameraPos.x += normalized.x * speed * deltaTime;
	cameraPos.z += normalized.y * speed * deltaTime;
	cameraPos.y += nextPosition.y * deltaTime * 7.5f;
		

	nextPosition = glm::vec3(0.0f);
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


	///THE FORWARD VECTOR IS AFFECTED BY LOOKING UP AND DOWN AFFECTING SPEED DEPENDING 
	//ON HOW HIGH OR LOW THE CAMERAS PITCH IS SO WE USE A SEPERATE VECTOR FOR MOVEMENT
	//THE Y VECTOR IS NOT NECESSARY AND THUS NO CALCULATIONS NEED TO BE MADE
	forw.x = cos(glm::radians(yaw));
	forw.z = sin(glm::radians(yaw));


	cameraFront = glm::normalize(front);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(glm::radians(fov), (float)scrWidth / (float)scrHeight, 0.1f, 10000.0f);
	
}
void camera::fovMod(float val)
{
	fov -= val;
}
