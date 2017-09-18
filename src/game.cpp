#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include "game.h"

#include "libs/shader.cpp"
#include "libs/texture.cpp"
#include "libs/mesh.cpp"
#include "libs/model.cpp"

#include "entities/Camera.h"
#include "entities/Particles.cpp"
#include "entities/Grass.cpp"
#include "entities/Train.cpp"
#include "entities/SkyBox.cpp"
#include "entities/Lights.cpp"
#include "screens/MainMenu.cpp"
#include "screens/OverlayMenu.cpp"

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

static void init(State *state)
{
	printf("INIT");
	
	//Setup our entire game and GL state 
    GameState *game = &state->game_state;

    //Game state for runtime
    game->game_started = false;
    game->quit_game = false;
    changeScreen(state, MAIN_MENU);
    
    game->camera_locked = true;
    game->input_timeout = 0;

    game->ground = -2;
    game->speed = 10;

    //GL Setup
    printf("screenWidth: %i\n", state->platform.screenWidth);
    glViewport(0, 0, state->platform.screenWidth, state->platform.screenHeight);
	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    //platform for input + rendering
	    

    //load shaders
    
    /* -- Particles Setup -- */
    Particles::setup(state);
    
    /* -- Lights Setup -- */
    Lights::setup(state);

    /* -- Grasss Setup -- */
    Grasses::setup(state);
    
    /* -- Camera Setup -- */
    game->camera = Camera(glm::vec3(0.0f, 11.71f, 34.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -17.0f);
    
    /* -- Menu Setup --*/
    MainMenu::setup(state);
    OverlayMenu::setup(state);

	/* -- Set Up Sky --*/
	SkyBoxes::setup(state);

    /* -- Train Setup -- */
    Trains::setup(state);

    /* -- Shadow Setup --*/
    unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  


	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  

	game->shadowDepthMapFBO = depthMapFBO;
	game->shadowDepthMap = depthMap;
	game->shadowDepthShader = loadShader("light", "src/shaders/simpleDepthShader.vs","src/shaders/simpleDepthShader.fs");

	game->lightPos = glm::vec3(game->sun->x, game->sun->y, game->sun->z);

	game->debugDepthQuad = loadShader("debugQuad", "src/shaders/debugQuad.vs", "src/shaders/debugQuad.fs");

	/* -- Ground Setup -- */
	game->groundShader = loadShader("ground", "src/shaders/ground.vs", "src/shaders/ground.fs");

	float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVAO;
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    game->Plane_VAO = planeVAO;
    game->Plane_VBO = planeVBO;

}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


static void updateAndRender(State *state){

	glm::vec3 background(1,1,1);
	glClearColor(background.x, background.y, background.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	GameState *game = &state->game_state;
	PlatformState *platform = &state->platform;
	Camera *camera = &game->camera;
	
	if(platform->windowResized){
		printf("screenWidth: %i\n", platform->screenWidth);
		glViewport(0, 0, platform->screenWidth, platform->screenWidth); 
	}


	//we set all input to empty when 
	//the input timeout is running
	if(game->input_timeout > 0){
		printf("input_timeout: %f\n", game->input_timeout);
		game->input_timeout -= platform->deltaTime;
		Input empty = {0};
		platform->input = empty;
	}

	// view/projection transformations to pass to render functions
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)platform->screenWidth / (float)platform->screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	
	switch(game->current_screen)
	{
		case MAIN_MENU: {
			
			MainMenu::update(state, platform->currTime, platform->deltaTime);
			
			MainMenu::render(state, projection, view);

		} break;

		case CHOOSE: {
			



		} break;

		case GAME: {
			
			OverlayMenu::update(state, platform->currTime, platform->deltaTime);

			if(platform->input.u_pressed){
				if(game->camera_locked){
					game->camera_locked = false;
					game->input_timeout = 0.1;
				} else {
					game->camera_locked = true;	
					game->input_timeout = 0.1;
					camera->Reset();
				}
			}

			if(!game->camera_locked){
				/* camera position update */

				if(platform->input.up_pressed) camera->UpdatePosition(FORWARD, platform->deltaTime);
				if(platform->input.down_pressed) camera->UpdatePosition(BACKWARD, platform->deltaTime);
				if(platform->input.left_pressed) camera->UpdatePosition(LEFT, platform->deltaTime);
				if(platform->input.right_pressed) camera->UpdatePosition(RIGHT, platform->deltaTime);
				
				if(platform->input.right_bracket_pressed) camera->UpdatePosition(UP, platform->deltaTime);
				if(platform->input.left_bracket_pressed) camera->UpdatePosition(DOWN, platform->deltaTime);
				if(platform->input.semicolon_pressed) camera->UpdatePosition(ROT_LEFT, platform->deltaTime);
				if(platform->input.apostrophe_pressed) camera->UpdatePosition(ROT_RIGHT, platform->deltaTime);

				// printf("camera(%f, %f, %f)\n", camera->Position.x, camera->Position.y, camera->Position.z);
				// printf("camera YAW(%f)\n", camera->Yaw);
				// printf("camera PITCH(%f)\n", camera->Pitch);
			} else {
				if(platform->input.apostrophe_pressed){
					game->showDepthMap = !game->showDepthMap;
					game->input_timeout = 0.1;
				}
			}
			
			if(!game->paused){
				//update camera based on state
				//this is just for now, we're going to have a fixed camera in the future.

				Grasses::update(state, platform->currTime, platform->deltaTime);
				Trains::update(state, platform->currTime, platform->deltaTime);
				Particles::update(state, platform->currTime, platform->deltaTime);
				Lights::update(state, platform->currTime, platform->deltaTime);
			}	

			// 1. first render to depth map
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, game->shadowDepthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			
			//configure shader and matrices
			float near_plane = 10.0f, far_plane = 60.5f;
			
			glm::mat4 lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane); 
			glm::mat4 lightView = glm::lookAt(game->lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;

			useShader(game->shadowDepthShader.ID);
			shaderSetMat4(game->shadowDepthShader.ID, "lightSpaceMatrix", lightSpaceMatrix);

			Particles::renderShadow(state, game->shadowDepthShader);
			Grasses::renderShadow(state, game->shadowDepthShader);
			Trains::renderShadow(state, game->shadowDepthShader);
			

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// 2. then render scene as normal with shadow mapping (using depth map)
			glViewport(0, 0, platform->screenWidth, platform->screenWidth); 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, game->shadowDepthMap);
			
			// render scene 
			SkyBoxes::render(state, projection, view);
			Lights::render(state, projection, view);
			Grasses::render(state, projection, view);
			Trains::render(state, projection, view);
			Particles::render(state, projection, view);
			OverlayMenu::render(state, projection, view);

			//Drawing the ground
			//TODO(AL): Move into separate namespace
			// shaderSetMat4(game->groundShader.ID, "model", model);
			Shader shader = game->groundShader;
			unsigned int ID = shader.ID;

			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0, game->ground, 0));
			
			useShader(ID);
			shaderSetMat4(ID, "projection", projection);
			shaderSetMat4(ID, "view", view);
			shaderSetMat4(ID, "model", model);
			shaderSetMat4(ID, "lightSpaceMatrix", lightSpaceMatrix);
			shaderSetVec3(ID, "color", glm::vec3(0.1,0.8,0.1));

			shaderSetInt(ID, "diffuseTexture", 0);
    		shaderSetInt(ID, "shadowMap", 1);

		 	glActiveTexture(GL_TEXTURE1);
        	glBindTexture(GL_TEXTURE_2D, game->shadowDepthMap);

    		glBindVertexArray(game->Plane_VAO);
    		glDrawArrays(GL_TRIANGLES, 0, 6);
    		
    		glBindVertexArray(0);


			if(game->showDepthMap)
			{
				useShader(game->debugDepthQuad.ID);
				shaderSetFloat(game->debugDepthQuad.ID, "near_plane", near_plane);
				shaderSetFloat(game->debugDepthQuad.ID, "far_plane", far_plane);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, game->shadowDepthMap);
				renderQuad();
			}



		} break;
	}

}

void changeScreen(State *state, Screens screen) {
   state->game_state.current_screen = screen; 
}

void paused(State *state, bool paused) {
    state->game_state.paused = paused;
}

static bool shouldClose(State *state){
	if(state->game_state.quit_game){
		printf("SHOULD CLOSE\n");
	}
	return state->game_state.quit_game;
}

static void finalize(State *state){
	printf("FINALIZE\n");
	free(state);
}

static void reload(State *state){
	printf("RELOAD\n");
	
}
static void unload(State *state){
	printf("UNLOAD\n");
}

const GameAPI GAME_API = {
    .init = init,
    .finalize = finalize,
    .reload = reload,
    .unload = unload,
    .updateAndRender = updateAndRender,
    .shouldClose = shouldClose
};
