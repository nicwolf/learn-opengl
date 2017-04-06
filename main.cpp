#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/light.h>

#include "shader.h"
#include "camera.h"
#include "model.h"

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
Camera camera(glm::vec3(0.5f, 1.0f, 4.0f));

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
    GLfloat cubeVertices[] = {
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

    // Setup Graphics Memory
    // ---------------------
    // Generate a VBO
    // This stores our vertex attributes.
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Generate a Lighting VAO
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);

    // Setup VAOs
    // ----------
    glBindVertexArray(lightVAO);
        // Bind VBO into an OpenGL Array Buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        // Setup Attribute 0: Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0);
        glEnableVertexAttribArray(0);
    // Unbind the VAO
    glBindVertexArray(0);

    // Light Settings
    // --------------
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,   2.0f),
        glm::vec3( 2.3f, -3.3f,  -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f,  -3.0f)
    };

    // Load Shaders
    // ------------
    Shader shader("../learn-opengl/shaders/nanosuit.vert",
                  "../learn-opengl/shaders/nanosuit.frag");

    Shader lightShader("../learn-opengl/shaders/light.vert",
                       "../learn-opengl/shaders/light.frag");

    // Load Models
    // -----------
    Model nanosuitModel = Model("../learn-opengl/assets/models/nanosuit/nanosuit.obj");

    // Render Loop
    // -----------
    while(!glfwWindowShouldClose(window)) {
        // Event Handling
        // --------------
        glfwPollEvents();
        do_movement();

        // Time Data
        // ---------
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Clear buffer
        // ------------
        glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate our Shader
        // -------------------
        shader.Use();

        // Transformation Matrices
        // -----------------------
        glm::mat4 view;
        glm::mat4 perspective;
        glm::mat4 model;

        view        = camera.getViewMatrix();
        perspective = glm::perspective(glm::radians(camera.fov), (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);
        model       = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
        model       = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene

        GLuint viewMatrixLoc;
        GLuint perspectiveMatrixLoc;
        GLuint modelMatrixLoc;

        viewMatrixLoc        = glGetUniformLocation(shader.Program, "view");
        perspectiveMatrixLoc = glGetUniformLocation(shader.Program, "perspective");
        modelMatrixLoc       = glGetUniformLocation(shader.Program, "model");

        glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(perspectiveMatrixLoc, 1, GL_FALSE, glm::value_ptr(perspective));
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Lighting
        // --------
        // Directional Light
        GLint lightDirectionLoc = glGetUniformLocation(shader.Program, "dirLight.direction");
        GLint lightAmbientLoc   = glGetUniformLocation(shader.Program, "dirLight.ambient");
        GLint lightDiffuseLoc   = glGetUniformLocation(shader.Program, "dirLight.diffuse");
        GLint lightSpecularLoc  = glGetUniformLocation(shader.Program, "dirLight.specular");

        glm::vec4 lightDirection = glm::transpose(glm::inverse(view * model)) * glm::vec4(0.2f, -0.2f, 0.0f, 1.0);
        glUniform3f(lightDirectionLoc, lightDirection.x, lightDirection.y, lightDirection.z);

        glUniform3f(lightAmbientLoc,  0.2f, 0.2f, 0.2f);
        glUniform3f(lightDiffuseLoc,  0.3f, 0.3f, 0.3f);
        glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

        // Point Light #0
        GLint lightPosLoc      = glGetUniformLocation(shader.Program, "pointLights[0].position");
        lightAmbientLoc  = glGetUniformLocation(shader.Program, "pointLights[0].ambient");
        lightDiffuseLoc  = glGetUniformLocation(shader.Program, "pointLights[0].diffuse");
        lightSpecularLoc = glGetUniformLocation(shader.Program, "pointLights[0].specular");
        GLint constantFallLoc  = glGetUniformLocation(shader.Program, "pointLights[0].constantFalloff");
        GLint linearFallLoc    = glGetUniformLocation(shader.Program, "pointLights[0].linearFalloff");
        GLint quadraticFallLoc = glGetUniformLocation(shader.Program, "pointLights[0].quadraticFalloff");

        glm::vec4 lightPosEye = view * glm::vec4(pointLightPositions[0], 1.0);
        glUniform3f(lightPosLoc, lightPosEye.x, lightPosEye.y, lightPosEye.z);

        glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
        glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
        glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

        glUniform1f(constantFallLoc,    1.0f);
        glUniform1f(linearFallLoc,     0.09f);
        glUniform1f(quadraticFallLoc, 0.032f);

        // Point Light #1
        lightPosLoc      = glGetUniformLocation(shader.Program, "pointLights[1].position");
        lightAmbientLoc  = glGetUniformLocation(shader.Program, "pointLights[1].ambient");
        lightDiffuseLoc  = glGetUniformLocation(shader.Program, "pointLights[1].diffuse");
        lightSpecularLoc = glGetUniformLocation(shader.Program, "pointLights[1].specular");
        constantFallLoc  = glGetUniformLocation(shader.Program, "pointLights[1].constantFalloff");
        linearFallLoc    = glGetUniformLocation(shader.Program, "pointLights[1].linearFalloff");
        quadraticFallLoc = glGetUniformLocation(shader.Program, "pointLights[1].quadraticFalloff");

        lightPosEye = view * glm::vec4(pointLightPositions[1], 1.0);
        glUniform3f(lightPosLoc, lightPosEye.x, lightPosEye.y, lightPosEye.z);

        glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
        glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
        glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

        glUniform1f(constantFallLoc,    1.0f);
        glUniform1f(linearFallLoc,     0.09f);
        glUniform1f(quadraticFallLoc, 0.032f);

        // Point Light #2
        lightPosLoc      = glGetUniformLocation(shader.Program, "pointLights[2].position");
        lightAmbientLoc  = glGetUniformLocation(shader.Program, "pointLights[2].ambient");
        lightDiffuseLoc  = glGetUniformLocation(shader.Program, "pointLights[2].diffuse");
        lightSpecularLoc = glGetUniformLocation(shader.Program, "pointLights[2].specular");
        constantFallLoc  = glGetUniformLocation(shader.Program, "pointLights[2].constantFalloff");
        linearFallLoc    = glGetUniformLocation(shader.Program, "pointLights[2].linearFalloff");
        quadraticFallLoc = glGetUniformLocation(shader.Program, "pointLights[2].quadraticFalloff");

        lightPosEye = view * glm::vec4(pointLightPositions[2], 1.0);
        glUniform3f(lightPosLoc, lightPosEye.x, lightPosEye.y, lightPosEye.z);

        glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
        glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
        glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

        glUniform1f(constantFallLoc,    1.0f);
        glUniform1f(linearFallLoc,     0.09f);
        glUniform1f(quadraticFallLoc, 0.032f);

        // Point Light #3
        lightPosLoc      = glGetUniformLocation(shader.Program, "pointLights[3].position");
        lightAmbientLoc  = glGetUniformLocation(shader.Program, "pointLights[3].ambient");
        lightDiffuseLoc  = glGetUniformLocation(shader.Program, "pointLights[3].diffuse");
        lightSpecularLoc = glGetUniformLocation(shader.Program, "pointLights[3].specular");
        constantFallLoc  = glGetUniformLocation(shader.Program, "pointLights[3].constantFalloff");
        linearFallLoc    = glGetUniformLocation(shader.Program, "pointLights[3].linearFalloff");
        quadraticFallLoc = glGetUniformLocation(shader.Program, "pointLights[3].quadraticFalloff");

        lightPosEye = view * glm::vec4(pointLightPositions[3], 1.0);
        glUniform3f(lightPosLoc, lightPosEye.x, lightPosEye.y, lightPosEye.z);

        glUniform3f(lightAmbientLoc,  0.1f, 0.1f, 0.1f);
        glUniform3f(lightDiffuseLoc,  0.6f, 0.6f, 0.6f);
        glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

        glUniform1f(constantFallLoc,    1.0f);
        glUniform1f(linearFallLoc,     0.09f);
        glUniform1f(quadraticFallLoc, 0.032f);

        // Cone Light (Flashlight)
        lightPosLoc        = glGetUniformLocation(shader.Program, "coneLight.position");
        lightAmbientLoc    = glGetUniformLocation(shader.Program, "coneLight.ambient");
        lightDiffuseLoc    = glGetUniformLocation(shader.Program, "coneLight.diffuse");
        lightSpecularLoc   = glGetUniformLocation(shader.Program, "coneLight.specular");
        lightDirectionLoc  = glGetUniformLocation(shader.Program, "coneLight.direction");
        constantFallLoc  = glGetUniformLocation(shader.Program, "coneLight.constantFalloff");
        linearFallLoc    = glGetUniformLocation(shader.Program, "coneLight.linearFalloff");
        quadraticFallLoc = glGetUniformLocation(shader.Program, "coneLight.quadraticFalloff");
        GLint lightCutoffLoc     = glGetUniformLocation(shader.Program, "coneLight.cutoff");
        GLint lightOuterCutoffLoc = glGetUniformLocation(shader.Program, "coneLight.outerCutoff");

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
        // ----
        // Models
        nanosuitModel.Draw(shader);

        // Lights
        lightShader.Use();
        glBindVertexArray(lightVAO);
            for (int i = 0; i < 4; i ++) {
                // Send the View Matrix
                view = camera.getViewMatrix();
                viewMatrixLoc = glGetUniformLocation(lightShader.Program, "view");
                glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));

                // Send the Perspective Matrix
                perspective = glm::perspective(glm::radians(camera.fov), (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);
                perspectiveMatrixLoc = glGetUniformLocation(lightShader.Program, "perspective");
                glUniformMatrix4fv(perspectiveMatrixLoc, 1, GL_FALSE, glm::value_ptr(perspective));

                // Send colors
                glUniform3f(glGetUniformLocation(lightShader.Program, "lightColor"), 1.0f, 1.0f, 1.0f);

                // Send the Model Matrix
                model = glm::mat4();
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f));
                modelMatrixLoc = glGetUniformLocation(lightShader.Program, "model");
                glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));
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
    GLfloat cameraSpeed = 5.0f * deltaTime;
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

    GLfloat sensitivity = 0.05;
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
