#pragma once
#include "Model.h"
#include "Animator.h"
#include "Shader.h"
#include "Camera.h"
#include <GLFW/glfw3.h>

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
		int thisAnim = 4;
		int playAnim = 1;
		int reset = 0;


	public:
		Viewmodel(short int animLength, std::string path);

		void render(camera c, Shader& shader, GLFWwindow* window);

		void animController(GLFWwindow* window);

		void setState(int set);
};