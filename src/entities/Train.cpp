#include "Train.h"

namespace Trains {
	
	void setup(State *state){
		GameState *game = &state->game_state;

		game->trainModel = new Model("train", "models/train/locomotive/Locomotive C36.obj");
		game->trainShader = loadShader("train", "src/shaders/train.vs", "src/shaders/train.fs");
    	game->train = new Train();


		game->train->x = 0;
		game->train->y = -2.0f;
		game->train->z = 0.0f;

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
		train->y = -4;
		// train->x =  sin(time) * deltaTime;
		train->z = 0;
		train->z_rot = 0;

		Input input = state->platform.input;

		if(input.a_pressed){
			bogieFront->x -= 0.05;
		} else if(input.d_pressed) {
			bogieFront->x += 0.05;
		}

		if(bogieBack->x > 3){
			bogieBack->x_vel = -2;
		}

		if(bogieBack->x < 1){
			bogieBack->x_vel = 2;
		}

		bogieBack->x += bogieBack->x_vel * deltaTime;
		

		// bogieBack->x -= bogieBack->x;
		// if(bogieBack->x > 2){
		// 	// bogieFront->x = 2;
		// }
		// bogieFront->x += 10 * deltaTime * sin(time);
		// bogieBack->x += 10 * deltaTime; * sin(time);

		// bogieFront->x = 1;
		// bogieBack->x = 1;
		// bogieFront->z = 1;


		printf("bogieFront->x: %f\n", bogieFront->x);

		float b_x_diff = bogieBack->x - bogieFront->x;
		printf("x_diff: %f\n", b_x_diff);
		float b_z_diff = abs(bogieBack->z - bogieFront->z);
		printf("z_diff: %f\n", b_z_diff);
		float angle = atan(b_x_diff/b_z_diff);

		printf("%f\n", angle);

		train->y_rot = angle;
		train->x = (bogieBack->x + bogieFront->x)/2;

	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){
		
		Model *trainModel = state->game_state.trainModel;
		Train *train = state->game_state.train;

		Shader trainShader = state->game_state.trainShader;
		unsigned int ID = trainShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(train->x, train->y, train->z));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));

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

		glDrawArrays(GL_TRIANGLES, 0, 6);


		Entity *bogieBack = state->game_state.bogieBack;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(bogieBack->x, bogieBack->y, bogieBack->z));
		model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
		shaderSetMat4(ID, "model", model);
		shaderSetVec3(ID, "color", glm::vec3(0,0.1,0.0));
		shaderSetFloat(ID, "alpha", 0.5);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glBindVertexArray(0);



	}
	
}