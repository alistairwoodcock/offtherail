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
#include "libs/music.cpp"
#include "libs/font.cpp"


#include "entities/Camera.h"
#include "entities/Particles.cpp"
#include "entities/Grass.cpp"
#include "entities/Puddle.cpp"
#include "entities/Track.cpp"
#include "entities/Train.cpp"
#include "entities/Lights.cpp"
#include "entities/Ground.cpp"
#include "entities/SkyBox.cpp"

#include "screens/MainMenu.cpp"
#include "screens/ChooseMenu.cpp"
#include "screens/OverlayMenu.cpp"

//the larger these are, the higher resolution shadow we can have
const unsigned int SHADOW_WIDTH = 1024*4, SHADOW_HEIGHT = 1024*4;

static void init(State *state)
{
	printf("INIT\n");
	
	//Setup our entire game and GL state 

	GlobalState = state;
	game = &state->game_state;
	platform = &state->platform;

	//Game state for runtime
	game->game_started = false;
	game->quit_game = false;
	
	game->camera_locked = true;
	game->input_timeout = 0;

	game->ground = -2;
	game->speed = 70;

	game->showDepthMap = false;

	//GL Setup
	glViewport(0, 0, state->platform.screenWidth, state->platform.screenHeight);
	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	/* -- Music Setup -- */
	Music::init(); // Need to init before first changeScreen()
	
	/* -- Shaders Setup -- */
	Shaders::setup();

 
	/* -- Particles Setup -- */
	Particles::setup();
	
	/* -- Lights Setup -- */
	Lights::setup();

	/* -- Grasss Setup -- */
	Grasses::setup();
	Puddles::setup();
	
	/* -- Camera Setup -- */
	game->camera = Camera(glm::vec3(0.0f, 11.71f, 34.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -17.0f);
	
	/* -- Font Setup -- */
	game->openSans = createFont("./fonts/OpenSans-Regular.ttf");
	game->comicSans = createFont("./fonts/comic.ttf");

	/* -- Set Up Sky --*/
	SkyBoxes::setup();

	/* -- Train Setup -- */
	Trains::setup();

	/* -- Track Setup -- */
	Tracks::setup();
	
	/* -- Menu Setup --*/
	MainMenu::setup();
	ChooseMenu::setup();
	OverlayMenu::setup();

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
	
	game->lightPos = glm::vec3(15, 30, -40);

	/* -- Ground Setup -- */
	Ground::setup();

	/* -- Score Setup -- */
	game->score = 0;
	game->pointCountdown = 0;

	game->scoreText = createTextArea(game->comicSans, 700, 520, 128, "SCORE: 00000", 12);
	game->scoreText->x = 0;
	game->scoreText->y = 0.7;
	game->scoreText->z = 0;
	game->scoreText->scale = glm::vec3(0.3);
	game->scoreText->scale_vel = 0;


	changeScreen(MAIN_MENU);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions		// texture Coords
			-1.0f,	-0.25f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 -0.25f, -0.25f, 0.0f, 1.0f, 1.0f,
			 -0.25f, -1.0f, 0.0f, 1.0f, 0.0f,
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


static void updateAndRender(){

	glm::vec3 background(1,1,1);
	glClearColor(background.x, background.y, background.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
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
	float drawDistance = 160.0f;
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)platform->screenWidth / (float)platform->screenHeight, 0.1f, drawDistance);
	glm::mat4 view = camera->GetViewMatrix();

	// update shaders
	Shaders::update();
	
	switch(game->current_screen)
	{
		case MAIN_MENU: {
			
			MainMenu::update(platform->currTime, platform->deltaTime);
			MainMenu::render(projection, view);

		} break;

		case CHOOSE: {
			
			ChooseMenu::update(platform->currTime, platform->deltaTime);
			ChooseMenu::render(projection, view);

		} break;

		case GAME: {
			
			OverlayMenu::update(platform->currTime, platform->deltaTime);
				
			/* UPDATE SCORING */	
			if(game->pointCountdown > 0){
				game->pointCountdown -= platform->deltaTime;
			}

			if(game->bogieFront->currentTrack != game->bogieBack->currentTrack){
				if(game->pointCountdown <= 0 && !game->fallen) {
					game->pointCountdown = 0.5;
					game->score += 1;

					char buff[1000];
					sprintf(buff, "SCORE: %05i\0", game->score);
					setText(game->scoreText, buff);

				}			
			}
			
			if(platform->input.u_pressed){
				if(game->camera_locked){
					game->camera_locked = false;
					game->input_timeout = 0.1;
				} else {
					game->camera_locked = true;	
					game->input_timeout = 0.1;
					camera->Reset();
					game->speed = 70; // Reset speed
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

				if(platform->input.left_shift_pressed) game->speed -= 1;
				if(platform->input.right_shift_pressed) game->speed += 1;
			} else {
				if(platform->input.apostrophe_pressed){
					game->showDepthMap = !game->showDepthMap;
					game->input_timeout = 0.1;
				}

				if(game->showDepthMap){
					int s = 10;
					if(platform->input.up_pressed) game->lightPos.y += s * platform->deltaTime;
					if(platform->input.down_pressed) game->lightPos.y -= s * platform->deltaTime;
					if(platform->input.left_pressed) game->lightPos.x -= s * platform->deltaTime;
					if(platform->input.right_pressed) game->lightPos.x += s * platform->deltaTime;
				}
			}
			
			if(!game->paused){
				//update camera based on state
				//this is just for now, we're going to have a fixed camera in the future.

				Grasses::update(platform->currTime, platform->deltaTime);
				Puddles::update(platform->currTime, platform->deltaTime);
				Trains::update(platform->currTime, platform->deltaTime);
				Particles::update(platform->currTime, platform->deltaTime);
				Lights::update(platform->currTime, platform->deltaTime);
				Tracks::update(platform->currTime, platform->deltaTime);

			}	

			//First render to depth map (for shadows)
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, game->shadowDepthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			
			//Shadow matrices
			float near_plane = 0.0f, far_plane = 100.5f;
			glm::mat4 lightProjection = glm::ortho(-64.0f, 64.0f, -64.0f, 64.0f, near_plane, far_plane); 
			// game->lightPos *= 1.01;
			glm::mat4 lightView = glm::lookAt(game->lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;

			//Render to depth buffer to produce shadows
			Shader shadow = Shaders::get("simpleDepthShader");

			useShader(shadow.ID);
			shaderSetMat4(shadow.ID, "lightSpaceMatrix", lightSpaceMatrix);

			//Render anything you want to have shadows here
			Grasses::renderShadow(shadow);
			Trains::renderShadow(shadow);
			
			//Render scene as normal with shadow mapping (using depth map)
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, platform->screenWidth, platform->screenWidth); 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			// render scene 
			SkyBoxes::render(projection, view);
		glDepthMask(GL_FALSE);
			Ground::render(projection, view, lightSpaceMatrix); //ground first for shadows
			Puddles::render(projection, view);
		glDepthMask(GL_TRUE);

			Lights::render(projection, view);
			Grasses::render(projection, view);
			Tracks::render(projection, view);
			Trains::render(projection, view);
			Particles::render(projection, view);
			OverlayMenu::render(projection, view);

			
			if(game->showDepthMap)
			{
				Shader debug = Shaders::get("debugQuad");

				useShader(debug.ID);
				shaderSetFloat(debug.ID, "near_plane", near_plane);
				shaderSetFloat(debug.ID, "far_plane", far_plane);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, game->shadowDepthMap);
				renderQuad();
			}


			renderText(game->scoreText);

		} break;


	}

	/* -- FONT UPDATE -- */
	if(platform->input.c_pressed){
		//we are switching the fonts for all TextAreas (manual process for now)

		Font* newFont = game->openSans;
		if(game->startText->font == game->openSans){
			newFont = game->comicSans;
			game->startText->min_kern = 0;
			game->exitText->min_kern = 0;
			game->resumeText->min_kern = 0;
			game->scoreText->min_kern = 0;
		} else {
			game->startText->min_kern = 10;
			game->exitText->min_kern = 10;
			game->resumeText->min_kern = 10;
			game->scoreText->min_kern = 6;
		}

		setFont(game->startText, newFont);

		setFont(game->exitText, newFont);
		setFont(game->resumeText, newFont);
		setFont(game->scoreText, newFont);

		game->input_timeout = 0.1;

	}

}

void reset() {
	game->score = 0;
	setText(game->scoreText, "SCORE: 00000");
	game->speed = 70;
	Trains::reset();
	Tracks::reset();
}

void changeScreen(Screens screen) {
	game->current_screen = screen; 

	switch(screen) {
		case MAIN_MENU: {
			Music::play("sounds/running90s.wav");
		} break;

		case CHOOSE: {

		} break;

		case GAME: {
			Music::play("sounds/dejavu.wav");
		} break;
	}

}

void paused(bool paused) {
	game->paused = paused;
	Music::pause(paused);
}

static bool shouldClose(){
	if(game->quit_game){
		printf("SHOULD CLOSE\n");
	}
	return game->quit_game;
}

static void finalize(State *state){
	printf("FINALIZE\n");
	free(state);
	Music::destroy();
}

static void reload(State *state){
	printf("RELOAD\n");
	GlobalState = state;
	game = &state->game_state;
	platform = &state->platform;
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
