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
		
		int playAnim = 1;
		int reset = 0;

		uint32_t shoot = SoundBuffer::get()->addSoundEffect("SOUNDS/SNIPE.mp3");
		uint32_t reload = SoundBuffer::get()->addSoundEffect("SOUNDS/Reload.mp3");
		uint32_t reloadEmpty = SoundBuffer::get()->addSoundEffect("SOUNDS/ReloadEmpty.mp3");
		SoundSource playerSpeaker[3];
		
	public:
		int ammo = 5;
		bool shootRay;
		float spread;
		float length;
		float thickness;
		float fallSpeed = 0;
		float thisTimer = 0.0;
		bool isShoot = false;
		float randomNum = 0.0;
		double accumulator = 0.0;
		bool animBuffer = true;
		int thisAnim = 1;
		float PI = 3.1415926535;
		float recoil = 0.0;
		float recoilX = 0.0;
		double cposx = 0.0, cposy = 0.0;
		double clastX = 1.0, clastY = 1.0;
		double offsetX, offsetY;
		double totalAMT_X = 0.0, totalAMT_Y = 0.0;
		double delta = 0.0;
		double swayX = 0.0, swayY = 0.0;

		glm::vec3 lastPos;


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