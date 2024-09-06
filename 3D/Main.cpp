//STANDARD LIBRARY
#include <iostream>

//MY CLASSES
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Viewmodel.h"
#include "Animator.h"
#include "Model.h"

#include "Level.h"

//OPENGL LIBRARIES
#include <GLFW/glfw3.h>

//GLFW INPUT FUNCTIONS
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);


//INITIALIZE OBJECTS
void renderCube();
void renderQuad();
void initShadowMap();


//RENDER FUNCTIONS
void lightCube(const Shader& shader);
void renderScene(const Shader& shader);
void staticRender(Shader& shader, Model m, float xR);

unsigned int loadTexture(char const* path);

// RESOLUTION
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

const unsigned int SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 1440;
unsigned int depthMapFBO;
unsigned int depthMap;

camera c(SCR_WIDTH, SCR_HEIGHT, 52);

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
    
    
    // configure depth map FBO
    // -----------------------
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture

    initShadowMap();
    return window;
}


//DELTATIME VALUES
double deltaTime = 0.0f;
float lastFrame = 0.0f;

//FRAME TIMER
double currtime;


//VIEWMODEL POSITIONING AND BEHAVIOUR
double xoffset = 0.0f;
double yoffset = 0.0f;
float x = 4.68005f, y = -0.52, z = 0.1333;
double xBand = 0.0f, yBand = 0.0f;
float shakeX = 0.0f, shakeY = 0.0f;


double sensitivity = 0.05f;

//ANIMATION CONTROLLER
int thisAnim = 4;
int playAnim = 1;
int reset = 0;

float xC = 0, yC = 0, zC = 0;

glm::mat4 lightProjection, lightView;
glm::mat4 lightSpaceMatrix;
float near_plane = 1.0f, far_plane = 7.5f;

unsigned int planeVAO;
glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
int main()
{
   
    // glfw window creation
    // --------------------
    GLFWwindow* window = init();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // build and compile our shader program
    // ------------------------------------
    Shader viewShader("SHADERS/color.vs", "SHADERS/color.fs");
    Shader staticShader("SHADERS/static.vs", "SHADERS/static.fs");
    Shader depthShader("SHADERS/depth.vs", "SHADERS/depth.fs");
    Shader lShader("SHADERS/cLight.vs", "SHADERS/cLight.fs");
    Viewmodel v(12, "Models/GUN/PEESTOL.fbx");
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

    
   



 

    //////
    //////
    //////

    //viewShader.use();
    //fBuffer.use();
    //fBuffer.setInt("screenTexture", 0);
    // framebuffer configuration
    // -------------------------
    
  

    float xL = 0;
    Model map("Models/DUST2/source/shit.fbx");
    Model thingy("Models/DUST2/source/THINGY.fbx");
    
    unsigned int woodTexture = loadTexture("TEXTURES/wood.png");
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    staticShader.use();
    staticShader.setInt("diffuseTexture", 0);
    staticShader.setInt("shadowMap", 1);
    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        xL += 20.0 * deltaTime;
       

        //glm::vec3 lightPos(-2.0f + xC, 4.0f + yC, -1.0f + zC);
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(c.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = c.view;
        glm::mat4 normal;
        
      
        //STATIC OBJECT RENDER

        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(1.0f), glm::vec3(5.0, 1.0, 10.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        
        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        renderScene(depthShader);
        staticRender(depthShader, thingy, xL);
      
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
       
        // 2. render scene as normal using the generated depth/shadow map  
        // --------------------------------------------------------------
        /**/
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
        
        lShader.use();
        lShader.setMat4("projection", projection);
        lShader.setMat4("view", view);
        lShader.setVec3("viewPos", c.cameraPos);
        lightCube(lShader);

        v.render(c, viewShader, window);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        //PRINT FRAMERATE
        std::cout << (int)(1000 / ((glfwGetTime() - currtime) * 1000)) << " FPS\n";

        //std::cout << c.fov << " " << x << " " << y << " " << z << "\n\n\n\n\n\n";



        /// VIEWMODEL POSITIONING
        /// 
        /// 
        /// 
        if (glfwGetKey(window, GLFW_KEY_UP))
        {
            //y += 0.001f;
            yC += 10 * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN))
        {
            //y -= 0.001f;
            yC -= 10 * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT))
        {
           // x += 0.001f;
           xC += 10 * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT))
        {
           // x -= 0.001f;
           xC -= 10 * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_E))
        {
           // z += 0.001f;
            zC += 10 * deltaTime;

        }
        if (glfwGetKey(window, GLFW_KEY_F))
        {
            // z -= 0.001f;
            zC -= 10 * deltaTime;

        }

    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // 
    glfwTerminate();
    return 0;
}









//CREATES THE FRAME BUFFER FOR THE SHADOWMAP
void initShadowMap()
{
    
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        // v.setState(0);

    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        //  v.setState(1);
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        // v.setState(2);
    }
}

//renders a model without animation
void staticRender(Shader& shader, Model m, float xR)
{
    shader.use();

    shader.setMat4("projection", c.projection);
    shader.setMat4("view", c.view);
    // set light uniforms
    shader.setVec3("viewPos", c.cameraPos);
    shader.setVec3("lightPos", lightPos);
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glm::mat3 normal;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f / 10, 0.5f / 10, 3.0f / 40));
    model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, (xR + 90) * 0.0174533f, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.08f / 10, 0.08f / 10, 0.08f / 10));
    shader.setMat4("model", model);

    m.draw(shader);

}

// renders the 3D scene
// --------------------
void renderScene(const Shader& shader)
{

    // floor
    glCullFace(GL_FRONT);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f / 10));
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glCullFace(GL_BACK);

    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f/10, 0.0));
    model = glm::scale(model, glm::vec3(0.5f/10));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f/10, 0.0f, 1.0/10));
    model = glm::scale(model, glm::vec3(0.5f/10));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f/10, 0.0f, 2.0/10));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25/10));
    shader.setMat4("model", model);
    renderCube();
}

void lightCube(const Shader& shader)
{
    
    glm::mat4 model = glm::mat4(1.0f);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(xC, yC, zC));
    model = glm::scale(model, glm::vec3(0.01f));
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