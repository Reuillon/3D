#include "Viewmodel.h"



Viewmodel::Viewmodel(short int animLength, std::string path)
{
	spread = 2;
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

int ammo = 7;
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



glm::vec3 lightDirTEST = glm::normalize(glm::vec3(30.0f, -5.0, 30.0f));
//glm::vec3 lightDirTEST = glm::normalize(glm::vec3(30.0f, -80, 30.0f));
void Viewmodel::render(camera& c, Shader& shader, GLFWwindow* window)
{
	if (lastPos != c.cameraPos)
	{
		
		swayX += deltaTime * 1 * 8;
		swayY += deltaTime * 2 * 8;
	}
	else
	{
		swayX /= 1.0008f;
		swayY /= 1.0008f;
	}
	if (swayX > 2 * PI) {swayX = swayX - (2 * PI);}
	if (swayY > 2 * PI) {swayY = swayY - (2 * PI);}


	lastPos = c.cameraPos;
	glfwGetCursorPos(window, &cposx, &cposy);




	offsetX = ((double)cposx - (double)clastX) * 0.2;
	offsetY = ((double)clastY - (double)cposy) * 0.2;
	clastX = (double)cposx;
	clastY = (double)cposy;

	if (offsetX > 2.0){
		offsetX = 2.0;}
	if (offsetY > 1.2){
		offsetY = 1.2;}
	if (offsetX < -2.0){
		offsetX = -2.0;}
	if (offsetY < -1.2){
		offsetY = -1.2;}
	totalAMT_X += offsetX * 0.001;
	totalAMT_Y += offsetY * 0.001;

	totalAMT_X /= (1.026 / (1 + deltaTime));
	totalAMT_Y /= (1.026 / (1 + deltaTime));
	
	c.pitch += recoil * 1.1;
	c.yaw += recoilX * 1.1;
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;

	lastFrame = currentFrame;

	shader.use();

	shader.setInt("albedoMap", 0);
	shader.setInt("normalMap", 1);
	shader.setInt("metallicMap", 2);
	shader.setInt("roughnessMap", 3);
	shader.setInt("aoMap", 4);
	shader.setInt("irradianceMap", 5);
	shader.setInt("prefilterMap", 6);
	shader.setInt("brdfLUT", 7);
	c.fov = 45;
	c.update(deltaTime);
	shader.setMat4("projection", c.projection);
	shader.setMat4("view", c.view);
	shader.setVec3("camPos", c.cameraPos);
	shader.setVec3("lightDir", lightDirTEST);

	if (spread > 1.5)
	{
		spread /= 1.0065 * (1 + deltaTime);
	}
	if (spread < 1.5)
	{
		spread = 1.5;
	}

	glm::vec3 lightPositions[] =
	{
		glm::vec3(-10.0f,  10.0f, 00.0f),
		glm::vec3(10.0f,  10.0f, 00.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f)

	};
	glm::vec3 lightColors[] =
	{
		glm::vec3(200.0f, 200.0f, 200.0f),
		glm::vec3(200.0f, 200.0f, 200.0f),
		glm::vec3(200.0f, 200.0f, 200.0f),
		glm::vec3(200.0f, 200.0f, 200.0f)
	};
	/**/
	// render light source (simply re-render sphere at light positions)
	// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
	// keeps the codeprint small.
	/*
	for (unsigned int i = 0; i < 4; ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(0.0, 0.0, 0.0);
		newPos = lightPositions[i];
		shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
	}
	*/
	//CALCULATE BONE TRANSFORM
	auto transforms = animate.GetFinalBoneMatrices();

	for (int i = 0; i < transforms.size(); ++i)
	{
		shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}
	//INITIALIZE OBJECT ORIENTATIONS
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::inverse(model) * glm::inverse(c.view);
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(5.0f, -0.5 + (( - recoil/2) * deltaTime) + ((-totalAMT_Y * 2) / (1 - deltaTime)) + (0.010 * sin(swayY)), (0.41 + (( - recoilX / 2) * deltaTime) + ((-totalAMT_X * 2) / (1 - deltaTime))) + (0.010 * sin(swayX))));
	model = glm::rotate(model, (float)( ((0.4 * sin(swayY * 0.5))) * 0.0174533f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, (float)( 0.2 * sin(swayX * 0.5) * 0.0174533f), glm::vec3(0.0f, 1.0f, 0.0f));
	

	//SEND OBJECT DATA TO SHADER AND DRAW
	shader.setMat4("model", model);

	animate.loopAnim(true);
	animController(window);
	glClear(GL_DEPTH_BUFFER_BIT);

	m.draw(shader);
}
float thisTimer = 0.0;
bool isShoot = false; 
float randomNum = 0.0;
double accumulator = 0.0;
bool animBuffer = true;

void Viewmodel::animController(GLFWwindow* window)
{
	//accumulator += deltaTime;
	//SELECTS ANIMATION BASED ON WHICH VALUE THISANIM IS SET TO
	

	
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
	if (glfwGetKey(window, GLFW_KEY_R))
	{
		if (ammo < 8)
		{
			if (animBuffer)
			{
				reset = 1;
				if (ammo > 0)
				{
					thisAnim = 6;
				}
				else
				{
					thisAnim = 5;
				}
			}
			animBuffer = false;
		}
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
	
	//std::cout << ammo << "\n";
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && ammo > 0 && animBuffer)
	{
		if (thisTimer == 0)
		{
			ammo -= 1;
			spread = 5;
			reset = 1;

			if (ammo > 0)
			{
				animate.loopAnim(false);
				animate.PlayAnimation(&animation[7]);
				thisAnim = 7;
			}
			else
			{
				animate.loopAnim(false);
				animate.PlayAnimation(&animation[8]);
				thisAnim = 8;
			}
			
		}
		if (thisTimer < 0.165)
		{
			thisTimer += deltaTime;
		}
		if (thisTimer < (0.015))
		{
			
			//input.x = 4000;
		}
		
	}
	else
	{
		if (thisTimer > 0.0 && thisTimer <= 0.165)
		{
			thisTimer += deltaTime;
		}
		
			
		
		
		if (thisTimer > 0.165)
		{
			thisTimer = 0;
		}
		
		
	}
	if (thisTimer == 0.0)
	{
		recoil = 0;
		recoilX = 0;
		randomNum = ((rand() % 401) - 200);
	}
	if (thisTimer < 0.045 && thisTimer > 0.035)
	{
		recoil = (95.5f * deltaTime) ;
		recoilX = (randomNum * 0.1f * deltaTime) ;
	}
	if (thisTimer < 0.08 && thisTimer > 0.065)
	{
		recoil = ( -(25.5f * deltaTime)) ;
		recoilX = (-(recoilX / 3) * deltaTime * 0.1f) ;
	}
	
	
		//accumulator = 0;
	
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
		animate.PlayAnimation(&animation[1]);
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
		animate.PlayAnimation(&animation[1]);
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
			ammo = 8;
		}
		if (thisAnim == 5)
		{
			animBuffer = true;
			ammo = 7;
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

double mSway(double offset)
{
	double swayAmt = 0.0;
	return 1.0;
}
