#include "Player.h"

Player::Player(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, GLFWwindow* window)
{
	primary = new Viewmodel(8, "Models/GUN/BOLTON.fbx");
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

void Player::update(float deltaTime, Shader& shader)
{

    if (playerCamera.cameraCollider.pos.y < -100)
    {
        playerCamera.cameraCollider.setTransform(glm::vec3(100.0f, 105.0f, 100.0), glm::vec3(0.0));
        gravity = glm::vec3(0.0);
    }

    /*
    isFalling = false;
    isGrounded = false;

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
    if (r.hasCollision)
    {
        isFalling = false;
    }
    else
    {
        isFalling = true;
    }
    */
    if (isFalling)
    {
        playerCamera.cameraCollider.moveCollider(glm::vec3(0.0, gravity.y * deltaTime, 0.0));

        gravity.y -= (20 * deltaTime);
        if (gravity.y < -300)
        {
            gravity.y = -300;
        }
    }
    if (glfwGetKey(pWindow, GLFW_KEY_P))
    {
        gravity = glm::vec3(0);
        playerCamera.cameraCollider.pos = glm::vec3(100, 105, 100);
    }

    
    if (isGrounded)
    {
        if (normalizedSpeed != glm::vec2(0))
        {
            momentum = normalizedSpeed / 1.5f;
        }
        momentum /= (1 + (deltaTime * 25));
        playerCamera.cameraCollider.moveCollider(glm::vec3((momentum.x) * deltaTime * (playerCamera.speed), 0.0, (momentum.y) * deltaTime * playerCamera.speed));
        lastSpeed = glm::vec3(0); 
        normalizedSpeed = glm::vec2(0);
    }
    if (!isGrounded)
    {
        if (lastSpeed.x != 0 && lastSpeed.z != 0)
        {
            normalizedSpeed = glm::normalize(glm::vec2(lastSpeed.x, lastSpeed.z));
        }

        playerCamera.cameraCollider.moveCollider(glm::vec3((normalizedSpeed.x) * deltaTime * (playerCamera.speed), 0.0, (normalizedSpeed.y) * deltaTime * playerCamera.speed));
    }

    glDisable(GL_CULL_FACE);
    primary->render(playerCamera, shader, pWindow);
    glEnable(GL_CULL_FACE);
   
    playerCamera.fov = 70;
    playerCamera.update(deltaTime);
    playerControls();
    mouseControl();
}
const float speed = 10.0f;
void Player::playerControls()
{
    if (glfwGetKey(pWindow, GLFW_KEY_W))
    {
        lastSpeed += (playerCamera.forw);
        if (isGrounded)
        {
            playerCamera.forward();
        }
    }
    if (glfwGetKey(pWindow, GLFW_KEY_A))
    {
        lastSpeed -= (glm::cross(playerCamera.forw, playerCamera.cameraUp));
        if (isGrounded)
        {
            playerCamera.left();
        }
    }
    if (glfwGetKey(pWindow, GLFW_KEY_S))
    {
        lastSpeed -= (playerCamera.forw);
        if (isGrounded)
        {
           playerCamera.back();
        }
    }
    if (glfwGetKey(pWindow, GLFW_KEY_D))
    {
        lastSpeed += (glm::cross(playerCamera.forw, playerCamera.cameraUp));
        if (isGrounded)
        {
            playerCamera.right();
        }
    }
    if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT))
    {
        playerCamera.speed = 1.35 * speed;
    }
    else
    {
        playerCamera.speed = 1.35 * speed;
    }
    
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE))
    {
        isJump = true;


    }
    else
    {
        isJump = false;
    }

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