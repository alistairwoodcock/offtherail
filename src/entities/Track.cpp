#ifndef TRACK_CPP
#define TRACK_CPP

#include "Track.h"
#include "../libs/model.h"

namespace Tracks {
	
	void setup(State *state) {
		GameState *game = &state->game_state;
		std::cout << "SETTING UP TRACK" << std::endl;

		Model* trackModel = new Model("track", "models/track/longstraight.obj");
		game->trackModel = trackModel;
		game->track = new Track();

		game->track->x = 0;
		game->track->y = game->ground;
		game->track->z = 0.0f;

	}


	void update(State *state, float time, float deltaTime) {
		if (state->game_state.track->z < 200) {
			state->game_state.track->z++;
		}
		else {
			state->game_state.track->z = 0;
		}
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view) {

		Model *trackModel = state->game_state.trackModel;
		Track *track = state->game_state.track;

		Shader trackShader = Shaders::get(state, "track");
		unsigned int ID = trackShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		// render the loaded model
		glm::mat4 model;

		//model = glm::translate(model, glm::vec3(train->x, train->y, train->z));
		//model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));

		//model = glm::rotate(model, train->y_rot, glm::vec3(0.0, 1.0, 0.0));
		//model = glm::rotate(model, train->z_rot, glm::vec3(0.0, 0.0, 1.0));

		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		//model = glm::translate(model, glm::vec3(track->x, track->y, track->z));
		model = glm::translate(model, glm::vec3(track->x, -0.6, track->z));

		shaderSetMat4(ID, "model", model);
		trackModel->Draw(trackShader);

	}

	
}

#endif

