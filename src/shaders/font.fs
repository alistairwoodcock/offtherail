#version 330 core

uniform sampler2D mainTex;
uniform vec4 textColour;

in vec2 uv0;

out vec4 fragColor;

void main()
{
	vec4 c = texture(mainTex, uv0);

	if(c.r == 0 && c.g == 0 && c.b == 0) discard;


	fragColor = vec4(textColour.r, textColour.g, textColour.b, c.a);
}