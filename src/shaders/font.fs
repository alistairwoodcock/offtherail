#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D textTexture;

void main()
{
    FragColor = texture(textTexture, TexCoord);
    // if(FragColor.r > 0.9 && FragColor.g >= 0.9 && FragColor.b >= 0.9) discard;
    // FragColor *= vec4(0,0,0,1.0);
    // if(FragColor.r >= 0.7 && FragColor.g >= 0.7 && FragColor.b >= 0.7) discard;
    
}