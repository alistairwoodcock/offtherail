#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 position;

out vec2 uv0;

void main()
{
	gl_Position = position * vec4(aPos, 1);
	uv0 = aTexCoord;
}

