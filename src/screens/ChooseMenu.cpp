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

		game->logo = new MenuImage();
		game->logo->x = 0;
		game->logo->y = 0.65;
		game->logo->z = 0;
		game->logo->scale = glm::vec3(1.1);
		game->logo->scale_vel = 0.1;
		setupImage("images/logo.png", game->logo, logo_vertices, sizeof(logo_vertices));


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

        const char* curr = game->trainModel->name.c_str();

        bool nextTrain = false;

        if(input->a_pressed || input->left_pressed) {
            game->input_timeout = 0.1;
            nextTrain = true;
        }

        if(input->d_pressed || input->right_pressed) {
            game->input_timeout = 0.1;
            nextTrain = true;
        }

        if(nextTrain){
        	if (strcmp(curr, "locomotive") == 0){
            	Trains::setTrainModel(ROCK);
            } else if (strcmp(curr, "train2") == 0) {
            	Trains::setTrainModel(OUR_BOY_THOMAS);
            } else if (strcmp(curr, "thomas") == 0) {
            	Trains::setTrainModel(DEFAULT);
            }
        }

        if (strcmp(curr, game->trainModel->name.c_str()) != 0) {
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
		renderImage(game->logo, projection, view);
		renderImage(game->chooseRight, projection, view);
		renderImage(game->chooseLeft, projection, view);
        
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
        }


	}

}
