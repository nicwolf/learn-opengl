// Std. Includes
#include <string>
#include <vector>
#include <map>
#include <random>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "shader.h"
#include "camera.h"
#include "model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// Other Libs
#include <SOIL.h>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
void setWindowFPS(GLFWwindow* window);
GLuint loadTexture(GLchar* path, GLboolean alpha);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat near =   0.01f;
GLfloat far  = 30.0f;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLuint nbFrames = 0;
GLfloat lastTime = 0.0f;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Administrivia
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> normalDist(0, 1);
    std::uniform_real_distribution<> uniformDist(-1, 1);

    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_MULTISAMPLE);

    // Setup and compile our shaders
    Shader shader("../learn-opengl/shaders/planet.vert",
                  "../learn-opengl/shaders/planet.frag");
    Shader asteroidShader("../learn-opengl/shaders/instancing.vert",
                          "../learn-opengl/shaders/instancing.frag");
    Shader screenShader("../learn-opengl/shaders/screen.vert",
                        "../learn-opengl/shaders/screen.frag");

    // Load Models
    Model planet   = Model("../learn-opengl/assets/models/planet/planet.obj");
    Model asteroid = Model("../learn-opengl/assets/models/rock/rock.obj");

    // Setup Postprocess Framebuffer
    // =============================
    GLuint postFBO;
    glGenFramebuffers(1, &postFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postFBO);

    // With Texture:
    // -------------
    GLuint postTex;
    glGenTextures(1, &postTex);
    glBindTexture(GL_TEXTURE_2D, postTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postTex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR :: FRAMEBUFFER :: Post Framebuffer not complete!" << std::endl;
    }

    // Setup Multisample Framebuffer
    // =============================
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // With Texture:
    // -------------
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, screenWidth, screenHeight, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0);

    // With Renderbuffer
    // -----------------
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR :: FRAMEBUFFER :: Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup Postprocessing
    GLfloat screenVertices[] = {
        // Position // Texture
        -1.0, -1.0, 0.0, 0.0, 0.0,
         1.0,  1.0, 0.0, 1.0, 1.0,
        -1.0,  1.0, 0.0, 0.0, 1.0,
        -1.0, -1.0, 0.0, 0.0, 0.0,
         1.0, -1.0, 0.0, 1.0, 0.0,
         1.0,  1.0, 0.0, 1.0, 1.0
    };
    GLuint screenVAO, screenVBO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Setup Instancing
    GLuint nrAsteroids = 5000;
    glm::mat4 asteroidModelMatrices[nrAsteroids];
    GLfloat ringRadius = 10.0;
    GLfloat avgPeriod = 30.0;
    for (GLuint i = 0; i < nrAsteroids; i++) {
        glm::mat4 model = glm::mat4();

        GLfloat TWO_PI = 2 * glm::pi<float>();
        GLfloat angle = TWO_PI * (GLfloat)i / (GLfloat)nrAsteroids;

        GLfloat x = cos(angle) * ringRadius;
        x += 1.3 * normalDist(gen);
        GLfloat z = sin(angle) * ringRadius;
        z += 1.3 * normalDist(gen);
        GLfloat y = 0.3 * normalDist(gen);
        glm::vec3 location = glm::vec3(x, y, z);

        GLfloat rX, rY, rZ;
        rX = uniformDist(gen);
        rY = uniformDist(gen);
        rZ = uniformDist(gen);

        GLfloat scale;
        scale = uniformDist(gen) * 0.05;

        model = glm::translate(model, location);
        model = glm::scale(model, glm::vec3(scale));
        model = glm::rotate(model, glm::radians(30.0f), glm::vec3(rX, rY, rZ));
        asteroidModelMatrices[i] = model;

    }
    GLuint instanceBuffer;
    glGenBuffers(1, &instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, nrAsteroids * sizeof(glm::mat4), &asteroidModelMatrices[0], GL_STATIC_DRAW);
    for (GLuint i = 0; i < asteroid.meshes.size(); i++) {
        glBindVertexArray(asteroid.meshes[i].VAO);
        GLsizei vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*) vec4Size);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*) (2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*) (3 * vec4Size));
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Setup Transformation Matrices UBO
    GLuint transUBOIndex = glGetUniformBlockIndex(shader.Program, "Matrices");
    glUniformBlockBinding(shader.Program, transUBOIndex, 0);
    GLuint transUBO;
    glGenBuffers(1, &transUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, transUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, transUBO);

    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        setWindowFPS(window);

        // Check and call events
        glfwPollEvents();
        Do_Movement();

        // Update Transformation UBO
        glBindBuffer(GL_UNIFORM_BUFFER, transUBO);
            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 projection = glm::perspective(camera.fov, (float)screenWidth/(float)screenHeight, near, far);
            glBufferSubData(GL_UNIFORM_BUFFER,  0, 64, glm::value_ptr(view));
            glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glm::mat4 model;

        // Rendering Administrivia
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Render to FBO
        shader.Use();
            // Transformation Matrices
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
            GLuint modelLocation = glGetUniformLocation(shader.Program, "model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            planet.Draw(shader);

        asteroidShader.Use();
            asteroid.DrawInstanced(asteroidShader, nrAsteroids);

        // Swap the buffers
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postFBO);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight,
                          0, 0, screenWidth, screenHeight,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Postprocessing
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        screenShader.Use();
            glBindVertexArray(screenVAO);
            glDisable(GL_DEPTH_TEST);
            glBindTexture(GL_TEXTURE_2D, postTex);
            glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// This function loads a texture from file. Note: texture loading functions like these are usually
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio).
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path, GLboolean alpha)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );	// Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.processKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.processKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.processKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.processKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);
        if (*polygonMode == GL_LINE)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (*polygonMode == GL_FILL)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        }
        if (*polygonMode == GL_POINT)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

    }
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
//    camera.processMouseScroll(yoffset);
}

void setWindowFPS(GLFWwindow* window)
{
    double currentTime = glfwGetTime();
    nbFrames++;

    if (currentTime - lastTime >= 1.0) {
        char title[256];
        title[255] = '\0';

        snprintf(title, 255, "[FPS: %3.2f]", (float)nbFrames * 1.0f);
        glfwSetWindowTitle(window, title);
        nbFrames = 0;
        lastTime += 1.0;
    }
}

#pragma endregion

