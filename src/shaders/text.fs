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
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D textTexture;

void main()
{
	FragColor = texture(textTexture, TexCoord);
	// if(FragColor.r > 0.9 && FragColor.g >= 0.9 && FragColor.b >= 0.9) discard;
	
	// FragColor *= vec4(0,0,0,1.0);
	if(FragColor.r >= 0.7 && FragColor.g >= 0.7 && FragColor.b >= 0.7) discard;
	
}
