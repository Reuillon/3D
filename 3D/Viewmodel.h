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
		Viewmodel(short int animLength, std::string path)
		{
			animation = new Animation[animLength];
			m.initialize(path);
			for (int i = 0; i < animLength; i++)
			{
				Animation temp;
				temp.initialize(path, &m, i);
				animation[i].initialize(path, &m, i);
			}
			animate.initialize(&animation[0]);
			animate.UpdateAnimation(deltaTime);
		}

		void render(camera c, Shader& shader, GLFWwindow* window)
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			shader.use();
			shader.setVec3("viewPos", c.cameraPos);
			shader.setMat4("projection", c.projection);
			shader.setMat4("view", c.view);

			//DIR LIGHT
			shader.setFloat("material.shininess", 1.0f);
			shader.setFloat("material2.shininess", 1.0f);
			shader.setVec3("dirLight.direction", 0.1f, -0.7f, 1.0f);
			shader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
			shader.setVec3("dirLight.diffuse", 0.33f, 0.33f, 0.33f);
			shader.setVec3("dirLight.specular", 0.15f, 0.15f, 0.15f);



			//CALCULATE BONE TRANSFORM
			auto transforms = animate.GetFinalBoneMatrices();
			
			for (int i = 0; i < transforms.size(); ++i)
			{
				shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}
			//INITIALIZE OBJECT ORIENTATIONS
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 normal = glm::mat4(1.0f);
			model = glm::inverse(model) * glm::inverse(c.view);
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
			model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(5.0f, -0.42, 0.1333));

			//SEND OBJECT DATA TO SHADER AND DRAW
			shader.setMat4("model", model);
			normal = glm::mat3(glm::transpose(glm::inverse(model)));
			shader.setMat3("inverse", normal);

			
			//INITIALIZE OBJECT ORIENTATIONS
			glm::mat4 viewModel = glm::mat4(1.0f);
			viewModel = glm::inverse(viewModel) * glm::inverse(c.view);
			viewModel = glm::scale(viewModel, glm::vec3(10.0f, 10.0f, 10.0f));
			viewModel = glm::rotate(viewModel, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
			viewModel = glm::translate(viewModel, glm::vec3(0, 0, 0));

			
			//SEND OBJECT DATA TO SHADER AND DRAW
			
			
			
			animate.loopAnim(true);
			animController(window);
			glClear(GL_DEPTH_BUFFER_BIT);
			
			m.draw(shader);
		}

		void animController(GLFWwindow* window)
		{
			//SELECTS ANIMATION BASED ON WHICH VALUE THISANIM IS SET TO
			switch (thisAnim)
			{
				case 0:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[0]);
					break;
				}

				case 1:
				{
					animate.loopAnim(true);
					animate.PlayAnimation(&animation[1]);
					break;
				}
				case 2:
				{
					animate.loopAnim(true);
					animate.PlayAnimation(&animation[2]);
					break;
				}
				case 3:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[3]);
					break;
				}
				case 4:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[4]);
					break;
				} case 5:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[5]);
					break;
				} 
				case 6:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[6]);
					break;
				} 
				case 7:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[7]);
					break;
				}	
				case 8:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[8]);
					break;
				}
				case 9:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[9]);
					break;
				}
				default:
				{
					animate.loopAnim(false);
					animate.PlayAnimation(&animation[0]);
					break;
				}
			}
			//SIMPLE ANIMATION CONTROLLER
			if (playAnim == 1)
			{
				animate.UpdateAnimation(deltaTime);
			}
			else
			{
				animate.PauseAnim();
			}
			if (animate.finishedAnim() == true)
			{
				animate.ResetAnim();
				thisAnim = 0;
			}
			if (reset == 1)
			{
				animate.ResetAnim();
				reset = 0;
			}


			///ANIMATION CONTROLS
			///
			/// 
			///
			if (glfwGetKey(window, GLFW_KEY_1))
			{
				reset = 1;
				thisAnim = 0;
			}
			if (glfwGetKey(window, GLFW_KEY_2))
			{
				reset = 1;
				thisAnim = 1;
			}
			if (glfwGetKey(window, GLFW_KEY_3))
			{
				reset = 1;
				thisAnim = 2;
			}
			if (glfwGetKey(window, GLFW_KEY_4))
			{
				reset = 1;
				thisAnim = 3;
			}
			if (glfwGetKey(window, GLFW_KEY_5))
			{
				reset = 1;
				thisAnim = 4;
			}
			if (glfwGetKey(window, GLFW_KEY_6))
			{
				reset = 1;
				thisAnim = 5;
			}
			if (glfwGetKey(window, GLFW_KEY_7))
			{
				reset = 1;
				thisAnim = 6;
			}
			if (glfwGetKey(window, GLFW_KEY_8))
			{
				reset = 1;
				thisAnim = 7;
			}
			if (glfwGetKey(window, GLFW_KEY_0))
			{
				reset = 1;
				thisAnim = 9;
			}

		}

		void setState(int set)
		{
			switch (set)
			{
				case 0:
					playAnim *= -1;
					break;
				case 1:
					reset = 1;
					break;				
				case 2:
					reset = 1;
					thisAnim = 7;
					break;
			}
		}
};