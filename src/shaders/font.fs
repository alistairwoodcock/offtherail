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

#version 330 core

uniform sampler2D mainTex;
uniform vec4 textColour;

in vec2 uv0;

out vec4 fragColor;

void main()
{
	vec4 c = texture(mainTex, uv0);

	if(c.r == 0) discard;
	fragColor = vec4(textColour.r, textColour.g, textColour.b, c.r);


}
