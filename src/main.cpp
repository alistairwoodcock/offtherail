// #define GLEW_STATIC
// #include <GL/glew.h>

// #include <GLFW/glfw3.h>

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

// #define STB_IMAGE_IMPLEMENTATION
// #include "libs/stb_image.h"

// #include "libs/shader.h"
// #include "libs/model.h"

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
			
			game.api.update(game.state, 0.01f); //TODO(AL): use GLFW

			if(game.api.shouldClose(game.state)) break;
		}
	}

	return 0;

	// glfwInit( );

	// glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	

	// int screenWidth = 500, screenHeight = 500;
	// GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Off The Rail", NULL, NULL);

	// glfwGetFramebufferSize( window, &screenWidth, &screenHeight );
	// glfwSetFramebufferSizeCallback(window, window_resize);

	// Game::updateDimensions(screenWidth, screenHeight);

	// if(window == NULL) {
	// 	printf("Failed to create GLFW window\n");
	// 	glfwTerminate();
	// 	return -1;
	// }

	// glfwMakeContextCurrent(window);

	// glewExperimental = GL_TRUE;

	// if(glewInit() != GLEW_OK) {
	// 	printf("Failed to initialize GLEW\n");
	// 	glfwTerminate();
	// 	return -1;
	// }

	// glViewport(0, 0, screenWidth, screenHeight);
	// glEnable(GL_DEPTH_TEST);  
	// glEnable(GL_BLEND); 
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 


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

