//STANDARD LIBRARY
#include <iostream>
#include <random>

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

std::vector<glm::mat4> getLightSpaceMatrices();
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);
void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);


//INITIALIZE OBJECTS
void renderCube();
void renderQuad();
void initShadowMap();


//RENDER FUNCTIONS
void lightCube(const Shader& shader);
void renderScene(const Shader& shader);
void staticRender(Shader& shader, Model& m, float xR, float xV, float yV, float zV);

unsigned int loadTexture(char const* path);

// RESOLUTION
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

// framebuffer size
int fb_width;
int fb_height;


float cameraNearPlane = 0.1f;
float cameraFarPlane = 500.0f;



std::vector<float> shadowCascadeLevels{ cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f };
int debugLayer = 0;

// lighting info
// -------------
unsigned int matricesUBO;
const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
unsigned int lightFBO;
unsigned int lightDepthMaps;
constexpr unsigned int depthMapResolution = 4096;
bool showQuad = false;



std::vector<glm::mat4> lightMatricesCache;


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
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
   //stbi_set_flip_vertically_on_load(true);
    //ENABLE DEPTHBUFFER
    
    
    

    initShadowMap();
    return window;
}

float planeVertices[] = 
{
    // positions            // normals         // texcoords
     25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

     25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
     25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
};


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
glm::vec3 lightPos(-2.0f + xC, 4.0f + yC, -1.0f + zC);
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
    Shader depthShader("SHADERS/depth.vs", "SHADERS/depth.fs", "SHADERS/depth.gs");
    Shader debugCascade("SHADERS/debug.vs", "SHADERS/debug.fs");
    Shader debugDepth("SHADERS/debugDepth.vs", "SHADERS/debugDepth.fs");

    Shader lShader("SHADERS/cLight.vs", "SHADERS/cLight.fs");
    Viewmodel v(7, "Models/GUN/PEESTOL.fbx");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    
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
    //Model map("Models/DUST2/source/shit.fbx");
    Model map("Models/NUKE/NUKE.fbx");
    //Model map("Models/NUKE/Garage Doors and Thresholds.fbx");
    //Model map("Models/chum/scene.gltf");
    //Model thingy("Models/DUST2/source/MACCY.obj");
    
    unsigned int woodTexture = loadTexture("TEXTURES/white.png");
    //unsigned int woodTexture = loadTexture("Models/DUST2/source/Garfield_BaseColor.png");
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    staticShader.use();
    staticShader.setInt("diffuseTexture", 0);
    staticShader.setInt("shadowMap", 1);
    debugDepth.use();
    debugDepth.setInt("depthMap", 0);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        xL += 20.0 * deltaTime;
       


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
        glm::mat4 normal;
        
      
        //STATIC OBJECT RENDER

        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        // 0. UBO setup
        const auto lightMatrices = getLightSpaceMatrices();
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        for (size_t i = 0; i < lightMatrices.size(); ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // render scene from light's point of view
        
        depthShader.use();

        glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
        glViewport(0, 0, depthMapResolution, depthMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);  // peter panning
        //renderScene(depthShader);
        staticRender(depthShader, map, 0,0,0,0);
       // staticRender(depthShader, thingy, xL,xC , yC, zC);
 
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, fb_width, fb_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
      
 
       
        // 2. render scene as normal using the generated depth/shadow map  
        // --------------------------------------------------------------
        /**/
        glViewport(0, 0, fb_width, fb_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        staticShader.use();
        const glm::mat4 projection = glm::perspective(glm::radians(c.fov), (float)fb_width / (float)fb_height, cameraNearPlane, cameraFarPlane);
        const glm::mat4 view = c.view;
        staticShader.setMat4("projection", projection);
        staticShader.setMat4("view", view);
        // set light uniforms
        staticShader.setVec3("viewPos", c.cameraPos);
        staticShader.setVec3("lightDir", lightDir);
        staticShader.setFloat("farPlane", cameraFarPlane);
        staticShader.setInt("cascadeCount", shadowCascadeLevels.size());
        for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
        {
            staticShader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
       // renderScene(staticShader);
        staticRender(staticShader, map, 0,0 ,0,0);
        //staticRender(staticShader, thingy, xL,xC, yC, zC);
        
        /*
        if (lightMatricesCache.size() != 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            debugCascade.use();
            debugCascade.setMat4("projection", projection);
            debugCascade.setMat4("view", view);
            drawCascadeVolumeVisualizers(lightMatricesCache, &debugCascade);
            glDisable(GL_BLEND);
        }
        /*
        // render Depth map to quad for visual debugging
        // ---------------------------------------------
        debugDepth.use();
        debugDepth.setInt("layer", debugLayer);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
        if (showQuad)
        {
            renderQuad();
        }
        */

        //RENDERS VIEWMODEL
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
    
    // configure depth map FBO
    // -----------------------
    glGenFramebuffers(1, &lightFBO);

    glGenTextures(1, &lightDepthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution, int(shadowCascadeLevels.size()) + 1,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthMaps, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure UBO
    // --------------------

    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // create depth texture
    
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
void staticRender(Shader& shader, Model& m, float xR, float xV, float yV, float zV)
{
    shader.use();

    shader.setMat4("projection", c.projection);
    shader.setMat4("view", c.view);
    // set light uniforms
   

    //glActiveTexture(GL_TEXTURE1);
   // glBindTexture(GL_TEXTURE_2D, depthMap);
    glm::mat3 normal;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f + xV, 0.5f + zV , 3.0f + yV));
    model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, (xR * 0.1f) * 0.0174533f, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, (180.0f) * 0.0174533f, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f , 0.05f , 0.05f ));
    shader.setMat4("model", model);

    m.draw(shader);

}

// renders the 3D scene
// --------------------
void renderScene(const Shader& shader)
{
   
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

std::vector<GLuint> visualizerVAOs;
std::vector<GLuint> visualizerVBOs;
std::vector<GLuint> visualizerEBOs;
void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader)
{
    visualizerVAOs.resize(8);
    visualizerEBOs.resize(8);
    visualizerVBOs.resize(8);

    const GLuint indices[] = {
        0, 2, 3,
        0, 3, 1,
        4, 6, 2,
        4, 2, 0,
        5, 7, 6,
        5, 6, 4,
        1, 3, 7,
        1, 7, 5,
        6, 7, 3,
        6, 3, 2,
        1, 5, 4,
        0, 1, 4
    };

    const glm::vec4 colors[] = {
        {1.0, 0.0, 0.0, 0.5f},
        {0.0, 1.0, 0.0, 0.5f},
        {0.0, 0.0, 1.0, 0.5f},
    };

    for (int i = 0; i < lightMatrices.size(); ++i)
    {
        const auto corners = getFrustumCornersWorldSpace(lightMatrices[i]);
        std::vector<glm::vec3> vec3s;
        for (const auto& v : corners)
        {
            vec3s.push_back(glm::vec3(v));
        }

        glGenVertexArrays(1, &visualizerVAOs[i]);
        glGenBuffers(1, &visualizerVBOs[i]);
        glGenBuffers(1, &visualizerEBOs[i]);

        glBindVertexArray(visualizerVAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, visualizerVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindVertexArray(visualizerVAOs[i]);
        shader->setVec4("color", colors[i % 3]);
        glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &visualizerVBOs[i]);
        glDeleteBuffers(1, &visualizerEBOs[i]);
        glDeleteVertexArrays(1, &visualizerVAOs[i]);

        glBindVertexArray(0);
    }

    visualizerVAOs.clear();
    visualizerEBOs.clear();
    visualizerVBOs.clear();
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


std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}


std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    return getFrustumCornersWorldSpace(proj * view);
}

glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane)
{
    const auto proj = glm::perspective(
        glm::radians(c.fov), (float)fb_width / (float)fb_height, nearPlane,
        farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, c.view);

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

std::vector<glm::mat4> getLightSpaceMatrices()
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i]));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane));
        }
    }
    return ret;
}