#ifndef UI_FONT
#define UI_FONT

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


class Font{
public:
	bool initialised;

	// stbtt_bakedchar cdata[96];
	stbtt_fontinfo info;
};


class TextArea: public Entity{
public:
	unsigned int glTexture;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int UVB;

	glm::vec3 scale; //scale the font when rendering
	float scale_vel;

	int width;
	int height;
	int line_height;
	int min_kern;
	unsigned char* bitmap; //used multiple times for text image allocation

	glm::vec4 colour; //text colour


	char* text; // the actual text
	bool text_set;
	bool text_updated; //flag to avoid many renders of the font

	Font* font; //the font that we will use to render with
};


Font* createFont(const char* fontPath);
TextArea* createTextArea(Font* font, int width, int height, int line_height, char* text, int min_kern);
void setText(TextArea *ta, char* text);
void renderText(TextArea *ta);
void setFont(TextArea *ta, Font* newFont);
#endif