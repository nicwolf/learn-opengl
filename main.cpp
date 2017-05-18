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

// Function Prototypes
void do_movement();
void mouse_callback(GLFWwindow*, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOff, double yOff);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
float clip(float a, float min, float max);

// Input Monitoring
GLfloat mouseXLast = 400, mouseYLast = 300;
bool firstMouseMovement = true;
bool keys[1024];

// Camera Setup
Camera camera(glm::vec3(0.0f, 0.5f, 2.0f));

// Global Properties
GLfloat mixConstant;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main(int argc, char *argv[])
{
    // Administrivia
    // -------------
    // Initialize GLFW
    glfwInit();
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    const GLuint WIDTH  = 800;
    const GLuint HEIGHT = 600;

    // Window Setup
    // ------------
    // Create a window object
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make the window object we created the focus of the current context
    glfwMakeContextCurrent(window);
    // Register our key callbacks
    glfwSetKeyCallback(window, key_callback);
    // Capture Mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // GLEW Setup
    // ----------
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Viewport Setup
    // --------------
    glViewport(0, 0, WIDTH, HEIGHT);

    // OpenGL Options
    // --------------
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);

    // Create Geometry
    // ---------------
    Plane plane = Plane();
    Box light = Box();
    Box cube = Box();

    // Setup Textures
    // --------------
    // Floor Diffuse Map
    int floorDiffuseMapImgWidth, floorDiffuseMapImgHeight;
    const char* floorDiffuseMapImgPath = "../learn-opengl/assets/wall.jpg";
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
    GLuint floorSpecularMap;
    glGenTextures(1, & floorSpecularMap);
    glBindTexture(GL_TEXTURE_2D, floorSpecularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 100, 100,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Box Diffuse Map
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

    // Load Shaders
    // ------------
    Shader shaderPhongBase("../learn-opengl/shaders/test.vert",
                           "../learn-opengl/shaders/test.frag");
    Shader shaderConstInst("../learn-opengl/shaders/instanced.vert",
                           "../learn-opengl/shaders/constant.frag");
    Shader shaderDepthMap("../learn-opengl/shaders/depth.vert",
                          "../learn-opengl/shaders/depth.frag");
    Shader shaderScreen("../learn-opengl/shaders/screen.vert",
                        "../learn-opengl/shaders/screen.frag");

    // Transformation Matrix Uniform Buffers
    // -------------------------------------
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

    // Lighting Uniform Buffer
    // -----------------------
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

    // Lighting
    // --------
    // Directional Light
    lights.dirLight.ambient  = glm::vec3(0.05f, 0.05f, 0.05f);
    lights.dirLight.diffuse  = glm::vec3(0.3f, 0.3f, 0.3f);
    lights.dirLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // Point Light #0
    lights.pointLights[0].ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
    lights.pointLights[0].diffuse  = glm::vec3(0.6f, 0.6f, 0.6f);
    lights.pointLights[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lights.pointLights[0].constantFalloff  = 1.000f;
    lights.pointLights[0].linearFalloff    = 0.090f;
    lights.pointLights[0].quadraticFalloff = 0.032f;

    // Point Light #1
    lights.pointLights[1].ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
    lights.pointLights[1].diffuse  = glm::vec3(0.6f, 0.6f, 0.6f);
    lights.pointLights[1].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lights.pointLights[1].constantFalloff  = 1.000f;
    lights.pointLights[1].linearFalloff    = 0.090f;
    lights.pointLights[1].quadraticFalloff = 0.032f;

    // Point Light #2
    lights.pointLights[2].ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
    lights.pointLights[2].diffuse  = glm::vec3(0.6f, 0.6f, 0.6f);
    lights.pointLights[2].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lights.pointLights[2].constantFalloff  = 1.000f;
    lights.pointLights[2].linearFalloff    = 0.090f;
    lights.pointLights[2].quadraticFalloff = 0.032f;

    // Point Light #3
    lights.pointLights[3].ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
    lights.pointLights[3].diffuse  = glm::vec3(0.6f, 0.6f, 0.6f);
    lights.pointLights[3].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lights.pointLights[3].constantFalloff  = 1.000f;
    lights.pointLights[3].linearFalloff    = 0.090f;
    lights.pointLights[3].quadraticFalloff = 0.032f;

    // Cone Light (Flashlight)
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

    // Send to UBO
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), &lights, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Lights
    // ------
    GLuint numPointLightInstances = 4;
    glm::mat4 pointLightInstanceModelMatrices[numPointLightInstances];
    glm::vec3 pointLightInstancePositions[] = {
        glm::vec3( 0.0f,  1.5f,  -1.0f),
        glm::vec3( 0.0f,  1.5f,  -1.0f),
        glm::vec3( 0.0f,  1.5f,  -1.0f),
        glm::vec3( 0.0f,  1.5f,  -1.0f)
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

    // Shadow Map FBO
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    const GLuint DEPTH_MAP_WIDTH = 1024, DEPTH_MAP_HEIGHT = 1024;

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Screenbuffer FBO
    GLuint screenFBO;
    glGenFramebuffers(1, &screenFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

    GLuint screenColorBuffer;
    glGenTextures(1, &screenColorBuffer);
    glBindTexture(GL_TEXTURE_2D, screenColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorBuffer, 0);

    GLuint screenRBO;
    glGenRenderbuffers(1, &screenRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, screenRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRBO);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
    // -----------
    while(!glfwWindowShouldClose(window)) {
        // Check if any events need to be processed
        glfwPollEvents();
        do_movement();

        // Store the current time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Settings
        glEnable(GL_DEPTH_TEST);

        // Transformation Matrices
        glm::mat4 lightSpaceMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 modelMatrix;
        glm::mat4 modelViewMatrix;
        glm::mat4 modelViewMatrixInverseTranspose;
        glm::mat4 modelViewProjectionMatrix;
        GLuint lightSpaceMatrixLocation;
        GLuint viewMatrixLocation;
        GLuint projectionMatrixLocation;
        GLuint modelMatrixLocation;
        GLuint modelViewMatrixLocation;
        GLuint modelViewMatrixInverseTransposeLocation;
        GLuint modelViewProjectionMatrixLocation;

        glViewport(0, 0, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            // Clear Buffer
            glClear(GL_DEPTH_BUFFER_BIT);

            // Scene Transformation Matrices
            GLfloat SHADOW_PASS_NEAR_PLANE = 0.1f, SHADOW_PASS_FAR_PLANE = 25.0f;
            projectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, SHADOW_PASS_NEAR_PLANE, SHADOW_PASS_FAR_PLANE);
            viewMatrix = glm::lookAt(glm::vec3( 1.0f,  4.0f,  1.0f),
                                     glm::vec3( 0.0f,  0.0f,  0.0f),
                                     glm::vec3( 0.0f,  1.0f,  0.0f));
            lightSpaceMatrix = projectionMatrix * viewMatrix;
            glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
            glBufferSubData(GL_UNIFORM_BUFFER,  0, 64, glm::value_ptr(projectionMatrix));
            glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(viewMatrix));

            // Draw Plane
            shaderDepthMap.Use();
                modelMatrix = glm::mat4();
                modelMatrix = glm::scale(modelMatrix, glm::vec3(15.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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

                // Draw
//                glDisable(GL_CULL_FACE);
                plane.Draw(shaderDepthMap);
//                glEnable(GL_CULL_FACE);

            // Draw Box
            shaderDepthMap.Use();
                modelMatrix = glm::mat4();
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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

                // Draw
                cube.Draw(shaderDepthMap);

        glViewport(0, 0, WIDTH, HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
            // Clear buffer
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Scene Transformation Matrices
            projectionMatrix = glm::perspective(glm::radians(camera.fov), (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 15.0f);
            viewMatrix = camera.getViewMatrix();
            glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
            glBufferSubData(GL_UNIFORM_BUFFER,  0, 64, glm::value_ptr(projectionMatrix));
            glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(viewMatrix));

            // Scene Lighting
            glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
                lights.dirLight.direction = glm::vec3(glm::transpose(glm::inverse(viewMatrix)) * glm::vec4(-0.5f, -1.0f, 0.0f, 1.0));
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

            // Draw Plane
            shaderPhongBase.Use();
                modelMatrix = glm::mat4();
                modelMatrix = glm::scale(modelMatrix, glm::vec3(15.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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

                // Bind the Diffuse Map
                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.diffuse"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);

                // Bind the Diffuse Map
                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "diffuse"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);

                // Bind the Specular Map
                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.specular"), 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, floorSpecularMap);

                // Bind the Shadow Map
                GLuint depthMapLocation = glGetUniformLocation(shaderPhongBase.Program, "depthMap");
                glUniform1i(depthMapLocation, 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, depthMap);

                // Send Uniforms
                // -------------
                // Material
                GLuint materialShininessLoc = glGetUniformLocation(shaderPhongBase.Program, "material.shininess");
                glUniform1f(materialShininessLoc, 32.0f);

                // Draw
//                glDisable(GL_CULL_FACE);
                plane.Draw(shaderPhongBase);
//                glEnable(GL_CULL_FACE);

            // Draw Box
            shaderPhongBase.Use();
                modelMatrix = glm::mat4();
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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

                // Bind the Diffuse Map
                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.diffuse"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, diffuseMap);

                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "diffuse"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, diffuseMap);

                // Bind the Specular Map
                glUniform1i(glGetUniformLocation(shaderPhongBase.Program, "material.specular"), 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, specularMap);

                depthMapLocation = glGetUniformLocation(shaderPhongBase.Program, "depthMap");
                glUniform1i(depthMapLocation, 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, depthMap);

                // Send Uniforms
                // -------------
                // Material
                materialShininessLoc = glGetUniformLocation(shaderPhongBase.Program, "material.shininess");
                glUniform1f(materialShininessLoc, 32.0f);

                // Draw
                cube.Draw(shaderPhongBase);

            // Draw Light
            shaderConstInst.Use();
                // Send colors
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

                // Draw
                // light.DrawInstanced(shaderConstInst, 4);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_FRAMEBUFFER_SRGB);
            // Clear buffer
            glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderScreen.Use();
                glBindVertexArray(screenVAO);
                glDisable(GL_DEPTH_TEST);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, screenColorBuffer);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);

        // Swap the current color buffer out for the one just drawn
        glfwSwapBuffers(window);
        glDisable(GL_FRAMEBUFFER_SRGB);

    }

    // Clean Up
    // --------
    glfwTerminate();

    // Exit
    // ----
    return 0;
}

void do_movement() {
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

void key_callback(GLFWwindow* window, int key, int scandcode, int action, int mode) {
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
    // "W" Key toggles between solid and wireframe polygon mode.
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
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        mixConstant += 0.08;
        mixConstant = clip(mixConstant, 0.0, 1.0);
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        mixConstant -= 0.08;
        mixConstant = clip(mixConstant, 0.0, 1.0);
    }
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
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

void scroll_callback(GLFWwindow* window, double xOff, double yOff) {
    camera.processMouseScroll(yOff);
}

float clip(float a, float min, float max) {
    return a <= min ? min : a >= max ? max : a;
}
