#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"


#include "game.h"
#include "entities/Camera.h"


// #include "entities/Train.cpp"


// #include "screens/MainMenu.cpp"

// void loadShaders(State *state){
	
	// float particle_vertices[] = {
 //        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
 //         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
 //         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
 //         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
 //        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
 //        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	// };

	// GLuint VAO;
 //    GLuint VBO;

 //    glGenVertexArrays(1, &VAO);
 //    glGenBuffers(1, &VBO);

 //    glBindVertexArray(VAO);
 //    glBindBuffer(GL_ARRAY_BUFFER, VBO);
 //    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);
 //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0*sizeof(float)));
 //    glEnableVertexAttribArray(0);

    // state->game_state.Particle_VAO = VAO;
    // state->game_state.Particle_VBO = VBO;

	// state->platform.loadShader("train", "src/shaders/train.vs", "src/shaders/train.fs");
    // state->platform.loadShader("particle", "src/shaders/particle.vs","src/shaders/particle.fs");
// }

static void init(State *state)
{
	//Setup our entire game and GL state 
    
    //Game state for runtime
    state->game_state.game_started = false;
    state->game_state.quit_game = false;
    state->game_state.current_screen = MAIN_MENU;
    
    state->game_state.camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
    
    //GL Setup
    glViewport(0, 0, state->platform.screenWidth, state->platform.screenHeight);
	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    //platform for input + rendering
	    

    // //load shaders
    
    // /* -- Particles Setup -- */
    
    // state->game_state.particle_count = 10000;

    // Particles::setup(state);

    /* -- End Particle Setup -- */



    // Shader s = state->platform.getShader("particle");
    
    //load models
    // state->platform.loadModel("train", "models/train/locomotive/Locomotive C36.obj");

    // state->game_state.train = (void*)new Train(state);

}

static void updateAndRender(State *state){
	glm::vec3 background(1,1,1);
	glClearColor(background.x, background.y, background.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	GameState *game = &state->game_state;
	PlatformState *platform = &state->platform;
	
	if(platform->windowResized){
		glViewport(0, 0, platform->screenWidth, platform->screenWidth); 
	}

	Camera *camera = &game->camera;

	// printf("deltaTime: %f\n", platform->deltaTime);

	// view/projection transformations to pass to render functions
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)platform->screenWidth / (float)platform->screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	

	// switch(state->current_screen)
	// {
	// 	case MAIN_MENU: {
			
	// 		// MainMenu::update(state, time, deltaTime);

	// 		// MainMenu::render(projection, view);

	// 	} break;

	// 	case CHOOSE: {



	// 	} break;

	// 	case GAME: {

	// 		//update camera based on state
	// 		//this is just for now, we're going to have a fixed camera in the future.
			if(platform->input.w_pressed) camera->ProcessKeyboard(FORWARD, platform->deltaTime);
			if(platform->input.s_pressed) camera->ProcessKeyboard(BACKWARD, platform->deltaTime);
			if(platform->input.a_pressed) camera->ProcessKeyboard(LEFT, platform->deltaTime);
			if(platform->input.d_pressed) camera->ProcessKeyboard(RIGHT, platform->deltaTime);
			
			// ((Train*)game->train)->update(state, platform->currTime, platform->deltaTime);
			
			// Particles::update(state, platform->currTime, platform->deltaTime);
		

			
			// ((Train*)game->train)->render(state, projection, view);
			// Particles::render(state, projection, view);
			


	// 	} break;
	// }
}

// static void switchScreen(State *state, Screens newScreen){
// 	GameState *game = &state->game_state;

// 	game->current_screen = newScreen;

// 	switch(newScreen)
// 	{
// 		case MAIN_MENU: {

// 			// MainMenu::setup(state);

// 		} break;

// 		case CHOOSE: {


// 		} break;

// 		case GAME: {

// 			// Train::setup(state);
// 			// Particles::setup(state);

// 		} break;
// 	}
// }

static bool shouldClose(State *state){
	return state->game_state.quit_game;
}

static void finalize(State *state){
	printf("UNLOAD\n");
	free(state);
}

static void reload(State *state){
	printf("RELOAD\n");
	
}
static void unload(State *state){
	printf("UNLOAD\n");
	// if(state->platform.initialized){
	// 	glDeleteVertexArrays(1, &state->game_state.Particle_VAO);
 //    	glDeleteBuffers(1, &state->game_state.Particle_VBO);

	// 	state->platform.freeShaders();		
	// }
}

const GameAPI GAME_API = {
    .init = init,
    .finalize = finalize,
    .reload = reload,
    .unload = unload,
    .updateAndRender = updateAndRender,
    .shouldClose = shouldClose
};


/*

namespace Game {

	//Game state object
	// State state = {0};
	
	Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
	
	unsigned int screenWidth;
	unsigned int screenHeight;
	
	float time = 0; //total game time

	//initialise the very start of the game code
	void setup(){

		state.game_started = false;
		
		switchScreen(MAIN_MENU);
	}

	//does all initialisation for screens
	//lets discuss if this is the best way to go
	void switchScreen(Screens newScreen){
		state.current_screen = newScreen;

		switch(newScreen)
		{
			case MAIN_MENU: {

				MainMenu::setup(state);

			} break;

			case CHOOSE: {


			} break;

			case GAME: {

				Train::setup(state);
				Particles::setup(state);

			} break;
		}
	}

	//gets called from the main loop
	//we render every frame and update logic here
	void update(float deltaTime){
		time += deltaTime;

		// printf("Time: %f\n", time);

		// view/projection transformations to pass to render functions
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		switch(state.current_screen)
		{
			case MAIN_MENU: {
				
				MainMenu::update(state, time, deltaTime);

				MainMenu::render(projection, view);

			} break;

			case CHOOSE: {



			} break;

			case GAME: {

				//update camera based on state
				//this is just for now, we're going to have a fixed camera in the future.
				if(state.up_pressed) camera.ProcessKeyboard(FORWARD, deltaTime);
				if(state.down_pressed) camera.ProcessKeyboard(BACKWARD, deltaTime);
				if(state.left_pressed) camera.ProcessKeyboard(LEFT, deltaTime);
				if(state.right_pressed) camera.ProcessKeyboard(RIGHT, deltaTime);
				
				Train::update(state, time, deltaTime);
				Particles::update(state, time, deltaTime);
			

				Train::render(projection, view);
				Particles::render(projection, view);

			} break;
		}
		
	}

	//Stuff below mostly gets called from our main function
	//for handing window input rendering setup and all that

	void updateDimensions(unsigned int width, unsigned int height){
		screenWidth = width;
		screenHeight = height;
	}

	glm::vec3 getBackgroundColor(){
		return glm::vec3(1,1,1);
	}

	bool shouldClose(){
		return state.quit_game;
	}
}
*/