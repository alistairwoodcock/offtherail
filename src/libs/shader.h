#ifndef SHADER_H
#define SHADER_H

#include <dirent.h>
#include <string>
#include <iostream>

struct Shader {
    unsigned int ID;
    char* name;
    char* vsFileName;
    char* fsFileName;
};


struct ShaderMapElement{
	char* key;
	Shader val;
};

struct ShaderMap {
	int count;
	int maxCount;
	ShaderMapElement* elements;
};

void checkCompileErrors(GLuint shader, std::string type);
char* loadFileText(const char* fileName);
void freeShader(Shader s);
Shader loadShader(const char* shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);


#endif