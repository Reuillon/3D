#ifndef CAMERA_H
#define CAMERA_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class camera
{

	public:
		unsigned int scrWidth = 1;
		unsigned int scrHeight = 1;
		float yaw = -90.0f;
		float pitch = 0.0f;
		float fov = 95;
		float mSpeed = 0.0;
		float speed = 1.0;
		camera(const unsigned int width, const unsigned int height, const unsigned int cFov);
		glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 front = glm::vec3(0.0f);
		glm::vec3 forw = front;
		glm::mat4 projection;
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::vec3 nextPosition = glm::vec3(0.0f);;
		
		void forward();
		void left();
		void right();
		void back();
		void jump();
		void crouch();
		void update(float deltaTime);
		void camRot(float xOffset, float yOffset);
		void fovMod(float val);
};
#endif