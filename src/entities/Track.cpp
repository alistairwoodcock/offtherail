#ifndef TRACK_CPP
#define TRACK_CPP

#include "Track.h"
#include "../libs/model.h"

namespace Tracks {
	
	void setup() {
		std::cout << "SETTING UP TRACK" << std::endl;

		game->trackModel = new Model("track", "models/smalltrack/smallstraighttextured.obj");
		
		game->trackLen = 62.5;
		game->trackCount = 10;
		for(int i = 0; i < game->trackCount; i++){
			game->tracks[i].x = 0;
			game->tracks[i].y = game->ground - 4.85;
			game->tracks[i].z = game->trackLen - i*game->trackLen;
		}
	}

	void update(float time, float deltaTime) {
		
		for(int i = 0; i < game->trackCount; i++){
			Track *track = &game->tracks[i];

			track->z += game->speed * deltaTime;
			
			if(track->z > 125){
				track->z = game->trackLen - (game->trackCount-1)*game->trackLen;
			}
		}
	}

	void render(glm::mat4 &projection, glm::mat4 &view) {

		Model *trackModel = game->trackModel;


		Shader trackShader = Shaders::get("track");
		unsigned int ID = trackShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		for(int i = 0; i < game->trackCount; i++){
			Track *track = &game->tracks[i];
			
			glm::mat4 model;
			model = glm::scale(model, glm::vec3(0.3f));
			model = glm::translate(model, glm::vec3(track->x, track->y, track->z));
			shaderSetMat4(ID, "model", model);
			trackModel->Draw(trackShader);
		}
	}
	
}

#endif

