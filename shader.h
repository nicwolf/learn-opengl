#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
    // Program ID
    GLuint Program;
    // Constructor for reading and building the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // Use the Program
    void Use();
};

#endif // SHADER_H
