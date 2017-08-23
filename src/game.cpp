// #include "screens/MainMenu.cpp"

// #include "entities/Camera.cpp"
// #include "entities/Particles.cpp"
// #include "entities/Train.cpp"

enum Screens {
	MAIN_MENU,
	CHOOSE,
	GAME
};

struct State {
	bool game_started;
	bool quit_game;
	
	Screens current_screen;

	bool left_pressed;
	bool right_pressed;
	bool up_pressed;
	bool down_pressed;
	bool space_pressed;
	bool enter_pressed;
	bool escape_pressed;

	/* MENU SCREEN STATE */
	bool start_active;
	bool exit_active;
};


static struct State *game_init()
{
    // int width, height;
    // initscr();  // peek at terminal size
    // getmaxyx(stdscr, height, width);
    // endwin();
    // struct game_state *state = malloc(sizeof(*state) + width * height * 2);
    // state->select = 0;
    // state->width = width;
    // state->height = height;
    // randomize(state);
    
    State *state = calloc(sizeof(*state));

    state->game_started = false;
    state->current_screen = MAIN_MENU;
		
	// switchScreen(MAIN_MENU);

    return state;
}

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