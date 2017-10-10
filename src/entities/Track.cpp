#ifndef TRACK_CPP
#define TRACK_CPP

#include "Track.h"
#include "../libs/model.h"

namespace Tracks {
	
	void setup() {
		std::cout << "SETTING UP TRACK" << std::endl;

		game->trackModel = new Model("track", "models/smalltrack/smallstraighttextured.obj");
		
		game->trackLen = 62.5 * 0.3;
		game->trackCount = 10;
		game->tracks = (Track*)malloc(sizeof(Track)*game->trackCount*3);

		game->track1 = game->tracks+0;
		game->track2 = game->tracks+10;
		game->track3 = game->tracks+20;


		for(int i = 0; i < game->trackCount; i++){
			game->track1[i].x = -20*0.2;
			game->track1[i].y = game->ground;
			game->track1[i].z = game->trackLen - i*game->trackLen;

			game->track2[i].x = 0;
			game->track2[i].y = game->ground;
			game->track2[i].z = game->trackLen - i*game->trackLen;

			game->track3[i].x = 20*0.2;
			game->track3[i].y = game->ground;
			game->track3[i].z = game->trackLen - i*game->trackLen;
		}

		game->nextSwitchCountdown = rand()%30;
		game->switchesCount = 0;
		game->maxSwitches = 20;
		game->selectedTrack = 0;		



	}

	Track* getTrack(int trackNum){
		switch(trackNum){
			case 0: return game->track1;
			case 1: return game->track2;
			case 2: return game->track3;
		}
	}

	Track* getSelectedTrack(){
		return getTrack(game->selectedTrack);
	}


	
	void changeSwitch(int i, int toTrack){
		if(i == -1 || i > game->switchesCount) return;

		bool canSwitch = true;

		int fromTrack = game->switches[i].fromTrack;

		if(fromTrack == 0 && toTrack == 2){
			canSwitch = false;
		}

		if(fromTrack == 2 && toTrack == 0){
			canSwitch = false;
		}

		if(canSwitch){
			game->switches[i].toTrack = toTrack;
		}
		
		game->switches[i].target_y_rot = M_PI/14 * (game->switches[i].fromTrack - game->switches[i].toTrack);
		game->switches[i].rotate_speed = 1.0f;

		printf("from: %i, to: %i\n", game->switches[i].fromTrack, game->switches[i].toTrack);

	}

	void toggleSwitch(int i){
		if(i == -1 || i > game->switchesCount) return;

		int fromTrack = game->switches[i].fromTrack;
		int toTrack = game->switches[i].toTrack;

		if(fromTrack == 0)
		{
			if(toTrack == 0)
			{
				toTrack = 1;
			}
			else
			{
				toTrack = 0;
			}
		}
		else if(fromTrack == 1)
		{
			if(toTrack == 0)
			{
				toTrack = 1;
			}
			else if(toTrack == 1)
			{
				toTrack = 2;
			}
			else
			{
				toTrack = 0;
			}
		}
		else if(fromTrack == 2)
		{
			if(toTrack == 2)
			{
				toTrack = 1;
			}
			else
			{
				toTrack = 2;
			}
		}


		changeSwitch(i, toTrack);
	}

	void placeSwitch(){
		printf("place switch\n");

		// game->speed += 4;

		if(game->switchesCount == game->maxSwitches) return;

		int i = game->switchesCount;
		
		game->switches[i].y = game->ground;
		game->switches[i].z = game->trackLen - (game->trackCount-1)*game->trackLen;
		game->switches[i].y_rot = 0;
		
		int rndTrackNum = (int)(rand()%3);
		int toTrack = 0;
		
		game->switches[i].fromTrack = rndTrackNum;

		Track *t = NULL;
		switch(rndTrackNum){
			case 0:{
				t = game->track1;
				toTrack = 1;

			} break;
			case 1: {
				t = game->track2; 
				
				toTrack = 0;

				int switchDir = (int)rand()%2;
				if(switchDir == 1){
					toTrack = 2;
				}

			} break;
			case 2: {

				t = game->track3;
				toTrack = 1;

			} break;
		}

		changeSwitch(i, toTrack);

		game->switches[i].x = t->x;


		game->switchesCount++;
	}



	void update(float time, float deltaTime) {

		
		for(int i = 0; i < game->trackCount * 3; i++){

			Track *track = &game->tracks[i];

			track->z += game->speed * deltaTime;
			
			if(track->z > 125 * 0.3){
				track->z = game->trackLen - (game->trackCount-1)*game->trackLen;
			}
		}

		game->nextSwitchCountdown -= deltaTime;

		if(game->nextSwitchCountdown < 0){
			game->nextSwitchCountdown = rand()%10;

			placeSwitch();
		}

		Input input = platform->input;

		if(input.a_pressed){
			game->selectedTrack -= 1;
			if(game->selectedTrack < 0){
				game->selectedTrack = 0;
			}	
			game->input_timeout = 0.1;
		}

		if(input.d_pressed){
			game->selectedTrack += 1;
			if(game->selectedTrack > 2){
				game->selectedTrack = 2;
			}
			game->input_timeout = 0.1;
		}

		//update switches and set the current one
		int closestSwitch = -1;

		for(int i = 0; i < game->switchesCount; i++){
			TrackSwitch *s = &game->switches[i];

			s->z += game->speed * deltaTime;

			float rot_diff = s->target_y_rot - s->y_rot;

			if(rot_diff != 0)
			{
				float dir = abs(rot_diff) / rot_diff;

				s->y_rot += dir * game->switches[i].rotate_speed * deltaTime;


				printf("s->target_rot: %f,  s->y_rot: %f,  rot_diff: %f\n", s->target_y_rot, s->y_rot, rot_diff);

				if(abs(rot_diff) < 0.01){
					s->y_rot = s->target_y_rot;
					s->rotate_speed = 0;
				}
			}

			if(s->z > 125 * 0.3){
				for(int f = 0; f < game->switchesCount; f++){
					game->switches[f] = game->switches[f+1];
				}
				game->switchesCount--;
			}

			bool onSelectedTrack = s->fromTrack == game->selectedTrack;
			
			if(onSelectedTrack && s->z < 10)
			{
				if(closestSwitch == -1) closestSwitch = i;
				
				if(s->z > game->switches[closestSwitch].z) closestSwitch = i;
			}
		}

		game->selectedSwitch = closestSwitch;

		if(input.space_pressed)
		{
			toggleSwitch(game->selectedSwitch);
			game->input_timeout = 0.1;
		}
	}

	void render(glm::mat4 &projection, glm::mat4 &view) {

		Model *trackModel = game->trackModel;


		Shader trackShader = Shaders::get("track");
		unsigned int ID = trackShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		for(int i = 0; i < game->trackCount * 3; i++){
			Track *track = &game->tracks[i];

			bool highlight = (i < 10 && i >= 0 && game->selectedTrack == 0) || 
							 (i < 20 && i >= 10 && game->selectedTrack == 1) ||
							 (i < 30 && i >= 20 && game->selectedTrack == 2);


			if(highlight){
				shaderSetVec3(ID, "colour", glm::vec3(0.7f, 0.7f, 0.95f));
			} else {
				shaderSetVec3(ID, "colour", glm::vec3(0.0f, 0.0f, 0.0f));
			}
					
			
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(track->x, track->y, track->z));
			model = glm::scale(model, glm::vec3(0.3f));
			shaderSetMat4(ID, "model", model);
			trackModel->Draw(trackShader);

			
		}
		
		
		for(int i = 0; i < game->switchesCount; i++){
			TrackSwitch *s = &game->switches[i];
			
			if(i == game->selectedSwitch){
				shaderSetVec3(ID, "colour", glm::vec3(0.5f, 1.0f, 1.0f));
			} else {
				shaderSetVec3(ID, "colour", glm::vec3(0.0f, 0.0f, 0.0f));
			}

			glm::mat4 model;
			model = glm::translate(model, glm::vec3(s->x, s->y, s->z));
			model = glm::rotate(model, s->y_rot, glm::vec3(0.0, 1.0, 0.0));
			model = glm::scale(model, glm::vec3(0.3f));
			shaderSetMat4(ID, "model", model);
			trackModel->Draw(trackShader);
		}
	}
	
}

#endif

