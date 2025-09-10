#pragma once
#include <array>
#include <vector>
#include <iostream>

#include "Collision.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"

#include <GLFW/glfw3.h>

void renderCube();
void renderQuad();
void renderSphere();
unsigned int loadTexture(char const* path);
void drawSand(camera& c, Shader& shader, Model& m, unsigned int envTexture);
void drawWater(camera& c, Shader& shader, Model& m, unsigned int envTexture);
void mapRender(camera& c, Shader& shader, Model& m, glm::vec3 pos = glm::vec3(0.0f));
void staticRender(camera& c, Shader& shader, Model& m);