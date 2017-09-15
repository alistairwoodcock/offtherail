#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <iostream>

struct Shader {
    unsigned int ID;
    char* name;
};

void checkCompileErrors(GLuint shader, std::string type);
char* loadFileText(const char* fileName);
void freeShaders(Shader s);
Shader loadShader(const char* shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);


#endif