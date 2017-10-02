#include "MenuImage.h"
#include "ImageFuncs.h"

namespace ChooseMenu {
	
	void setup() {
		
        game->chooseTrain = new Train();
        game->chooseTrain->x = 0.0f;
        game->chooseTrain->y = game->ground;
        game->chooseTrain->z = 5.0f;
		
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
            game->input_timeout = 0.05;
            if (strcmp(curr, "locomotive") == 0)
		        game->trainModel = new Model("train2", "models/rocks/rock4.obj");
            
            else if (strcmp(curr, "train2") == 0)
		        game->trainModel = new Model("locomotive", "models/train/locomotive/Locomotive C36.obj");
        }

        if(input->d_pressed) {
            game->input_timeout = 0.05;
            if (strcmp(curr, "locomotive") == 0)
		        game->trainModel = new Model("train2", "models/rocks/rock4.obj");
            
            else if (strcmp(curr, "train2") == 0)
		        game->trainModel = new Model("locomotive", "models/train/locomotive/Locomotive C36.obj");
        }

        if(input->space_pressed || input->enter_pressed){
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
		
        Shader trainShader = Shaders::get("train");
		unsigned int ID = trainShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(train->x, train->y, train->z));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));

		model = glm::rotate(model, train->y_rot, glm::vec3(0.0, 1.0, 0.0));
		
        shaderSetMat4(ID, "model", model);
		trainModel->Draw(trainShader);
	}

}
