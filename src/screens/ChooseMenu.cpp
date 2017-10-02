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
		game->logo->y = 0.4;
		game->logo->z = 0;
		game->logo->scale = glm::vec3(1.1);
		game->logo->scale_vel = 0.1;
		setupImage("images/logo.png", game->logo, logo_vertices, sizeof(logo_vertices));

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

        if(input->a_pressed) {
            game->input_timeout = 0.1;
            if (strcmp(curr, "locomotive") == 0)
		        game->trainModel = new Model("train2", "models/rocks/rock4.obj", glm::vec3(0.2f));
            
            else if (strcmp(curr, "train2") == 0)
		        game->trainModel = new Model("locomotive", "models/train/locomotive/Locomotive C36.obj", glm::vec3(65.0f));
        }

        if(input->d_pressed) {
            game->input_timeout = 0.1;
            if (strcmp(curr, "locomotive") == 0)
		        game->trainModel = new Model("train2", "models/rocks/rock4.obj", glm::vec3(0.2f));
            
            else if (strcmp(curr, "train2") == 0)
		        game->trainModel = new Model("locomotive", "models/train/locomotive/Locomotive C36.obj", glm::vec3(65.0f));
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
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		renderImage(game->logo, projection, view);
        
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
