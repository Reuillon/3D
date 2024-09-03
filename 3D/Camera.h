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
		float fov = 90;
		
		camera(const unsigned int width, const unsigned int height, const unsigned int cFov);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
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
		
		void forward(float speed);
		void left(float speed);
		void right(float speed);
		void back(float speed);
		void jump(float speed);
		void crouch(float speed);
		void update();
		void camRot(float xOffset, float yOffset);
		void fovMod(float val);
};
#endif