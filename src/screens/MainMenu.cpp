namespace MainMenu{
	

	class MenuImage: public Entity {
	public:
		unsigned int VAO;
		unsigned int VBO;
		unsigned int glTexture;
		
		int width;
		int height;
		int nChannels;

		glm::vec3 scale;
		float scale_vel;

		MenuImage(){
			scale = glm::vec3(1.0);
		}

	};
	
	Shader *textShader;

	MenuImage *logo;
	MenuImage *startText;
	MenuImage *exitText;

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

		stbi_image_free(data);
		stbi_set_flip_vertically_on_load(false);

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
		
		textShader->use();
		textShader->setMat4("projection", projection);
		textShader->setMat4("view", view);
		textShader->setMat4("model", model);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
	}

	void setup(State &state){

		state.start_active = true;
		state.exit_active = false;

		float logo_vertices[] = {
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
	         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		};

		logo = new MenuImage();
		logo->x = 0;
		logo->y = 0.3;
		logo->z = -1;
		logo->scale = glm::vec3(0.95);
		logo->scale_vel = 0.1;
		setupImage("images/logo.png", logo, logo_vertices, sizeof(logo_vertices));

		
		startText = new MenuImage();
		startText->z = -1;
		startText->y = -0.1;
		startText->scale = glm::vec3(0.3);
		startText->scale_vel = 0;
		setupImage("images/start.png", startText, logo_vertices, sizeof(logo_vertices));
		
		exitText = new MenuImage();
		exitText->z = -1;
		exitText->y = -0.3;
		exitText->scale = glm::vec3(0.15);
		exitText->scale_vel = 0;
		setupImage("images/exit.png", exitText, logo_vertices, sizeof(logo_vertices));

		
		textShader = new Shader("src/shaders/text.vs","src/shaders/text.fs");
		textShader->use(); 
		textShader->setInt("textTexture", 0);
	}	

	void update(State &state, float time, float deltaTime){
		logo->scale += glm::vec3(1.0)*logo->scale_vel*deltaTime*0.5f;
		startText->scale += glm::vec3(1.0)*startText->scale_vel*deltaTime;
		exitText->scale += glm::vec3(1.0)*exitText->scale_vel*deltaTime;
		
		if(logo->scale.x > 0.98) logo->scale_vel = -0.1;
		if(logo->scale.x <= 0.95) logo->scale_vel = 0.1;
		
		if(state.start_active){
			if(startText->scale.x < 0.3) startText->scale_vel = 0.5;
			if(startText->scale.x >= 0.3) startText->scale_vel = 0;
		} else {
			if(startText->scale.x > 0.2) startText->scale_vel = -0.5;
			if(startText->scale.x <= 0.2) startText->scale_vel = 0;
		}

		if(state.exit_active){
			if(exitText->scale.x < 0.3) exitText->scale_vel = 0.5;
			if(exitText->scale.x >= 0.3) exitText->scale_vel = 0;
		} else {
			if(exitText->scale.x > 0.2) exitText->scale_vel = -0.5;
			if(exitText->scale.x <= 0.2) exitText->scale_vel = 0;
		}

		if(state.start_active && state.down_pressed){
			state.start_active = false;
			state.exit_active = true;
		}

		if(state.exit_active && state.up_pressed){
			state.start_active = true;
			state.exit_active = false;
		}

		if(state.space_pressed || state.enter_pressed){
			
			if(state.exit_active){
				state.quit_game = true;
			} 
			if(state.start_active){
				Game::switchScreen(GAME);
			}
		}

		if(state.escape_pressed)
		{
			state.quit_game = true;
		}
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		renderImage(logo, projection, view);
		renderImage(startText, projection, view);
		renderImage(exitText, projection, view);
	}

}