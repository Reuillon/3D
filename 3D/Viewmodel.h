#pragma once
#include "Model.h"
#include "Animator.h"
#include "Shader.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include "Input.h"
#include "SoundBuffer.h"
#include "SoundSource.h"

class Viewmodel
{
	private:
		//OBJECT DATA
		Model m;
		Animator animate;
		Animation *animation;

		double deltaTime = 0.0f;
		float lastFrame = 0.0f;
		//ANIMATION CONTROLLER
		int thisAnim = 1;
		int playAnim = 1;
		int reset = 0;

		uint32_t sound1 = SoundBuffer::get()->addSoundEffect("SOUNDS/SNIPE.mp3");
		uint32_t sound2 = SoundBuffer::get()->addSoundEffect("SOUNDS/Fah.mp3");
		SoundSource playerSpeaker[2];
		int ammo = 7;
	public:
		bool shootRay;
		float spread;
		float length;
		float thickness;
		float fallSpeed = 0;

		glm::vec3 viewPos = glm::vec3(4.85f, -0.548, 0.443);
		//CALCULATE BONE TRANSFORM
	    std::vector<glm::mat4> transforms = animate.GetFinalBoneMatrices();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat3 normalMatrix = glm::mat3(1.0f);
		Viewmodel(short int animLength, std::string path);

		void updateViewmodel(camera& c, GLFWwindow* window, float speed, float gravity, bool isGrounded);

		void render(camera& c, Shader& shader, GLFWwindow* window);

		void animController(GLFWwindow* window);

		void setState(int set);
}; 

extern Input &input;