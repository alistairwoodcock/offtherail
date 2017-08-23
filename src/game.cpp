#include <stdlib.h>
#include <stdio.h>
#include "game.h"
// #include "screens/MainMenu.cpp"

// #include "entities/Camera.cpp"
// #include "entities/Particles.cpp"
// #include "entities/Train.cpp"

static struct State *init()
{
    State *state = (State *)malloc(sizeof(*state));

    state->game_started = false;
    state->quit_game = false;

    state->current_screen = MAIN_MENU;
    // state->camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));
		
	// switchScreen(MAIN_MENU);

    return state;
}

static void finalize(State *state){
	free(state);
}

static void reload(State *state){

}

static void unload(State *state){
	
}

static void update(State *state, float deltaTime){
	// Camera* camera = state->camera;
	
	state->time += deltaTime;

	// view/projection transformations to pass to render functions
	// glm::mat4 projection = glm::perspective(glm::radians(state->camera.Zoom), (float)state->screenWidth / (float)state->screenHeight, 0.1f, 100.0f);
	// glm::mat4 view = camera->GetViewMatrix();

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
	// 		// if(state->up) camera->ProcessKeyboard(FORWARD, deltaTime);
	// 		// if(state->down) camera->ProcessKeyboard(BACKWARD, deltaTime);
	// 		// if(state->left) camera->ProcessKeyboard(LEFT, deltaTime);
	// 		// if(state->right) camera->ProcessKeyboard(RIGHT, deltaTime);
			
	// 		// Train::update(state, time, deltaTime);
	// 		// Particles::update(state, time, deltaTime);
		

	// 		// Train::render(projection, view);
	// 		// Particles::render(projection, view);

	// 	} break;
	// }
	
}

static bool shouldClose(State *state){
	return false;	
}

// static void *handleInput(State *state, Input input){
// 	state->up = 
// 	state->down = 
// 	state->left = 
// 	state->right = 
// }

static void switchScreen(State *state, Screens newScreen){
	state->current_screen = newScreen;

	switch(newScreen)
	{
		case MAIN_MENU: {

			// MainMenu::setup(state);

		} break;

		case CHOOSE: {


		} break;

		case GAME: {

			// Train::setup(state);
			// Particles::setup(state);

		} break;
	}
}


const GameAPI GAME_API = {
    .init = init,
    .finalize = finalize,
    .reload = reload,
    .unload = unload,
    .update = update,
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

	void handleInput(GLFWwindow *window) {
		state.escape_pressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
		state.up_pressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
		state.down_pressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
		state.left_pressed = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
		state.right_pressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
		state.space_pressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
		state.enter_pressed = (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
	}

	glm::vec3 getBackgroundColor(){
		return glm::vec3(1,1,1);
	}

	bool shouldClose(){
		return state.quit_game;
	}
}
*/