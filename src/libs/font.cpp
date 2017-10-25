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

#include "font.h"

// -- Public --
// Role: Create a font given a .tff file 
//
Font* createFont(const char* fontPath){
	Font *font = new Font();
	font->initialised = false;

	/* load font file */
	long size;
	unsigned char* fontBuffer;
	
	FILE* fontFile = fopen(fontPath, "rb");
	fseek(fontFile, 0, SEEK_END);
	size = ftell(fontFile); /* how long is the file ? */
	fseek(fontFile, 0, SEEK_SET); /* reset */
	
	fontBuffer = (unsigned char*)malloc(size);
	
	fread(fontBuffer, size, 1, fontFile);
	fclose(fontFile);

	/* prepare font */
	stbtt_fontinfo info;
	if(stbtt_InitFont(&info, fontBuffer, 0))
	{
		printf("initialised new font from file: %s\n", fontPath);
		font->initialised = true;
	}

	font->info = info;

	return font;
}

// -- Public --
// Role: Create a text area to write into
//
TextArea* createTextArea(Font* font, int width, int height, int line_height, char* text = NULL, int min_kern = 0){
	TextArea *textarea = new TextArea();
	
	//default position
	textarea->x = 0;
	textarea->y = 0;
	textarea->z = 0;

	//setting our default colours
	textarea->colour = glm::vec4(0,0,0,1);


	textarea->scale = glm::vec3(1.0f);
	textarea->width = width;
	textarea->height = height;

	textarea->line_height = line_height;
	textarea->min_kern = 0;

	textarea->font = font;

	if(textarea == NULL){
		textarea->text_set = false;
		textarea->text_updated = false;
		textarea->text = NULL;
	} else {
		textarea->text = text;
		textarea->text_set = true;
		textarea->text_updated = true;
	}

	textarea->bitmap = (unsigned char*)calloc(textarea->width * textarea->height, sizeof(char));

	glGenTextures(1, &textarea->glTexture);
	glBindTexture(GL_TEXTURE_2D, textarea->glTexture);
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textarea->width, textarea->height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
	// glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	// glGenerateMipmap(GL_TEXTURE_2D);

	const float vertices[] =
	{
		-1, -1, 0,
		-1,  1, 0,
		 1,  1, 0,
		-1, -1, 0,
		 1,  1, 0,
		 1, -1, 0,
	};

	const float uvs[] =
	{
		0, 1,
		0, 0,
		1, 0,
		0, 1,
		1, 0,
		1, 1,
	};

	glGenVertexArrays(1, &textarea->VAO);
	glBindVertexArray(textarea->VAO);

	glGenBuffers(1, &textarea->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, textarea->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 18, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &textarea->UVB);
	glBindBuffer(GL_ARRAY_BUFFER, textarea->UVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 12, uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	return textarea;
}

// -- Public --
// Role: Set text for a given text area
//
void setText(TextArea *ta, char* text){
	if(text == NULL){
		ta->text = NULL;
		ta->text_updated = false;
		ta->text_set = false;

		return;
	}

	ta->text = text;
	ta->text_updated = true;
	ta->text_set = true;
}

// -- Public --
// Role: Set the font for a given text area 
//
void setFont(TextArea *ta, Font* newFont){
	ta->font = newFont;
	ta->text_updated = true;
}

// -- Public --
// Role: Render the given text area 
//
void renderText(TextArea *ta){
	if(!ta->text_set) return;
	if(ta->text == NULL) return;
	if(ta->bitmap == NULL) return;
	
	if(ta->text_updated)
	{
		//we will re-generate the textarea bitmap with the new text
		ta->text_updated = false;
		
		Font *font = ta->font;
		stbtt_fontinfo info = font->info;

		/* calculate font scaling */
		float scale = stbtt_ScaleForPixelHeight(&info, ta->line_height);

		int x = 0;
		   
		int ascent, descent, lineGap;
		stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
		
		ascent *= scale;
		descent *= scale;
		

		char *word = ta->text;

		int text_length = strlen(word);

		memset(ta->bitmap, 0, ta->width * ta->height);
		
		for (int i = 0; i < text_length; ++i)
		{
			/* get bounding box for character (may be offset to account for chars that dip above or below the line */
			int c_x1, c_y1, c_x2, c_y2;
			stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
			
			/* compute y (different characters have different heights */
			int y = ascent + c_y1;
			
			/* render character (stride and offset is important here) */
			int byteOffset = x + (y  * ta->width);
			stbtt_MakeCodepointBitmap(&info, ta->bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, ta->width, scale, scale, word[i]);
			
			/* how wide is this character */
			int ax;
			stbtt_GetCodepointHMetrics(&info, word[i], &ax, 0);
			x += ax * scale;
			
			/* add kerning */
			int kern;
			kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
			x += kern * scale;

			x += ta->min_kern;
		}

		glBindTexture(GL_TEXTURE_2D, ta->glTexture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ta->width, ta->height, 0, GL_RED, GL_UNSIGNED_BYTE, ta->bitmap);
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		glGenerateMipmap(GL_TEXTURE_2D);	
	}


	unsigned int ID = Shaders::get("font").ID;
	
	useShader(ID);

	glBindTexture(GL_TEXTURE_2D, ta->glTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(ID, "mainTex"), 0);

	glBindVertexArray(ta->VAO);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(ta->x, ta->y, ta->z));
	model = glm::scale(model, ta->scale);
	
	shaderSetMat4(ID, "position", model);

	shaderSetVec4(ID, "textColour", ta->colour);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}
