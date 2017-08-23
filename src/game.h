#pragma once

struct State;
enum Screens;

struct Game = {

	struct get_state *(*int)();

	void (*finalize)(struct state *s);

	void (*reload)(struct state *s);

	void (*unload)(struct state *s);

	void (*update)(struct state *s);
};

extern const struct Game GAME_API;


//Old funcs we may add some of these back l8r
// void setup();
// void updateDimensions(unsigned int width, unsigned int height);
// void update(float deltaTime);
// void switchScreen(Screens newScreen);
// void handleInput(GLFWwindow *window) ;
// glm::vec3 getBackgroundColor();
// bool shouldClose();
