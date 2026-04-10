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
		std::vector<MeshCollider> actorCollider;
		glm::mat4 model = glm::mat4(1.0);
		glm::mat3 inverseMatrix = glm::mat3(1.0);
		Actor(std::string loadActorModel, std::string loadActorCollider, glm::vec3 setPosition, glm::vec3 setRotation);
		void setPosition(glm::vec3 position);
		void drawActor(camera& c, Shader& shader);
};

