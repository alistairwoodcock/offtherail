#ifndef MENU_IMAGE_FUNCS
#define MENU_IMAGE_FUNCS


void setupFont(const char* fontPath, Font *font, float vertices[], unsigned int vert_size){
	
	font->width = 512;
	font->height = 512;
	
	unsigned char ttf_buffer[1<<20];
	
	fread(ttf_buffer, 1, 1<<20, fopen(fontPath, "rb"));
	
	unsigned char* temp_bitmap;
	
	stbtt_BakeFontBitmap(ttf_buffer,0, 32.0, temp_bitmap,font->width, font->height, 32,96, font->cdata); // no guarantee this fits!
	// can free ttf_buffer at this point

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &font->glTexture);
	glBindTexture(GL_TEXTURE_2D, font->glTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	// const char* text = "H";
	// int w, h;
	// unsigned char* bitmap = stbtt_GetCodepointBitmap(&font->info, 0,stbtt_ScaleForPixelHeight(&font->info, 1), text[0], &w, &h, 0,0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, font->width, font->height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);

	glGenVertexArrays(1, &font->VAO);
	glGenBuffers(1, &font->VBO);

	glBindVertexArray(font->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vert_size, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0*sizeof(float)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);  



}

void renderFont(Font* font){
	// printf("attempting to render: %s\n", text);
	const char* text = "H";
  	
  	glBindVertexArray(font->VAO);
  	glActiveTexture(GL_TEXTURE0);
  	glBindTexture(GL_TEXTURE_2D, font->glTexture);

	// while (*text) {
 //      if (*text >= 32 && *text < 128) {
 //      	int w, h;
 //      	unsigned char* bitmap = stbtt_GetCodepointBitmap(&font->info, 0,stbtt_ScaleForPixelHeight(&font->info, 20), text[0], &w, &h, 0,0);

      	// for (int j=0; j < h; ++j) {
      	// 	for (int i=0; i < w; ++i)
      	// 		putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
      	// 	putchar('\n');
      	// }
		
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);

		glm::mat4 model;
      	model = glm::translate(model, glm::vec3(font->x, font->y, font->z));
      	// model = glm::scale(model, image->scale);

     //  	FILE *fp = fopen( "./images/font.bmp" , "w");
   		// fwrite(bitmap , 1 , sizeof(bitmap) , fp );
   		// fclose(fp);

      	

      	unsigned int ID = Shaders::get("font").ID;

      	useShader(ID);
      	shaderSetMat4(ID, "position", model);

      	glDrawArrays(GL_TRIANGLES, 0, 6);

      	glBindVertexArray(0);

   //    }
   //    ++text;
   // }
}

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

void renderImage(MenuImage *image, glm::mat4 &projection, glm::mat4 &view){
	
	glBindVertexArray(image->VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image->glTexture);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(image->x, image->y, image->z));
	model = glm::scale(model, image->scale);
	
	unsigned int ID = Shaders::get("text").ID;

	useShader(ID);
	// shaderSetMat4(ID, "projection", projection);
	// shaderSetMat4(ID, "view", view);
	shaderSetMat4(ID, "position", model);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}

#endif