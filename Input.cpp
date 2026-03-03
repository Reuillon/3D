#include "Input.h"
#include <iostream>

Input::Input(void)
{

}


//MOUSE SINGLE INPUT
void Input::mouse_button_callback(GLFWwindow* inputWindow, int button, int action, int mods)
{
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {

       //std::cout << "pressed\n";
    }
    else
    {
        //std::cout << "notpressed\n";
    }

}