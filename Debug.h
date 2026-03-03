#ifndef DEBUG_H
#define DEBUG_H
#include <array>
#include <vector>
#include <iostream>

#include "Collision.h"
#include "Camera.h"
#include "Shader.h"

#include <GLFW/glfw3.h>

class Debug
{
	private:

	public:
		Shader defaultShader;
		
		
		float xC = -45.9203f, yC = 35.2217f, zC = -53.3815f;
		float xVal = 0, yVal = 0, zVal = 0;
		glm::mat4 gridPos = glm::mat4(1.0f);
		unsigned int lineVAO = 0;
		unsigned int lineVBO;
		//GRID MESH 
		std::vector<float> grid;
		

		void initializeGrid(float size);
		void debugControls(GLFWwindow* window, float deltaTime);
		
		void drawGrid(camera c);
		void drawCollider(MeshCollider& collider, camera c);
		void drawLine(camera c, glm::vec3 origin, glm::vec3 pos, glm::vec4 color = glm::vec4(1.0f));
		void printVector(glm::vec3 vec);
		void printVector(glm::vec2 vec);
};

#endif