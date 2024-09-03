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
	std::vector<Model> models;
	std::vector<Shader> shaders;
	int mCount = 0;

public:
	void run();
	Model iniModel(std::string mName,bool isStatic);
	void iniShader(const char* vertexPath, const char* fragmentPath);
	void iniLight();
};

