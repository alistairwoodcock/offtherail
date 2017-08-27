#include "MenuImage.h"
#include "ImageFuncs.h"

namespace OverlayMenu{
	
	void setup(State *state){
		GameState *game = &state->game_state;

		game->overlay_active = false;
		game->resume_active = false;
		game->exit_active = false;

		float logo_vertices[] = {
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
	         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		};

		// game->overlay = new MenuImage();
		// game->overlay->z = -1;
		// game->overlay->y = -0.1;
		// game->overlay->scale = glm::vec3(1);
		// game->overlay->scale_vel = 0;
		// setupImage("images/overlay.png", game->overlay, logo_vertices, sizeof(logo_vertices));
		
		game->resumeText = new MenuImage();
		game->resumeText->z = -1;
		game->resumeText->y = -0.3;
		game->resumeText->scale = glm::vec3(0.15);
		game->resumeText->scale_vel = 0;
		setupImage("images/exit.png", game->resumeText, logo_vertices, sizeof(logo_vertices));
	}	

	void update(State *state, float time, float deltaTime){
		GameState *game = &state->game_state;
		PlatformState *platform = &state->platform;
		Input input = platform->input;

		printf("game->overlay_active: %i\n", game->overlay_active);
		printf("game->resume_active: %i\n", game->resume_active);
		printf("game->exit_active: %i\n", game->exit_active);
		printf("game->paused: %i\n", game->paused);

		printf("escape_pressed: %i\n", input.escape_pressed);

		if(!game->overlay_active && (input.p_pressed || input.escape_pressed)){
			game->overlay_active = true;
			game->resume_active = true;
			game->paused = true;
		} 

		if(!game->overlay_active){
			return;
		}


		MenuImage *logo = game->logo;
		MenuImage *resumeText = game->resumeText;
		MenuImage *exitText = game->exitText;

		logo->scale += glm::vec3(1.0)*logo->scale_vel*deltaTime*0.5f;
		resumeText->scale += glm::vec3(1.0)*resumeText->scale_vel*deltaTime;
		exitText->scale += glm::vec3(1.0)*exitText->scale_vel*deltaTime;
		
		logo->x = game->camera.Position.x + 0.5;
		logo->y = game->camera.Position.y;
		logo->z = game->camera.Position.z-2.0;

		resumeText->x = game->camera.Position.x;
		resumeText->y = game->camera.Position.y + 0.25;
		resumeText->z = game->camera.Position.z-2.0;

		exitText->x = game->camera.Position.x;
		exitText->y = game->camera.Position.y - 0.25;
		exitText->z = game->camera.Position.z-2.0;

		if(logo->scale.x > 0.98) logo->scale_vel = -0.1;
		if(logo->scale.x <= 0.95) logo->scale_vel = 0.1;
		
		if(game->resume_active){
			if(resumeText->scale.x < 0.3) resumeText->scale_vel = 0.5;
			if(resumeText->scale.x >= 0.3) resumeText->scale_vel = 0;
		} else {
			if(resumeText->scale.x > 0.2) resumeText->scale_vel = -0.5;
			if(resumeText->scale.x <= 0.2) resumeText->scale_vel = 0;
		}

		if(game->exit_active){
			if(exitText->scale.x < 0.3) exitText->scale_vel = 0.5;
			if(exitText->scale.x >= 0.3) exitText->scale_vel = 0;
		} else {
			if(exitText->scale.x > 0.2) exitText->scale_vel = -0.5;
			if(exitText->scale.x <= 0.2) exitText->scale_vel = 0;
		}


		if(game->resume_active && input.s_pressed){
			game->resume_active = false;
			game->exit_active = true;
		}

		if(game->exit_active && input.w_pressed){
			game->resume_active = true;
			game->exit_active = false;
		}

		if(input.space_pressed || input.enter_pressed){
			
			if(game->exit_active){
				game->current_screen = MAIN_MENU;
				game->paused = false;
				game->overlay_active = false;
				game->exit_active = false;
				game->resume_active = false;
				game->start_active = true;
			} 
			if(game->resume_active){
				game->current_screen = GAME;
				game->paused = false;
				game->overlay_active = false;
			}
		}
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){
		GameState *game = &state->game_state;
		if(game->overlay_active){

			renderImage(state, game->logo, projection, view);
			renderImage(state, game->resumeText, projection, view);
			renderImage(state, game->exitText, projection, view);
		}
	}

}