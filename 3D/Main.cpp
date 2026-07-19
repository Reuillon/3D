//STANDARD LIBRARY
#include <array>
#include <iostream>
#include <random>
#include <thread>
#include <time.h>



//ENGINE CLASSES
#include "Actor.h"
#include "Animator.h"
#include "Camera.h"
#include "Collision.h"
#include "Debug.h"
#include "Level.h"
#include "MeshDraw.h"
#include "Model.h"
#include "Player.h"
#include "stb_image.h"
#include "Shader.h"

//GLOBAL HEADER FOR ALL VITAL DATA
#include "Global.h"

//THESE NEED TO BE WORKED ON
#include "Input.h"
#include "Server.h"

//OPENGL LIBRARIES
#include <GLFW/glfw3.h>

//OPENAL LIBRARIES
#include "InitializeSound.h"

//GLFW INPUT FUNCTIONS
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//INITIALIZER FUNCTIONS
static void initShadowMap();
static void initFramebuffer();
static void initPBR(const char* hdrPath, int resolution);
static void initSSAO();

// RESOLUTION
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

//FRAMEBUFFER SIZE
int fb_width;
int fb_height;

//FRAMEBUFFER PROPERTIES
unsigned int gBuffer;
unsigned int gPosition, gNormal, gAlbedo, gPBR, gShadow;

//SHADOWS
unsigned int shadowFBO;
unsigned int shadowMap;



//SSAO
unsigned int ssaoFBO, ssaoBlurFBO;
unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
unsigned int noiseTexture;
std::vector<glm::vec3> ssaoKernel;
std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
std::default_random_engine generator;

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
constexpr unsigned int depthMapResolution = 1024 * 4;
glm::vec3 lightDir;

//CASCADED SHADOW MAP 
std::vector<float> shadowCascadeLevels{ 67.0f, 300.0f, 545.0f};
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);
std::vector<glm::mat4> getLightSpaceMatrices();



//FRAME TIMER
double currtime;

// Creates an identity matrix
float rayIdentity[6] =
{
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0
};
float pointIdentity[6] =
{
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0
};

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
    glfwSwapInterval(0);
    //glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_REFRESH_RATE, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RED_BITS, 16);
    glfwWindowHint(GLFW_GREEN_BITS, 16);
    glfwWindowHint(GLFW_BLUE_BITS, 16);
    glfwWindowHint(GLFW_ALPHA_BITS, 16);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GAME ^_^", glfwGetPrimaryMonitor(), NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    //SET LISTENERS FOR DETECTING INPUT
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    //☺
    //initPBR("TEXTURES/hdri/port.hdr", 2048);
    //initPBR("TEXTURES/hdri/german.hdr", 4096);
    
    
    //initPBR("TEXTURES/hdri/color.hdr", 2048);
    //initPBR("TEXTURES/hdri/rosendal_park_sunset_puresky_2k.hdr", 2048);
    //initPBR("TEXTURES/hdri/kloofendal_48d_partly_cloudy_puresky_2k.hdr", 2048);
    srand(time(NULL));
    int skybox = rand() % 4;
    std::cout << skybox << "\n";
    skybox = 1;
    switch (skybox)
    {
        case 0:
        default:    
            initPBR("TEXTURES/hdri/sunset_fairway_16k.hdr", 2048);
            break;
        case 1:
            initPBR("TEXTURES/hdri/meadow_16k.hdr", 2048);
            break;
        case 2:
            initPBR("TEXTURES/hdri/DESERT.hdr", 2048);
            break;
        case 3:
            initPBR("TEXTURES/hdri/qwantani_dusk_2_4k.hdr", 2048);
            break;
        case 4:
            initPBR("TEXTURES/hdri/lakeside_night_4k.hdr", 2048);
            break;
        case 5:
            initPBR("TEXTURES/hdri/belfast_sunset_puresky_2k.hdr", 2048);
            break;
    }

    
    //initPBR("TEXTURES/hdri/whipple_creek_regional_park_04_2k.hdr");
    //initPBR("TEXTURES/hdri/SKY.hdr");
    //initPBR("TEXTURES/hdri/newport_loft.hdr");
    //initPBR("TEXTURES/hdri/snowy_forest_2k.hdr");
    //initPBR("TEXTURES/hdri/venice_sunset_2k.hdr");

    //initPBR("TEXTURES/hdri/charolettenbrunn_park_4k.hdr");
    //initPBR("TEXTURES/hdri/autumn_field_4k.hdr");
    //initPBR("TEXTURES/hdri/preller_drive_4k.hdr");
    //initPBR("TEXTURES/hdri/tief_etz_4k.hdr");
    //initPBR("TEXTURES/hdri/little_paris_eiffel_tower_4k.hdr");
    //initPBR("TEXTURES/hdri/tiergarten_4k.hdr");
    //initPBR("TEXTURES/hdri/studio_garden_2k.hdr");
    //initPBR("TEXTURES/hdri/wildflower_field_2k.hdr");
    //initPBR("TEXTURES/hdri/cannon_2k.hdr");

    
    //initPBR("TEXTURES/hdri/ballawley_park_4k.hdr");
    
    
    
    //initPBR("TEXTURES/hdri/lakeside_night_4k.hdr");
    //initPBR("TEXTURES/hdri/soliltude_4k.hdr");

    return window;
}
Player* p;
int toggleDebug = -1;
//std::thread networkThread;
int main()
{
    std::cout << "\033[2J\033[1;1H";
    //WINDOW///    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    //INITIALIZATION/////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////

    GLFWwindow* window = windowInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    /////SOUNDS    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    InitializeSound* defaultSoundDevice = InitializeSound::get();
    uint32_t hurt = SoundBuffer::get()->addSoundEffect("SOUNDS/marioHurt.mp3");
    uint32_t hit = SoundBuffer::get()->addSoundEffect("SOUNDS/HIT.mp3");
    uint32_t headshot = SoundBuffer::get()->addSoundEffect("SOUNDS/HEADSHOT.mp3");
    SoundSource worldSpeaker;
    SoundSource hitSpeaker;
    SoundSource headSpeaker;
    worldSpeaker.p_Gain = 0.25f;
    hitSpeaker.p_Gain = 2.0f;
    headSpeaker.p_Gain = 0.25;

    ////SHADERS    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////

    //SHADOWS
    Shader depthShader("SHADERS/depth.vs", "SHADERS/depth.fs", "SHADERS/depth.gs");
 
    //POSTPROCESSING
    Shader shaderSSAO("SHADERS/SSAO.vs", "SHADERS/SSAO.fs");
    Shader shaderSSAOBlur("SHADERS/SSAO.vs", "SHADERS/SSAOBlur.fs");

    //DEFERRED RENDERING
    Shader shaderGeometryPass("SHADERS/geometry.vs", "SHADERS/geometry.fs");
    Shader shaderLightingPass("SHADERS/DeferredPBR.vs", "SHADERS/DeferredPBR.fs");

    //CUBEMAPPING
    Shader backgroundShader("SHADERS/background.vs", "SHADERS/background.fs");

    //DEBUG SHADERS
    Shader defaultShader("SHADERS/Default.vs", "SHADERS/Default.fs");
    debug.defaultShader = defaultShader;

    //WATER SHADER 
    //Shader waterShader("SHADERS/water.vs", "SHADERS/water.fs");
    //Shader sandShader("SHADERS/sand.vs", "SHADERS/sand.fs");

    //NETWORKING   //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    Server gameServer;

    /////ACTORS    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    p = new Player(SCR_WIDTH, SCR_HEIGHT, window);

    /////MODELS    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    Model contact("Models/GUN/HITPOINT.fbx");

    //MAPS
    Level shipment("Models/Level_Maps/Structure.fbx", "Models/Level_Maps/StructureCollisionMap.obj", glm::vec3(0), glm::vec3(0, 0, 0), glm::vec3(1.0));
    
    //SHIPMENT
    //Level shipment("Models/GUN/TESTLEVEL/SHIPMENTWIP.fbx", "Models/GUN/TESTLEVEL/COLLISIONMAP2.obj", glm::vec3(0), glm::vec3(0, 0, 0), glm::vec3(1.0));
    
    //COLLIDERS    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    MeshCollider ray(rayIdentity, sizeof(rayIdentity) / sizeof(*rayIdentity));
    MeshCollider point(pointIdentity, sizeof(pointIdentity) / sizeof(*pointIdentity));

    //INITIALIZE SHADER UNIFORM DATA
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////



    //DEFERRED GEOMETRY PASS
    shaderGeometryPass.use();
    shaderGeometryPass.setInt("albedoMap", 0);
    shaderGeometryPass.setInt("metallicMap", 2);
    shaderGeometryPass.setInt("roughnessMap", 3);

    //DEFERRED LIGHTING PASS
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedo", 2);
    shaderLightingPass.setInt("ssao", 3);
    shaderLightingPass.setInt("lightDepthMap", 4);
    shaderLightingPass.setInt("gPBR", 5);
    shaderLightingPass.setInt("irradianceMap", 6);
    shaderLightingPass.setInt("prefilterMap", 7);
    shaderLightingPass.setInt("brdfLUT", 8);


    //SSAO PASS
    shaderSSAO.use();
    shaderSSAO.setInt("gPosition", 0);
    shaderSSAO.setInt("gNormal", 1);
    shaderSSAO.setInt("texNoise", 2);
    shaderSSAOBlur.use();
    shaderSSAOBlur.setInt("ssaoInput", 0);

    //CUBEMAP SHADER
    glm::mat4 projection = glm::perspective(glm::radians(p->playerCamera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, p->playerCamera.cameraNear, p->playerCamera.cameraFar);
    backgroundShader.use();
    backgroundShader.setMat4("projection", p->playerCamera.projection);



    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
    lightDir = glm::normalize(glm::vec3(1.25, 1.25, 0.85));
    glm::vec3 spawns[8] = 
    {
        glm::vec3(3.85,0.0,54.82),
        glm::vec3(3.38,0.0,-24.151),
        glm::vec3(18.0, 0.0,-3.3),
        glm::vec3(-38.608,0.0,47.7),
        glm::vec3(36.5, 0.0,54.32),
        glm::vec3(12.6, 0.0,14.4),
        glm::vec3(-38.475,0.0,-29.65),
        glm::vec3(28.8, 0.0,-24.8)
    };


    //RANDOMLY SPAWNS DUMMY TARGETS
    srand(time(0));
    int enemyRespawn = rand() % 8;
    int lastSpawn = enemyRespawn;
    float randRot = rand() % 360;
    Actor enemy("Models/GUN/BODY.fbx", "Models/GUN/TESTLEVEL/EnemyCollision.obj", spawns[enemyRespawn], glm::vec3(0.0, (float)glfwGetTime(), 0.0));
    enemyRespawn = rand() % 8;
    Actor enemy2("Models/GUN/BODY.fbx", "Models/GUN/TESTLEVEL/EnemyCollision.obj", spawns[enemyRespawn], glm::vec3(0.0, (float)glfwGetTime(), 0.0));
    enemyRespawn = rand() % 8;
    Actor enemy3("Models/GUN/BODY.fbx", "Models/GUN/TESTLEVEL/EnemyCollision.obj", spawns[enemyRespawn], glm::vec3(0.0, (float)glfwGetTime(), 0.0));

    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    
    glm::vec3 finalPoint = glm::vec3(0.0);
    float rayDistance = 100.0f;


    ///MAINLOOP    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    ///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
    while (!glfwWindowShouldClose(window))
    {
        //CALCULATES UPDATES PER FRAME
        FixedUpdate();



        for (int i = iterations; i > 0; --i)
        {
            ray.vertices[0] = p->playerCamera.cameraPos;
            ray.vertices[1] = p->playerCamera.cameraPos + (p->playerCamera.cameraFront * rayDistance);
            point.vertices[0] = p->playerCamera.cameraPos;
            glm::vec3 closestPoint = glm::vec3(p->playerCamera.cameraPos + (p->playerCamera.cameraFront * rayDistance * 2.0f));
            for (int i = 0; i < enemy.actorCollider.size(); ++i)
            {
                ResolutionData rayCast = GJK(ray, enemy.actorCollider[i], false);
                if (rayCast.hasCollision && p->primary->shootRay)
                {
                    finalPoint = glm::vec3(p->playerCamera.cameraPos + (p->playerCamera.cameraFront * rayDistance * 2.0f));
                    if (i == 0)
                    {
                        headSpeaker.Play(headshot);
                    }

                    ResolutionData hitPoint;
                    for (float j = 0.0f; j < rayDistance; j += 0.1)
                    {
                        point.vertices[1] = p->playerCamera.cameraPos + (p->playerCamera.cameraFront * (j));
                        hitPoint = GJK(point, enemy.actorCollider[i], false);
                        if (hitPoint.hasCollision)
                        {
                            j = rayDistance;

                            if (glm::distance(p->playerCamera.cameraPos, point.vertices[1]) < glm::distance(p->playerCamera.cameraPos, closestPoint))
                            {
                                closestPoint = point.vertices[1];
                            }

                        }
                    }

                    hitSpeaker.Play(hit);
                    worldSpeaker.Play(hurt);
                    randRot = rand() % 360;
                    enemyRespawn = rand() % 8;

                    if (lastSpawn == enemyRespawn)
                    {
                        if (enemyRespawn == 7)
                        {
                            enemyRespawn -= 1;
                        }
                        else
                        {
                            enemyRespawn += 1;
                        }
                    }
                    lastSpawn = enemyRespawn;
                    enemy.setTransform(spawns[enemyRespawn], glm::vec3(0.0));
                }
            }
            enemy.setTransform(enemy.actorPosition, glm::vec3((float)glfwGetTime() * -1.16548f, (float)glfwGetTime() * 3.16548f, (float)glfwGetTime() * 1.16548f));
            if (glm::distance(p->playerCamera.cameraPos, closestPoint) < glm::distance(p->playerCamera.cameraPos, finalPoint))
            {
                finalPoint = closestPoint;
            }
            for (int i = 0; i < enemy2.actorCollider.size(); ++i)
            {

                ResolutionData rayCast = GJK(ray, enemy2.actorCollider[i], false);
                if (rayCast.hasCollision && p->primary->shootRay)
                {
                    finalPoint = glm::vec3(p->playerCamera.cameraPos + (p->playerCamera.cameraFront * rayDistance * 2.0f));
                    ResolutionData hitPoint;
                    for (float j = 0.0f; j < rayDistance; j += 0.1)
                    {
                        point.vertices[1] = p->playerCamera.cameraPos + (p->playerCamera.cameraFront * (j));
                        hitPoint = GJK(point, enemy2.actorCollider[i], false);
                        if (hitPoint.hasCollision)
                        {
                            j = rayDistance;
                            if (glm::distance(p->playerCamera.cameraPos, point.vertices[1]) < glm::distance(p->playerCamera.cameraPos, closestPoint))
                            {
                                closestPoint = point.vertices[1];
                            }
                        }
                    }

                    if (i == 0)
                    {
                        headSpeaker.Play(headshot);
                    }
                    hitSpeaker.Play(hit);
                    worldSpeaker.Play(hurt);
                    randRot = rand() % 360;
                    enemyRespawn = rand() % 8;

                    if (lastSpawn == enemyRespawn)
                    {
                        if (enemyRespawn == 7)
                        {
                            enemyRespawn -= 1;
                        }
                        else
                        {
                            enemyRespawn += 1;
                        }
                    }
                    lastSpawn = enemyRespawn;
                    enemy2.setTransform(spawns[enemyRespawn], glm::vec3((float)glfwGetTime(), (float)glfwGetTime(), (float)glfwGetTime()));

                }

            }
            if (glm::distance(p->playerCamera.cameraPos, closestPoint) < glm::distance(p->playerCamera.cameraPos, finalPoint))
            {
                finalPoint = closestPoint;
            }
            for (int i = 0; i < enemy3.actorCollider.size(); ++i)
            {

                ResolutionData rayCast = GJK(ray, enemy3.actorCollider[i], false);
                if (rayCast.hasCollision && p->primary->shootRay)
                {
                    finalPoint = glm::vec3(p->playerCamera.cameraPos + (p->playerCamera.cameraFront * rayDistance * 2.0f));
                    ResolutionData hitPoint;
                    for (float j = 0.0f; j < rayDistance; j += 0.1)
                    {

                        point.vertices[1] = p->playerCamera.cameraPos + (p->playerCamera.cameraFront * (j));
                        hitPoint = GJK(point, enemy3.actorCollider[i], false);
                        if (hitPoint.hasCollision)
                        {
                            j = rayDistance;
                            if (glm::distance(p->playerCamera.cameraPos, point.vertices[1]) < glm::distance(p->playerCamera.cameraPos, closestPoint))
                            {
                                closestPoint = point.vertices[1];
                            }
                        }
                    }

                    if (i == 0)
                    {
                        headSpeaker.Play(headshot);
                    }
                    hitSpeaker.Play(hit);

                    worldSpeaker.Play(hurt);
                    randRot = rand() % 360;
                    enemyRespawn = rand() % 8;

                    if (lastSpawn == enemyRespawn)
                    {
                        if (enemyRespawn == 7)
                        {
                            enemyRespawn -= 1;
                        }
                        else
                        {
                            enemyRespawn += 1;
                        }
                    }
                    lastSpawn = enemyRespawn;


                }
            }
            if (glm::distance(p->playerCamera.cameraPos, closestPoint) < glm::distance(p->playerCamera.cameraPos, finalPoint))
            {
                finalPoint = closestPoint;
            }
            for (int i = 0; i < shipment.collisionMap.size(); ++i)
            {
                ResolutionData rayCast = GJK(ray, shipment.collisionMap[i], false);
                if (rayCast.hasCollision && p->primary->shootRay)
                {
                    finalPoint = glm::vec3(p->playerCamera.cameraPos + (p->playerCamera.cameraFront * rayDistance * 2.0f));
                    ResolutionData hitPoint;
                    for (float j = 0.0f; j < rayDistance; j += 0.1)
                    {

                        point.vertices[1] = p->playerCamera.cameraPos + (p->playerCamera.cameraFront * (j));
                        hitPoint = GJK(point, shipment.collisionMap[i], false);
                        if (hitPoint.hasCollision)
                        {
                            if (glm::distance(p->playerCamera.cameraPos, point.vertices[1]) < glm::distance(p->playerCamera.cameraPos, closestPoint))
                            {
                                closestPoint = point.vertices[1];
                            }
                        }
                    }
                }
            }
            if (glm::distance(p->playerCamera.cameraPos, closestPoint) < glm::distance(p->playerCamera.cameraPos, finalPoint))
            {
                finalPoint = closestPoint;
            }
        }

        p->update(shipment.collisionMap);

        //RESET FIXED UPDATE ITERATOR
        iterations = 0;

        //gameServer.clientData[4] = -FLT_MAX;
        //gameServer.clientData[5] = -FLT_MAX;
        //gameServer.clientData[6] = -FLT_MAX;
        //gameServer.clientData[7] = -FLT_MAX;
        //gameServer.clientData[0] = p->playerCollider.pos.x;
        //gameServer.clientData[1] = p->playerCollider.pos.y + (1.37);
        //gameServer.clientData[2] = p->playerCollider.pos.z;
        //gameServer.clientData[3] = -p->playerCamera.yaw * 0.0174533;
        //gameServer.getGameState();
        //
        //enemy3.setTransform(glm::vec3(gameServer.Data[4], gameServer.Data[5] + (1.37), gameServer.Data[6]), glm::vec3(0.0f, gameServer.Data[7], 0.0f));

        //networkThread = std::thread(&Server::getGameState, Server());
        //networkThread.join();
        ///COLLISION///             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////
        ///BEHAVIOR ///             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////
        ///PIPELINE ///             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////

     
        
        ///DEFFERED////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////
        ///RENDER  ////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////
        ///PIPELINE////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////             ///////////////

        //RENDER DEPTH MAP FOR SHADOW CALCULATIONS
        glEnable(GL_CULL_FACE);
        const auto lightMatrices = getLightSpaceMatrices();
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, lightMatrices.size() * sizeof(glm::mat4), lightMatrices.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        depthShader.use();
        glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
        glViewport(0, 0, depthMapResolution, depthMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        //STATIC MODELS
        depthShader.setBool("isStatic", true);
        shipment.mapRender(p->playerCamera, depthShader);
        enemy.drawActor(p->playerCamera, depthShader);
        enemy2.drawActor(p->playerCamera, depthShader);
        enemy3.drawActor(p->playerCamera, depthShader);
        
        //ANIMATED MODELS
        depthShader.setBool("isStatic", false);

        //glEnable(GL_CULL_FACE);
        //p->primary->render(p->playerCamera, depthShader, window);
        //glDisable(GL_CULL_FACE);


        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        glViewport(0, 0, fb_width, fb_height);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderGeometryPass.use();
        glEnable(GL_CULL_FACE);
        
        //STATIC MODELS
        shaderGeometryPass.setBool("isStatic", true);
        
        shipment.mapRender(p->playerCamera, shaderGeometryPass);




        enemy.drawActor(p->playerCamera, shaderGeometryPass);
        enemy2.drawActor(p->playerCamera, shaderGeometryPass);
        enemy3.drawActor(p->playerCamera, shaderGeometryPass);
        
        staticRender(p->playerCamera, shaderGeometryPass, contact, finalPoint, glm::vec3(0.0f), glm::vec3(1.0f));
        
        //ANIMATED MODELS
        shaderGeometryPass.setBool("isStatic", false);
        
        //RENDERS VIEWMODEL (CLEARS DEPTH BUFFER SO MODEL ALWAYS RENDERS ON TOP)
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        if (p->scopedIn < 0.8) { p->primary->render(p->playerCamera, shaderGeometryPass, window); }
        glEnable(GL_CULL_FACE);

        // 2. generate SSAO texture
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.use();
        // Send kernel + rotation
        for (unsigned int i = 0; i < 16; ++i) { shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]); }
        shaderSSAO.setMat4("projection", p->playerCamera.projection);
        shaderSSAO.setMat4("view", p->playerCamera.view);
        shaderSSAO.setVec3("camPos", p->playerCamera.cameraPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        renderQuad();

        // 3. blur SSAO texture to remove noise
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAOBlur.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        renderQuad();

        // 4. lighting pass: PBR + IBL LIGHTING
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, fb_width, fb_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //DRAW SKYBOX
        /////////////
        /////////////
        /////////////
        glDisable(GL_CULL_FACE);
        backgroundShader.use();
        backgroundShader.setMat4("view", p->playerCamera.view);
        backgroundShader.setMat4("projection", p->playerCamera.projection);
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); //DRAWS LOW RES MAP
        //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); //DRAWS BLURRED MAP
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap); //DRAWS FULL RES CUBE MAP
        renderCube();
        /////////////
        /////////////
        /////////////
        //END SKYBOX

        glEnable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        shaderLightingPass.use();
        shaderLightingPass.setVec3("camPos", p->playerCamera.cameraPos);
        shaderLightingPass.setVec3("lightDir", lightDir);
        shaderLightingPass.setFloat("farPlane", p->playerCamera.cameraFar);
        shaderLightingPass.setInt("cascadeCount", shadowCascadeLevels.size());
        shaderLightingPass.setMat4("view", p->playerCamera.view);
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
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, gPBR);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (toggleDebug == 1)
        {
            //debug.drawCollider(p->floorCollider, p->playerCamera);
            debug.drawCollider(p->playerCollider, p->playerCamera);
            for (int i = 0; i < enemy.actorCollider.size(); ++i)
            {
                debug.drawCollider(enemy.actorCollider[i], p->playerCamera);
                debug.drawCollider(enemy2.actorCollider[i], p->playerCamera);
                debug.drawCollider(enemy3.actorCollider[i], p->playerCamera);
            }
            for (int i = 0; i < shipment.collisionMap.size(); ++i)
            {
                debug.drawCollider(shipment.collisionMap[i], p->playerCamera);
            }
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            renderQuad();
        }
        p->renderOverlay(defaultShader);
        
        glClear(GL_DEPTH_BUFFER_BIT);
        //DISABLES TRANSPARENCY ALLOWING FOR SKYBOX AND MODEL RENDERING
        glDisable(GL_BLEND);

        // input
        // -----
        glfwSwapBuffers(window);
        glfwPollEvents();
        //debug.debugControls(window, deltaTime);
        showFPS();
        
    }


    //glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}


//UPDATES GAME STATE
void GameUpdate()
{

}

//INITIALIZERS //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////
///////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////    //////////

//CREATES THE FRAME BUFFER FOR THE SHADOWMAP
static void initShadowMap()
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
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
    


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

//CREATES THE FRAME BUFFER FOR SSAO
static void initSSAO()
{
    ssaoKernel.reserve(16);
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

    for (unsigned int i = 0; i < 16; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 512;

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

//INITIALIZED FRAMEBUFFER FOR DEFERRED RENDERING
static void initFramebuffer()
{
    unsigned int rboDepth;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
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
    // PBR Values (METALLIC + ROUGHNESS)
    glGenTextures(1, &gPBR);
    glBindTexture(GL_TEXTURE_2D, gPBR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gPBR, 0);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);
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
static void initPBR(const char* hdrPath, int resolution)
{
    unsigned int captureFBO;
    unsigned int captureRBO;
    unsigned int hdrTexture;

    Shader equirectangularToCubemapShader("SHADERS/cubemap.vs", "SHADERS/cubemapConvert.fs");
    Shader irradianceShader("SHADERS/cubemap.vs", "SHADERS/irradianceConvolution.fs");
    Shader backgroundShader("SHADERS/background.vs", "SHADERS/background.fs");
    Shader prefilterShader("SHADERS/cubemap.vs", "SHADERS/prefilter.fs");
    Shader brdfShader("SHADERS/brdf.vs", "SHADERS/brdf.fs");

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
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
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
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

    glViewport(0, 0, resolution, resolution); // don't forget to configure the viewport to the capture dimensions.
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, resolution, resolution, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, resolution, resolution);
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        toggleDebug *= -1;
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
    const auto proj = glm::perspective(glm::radians(p->playerCamera.fov), (float)fb_width / (float)fb_height, nearPlane, farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, p->playerCamera.view);

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
            ret.push_back(getLightSpaceMatrix(p->playerCamera.cameraNear, shadowCascadeLevels[i]));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], p->playerCamera.cameraFar));
        }
    }
    return ret;
}