#pragma once
#include "Model.h"
#include "Animator.h"
#include "Shader.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include "Input.h"
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
		

	public:
		int ammo = 7;
		float spread;
		float length;
		float thickness;
		float fallSpeed = 0;
		//CALCULATE BONE TRANSFORM
	    std::vector<glm::mat4> transforms = animate.GetFinalBoneMatrices();
		glm::mat4 model = glm::mat4(1.0f);
		Viewmodel(short int animLength, std::string path);

		void updateViewmodel(camera& c, GLFWwindow* window, float speed, float gravity, bool isGrounded);

		void render(camera& c, Shader& shader, GLFWwindow* window);

		void animController(GLFWwindow* window);

		void setState(int set);
}; 

extern Input &input;