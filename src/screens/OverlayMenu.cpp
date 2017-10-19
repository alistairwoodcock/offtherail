#include "MenuImage.h"
#include "ImageFuncs.h"

namespace OverlayMenu{
	
	void setup(){
		
		game->overlay_active = false;
		game->resume_active = false;
		game->exit_active = false;

		float logo_vertices[] = {
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
	         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
	        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
	        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		};

		game->overlay = new Entity();
		game->overlay->x = 0;
		game->overlay->y = 0;
		game->overlay->z = 5;

		game->resumeText = createTextArea(game->comicSans, 700, 512, 128, "RESUME", 7);
		game->resumeText->x = 0.25;
		game->resumeText->y = -0.4;
		game->resumeText->z = 0;
		game->resumeText->scale = glm::vec3(0.4);
		game->resumeText->scale_vel = 0;
	}	

	void update(float time, float deltaTime){
		
		Input input = platform->input;

		if(!game->overlay_active && (input.p_pressed || input.escape_pressed)){
			game->overlay_active = true;
			game->resume_active = true;
            paused(true);
		} 

		if(!game->overlay_active){
			return;
		}

		MenuImage *logo = game->logo;
		TextArea *resumeText = game->resumeText;
		TextArea *exitText = game->exitText;
		
		logo->scale += glm::vec3(1.0)*logo->scale_vel*deltaTime*0.5f;
		resumeText->scale += glm::vec3(1.0)*resumeText->scale_vel*deltaTime;
		exitText->scale += glm::vec3(1.0)*exitText->scale_vel*deltaTime;
		
		if(logo->scale.x > 1.2) logo->scale_vel = -0.1;
		if(logo->scale.x <= 1.17) logo->scale_vel = 0.1;
		
		if(game->resume_active){
			if(resumeText->scale.x < 0.4) resumeText->scale_vel = 0.5;
			if(resumeText->scale.x >= 0.4) resumeText->scale_vel = 0;
		} else {
			if(resumeText->scale.x > 0.3) resumeText->scale_vel = -0.5;
			if(resumeText->scale.x <= 0.3) resumeText->scale_vel = 0;
		}

		if(game->exit_active){
			if(exitText->scale.x < 0.4) exitText->scale_vel = 0.5;
			if(exitText->scale.x >= 0.4) exitText->scale_vel = 0;
		} else {
			if(exitText->scale.x > 0.3) exitText->scale_vel = -0.5;
			if(exitText->scale.x <= 0.3) exitText->scale_vel = 0;
		}


		if(game->resume_active && input.s_pressed){
			game->resume_active = false;
			game->exit_active = true;
		}

		if(game->exit_active && input.w_pressed){
			game->resume_active = true;
			game->exit_active = false;
		}

		if(input.space_pressed || input.enter_pressed){
			
			if(game->exit_active){
                changeScreen(MAIN_MENU);
                paused(false);
				game->overlay_active = false;
				game->exit_active = false;
				game->resume_active = false;
				game->start_active = true;
				game->input_timeout = 0.5;
			} 
			if(game->resume_active){
                changeScreen(GAME);
                paused(false);
				game->overlay_active = false;
			}
		}
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		if(game->overlay_active){
			Entity *overlay = game->overlay;

			renderImage(game->logo);
			
			renderText(game->resumeText);
			renderText(game->exitText);

			Shader shader = Shaders::get("flat");
			unsigned int ID = shader.ID;

			glm::mat4 model;
			model = glm::translate(model, glm::vec3(overlay->x,overlay->y,overlay->z));
			model = glm::scale(model, glm::vec3(10.0f));
			
			glBindVertexArray(game->Particle_VAO);
			useShader(ID);
			shaderSetVec3(ID, "color", glm::vec3(1.0,1,1.0));
			shaderSetFloat(ID, "alpha", 0.7);
			shaderSetMat4(ID, "position", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
	
		}
	}

}
