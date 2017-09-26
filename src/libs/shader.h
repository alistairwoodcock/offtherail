#ifndef SHADER_H
#define SHADER_H

#include <dirent.h>
#include <string>
#include <iostream>

struct Shader {
    unsigned int ID;
    std::string name;
    std::string vsFileName;
    std::string fsFileName;
    bool callback_set;
    void(*callback)(void* state);
};


struct ShaderMapElement{
	std::string key;
	Shader val;
};

struct ShaderMap {
	int count;
	static const int maxCount = 256;
	ShaderMapElement elements[maxCount];
};

void checkCompileErrors(GLuint shader, std::string type);
char* loadFileText(const char* fileName);
void freeShader(Shader s);
Shader loadShader(const char* shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);


#endif