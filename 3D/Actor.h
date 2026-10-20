#pragma once
#include <iostream>


//MY CLASSES
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"

#include "Animator.h"
#include "Model.h"

//OPENGL LIBRARIES
#include <GLFW/glfw3.h>


class Actor
{
	private:
	public:
		glm::vec3 actorPosition;
		glm::vec3 actorRotation;
		Model actorModel;
		Animator actorAnimate;
		Animation* actorAnimation;
		
		std::vector<glm::mat4> transforms = actorAnimate.GetFinalBoneMatrices();

		std::vector<MeshCollider> actorCollider;	
		glm::mat4 model = glm::mat4(1.0);
		glm::mat3 inverseMatrix = glm::mat3(1.0);
		Actor(short int animLength, std::string loadActorModel, std::string loadActorCollider, glm::vec3 setPosition, glm::vec3 setRotation);
	 	
		short int animation_size = 0;
		
		bool resetAnimation = false;
		bool actorIsPaused = false;
		
		void PauseAnimation();
		void PlayAnimation();
		void setTransform(glm::vec3 position, glm::vec3 rotation);
		void drawActor(camera& c, Shader& shader);
		void updateActor();
};

