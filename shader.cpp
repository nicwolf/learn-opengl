#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // Read Shader Source from File
    // ----------------------------
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    // Set exceptions for input streams
    vertexShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Process vertex shader
        vertexShaderFile.open(vertexPath);
        std::stringstream vertexShaderStream;
        vertexShaderStream << vertexShaderFile.rdbuf();
        vertexShaderFile.close();
        vertexShaderSource = vertexShaderStream.str();
        // Process fragment shader
        fragmentShaderFile.open(fragmentPath);
        std::stringstream fragmentShaderStream;
        fragmentShaderStream << fragmentShaderFile.rdbuf();
        fragmentShaderFile.close();
        fragmentShaderSource = fragmentShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR:SHADER:FILE_NOT_SUCCESSFULY_READ" << std::endl;
    }
    const GLchar* vertexShaderCode = vertexShaderSource.c_str();
    const GLchar* fragmentShaderCode = fragmentShaderSource.c_str();

    // Shader Compilation
    // ==================
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader
    // -------------
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);
    // Check compilation
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    // ---------------
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);
    // Check compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link Shader Program
    // -------------------
    this->Program = glCreateProgram();
    glAttachShader(this->Program, vertexShader);
    glAttachShader(this->Program, fragmentShader);
    glLinkProgram(this->Program);
    // Check linkage
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Cleanup
    // -------
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Use() { glUseProgram(this->Program); }
