#include "Player.h"

Player::Player(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, GLFWwindow* window)
{
    //LOAD VIEWMODELS
    //primary = new Viewmodel(12, "Models/GUN/PEESTOLOLD.fbx");
    primary = new Viewmodel(8, "Models/GUN/BOLTON.fbx");

	playerCamera.init(SCR_WIDTH, SCR_HEIGHT, 52);
	pWindow = window;
	playerCollider.init(capsule, sizeof(capsule) / sizeof(*capsule));
	floorCollider.init(floor, sizeof(floor) / sizeof(*floor));
    playerCamera.fov = 70;
    playerCamera.update(0);
    playerCamera.cameraPos = glm::vec3(0, 0, 0);
    screenX = SCR_WIDTH;
    screenY = SCR_HEIGHT;
    lastX = screenX / 2.0f;
    lastY = screenY / 2.0f;
}

int swapWeapon = -1;

float fovZoom = 0.0;
float footstepSpeed = 0.4;
float footStepAcceleration;

Debug debugger;
Utility u;
void Player::update(float deltaTime, std::vector<MeshCollider>& collisionMap)
{
    playerCamera.cameraFar = 2500;
    playerCamera.cameraNear = 0.1;
    newDelta = deltaTime;
    normalizedSpeed = glm::vec2(0);
    
    //COLLISION CHECKS
    ResolutionData r;
    isFalling = false;
    isGrounded = false;
    for (int i = 0; i < collisionMap.size(); i++)
    {
        r = GJK(floorCollider, collisionMap[i], deltaTime, false);
        if (r.hasCollision && !isJump)
        {
            gravity = glm::vec3(0.0);
        }
        if (r.hasCollision)
        {
            isGrounded = true;
            if (isJump)
            {
                gravity.y = 11;
            }
        }
        r = GJK(playerCollider, collisionMap[i], deltaTime, true);
        if (r.hasCollision)
        {
            
            //COLLISION RESOLUTION FOR HITTING THE WALL IN AIR
            if (lastSpeed != glm::vec3(0.0))
            {
                //lastSpeed = (glm::normalize(-r.Normal));
                airAcceleration /= 1 + (deltaTime * 40);

            }
            else
            {
                airAcceleration /= 1 + (deltaTime * 35);
                MAXSPEED /= 1 + (deltaTime * 2);
            }

            normalizedSpeed = glm::vec3(normalizedSpeed.x,0,normalizedSpeed.y) - r.Normal;
            isFalling = false;
        }
        else
        {
            isFalling = true;
        }

    }
    playerPosition = playerCollider.pos;
    playerRotation = playerCollider.rot;
    playerForward.x = cos(glm::radians(playerCamera.yaw));
    playerForward.z = sin(glm::radians(playerCamera.yaw));
    //RESET POSITION IF OUT OF BOUNDS
    if (playerCollider.pos.y < -100)
    {
        playerCollider.setTransform(glm::vec3(0.0f, 5.0f, 0.0), glm::vec3(0.0));
        gravity = glm::vec3(0.0);
    }

    //GRAVITY CHECK
    if (isFalling)
    {
        playerCollider.moveCollider(glm::vec3(0.0, gravity.y * deltaTime, 0.0));
        gravity.y -= (20 * deltaTime);
        if (gravity.y < -300)
        {
            gravity.y = -300;
        }
    }


    //RESET POSITION OF PLAYER
    if (glfwGetKey(pWindow, GLFW_KEY_P))
    {
        gravity = glm::vec3(0);
        lastSpeed = glm::vec3(0.0);
        playerCollider.pos = glm::vec3(0, 5, 0);
    }

    if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_2) && primary->thisTimer < 0.155 && primary->thisAnim != 6 && primary->thisAnim != 5)
    {
        scopedIn += (5.0 * deltaTime);
        fovZoom += (300.0 * deltaTime);
        primary->viewPos.x -= (4 * deltaTime);
        primary->viewPos.y += (2 * deltaTime);
        primary->viewPos.z -= (5 * deltaTime);
        playerCamera.pitch += sin(glfwGetTime() * 1.8154f) * 0.35f * deltaTime;
        playerCamera.yaw += cos(glfwGetTime() * 1.045f) * 0.35f * deltaTime;
    }
    else
    {
        scopedIn -= (5.0 * deltaTime);
        fovZoom -= (300.0 * deltaTime);
        primary->viewPos.x += (6 * deltaTime);
        primary->viewPos.y -= (2 * deltaTime);
        primary->viewPos.z += (6 * deltaTime);
    }
    
    //SETS MINIMUM AND MAXIMUM POSITION VALUES FOR AIMING DOWN SIGHTS
    scopedIn = glm::clamp(scopedIn, 0.0f, 1.0f);
    fovZoom = glm::clamp(fovZoom, 0.0f, 55.0f);
    playerCamera.fov = 70.0 - fovZoom;
    primary->viewPos.x = glm::clamp(primary->viewPos.x, 4.07f, 4.85f);
    primary->viewPos.y = glm::clamp(primary->viewPos.y, -0.548f, -0.286f);
    primary->viewPos.z = glm::clamp(primary->viewPos.z, -0.405f, 0.443f);

 

    //CHECK IF PLAYER IS ON THE GROUND
    if (isGrounded)
    {
        footStepAcceleration = (glm::clamp((double)(abs(verticalSpeed / 15) + abs(horizontalSpeed / 15)), 0.0, 1.0));
        footstepSpeed = 1.0 - (footStepAcceleration * 0.6);

        //KEEPS VALUE SMALL ENOUGH FOR AIR STRAFE TO BE MORE CONTROLLABLE
        if (lastSpeed != glm::vec3(0)) {lastSpeed /= 1 + (newDelta * 350);}
        if (movingHorizontal && movingVertical) { groundAcceleration = 105; }
        else { groundAcceleration = 70; }

        
        //PLAY WALKING SOUNDS
        stepTimer += deltaTime;
        if ((verticalSpeed != 0 || horizontalSpeed != 0) && stepTimer > footstepSpeed)
        {
            float randomValue = ((((double)(rand() % 21)) / 100)) + 0.9f;
            playerSpeaker.p_Pitch = randomValue * (1.0 - (footStepAcceleration * 0.1));
            playerSpeaker.p_Gain = randomValue * (1.0 - (footStepAcceleration * 0.1));
            playerSpeaker.Play(sound1);
            stepTimer = 0;
        }
        
        //WASD CONTROLS ON THE GROUND
        glm::vec3 temp = glm::cross(playerForward, glm::vec3(0.0, 1.0, 0.0));
        glm::vec2 hori = glm::vec2(temp.x, temp.z);
        glm::vec2 speedNorm = glm::vec2(horizontalSpeed, verticalSpeed);
        if (speedNorm.x != 0.0f || speedNorm.y != 0.0f)
        {
            speedNorm = glm::normalize(glm::vec2(speedNorm));
        }
        normalizedSpeed = (hori * (abs(speedNorm.x) * horizontalSpeed)) + (glm::vec2(playerForward.x, playerForward.z) * (abs(speedNorm.y) * verticalSpeed));
        playerCollider.moveCollider(glm::vec3(normalizedSpeed.x * deltaTime, 0.0, normalizedSpeed.y * deltaTime));
        playerCollider.moveCollider(glm::vec3(momentum.x * deltaTime, 0.0, momentum.y * deltaTime));
        momentum /= 1 + (deltaTime * 10);
        
        //KEEPS PLAYER MOMENTUM WHEN JUMPING AS LONG AS NEXT JUMP IS FAST ENOUGH
        airMomentumTimer += deltaTime;

        if (airMomentumTimer > 0.025f)
        {
            airVelocity = glm::vec2(0);
            lastSpeed = glm::vec3(0);
            MAXSPEED = 1.0;
            if (u.checkBounds(-1,1, horizontalSpeed) && u.checkBounds(-1, 1, verticalSpeed)) { airAcceleration /= 1 + (newDelta * 10); }
        }
   
    }
    else
    {
        stepTimer = 0.5;
        glm::vec2 normalAirVector = glm::vec3(0.0);
        if (lastSpeed.x != 0 && lastSpeed.z != 0)
        {
            normalAirVector = glm::normalize(glm::vec2(lastSpeed.x, lastSpeed.z));
            momentum = normalAirVector;
        }
        else
        {
            momentum = glm::vec2(0.0);
            normalAirVector = glm::vec2(0);
        }
        verticalSpeed = 0;
        horizontalSpeed = 0;
        
        airMomentumTimer = 0.0;
        momentum = glm::vec2(normalAirVector.x * airAcceleration * MAXSPEED, normalAirVector.y * airAcceleration * MAXSPEED);
        playerCollider.moveCollider(glm::vec3(normalAirVector.x * airAcceleration * MAXSPEED * deltaTime , 0.0, normalAirVector.y * airAcceleration * MAXSPEED * deltaTime));
    }
  
    playerCamera.cameraPos = glm::vec3(playerCollider.pos.x, playerCollider.pos.y + 2.38, playerCollider.pos.z);
    floorCollider.setTransform(playerCollider.pos, glm::vec3(0.0));
    movingHorizontal = false;
    movingVertical = false;

    if (swapWeapon == -1)
    {
    }
    else
    {
    }
    glEnable(GL_CULL_FACE);

    playerCamera.update(deltaTime);
    playerControls();
    mouseControl();
    primary->updateViewmodel(playerCamera, pWindow, (footStepAcceleration * 15) / 14.0f, gravity.y, isGrounded);
}

void Player::playerControls()
{

    if (glfwGetKey(pWindow, GLFW_KEY_W))
    {
        movingVertical = true;
        if (isGrounded)
        {
            if (verticalSpeed < 0) { verticalSpeed /= (1 + (newDelta * 20)); }
            verticalSpeed += groundAcceleration * newDelta;
        }
        airVelocity.y += 10 * newDelta;
        airAcceleration += 35 * newDelta * MAXSPEED;
        lastSpeed += playerForward * airVelocity.y * MAXSPEED;
        if (lastSpeed != glm::vec3(0.0))
        {
            normalizedAirSpeed = glm::normalize(lastSpeed);
            if (u.oppositeDirection(1.0, 1.0, glm::vec2(normalizedAirSpeed.x, normalizedAirSpeed.z), glm::vec2(playerForward.x, playerForward.z)))
            {
                airAcceleration /= 1 + (newDelta * 20);
                MAXSPEED = airAcceleration / 15;
            }

        }
    }
    if (glfwGetKey(pWindow, GLFW_KEY_S))
    {
        movingVertical = true;
        if (isGrounded)
        {
            if (verticalSpeed > 0) { verticalSpeed /= (1 + (newDelta * 20)); }
            verticalSpeed -= groundAcceleration * newDelta;
        }
        airVelocity.y += 10 * newDelta;
        airAcceleration += 35 * newDelta * MAXSPEED;
        lastSpeed -= playerForward * airVelocity.y * MAXSPEED;
        if (lastSpeed != glm::vec3(0.0))
        {
            normalizedAirSpeed = glm::normalize(-lastSpeed);
            if (u.oppositeDirection(1.0, 1.0, glm::vec2(normalizedAirSpeed.x, normalizedAirSpeed.z), glm::vec2(playerForward.x, playerForward.z)))
            {
                airAcceleration /= 1 + (newDelta * 20);
                MAXSPEED = airAcceleration / 15;
            }

        }
    }

    if (glfwGetKey(pWindow, GLFW_KEY_A))
    {
        movingHorizontal = true;
        if (isGrounded)
        {
            if (horizontalSpeed > 0) { horizontalSpeed /= (1 + (newDelta * 20)); }
            horizontalSpeed -= groundAcceleration * newDelta;
        }
        airVelocity.x += 10 * newDelta;
        airAcceleration += 35 * newDelta * MAXSPEED;
        lastSpeed -= glm::cross(playerForward, glm::vec3(0.0, 1.0, 0.0)) * airVelocity.x * MAXSPEED;
        if (lastSpeed != glm::vec3(0.0))
        {
            normalizedAirSpeed = glm::normalize(-lastSpeed);
            glm::vec3 airStrafe = glm::cross(playerForward, glm::vec3(0.0, 1.0, 0.0));
            if (u.oppositeDirection(0.2, 0.2, glm::vec2(normalizedAirSpeed.x, normalizedAirSpeed.z), glm::vec2(airStrafe.x, airStrafe.z)))
            {
                airAcceleration /= 1 + (newDelta * 20);
                MAXSPEED = airAcceleration / 15;
            }
        }
    }
    if (glfwGetKey(pWindow, GLFW_KEY_D))
    {
        movingHorizontal = true;
        if (isGrounded)
        {
            if (horizontalSpeed < 0) { horizontalSpeed /= (1 + (newDelta * 20)); }
            horizontalSpeed += groundAcceleration * newDelta;
        }
        airVelocity.x += 10 * newDelta;
        airAcceleration += 35 * newDelta * MAXSPEED;
        lastSpeed += glm::cross(playerForward, glm::vec3(0.0, 1.0, 0.0)) * airVelocity.x * MAXSPEED;
        if (lastSpeed != glm::vec3(0.0))
        {
            normalizedAirSpeed = glm::normalize(lastSpeed);
            glm::vec3 airStrafe = glm::cross(playerForward, glm::vec3(0.0, 1.0, 0.0));
            if (u.oppositeDirection(0.2, 0.2, glm::vec2(normalizedAirSpeed.x, normalizedAirSpeed.z), glm::vec2(airStrafe.x, airStrafe.z)))
            {
                airAcceleration /= 1 + (newDelta * 20);
                MAXSPEED = airAcceleration / 15;
            }
        }
    }

    //SLOWS PLAYER DOWN WHEN NO INPUTS ARE PRESENT
    if (!movingVertical && isGrounded)
    {
        verticalSpeed /= (1 + (newDelta * 10));
    }
    if (!movingHorizontal && isGrounded)
    {
        horizontalSpeed /= (1 + (newDelta * 10));
    }

    //SPEED CAPS FOR GROUND MOVEMENT
    horizontalSpeed = glm::clamp(horizontalSpeed, -15.0f, 15.0f);
    if (u.checkBounds(-0.01, 0.01, horizontalSpeed)) { horizontalSpeed = 0; }
    verticalSpeed = glm::clamp(verticalSpeed, -15.0f, 15.0f);
    if (u.checkBounds(-0.01, 0.01, verticalSpeed)) { verticalSpeed = 0; }

    //SPEED CAPS FOR AIR MOVEMENT
    if (airAcceleration > 15) { airAcceleration = 15; }



    if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT))
    {
        playerCamera.speed = 13.5;
    }
    else
    {
        playerCamera.speed = 13.5;
    }

    //AUTO HOP JUMPING


    if (glfwGetKey(pWindow, GLFW_KEY_SPACE))
    {
        isJump = true;
    }
    else
    {
        isJump = false;
    }

    /*
    //SINGLE INPUT CONTROL FOR JUMPING
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE))
    {
        if (!latch)
        {
            isJump = true;
            latch = true;
        }
        else
        {
            isJump = false;
        }
    }
    else
    {
        latch = false;
        isJump = false;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_N))
    {
        swapWeapon *= -1;
    }
    */
    /*
    if (glfwGetKey(pWindow, GLFW_KEY_UP))
    {
        primary->viewPos.z += 0.1 * newDelta;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_DOWN))
    {
        primary->viewPos.z -= 0.1 * newDelta;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_RIGHT))
    {
        primary->viewPos.x += 0.1 * newDelta;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_LEFT))
    {
        primary->viewPos.x -= 0.1 * newDelta;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_L))
    {
        primary->viewPos.y += 0.1 * newDelta;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_M))
    {
        primary->viewPos.y -= 0.1 * newDelta;
    }
    std::cout << primary->viewPos.x << " " << primary->viewPos.y << " " << primary->viewPos.z  << "\n";
    */
}
void Player::mouseControl()
{
    GLdouble xPos, yPos;
    glfwGetCursorPos(pWindow, &xPos, &yPos);
    static double xoffsetS = 0.0f;
    static double yoffsetS = 0.0f;
    double sensitivity = 0.033f;
    double xpos = (xPos);
    double ypos = (yPos);
    xoffsetS = xpos - lastX;
    yoffsetS = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    xoffsetS *= sensitivity;
    yoffsetS *= sensitivity;
    playerCamera.camRot(xoffsetS, yoffsetS);
}