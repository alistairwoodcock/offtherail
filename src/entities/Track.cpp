#ifndef TRACK_CPP
#define TRACK_CPP

#include "Track.h"
#include "../libs/model.h"

namespace Tracks {
	
	void setup() {
		printf("SETTING UP TRACK\n");

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
		
	}

	Track* getTrack(int trackNum){
		switch(trackNum){
			case 0: return game->track1;
			case 1: return game->track2;
			case 2: return game->track3;
		}
		return NULL;
	}

	Track* getSelectedTrack(){
		return getTrack(game->selectedTrack);
	}

	
	void placeSwitch(){
		printf("place switch\n");

		if(game->switchesCount == game->maxSwitches) return;

		int x, y, z;
		int rndTrackNum = (int)(rand()%3);
		int toTrack = 0;
		int fromTrack = rndTrackNum;
		
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

		y = game->ground;
		z = game->trackLen - (game->trackCount-1)*game->trackLen;
		x = t->x;

		Switches::addSwitch(x, y, z, fromTrack, toTrack);
	}



	void update(float time, float deltaTime) {
		
		//update tracks and also get the furthest behind
		int lastTrack = 0;
		for(int i = 0; i < game->trackCount * 3; i++){
			game->tracks[i].z += game->speed * deltaTime;
			if(game->tracks[i].z < game->tracks[lastTrack].z) lastTrack = i;
		}

		for(int i = 0; i < game->trackCount * 3; i++){
			Track *t = &game->tracks[i];

			//move track back to last
			if(t->z > 125 * 0.3){
				t->z = game->tracks[lastTrack].z - game->trackLen;
			}
		}


		game->nextSwitchCountdown -= deltaTime;

		if(game->nextSwitchCountdown < 0){
			game->nextSwitchCountdown = rand()%5+0.5;

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

	}

	void render(glm::mat4 &projection, glm::mat4 &view) {

		Model *trackModel = game->trackModel;


		Shader trackShader = Shaders::get("track");
		unsigned int ID = trackShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		bool switchSelected = false;
		Switch *selectedSwitch = NULL;

		if(game->selectedSwitch >= 0){
			switchSelected = true;
			selectedSwitch = &game->switches[game->selectedSwitch];
		}


		for(int i = 0; i < game->trackCount * 3; i++){
			Track *track = &game->tracks[i];

			//tracks are separated into 3 groups (0-9, 10-19, 20-29)
			//so we only wanna higlight certain tracks if their selected track matches

			int trackNum = 0;
			if(i < 20 && i >= 10) trackNum = 1;
			if(i < 30 && i >= 20) trackNum = 2;

			bool highlight = false;

			if(switchSelected)
			{
				if(track->z <= selectedSwitch->z && selectedSwitch->toTrack == trackNum){
					highlight = true;
				}

				if(track->z > selectedSwitch->z && selectedSwitch->fromTrack == trackNum){
					highlight = true;
				}
			}
			else 
			{
				highlight = (i < 10 && i >= 0 && game->selectedTrack == 0) || 
							 (i < 20 && i >= 10 && game->selectedTrack == 1) ||
							 (i < 30 && i >= 20 && game->selectedTrack == 2);	
			}
			
			
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

	}
	
}

#endif

