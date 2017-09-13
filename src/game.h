#ifndef GAME_H
#define GAME_H

#include <GLFW/glfw3.h>

#include "libs/shader.h"
#include "libs/model.h"

#include "entities/Entity.h"
#include "entities/Camera.h"
#include "entities/Train.h"
#include "entities/Particles.h"
#include "entities/SkyBox.h"

#include "screens/MenuImage.h"


enum Screens {
	MAIN_MENU,
	CHOOSE,
	GAME
};

struct Input {
	bool a_pressed;
	bool d_pressed;
	bool w_pressed;
	bool s_pressed;
	bool p_pressed;
	bool u_pressed;
	bool up_pressed;
	bool down_pressed;
	bool left_pressed;
	bool right_pressed;
	bool space_pressed;
	bool enter_pressed;
	bool escape_pressed;
	bool left_bracket_pressed;
	bool right_bracket_pressed;
	bool semicolon_pressed;
	bool apostrophe_pressed;
};

struct PlatformState {
	GLFWwindow* window;
	Input input;

	bool windowResized;
	int screenWidth;
	int screenHeight;

	float currTime;
	float prevTime;
	float deltaTime;
};


struct GameState {
	bool game_started;
	bool quit_game;
	bool paused;
	
	Screens current_screen;

	/* LEVEL STATE */
	float ground; //y position for the ground
    float speed; //Speed of train, scenery, etc.
	
	/* PARTICLE STATE */
	int particle_count;
	Particle* particles;
	Shader particleShader;
	unsigned int Particle_VBO;
	unsigned int Particle_VAO;

	/* SKYDOME STATE*/
	//THIS LINE BREAKS THE MAIN MENU ??
	SkyBox *skybox;

	/* TRAIN STATE */ 
	Shader trainShader;
	Model *trainModel;
	Train *train;
	Entity *bogieFront;
	Entity *bogieBack;

	/* INPUT STATE */
	float input_timeout;

	/* MENU SCREEN STATE */
	Shader textShader;
	MenuImage *logo;
	MenuImage *startText;
	MenuImage *exitText;
	bool start_active;
	bool exit_active;

	/* OVERLAY SCREEN STATE */
	Shader overlayShader;
	Entity *overlay;
	MenuImage *resumeText;
	bool overlay_active;
	bool resume_active;

	/* CAMERA STATE */
	Camera camera;
	bool camera_locked;	
	
};

struct State {
	GameState game_state;
	PlatformState platform;
};

void changeScreen(State *state, Screens screen);
void paused(State *state, bool paused);

struct GameAPI {

	void (*init)(State* s);

	void (*finalize)(State *s);

	void (*reload)(State *s);

	void (*unload)(State *s);

	void (*updateAndRender)(State *s);

	bool (*shouldClose)(State *s);
};

extern const struct GameAPI GAME_API;

#endif
