#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include "game.h"

#include "libs/shader.cpp"
#include "libs/texture.cpp"
#include "libs/mesh.cpp"
#include "libs/model.cpp"
#include "libs/music.cpp"

#include "entities/Camera.h"
#include "entities/Particles.cpp"
#include "entities/Grass.cpp"
#include "entities/Train.cpp"
#include "entities/SkyBox.cpp"
#include "entities/Lights.cpp"
#include "screens/MainMenu.cpp"
#include "screens/OverlayMenu.cpp"


static void init(State *state)
{
	printf("INIT\n");
	
	//Setup our entire game and GL state 
    
    /* -- Music Setup -- */
    Music::init(); // Need to init before first changeScreen()
    
    //Game state for runtime
    state->game_state.game_started = false;
    state->game_state.quit_game = false;
    changeScreen(state, MAIN_MENU);
    
    state->game_state.camera_locked = true;
    state->game_state.input_timeout = 0;

    state->game_state.ground = -2;
    state->game_state.speed = 10;

    //GL Setup
    printf("screenWidth: %i\n", state->platform.screenWidth);
    glViewport(0, 0, state->platform.screenWidth, state->platform.screenHeight);
	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    //platform for input + rendering
	    

    //load shaders
    
    /* -- Particles Setup -- */
    Particles::setup(state);
    
    /* -- Lights Setup -- */
    Lights::setup(state);

    /* -- Grasss Setup -- */
    Grasses::setup(state);
    
    /* -- Camera Setup -- */
    state->game_state.camera = Camera(glm::vec3(0.0f, 11.71f, 34.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -17.0f);
    
    /* -- Menu Setup --*/
    MainMenu::setup(state);
    OverlayMenu::setup(state);

	/* -- Set Up Sky --*/
	SkyBoxes::setup(state);

    /* -- Train Setup -- */
    Trains::setup(state);
}

static void updateAndRender(State *state){

	glm::vec3 background(1,1,1);
	glClearColor(background.x, background.y, background.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	GameState *game = &state->game_state;
	PlatformState *platform = &state->platform;
	Camera *camera = &game->camera;
	
	if(platform->windowResized){
		printf("screenWidth: %i\n", platform->screenWidth);
		glViewport(0, 0, platform->screenWidth, platform->screenWidth); 
	}


	//we set all input to empty when 
	//the input timeout is running
	if(game->input_timeout > 0){
		printf("input_timeout: %f\n", game->input_timeout);
		game->input_timeout -= platform->deltaTime;
		Input empty = {0};
		platform->input = empty;
	}

	// view/projection transformations to pass to render functions
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)platform->screenWidth / (float)platform->screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	
	switch(game->current_screen)
	{
		case MAIN_MENU: {
			
			MainMenu::update(state, platform->currTime, platform->deltaTime);
			
			MainMenu::render(state, projection, view);

		} break;

		case CHOOSE: {
			



		} break;

		case GAME: {
			
			OverlayMenu::update(state, platform->currTime, platform->deltaTime);

			if(platform->input.u_pressed){
				if(game->camera_locked){
					game->camera_locked = false;
					game->input_timeout = 0.1;
				} else {
					game->camera_locked = true;	
					game->input_timeout = 0.1;
					camera->Reset();
				}
			}

			if(!game->camera_locked){
				/* camera position update */

				if(platform->input.up_pressed) camera->UpdatePosition(FORWARD, platform->deltaTime);
				if(platform->input.down_pressed) camera->UpdatePosition(BACKWARD, platform->deltaTime);
				if(platform->input.left_pressed) camera->UpdatePosition(LEFT, platform->deltaTime);
				if(platform->input.right_pressed) camera->UpdatePosition(RIGHT, platform->deltaTime);
				
				if(platform->input.right_bracket_pressed) camera->UpdatePosition(UP, platform->deltaTime);
				if(platform->input.left_bracket_pressed) camera->UpdatePosition(DOWN, platform->deltaTime);
				if(platform->input.semicolon_pressed) camera->UpdatePosition(ROT_LEFT, platform->deltaTime);
				if(platform->input.apostrophe_pressed) camera->UpdatePosition(ROT_RIGHT, platform->deltaTime);

				// printf("camera(%f, %f, %f)\n", camera->Position.x, camera->Position.y, camera->Position.z);
				// printf("camera YAW(%f)\n", camera->Yaw);
				// printf("camera PITCH(%f)\n", camera->Pitch);
			}
			
			if(!game->paused){
				//update camera based on state
				//this is just for now, we're going to have a fixed camera in the future.

				Grasses::update(state, platform->currTime, platform->deltaTime);
				Trains::update(state, platform->currTime, platform->deltaTime);
				Particles::update(state, platform->currTime, platform->deltaTime);
				Lights::update(state, platform->currTime, platform->deltaTime);
			}			
			

			//Draw CubeMap
			SkyBoxes::render(state, projection, view);
			Lights::render(state, projection, view);
			Grasses::render(state, projection, view);
			Trains::render(state, projection, view);
			Particles::render(state, projection, view);
			OverlayMenu::render(state, projection, view);

		} break;
	}

}

void changeScreen(State *state, Screens screen) {
    state->game_state.current_screen = screen;

    switch(screen)
    {
    	case MAIN_MENU: {
        	Music::play("sounds/dejavu.wav");
    	} break;

    	case CHOOSE: {

    	} break;

    	case GAME: {
        	Music::play("sounds/running90s.wav");
    	} break;
    }
    
}

void paused(State *state, bool paused) {
    state->game_state.paused = paused;
    Music::pause(paused);
}

static bool shouldClose(State *state){
	if(state->game_state.quit_game){
		printf("SHOULD CLOSE\n");
	}
	return state->game_state.quit_game;
}

static void finalize(State *state){
	printf("FINALIZE\n");
	free(state);
    Music::destroy();
}

static void reload(State *state){
	printf("RELOAD\n");
	
}
static void unload(State *state){
	printf("UNLOAD\n");
}

const GameAPI GAME_API = {
    .init = init,
    .finalize = finalize,
    .reload = reload,
    .unload = unload,
    .updateAndRender = updateAndRender,
    .shouldClose = shouldClose
};
