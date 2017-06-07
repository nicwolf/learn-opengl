// ############
// Learn OpenGL
// ############

// ********
// Includes
// ********
#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "box.h"
#include "plane.h"
#include "lights.h"

using namespace std;

// *******************
// Function Prototypes
// *******************
void doMovement();
void mouseCallback(GLFWwindow*, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOff, double yOff);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
float clip(float a, float min, float max);

// *****
// Input
// *****
GLfloat mouseXLast, mouseYLast;
bool firstMouseMovement = true;
bool keys[1024];
const glm::vec3 CAMERA_START_POS = glm::vec3(0.0f, 0.0f, 6.0f);
Camera camera(CAMERA_START_POS);

// *****************
// Global Properties
// *****************
GLuint WINDOW_WIDTH, WINDOW_HEIGHT;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool visualizeDepth = false;

// ****
// Main
// ****
int main(int argc, char *argv[])
{
    // GLFW Setup
    // ==========
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    WINDOW_WIDTH  = 800;
    WINDOW_HEIGHT = 600;

    // Window Setup
    // ============
    // Create a window object
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // GLEW Setup
    // ==========
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Viewport Setup
    // ==============
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Geometry Creation
    // =================
    Plane plane = Plane();
    Box light = Box();
    Box cube = Box();

    // Texture Loading
    // ===============

    // Floor Diffuse Map
    // -----------------
    int floorDiffuseMapImgWidth, floorDiffuseMapImgHeight;
    const char* floorDiffuseMapImgPath = "../learn-opengl/assets/bricks2.jpg";
    unsigned char* floorDiffuseMapImg = SOIL_load_image(floorDiffuseMapImgPath, &floorDiffuseMapImgWidth, &floorDiffuseMapImgHeight, 0, SOIL_LOAD_RGB);
    GLuint floorDiffuseMap;
    glGenTextures(1, & floorDiffuseMap);
    glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB,
                 floorDiffuseMapImgWidth, floorDiffuseMapImgHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, floorDiffuseMapImg);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(floorDiffuseMapImg);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Floor Specular Map
    // ------------------
    GLuint floorSpecularMap;
    glGenTextures(1, & floorSpecularMap);
    glBindTexture(GL_TEXTURE_2D, floorSpecularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 100, 100,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Floor Normal Map
    // ----------------
    int floorNormalMapImgWidth, floorNormalMapImgHeight;
    const char* floorNormalMapImgPath = "../learn-opengl/assets/bricks2_normal.jpg";
    unsigned char* floorNormalMapImg = SOIL_load_image(floorNormalMapImgPath, &floorNormalMapImgWidth, &floorNormalMapImgHeight, 0, SOIL_LOAD_RGB);
    GLuint floorNormalMap;
    glGenTextures(1, & floorNormalMap);
    glBindTexture(GL_TEXTURE_2D, floorNormalMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 floorNormalMapImgWidth, floorNormalMapImgHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, floorNormalMapImg);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(floorNormalMapImg);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Floor Height Map
    // ----------------
    int floorHeightMapImgWidth, floorHeightMapImgHeight;
    const char* floorHeightMapImgPath = "../learn-opengl/assets/bricks2_disp.jpg";
    unsigned char* floorHeightMapImg = SOIL_load_image(floorHeightMapImgPath, &floorHeightMapImgWidth, &floorHeightMapImgHeight, 0, SOIL_LOAD_RGB);
    GLuint floorHeightMap;
    glGenTextures(1, & floorHeightMap);
    glBindTexture(GL_TEXTURE_2D, floorHeightMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 floorHeightMapImgWidth, floorHeightMapImgHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, floorHeightMapImg);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(floorHeightMapImg);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Box Diffuse Map
    // ---------------
    int diffuseMapImgWidth, diffuseMapImgHeight;
    const char* diffuseMapImgPath = "../learn-opengl/assets/diffuseMap.png";
    unsigned char* diffuseMapImg = SOIL_load_image(diffuseMapImgPath, &diffuseMapImgWidth, &diffuseMapImgHeight, 0, SOIL_LOAD_RGB);
    GLuint diffuseMap;
    glGenTextures(1, & diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB,
                 diffuseMapImgWidth, diffuseMapImgHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, diffuseMapImg);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    SOIL_free_image_data(diffuseMapImg);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Box Specular Map
    // ----------------
    int specularMapImgWidth, specularMapImgHeight;
    const char* specularMapImgPath = "../learn-opengl/assets/specularMap.png";
    unsigned char* specularMapImg = SOIL_load_image(specularMapImgPath, &specularMapImgWidth, &specularMapImgHeight, 0, SOIL_LOAD_RGB);
    GLuint specularMap;
    glGenTextures(1, & specularMap);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 specularMapImgWidth, specularMapImgHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, specularMapImg);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(specularMapImg);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Shader Compilation
    // ==================
    Shader shaderPhongBase("../learn-opengl/shaders/base.vert",
                           "../learn-opengl/shaders/blinn.frag");
    Shader shaderConstInst("../learn-opengl/shaders/instanced.vert",
                           "../learn-opengl/shaders/constant.frag");
    Shader shaderDepthMap("../learn-opengl/shaders/omnidir-depth.vert",
                          "../learn-opengl/shaders/omnidir-depth.frag",
                          "../learn-opengl/shaders/omnidir-depth.geom");
    Shader shaderPost("../learn-opengl/shaders/post.vert",
                        "../learn-opengl/shaders/post.frag");
    Shader shaderPostDepth("../learn-opengl/shaders/post.vert",
                           "../learn-opengl/shaders/post-depth.frag");
    Shader shaderBlur("../learn-opengl/shaders/post.vert",
                      "../learn-opengl/shaders/blur.frag");

    // Uniform Buffer Setup
    // ====================

    // Transformation Matrix UBO
    // -------------------------
    GLuint uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 128);

    GLuint uboMatricesIndexPhongBase = glGetUniformBlockIndex(shaderPhongBase.Program, "Matrices");
    glUniformBlockBinding(shaderPhongBase.Program, uboMatricesIndexPhongBase, 0);
    GLuint uboMatricesIndexConstInst = glGetUniformBlockIndex(shaderConstInst.Program, "Matrices");
    glUniformBlockBinding(shaderConstInst.Program, uboMatricesIndexConstInst, 0);
    GLuint uboMatricesIndexDepthMap  = glGetUniformBlockIndex(shaderDepthMap.Program, "Matrices");
    glUniformBlockBinding(shaderDepthMap.Program, uboMatricesIndexConstInst, 0);

    // Lighting UBO
    // ------------
    struct Lights {
        PointLight pointLights[4];
        DirectionalLight dirLight;
        ConeLight coneLight;
    } lights;

    GLuint uboLights;
    glGenBuffers(1, &uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboLights, 0, sizeof(lights));

    GLuint uboLightsIndexPhongBase = glGetUniformBlockIndex(shaderPhongBase.Program, "Lights");
    glUniformBlockBinding(shaderPhongBase.Program, uboLightsIndexPhongBase, 1);
    GLuint uboLightsIndexConstInst = glGetUniformBlockIndex(shaderConstInst.Program, "Lights");
    glUniformBlockBinding(shaderConstInst.Program, uboLightsIndexConstInst, 1);

    // Lighting Setup
    // ==============

    // Directional Light
    // -----------------
    lights.dirLight.ambient  = glm::vec3(0.05f, 0.05f, 0.05f);
    lights.dirLight.diffuse  = glm::vec3(0.3f, 0.3f, 0.3f);
    lights.dirLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // Point Light #0
    // --------------
    lights.pointLights[0].ambient  = glm::vec3(0.1f, 0.0f, 0.1f);
    lights.pointLights[0].diffuse  = glm::vec3(0.6f, 0.0f, 0.6f);
    lights.pointLights[0].specular = glm::vec3(1.0f, 0.0f, 1.0f);
    lights.pointLights[0].constantFalloff  = 1.000f;
    lights.pointLights[0].linearFalloff    = 0.090f;
    lights.pointLights[0].quadraticFalloff = 0.032f;

    // Point Light #1
    // --------------
    lights.pointLights[1].ambient  = glm::vec3(0.0f, 0.1f, 0.1f);
    lights.pointLights[1].diffuse  = glm::vec3(0.0f, 0.6f, 0.6f);
    lights.pointLights[1].specular = glm::vec3(0.0f, 1.0f, 1.0f);
    lights.pointLights[1].constantFalloff  = 1.000f;
    lights.pointLights[1].linearFalloff    = 0.090f;
    lights.pointLights[1].quadraticFalloff = 0.032f;

    // Point Light #2
    // --------------
    lights.pointLights[2].ambient  = glm::vec3(0.1f, 0.1f, 0.0f);
    lights.pointLights[2].diffuse  = glm::vec3(0.6f, 0.6f, 0.0f);
    lights.pointLights[2].specular = glm::vec3(1.0f, 1.0f, 0.0f);
    lights.pointLights[2].constantFalloff  = 1.000f;
    lights.pointLights[2].linearFalloff    = 0.090f;
    lights.pointLights[2].quadraticFalloff = 0.032f;

    // Point Light #3
    // --------------
    lights.pointLights[3].ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
    lights.pointLights[3].diffuse  = glm::vec3(6.0f, 6.0f, 6.0f);
    lights.pointLights[3].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lights.pointLights[3].constantFalloff  = 1.000f;
    lights.pointLights[3].linearFalloff    = 0.090f;
    lights.pointLights[3].quadraticFalloff = 0.032f;

    // Cone Light (Flashlight)
    // -----------------------
    lights.coneLight.position  = glm::vec3(0.0f, 0.0f,  0.0f);
    lights.coneLight.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    lights.coneLight.ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
    lights.coneLight.diffuse  = glm::vec3(0.6f, 0.6f, 0.6f);
    lights.coneLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lights.coneLight.cutoff      = glm::cos(glm::radians(12.5));
    lights.coneLight.outerCutoff = glm::cos(glm::radians(18.5));
    lights.coneLight.constantFalloff  = 1.000f;
    lights.coneLight.linearFalloff    = 0.090f;
    lights.coneLight.quadraticFalloff = 0.032f;

    // Upload
    // ------
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), &lights, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Point Light Instancing
    // ----------------------
    GLuint numPointLightInstances = 4;
    glm::mat4 pointLightInstanceModelMatrices[numPointLightInstances];
    glm::vec3 pointLightInstancePositions[] = {
        glm::vec3( 1.0f, -0.5f,  0.0f),
        glm::vec3( 0.0f, -0.5f,  0.0f),
        glm::vec3( 0.0f, -0.5f, -1.0f),
        glm::vec3(-1.0f, -0.5f,  1.0f)
    };
    for (GLuint i = 0; i < numPointLightInstances; i++) {
        glm::mat4 modelMatrix = glm::mat4();
        modelMatrix = glm::translate(modelMatrix, pointLightInstancePositions[i]);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        pointLightInstanceModelMatrices[i] = modelMatrix;
    }
    GLuint lightInstanceBuffer;
    glGenBuffers(1, &lightInstanceBuffer);
    glBindVertexArray(light.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, lightInstanceBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pointLightInstanceModelMatrices), &pointLightInstanceModelMatrices, GL_STATIC_DRAW);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*) ( 0 * sizeof(GLfloat)));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*) ( 4 * sizeof(GLfloat)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*) ( 8 * sizeof(GLfloat)));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*) (12 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Frame Buffer Setup
    // ==================

    // Depth Map FBO
    // -------------
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    const GLuint DEPTH_MAP_WIDTH = 1024, DEPTH_MAP_HEIGHT = 1024;
    GLuint depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (GLuint i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
//    glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Scene FBO
    // ---------
    GLuint sceneFBO;
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    GLuint sceneColorBuffers[2];
    glGenTextures(2, sceneColorBuffers);
    for (unsigned int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, sceneColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, sceneColorBuffers[i], 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint sceneRBO;
    glGenRenderbuffers(1, &sceneRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRBO);

    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping Pong FBO
    // -------------
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Post Processing Setup
    // =====================

    // Screen Geometry Creation
    // ------------------------
    GLfloat screenVertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };
    GLuint screenVBO, screenVAO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_BUFFER, 0);

    // Render Loop
    // ===========
    while(!glfwWindowShouldClose(window)) {

        // Event Processing
        // ----------------
        glfwPollEvents();
        doMovement();
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

//        pointLightInstancePositions[0].y = sin(glfwGetTime() * 0.25) * 2.0;
//        pointLightInstancePositions[1].y = sin(glfwGetTime() * 0.25) * 2.0;
//        pointLightInstancePositions[2].y = sin(glfwGetTime() * 0.25) * 2.0;
//        pointLightInstancePositions[3].y = sin(glfwGetTime() * 0.25) * 2.0;
//        for (GLuint i = 0; i < numPointLightInstances; i++) {
//            glm::mat4 modelMatrix = glm::mat4();
//            modelMatrix = glm::translate(modelMatrix, pointLightInstancePositions[i]);
//            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
//            pointLightInstanceModelMatrices[i] = modelMatrix;
//        }
        GLuint lightInstanceBuffer;
        glGenBuffers(1, &lightInstanceBuffer);
        glBindVertexArray(light.VAO);
            glBindBuffer(GL_ARRAY_BUFFER, lightInstanceBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(pointLightInstanceModelMatrices), &pointLightInstanceModelMatrices, GL_STATIC_DRAW);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*) ( 0 * sizeof(GLfloat)));
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*) ( 4 * sizeof(GLfloat)));
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*) ( 8 * sizeof(GLfloat)));
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*) (12 * sizeof(GLfloat)));
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);
            glEnableVertexAttribArray(5);
            glEnableVertexAttribArray(6);
            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Transformation Matrices
        // -----------------------
        glm::mat4 lightSpaceMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverse;
        glm::mat4 modelMatrix;
        glm::mat4 modelViewMatrix;
        glm::mat4 modelViewMatrixInverseTranspose;
        glm::mat4 modelViewProjectionMatrix;
        GLuint lightSpaceMatrixLocation;
        GLuint viewMatrixLocation;
        GLuint viewMatrixInverseLocation;
        GLuint projectionMatrixLocation;
        GLuint modelMatrixLocation;
        GLuint modelViewMatrixLocation;
        GLuint modelViewMatrixInverseTransposeLocation;
        GLuint modelViewProjectionMatrixLocation;
        glm::vec3 dirLightDirection = glm::vec3(-1.5f, -1.5f, -4.0f);

        // Depth Pass
        // ----------

        // Transformation Matrix Computation
        GLfloat DEPTH_PASS_ASPECT = 1.0; /*(GLfloat) DEPTH_MAP_WIDTH / (GLfloat) DEPTH_MAP_HEIGHT;*/
        GLfloat DEPTH_PASS_NEAR =  0.1;
        GLfloat DEPTH_PASS_FAR  = 15.0;
        projectionMatrix = glm::perspective(glm::radians(90.0f), DEPTH_PASS_ASPECT, DEPTH_PASS_NEAR, DEPTH_PASS_FAR);
        std::vector<glm::mat4> lightSpaceMatrices;
        viewMatrix = glm::lookAt(pointLightInstancePositions[0], pointLightInstancePositions[0] + glm::vec3( 1.0,  0.0,  0.0), glm::vec3( 0.0, -1.0,  0.0));
        lightSpaceMatrices.push_back(projectionMatrix * viewMatrix);
        viewMatrix = glm::lookAt(pointLightInstancePositions[0], pointLightInstancePositions[0] + glm::vec3(-1.0,  0.0,  0.0), glm::vec3( 0.0, -1.0,  0.0));
        lightSpaceMatrices.push_back(projectionMatrix * viewMatrix);
        viewMatrix = glm::lookAt(pointLightInstancePositions[0], pointLightInstancePositions[0] + glm::vec3( 0.0,  1.0,  0.0), glm::vec3( 0.0,  0.0,  1.0));
        lightSpaceMatrices.push_back(projectionMatrix * viewMatrix);
        viewMatrix = glm::lookAt(pointLightInstancePositions[0], pointLightInstancePositions[0] + glm::vec3( 0.0, -1.0,  0.0), glm::vec3( 0.0,  0.0, -1.0));
        lightSpaceMatrices.push_back(projectionMatrix * viewMatrix);
        viewMatrix = glm::lookAt(pointLightInstancePositions[0], pointLightInstancePositions[0] + glm::vec3( 0.0,  0.0,  1.0), glm::vec3( 0.0, -1.0,  0.0));
        lightSpaceMatrices.push_back(projectionMatrix * viewMatrix);
        viewMatrix = glm::lookAt(pointLightInstancePositions[0], pointLightInstancePositions[0] + glm::vec3( 0.0,  0.0, -1.0), glm::vec3( 0.0, -1.0,  0.0));
        lightSpaceMatrices.push_back(projectionMatrix * viewMatrix);

        // Draw
        glViewport(0, 0, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            shaderDepthMap.Use();

                // Send Uniforms
                for (GLuint i = 0; i < 6; ++i)
                    glUniformMatrix4fv(glGetUniformLocation(shaderDepthMap.Program, ("lightSpaceMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrices[i]));
                GLuint nearLocation = glGetUniformLocation(shaderDepthMap.Program, "near");
                glUniform1f(nearLocation, DEPTH_PASS_NEAR);
                GLuint farLocation  = glGetUniformLocation(shaderDepthMap.Program, "far");
                glUniform1f(farLocation, DEPTH_PASS_FAR);
                GLuint lightPosLocation = glGetUniformLocation(shaderDepthMap.Program, "lightPos");
                glUniform3f(lightPosLocation,
                            pointLightInstancePositions[0].x,
                            pointLightInstancePositions[0].y,
                            pointLightInstancePositions[0].z);

                // Draw Plane
                modelMatrix = glm::mat4();
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(5.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                modelMatrixLocation = glGetUniformLocation(shaderDepthMap.Program, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                modelViewMatrix = viewMatrix * modelMatrix;
                modelViewMatrixLocation = glGetUniformLocation(shaderDepthMap.Program, "modelViewMatrix");
                glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

                modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix * modelMatrix));;
                modelViewMatrixInverseTransposeLocation = glGetUniformLocation(shaderDepthMap.Program, "modelViewMatrixInverseTranspose");
                glUniformMatrix4fv(modelViewMatrixInverseTransposeLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrixInverseTranspose));

                modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
                modelViewProjectionMatrixLocation = glGetUniformLocation(shaderDepthMap.Program, "modelViewProjectionMatrix");
                glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

                plane.Draw(shaderDepthMap);

                // Draw Box
                modelMatrix = glm::mat4();
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(68.0f), glm::vec3(0.5f, 0.8f, -0.5f));
                modelMatrixLocation = glGetUniformLocation(shaderDepthMap.Program, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                modelViewMatrix = viewMatrix * modelMatrix;
                modelViewMatrixLocation = glGetUniformLocation(shaderDepthMap.Program, "modelViewMatrix");
                glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

                modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix * modelMatrix));;
                modelViewMatrixInverseTransposeLocation = glGetUniformLocation(shaderDepthMap.Program, "modelViewMatrixInverseTranspose");
                glUniformMatrix4fv(modelViewMatrixInverseTransposeLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrixInverseTranspose));

                modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
                modelViewProjectionMatrixLocation = glGetUniformLocation(shaderDepthMap.Program, "modelViewProjectionMatrix");
                glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

//                cube.Draw(shaderDepthMap);

        // Scene Pass
        // ----------

        // Transformation Matrix Computation
        projectionMatrix = glm::perspective(glm::radians(camera.fov), (GLfloat) WINDOW_WIDTH / (GLfloat) WINDOW_HEIGHT, 0.1f, 15.0f);
        viewMatrix = camera.getViewMatrix();

        // Light UBO Update
        glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
            lights.dirLight.direction = glm::vec3(glm::transpose(glm::inverse(viewMatrix)) * glm::vec4(dirLightDirection, 1.0));
            glBufferSubData(GL_UNIFORM_BUFFER, offsetof(Lights, dirLight.direction), sizeof(glm::vec3), &lights.dirLight.direction);
            lights.pointLights[0].position = viewMatrix * glm::vec4(pointLightInstancePositions[0], 1.0);
            glBufferSubData(GL_UNIFORM_BUFFER, offsetof(Lights, pointLights[0].position), sizeof(glm::vec3), &lights.pointLights[0].position);
            lights.pointLights[1].position = viewMatrix * glm::vec4(pointLightInstancePositions[1], 1.0);
            glBufferSubData(GL_UNIFORM_BUFFER, offsetof(Lights, pointLights[1].position), sizeof(glm::vec3), &lights.pointLights[1].position);
            lights.pointLights[2].position = viewMatrix * glm::vec4(pointLightInstancePositions[2], 1.0);
            glBufferSubData(GL_UNIFORM_BUFFER, offsetof(Lights, pointLights[2].position), sizeof(glm::vec3), &lights.pointLights[2].position);
            lights.pointLights[3].position = viewMatrix * glm::vec4(pointLightInstancePositions[3], 1.0);
            glBufferSubData(GL_UNIFORM_BUFFER, offsetof(Lights, pointLights[3].position), sizeof(glm::vec3), &lights.pointLights[3].position);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Transformation Matrix UBO Update
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER,  0, 64, glm::value_ptr(projectionMatrix));
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(viewMatrix));

        // Draw
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
//            glCullFace(GL_BACK);

            shaderPhongBase.Use();

                // Send Uniforms
                nearLocation = glGetUniformLocation(shaderPhongBase.Program, "nearDepth");
                glUniform1f(nearLocation, DEPTH_PASS_NEAR);
                farLocation  = glGetUniformLocation(shaderPhongBase.Program, "farDepth");
                glUniform1f(farLocation, DEPTH_PASS_FAR);

                // Draw Plane
                modelMatrix = glm::mat4();
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(5.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                modelMatrixLocation = glGetUniformLocation(shaderPhongBase.Program, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                modelViewMatrix = viewMatrix * modelMatrix;
                modelViewMatrixLocation = glGetUniformLocation(shaderPhongBase.Program, "modelViewMatrix");
                glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

                modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix * modelMatrix));;
                modelViewMatrixInverseTransposeLocation = glGetUniformLocation(shaderPhongBase.Program, "modelViewMatrixInverseTranspose");
                glUniformMatrix4fv(modelViewMatrixInverseTransposeLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrixInverseTranspose));

                modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
                modelViewProjectionMatrixLocation = glGetUniformLocation(shaderPhongBase.Program, "modelViewProjectionMatrix");
                glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

                lightSpaceMatrixLocation = glGetUniformLocation(shaderPhongBase.Program, "lightSpaceMatrix");
                glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

                viewMatrixInverse = glm::inverse(viewMatrix);
                viewMatrixInverseLocation = glGetUniformLocation(shaderPhongBase.Program, "viewMatrixInverse");
                glUniformMatrix4fv(viewMatrixInverseLocation, 1, GL_FALSE, glm::value_ptr(viewMatrixInverse));

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.diffuse"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.specular"), 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, floorSpecularMap);

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.normal"), 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, floorNormalMap);

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.depth"), 3);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, floorHeightMap);

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "depthMap"), 4);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

                GLuint materialShininessLoc = glGetUniformLocation(shaderPhongBase.Program, "material.shininess");
                glUniform1f(materialShininessLoc, 32.0f);

                glDisable(GL_CULL_FACE);
                plane.Draw(shaderPhongBase);
                glEnable(GL_CULL_FACE);

                // Draw Box
                modelMatrix = glm::mat4();
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(68.0f), glm::vec3(0.5f, 0.8f, -0.5f));
                modelMatrixLocation = glGetUniformLocation(shaderPhongBase.Program, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                modelViewMatrix = viewMatrix * modelMatrix;
                modelViewMatrixLocation = glGetUniformLocation(shaderPhongBase.Program, "modelViewMatrix");
                glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

                modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix * modelMatrix));;
                modelViewMatrixInverseTransposeLocation = glGetUniformLocation(shaderPhongBase.Program, "modelViewMatrixInverseTranspose");
                glUniformMatrix4fv(modelViewMatrixInverseTransposeLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrixInverseTranspose));

                modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
                modelViewProjectionMatrixLocation = glGetUniformLocation(shaderPhongBase.Program, "modelViewProjectionMatrix");
                glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

                lightSpaceMatrixLocation = glGetUniformLocation(shaderPhongBase.Program, "lightSpaceMatrix");
                glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.diffuse"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, diffuseMap);

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.specular"), 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, specularMap);

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "depthMap"), 4);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

                materialShininessLoc = glGetUniformLocation(shaderPhongBase.Program, "material.shininess");
                glUniform1f(materialShininessLoc, 32.0f);

//                cube.Draw(shaderPhongBase);

            // Draw Lights
            shaderConstInst.Use();
                glUniform3f(glGetUniformLocation(shaderConstInst.Program, "lightColor"), 1.0f, 1.0f, 1.0f);

                modelViewMatrix = viewMatrix * modelMatrix;
                modelViewMatrixLocation = glGetUniformLocation(shaderConstInst.Program, "modelViewMatrix");
                glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

                modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix * modelMatrix));;
                modelViewMatrixInverseTransposeLocation = glGetUniformLocation(shaderConstInst.Program, "modelViewMatrixInverseTranspose");
                glUniformMatrix4fv(modelViewMatrixInverseTransposeLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrixInverseTranspose));

                modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
                modelViewProjectionMatrixLocation = glGetUniformLocation(shaderConstInst.Program, "modelViewProjectionMatrix");
                glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

//                light.DrawInstanced(shaderConstInst, 4);

        // Visualize Depth
        // ---------------
        if (visualizeDepth) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glDisable(GL_DEPTH_TEST);
                shaderPostDepth.Use();
                    GLuint nearLocation = glGetUniformLocation(shaderPostDepth.Program, "near");
                    glUniform1f(nearLocation, DEPTH_PASS_NEAR);
                    GLuint farLocation  = glGetUniformLocation(shaderPostDepth.Program, "far");
                    glUniform1f(nearLocation, DEPTH_PASS_FAR);
                    glBindVertexArray(screenVAO);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                    glBindVertexArray(0);
            glfwSwapBuffers(window);
            continue;
        }

        // Blur Pass
        // ---------
        int horizontal = true, firstPass = true;
        int numPasses = 10;
        shaderBlur.Use();
        glBindVertexArray(screenVAO);
        for (int i=0; i<numPasses; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, firstPass ? sceneColorBuffers[1] : pingpongBuffer[!horizontal]);
            glUniform1i(glGetUniformLocation(shaderBlur.Program, "horizontal"), horizontal);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            horizontal = !horizontal;
            if (firstPass) firstPass = false;
        }

        // Post Pass
        // ---------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_FRAMEBUFFER_SRGB);
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            shaderPost.Use();
                glBindVertexArray(screenVAO);

                glUniform1i(glGetUniformLocation(shaderPost.Program, "colorBuffer"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, sceneColorBuffers[0]);

                glUniform1i(glGetUniformLocation(shaderPost.Program, "bloomBuffer"), 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);

                GLfloat exposure = 2.0;
                glUniform1f(glGetUniformLocation(shaderPost.Program, "exposure"), exposure);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);
        glDisable(GL_FRAMEBUFFER_SRGB);
        glfwSwapBuffers(window);
    }

    // Clean Up
    // ========
    glfwTerminate();

    // Exit
    // ====
    return 0;
}

void doMovement() {
    GLfloat cameraSpeed = 10.0f * deltaTime;
    if (keys[GLFW_KEY_W]) {
        camera.processKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S]) {
        camera.processKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A]) {
        camera.processKeyboard(LEFT, deltaTime);
    }
    if (keys[GLFW_KEY_D]) {
        camera.processKeyboard(RIGHT, deltaTime);
    }
}

void keyCallback(GLFWwindow* window, int key, int scandcode, int action, int mode) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
    // "ESCAPE" Key prompts the window to close at the start of the next loop.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // "Q" Key toggles between solid and wireframe polygon mode.
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        // This tripped me up for a second --- fetching GL_POLYGON_MODE returns
        // two integers, one for the front and one for the back of polygons.
        // Each face can have a different mode.
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);
        if (polygonMode[0] == GL_FILL) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (polygonMode[0] == GL_LINE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    // "E" Key toggles between the 3D scene and a visualiztion of the scene's
    // shadow map.
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        visualizeDepth ^= true;
    }
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouseMovement) {
        mouseXLast = xPos;
        mouseYLast = yPos;
        firstMouseMovement = false;
    }
    GLfloat xOff = xPos - mouseXLast;
    GLfloat yOff = mouseYLast - yPos;
    mouseXLast = xPos;
    mouseYLast = yPos;

    GLfloat sensitivity = 0.2;
    xOff *= sensitivity;
    yOff *= sensitivity;

    camera.processMouseMovement(xOff, yOff);
}

void scrollCallback(GLFWwindow* window, double xOff, double yOff) {
    camera.processMouseScroll(yOff);
}

float clip(float a, float min, float max) {
    return a <= min ? min : a >= max ? max : a;
}
