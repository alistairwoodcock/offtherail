#include "Train.h"

namespace Trains {
	
	void setup(State *state){
		
		state->game_state.train->x = 0;
		state->game_state.train->y = -2.0f;
		state->game_state.train->z = 0.0f;
	}

	void update(State *state, float time, float deltaTime){
		
		state->game_state.train->y = -0;
		state->game_state.train->y_rot += 50 * deltaTime;
		state->game_state.train->z_rot = 0 ;

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
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, glm::radians(train->y_rot), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians(train->z_rot), glm::vec3(0.0, 0.0, 1.0));
		shaderSetMat4(ID, "model", model);
		trainModel->Draw(trainShader);

	}
	
}