#include "Camera.h"



camera::camera(const unsigned int width, const unsigned int height, const unsigned int cFov)
{
	scrWidth = width;
	scrHeight = height;
	fov = cFov;
	projection = glm::perspective(glm::radians(fov), (float)scrWidth / (float)scrHeight, 0.1f, 500.0f);
}
void camera::forward(float speed)
{
	forw = front;
	forw.y = 0.0;
	forw = glm::normalize(forw);
	cameraPos += speed * forw;
}
void camera::left(float speed)
{
	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
}
void camera::right(float speed)
{
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
}
void camera::back(float speed)
{
	forw = front;
	forw.y = 0.0;
	forw = glm::normalize(forw);
	cameraPos -= speed * forw;
}
void camera::jump(float speed)
{
	cameraPos += speed * cameraUp;
}
void camera::crouch(float speed)
{
	cameraPos -= speed * cameraUp;
}
void camera::camRot(float xOffset, float yOffset)
{
	
	yaw += xOffset;
	pitch += yOffset;
	
}
void camera::update()
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
	projection = glm::perspective(glm::radians(fov), (float)scrWidth / (float)scrHeight, 0.001f, 1000.0f);
	//std::cout <<"x: " << cameraPos.x << " y: " << cameraPos.y << " z: " << cameraPos.z << "\n";
}
void camera::fovMod(float val)
{
	fov -= val;
}
