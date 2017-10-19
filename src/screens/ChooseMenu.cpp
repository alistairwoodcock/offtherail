#include "MenuImage.h"
#include "ImageFuncs.h"

#include "glm/glm.hpp"

namespace ChooseMenu {

	void updateScale() {
		glm::vec3 mSize = game->trainModel->size;
		glm::vec3 tSize = game->chooseTrain->size;
		game->chooseTrain->scale = glm::vec3(tSize.x/mSize.x, tSize.y/mSize.y, tSize.z/mSize.z);
	}

	void setup() {
		
		game->chooseTrain = new Train();
		game->chooseTrain->x = 0.0f;
		game->chooseTrain->y = game->ground;
		game->chooseTrain->z = 5.0f;

		game->chooseTrain->size = glm::vec3(1.0f);
		updateScale();
		
		float logo_vertices[] = {
			-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		};

		game->chooseRight = new MenuImage();
		game->chooseRight->x = 0.75;
		game->chooseRight->y = 0;
		game->chooseRight->z = 0;
		game->chooseRight->scale = glm::vec3(0.4);
		game->chooseRight->scale_vel = 0.1;
		setupImage("images/arrow_right.png", game->chooseRight, logo_vertices, sizeof(logo_vertices));

		game->chooseLeft = new MenuImage();
		game->chooseLeft->x = -0.75;
		game->chooseLeft->y = 0;
		game->chooseLeft->z = 0;
		game->chooseLeft->scale = glm::vec3(0.4);
		game->chooseLeft->scale_vel = 0.1;
		setupImage("images/arrow_left.png", game->chooseLeft, logo_vertices, sizeof(logo_vertices));

		Shader textShader = Shaders::get("text");
		useShader(textShader.ID);
		shaderSetInt(textShader.ID, "textTexture", 0);
	}	

	void update(float time, float deltaTime){
		
		MenuImage *logo = game->logo;


		logo->scale += glm::vec3(1.0)*logo->scale_vel*deltaTime*0.5f;

		if(logo->scale.x > 1.2) logo->scale_vel = -0.1;
		if(logo->scale.x <= 1.17) logo->scale_vel = 0.1;
		
		Input *input = &platform->input;

		TrainTypes current = game->currentTrain;

		if(input->a_pressed || input->left_pressed) {
			game->input_timeout = 0.1;
			TrainTypes prev = static_cast<TrainTypes>((current - 1 + TRAIN_MODEL_NUM) % TRAIN_MODEL_NUM);
			Trains::setTrainModel(prev);
			updateScale();
		}

		if(input->d_pressed || input->right_pressed) {
			game->input_timeout = 0.1;
			TrainTypes next = static_cast<TrainTypes>((current + 1) % TRAIN_MODEL_NUM);
			Trains::setTrainModel(next);
			updateScale();
		}

		if(input->space_pressed || input->enter_pressed) {
			changeScreen(GAME);
			game->input_timeout = 0.5;
		}

		if(input->escape_pressed) {
			changeScreen(MAIN_MENU);
			game->input_timeout = 0.5;
		}

		// Update Mr Train
		Train *train = game->chooseTrain;
		train->y_rot += 0.5 * deltaTime;
		train->y = game->ground + sin(time) * 0.8;
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		renderImage(game->logo);
		renderImage(game->chooseRight);
		renderImage(game->chooseLeft);
		
		// Draw Mr Train
		Model *trainModel = game->trainModel;
		Train *train = game->chooseTrain;
		
		// TEMP GRASS SHIT //
		const char* curr = game->trainModel->name.c_str();
		bool rock = strcmp(curr, "train2") == 0;
	   
		Shader grassShader;
		Shader trainShader;
		unsigned int ID;
		if (rock) {
		grassShader = Shaders::get("grass");
		ID = grassShader.ID;
		} else {
		trainShader = Shaders::get("train");
		ID = trainShader.ID;
		}

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(train->x, train->y, train->z));
		model = glm::scale(model, train->scale);

		model = glm::rotate(model, train->y_rot, glm::vec3(0.0, 1.0, 0.0));

		// Anothe temp grass/rock shit    
		if (rock) {
			shaderSetVec3(ID, "objectColor", 1.0f, 1.0f, 1.0f);
			shaderSetVec3(ID, "lightColor", 1.0f, 0.0f, 0.0f);
			shaderSetVec3(ID, "viewPos", game->camera.Position);
			shaderSetVec3(ID, "material.ambient",  1.0f, 1.0f, 1.0f);
			shaderSetVec3(ID, "material.diffuse",  0.5f, 0.5f, 0.31f);
			shaderSetVec3(ID, "material.specular", 0.25f, 0.25f, 0.25f);
			shaderSetFloat(ID, "material.shininess", 128.0f);
				
			shaderSetMat4(ID, "model", model);
			shaderSetMat4(ID, "inverseModel", glm::inverse(model));
				
			trainModel->Draw(grassShader);
		} else {
			shaderSetMat4(ID, "model", model);
			trainModel->Draw(trainShader);
		
			// Reflections??

			glBindVertexArray(game->Particle_VAO);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(0.0, game->ground, 5.0));
			model = glm::scale(model, glm::vec3(15.0f,15.0f,15.0f));
			model = glm::rotate(model, -3.1415f / 2, glm::vec3(1.0, 0.0, 0.0));
			shaderSetMat4(ID, "model", model);
			shaderSetVec3(ID, "color", glm::vec3(0.5));
			shaderSetFloat(ID, "alpha", 0.5);

			glEnable(GL_STENCIL_TEST);
			
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilMask(0xFF);
			
			glDepthMask(GL_FALSE);
			glClear(GL_STENCIL_BUFFER_BIT);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			glStencilFunc(GL_EQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDepthMask(GL_TRUE);
			glBindVertexArray(0);
		
			model = glm::mat4(); 
			model = glm::translate(model, glm::vec3(train->x, 3*game->ground - train->y, train->z));
			
			model = glm::scale(model, glm::vec3(1, -1, 1));
			model = glm::scale(model, train->scale);
			model = glm::rotate(model, train->y_rot, glm::vec3(0.0, 1.0, 0.0));
			shaderSetMat4(ID, "model", model);
			trainModel->Draw(trainShader);
			
			glDisable(GL_STENCIL_TEST);
		}




	}

}