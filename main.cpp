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
float lerp(float a, float b, float f);

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
bool visualizeTexture = false;
bool ambientOcclusionOn = true;

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
    std::vector<glm::mat4> cubeModelMatrices;
    const unsigned int NR_CUBES = 40;
    for (unsigned int i=0; i<NR_CUBES; ++i) {
        float px = ((rand() % 100) / 100.0) * 4.0 - 2.0;
        float py = ((rand() % 100) / 100.0) * 4.0 - 2.0;
        float pz = ((rand() % 100) / 100.0) * 4.0 - 2.0;
        float rx = ((rand() % 100) / 100.0);
        float ry = ((rand() % 100) / 100.0);
        float rz = ((rand() % 100) / 100.0);
        float r  = ((rand() % 100) / 100.0) * 90;
        float s  = ((rand() % 100) / 100.0);
        glm::mat4 modelMatrix = glm::mat4();
        modelMatrix = glm::translate(modelMatrix, glm::vec3(px, py, pz));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(s));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(r), glm::vec3(rx, ry, rz));
        cubeModelMatrices.push_back(modelMatrix);
    }

    // Texture Loading
    // ===============

    // Brick Diffuse Map
    // -----------------
    int floorDiffuseMapImgWidth, floorDiffuseMapImgHeight;
    const char* floorDiffuseMapImgPath = "../learn-opengl/assets/bricks2.jpg";
    unsigned char* floorDiffuseMapImg = SOIL_load_image(floorDiffuseMapImgPath, &floorDiffuseMapImgWidth, &floorDiffuseMapImgHeight, 0, SOIL_LOAD_RGB);
    GLuint floorDiffuseMap;
    glGenTextures(1, & floorDiffuseMap);
    glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 floorDiffuseMapImgWidth, floorDiffuseMapImgHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, floorDiffuseMapImg);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(floorDiffuseMapImg);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Brick Specular Map
    // ------------------
    GLuint floorSpecularMap;
    glGenTextures(1, & floorSpecularMap);
    glBindTexture(GL_TEXTURE_2D, floorSpecularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 100, 100,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Brick Normal Map
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

    // Brick Height Map
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

    // Shader Compilation
    // ==================
    Shader shaderDeferredGeom("../learn-opengl/shaders/deferred-geom.vert",
                              "../learn-opengl/shaders/deferred-geom.frag");
//    Shader shaderDeferredLight("../learn-opengl/shaders/post.vert",
//                               "../learn-opengl/shaders/post.frag");
    Shader shaderDeferredLight("../learn-opengl/shaders/deferred-light.vert",
                               "../learn-opengl/shaders/deferred-light.frag");
    Shader shaderForwardConst("../learn-opengl/shaders/base.vert",
                              "../learn-opengl/shaders/constant.frag");
    Shader shaderSSAO("../learn-opengl/shaders/screen.vert",
                      "../learn-opengl/shaders/ssao.frag");
    Shader shaderSSAOBlur("../learn-opengl/shaders/screen.vert",
                          "../learn-opengl/shaders/ssao-blur.frag");
    Shader shaderImage("../learn-opengl/shaders/screen.vert",
                       "../learn-opengl/shaders/image.frag");

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

    GLuint uboMatricesIndexPhongBase = glGetUniformBlockIndex(shaderDeferredGeom.Program, "Matrices");
    glUniformBlockBinding(shaderDeferredGeom.Program, uboMatricesIndexPhongBase, 0);

    // Frame Buffer Setup
    // ==================

    // Geometry Buffer
    // ---------------
    GLuint geometryBuffer;
    glGenFramebuffers(1, &geometryBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, geometryBuffer);

    GLuint geometryPositionBuffer;
    glGenTextures(1, &geometryPositionBuffer);
    glBindTexture(GL_TEXTURE_2D, geometryPositionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, geometryPositionBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint geometryNormalBuffer;
    glGenTextures(1, &geometryNormalBuffer);
    glBindTexture(GL_TEXTURE_2D, geometryNormalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, geometryNormalBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint geometryAlbedoSpecularBuffer;
    glGenTextures(1, &geometryAlbedoSpecularBuffer);
    glBindTexture(GL_TEXTURE_2D, geometryAlbedoSpecularBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, geometryAlbedoSpecularBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint sceneRBO;
    glGenRenderbuffers(1, &sceneRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRBO);

    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SSAO Setup
    // ==========

    // Buffer Creation
    // ---------------
    GLuint ssaoFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    GLuint ssaoColorBuffer;
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blur Buffer Creation
    // --------------------
    GLuint ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

    GLuint ssaoBlurColorBuffer;
    glGenTextures(1, &ssaoBlurColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColorBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Sampling Kernel
    // ---------------
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i=0; i<32; ++i) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator));
        sample  = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = (float)i/64.0;
        scale   = lerp(0.1, 1.0, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // Sampling Noise
    // --------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i=0; i<16; ++i) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                        randomFloats(generator) * 2.0 - 1.0,
                        0.0);
        ssaoNoise.push_back(noise);
    }
    GLuint ssaoNoiseTexture;
    glGenTextures(1, &ssaoNoiseTexture);
    glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);


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

    // Lighting Setup
    // ==============
    const unsigned int NR_LIGHTS = 20;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(12);
    for (unsigned int i=0; i<NR_LIGHTS; ++i) {
        float x = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float y = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float z = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(glm::vec3(x, y, z));
        float r = ((rand() % 100) / 100.0);
        float g = ((rand() % 100) / 100.0);
        float b = ((rand() % 100) / 100.0);
        lightColors.push_back(glm::vec3(r, g, b));
    }

    // Render Loop
    // ===========
    while(!glfwWindowShouldClose(window)) {

        glEnable(GL_CULL_FACE);

        // Event Processing
        // ----------------
        glfwPollEvents();
        doMovement();
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Transformation Matrices
        // -----------------------
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 viewMatrixInverse;
        glm::mat4 modelMatrix;
        glm::mat4 modelViewMatrix;
        glm::mat4 modelViewMatrixInverseTranspose;
        glm::mat4 modelViewProjectionMatrix;
        GLuint viewMatrixInverseLocation;
        GLuint modelMatrixLocation;
        GLuint modelViewMatrixLocation;
        GLuint modelViewMatrixInverseTransposeLocation;
        GLuint modelViewProjectionMatrixLocation;

        // Geometry Pass
        // -------------

        // Transformation Matrix Computation
        projectionMatrix = glm::perspective(glm::radians(camera.fov), (GLfloat) WINDOW_WIDTH / (GLfloat) WINDOW_HEIGHT, 0.1f, 15.0f);
        viewMatrix = camera.getViewMatrix();

        // Transformation Matrix UBO Update
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER,  0, 64, glm::value_ptr(projectionMatrix));
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(viewMatrix));

        // Draw
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, geometryBuffer);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            shaderDeferredGeom.Use();

                viewMatrixInverse = glm::inverse(viewMatrix);
                viewMatrixInverseLocation = glGetUniformLocation(shaderDeferredGeom.Program, "viewMatrixInverse");
                glUniformMatrix4fv(viewMatrixInverseLocation, 1, GL_FALSE, glm::value_ptr(viewMatrixInverse));

                glUniform1i(glGetUniformLocation(shaderDeferredGeom.Program, "material.diffuse"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);

                glUniform1i(glGetUniformLocation(shaderDeferredGeom.Program, "material.specular"), 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, floorSpecularMap);

                glUniform1i(glGetUniformLocation(shaderDeferredGeom.Program, "material.normal"), 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, floorNormalMap);

                glUniform1i(glGetUniformLocation(shaderDeferredGeom.Program, "material.depth"), 3);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, floorHeightMap);

                GLuint materialShininessLoc = glGetUniformLocation(shaderDeferredGeom.Program, "material.shininess");
                glUniform1f(materialShininessLoc, 32.0f);

                // Draw Cube
                for (unsigned int i=0; i<NR_CUBES; ++i) {
                    modelMatrix = cubeModelMatrices[i];
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
                    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    modelMatrixLocation = glGetUniformLocation(shaderDeferredGeom.Program, "modelMatrix");
                    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                    modelViewMatrix = viewMatrix * modelMatrix;
                    modelViewMatrixLocation = glGetUniformLocation(shaderDeferredGeom.Program, "modelViewMatrix");
                    glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

                    modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix * modelMatrix));;
                    modelViewMatrixInverseTransposeLocation = glGetUniformLocation(shaderDeferredGeom.Program, "modelViewMatrixInverseTranspose");
                    glUniformMatrix4fv(modelViewMatrixInverseTransposeLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrixInverseTranspose));

                    modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
                    modelViewProjectionMatrixLocation = glGetUniformLocation(shaderDeferredGeom.Program, "modelViewProjectionMatrix");
                    glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

                    cube.Draw(shaderDeferredGeom);
                }

        // SSAO Pass
        // ---------
        if (ambientOcclusionOn) {
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderSSAO.Use();
                for (unsigned int i = 0; i < 64; ++i) {
                    GLuint kernelSampleLocation = glGetUniformLocation(shaderSSAO.Program, ("kernelSamples[" + std::to_string(i) + "]").c_str());
                    glm::vec3 kernelSample = ssaoKernel[i];
                    glUniform3f(kernelSampleLocation, kernelSample.x, kernelSample.y, kernelSample.z);
                }
                glBindVertexArray(screenVAO);
                glUniform1i(glGetUniformLocation(shaderSSAO.Program, "gPosition"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, geometryPositionBuffer);
                glUniform1i(glGetUniformLocation(shaderSSAO.Program, "gNormal"), 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, geometryNormalBuffer);
                glUniform1i(glGetUniformLocation(shaderSSAO.Program, "gAlbedoSpecular"), 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, geometryAlbedoSpecularBuffer);
                glUniform1i(glGetUniformLocation(shaderSSAO.Program, "kernelRotationTexture"), 3);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderSSAOBlur.Use();
                glBindVertexArray(screenVAO);
                glUniform1i(glGetUniformLocation(shaderSSAOBlur.Program, "image"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);
        }

        // Lighting Pass
        // -------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        shaderDeferredLight.Use();
            glBindVertexArray(screenVAO);

            glUniform1i(glGetUniformLocation(shaderDeferredLight.Program, "gPosition"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, geometryPositionBuffer);

            glUniform1i(glGetUniformLocation(shaderDeferredLight.Program, "gNormal"), 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, geometryNormalBuffer);

            glUniform1i(glGetUniformLocation(shaderDeferredLight.Program, "gAlbedoSpecular"), 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, geometryAlbedoSpecularBuffer);

            glUniform1i(glGetUniformLocation(shaderDeferredLight.Program, "ssao"), 3);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);

            GLuint ambientOcclusionSwitchLocation = glGetUniformLocation(shaderDeferredLight.Program, "ambientOcclusionOn");
            glUniform1f(ambientOcclusionSwitchLocation, ambientOcclusionOn);

            for (unsigned int i=0; i<NR_LIGHTS; ++i) {
                GLuint lightPositionLocation = glGetUniformLocation(shaderDeferredLight.Program, ("lights[" + std::to_string(i) + "].position").c_str());
                glUniform3f(lightPositionLocation, lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
                GLuint lightColorLocation = glGetUniformLocation(shaderDeferredLight.Program, ("lights[" + std::to_string(i) + "].color").c_str());
                glUniform3f(lightColorLocation, lightColors[i].x, lightColors[i].y, lightColors[i].z);
                GLuint lightConstFalloffLocation = glGetUniformLocation(shaderDeferredLight.Program, ("lights[" + std::to_string(i) + "].constFalloff").c_str());
                glUniform1f(lightConstFalloffLocation, 0.3);
                GLuint lightLinFalloffLocation = glGetUniformLocation(shaderDeferredLight.Program, ("lights[" + std::to_string(i) + "].linFalloff").c_str());
                glUniform1f(lightLinFalloffLocation, 0.7);
                GLuint lightQuadFalloffLocation = glGetUniformLocation(shaderDeferredLight.Program, ("lights[" + std::to_string(i) + "].quadFalloff").c_str());
                glUniform1f(lightQuadFalloffLocation, 1.8);
            }
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);

        // Forward Render Lights
        // ---------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, geometryBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        shaderForwardConst.Use();
            for (unsigned int i=0; i<NR_LIGHTS; ++i) {
                glm::vec3 lightPosition = lightPositions[i];
                glm::vec3 lightColor    = lightColors[i];

                modelMatrix = glm::mat4();
                modelMatrix = glm::translate(modelMatrix, lightPosition);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
                modelMatrixLocation = glGetUniformLocation(shaderForwardConst.Program, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                modelViewMatrix = viewMatrix * modelMatrix;
                modelViewMatrixLocation = glGetUniformLocation(shaderForwardConst.Program, "modelViewMatrix");
                glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

                modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
                modelViewProjectionMatrixLocation = glGetUniformLocation(shaderForwardConst.Program, "modelViewProjectionMatrix");
                glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

                GLuint lightColorLocation = glGetUniformLocation(shaderForwardConst.Program, "color");
                glUniform3f(lightColorLocation, lightColor.r, lightColor.g, lightColor.b);

                cube.Draw(shaderForwardConst);
            }

        // Draw Texture
        // ------------
        if (visualizeTexture) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderImage.Use();
                glBindVertexArray(screenVAO);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);

        }

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
    // "T" Key toggles between the 3D scene and a visualiztion of the scene's
    // ssao map.
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        visualizeTexture ^= true;
    }
    // "G" Key toggles SSAO on/off
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        ambientOcclusionOn ^= true;
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

float lerp(float a, float b, float f) {
    return a + f*(b - a);
}
