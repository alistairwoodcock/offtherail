#ifndef UI_FONT
#define UI_FONT

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

class Font: public Entity {
public:
	unsigned int glTexture;
	unsigned int VAO;
	unsigned int VBO;

	int width;
	int height;

	stbtt_bakedchar cdata[96];
	stbtt_fontinfo info;
};


#endif