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
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));

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
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // OpenGL Options
    // --------------
    glEnable(GL_DEPTH_TEST);

    // Create Geometry Data
    // --------------------
    // Define the vertices for a cube
    GLfloat vertices[] = {
        // Positions          // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // Setup Textures
    // --------------
    // Diffuse Map
    int diffuseMapImgWidth, diffuseMapImgHeight;
    const char* diffuseMapImgPath = "../learn-opengl/assets/diffuseMap.png";
    unsigned char* diffuseMapImg = SOIL_load_image(diffuseMapImgPath, &diffuseMapImgWidth, &diffuseMapImgHeight, 0, SOIL_LOAD_RGB);
    GLuint diffuseMap;
    glGenTextures(1, & diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 diffuseMapImgWidth, diffuseMapImgHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, diffuseMapImg);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(diffuseMapImg);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Specular Map
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

    // Setup Buffers
    // -------------
    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(VAO);
        // Bind VBO into an OpenGL Array Buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Setup Attribute 0: Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0);
        glEnableVertexAttribArray(0);
        // Setup Attribute 1: Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // Setup Attribute 2: Tex Coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    // Unbind the VAO
    glBindVertexArray(0);

    glBindVertexArray(lightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Load Shaders
    // ------------
    Shader cubeShader("../learn-opengl/shaders/base.vert",
                      "../learn-opengl/shaders/phong.frag");
    Shader lightShader("../learn-opengl/shaders/base.vert",
                       "../learn-opengl/shaders/constant.frag");

    // Lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,   2.0f),
        glm::vec3( 2.3f, -3.3f,  -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f,  -3.0f)
    };

    // Render Loop
    // -----------
    while(!glfwWindowShouldClose(window)) {
        // Check if any events need to be processed
        glfwPollEvents();
        do_movement();

        // Clear buffer
        glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Store the current time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Transformation Matrices

        // Draw Cube
        cubeShader.Use();
        glBindVertexArray(VAO);
            glm::mat4 viewMatrix;
            viewMatrix = camera.getViewMatrix();
            GLuint viewMatrixLocation = glGetUniformLocation(cubeShader.Program, "viewMatrix");
            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

            glm::mat4 projectionMatrix;
            projectionMatrix = glm::perspective(glm::radians(camera.fov), (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);
            GLuint projectionMatrixLocation = glGetUniformLocation(cubeShader.Program, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            glm::mat4 modelMatrix;
            modelMatrix = glm::mat4();
            GLuint modelMatrixLocation = glGetUniformLocation(cubeShader.Program, "modelMatrix");
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

            glm::mat4 modelViewMatrix;
            modelViewMatrix = viewMatrix * modelMatrix;
            GLuint modelViewMatrixLocation = glGetUniformLocation(cubeShader.Program, "modelViewMatrix");
            glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

            glm::mat4 modelViewMatrixInverseTranspose;
            modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix * modelMatrix));;
            GLuint modelViewMatrixInverseTransposeLocation = glGetUniformLocation(cubeShader.Program, "modelViewMatrixInverseTranspose");
            glUniformMatrix4fv(modelViewMatrixInverseTransposeLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrixInverseTranspose));

            glm::mat4 modelViewProjectionMatrix;
            modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
            GLuint modelViewProjectionMatrixLocation = glGetUniformLocation(cubeShader.Program, "modelViewProjectionMatrix");
            glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

            // Bind the Diffuse Map
            glUniform1i(glGetUniformLocation(cubeShader.Program, "material.diffuse"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);

            // Bind the Specular Map
            glUniform1i(glGetUniformLocation(cubeShader.Program, "material.specular"), 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);

            // Send Uniforms
            // -------------
            // Material
            GLint materialAmbientLoc   = glGetUniformLocation(cubeShader.Program, "material.ambient");
            GLint materialDiffuseLoc   = glGetUniformLocation(cubeShader.Program, "material.diffuse");
            GLint materialSpecularLoc  = glGetUniformLocation(cubeShader.Program, "material.specular");
            GLint materialShininessLoc = glGetUniformLocation(cubeShader.Program, "material.shininess");

            glUniform3f(materialAmbientLoc,   1.0f, 0.5f, 0.31f);
            glUniform3f(materialDiffuseLoc,   1.0f, 0.5f, 0.31f);
            glUniform3f(materialSpecularLoc,  0.5f, 0.5f, 0.5f);
            glUniform1f(materialShininessLoc, 32.0f);

            // Lighting
            // --------
            // Directional Light
            GLint lightDirectionLoc = glGetUniformLocation(cubeShader.Program, "dirLight.direction");
            GLint lightAmbientLoc   = glGetUniformLocation(cubeShader.Program, "dirLight.ambient");
            GLint lightDiffuseLoc   = glGetUniformLocation(cubeShader.Program, "dirLight.diffuse");
            GLint lightSpecularLoc  = glGetUniformLocation(cubeShader.Program, "dirLight.specular");

            glm::vec4 lightDirection = glm::transpose(glm::inverse(viewMatrix * modelMatrix)) * glm::vec4(0.2f, -0.2f, 0.0f, 1.0);
            glUniform3f(lightDirectionLoc, lightDirection.x, lightDirection.y, lightDirection.z);

            glUniform3f(lightAmbientLoc,  0.2f, 0.2f, 0.2f);
            glUniform3f(lightDiffuseLoc,  0.3f, 0.3f, 0.3f);
            glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

            // Point Light #0
            GLint lightPosLoc      = glGetUniformLocation(cubeShader.Program, "pointLights[0].position");
            lightAmbientLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[0].ambient");
            lightDiffuseLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[0].diffuse");
            lightSpecularLoc = glGetUniformLocation(cubeShader.Program, "pointLights[0].specular");
            GLint constantFallLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[0].constantFalloff");
            GLint linearFallLoc    = glGetUniformLocation(cubeShader.Program, "pointLights[0].linearFalloff");
            GLint quadraticFallLoc = glGetUniformLocation(cubeShader.Program, "pointLights[0].quadraticFalloff");

            glm::vec4 lightPosEye = viewMatrix * glm::vec4(pointLightPositions[0], 1.0);
            glUniform3f(lightPosLoc, lightPosEye.x, lightPosEye.y, lightPosEye.z);

            glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
            glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
            glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

            glUniform1f(constantFallLoc,    1.0f);
            glUniform1f(linearFallLoc,     0.09f);
            glUniform1f(quadraticFallLoc, 0.032f);

            // Point Light #1
            lightPosLoc      = glGetUniformLocation(cubeShader.Program, "pointLights[1].position");
            lightAmbientLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[1].ambient");
            lightDiffuseLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[1].diffuse");
            lightSpecularLoc = glGetUniformLocation(cubeShader.Program, "pointLights[1].specular");
            constantFallLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[1].constantFalloff");
            linearFallLoc    = glGetUniformLocation(cubeShader.Program, "pointLights[1].linearFalloff");
            quadraticFallLoc = glGetUniformLocation(cubeShader.Program, "pointLights[1].quadraticFalloff");

            lightPosEye = viewMatrix * glm::vec4(pointLightPositions[1], 1.0);
            glUniform3f(lightPosLoc, lightPosEye.x, lightPosEye.y, lightPosEye.z);

            glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
            glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
            glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

            glUniform1f(constantFallLoc,    1.0f);
            glUniform1f(linearFallLoc,     0.09f);
            glUniform1f(quadraticFallLoc, 0.032f);

            // Point Light #2
            lightPosLoc      = glGetUniformLocation(cubeShader.Program, "pointLights[2].position");
            lightAmbientLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[2].ambient");
            lightDiffuseLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[2].diffuse");
            lightSpecularLoc = glGetUniformLocation(cubeShader.Program, "pointLights[2].specular");
            constantFallLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[2].constantFalloff");
            linearFallLoc    = glGetUniformLocation(cubeShader.Program, "pointLights[2].linearFalloff");
            quadraticFallLoc = glGetUniformLocation(cubeShader.Program, "pointLights[2].quadraticFalloff");

            lightPosEye = viewMatrix * glm::vec4(pointLightPositions[2], 1.0);
            glUniform3f(lightPosLoc, lightPosEye.x, lightPosEye.y, lightPosEye.z);

            glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
            glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
            glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

            glUniform1f(constantFallLoc,    1.0f);
            glUniform1f(linearFallLoc,     0.09f);
            glUniform1f(quadraticFallLoc, 0.032f);

            // Point Light #3
            lightPosLoc      = glGetUniformLocation(cubeShader.Program, "pointLights[3].position");
            lightAmbientLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[3].ambient");
            lightDiffuseLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[3].diffuse");
            lightSpecularLoc = glGetUniformLocation(cubeShader.Program, "pointLights[3].specular");
            constantFallLoc  = glGetUniformLocation(cubeShader.Program, "pointLights[3].constantFalloff");
            linearFallLoc    = glGetUniformLocation(cubeShader.Program, "pointLights[3].linearFalloff");
            quadraticFallLoc = glGetUniformLocation(cubeShader.Program, "pointLights[3].quadraticFalloff");

            lightPosEye = viewMatrix * glm::vec4(pointLightPositions[3], 1.0);
            glUniform3f(lightPosLoc, lightPosEye.x, lightPosEye.y, lightPosEye.z);

            glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
            glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
            glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

            glUniform1f(constantFallLoc,    1.0f);
            glUniform1f(linearFallLoc,     0.09f);
            glUniform1f(quadraticFallLoc, 0.032f);

            // Cone Light (Flashlight)
            lightPosLoc        = glGetUniformLocation(cubeShader.Program, "coneLight.position");
            lightAmbientLoc    = glGetUniformLocation(cubeShader.Program, "coneLight.ambient");
            lightDiffuseLoc    = glGetUniformLocation(cubeShader.Program, "coneLight.diffuse");
            lightSpecularLoc   = glGetUniformLocation(cubeShader.Program, "coneLight.specular");
            lightDirectionLoc  = glGetUniformLocation(cubeShader.Program, "coneLight.direction");
            constantFallLoc  = glGetUniformLocation(cubeShader.Program, "coneLight.constantFalloff");
            linearFallLoc    = glGetUniformLocation(cubeShader.Program, "coneLight.linearFalloff");
            quadraticFallLoc = glGetUniformLocation(cubeShader.Program, "coneLight.quadraticFalloff");
            GLint lightCutoffLoc     = glGetUniformLocation(cubeShader.Program, "coneLight.cutoff");
            GLint lightOuterCutoffLoc = glGetUniformLocation(cubeShader.Program, "coneLight.outerCutoff");

            glUniform3f(lightPosLoc,       0.0f, 0.0f,  0.0f);
            glUniform3f(lightDirectionLoc, 0.0f, 0.0f, -1.0f);

            glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
            glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
            glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

            glUniform1f(lightCutoffLoc,      glm::cos(glm::radians(12.5)));
            glUniform1f(lightOuterCutoffLoc, glm::cos(glm::radians(18.5)));

            glUniform1f(constantFallLoc, 1.0f);
            glUniform1f(linearFallLoc, 0.09f);
            glUniform1f(quadraticFallLoc, 0.032f);

            // Draw
            glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        // Draw Light
        lightShader.Use();
        glBindVertexArray(lightVAO);
            for (int i = 0; i < 4; i ++) {
                // Send colors
                glUniform3f(glGetUniformLocation(lightShader.Program, "lightColor"), 1.0f, 1.0f, 1.0f);

                glm::mat4 viewMatrix;
                viewMatrix = camera.getViewMatrix();
                GLuint viewMatrixLocation = glGetUniformLocation(lightShader.Program, "viewMatrix");
                glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

                glm::mat4 projectionMatrix;
                projectionMatrix = glm::perspective(glm::radians(camera.fov), (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);
                GLuint projectionMatrixLocation = glGetUniformLocation(lightShader.Program, "projectionMatrix");
                glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

                glm::mat4 modelMatrix;
                modelMatrix = glm::mat4();
                modelMatrix = glm::translate(modelMatrix, pointLightPositions[i]);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
                GLuint modelMatrixLocation = glGetUniformLocation(lightShader.Program, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                glm::mat4 modelViewMatrix;
                modelViewMatrix = viewMatrix * modelMatrix;
                GLuint modelViewMatrixLocation = glGetUniformLocation(lightShader.Program, "modelViewMatrix");
                glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

                glm::mat4 modelViewMatrixInverseTranspose;
                modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix * modelMatrix));;
                GLuint modelViewMatrixInverseTransposeLocation = glGetUniformLocation(lightShader.Program, "modelViewMatrixInverseTranspose");
                glUniformMatrix4fv(modelViewMatrixInverseTransposeLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrixInverseTranspose));

                glm::mat4 modelViewProjectionMatrix;
                modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
                GLuint modelViewProjectionMatrixLocation = glGetUniformLocation(lightShader.Program, "modelViewProjectionMatrix");
                glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

                // Draw
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        glBindVertexArray(0);
        // Swap the current color buffer out for the one just drawn
        glfwSwapBuffers(window);

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
