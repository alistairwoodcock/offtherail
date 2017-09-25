#include "Train.h"

namespace Trains {
	
	void setup(State *state){
		GameState *game = &state->game_state;

		game->trainModel = new Model("train", "models/train/locomotive/Locomotive C36.obj");
		game->trainShader = loadShader("train", "src/shaders/train.vs", "src/shaders/train.fs");
    	game->train = new Train();

        glm::vec3 s = game->trainModel->size;
        printf("Train size is %f,%f,%f\n", s.x,s.y,s.z);

		game->train->x = 0;
		game->train->y = game->ground;
		game->train->z = 0.0f;

        game->train->size = glm::vec3(0.03f);

        glm::vec3 mSize = game->trainModel->size;
        glm::vec3 tSize = game->train->size;
        game->train->scale = glm::vec3(tSize.x/mSize.x, tSize.y/mSize.y, tSize.z/mSize.z);

		game->bogieFront = new Entity();
		game->bogieFront->z = -1;
		game->bogieBack = new Entity();
		game->bogieBack->z = 9;
	}

	void update(State *state, float time, float deltaTime){
		GameState *game = &state->game_state;

		Entity *train = game->train;
		Entity *bogieFront = game->bogieFront;
		Entity *bogieBack = game->bogieBack;
		
		Input input = state->platform.input;

		if(input.a_pressed){
			bogieFront->x_vel = -5;
		} else if(input.d_pressed) {
			bogieFront->x_vel = 5;
		} else {
			bogieFront->x_vel = 0;
		}

		if(input.s_pressed){
			bogieFront->z_vel = 5;
		} else if(input.w_pressed){
			bogieFront->z_vel = -5;
		} else {
			bogieFront->z_vel = 0;
		}



		float bogie_mid_x = (bogieBack->x + bogieFront->x)/2;
		float bogie_mid_z = (bogieBack->z + bogieFront->z)/2;

		float b_x_diff = bogieBack->x - bogieFront->x;
		float b_z_diff = bogieBack->z - bogieFront->z;

		if(bogieBack->x > bogie_mid_x + 1.5) bogieBack->x_vel = -2;
		if(bogieBack->x < bogie_mid_x - 1.5) bogieBack->x_vel = 2;
		
		if(bogieBack->z > bogie_mid_z + 1.5) bogieBack->z_vel = -2;
		if(bogieBack->z < bogie_mid_z - 1.5) bogieBack->z_vel = 2;

		glm::vec3 v1(bogieBack->x, bogieBack->y, bogieBack->z);
		glm::vec3 v2(bogieFront->x, bogieFront->y, bogieFront->z);
		glm::vec3 dist_vec = glm::normalize(glm::vec3(v1 - v2));
		dist_vec *= 7; //arbitrarily chosen distance between bogies
		dist_vec += v2;

		bogieBack->x = dist_vec.x;
		bogieBack->y = dist_vec.y;
		bogieBack->z = dist_vec.z;


		//Update bogie position
		bogieFront->x += bogieFront->x_vel * deltaTime;
		bogieFront->z += bogieFront->z_vel * deltaTime;

		// bogieBack->x += bogieBack->x_vel * deltaTime;
		// bogieBack->z += bogieBack->z_vel * deltaTime;

		b_x_diff = bogieBack->x - bogieFront->x;
		b_z_diff = bogieBack->z - bogieFront->z;

		if(b_z_diff == 0) b_z_diff = 0.00001;

		float bogie_dist = sqrt(b_z_diff*b_z_diff + b_x_diff*b_x_diff);

		float angle = 0;
		angle = atan(b_x_diff/b_z_diff);

		if(bogieBack->z > bogieFront->z) angle += M_PI;

		
		if(bogie_dist <= 10){
			train->y_rot = angle;
			train->x = (bogieBack->x + bogieFront->x)/2;
			train->z = (bogieBack->z + bogieFront->z)/2;
			train->z_rot = 0;
		} else {
			train->z_rot = M_PI / 2;
			if(bogieFront->x > bogieBack->x){
				train->z_rot *= -1;	
			}
		}
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){
		GameState *game = &state->game_state;
		
		Model *trainModel = game->trainModel;
		Train *train = game->train;

		Shader trainShader = state->game_state.trainShader;
		unsigned int ID = trainShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(train->x, train->y, train->z));
		model = glm::scale(model, game->train->scale);

		model = glm::rotate(model, train->y_rot, glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, train->z_rot, glm::vec3(0.0, 0.0, 1.0));
		shaderSetMat4(ID, "model", model);
		trainModel->Draw(trainShader);

		Shader particleShader = state->game_state.particleShader;
		ID = particleShader.ID;

		useShader(ID);
		glBindVertexArray(state->game_state.Particle_VAO);

		Entity *bogieFront = state->game_state.bogieFront;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(bogieFront->x, bogieFront->y, bogieFront->z));
		model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
		shaderSetMat4(ID, "model", model);
		shaderSetVec3(ID, "color", glm::vec3(0,0.0,1.0));
		shaderSetFloat(ID, "alpha", 0.5);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);


		Entity *bogieBack = state->game_state.bogieBack;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(bogieBack->x, bogieBack->y, bogieBack->z));
		model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
		shaderSetMat4(ID, "model", model);
		shaderSetVec3(ID, "color", glm::vec3(0,0.1,0.0));
		shaderSetFloat(ID, "alpha", 0.5);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glBindVertexArray(0);



	}

	void renderShadow(State *state, Shader &shader){
		
		Model *trainModel = state->game_state.trainModel;
		Train *train = state->game_state.train;

		unsigned int ID = shader.ID;

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(train->x, train->y, train->z));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));

		model = glm::rotate(model, train->y_rot, glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, train->z_rot, glm::vec3(0.0, 0.0, 1.0));
		shaderSetMat4(ID, "model", model);
		
		trainModel->Draw(shader);
	}
	
}
