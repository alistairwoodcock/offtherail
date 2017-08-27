#include "MainMenu.h"

namespace MainMenu{
	
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

	void renderImage(State *state, MenuImage *image, glm::mat4 &projection, glm::mat4 &view){

		glBindVertexArray(image->VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, image->glTexture);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(image->x, image->y, image->z));
		model = glm::scale(model, image->scale);
		
		unsigned int ID = state->game_state.textShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);
		shaderSetMat4(ID, "model", model);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
	}

	void setup(State *state){
		GameState *game = &state->game_state;

		game->start_active = true;
		game->exit_active = false;

		float logo_vertices[] = {
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
	         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		};

		game->logo = new MenuImage();
		game->logo->x = 0;
		game->logo->y = 0.3;
		game->logo->z = -1;
		game->logo->scale = glm::vec3(0.95);
		game->logo->scale_vel = 0.1;
		setupImage("images/logo.png", game->logo, logo_vertices, sizeof(logo_vertices));

		printf("logo->glTexture: %u\n", game->logo->glTexture);
		
		game->startText = new MenuImage();
		game->startText->z = -1;
		game->startText->y = -0.1;
		game->startText->scale = glm::vec3(0.3);
		game->startText->scale_vel = 0;
		setupImage("images/start.png", game->startText, logo_vertices, sizeof(logo_vertices));
		
		printf("startText->glTexture: %u\n", game->startText->glTexture);

		game->exitText = new MenuImage();
		game->exitText->z = -1;
		game->exitText->y = -0.3;
		game->exitText->scale = glm::vec3(0.15);
		game->exitText->scale_vel = 0;
		setupImage("images/exit.png", game->exitText, logo_vertices, sizeof(logo_vertices));

		printf("exitText->glTexture: %u\n", game->exitText->glTexture);
		
		game->textShader = loadShader("text", "src/shaders/text.vs","src/shaders/text.fs");
		useShader(game->textShader.ID);
		shaderSetInt(game->textShader.ID, "textTexture", 0);
	}	

	void update(State *state, float time, float deltaTime){
		GameState *game = &state->game_state;
		PlatformState *platform = &state->platform;

		MenuImage *logo = game->logo;
		MenuImage *startText = game->startText;
		MenuImage *exitText = game->exitText;

		logo->scale += glm::vec3(1.0)*logo->scale_vel*deltaTime*0.5f;
		startText->scale += glm::vec3(1.0)*startText->scale_vel*deltaTime;
		exitText->scale += glm::vec3(1.0)*exitText->scale_vel*deltaTime;
		
		if(logo->scale.x > 0.98) logo->scale_vel = -0.1;
		if(logo->scale.x <= 0.95) logo->scale_vel = 0.1;
		
		if(game->start_active){
			if(startText->scale.x < 0.3) startText->scale_vel = 0.5;
			if(startText->scale.x >= 0.3) startText->scale_vel = 0;
		} else {
			if(startText->scale.x > 0.2) startText->scale_vel = -0.5;
			if(startText->scale.x <= 0.2) startText->scale_vel = 0;
		}

		if(game->exit_active){
			if(exitText->scale.x < 0.3) exitText->scale_vel = 0.5;
			if(exitText->scale.x >= 0.3) exitText->scale_vel = 0;
		} else {
			if(exitText->scale.x > 0.2) exitText->scale_vel = -0.5;
			if(exitText->scale.x <= 0.2) exitText->scale_vel = 0;
		}

		Input *input = &platform->input;

		if(game->start_active && input->s_pressed){
			game->start_active = false;
			game->exit_active = true;
		}

		if(game->exit_active && input->w_pressed){
			game->start_active = true;
			game->exit_active = false;
		}

		if(input->space_pressed || input->enter_pressed){
			
			if(game->exit_active){
				game->quit_game = true;
			} 
			if(game->start_active){
				printf("WE GET HERE\n");
				game->current_screen = GAME;
			}
		}

		if(input->escape_pressed)
		{
			game->quit_game = true;
		}

		printf("game->current_screen: %i\n", game->current_screen);
		printf("game->exit_active: %i\n", game->exit_active);
		printf("game->start_active: %i\n", game->start_active);
		
		printf("input->w_pressed: %i\n", input->w_pressed);
		printf("input->space_pressed: %i\n", input->space_pressed);
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){

		renderImage(state, state->game_state.logo, projection, view);
		renderImage(state, state->game_state.startText, projection, view);
		renderImage(state, state->game_state.exitText, projection, view);
	}

}