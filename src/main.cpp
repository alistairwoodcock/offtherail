/* 
 * Course: COMP3320
 * Project: Off The Rail
 * Members: 
 *	Alistair Woodcock	(c3138738)
 *	Lachlan Meyer		(c3203676)
 *	Joshua Crompton		(c3165877)
 *	Scott Walker		(c3232582)
 *	Timothy Pitts		(c3220826)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "libs/stb_image.h"


#include <GL/glew.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>

//dynamic lib loading on unix
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "game.h"

// Game struct to handle state, api, and the window
struct Game {
	void *handle;
	ino_t id;
	GameAPI api;
	State *state;
	GLFWwindow *window;
};

//Func definitions
Input get_current_input(GLFWwindow *window);
void window_resize(GLFWwindow* window, int width, int height);
void load_game_lib(Game *game);
void unload_game_lib(Game *game);

const char *GAME_LIBRARY = "build/game.dylib";
const char *GAME_LIBRARY_TMP = "build/game_tmp.dylib";

#ifdef _WIN32 
#include <windows.h>
int load_counter = 0;
#endif

bool first_load = true;

// -- Public --
// Role: Load game library, different methods if unix or windows 
//
void load_game_lib(Game *game){
#ifdef _WIN32

	if(load_counter++ > 66){
		load_counter = 0;
	
		if (game->handle) {
			game->api.unload(game->state);
			FreeLibrary((HINSTANCE)game->handle);
		}

		CopyFile(GAME_LIBRARY, GAME_LIBRARY_TMP, false);
		void* handle = (void*)LoadLibrary(GAME_LIBRARY_TMP);
		
		if(handle){
			game->handle = handle;
			
			const struct GameAPI *api = (GameAPI*)GetProcAddress((HINSTANCE)game->handle, "GAME_API");
		
			if(api != NULL){
				game->api = *api;
				if(first_load){
					first_load = false;
					game->api.init(game->state);
				}

				game->api.reload(game->state);
			} else {
				FreeLibrary((HINSTANCE)game->handle);
				game->handle = NULL;
				game->id = 0;

			}
		} else {
			game->handle = NULL;
			game->id = 0;
		}
	}

	
#else
	struct stat attr;
	if ((stat(GAME_LIBRARY, &attr) == 0) && (game->id != attr.st_ino)) {
		if (game->handle) {
			game->api.unload(game->state);
			dlclose(game->handle);
		}
		void *handle = dlopen(GAME_LIBRARY, RTLD_LAZY);
		if (handle) {
			game->handle = handle;
			game->id = attr.st_ino;
			const struct GameAPI *api = (GameAPI*)dlsym(game->handle, "GAME_API");
			if (api != NULL) {
				game->api = *api;
				if (first_load){
					first_load = false;
					game->api.init(game->state);
				}

				game->api.reload(game->state);
			} else {
				dlclose(game->handle);
				game->handle = NULL;
				game->id = 0;
			}
		} else {
			game->handle = NULL;
			game->id = 0;
		}
	}
#endif
}

// -- Public --
// Role: Close down the dynamic game library
//
void unload_game_lib(Game *game){
	if(game->handle){
		game->api.finalize(game->state);
		game->state = NULL;
		#ifdef _WIN32
		FreeLibrary((HINSTANCE)game->handle);
		#else 
		dlclose(game->handle);
		#endif
		game->handle = NULL;
		game->id = 0;
	}
}

int screenWidth = 500;
int screenHeight = 500;

bool windowResized = false;

// -- Public --
// Role: Main game loop, this controls the window and accessing the dynamic library 
//
int main(){
	Game game = {0};
	State *state = (State *)malloc(sizeof(*state));

	glfwInit();

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

	game.window = glfwCreateWindow(screenWidth, screenHeight, "Off The Rail", NULL, NULL);
	
	if(game.window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return 1;
	}

	game.state = state;
	game.state->platform.window = game.window;

	glfwSetFramebufferSizeCallback(game.window, window_resize);
	glfwMakeContextCurrent(game.window);

	glfwGetFramebufferSize( game.window, &screenWidth, &screenHeight );
	game.state->platform.screenWidth = screenWidth;
	game.state->platform.screenHeight = screenHeight;

	printf("SCREEN width: %i\n", screenWidth);

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		glfwTerminate();
		return 1;
	}

	// While the game is meant to be running
	bool close = false;
	while(!close)
	{
		glfwPollEvents();

		load_game_lib(&game);
		if(game.handle){

			//time update
			game.state->platform.prevTime = game.state->platform.currTime;
			game.state->platform.currTime = glfwGetTime();
			game.state->platform.deltaTime = game.state->platform.currTime - game.state->platform.prevTime;

			float frame = game.state->platform.deltaTime*1000;
			#define avg_max 50
			static float avg[avg_max] = {0};
			static int avg_i = 0;

			avg[avg_i] = frame;
			// Calculate average framerate
			if(avg_i+1 == avg_max) {
				float total = 0;
				for(int i = 0; i < avg_max; i++) total += avg[i];
				total /= avg_max;
				printf("time: %f\n", total);
				
				avg_i = 0;
			}
			else {
				avg_i++;
			}
			

			// Limit fps
			float sleep_time_ms = 33 - game.state->platform.deltaTime * 1000;
			if(sleep_time_ms > 0){
				#ifdef _WIN32
				Sleep(sleep_time_ms);
				#else
				usleep(sleep_time_ms * 1000);
				#endif
			}


			Input input = get_current_input(game.window);
			game.state->platform.input = input;

			//set width + height 
			game.state->platform.windowResized = windowResized;
			windowResized = false; //reset so we can flag for next resize
			game.state->platform.screenWidth = screenWidth;
			game.state->platform.screenHeight = screenHeight;
				
			// Call updates and reders for the entire game
			game.api.updateAndRender();

			close = game.api.shouldClose();
		}

		glfwSwapBuffers(game.window);
		close = (close || glfwWindowShouldClose(game.window));
	}

	// Close down the game
	unload_game_lib(&game);
	return 0;
}

// -- Public --
// Role: Assign booleans in the input state to be true if the key is currently being pressed.
//
Input get_current_input(GLFWwindow* window){
	Input in = {0};

	in.escape_pressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
	in.w_pressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	in.s_pressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
	in.a_pressed = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	in.d_pressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
	in.p_pressed = (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
	in.u_pressed = (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS);
	in.c_pressed = (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);
	in.m_pressed = (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS);
	in.up_pressed = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
	in.down_pressed = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
	in.left_pressed = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
	in.right_pressed = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
	in.space_pressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
	in.enter_pressed = (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
	in.left_bracket_pressed = (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS);
	in.right_bracket_pressed = (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS);
	in.semicolon_pressed = (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS);
	in.apostrophe_pressed = (glfwGetKey(window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS);
	in.left_shift_pressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	in.right_shift_pressed = (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

	return in;
}

// -- Public --
// Role: Resize the given window to be the specified size.
//
void window_resize(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	windowResized = true;
}

