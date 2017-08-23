#pragma once

enum Screens {
	MAIN_MENU,
	CHOOSE,
	GAME
};

struct State {
	bool game_started;
	bool quit_game;
	
	Screens current_screen;

	float time;

	bool up;
	bool down;
	bool left;
	bool right;

	/* MENU SCREEN STATE */
	bool start_active;
	bool exit_active;



	// Camera *camera;
};

struct GameAPI {

	struct State* (*init)();

	void (*finalize)(State *s);

	void (*reload)(State *s);

	void (*unload)(State *s);

	void (*update)(State *s, float deltaTime);

	// void (*handleInput)(State *s, Input input);

	bool (*shouldClose)(State *s);
};

// struct Input {
// 	bool left_pressed;
// 	bool right_pressed;
// 	bool up_pressed;
// 	bool down_pressed;
// 	bool space_pressed;
// 	bool enter_pressed;
// 	bool escape_pressed;
// };

extern const struct GameAPI GAME_API;


//Old funcs we may add some of these back l8r
// void setup();
// void updateDimensions(unsigned int width, unsigned int height);
// void update(float deltaTime);
// void switchScreen(Screens newScreen);
// void handleInput(GLFWwindow *window) ;
// glm::vec3 getBackgroundColor();
// bool shouldClose();
