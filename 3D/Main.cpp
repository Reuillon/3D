//STANDARD LIBRARY
#include <iostream>
#include <random>
#include <array>

//ENGINE CLASSES
#include "stb_image.h"
#include "Collision.h"
#include "Viewmodel.h"
#include "Animator.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "MeshDraw.h"
#include "Player.h"
#include "Debug.h"

//THESE NEED TO BE WORKED ON
#include "Input.h"
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
void initShadowMap();
void initFramebuffer();
void initPBR(const char* hdrPath);
void initSSAO();


// RESOLUTION
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

//CAMERA DATA
float cameraNearPlane = 1.0f;
float cameraFarPlane = 1000.0f;
camera c(SCR_WIDTH, SCR_HEIGHT, 52);

//FRAMEBUFFER SIZE
int fb_width;
int fb_height;

//FRAMEBUFFER PROPERTIES
unsigned int gBuffer;
unsigned int gPosition, gNormal, gAlbedo, gShadow;

//SHADOWS
unsigned int shadowFBO;
unsigned int shadowMap;

//CASCADED SHADOW MAP 
std::vector<float> shadowCascadeLevels{ cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f };
std::vector<glm::mat4> getLightSpaceMatrices();
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

//SSAO
unsigned int ssaoFBO, ssaoBlurFBO;
unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
unsigned int noiseTexture;
std::vector<glm::vec3> ssaoKernel;

//PBR FRAMEBUFFER PROPERTIES
unsigned int irradianceMap;
unsigned int envCubemap;
unsigned int brdfLUTTexture;
unsigned int prefilterMap;

// lighting info
// -------------
unsigned int lightFBO;
unsigned int matricesUBO;
unsigned int lightDepthMaps;
constexpr unsigned int depthMapResolution = 4096;
glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50.0f, 20.0f));

//DELTATIME VALUES
double deltaTime = 0.0f;
float lastFrame = 0.0f;

//FRAME TIMER
double currtime;

int shooting = -1;

// Creates an identity matrix
float rayIdentity[6] =
{
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0
};

float cubeIdentity[24] =
{
    1.0,1.0,1.0,
    1.0,1.0,-1.0,
    1.0,-1.0,1.0,
    1.0,-1.0,-1.0,
    -1.0,1.0,1.0,
    -1.0,1.0,-1.0,
    -1.0,-1.0,1.0,
    -1.0,-1.0,-1.0
};

float sphereIdentity[126] =
{
    0.000000, -1.000000, 0.000000,
    0.723607, -0.447220, 0.525725,
    -0.276388, -0.447220, 0.850649,
    -0.894426, -0.447216, 0.000000,
    -0.276388, -0.447220, -0.850649,
    0.723607, -0.447220, -0.525725,
    0.276388, 0.447220, 0.850649,
    -0.723607, 0.447220, 0.525725,
    -0.723607, 0.447220, -0.525725,
    0.276388, 0.447220, -0.850649,
    0.894426, 0.447216, 0.000000,
    0.000000, 1.000000, 0.000000,
    -0.162456, -0.850654, 0.499995,
    0.425323, -0.850654, 0.309011,
    0.262869, -0.525738, 0.809012,
    0.850648, -0.525736, 0.000000,
    0.425323, -0.850654, -0.309011,
    -0.525730, -0.850652, 0.000000,
    -0.688189, -0.525736, 0.499997,
    -0.162456, -0.850654, -0.499995,
    -0.688189, -0.525736, -0.499997,
    0.262869, -0.525738, -0.809012,
    0.951058, 0.000000, 0.309013,
    0.951058, 0.000000, -0.309013,
    0.000000, 0.000000, 1.000000,
    0.587786, 0.000000, 0.809017,
    -0.951058, 0.000000, 0.309013,
    -0.587786, 0.000000, 0.809017,
    -0.587786, 0.000000, -0.809017,
    -0.951058, 0.000000, -0.309013,
    0.587786, 0.000000, -0.809017,
    0.000000, 0.000000, -1.000000,
    0.688189, 0.525736, 0.499997,
    -0.262869, 0.525738, 0.809012,
    -0.850648, 0.525736, 0.000000,
    -0.262869, 0.525738, -0.809012,
    0.688189, 0.525736, -0.499997,
    0.162456, 0.850654, 0.499995,
    0.525730, 0.850652, 0.000000,
    -0.425323, 0.850654, 0.309011,
    -0.425323, 0.850654, -0.309011,
    0.162456, 0.850654, -0.499995
};

float cylinderIdentity[126] =
{
   0.646264, -0.000001, -0.646781,
   0.894721, -0.000001, -0.000001,
   0.646264, -0.000001, 0.646779,
   -0.001598, -0.000001, -0.894742,
   -0.647296, -0.000001, -0.646780,
   -0.001598, -0.000001, 0.894741,
   -0.894762, -0.000001, -0.000001,
   -0.647296, -0.000001, 0.646779,
   -0.480277, -1.480080, 0.479378,
   -0.624203, -0.914345, 0.623636,
   -0.480194, 1.480092, 0.479118,
   -0.624193, 0.914341, 0.623601,
   -0.480277, -1.480080, -0.479370,
   -0.624203, -0.914345, -0.623635,
   -0.480186, 1.480092, -0.479634,
   -0.624190, 0.914344, -0.623671,
   0.478484, -1.480080, 0.479391,
   0.623072, -0.914339, 0.623639,
   0.478576, 1.480092, 0.479128,
   0.623085, 0.914336, 0.623604,
   0.478484, -1.480080, -0.479383,
   0.623072, -0.914339, -0.623639,
   0.478569, 1.480092, -0.479645,
   0.623083, 0.914339, -0.623674,
   -0.001610, -0.914342, -0.862734,
   -0.001685, -1.480080, -0.663685,
   -0.001598, 0.914343, -0.862768,
   -0.001597, 1.480092, -0.663945,
   -0.001610, -0.914342, 0.862734,
   -0.001685, -1.480080, 0.663693,
   -0.001598, 0.914338, 0.862700,
   -0.001597, 1.480092, 0.663436,
   -0.862820, -0.914346, 0.000000,
   -0.664206, -1.480080, 0.000004,
   -0.862809, 0.914344, -0.000036,
   -0.664121, 1.480092, -0.000262,
   0.862648, -0.914338, -0.000000,
   0.663172, -1.480080, 0.000004,
   0.862660, 0.914337, -0.000036,
   0.663260, 1.480092, -0.000262,
   0.000467, -1.655706, -0.000201,
   0.000906, 1.656405, 0.000008
};

int isCollide = -1;

//GLOBAL INPUT OBJECT
Input& input = Input::getInstance();
Debug debug;

//INITIALZES WINDOW AND RENDER PIPELINE
static GLFWwindow* windowInit()
{
    
    #ifdef __APPLE__
         glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    //SETS WINDOW SETTINGS
    glfwInit();
    glfwSwapInterval(1);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_REFRESH_RATE, 1000);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RENDERER", glfwGetPrimaryMonitor(), NULL);
    
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    //SET LISTENERS FOR DETECTING INPUT
    glfwMakeContextCurrent(window); 
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetMouseButtonCallback(window, &Input::mouse_button_callback);
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    
    //INITIALIZERS FOR VARIOUS GRAPHICAL EFFECTS
    initSSAO();
    initShadowMap();
    initFramebuffer();
    debug.initializeGrid(4);
    //initPBR("TEXTURES/hdri/sunset_fairway_2k.hdr");
    //initPBR("TEXTURES/hdri/SKY.hdr");
    //initPBR("TEXTURES/hdri/meadow_2k.hdr");
    //initPBR("TEXTURES/hdri/newport_loft.hdr");
    //initPBR("TEXTURES/hdri/snowy_forest_2k.hdr");
    //initPBR("TEXTURES/hdri/venice_sunset_2k.hdr");
    //initPBR("TEXTURES/hdri/kloofendal_28d_misty_puresky_2k.hdr");
    //initPBR("TEXTURES/hdri/charolettenbrunn_park_4k.hdr");
    //initPBR("TEXTURES/hdri/autumn_field_4k.hdr");
    //initPBR("TEXTURES/hdri/rosendal_park_sunset_puresky_2k.hdr");
    //initPBR("TEXTURES/hdri/ballawley_park_4k.hdr");
    //initPBR("TEXTURES/hdri/preller_drive_4k.hdr");
    //initPBR("TEXTURES/hdri/tief_etz_4k.hdr");
    //initPBR("TEXTURES/hdri/little_paris_eiffel_tower_4k.hdr");
    //initPBR("TEXTURES/hdri/tiergarten_4k.hdr");
  

    //initPBR("TEXTURES/hdri/belfast_sunset_puresky_2k.hdr");
    //initPBR("TEXTURES/hdri/color.hdr");
    //initPBR("TEXTURES/hdri/qwantani_dusk_2_4k.hdr");
    initPBR("TEXTURES/hdri/lakeside_night_4k.hdr");
    //initPBR("TEXTURES/hdri/soliltude_4k.hdr");
    
    return window;
}
glm::vec3 gravity = glm::vec3(0.0, 0.0, 0.0);
bool isJump;
bool isGrounded = false;
glm::vec3 lastSpeed = glm::vec3(0);
glm::vec2 normalizedSpeed = glm::vec2(0);
glm::vec2 momentum = glm::vec2(0);
int toggleDebug = -1;



int main()
{   
    // glfw window creation
    // --------------------
    GLFWwindow* window = windowInit();  
    
    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ////SHADERS    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    
    //UI
    Shader crosshair("SHADERS/depth.vs", "SHADERS/UI.fs");

    //VIEWMODEL
    Shader viewShader("SHADERS/animated.vs", "SHADERS/pbrTexture.fs");

    //SHADOWS
    Shader shadowPass("SHADERS/shadow.vs", "SHADERS/shadow.fs");
    Shader depthShader("SHADERS/depth.vs", "SHADERS/depth.fs", "SHADERS/depth.gs");

    //DEFERRED RENDERING
    Shader shaderGeometryPass("SHADERS/geometry.vs", "SHADERS/geometry.fs");
    Shader shaderLightingPass("SHADERS/lighting.vs", "SHADERS/lighting.fs");

    //POSTPROCESSING
    Shader shaderSSAO("SHADERS/SSAO.vs", "SHADERS/SSAO.fs");
    Shader shaderSSAOBlur("SHADERS/SSAO.vs", "SHADERS/SSAOBlur.fs");

    //PBR
    Shader shaderPBRT("SHADERS/pbr.vs", "SHADERS/pbrTexture.fs");
    Shader prefilterShader("SHADERS/cubemap.vs", "SHADERS/prefilter.fs");
    Shader brdfShader("SHADERS/brdf.vs", "SHADERS/brdf.fs");

    //CUBEMAPPING
    Shader equirectangularToCubemapShader("SHADERS/cubemap.vs", "SHADERS/cubemapConvert.fs");
    Shader irradianceShader("SHADERS/cubemap.vs", "SHADERS/irradianceConvolution.fs");
    Shader backgroundShader("SHADERS/background.vs", "SHADERS/background.fs");

    //DEBUG SHADERS
    Shader defaultShader("SHADERS/Default.vs", "SHADERS/Default.fs");
    debug.defaultShader = defaultShader;

    //WATER SHADER 
    Shader waterShader("SHADERS/water.vs", "SHADERS/water.fs");
    Shader sandShader("SHADERS/sand.vs", "SHADERS/sand.fs");

    /////MODELS    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    
    //VIWEMODELS
    //Viewmodel v(12, "Models/GUN/PEESTOL.fbx");
    Viewmodel v(8, "Models/GUN/BOLTON.fbx");
    //Viewmodel v(11, "Models/GUN/BS2.fbx");
    //Viewmodel v(11, "Models/GUN/DEGGLETMP.fbx");
    
    Player p(SCR_WIDTH, SCR_HEIGHT, window);
    //MAPS
    //Model map("Models/NEWDUST/DUST.fbx");
    Model map("Models/GUN/TESTLEVEL/TESTLEVEL.fbx");
    //Model map("Models/Aztec/aztec.fbx");
    //Model map("Models/NTOWN/NTOWN.obj");
    //Model map("Models/RUST/RUST.obj");

    //STATIC OBJECTS
    //Model gun("Models/DUST2/source/BS1.fbx");
    Model water("Models/GUN/water.fbx");
    //Model sand("Models/GUN/water.fbx");
    //Model base("Models/GUN/base.fbx");
    //Model shib("Models/shiba/1.fbx");
    //stbi_set_flip_vertically_on_load(false);\
    //Model gun("Models/DUST2/source/KNIFE.fbx");
    //Model gun("Models/GUN/DEGGLETMP.fbx");
    //Model gun("Models/DUST2/source/REV.fbx");
    //Model macHand("Models/MAC10VIEWMODEL.obj");
    //Model gun("Models/DUST2/source/REVOLVER.obj");
    //Model gun("Models/GUN/PEESTOL.obj");
    //Model gun("Models/GUN/BS2.obj");
    //Model mySphere("Models/GUN/sphere.fbx");
    Model myPaddle("Models/GUN/paddle.fbx");

    //COLLIDERS    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    
    std::vector<MeshCollider> testMap = initCollisionMap("Models/GUN/TESTLEVEL/COLLISIONMAP.obj");

    MeshCollider ray(rayIdentity, sizeof(rayIdentity) / sizeof(*rayIdentity));
    
    MeshCollider sphere1(sphereIdentity, sizeof(sphereIdentity) / sizeof(*sphereIdentity));
    MeshCollider sphere2(sphereIdentity, sizeof(sphereIdentity) / sizeof(*sphereIdentity));
    MeshCollider sphere3(sphereIdentity, sizeof(sphereIdentity) / sizeof(*sphereIdentity));
    MeshCollider sphere4(sphereIdentity, sizeof(sphereIdentity) / sizeof(*sphereIdentity));
    MeshCollider sphere5(sphereIdentity, sizeof(sphereIdentity) / sizeof(*sphereIdentity));
    
    MeshCollider cube(cubeIdentity, sizeof(cubeIdentity) / sizeof(*cubeIdentity));
    MeshCollider AnotherPlatform(cubeIdentity, sizeof(cubeIdentity) / sizeof(*cubeIdentity));
    
    unsigned int woodTexture = loadTexture("TEXTURES/white.png");

    

    //INITIALIZE SHADER UNIFORM DATA
    
    /*
    //SHADOW MAP DEPTH TEXTURE
    shadowPass.use();
    shadowPass.setInt("diffuseTexture", 0);

    //DEFERRED LIGHTING PASS
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedo", 2);
    shaderLightingPass.setInt("ssao", 3);
    shaderLightingPass.setInt("shadowMap", 4);

    //DEFERRED GEOMETRY PASS
    shaderGeometryPass.use();
    shaderGeometryPass.setInt("diffuseTexture", 0);

    //SSAO PASS
    shaderSSAO.use();
    shaderSSAO.setInt("gPosition", 0);
    shaderSSAO.setInt("gNormal", 1);
    shaderSSAO.setInt("texNoise", 2);
    shaderSSAOBlur.use();
    shaderSSAOBlur.setInt("ssaoInput", 0);
    */

    //CUBEMAP SHADER
    glm::mat4 projection = glm::perspective(glm::radians(p.playerCamera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, cameraNearPlane, cameraFarPlane);
    backgroundShader.use();
    backgroundShader.setMat4("projection", p.playerCamera.projection);

    //PBR SHADER
    shaderPBRT.use();
    shaderPBRT.setInt("albedoMap", 0);
    shaderPBRT.setInt("normalMap", 1);
    shaderPBRT.setInt("metallicMap", 2);
    shaderPBRT.setInt("roughnessMap", 3);
    shaderPBRT.setInt("aoMap", 4);
    shaderPBRT.setInt("irradianceMap", 5);
    shaderPBRT.setInt("prefilterMap", 6);
    shaderPBRT.setInt("brdfLUT", 7);
    
    float r1 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    float r2 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    sphere1.setTransform(glm::vec3(r1, r2, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
    r1 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    r2 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    sphere2.setTransform(glm::vec3(r1, r2, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
    r1 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    r2 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    sphere3.setTransform(glm::vec3(r1, r2, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
    r1 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    r2 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    sphere4.setTransform(glm::vec3(r1, r2, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
    r1 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    r2 = -5.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0 - -5.0)));
    sphere5.setTransform(glm::vec3(r1, r2, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
    

    c.cameraPos = glm::vec3(0,0,0);
    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
    ///MAINLOOP    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    while (!glfwWindowShouldClose(window))
    {
        

        

        ResolutionData r;
        /**/
        p.isFalling = false;
        p.isGrounded = false;
        
        for (int i = 0; i < testMap.size(); i++)
        {

            r = GJK(p.playerCamera.floorCollider, testMap[i], deltaTime,false);
            if (r.hasCollision && !p.isJump)
            {
                p.gravity = glm::vec3(0.0);
            }
            if (r.hasCollision)
            {
                p.isGrounded = true;
                if (p.isJump)
                {
                    p.gravity.y = 11;
                }
            }
            r = GJK(p.playerCamera.cameraCollider, testMap[i], deltaTime, true);
            if (r.hasCollision)
            {
                p.isFalling = false;
            }
            else
            {
                p.isFalling = true;
            }

        }
    


        //c.update(deltaTime);
        glClearColor(1.0f, 0.87f, 0.64f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        p.update(deltaTime, viewShader);
        lightDir = glm::normalize(glm::vec3(30.0f, 5.0, 30.0f));
        //glm::vec3 lightPos = glm::vec3(0 + xC, -10 + yC, 10 + zC);
        //DELTA TIME CALCULATION
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        //UPDATE CAMERA POSITIONS

        //c.update(deltaTime);
        //debug.drawGrid(c);

        glm::mat4 model;
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        const auto lightMatrices = getLightSpaceMatrices();
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        for (size_t i = 0; i < lightMatrices.size(); ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // set light uniforms
        model = glm::mat4(1.0f);

        /*
        depthShader.setMat4("model", model);
        depthShader.setMat4("projection", c.projection);
        depthShader.setMat4("view", c.view);
        depthShader.use();

        glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
        glViewport(0, 0, depthMapResolution, depthMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);

        // peter panning
        glDisable(GL_DEPTH_CLAMP);
        staticRender(depthShader, map, 0, 0, 0, 0);

        staticRender(depthShader, shib, glfwGetTime(), 0, -80, 2);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, -15.5, -10));
        model = glm::rotate(model, ((float)(-glfwGetTime() * 50.0f) * 0.0174533f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, (360.0f) * 0.0174533f, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
        depthShader.setMat4("model", model);
        gun.draw(depthShader);



        glCullFace(GL_BACK);
        shadowPass.use();
        // reset viewport
        glViewport(0, 0, fb_width, fb_height);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.00784313725f, 0.431372549f, 0.678431373f, 1.0f);
        const glm::mat4 projection = glm::perspective(glm::radians(c.fov), (float)fb_width / (float)fb_height, cameraNearPlane, cameraFarPlane);
        const glm::mat4 view = c.view;
        shadowPass.setMat4("projection", c.projection);
        shadowPass.setMat4("view", view);
        // set light uniforms
        shadowPass.setVec3("viewPos", c.cameraPos);
        shadowPass.setVec3("lightDir", lightDir);
        shadowPass.setFloat("farPlane", cameraFarPlane);
        shadowPass.setInt("cascadeCount", shadowCascadeLevels.size());
        for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
        {
            shadowPass.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
        }
        shadowPass.setFloat("clampVal", 0.01f);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
        staticRender(shadowPass, map, 0, 0, 0, 0);
        shadowPass.setFloat("clampVal", 0.005f);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
        staticRender(shadowPass, shib, glfwGetTime(), 0, -80, 2);
        shadowPass.setFloat("clampVal", 0.005f);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, -15.5, -10));
        model = glm::rotate(model, ((float)(-glfwGetTime() * 50.0f) * 0.0174533f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, (360.0f) * 0.0174533f, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
        shadowPass.setMat4("model", model);
        glDisable(GL_CULL_FACE);
        gun.draw(shadowPass);
        glEnable(GL_CULL_FACE);



        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", c.projection);
        shaderGeometryPass.setMat4("view", c.view);
        shaderGeometryPass.setVec3("viewPos", c.cameraPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        staticRender(shaderGeometryPass, map, 0, 0, 0, 0);
        staticRender(shaderGeometryPass, shib, glfwGetTime(), 0, -80, 2);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glDisable(GL_CULL_FACE);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", c.projection);
        shaderGeometryPass.setMat4("view", c.view);
        shaderGeometryPass.setMat4("model", model);
        gun.draw(shaderGeometryPass);
        glEnable(GL_CULL_FACE);



        // 2. generate SSAO texture
              // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.use();
        // Send kernel + rotation
        for (unsigned int i = 0; i < 64; ++i)
            shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        shaderSSAO.setMat4("projection", c.projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);



        // 3. blur SSAO texture to remove noise
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAOBlur.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        glEnable(GL_FRAMEBUFFER_SRGB);



        // 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
        // -----------------------------------------------------------------------------------------------------
        glViewport(0, 0, fb_width, fb_height);
        glClear(GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        // send light relevant uniforms
        shaderLightingPass.setMat4("view", c.view);

        // set light uniforms
        shaderLightingPass.setVec3("viewPos", c.cameraPos);
        shaderLightingPass.setVec3("lightDir", lightDir);
        shaderLightingPass.setFloat("farPlane", cameraFarPlane);
        shaderLightingPass.setInt("cascadeCount", shadowCascadeLevels.size());
        for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
        {
            shaderLightingPass.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        glActiveTexture(GL_TEXTURE4); // add extra SSAO texture to lighting pass
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        renderQuad();
        */

        //PBR UNIFORMS
        shaderPBRT.use();
        shaderPBRT.setMat4("projection", p.playerCamera.projection);
        shaderPBRT.setMat4("view", p.playerCamera.view);
        shaderPBRT.setVec3("camPos", p.playerCamera.cameraPos);
        shaderPBRT.setVec3("lightDir", lightDir);

        //SETS BACKGROUND FOR IBL
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

        //DRAWS CUBEMAP FOR IBL
        glDisable(GL_CULL_FACE);
        backgroundShader.use();
        backgroundShader.setMat4("view", p.playerCamera.view);
        glActiveTexture(GL_TEXTURE0);
        //DRAWS BLURRED MAP
        //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        //DRAWS LOW RES CUBEMAP
        //glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        //DRAWS HIGH RES CUBEMAP
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderCube();

        //cube.setTransform(glm::vec3(debug.xVal * 40, (-1 + (debug.yVal * 40)), debug.zVal * 40), glm::vec3(0.0, 0.0, glfwGetTime() / 2), glm::vec3(10.0,1.0,5.0));
        //cube.setTransform(glm::vec3(debug.xVal * 10, debug.yVal * 10, debug.zVal * 10), glm::vec3(0.0, 45.0 * 0.0174533, 45.0 * 0.0174533), glm::vec3(1.0));
        //movingcube.moveCollider(gravity * glm::vec3(deltaTime));
        cube.setTransform(glm::vec3(debug.xVal * 40, (-1 + (debug.yVal * 40)), debug.zVal * 40), glm::vec3(0.0), glm::vec3(10.0, 1.0, 5.0));
        AnotherPlatform.setTransform(glm::vec3(5,1,3), glm::vec3(0.0), glm::vec3(10.0, 1.0, 5.0));

        //CREATES RAYCAST FROM CAMERA ORIGIN TO WHEREVER IT IS LOOKING
        //ray.vertices[0] = glm::vec3(c.cameraPos.x, c.cameraPos.y, c.cameraPos.z);
        //ray.vertices[1] = glm::vec3(c.cameraPos.x + (c.front.x * 1000.0f), c.cameraPos.y + (c.front.y * 1000.0f),c.cameraPos.z + (c.front.z * 1000.0f));



        //RESETS VALUE FOR COLLISION MESH FOR CONTINOUS COLLISION CHECKING
        ray.color = glm::vec3(1.0, 1.0, 1.0);
        cube.color = glm::vec3(0.0, 1.0, 1.0);
        AnotherPlatform.color = glm::vec3(0.0, 1.0, 1.0);
        sphere1.color = glm::vec3(1.0, 1.0, 1.0);
        sphere2.color = glm::vec3(1.0, 1.0, 1.0);
        sphere3.color = glm::vec3(1.0, 1.0, 1.0);
        sphere4.color = glm::vec3(1.0, 1.0, 1.0);
        sphere5.color = glm::vec3(1.0, 1.0, 1.0);

        
        
        


        glDisable(GL_CULL_FACE);
        //SHOOTABLE SPHERES
        /*
        if (GJK(sphere1, ray, false))
        {
            sphere1.color = glm::vec3(1.0, 0.0, 0.0);

            if (v.ammo > 0 && shooting == 1)
            {
                float r1 = -7.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (7.5 - -7.5)));
                float r2 = -7.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (7.5 - -7.5)));
                sphere1.setTransform(glm::vec3(r1, r2 + 7.5, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
            }
        }
        if (GJK(sphere2, ray, false))
        {
            sphere2.color = glm::vec3(1.0, 0.0, 0.0);
            if (v.ammo > 0 && shooting == 1)
            {
                float r1 = -7.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (7.5 - -7.5)));
                float r2 = -7.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (7.5 - -7.5)));
                sphere2.setTransform(glm::vec3(r1, r2 + 7.5, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
            }
        }
        if (GJK(sphere3, ray, false))
        {
            sphere3.color = glm::vec3(1.0, 0.0, 0.0);
            if (v.ammo > 0 && shooting == 1)
            {
                float r1 = -10.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0 - -10.0)));
                float r2 = -10.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0 - -10.0)));
                sphere3.setTransform(glm::vec3(r1, r2 + 7.5, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
            }
        }
        if (GJK(sphere4, ray, false))
        {
            sphere4.color = glm::vec3(1.0, 0.0, 0.0);
            if (v.ammo > 0 && shooting == 1)
            {
                float r1 = -10.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0 - -10.0)));
                float r2 = -10.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0 - -10.0)));
                sphere4.setTransform(glm::vec3(r1, r2 + 7.5, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
            }
        }
        if (GJK(sphere5, ray, false))
        {
            sphere5.color = glm::vec3(1.0, 0.0, 0.0);
            if (v.ammo > 0 && shooting == 1)
            {
                float r1 = -10.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0 - -10.0)));
                float r2 = -10.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0 - -10.0)));
                sphere5.setTransform(glm::vec3(r1, r2 + 7.5, debug.zVal * 10), glm::vec3(0.0, 0.0, 0.0));
            }
        }


        debug.ollider(sphere1, c);
        debug.drawCollider(sphere2, c);
        debug.drawCollider(sphere3, c);
        debug.drawCollider(sphere4, c);
        debug.drawCollider(sphere5, c);
        */



        //      debug.drawCollider(c.floorCollider,c);
        

        if (toggleDebug == 1)
        {
            for (int i = 0; i < testMap.size(); i++)
            {
               debug.drawCollider(testMap[i], p.playerCamera);
            }
        }
        else
        {
            mapRender(p.playerCamera, shaderPBRT, map, glm::vec3(0), glm::vec3(100, 100, 100));
        }
        
        /*
        //DRAWS XYZ LINES
        //DEFAULT ORIENTATIONS (X,Y,Z)
        debug.drawLine(c, glm::vec3(0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec4(0.0, 1.0, 0.0, 1.0));
        debug.drawLine(c, glm::vec3(0.0f), glm::vec3(1.0, 0.0, 0.0), glm::vec4(1.0, 0.0, 0.0, 1.0));
        debug.drawLine(c, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 1.0));
        */
        glEnable(GL_CULL_FACE);
        
        //staticRender(c, shaderPBRT, base);
        //drawSand(c, sandShader, sand, envCubemap);
        drawWater(p.playerCamera, waterShader, water, envCubemap);
        
        //DRAW DUST 2
        
        //mapRender(c,shaderPBRT, map, glm::vec3(debug.xVal * 5.0, debug.yVal * 5.0, debug.zVal * 5.0));
        

        
        glDisable(GL_CULL_FACE);
        
        //RENDERS VIEWMODEL
        //v.render(p.playerCamera, viewShader, window);
       
        //WIP CROSSHAIR RENDERER (USES 4 PLANES BECAUSE IM LAZY) 
        //*THE CURLY BRACKETS MAY CAUSE ERRORS ON OTHER PLATFORMS REMOVE TO FIX
        /*
        {
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.005f, 0.0025f, 0.005f)); 
            crosshair.use();
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.005f * v.length, 0.0025f * v.thickness, 0.005f));
            model = glm::translate(model, glm::vec3(v.spread, 0.0, 0.0f));
            crosshair.setMat4("model", model);
            crosshair.setMat4("projection", c.projection);
            crosshair.setMat4("view", c.view);
            renderQuad();

            crosshair.use();
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.005f * v.length, 0.0025f * v.thickness, 0.005f));
            model = glm::translate(model, glm::vec3(-v.spread, 0.0, 0.0f));
            crosshair.setMat4("model", model);
            crosshair.setMat4("projection", c.projection);
            crosshair.setMat4("view", c.view);
            renderQuad();

            crosshair.use();
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.00125f * v.thickness, 0.008f * v.length, 0.005f));
            model = glm::translate(model, glm::vec3(0.0, v.spread * 1.1, 0.0f));
            crosshair.setMat4("model", model);
            crosshair.setMat4("projection", c.projection);
            crosshair.setMat4("view", c.view);
            renderQuad();

            crosshair.use();
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.00125f * v.thickness, 0.008f * v.length, 0.005f));
            model = glm::translate(model, glm::vec3(0.0, -v.spread * 1.1, 0.0f));
            crosshair.setMat4("model", model);
            crosshair.setMat4("projection", c.projection);
            crosshair.setMat4("view", c.view);
            renderQuad();
        }
        */

       
        debug.debugControls(window, deltaTime); 
        glfwSwapBuffers(window);
        glfwPollEvents();
        //PRINT FRAMERATE
        std::cout << (int)(1000 / ((glfwGetTime() - currentFrame) * 1000)) << " FPS\n";

 

        

    }


    //glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

//INITIALIZERS //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////

//CREATES THE FRAME BUFFER FOR THE SHADOWMAP
void initShadowMap()
{
    unsigned int quadVAOs, quadVBOs;
    unsigned int rboShadow;
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
    // configure depth map FBO
    // -----------------------
    glGenFramebuffers(1, &lightFBO);

    glGenTextures(1, &lightDepthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution, int(shadowCascadeLevels.size()) + 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

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
    glGenVertexArrays(1, &quadVAOs);
    glGenBuffers(1, &quadVBOs);
    glBindVertexArray(quadVAOs);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBOs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    // create a color attachment texture

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowMap, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

    glGenRenderbuffers(1, &rboShadow);
    glBindRenderbuffer(GL_RENDERBUFFER, rboShadow);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboShadow); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

//CREATES THE FRAME BUFFER FOR SSAO
static void initSSAO()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    // also create framebuffer to hold SSAO processing stage 
    // -----------------------------------------------------

    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generate sample kernel
    // ----------------------

    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

static void initFramebuffer()
{
    unsigned int rboDepth;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//INITIALIZES CUBEMAPS FOR PBR + IBL
static void initPBR(const char* hdrPath)
{
    unsigned int captureFBO;
    unsigned int captureRBO;
    unsigned int hdrTexture;
    Shader prefilterShader("SHADERS/cubemap.vs", "SHADERS/prefilter.fs");
    Shader brdfShader("SHADERS/brdf.vs", "SHADERS/brdf.fs");

    //CUBEMAPPING
    Shader equirectangularToCubemapShader("SHADERS/cubemap.vs", "SHADERS/cubemapConvert.fs");
    Shader irradianceShader("SHADERS/cubemap.vs", "SHADERS/irradianceConvolution.fs");
    Shader backgroundShader("SHADERS/background.vs", "SHADERS/background.fs");

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(hdrPath, &width, &height, &nrComponents, 0);
    stbi_set_flip_vertically_on_load(false);
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------

    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------

    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------

    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------

    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    backgroundShader.use();
    backgroundShader.setInt("environmentMap", 0);
}

//INPUT////    LISTENERS/    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//CONTINOUS INPUT
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }
    const float speed = 10.0f;
    if (glfwGetKey(window, GLFW_KEY_W))
    {
        lastSpeed += (c.forw);
        if (isGrounded)
        {
            c.forward();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A))
    {
        lastSpeed -= (glm::cross(c.forw, c.cameraUp));
        if (isGrounded)
        {
            c.left();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S))
    {
        lastSpeed -= (c.forw);
        if (isGrounded)
        {
            c.back();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D))
    {
        lastSpeed += (glm::cross(c.forw, c.cameraUp));
        if (isGrounded)
        {
            c.right();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_Q))
    {
        //c.crouch();
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE))
    {
        //c.jump();
    }
    else
    {
        
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
    {
        c.speed = 1.35 * speed;
    }
    else
    {
        c.speed = 1.35 * speed;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4))
    {
       // c.crouch();
    }
    /**/
    if (glfwGetKey(window, GLFW_KEY_SPACE))
    {
        isJump = true;


    }
    else
    {
        isJump = false;
    }
    
}

//MOUSE MOVEMENT
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    static double xoffsetS = 0.0f;
    static double yoffsetS = 0.0f;
    double sensitivity = 0.05f;
    double xpos = (xposIn);
    double ypos = (yposIn);
    xoffsetS = xpos - lastX;
    yoffsetS = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    xoffsetS *= sensitivity;
    yoffsetS *= sensitivity;
    c.camRot(xoffsetS, yoffsetS);
}

//SCROLL INPUT
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    
}
bool latch = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        // v.setState(0);
    }
    /*
    if ((key == GLFW_KEY_SPACE && action == GLFW_PRESS))
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
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        toggleDebug *= -1;
        //  v.setState(1);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        // v.setState(2);
    }

}

//MOUSE SINGLE INPUT
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        shooting = 1;
    }
    else
    {
        shooting = -1;
    }
}

//SHADOW///    MAPPING///    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////

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

static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    return getFrustumCornersWorldSpace(proj * view);
}

static glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane)
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

    const glm::mat4 lightProjection = glm::ortho(maxX, minX, maxY, minY, minZ, maxZ);
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

