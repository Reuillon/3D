//STANDARD LIBRARY
#include <iostream>


//MY CLASSES
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"

#include "Animator.h"
#include "Model.h"

#include "Level.h"

//OPENGL LIBRARIES
#include <GLFW/glfw3.h>

//PRE DEFINED FUNCTIONS FOR GLFW INPUT
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);


// RESOLUTION
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

//FRAMEBUFFER VARIABLES
unsigned int framebuffer;
unsigned int textureColorbuffer;
unsigned int rbo;
unsigned int quadVAO, quadVBO;
float quadVertices[] =
{ // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
     // positions   // texCoords
     -1.0f,  1.0f,  0.0f, 1.0f,
     -1.0f, -1.0f,  0.0f, 0.0f,
      1.0f, -1.0f,  1.0f, 0.0f,

     -1.0f,  1.0f,  0.0f, 1.0f,
      1.0f, -1.0f,  1.0f, 0.0f,
      1.0f,  1.0f,  1.0f, 1.0f
};


camera c(SCR_WIDTH, SCR_HEIGHT, 52);

float timer = 0;
GLFWwindow* init()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RENDERER", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
   //stbi_set_flip_vertically_on_load(true);
    //ENABLE DEPTHBUFFER
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //VERTEX ARRAY OBJECT FRAMEBUFFER
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);  
    glFrontFace(GL_CCW);
    */
    
    return window;
}
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//DELTATIME VALUES
double deltaTime = 0.0f;
float lastFrame = 0.0f;



//VIEWMODEL POSITIONING AND BEHAVIOUR
double xoffset = 0.0f;
double yoffset = 0.0f;
float x = 4.68005f, y = -0.52, z = 0.1333;
double xBand = 0.0f;
double yBand = 0.0f;
float shakeX = 0.0f;
float shakeY = 0.0f;


double sensitivity = 0.05f;

//ANIMATION CONTROLLER
int thisAnim = 4;
int playAnim = 1;
int reset = 0;





int main()
{
   
    // glfw window creation
    // --------------------
    GLFWwindow* window = init();

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("SHADERS/color.vs", "SHADERS/color.fs");
    Shader fBuffer("SHADERS/framebuffer.vs", "SHADERS/framebuffer.fs");
    

    //MODEL ANIMATIONS
    Model anim("Models/GUN/PEESTOLOLD.fbx");
    Animation rot("Models/GUN/PEESTOL.fbx", &anim, 0);
    Animation rot1("Models/GUN/PEESTOL.fbx", &anim, 1);
    Animation rot2("Models/GUN/PEESTOL.fbx", &anim, 2);
    Animation rot3("Models/GUN/PEESTOL.fbx", &anim, 3);
    Animation rot4("Models/GUN/PEESTOL.fbx", &anim, 4);
    Animation rot5("Models/GUN/PEESTOL.fbx", &anim, 5);
    Animation rot6("Models/GUN/PEESTOL.fbx", &anim, 6);
    Animation rot7("Models/GUN/PEESTOL.fbx", &anim, 7);
    Animation rot8("Models/GUN/PEESTOL.fbx", &anim, 8);
    Animation rot9("Models/GUN/PEESTOL.fbx", &anim, 9);
    Animation rot10("Models/GUN/PEESTOL.fbx", &anim, 12);


    
    
    

  






    //FRAME TIMER
    double currtime;
    double cutoff = 0.175f;
    



  

    Animator animator(&rot);
    animator.loopAnim(false);
    //////
    //////
    //////

    ourShader.use();
    fBuffer.use();
    fBuffer.setInt("screenTexture", 0);
    // framebuffer configuration
    // -------------------------
    
    

    float xL = 0;
    

    Model map("Models/DUST2/source/shit.fbx");
    Animation mapAnim("Models/DUST2/source/shit.fbx", &map, 0);
    Animator mapAnmtr(&mapAnim);
   
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        
        
        
        //SELECTS ANIMATION BASED ON WHICH VALUE THISANIM IS SET TO
        switch (thisAnim)
        {

            case 0:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot10);
                break;
            }
            
            case 1:
            {
                animator.loopAnim(true);
                animator.PlayAnimation(&rot1);
                break;
            }
            case 2:
            {
                animator.loopAnim(true);
                animator.PlayAnimation(&rot2);
                break;
            }
            case 3:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot3);
                break;
            }
            case 4:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot4);
                break;
            } case 5:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot5);
                break;
            } case 6:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot6);
                break;
            } case 7:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot7);
                break;
            } case 8:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot8);
                break;
            }case 9:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot9);
                break;
            }
            

           
            default:
            {
                animator.loopAnim(false);
                animator.PlayAnimation(&rot1);
                break;
            }
        }

        //SIMPLE ANIMATION CONTROLLER
        if (playAnim == 1)
        {
            animator.UpdateAnimation(deltaTime);
        }
        else
        {
            animator.PauseAnim();
        }
        if (animator.finishedAnim() == true)
        {
            animator.ResetAnim();
            thisAnim = 2;
        }
        if (reset == 1)
        {
            animator.ResetAnim();
            reset = 0;
        }
       


        //VIEWMODEL INIT
        //std::cout << xBand << "\n";
        xBand = xoffset;
        yBand = yoffset;
        
        // z = -0.136999;
        // z = -0.136999 - (xBand * 0.02 * sensitivity * 20);
        // y = -0.361998 - (yBand * 0.025 * sensitivity *20);
        // y = -0.361998;


        //DELTA TIME CALCULATION
        currtime = glfwGetTime();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        // input
        // -----
        processInput(window);
        
       
        //UPDATE CAMERA POSITIONS
        c.update();
        ourShader.use();
        ourShader.setVec3("viewPos", c.cameraPos);
       


        
        
      

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(c.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
        glm::mat4 view = c.view;
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        ///ANIMATION CONTROLS
        ///
        /// 
        ///
        {

         

        if (glfwGetKey(window, GLFW_KEY_1))
        {
            reset = 1;
            thisAnim = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_2))
        {
            reset = 1;
            thisAnim = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_3))
        {
            reset = 1;
            thisAnim = 2;
        }
        if (glfwGetKey(window, GLFW_KEY_4))
        {
            reset = 1;
            thisAnim = 3;
        }
        if (glfwGetKey(window, GLFW_KEY_5))
        {
            reset = 1;
            thisAnim = 4;
        }
        if (glfwGetKey(window, GLFW_KEY_6))
        {
            reset = 1;
            thisAnim = 5;
        }
        if (glfwGetKey(window, GLFW_KEY_7))
        {
            reset = 1;
            thisAnim = 6;
        }if (glfwGetKey(window, GLFW_KEY_8))
        {
            reset = 1;
            thisAnim = 7;
        }if (glfwGetKey(window, GLFW_KEY_0))
        {
            reset = 1;
            thisAnim = 9;
        }
        }

      

        //DIR LIGHT
        ourShader.setFloat("material.shininess", 1.0f);
        ourShader.setFloat("material2.shininess", 1.0f);
       

        // point light 1
        ourShader.setVec3("pointLights[0].position", 0.0f, 1.0f, 0.0f);
        ourShader.setVec3("pointLights[0].ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("pointLights[0].diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 2.0f);
        ourShader.setFloat("pointLights[0].linear", 0.25f);
        ourShader.setFloat("pointLights[0].quadratic", 0.1f);

        /*
        * 
        * 
        ourShader.setVec3("dirLight.direction", 3.5f, -2.0f, 1.0f);
        ourShader.setVec3("dirLight.ambient", 0.33f, 0.33f, 0.33f);
        ourShader.setVec3("dirLight.diffuse", 1.0f *0.25f + 0.3f , 0.64f * 0.25f + 0.3f, 0.0f + 0.3f);
        ourShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
        */
        //INITIALIZE DIRECTIONAL LIGHT
        xL += 0.05f * deltaTime;

        if (xL > 1.0f)
        {
            xL = -1.0f;
        }

        /**/
        ourShader.setVec3("dirLight.direction", 0.1f, -0.7f, xL);
        ourShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("dirLight.diffuse", 0.33f, 0.33f, 0.33f);
        ourShader.setVec3("dirLight.specular", 0.15f, 0.15f, 0.15f);
        
        glm::mat3 normal;
        
        //CALCULATE BONE TRANSFORM
        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
            
        }
        
        //first pass
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        
        glEnable(GL_DEPTH_TEST);
        
        //INITIALIZE OBJECT ORIENTATIONS
        glm::mat4 animOBJ = glm::mat4(1.0f);
        animOBJ = glm::inverse(animOBJ) * glm::inverse(c.view);
        animOBJ = glm::scale(animOBJ, glm::vec3(0.1f, 0.1f, 0.1f));
        animOBJ = glm::rotate(animOBJ, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
        animOBJ = glm::translate(animOBJ, glm::vec3(x, y + shakeX, z + shakeY));
      
        //ENABLE DEPTHBUFFER
        
        //SEND OBJECT DATA TO SHADER AND DRAW
        ourShader.setMat4("model", animOBJ);
        normal = glm::mat3(glm::transpose(glm::inverse(animOBJ)));
        ourShader.setMat3("inverse", normal);
        
        anim.draw(ourShader);

        

        //STATIC OBJECT
        transforms = mapAnmtr.GetFinalBoneMatrices();
        ourShader.setMat4("finalBonesMatrices[" + std::to_string(0) + "]", transforms[0]);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(18.0f, -9.0f, 0.0f));
        model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
        ourShader.setMat4("model", model);
        normal = glm::mat3(glm::transpose(glm::inverse(model)));
        ourShader.setMat3("inverse", normal);
        map.draw(ourShader);
        
        ///
        ///
        ///
        /// 
        /// 
        /// 

        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        fBuffer.use();
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        //PRINT FRAMERATE
        timer += deltaTime;
        if (timer > 0.25f)
        {
            std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << (int)(1000 / ((glfwGetTime() - currtime) * 1000)) << " FPS\n";
            timer = 0;
        }
        //std::cout << c.fov << " " << x << " " << y << " " << z << "\n\n\n\n\n\n";



        /// VIEWMODEL POSITIONING
        /// 
        /// 
        /// 
        /*
        if (glfwGetKey(window, GLFW_KEY_UP))
        {
            y += 0.001f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN))
        {
            y -= 0.001f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT))
        {
            x += 0.001f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT))
        {
            x -= 0.001f;
        }
        if (glfwGetKey(window, GLFW_KEY_E))
        {
            z += 0.001f;
        }
        if (glfwGetKey(window, GLFW_KEY_F))
        {
            z -= 0.001f;
        }
        */

        /*
         ourShader.setVec3("spotLight.position", c.cameraPos);
        ourShader.setVec3("spotLight.direction", glm::vec3(c.front.x , c.front.y + 0.1 + shakeX * 2, c.front.z+ -shakeY * 4));
        ourShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));
        */

         
         
         

    }

    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);
    glfwTerminate();
    return 0;
}







// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


//CONTINOUS INPUT
float modi = 1;
float swayT = 0;
void processInput(GLFWwindow* window)
{



    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }
    const float speed = 6.5f;


    swayT += 1 * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W))
    {
        c.forward(modi);
        shakeX = (1 * (glm::sin(swayT * 15)) * 0.005);
        shakeY = (1 * (glm::sin(swayT * 7.5)) * 0.005);
    }
    if (glfwGetKey(window, GLFW_KEY_A))
    {
        c.left(modi);
        shakeX = (1 * (glm::sin(swayT * 15)) * 0.005);
        shakeY = (1 * (glm::sin(swayT * 7.5)) * 0.005);
    }
    if (glfwGetKey(window, GLFW_KEY_S))
    {
        c.back(modi);
        shakeX = (1 * (glm::sin(swayT * 15)) * 0.005);
        shakeY = (1 * (glm::sin(swayT * 7.5)) * 0.005);
    }
    if (glfwGetKey(window, GLFW_KEY_D))
    {
        c.right(modi);
        shakeX = (1 * (glm::sin(swayT * 15)) * 0.005);
        shakeY = (1 * (glm::sin(swayT * 7.5)) * 0.005);
    }
    if (!glfwGetKey(window, GLFW_KEY_S) && !glfwGetKey(window, GLFW_KEY_W) && !glfwGetKey(window, GLFW_KEY_D) && !glfwGetKey(window, GLFW_KEY_A))
    {
        swayT = 0;
        if (shakeX > 0.01f)
        {
            shakeX /= (1.1f * deltaTime);
        }
        if (shakeY > 0.01f)
        {
            shakeY /= (1.1f * deltaTime);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Q))
    {
        c.crouch(speed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE))
    {
        c.jump(speed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
    {
        modi = 4 * speed * deltaTime;
    }
    else
    {
        modi = 1 * speed * deltaTime;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4))
    {
        c.crouch(speed * deltaTime);
    }


}


//MOUSE MOVEMENT
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    /**/
    double xpos = (xposIn);
    double ypos = (yposIn);
    xoffset = xpos - lastX;
    yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    lastX = xpos;
    lastY = ypos;

    c.camRot(xoffset, yoffset);
}


//SCROLL INPUT
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    c.fovMod((float)yoffset);
}

//KEY SINGLE INPUT 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        playAnim *= -1;

    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        reset = 1;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        reset = 1;
        thisAnim = 7;
    }
}


//MOUSE SINGLE INPUT
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        reset = 1;
        thisAnim = 9;
    }
}