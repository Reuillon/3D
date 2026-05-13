#ifndef GLOBAL_H
#define GLOBAL_H
#include <GLFW/glfw3.h>
//FIXED TIME STEP VARIABLES
inline constexpr float globalTimeStep = 0.00833;
inline int iterations = 0;
inline float fixedTimer = 0.0f;

//DELTA TIME VARIABLES
inline double deltaTime = 0.0f;
inline float lastFrame = 0.0f;
inline float currentFrame = 0.0f;

//TIMER FOR UPDATING ATTRIBUTES
inline float printTimer = 0.0f;

//CALCULATES HOW MUCH TIME HAS PASSED SINCE THE LAST FRAME WAS RENDERED
inline void CalculateDeltaTime()
{
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

//CALCULATES THE NEXT STEP BASED ON A SPECIFIC TIME SCALE AND INCREMENTS THE ITERATION VALUE
//THIS ALLOWS FOR A FIXED UPDATE RATE FOR THE ENGINES RUNTIME
inline void FixedUpdate()
{
    CalculateDeltaTime();
    fixedTimer += deltaTime;
    while (fixedTimer >= globalTimeStep)
    {
        iterations += 1;
        fixedTimer -= globalTimeStep;
    }

}

//INTERPOLATES POSITIONS OF OBJECTS THAT RELY ON PHYSICS USING LINEAR INTERPOLATION
inline void Lerp()
{

}

//PRINTS FRAMERATE AT THE SPECIFIED RATE
inline void showFPS()
{
    printTimer += deltaTime;
    if (printTimer > 1.0)
    {
        //PRINT FRAMERATE
        std::cout << "\033[2J\033[1;1H" << "|||||| GPU: " << (int)(1000 / ((glfwGetTime() - currentFrame) * 1000)) << " FPS |||||| " << "\n";
        printTimer = 0.0f;
    }
}

#endif