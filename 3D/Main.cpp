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

unsigned int loadTexture(char const* path);

void initShadowMap();
void staticRender(Shader& shader, Model m, float xR);

void renderScene(const Shader& shader);
void renderCube();
void renderQuad();

// RESOLUTION
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;




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



unsigned int planeVAO;

int main()
{
   
    // glfw window creation
    // --------------------
    GLFWwindow* window = init();
    glEnable(GL_DEPTH_TEST);
    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("SHADERS/color.vs", "SHADERS/color.fs");
    Shader staticShader("SHADERS/static.vs", "SHADERS/static.fs");
    Shader depthShader("SHADERS/depth.vs", "SHADERS/depth.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);


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

    //ourShader.use();
    //fBuffer.use();
    //fBuffer.setInt("screenTexture", 0);
    // framebuffer configuration
    // -------------------------
    
  

    float xL = 0;
    Model map("Models/DUST2/source/shit.fbx");
    Model thingy("Models/DUST2/source/THINGY.fbx");
    

    unsigned int woodTexture = loadTexture("TEXTURES/wood.png");
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    staticShader.use();
    staticShader.setInt("diffuseTexture", 0);
    staticShader.setInt("shadowMap", 1);
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        xL += 20.0 * deltaTime;
        
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
        
       
       
       
       


        
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now      
        
        //UPDATE CAMERA POSITIONS
        c.update();
        
        /*
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(c.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
        glm::mat4 view = c.view;
        glm::mat3 normal;
        */
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
        }
        if (glfwGetKey(window, GLFW_KEY_8))
        {
            reset = 1;
            thisAnim = 7;
        }
        if (glfwGetKey(window, GLFW_KEY_0))
        {
            reset = 1;
            thisAnim = 9;
        }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(c.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = c.view;
        glm::mat4 normal;

        /**/
        ourShader.use();
        ourShader.setVec3("viewPos", c.cameraPos);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //DIR LIGHT
        ourShader.setFloat("material.shininess", 1.0f);
        ourShader.setFloat("material2.shininess", 1.0f);
        ourShader.setVec3("dirLight.direction", 0.1f, -0.7f, 1.0f);
        ourShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("dirLight.diffuse", 0.33f, 0.33f, 0.33f);
        ourShader.setVec3("dirLight.specular", 0.15f, 0.15f, 0.15f);
        
       
        
        //CALCULATE BONE TRANSFORM
        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
            
        }
        //INITIALIZE OBJECT ORIENTATIONS
        glm::mat4 animOBJ = glm::mat4(1.0f);
        animOBJ = glm::inverse(animOBJ) * glm::inverse(c.view);
        animOBJ = glm::scale(animOBJ, glm::vec3(0.1f, 0.1f, 0.1f));
        animOBJ = glm::rotate(animOBJ, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
        animOBJ = glm::translate(animOBJ, glm::vec3(x, y + shakeX, z + shakeY));
        
        //SEND OBJECT DATA TO SHADER AND DRAW
        ourShader.setMat4("model", animOBJ);
        normal = glm::mat3(glm::transpose(glm::inverse(animOBJ)));
        ourShader.setMat3("inverse", normal);
        
        anim.draw(ourShader);
        
        

        //STATIC OBJECT RENDER
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(5.0, 1.0, 10.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        renderScene(depthShader);
        staticRender(depthShader, thingy, xL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        

        // 2. render scene as normal using the generated depth/shadow map  
        // --------------------------------------------------------------
        staticShader.use();
        
        staticShader.setMat4("projection", projection);
        staticShader.setMat4("view", view);
        // set light uniforms
        staticShader.setVec3("viewPos", c.cameraPos);
        staticShader.setVec3("lightPos", lightPos);
        staticShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderScene(staticShader);
        staticRender(staticShader, thingy, xL);








        




        
        /*
        */


 

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

 

         
         
         

    }

    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    
    glfwTerminate();
    return 0;
}







//renders a model without animation
void staticRender(Shader &shader, Model m, float xR)
{
    
    glm::mat3 normal;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 0.5f, 3.0f));
    model = glm::rotate(model,  90 * 0.0174533f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, (xR + 90) * 0.0174533f, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
    shader.setMat4("model", model);
    normal = glm::mat3(glm::transpose(glm::inverse(model)));
    shader.setMat3("inverse", normal);
    m.draw(shader);

}


void initShadowMap()
{
    /*
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */
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




// renders the 3D scene
// --------------------
void renderScene(const Shader& shader)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    renderCube();
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}