#include <stdio.h>

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
    struct GameAPI api;
    struct State *state;
};

//Func definitions
// void handle_input(GLFWwindow *window);
// void window_resize(GLFWwindow* window, int width, int height);

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
                if (game->state == NULL)
                    game->state = game->api.init();
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

//Timing
float deltaTime = 0.0f;
float prevTime = 0.0f;

int main(){

	Game game = {0};

	while(true)
	{
		load_game_lib(&game);
		if(game.handle){
			
			game.api.updateAndRender(game.state); //TODO(AL): use GLFW

			if(game.api.shouldClose(game.state)) break;

			usleep(1000);
		}

	}

	return 0;

	// Game::updateDimensions(screenWidth, screenHeight);

	// Game::setup();

	// bool close = false;

	// while(!close)
	// {
		
	// 	//Time
	// 	float currentTime = glfwGetTime();
	// 	deltaTime = currentTime - prevTime;
	// 	prevTime = currentTime;

	// 	// input handling
	// 	Game::handleInput(window);

	// 	// clear buffer
	// 	glm::vec3 bg = Game::getBackgroundColor();
	// 	glClearColor(bg.x, bg.y, bg.z, 1.0f);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 	//run game update
	// 	Game::update(deltaTime);

	// 	//flip that graphcis buff to the screen
	// 	glfwSwapBuffers(window);
	// 	glfwPollEvents();

	// 	close = glfwWindowShouldClose(window) || Game::shouldClose();
	// }
}

// void window_resize(GLFWwindow* window, int width, int height)
// {
// 	Game::updateDimensions(width, height);
// 	glViewport(0, 0, width, height);
// }

