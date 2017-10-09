#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 colour;

void main()
{    
	if(colour.r == 0 && colour.g == 0 && colour.b == 0){
    	FragColor = texture(texture_diffuse1, TexCoords);
	} else {
    	FragColor = texture(texture_diffuse1, TexCoords) * vec4(colour, 1.0);
	}
}