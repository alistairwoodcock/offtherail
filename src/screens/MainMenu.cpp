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

		game->startText = new MenuImage();
		game->startText->x = 0;
		game->startText->y = -0.2;
		game->startText->z = 0;
		game->startText->scale = glm::vec3(0.5);
		game->startText->scale_vel = 0;
		setupImage("images/start.png", game->startText, logo_vertices, sizeof(logo_vertices));
		
		game->exitText = new MenuImage();
		game->exitText->x = 0;
		game->exitText->y = -0.5;
		game->exitText->z = 0;
		game->exitText->scale = glm::vec3(0.3);
		game->exitText->scale_vel = 0;
		setupImage("images/exit.png", game->exitText, logo_vertices, sizeof(logo_vertices));

		game->testFont = new Font();
		game->testFont->x = 0;
		game->testFont->y = 0;
		game->testFont->z = 0;
		
		setupFont("./fonts/OpenSans-Regular.ttf", game->testFont);

		Shader textShader = Shaders::get("text");
		useShader(textShader.ID);
		shaderSetInt(textShader.ID, "textTexture", 0);
	}	

	void update(float time, float deltaTime){

		game->testFont->y += 1 * deltaTime;
		
		MenuImage *logo = game->logo;
		MenuImage *startText = game->startText;
		MenuImage *exitText = game->exitText;
		
		logo->scale += glm::vec3(1.0)*logo->scale_vel*deltaTime*0.5f;
		startText->scale += glm::vec3(1.0)*startText->scale_vel*deltaTime;
		exitText->scale += glm::vec3(1.0)*exitText->scale_vel*deltaTime;


		if(logo->scale.x > 1.2) logo->scale_vel = -0.1;
		if(logo->scale.x <= 1.17) logo->scale_vel = 0.1;
		
		if(game->start_active){
			if(startText->scale.x < 0.4) startText->scale_vel = 0.5;
			if(startText->scale.x >= 0.4) startText->scale_vel = 0;
		} else {
			if(startText->scale.x > 0.3) startText->scale_vel = -0.5;
			if(startText->scale.x <= 0.3) startText->scale_vel = 0;
		}

		if(game->exit_active){
			if(exitText->scale.x < 0.4) exitText->scale_vel = 0.5;
			if(exitText->scale.x >= 0.4) exitText->scale_vel = 0;
		} else {
			if(exitText->scale.x > 0.3) exitText->scale_vel = -0.5;
			if(exitText->scale.x <= 0.3) exitText->scale_vel = 0;
		}

		Input *input = &platform->input;

		if(game->start_active && input->s_pressed){
			game->start_active = false;
			game->exit_active = true;
		}

		if(game->exit_active && input->w_pressed){
			game->start_active = true;
			game->exit_active = false;
		}

		if(input->space_pressed || input->enter_pressed){
			
			if(game->exit_active){
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
		// renderImage(game->logo);
		// renderImage(game->startText);
		// renderImage(game->exitText);

		renderFont(game->testFont);
	}

}
