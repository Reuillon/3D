#include "Viewmodel.h"



Viewmodel::Viewmodel(short int animLength, std::string path)
{
	length = 0.45f;
	thickness = 0.75;
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





void Viewmodel::updateViewmodel(camera& c, GLFWwindow* window, float speed, float gravity, bool isGrounded)
{
	FixedUpdateViewmodel(c, window, speed, gravity, isGrounded);
	
	
	animController();
	//CALCULATE BONE TRANSFORM
	transforms = animate.GetFinalBoneMatrices();
	animate.loopAnim(true);
	glfwGetCursorPos(window, &cposx, &cposy);
	offsetX = (cposx - clastX) * 0.2;
	offsetY = (clastY - cposy) * 0.2;
	clastX = cposx;
	clastY = cposy;
}

void Viewmodel::FixedUpdateViewmodel(camera & c, GLFWwindow * window, float speed, float gravity, bool isGrounded)
{


	
	if (speed != 0)
	{
		swayX += deltaTime * speed * 1 * 8;
		swayY += deltaTime * speed * 2 * 8;
	}
	else
	{
		swayX /= 1 + (deltaTime * 2);
		swayY /= 1 + (deltaTime * 2);
	}
	if (swayX > 2 * PI) { swayX = swayX - (2 * PI); }
	if (swayY > 2 * PI) { swayY = swayY - (2 * PI); }

	offsetX = glm::clamp(offsetX,-4.0f,4.0f);
	offsetY = glm::clamp(offsetY,-2.4f,2.4f);


	for (int steps = iterations; steps > 0; --steps)
	{
		totalAMT_X += offsetX * 0.35 * globalTimeStep;
		totalAMT_Y += offsetY * 0.35 * globalTimeStep;

		totalAMT_X /= (1 + (10.26 * globalTimeStep));
		totalAMT_Y /= (1 + (10.26 * globalTimeStep));

		c.pitch += recoil * 1001 * globalTimeStep;
		c.yaw += recoilX * 1001 * globalTimeStep;

		//INITIALIZE OBJECT ORIENTATIONS
		if (gravity == 0)
		{
			fallSpeed /= 1 + (deltaTime * 2);
		}
		if (gravity > 0)
		{
			fallSpeed += 0.1 * deltaTime;
		}
		else if (gravity < 0)
		{
			fallSpeed -= 0.1 * deltaTime;
		}
		fallSpeed = 0;
		WeaponBehavior(window);
	}
	model = glm::mat4(1.0f);
	model = glm::inverse(model) * glm::inverse(c.view);
	model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
	if (isGrounded)
	{
		model = glm::translate(model, glm::vec3(viewPos.x - (speed / 8), (viewPos.y + ((-recoil / 2) * deltaTime) + ((-totalAMT_Y * 2) / (1 - deltaTime)) + (0.010 * sin(swayY))) - (fallSpeed)-(gravity * 0.0025f), (viewPos.z + ((-recoilX / 2) * deltaTime) + ((-totalAMT_X * 2) / (1 - deltaTime))) + (0.010 * sin(swayX))));
	}
	else
	{
		model = glm::translate(model, glm::vec3(viewPos.x - (speed / 8), (viewPos.y + ((-recoil / 2) * deltaTime) + ((-totalAMT_Y * 2) / (1 - deltaTime))) - (fallSpeed)-(gravity * 0.0025f), (viewPos.z + ((-recoilX / 2) * deltaTime) + ((-totalAMT_X * 2) / (1 - deltaTime)))));
	}
	model = glm::rotate(model, (float)(((0.4 * sin(swayY * 0.5))) * 0.0174533f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, (float)(0.2 * sin(swayX * 0.5) * 0.0174533f), glm::vec3(0.0f, 1.0f, 0.0f));
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
}

void Viewmodel::render(camera& c, Shader& shader, GLFWwindow* window)
{
	shader.setMat4("projection", c.projection);
	shader.setMat4("view", c.view);
	shader.setVec3("camPos", c.cameraPos);
	for (int i = 0; i < transforms.size(); ++i)
	{
		shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}
	//SEND OBJECT DATA TO SHADER AND DRAW
	shader.setMat4("model", model);
	shader.setMat3("normalMatrix", normalMatrix);
	
	m.draw(shader);
}


void Viewmodel::WeaponBehavior(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_R) && thisTimer == 0.0)
	{
		if (ammo < 6)
		{
			if (animBuffer)
			{
				reset = 1;
				if (ammo > 0)
				{
					playerSpeaker[1].Play(reload);
					thisAnim = 5;
				}
				else
				{
					playerSpeaker[2].Play(reloadEmpty);
					thisAnim = 6;
				}
			}
			animBuffer = false;
		}
	}
	bool hasShot = false;
	if (thisTimer != 0)
	{
		shootRay = false;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && ammo > 0 && animBuffer && !hasShot)
	{
		if (thisTimer == 0)
		{
			hasShot = true;
			shootRay = true;
			ammo -= 1;
			reset = 1;
			if (ammo > 0)
			{
				animate.loopAnim(false);
				animate.PlayAnimation(&animation[7]);
				thisAnim = 4;
			}
			else
			{
				animate.loopAnim(false);
				animate.PlayAnimation(&animation[8]);
				thisAnim = 4;
			}
			playerSpeaker[0].Play(shoot);
		}

		if (thisTimer < 1.6)
		{
			thisTimer += globalTimeStep;
		}

	}
	else
	{

		if (thisTimer > 0.0)
		{
			thisTimer += globalTimeStep;
		}

		if (thisTimer > 1.6)
		{
			thisTimer = 0;
		}


	}

	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
	{
		hasShot = false;
	}

	if (thisTimer == 0.0 || thisTimer > 0.165)
	{
		recoil = 0;
		recoilX = 0;
		randomNum = ((rand() % 41) - 20);
	}
	if (thisTimer < 0.045 && thisTimer > 0.035)
	{
		recoil = (30.5f * globalTimeStep);
		recoilX = (randomNum * 0.1f * globalTimeStep);
	}
	if (thisTimer < 0.08 && thisTimer > 0.065)
	{
		recoil = (-(10.5f * globalTimeStep));
		recoilX = (-(recoilX / 3) * globalTimeStep * 0.1f);
	}
}

void Viewmodel::animController()
{
	//SELECTS ANIMATION BASED ON WHICH VALUE THISANIM IS SET TO
	

	
	///ANIMATION CONTROLS
	///
	/// 
	///
	/*
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
	*/

	
	switch (thisAnim)
	{
		case 0:
		{
			animate.loopAnim(true);
			animate.PlayAnimation(&animation[0]);
			break;
		}

		case 1:
		{
			animate.loopAnim(true);
			animate.PlayAnimation(&animation[0]);
			break;
		}
		case 2:
		{
			animate.loopAnim(false);
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
		if (thisAnim == 6)
		{
			animBuffer = true;
			ammo = 5;
		}
		if (thisAnim == 5)
		{
			animBuffer = true;
			ammo = 6;
		}

		thisTimer = 0.0;
		if (ammo > 0)
		{
			animate.loopAnim(true);
			animate.PlayAnimation(&animation[1]);
			thisAnim = 1;
		}
		else
		{
			animate.loopAnim(true);
			animate.PlayAnimation(&animation[0]);
			thisAnim = 0;
		}
		animate.ResetAnim();
	}
	if (reset == 1)
	{
		animate.ResetAnim();
		reset = 0;
	}

}

void Viewmodel::setState(int set)
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

