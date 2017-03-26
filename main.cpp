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
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // Setup Graphics Memory
    // ---------------------
    // Generate a VBO
    // This stores our vertex attributes.
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Generate a VAO
    // This holds all of the vertex attributes from our VBO and EBO.
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    // Generate a Lighting VAO
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);

    // Setup Textures
    // --------------
    // We set up two different textures here and will interpolate
    // between them according to the global `mixConstant`.
    //
    // First, we set up an image of a wooden container.
    int containerImageWidth, containerImageHeight;
    unsigned char* containerImage = SOIL_load_image("../learn-opengl/assets/container.jpg",
                                           &containerImageWidth, &containerImageHeight,
                                           0, SOIL_LOAD_RGB);
    GLuint containerTexture;
    glGenTextures(1, &containerTexture);
    glBindTexture(GL_TEXTURE_2D, containerTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 containerImageWidth, containerImageHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, containerImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(containerImage);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Second, we set up an image of a smiley face
    int faceImageWidth, faceImageHeight;
    unsigned char* faceImage = SOIL_load_image("../learn-opengl/assets/awesomeface.png",
                                           &faceImageWidth, &faceImageHeight,
                                           0, SOIL_LOAD_RGB);
    GLuint faceTexture;
    glGenTextures(1, &faceTexture);
    glBindTexture(GL_TEXTURE_2D, faceTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 faceImageWidth, faceImageHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, faceImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(faceImage);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Setup VAOs
    // ----------
    glBindVertexArray(VAO);
        // Bind VBO into an OpenGL Array Buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Setup Attribute 0: Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) 0);
        glEnableVertexAttribArray(0);
        // Setup Attribute 2: Texture Coordinate
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    // Unbind the VAO
    glBindVertexArray(0);

    glBindVertexArray(lightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) 0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Load Shaders
    // ------------
    Shader cubeShader("../learn-opengl/shaders/cube.vert",
                      "../learn-opengl/shaders/cube.frag");
    Shader lightShader("../learn-opengl/shaders/light.vert",
                       "../learn-opengl/shaders/light.frag");

    // Render Loop
    // -----------
    while(!glfwWindowShouldClose(window)) {
        // Check if any events need to be processed
        glfwPollEvents();
        do_movement();

        // Clear buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Store the current time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Transformation Matrices
        glm::mat4 view;
        GLuint viewMatrixLoc;
        glm::mat4 perspective;
        GLuint perspectiveMatrixLoc;
        glm::mat4 model;
        GLuint modelMatrixLoc;

        // Lighting
        glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

        // Draw Cube
        cubeShader.Use();
        glBindVertexArray(VAO);
            // Send the View Matrix
            view = camera.getViewMatrix();
            viewMatrixLoc = glGetUniformLocation(cubeShader.Program, "view");
            glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));

            // Send the Perspective Matrix
            perspective = glm::perspective(glm::radians(camera.fov), (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);
            perspectiveMatrixLoc = glGetUniformLocation(cubeShader.Program, "perspective");
            glUniformMatrix4fv(perspectiveMatrixLoc, 1, GL_FALSE, glm::value_ptr(perspective));

            // Bind the Container Texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, containerTexture);
            glUniform1i(glGetUniformLocation(cubeShader.Program, "containerTexture"), 0);

            // Bind the Face Texture
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, faceTexture);
            glUniform1i(glGetUniformLocation(cubeShader.Program, "faceTexture"), 1);

            // Send the Mix Constant
            glUniform1f(glGetUniformLocation(cubeShader.Program, "mixConstant"), mixConstant);

            // Send colors
            glUniform3f(glGetUniformLocation(cubeShader.Program, "objectColor"),
                        1.0f, 0.5f, 0.31f);
            glUniform3f(glGetUniformLocation(cubeShader.Program, "lightColor"),
                        lightColor.x, lightColor.y, lightColor.z);

            // Send the Model Matrix
            model = glm::mat4();
            modelMatrixLoc = glGetUniformLocation(cubeShader.Program, "model");
            glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Draw
            glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Draw Light
        lightShader.Use();
        glBindVertexArray(lightVAO);
            // Send the View Matrix
            view = camera.getViewMatrix();
            viewMatrixLoc = glGetUniformLocation(lightShader.Program, "view");
            glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));

            // Send the Perspective Matrix
            perspective = glm::perspective(glm::radians(camera.fov), (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);
            perspectiveMatrixLoc = glGetUniformLocation(lightShader.Program, "perspective");
            glUniformMatrix4fv(perspectiveMatrixLoc, 1, GL_FALSE, glm::value_ptr(perspective));

            // Send colors
            glUniform3f(glGetUniformLocation(lightShader.Program, "lightColor"),
                        lightColor.x, lightColor.y, lightColor.z);

            // Send the Model Matrix
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(1.2f, 1.0f, 2.0f));
            model = glm::scale(model, glm::vec3(0.2f));
            modelMatrixLoc = glGetUniformLocation(lightShader.Program, "model");
            glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));
            // Draw
            glDrawArrays(GL_TRIANGLES, 0, 36);
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
