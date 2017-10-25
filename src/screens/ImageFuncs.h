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

#endif
