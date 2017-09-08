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

#include "entities/Camera.h"
#include "entities/Particles.cpp"
#include "entities/Train.cpp"
#include "screens/MainMenu.cpp"
#include "screens/OverlayMenu.cpp"

static void init(State *state)
{
	printf("INIT");
	
	//Setup our entire game and GL state 
    
    //Game state for runtime
    state->game_state.game_started = false;
    state->game_state.quit_game = false;
    changeScreen(state, MAIN_MENU);
    
    state->game_state.camera_locked = true;
    state->game_state.input_timeout = 0;
    state->game_state.ground = -2;

    //GL Setup
    printf("screenWidth: %i\n", state->platform.screenWidth);
    glViewport(0, 0, state->platform.screenWidth, state->platform.screenHeight);
	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    //platform for input + rendering
	    

    //load shaders
    
    /* -- Particles Setup -- */
    float particle_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	};

	GLuint VAO;
    GLuint VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0*sizeof(float)));
    glEnableVertexAttribArray(0);

    state->game_state.Particle_VAO = VAO;
    state->game_state.Particle_VBO = VBO;

    state->game_state.particleShader = loadShader("particle", "src/shaders/particle.vs","src/shaders/particle.fs");
    state->game_state.particle_count = 10000;

    Particles::setup(state);

    /* -- End Particle Setup -- */


    /* -- Camera Setup -- */
    state->game_state.camera = Camera(glm::vec3(0.0f, 8.0f, 24.0f));
    
    /* -- Menu Setup --*/
    MainMenu::setup(state);
    OverlayMenu::setup(state);


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

			}
			
			if(!game->paused){
				//update camera based on state
				//this is just for now, we're going to have a fixed camera in the future.

				Trains::update(state, platform->currTime, platform->deltaTime);
				Particles::update(state, platform->currTime, platform->deltaTime);
			}			
			
			Trains::render(state, projection, view);
			Particles::render(state, projection, view);
			OverlayMenu::render(state, projection, view);

		} break;
	}

}

void changeScreen(State *state, Screens screen) {
   state->game_state.current_screen = screen; 
}

void paused(State *state, bool paused) {
    state->game_state.paused = paused;
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
