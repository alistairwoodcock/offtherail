#ifndef MENU_IMAGE_FUNCS
#define MENU_IMAGE_FUNCS


void setupImage(const char* imagePath, MenuImage *image, float vertices[], unsigned int vert_size){
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(imagePath, &image->width, &image->height, &image->nChannels,  STBI_rgb);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &image->glTexture);
	glBindTexture(GL_TEXTURE_2D, image->glTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_set_flip_vertically_on_load(false);
	stbi_image_free(data);

	glGenVertexArrays(1, &image->VAO);
	glGenBuffers(1, &image->VBO);

	glBindVertexArray(image->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, image->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vert_size, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0*sizeof(float)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);  
}

void renderImage(MenuImage *image, const char* shader = NULL){
	
	glBindVertexArray(image->VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image->glTexture);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(image->x, image->y, image->z));
	model = glm::scale(model, image->scale);
	
	unsigned int ID;

	if(shader == NULL){
		ID = Shaders::get("text").ID;
	} else {
		ID = Shaders::get(shader).ID;
	}

	useShader(ID);
	shaderSetMat4(ID, "position", model);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}


void setupFont(const char* fontPath, Font *font, float vertices[], unsigned int vert_size){

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
    if (!stbtt_InitFont(&info, fontBuffer, 0))
    {
        printf("failed\n");
    }
    
    int b_w = 512; /* bitmap width */
    int b_h = 128; /* bitmap height */
    int l_h = 64; /* line height */

    /* create a bitmap for the phrase */
    unsigned char* bitmap = (unsigned char*)calloc(b_w * b_h, sizeof(char));
    
    /* calculate font scaling */
    float scale = stbtt_ScaleForPixelHeight(&info, l_h);

    char* word = "Hello, There Pals!";
    
    int x = 0;
       
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    
    ascent *= scale;
    descent *= scale;
    
    int i;
    for (i = 0; i < strlen(word); ++i)
    {
        /* get bounding box for character (may be offset to account for chars that dip above or below the line */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
        
        /* compute y (different characters have different heights */
        int y = ascent + c_y1;
        
        /* render character (stride and offset is important here) */
        int byteOffset = x + (y  * b_w);
        stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);
        
        /* how wide is this character */
        int ax;
        stbtt_GetCodepointHMetrics(&info, word[i], &ax, 0);
        x += ax * scale;
        
        /* add kerning */
        int kern;
        kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
        x += kern * scale;
    }

    /* save out a 1 channel image */
    stbi_write_png("./images/out.png", b_w, b_h, 1, bitmap, b_w);
    
    font->width = b_w;
	font->height = b_h;
	font->info = info;
	
	MenuImage *img = new MenuImage();
	img->x = font->x;
	img->y = font->y;
	img->width = font->width;
	img->height = font->height;
	img->scale = glm::vec3(1);
	img->scale_vel = 0;

	setupImage("./images/out.png", img, vertices, vert_size);

	font->img = img;


	free(fontBuffer);
    free(bitmap);
}

void renderFont(Font* font){

	renderImage(font->img, "font");

}

#endif