/* 
 * Course: COMP3320
 * Project: Off The Rail
 * Members: 
 *	Alistair Woodcock	(c3138738)
 *	Lachlan Meyer		(c3203676)
 *	Joshua Crompton		(c3165877)
 *	Scott Walker		(c3232582)
 *	Timothy Pitts		(c3220826)
 *
 */

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

// Key value pair
struct ShaderMapElement{
	std::string key;
	Shader val;
};

// Struct to store each ShaderMapElement struct
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
