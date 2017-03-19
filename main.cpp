#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main(int argc, char *argv[])
{
    // Initialize GLFW
    glfwInit();
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Setup Viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Game Loop
    while(!glfwWindowShouldClose(window)) {
        // Check if any events need to be processed
        glfwPollEvents();

        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap the current color buffer out for the one just drawn
        glfwSwapBuffers(window);
    }

    // Clean up/delete any resources we allocated.
    glfwTerminate();

    // Exit
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scandcode, int action, int mode) {
    // When the user presses the escape key, set the WindowShouldClose
    // property to true and prompt the Game Loop to exit at the start
    // of the next frame.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
