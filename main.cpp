#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Vertex Shader
// -------------
// #version 330 core
// layout (location = 0) in vec3 position;
//
// void main() {
//     gl_Position = vec4(position.x, position.y, position.z, 1.0);
// }
const GLchar* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
        "}\0";

// Fragment Shader 0
// -----------------
// #version 330 core
// out vec4 color;
//
// void main() {
//    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
// }
const GLchar* fragmentShaderSource0 = "#version 330 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";

// Fragment Shader 1
// -----------------
// #version 330 core
// out vec4 color;
//
// void main() {
//    color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
// }
const GLchar* fragmentShaderSource1 = "#version 330 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "color = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
        "}\n";

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

    // Vertex Shader Compilation
    // -------------------------
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check that the Vertex Shader compiled properly
    GLint shaderCompiledSuccess;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompiledSuccess);
    if (!shaderCompiledSuccess) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader 0 Compilation
    // -----------------------------
    GLuint fragmentShader0;
    fragmentShader0 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader0, 1, &fragmentShaderSource0, NULL);
    glCompileShader(fragmentShader0);
    // Check that the Fragment Shader compiled properly
    glGetShaderiv(fragmentShader0, GL_COMPILE_STATUS, &shaderCompiledSuccess);
    if (!shaderCompiledSuccess) {
        glGetShaderInfoLog(fragmentShader0, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link Shader Program 0
    // ---------------------
    GLuint shaderProgram0;
    shaderProgram0 = glCreateProgram();
    glAttachShader(shaderProgram0, vertexShader);
    glAttachShader(shaderProgram0, fragmentShader0);
    glLinkProgram(shaderProgram0);
    // Check that the Shader Program linked successfully
    GLint programLinkSuccess;
    glGetProgramiv(shaderProgram0, GL_LINK_STATUS, &programLinkSuccess);
    if (!programLinkSuccess) {
        glGetProgramInfoLog(shaderProgram0, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader 0 Compilation
    // -----------------------------
    GLuint fragmentShader1;
    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader1);
    // Check that the Fragment Shader compiled properly
    glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &shaderCompiledSuccess);
    if (!shaderCompiledSuccess) {
        glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link Shader Program 1
    // ---------------------
    GLuint shaderProgram1;
    shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);
    // Check that the Shader Program linked successfully
    glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &programLinkSuccess);
    if (!programLinkSuccess) {
        glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
   }

    // Delete the shader objects, we don't need them anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader0);
    glDeleteShader(fragmentShader1);

    // Create Geometry Data
    // --------------------
    // Define the vertices for two triangles.
    // The first:
    GLfloat vertices0[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };
    // The second:
    GLfloat vertices1[] = {
        0.5f,  0.6f, 0.0f,
       -0.5f, -0.4f, 0.0f,
       -0.5f,  0.6f, 0.0f
    };

    // Setup Graphics Memory: Triangle 1
    // ---------------------------------
    // Generate the VBOs
    // This stores our vertex attributes.
    GLuint vbos[2];
    glGenBuffers(2, vbos);

    // Generate the VAOs
    // This holds all of the vertex attributes from our VBO and EBO.
    GLuint vaos[2];
    glGenVertexArrays(2, vaos);

    // Bind the VAO: Triangle 1
    glBindVertexArray(vaos[0]);
        // Bind VBO into an OpenGL Array Buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices0), vertices0, GL_STATIC_DRAW);
        // Set Vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
        // Enable the attribute we just setup
        glEnableVertexAttribArray(0);
    // Unbind the VAO
    glBindVertexArray(0);

    // Bind the VAO: Triangle 2
    glBindVertexArray(vaos[1]);
        // Bind VBO into an OpenGL Array Buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
        // Set Vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
        // Enable the attribute we just setup
        glEnableVertexAttribArray(0);
    // Unbind the VAO
    glBindVertexArray(0);

    // Render Loop
    // -----------
    while(!glfwWindowShouldClose(window)) {
        // Check if any events need to be processed
        glfwPollEvents();

        // Clear buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render triangles
        // Triangle 1
        glUseProgram(shaderProgram0);
        glBindVertexArray(vaos[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // Triangle 2
        glUseProgram(shaderProgram1);
        glBindVertexArray(vaos[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
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
