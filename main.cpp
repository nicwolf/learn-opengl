#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "shader.h"

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main(int argc, char *argv[])
{
    std::ifstream test;
    std::cout << test.rdbuf() << std::endl;
    // Administrivia
    // -------------
    // Initialize GLFW
    glfwInit();
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Window Setup
    // ------------
    // Create a window object
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make the window object we created the focus of the current context
    glfwMakeContextCurrent(window);
    // Register our key callbacks
    glfwSetKeyCallback(window, key_callback);

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

    // Create Geometry Data
    // --------------------
    // Define the vertices for a square
    GLfloat vertices[] = {
        // Positions         // Colors          // Texture  //
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Top Right
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom Left
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f  // Top Left
    };
    // Define the index order for drawing the triangles in the square
    GLuint indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3
    };


    // Setup Graphics Memory
    // ---------------------
    // Generate a VBO
    // This stores our vertex attributes.
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Generate an EBO
    // This stores our indices.
    GLuint EBO;
    glGenBuffers(1, &EBO);

    // Generate a VAO
    // This holds all of the vertex attributes from our VBO and EBO.
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    // Generate the Container texture
    int containerImageWidth, containerImageHeight;
    unsigned char* containerImage = SOIL_load_image("../learn-opengl/assets/container.jpg",
                                           &containerImageWidth, &containerImageHeight,
                                           0, SOIL_LOAD_RGB);
    GLuint containerTexture;
    glGenTextures(1, &containerTexture);
    glBindTexture(GL_TEXTURE_2D, containerTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
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

    // Generate the Face texture
    int faceImageWidth, faceImageHeight;
    unsigned char* faceImage = SOIL_load_image("../learn-opengl/assets/awesomeface.png",
                                           &faceImageWidth, &faceImageHeight,
                                           0, SOIL_LOAD_RGB);
    GLuint faceTexture;
    glGenTextures(1, &faceTexture);
    glBindTexture(GL_TEXTURE_2D, faceTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 faceImageWidth, faceImageHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, faceImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(faceImage);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Bind the VAO
    glBindVertexArray(VAO);
        // Bind VBO into an OpenGL Array Buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Bind EBO into an OpenGL Element Array Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // Setup Attribute 0: Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0);
        glEnableVertexAttribArray(0);
        // Setup Attribute 1: Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // Setup Attribute 2: Texture Coordinate
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    // Unbind the VAO
    glBindVertexArray(0);

    // Load Shader Program
    Shader shaderTriangle("../learn-opengl/shaders/triangle.vert",
                          "../learn-opengl/shaders/triangle.frag");

    // Render Loop
    // -----------
    while(!glfwWindowShouldClose(window)) {
        // Check if any events need to be processed
        glfwPollEvents();

        // Clear buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render square
        GLfloat timeValue = glfwGetTime();
        shaderTriangle.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, containerTexture);
        glUniform1i(glGetUniformLocation(shaderTriangle.Program, "containerTexture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, faceTexture);
        glUniform1i(glGetUniformLocation(shaderTriangle.Program, "faceTexture"), 1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

void key_callback(GLFWwindow* window, int key, int scandcode, int action, int mode) {
    // "ESCAPE" Key prompts the window to close at the start of the next loop.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // "W" Key toggles between solid and wireframe polygon mode.
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
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
}
