#ifndef UI_FONT
#define UI_FONT

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


class Font: public Entity {
public:
	unsigned int glTexture;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int UVB;

	int width;
	int height;

	MenuImage *img;

	stbtt_bakedchar cdata[96];
	stbtt_fontinfo info;
};


#endif