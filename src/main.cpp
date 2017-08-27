#include <stdlib.h>
#include <stdio.h>

#include "libs/stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//dynamic lib loading on unix
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "game.h"

const char *GAME_LIBRARY = "build/game.dylib";

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

bool first_load = true;

void load_game_lib(Game *game){
	struct stat attr;
    if ((stat(GAME_LIBRARY, &attr) == 0) && (game->id != attr.st_ino)) {
        if (game->handle) {
            game->api.unload(game->state);
            dlclose(game->handle);
        }
        void *handle = dlopen(GAME_LIBRARY, RTLD_NOW);
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
}

void unload_game_lib(Game *game)
{
    if (game->handle) {
        game->api.finalize(game->state);
        game->state = NULL;
        dlclose(game->handle);
        game->handle = NULL;
        game->id = 0;
    }
}


float screenWidth = 500;
float screenHeight = 500;

bool windowResized = false;

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
		return false;
	}

	game.state = state;
	game.state->platform.window = game.window;

	glfwSetFramebufferSizeCallback(game.window, window_resize);
    glfwMakeContextCurrent(game.window);

    glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		glfwTerminate();
		return false;
	}

	bool close = false;
	while(!close)
	{
		glfwPollEvents();

		load_game_lib(&game);
		if(game.handle){

			Input input = get_current_input(game.window);
			game.state->platform.input = input;

			//set width + height 
			game.state->platform.windowResized = windowResized;
			windowResized = false; //reset so we can flag for next resize
			game.state->platform.screenWidth = screenWidth;
			game.state->platform.screenHeight = screenHeight;

			//time update
			game.state->platform.prevTime = game.state->platform.currTime;
			game.state->platform.currTime = glfwGetTime();
			game.state->platform.deltaTime = game.state->platform.currTime - game.state->platform.prevTime;
			
			game.api.updateAndRender(game.state);

			close = game.api.shouldClose(game.state);

			usleep(1000);
		}

		glfwSwapBuffers(game.window);

		close = (close || glfwWindowShouldClose(game.window));
	}

	return 0;
}

Input get_current_input(GLFWwindow* window){
	Input in = {0};

	in.escape_pressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
	in.w_pressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	in.s_pressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
	in.a_pressed = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	in.d_pressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
	in.space_pressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
	in.enter_pressed = (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);

	return in;
}

void window_resize(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	windowResized = true;
}

