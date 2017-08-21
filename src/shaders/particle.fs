#version 330 core

uniform float alpha;
uniform vec3 color;

out vec4 FragColor;

void main()
{
	FragColor = vec4(color,alpha); 
}