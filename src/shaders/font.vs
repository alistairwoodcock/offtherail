#version 330 core

in vec4 position;
in vec2 texCoord0;

out vec2 uv0;

void main()
{
	gl_Position = position;
	uv0 = texCoord0;
}