#include "MenuImage.h"
#include "ImageFuncs.h"

namespace MainMenu{
	
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
		game->logo->y = 0.4;
		game->logo->z = 0;
		game->logo->scale = glm::vec3(1.1);
		game->logo->scale_vel = 0.1;
		setupImage("images/logo.png", game->logo, logo_vertices, sizeof(logo_vertices));

		game->startText = new MenuImage();
		game->startText->x = 0;
		game->startText->y = -0.2;
		game->startText->z = 0;
		game->startText->scale = glm::vec3(0.5);
		game->startText->scale_vel = 0;
		setupImage("images/start.png", game->startText, logo_vertices, sizeof(logo_vertices));
		
		game->exitText = new MenuImage();
		game->exitText->x = 0;
		game->exitText->y = -0.5;
		game->exitText->z = 0;
		game->exitText->scale = glm::vec3(0.3);
		game->exitText->scale_vel = 0;
		setupImage("images/exit.png", game->exitText, logo_vertices, sizeof(logo_vertices));

		Shader textShader = Shaders::get(state, "text");
		useShader(textShader.ID);
		shaderSetInt(textShader.ID, "textTexture", 0);
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


		if(logo->scale.x > 1.2) logo->scale_vel = -0.1;
		if(logo->scale.x <= 1.17) logo->scale_vel = 0.1;
		
		if(game->start_active){
			if(startText->scale.x < 0.4) startText->scale_vel = 0.5;
			if(startText->scale.x >= 0.4) startText->scale_vel = 0;
		} else {
			if(startText->scale.x > 0.3) startText->scale_vel = -0.5;
			if(startText->scale.x <= 0.3) startText->scale_vel = 0;
		}

		if(game->exit_active){
			if(exitText->scale.x < 0.4) exitText->scale_vel = 0.5;
			if(exitText->scale.x >= 0.4) exitText->scale_vel = 0;
		} else {
			if(exitText->scale.x > 0.3) exitText->scale_vel = -0.5;
			if(exitText->scale.x <= 0.3) exitText->scale_vel = 0;
		}

		Input *input = &platform->input;

		if(game->start_active && input->s_pressed){
			game->start_active = false;
			game->exit_active = true;
            Music::soundEffect("sounds/oh.wav");
		}

		if(game->exit_active && input->w_pressed){
			game->start_active = true;
			game->exit_active = false;
            Music::soundEffect("sounds/oh.wav");
		}

		if(input->space_pressed || input->enter_pressed){
			
			if(game->exit_active){
                Music::soundEffect("sounds/exit.wav", true);
				game->quit_game = true;
			} 
			if(game->start_active){
                changeScreen(state, GAME);
			}
		}

		if(input->escape_pressed)
		{
			game->quit_game = true;
		}
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){

		renderImage(state, state->game_state.logo, projection, view);
		renderImage(state, state->game_state.startText, projection, view);
		renderImage(state, state->game_state.exitText, projection, view);
	}

}
