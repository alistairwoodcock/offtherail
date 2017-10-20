#include "MenuImage.h"
#include "ImageFuncs.h"

namespace MainMenu{


	void my_stbtt_print(float x, float y, char *text)
	{
		printf("attempting to render: %s\n", text);
	   // assume orthographic projection with units = screen pixels, origin at top left
	   // glEnable(GL_TEXTURE_2D);
	   // glBindTexture(GL_TEXTURE_2D, ftex);
	   // glBegin(GL_QUADS);
	   // while (*text) {
	   //    if (*text >= 32 && *text < 128) {
	   //       stbtt_aligned_quad q;
	   //       stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
	   //       glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y0);
	   //       glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y0);
	   //       glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y1);
	   //       glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y1);
	   //    }
	   //    ++text;
	   // }
	   // glEnd();
	}
	
	void setup(){

		game->start_active = true;
		game->exit_active = false;

		float logo_vertices[] = {
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
	         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		};

		game->logo = new MenuImage();
		game->logo->x = 0;
		game->logo->y = 0.4;
		game->logo->z = 0;
		game->logo->scale = glm::vec3(1.1);
		game->logo->scale_vel = 0.1;
		setupImage("images/logo.png", game->logo, logo_vertices, sizeof(logo_vertices));

		game->startText = createTextArea(game->comicSans, 320, 128, 128, "START", 7);
		game->startText->x = 0.0;
		game->startText->y = -0.2;
		game->startText->z = 0;
		game->startText->scale = glm::vec3(0.15);
		game->startText->scale_vel = 0;
		game->startText->colour = glm::vec4(1,0,0,1);
		// setupImage("images/start.png", game->startText, logo_vertices, sizeof(logo_vertices));

		game->exitText = createTextArea(game->comicSans, 250, 128, 128, "EXIT", 7);
		game->exitText->x = 0.0;
		game->exitText->y = -0.5;
		game->exitText->z = 0;
		game->exitText->scale = glm::vec3(0.15);
		game->exitText->scale_vel = 0;
		// setupImage("images/exit.png", game->exitText, logo_vertices, sizeof(logo_vertices));

		// game->testFont = createFont("./fonts/OpenSans-Regular.ttf");
		// game->testTextArea = createTextArea(game->testFont, 512, 256, 256, "Hello, World!");

		Shader textShader = Shaders::get("text");
		useShader(textShader.ID);
		shaderSetInt(textShader.ID, "textTexture", 0);
	}	

	void update(float time, float deltaTime){


		
		game->startText->colour = glm::vec4(sin(time), cos(time), sin(time), 1.0f);
		
		MenuImage *logo = game->logo;
		TextArea *startText = game->startText;
		TextArea *exitText = game->exitText;
		
		logo->scale += glm::vec3(1.0)*logo->scale_vel*deltaTime*0.5f;
		startText->scale += glm::vec3(1.0)*startText->scale_vel*deltaTime;
		exitText->scale += glm::vec3(1.0)*exitText->scale_vel*deltaTime;


		if(logo->scale.x > 1.2) logo->scale_vel = -0.1;
		if(logo->scale.x <= 1.17) logo->scale_vel = 0.1;
		
		if(game->start_active){
			if(startText->scale.x < 0.15) startText->scale_vel = 0.5;
			if(startText->scale.x >= 0.15) startText->scale_vel = 0;
		} else {
			if(startText->scale.x > 0.07) startText->scale_vel = -0.5;
			if(startText->scale.x <= 0.07) startText->scale_vel = 0;
		}

		if(game->exit_active){
			if(exitText->scale.x < 0.15) exitText->scale_vel = 0.5;
			if(exitText->scale.x >= 0.15) exitText->scale_vel = 0;
		} else {
			if(exitText->scale.x > 0.07) exitText->scale_vel = -0.5;
			if(exitText->scale.x <= 0.07) exitText->scale_vel = 0;
		}

		Input *input = &platform->input;

		if(game->start_active && input->s_pressed){
			game->start_active = false;
			game->exit_active = true;
            Music::soundEffect("sounds/oh.wav");
		}

		if(game->exit_active && input->w_pressed){
			game->start_active = true;
			game->exit_active = false;
            Music::soundEffect("sounds/oh.wav");
		}

		if(input->space_pressed || input->enter_pressed){
			
			if(game->exit_active){
                Music::pause(true);
                Music::soundEffect("sounds/exit.wav", true);
				game->quit_game = true;
			} 
			if(game->start_active){
                changeScreen(CHOOSE);
                game->input_timeout = 0.5;
			}
		}

		if(input->escape_pressed)
		{
			game->quit_game = true;
		}
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		
		renderImage(game->logo);
		
		renderText(game->startText);
		renderText(game->exitText);

		// renderText(game->testTextArea);
	}

}
