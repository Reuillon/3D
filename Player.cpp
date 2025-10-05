#include "Player.h"

Player::Player(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, GLFWwindow* window)
{
    //LOAD VIEWMODELS
	primary = new Viewmodel(8, "Models/GUN/BOLTON.fbx");
    secondary = new Viewmodel(12, "Models/GUN/PEESTOL.fbx");
    
    //primary = new Viewmodel(11, "Models/GUN/BS2.fbx");
    //primary = new Viewmodel(11, "Models/GUN/DEGGLETMP.fbx");

	playerCamera.init(SCR_WIDTH, SCR_HEIGHT, 52);
	pWindow = window;
	playerCollider.init(capsule, sizeof(capsule) / sizeof(*capsule));
	floorCollider.init(floor, sizeof(floor) / sizeof(*floor));
    playerCamera.fov = 70;
    playerCamera.update(0.001);
    playerCamera.cameraPos = glm::vec3(0, 0, 0);
    screenX = SCR_WIDTH;
    screenY = SCR_HEIGHT;
    lastX = screenX / 2.0f;
    lastY = screenY / 2.0f;
}

int swapWeapon = -1;
float rate = 5000;
Debug debugger;
void Player::update(float deltaTime, Shader& shader, std::vector<MeshCollider>& collisionMap)
{
    newDelta = deltaTime;
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
        playerCollider.setTransform(glm::vec3(100.0f, 105.0f, 100.0), glm::vec3(0.0));
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
        playerCollider.pos = glm::vec3(100, 105, 100);
    }

    //CHECK IF PLAYER IS ON THE GROUND
    if (isGrounded)
    {
        normalizedSpeed = glm::vec2(0);
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
     
        
        //KEEPS PLAYER MOMENTUM WHEN JUMPING AS LONG AS NEXT JUMP IS FAST ENOUGH
        airMomentumTimer += deltaTime;

        if (airMomentumTimer > 0.05f)
        {
            lastSpeed = glm::vec3(0.0f);
        }
        lastSpeed = glm::vec3(normalizedSpeed.x , 0 , normalizedSpeed.y);
  
        
        
        
        normalizedSpeed = glm::vec2(0);

    }
    else
    {
        airMomentumTimer = 0.0;
        playerCollider.moveCollider(glm::vec3(lastSpeed.x, 0.0, lastSpeed.z));

    }

  


    playerCamera.cameraPos = glm::vec3(playerCollider.pos.x, playerCollider.pos.y + 1.5, playerCollider.pos.z);
    floorCollider.setTransform(playerCollider.pos, glm::vec3(0.0));
    groundVelocity = glm::vec3(0.0);
    movingHorizontal = false;
    movingVertical = false;

    glDisable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
    primary->render(playerCamera, shader, pWindow);
    //secondary->render(playerCamera, shader, pWindow);

    if (swapWeapon == -1)
    {
    }
    else
    {
    }
    glEnable(GL_CULL_FACE);
   
    playerCamera.fov = 70;
    playerCamera.update(deltaTime);
    playerControls();
    mouseControl();
}

const float speed = 2.0f;
void Player::playerControls()
{

    
    if (glfwGetKey(pWindow, GLFW_KEY_W))
    {
        if (isGrounded)
        {
            movingVertical = true;
            verticalSpeed += 50 * newDelta;
        }
    }
    if (glfwGetKey(pWindow, GLFW_KEY_A))
    {
        if (isGrounded)
        {
            movingHorizontal = true;
            horizontalSpeed -= 50 * newDelta;
        }

    }
    if (glfwGetKey(pWindow, GLFW_KEY_S))
    {
        if (isGrounded)
        {
            movingVertical = true;
            verticalSpeed -= 50 * newDelta;
        }

    }
    if (glfwGetKey(pWindow, GLFW_KEY_D))
    {
        if (isGrounded)
        {
            movingHorizontal = true;
            horizontalSpeed += 50 * newDelta;
        }
 
    }

    
    if (!movingVertical && isGrounded)
    {
        verticalSpeed /= (1 + (newDelta * 20));
    }
    if (!movingHorizontal && isGrounded)
    {
        horizontalSpeed /= (1 + (newDelta * 20));
    }
    if (horizontalSpeed > 15)
    {
        horizontalSpeed = 15;
    }
    else if (horizontalSpeed < -15)
    {
        horizontalSpeed = -15;
    }
    else if (horizontalSpeed > -0.001 && horizontalSpeed < 0.001)
    {
        horizontalSpeed = 0;
    }
    if (verticalSpeed > 15)
    {
        verticalSpeed = 15;
    }
    else if (verticalSpeed < -15)
    {
        verticalSpeed = -15;
    }
    else if (verticalSpeed > -0.001 && verticalSpeed < 0.001)
    {
        verticalSpeed = 0;
    }
    if (movingHorizontal && movingVertical)
    {
    }


    if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT))
    {
        playerCamera.speed = 13.5 * speed;
    }
    else
    {
        playerCamera.speed = 13.5 * speed;
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

}
void Player::mouseControl()
{
    GLdouble xPos, yPos;
    glfwGetCursorPos(pWindow, &xPos, &yPos);
    static double xoffsetS = 0.0f;
    static double yoffsetS = 0.0f;
    double sensitivity = 0.05f;
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