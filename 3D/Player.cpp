#include "Player.h"



Player::Player(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, GLFWwindow* window)
{
    //LOAD VIEWMODELS
    //primary = new Viewmodel(12, "Models/GUN/PEESTOLOLD.fbx");
    primary = new Viewmodel(8, "Models/GUN/BOLTON.fbx");
    scope = new Shader("SHADERS/framebuffer.vs", "SHADERS/crosshair.fs");
    //OVERLAY SHADER
    scope->use();
    scope->setFloat("x", SCR_WIDTH);
    scope->setFloat("y", SCR_HEIGHT);
	playerCamera.init(SCR_WIDTH, SCR_HEIGHT, 70);
	pWindow = window;
	playerCollider.init(capsule, sizeof(capsule) / sizeof(*capsule));
	floorCollider.init(floor, sizeof(floor) / sizeof(*floor));
    playerCamera.fov = 70;
    playerCamera.update();
    playerCamera.cameraPos = glm::vec3(0, 0, 0);
    screenX = SCR_WIDTH;
    screenY = SCR_HEIGHT;
    lastX = screenX / 2.0f;
    lastY = screenY / 2.0f;
    playerCamera.cameraFar = 2500;
    playerCamera.cameraNear = 0.1;
    playerPosition = playerCollider.pos;
    playerRotation = playerCollider.rot;
    
}

Utility u;
void Player::update(std::vector<MeshCollider>& collisionMap)
{
    //GETS INPUT AND SETS PLAYER FLAGS
    PlayerControls();
    PlayerFixedUpdate(collisionMap);



    //RESET POSITION OF PLAYER
    if (glfwGetKey(pWindow, GLFW_KEY_P))
    {
        gravity = glm::vec3(0);
        lastSpeed = glm::vec3(0.0);
        playerCollider.pos = glm::vec3(0, 5, 0);
    }
    //SETS MINIMUM AND MAXIMUM VALUES FOR AIMING DOWN SIGHTS
    scopedIn = glm::clamp(scopedIn, 0.0f, 1.0f);
    fovZoom = glm::clamp(fovZoom, 0.0f, 55.0f);
    primary->viewPos.x = glm::clamp(primary->viewPos.x, 4.07f, 4.85f);
    primary->viewPos.y = glm::clamp(primary->viewPos.y, -0.548f, -0.286f);
    primary->viewPos.z = glm::clamp(primary->viewPos.z, -0.405f, 0.443f);
    MAX_WALKING_SPEED = 15.0f * (1 - (scopedIn * 0.25));
    mouseSensitivity = 0.033f * (1 - (scopedIn * 0.5));
    
    //SETS CAMERA POSITION AND COLLIDER POSITIONS
    glm::vec3 interpolatedPos = Lerp(preFrame, fixedTimer / globalTimeStep, nextFrame);
    playerCamera.cameraPos = glm::vec3(interpolatedPos.x, interpolatedPos.y + 2.38 + (0.035 * (sin(primary->swayY))), interpolatedPos.z);
    floorCollider.setTransform(playerCollider.pos, glm::vec3(0.0));

    
    playerCamera.fov = 70.0 - fovZoom;
    playerCamera.camRot(mouseControl());
    playerCamera.update();
    primary->updateViewmodel(playerCamera, pWindow, (footStepAcceleration * 15) / 14.0f, gravity.y, isGrounded);
}

void Player::PlayerFixedUpdate(std::vector<MeshCollider>& collisionMap)
{
    //START FIXED UPDATE:
    // 
    for (int steps = iterations; steps > 0; --steps)
    {
        preFrame = playerCollider.pos;
        normalizedSpeed = glm::vec2(0);
        playerForward.x = cos(glm::radians(playerCamera.yaw));
        playerForward.z = sin(glm::radians(playerCamera.yaw));
        if (isScoped)
        {
            scopedIn += (5.0 * globalTimeStep);
            fovZoom += (300.0 * globalTimeStep);
            primary->viewPos.x -= (4 * globalTimeStep);
            primary->viewPos.y += (2 * globalTimeStep);
            primary->viewPos.z -= (5 * globalTimeStep);
            playerCamera.pitch += sin(glfwGetTime() * 1.8154f) * 0.35f * globalTimeStep;
            playerCamera.yaw += cos(glfwGetTime() * 1.045f) * 0.35f * globalTimeStep;
        }
        else
        {
            scopedIn -= (5.0 * globalTimeStep);
            fovZoom -= (300.0 * globalTimeStep);
            primary->viewPos.x += (6 * globalTimeStep);
            primary->viewPos.y -= (2 * globalTimeStep);
            primary->viewPos.z += (6 * globalTimeStep);
        }

        MoveVertical(forwardMotion);
        MoveHorizontal(horizontalMotion);

        //SPEED CAPS FOR GROUND MOVEMENT
        horizontalSpeed = glm::clamp(horizontalSpeed, -MAX_WALKING_SPEED, MAX_WALKING_SPEED);
        if (u.checkBounds(-0.01, 0.01, horizontalSpeed)) { horizontalSpeed = 0; }
        verticalSpeed = glm::clamp(verticalSpeed, -MAX_WALKING_SPEED, MAX_WALKING_SPEED);
        if (u.checkBounds(-0.01, 0.01, verticalSpeed)) { verticalSpeed = 0; }

        //SPEED CAPS FOR AIR MOVEMENT
        if (airAcceleration > 15) { airAcceleration = 15; }

        //SLOWS PLAYER DOWN WHEN NO MOVEMENT INPUTS ARE PRESENT AND PLAYER IS STANDING ON THE GROUND
        if (!movingVertical && isGrounded)
        {
            verticalSpeed /= (1 + (globalTimeStep * 10));
        }
        if (!movingHorizontal && isGrounded)
        {
            horizontalSpeed /= (1 + (globalTimeStep * 10));
        }

        //RESET POSITION IF OUT OF BOUNDS
        if (playerCollider.pos.y < -100)
        {
            playerCollider.setTransform(glm::vec3(0.0f, 5.0f, 0.0), glm::vec3(0.0));
            gravity = glm::vec3(0.0);
        }

        //GRAVITY CHECK
        if (isFalling)
        {
            playerCollider.moveCollider(glm::vec3(0.0, gravity.y * globalTimeStep, 0.0));
            if (gravity.y < 0)
            {
                fallAcceleration += (3.0 * globalTimeStep);
            }
            else
            {
                fallAcceleration = 1.0f;
            }
            gravity.y -= (30 * fallAcceleration * globalTimeStep);

            //CAPS SPEED FOR WHEN PLAYER IS FALLING
            if (gravity.y < -300) { gravity.y = -300; }
        }

        //PLAYER BEHAVIOR WHEN PLAYER IS ON THE GROUND
        if (isGrounded)
        {
            footStepAcceleration = (glm::clamp((double)(abs(verticalSpeed / 15) + abs(horizontalSpeed / 15)), 0.0, 1.0));
            footstepSpeed = 1.0 - (footStepAcceleration * 0.6);

            //KEEPS VALUE SMALL ENOUGH FOR AIR STRAFE TO BE MORE CONTROLLABLE
            if (lastSpeed != glm::vec3(0)) { lastSpeed /= 1 + (globalTimeStep * 350); }
            if (movingHorizontal && movingVertical) { groundAcceleration = 105; }
            else { groundAcceleration = 70; }


            //PLAY WALKING SOUNDS
            stepTimer += globalTimeStep;
            if ((abs(verticalSpeed / MAX_WALKING_SPEED) >= 0.5 || abs(horizontalSpeed / MAX_WALKING_SPEED) >= 0.5) && stepTimer > footstepSpeed)
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
            playerCollider.moveCollider(glm::vec3(normalizedSpeed.x * globalTimeStep, 0.0, normalizedSpeed.y * globalTimeStep));
            playerCollider.moveCollider(glm::vec3(momentum.x * globalTimeStep, 0.0, momentum.y * globalTimeStep));
            momentum /= 1 + (globalTimeStep * 10);

            //KEEPS PLAYER MOMENTUM WHEN JUMPING AS LONG AS NEXT JUMP IS FAST ENOUGH
            airMomentumTimer += globalTimeStep;
            if (airMomentumTimer > 0.025f)
            {
                airVelocity = glm::vec2(0);
                lastSpeed = glm::vec3(0);
                MAXSPEED = 1.0;
                if (u.checkBounds(-1, 1, horizontalSpeed) && u.checkBounds(-1, 1, verticalSpeed)) { airAcceleration /= 1 + (globalTimeStep * 10); }
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
            playerCollider.moveCollider(glm::vec3(normalAirVector.x * airAcceleration * MAXSPEED * globalTimeStep, 0.0, normalAirVector.y * airAcceleration * MAXSPEED * globalTimeStep));
        }

        //RESETS PLAYER MOVMENT FLAG TO FALSE
        movingHorizontal = false;
        movingVertical = false;




        //COLLISION CHECKS
        ResolutionData playerCollisionData;
        isFalling = false;
        isGrounded = false;
        for (int i = 0; i < collisionMap.size(); ++i)
        {
            playerCollisionData = GJK(floorCollider, collisionMap[i], false);
            if (playerCollisionData.hasCollision && !isJump)
            {
                gravity = glm::vec3(0.0);

            }
            if (playerCollisionData.hasCollision)
            {
                isGrounded = true;
                if (fallAcceleration > 1.0)
                {
                    float randomValue = ((((double)(rand() % 21)) / 100)) + 0.9f;
                    playerSpeaker.p_Pitch = randomValue * 0.9;
                    playerSpeaker.p_Gain = randomValue;
                    playerSpeaker.Play(sound1);
                    fallAcceleration = 1.0f;
                }
                if (isJump)
                {
                    gravity.y = 11;
                }
            }
            else
            {
                isFalling = true;
            }
            playerCollisionData = GJK(playerCollider, collisionMap[i], true);
            if (playerCollisionData.hasCollision)
            {
                //COLLISION RESOLUTION FOR HITTING THE WALL IN AIR
                if (lastSpeed != glm::vec3(0.0) && !movingHorizontal && !movingVertical)
                {
                    lastSpeed = (glm::normalize(-playerCollisionData.Normal) + (lastSpeed * 0.4f));
                    airAcceleration /= 1 + (globalTimeStep * 80);

                }
                normalizedSpeed = glm::vec3(normalizedSpeed.x, 0, normalizedSpeed.y) - playerCollisionData.Normal;
            }
        }
        nextFrame = playerCollider.pos;
    }
    //END FIXED UPDATE
}

//RENDERS SCREEN OVERLAY FOR WEAPON SCOPE
void Player::renderOverlay(Shader &shader)
{
    scope->use();
    scope->setFloat("isScoped", scopedIn);
    scope->setFloat("xSway", (7.5 * (sin(primary->swayX))) + ((-primary->totalAMT_X * 350) / (1 - deltaTime)));
    scope->setFloat("ySway", (7.5 * (sin(primary->swayY))) + ((-primary->totalAMT_Y * 350) / (1 - deltaTime)));

    renderQuad();
}

//MOVES PLAYER LEFT AND RIGHT
void Player::MoveHorizontal(int8_t direction)
{
    if (direction == 0)
    {
        return;
    }
    movingHorizontal = true;
    if (isGrounded)
    {
        if (direction > 0)
        {
            if (horizontalSpeed < 0) { horizontalSpeed /= (1 + (globalTimeStep * 20)); }
            horizontalSpeed += groundAcceleration * globalTimeStep;
        }
        else
        {
            if (horizontalSpeed > 0) { horizontalSpeed /= (1 + (globalTimeStep * 20)); }
            horizontalSpeed -= groundAcceleration * globalTimeStep;
        }
    }
    airVelocity.x += 10 * globalTimeStep;
    airAcceleration += 35 * globalTimeStep * MAXSPEED;
    lastSpeed += glm::cross(playerForward, glm::vec3(0.0, 1.0, 0.0)) * airVelocity.x * (MAXSPEED * direction);
    if (lastSpeed != glm::vec3(0.0))
    {
        normalizedAirSpeed = glm::normalize(lastSpeed * glm::vec3(direction));
        glm::vec3 airStrafe = glm::cross(playerForward, glm::vec3(0.0, 1.0, 0.0));
        if (u.oppositeDirection(0.2, 0.2, glm::vec2(normalizedAirSpeed.x, normalizedAirSpeed.z), glm::vec2(airStrafe.x, airStrafe.z)))
        {
            airAcceleration /= 1 + (globalTimeStep * 20);
            MAXSPEED = airAcceleration / 15;
        }
    }
}

//MOVES PLAYER FORWARD AND BACK
void Player::MoveVertical(int8_t direction)
{
    if (direction == 0)
    {
        return;
    }
    movingVertical = true;
    if (isGrounded)
    {
        if (direction > 0)
        {
            if (verticalSpeed < 0) { verticalSpeed /= (1 + (globalTimeStep * 20)); }
            verticalSpeed += (groundAcceleration * globalTimeStep);
        }
        else
        {
            if (verticalSpeed > 0) { verticalSpeed /= (1 + (globalTimeStep * 20)); }
            verticalSpeed -= (groundAcceleration * globalTimeStep);
        }
    }
    airVelocity.y += 10 * globalTimeStep;
    airAcceleration += 35 * globalTimeStep * MAXSPEED;
    lastSpeed += playerForward * airVelocity.y * (MAXSPEED * direction);
    if (lastSpeed != glm::vec3(0.0))
    {
        normalizedAirSpeed = glm::normalize(lastSpeed * glm::vec3(direction));
        if (u.oppositeDirection(1.0, 1.0, glm::vec2(normalizedAirSpeed.x, normalizedAirSpeed.z), glm::vec2(playerForward.x, playerForward.z)))
        {
            airAcceleration /= 1 + (globalTimeStep * 20);
            MAXSPEED = airAcceleration / 15;
        }

    }
}


//PERFORMS ALL OF THE PLAYERS ACTIONS SUCH AS MOVING AND SHOOTING
void Player::PlayerControls()
{
    //AIMS DOWN SIGHTS WHEN BUTTON IS PRESSED
    if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_2) && primary->thisTimer < 0.155 && primary->thisAnim != 6 && primary->thisAnim != 5)
    {
        isScoped = true;

       
    }
    else
    {
        isScoped = false;


    }

    //MOVE PLAYER WITH KEYPRESS
    if (glfwGetKey(pWindow, GLFW_KEY_W))
    {
        forwardMotion = 1;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_S))
    {
        forwardMotion = -1;
    }

    if ((glfwGetKey(pWindow, GLFW_KEY_W) && glfwGetKey(pWindow, GLFW_KEY_S)) || (!glfwGetKey(pWindow, GLFW_KEY_W) && !glfwGetKey(pWindow, GLFW_KEY_S)))
    {
        forwardMotion = 0;
    }

    if (glfwGetKey(pWindow, GLFW_KEY_A))
    {
        horizontalMotion = -1;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_D))
    {
        horizontalMotion = 1;
    }
    
    if ((glfwGetKey(pWindow, GLFW_KEY_A) && glfwGetKey(pWindow, GLFW_KEY_D)) || (!glfwGetKey(pWindow, GLFW_KEY_A) && !glfwGetKey(pWindow, GLFW_KEY_D)))
    {
        horizontalMotion = 0;
    }




    //SET FLAG FOR PLAYER ATTEMPTING TO JUMP
    //AUTO HOP JUMPING
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE))
    {
        isJump = true;
    }
    else
    {
        isJump = false;
    }

    //SINGLE INPUT CONTROL FOR JUMPING
    /*
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
    
}


//MOVES VIEWMODEL BASED ON KEYPRESS
void Player::MoveViewModel()
{
    if (glfwGetKey(pWindow, GLFW_KEY_UP))
    {
        primary->viewPos.z += 0.1 * globalTimeStep;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_DOWN))
    {
        primary->viewPos.z -= 0.1 * globalTimeStep;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_RIGHT))
    {
        primary->viewPos.x += 0.1 * globalTimeStep;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_LEFT))
    {
        primary->viewPos.x -= 0.1 * globalTimeStep;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_L))
    {
        primary->viewPos.y += 0.1 * globalTimeStep;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_M))
    {
        primary->viewPos.y -= 0.1 * globalTimeStep;
    }
    std::cout << primary->viewPos.x << " " << primary->viewPos.y << " " << primary->viewPos.z  << "\n";
}

glm::vec2 Player::mouseControl()
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
    xoffsetS *= mouseSensitivity;
    yoffsetS *= mouseSensitivity;
    return glm::vec2(xoffsetS, yoffsetS);
}