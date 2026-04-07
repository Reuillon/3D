#pragma once
//STANDARD LIBRARY
#include <iostream>


//MY CLASSES
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"

#include "Animator.h"
#include "Model.h"

//OPENGL LIBRARIES
#include <GLFW/glfw3.h>



class Level
{
	private:
		glm::vec3 position = glm::vec3(0.0);
		glm::vec3 rotation = glm::vec3(0.0);
		glm::vec3 scale = glm::vec3(1.0);
        glm::mat4 model = glm::mat3(1.0);
        glm::mat3 inverseMatrix = glm::mat3(1.0);
        Model map;
	public:
        std::vector<MeshCollider> collisionMap;
        Level(std::string mapModel, std::string mapCollision, glm::vec3 mapPos, glm::vec3 mapRot, glm::vec3 mapScale);
        

        void mapRender(camera& c, Shader& shader);

};

