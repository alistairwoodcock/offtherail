#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>

#include <GLFW/glfw3.h>

#include "libs/music.h"
#include "libs/shader.h"
#include "libs/model.h"

#include "entities/Entity.h"
#include "entities/Camera.h"
#include "entities/Track.h"
#include "entities/Train.h"
#include "entities/Particles.h"
#include "entities/SkyBox.h"
#include "entities/Grass.h"
#include "entities/Puddle.h"

#include "libs/font.h"
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
	bool c_pressed;
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
	bool left_shift_pressed;
	bool right_shift_pressed;
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

	/* SHADER STATE */
	ShaderMap* shaderMap;
	float shaderUpdateTimeout;

	/* MUSIC STATE */
	std::map<const char*, Sound> *sounds;
    Sound music;
	const char* current;

	/* LEVEL STATE */
    float speed; //Speed of train, scenery, etc.
	
	/* PARTICLE STATE */
	int particle_count;
	Particle* particles;
	unsigned int Particle_VBO;
	unsigned int Particle_VAO;

	/* SKYDOME STATE*/
	//THIS LINE BREAKS THE MAIN MENU ??
	SkyBox *skybox;

	/*LIGHTS STATE*/
	Entity *sun;

	/* GROUND STATE */
	float ground; //y position for the ground
	unsigned int Plane_VAO;
	unsigned int Plane_VBO;

	/* TRACK STATE*/
	//By having both these lines the game will not run propoerly, cannot
	//remove the depth map with '
	Model* trackModel;
	float trackLen;
	int trackCount;
	Track *tracks;
	Track *track1;
	Track *track2;
	Track *track3;
	int selectedTrack;

	/* TRACK SWITCH STATE */
	TrackSwitch switches[20];
	int switchesCount;
	int maxSwitches;
	float nextSwitchCountdown;
	int selectedSwitch;

    /* GRASS STATE */
    int grass_count;
    Grass* grass;

    /* PUDDLE STATE */
    int puddle_count;
    Puddle* puddle;

	/* TRAIN STATE */ 
	Model *trainModel;
	Train *train;
	Bogie *bogieFront;
	Bogie *bogieBack;
    bool fallen; // Help I've fallen and I can't get up

	Model* trainModels[TRAIN_MODEL_NUM];

	/* FONTS */
	Font *openSans;
	Font *comicSans;

	/* INPUT STATE */
	float input_timeout;

	/* MENU SCREEN STATE */
	MenuImage *logo;
	TextArea *startText;
	TextArea *exitText;
	

	bool start_active;
	bool exit_active;

    /* CHOOSE SCREEN STATE */
    Train *chooseTrain;
    Entity *chooseFloor;
    TrainTypes currentTrain;
    MenuImage *chooseRight;
    MenuImage *chooseLeft;

	/* OVERLAY SCREEN STATE */
	Entity *overlay;
	TextArea *resumeText;
	bool overlay_active;
	bool resume_active;

	/* CAMERA STATE */
	Camera camera;
	bool camera_locked;	

	/* SHADOW STATE */
	glm::vec3 lightPos;
	unsigned int shadowDepthMapFBO;
	unsigned int shadowDepthMap;
	bool showDepthMap;

	/* SCORING STATE */
	int score;
	float pointCountdown;

	TextArea *scoreText;

	
};

struct State {
	GameState game_state;
	PlatformState platform;
};

//Our global references to game state
//these get setup on init + update of game library
State *GlobalState;
GameState* game;
PlatformState* platform;

void reset();
void changeScreen(Screens screen);
void paused(bool paused);

struct GameAPI {

	void (*init)(State* s);

	void (*finalize)(State *s);

	void (*reload)(State *s);

	void (*unload)(State *s);

	void (*updateAndRender)();

	bool (*shouldClose)();
};

extern const struct GameAPI GAME_API;

#endif
