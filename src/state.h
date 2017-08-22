
//This state object is shared with all
//setup and update functions 
//feel free to add any game state data you want to it

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