#version 330 core

uniform sampler2D mainTex;

in vec2 uv0;

out vec4 fragColor;

void main()
{
	vec4 c = texture(mainTex, uv0);
	fragColor = vec4(c.r, c.g, c.b, ((c.r+c.g+c.b)/3));
}