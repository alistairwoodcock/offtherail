#include "Train.h"

namespace Trains {

	void updateScale() {
		glm::vec3 mSize = game->trainModel->size;
		glm::vec3 tSize = game->train->size;
		game->train->scale = glm::vec3(tSize.x/mSize.x, tSize.y/mSize.y, tSize.z/mSize.z);
	}

	void setupTrainModels(){
		game->trainModels[0] = new Model("locomotive", "models/train/locomotive/Locomotive C36.obj", glm::vec3(65.0f));
		game->trainModels[1] = new Model("taxi", "models/train/Taxi/Taxi.obj", glm::vec3(5.0f));
		game->trainModels[2] = new Model("train2", "models/rocks/rock1.obj", glm::vec3(0.3f));
	}

	void setTrainModel(TrainTypes type){
		if (type < 0 || type > TRAIN_MODEL_NUM) {
			type = DEFAULT;
		}
		game->trainModel = game->trainModels[type];
		game->currentTrain = type;
	}
   
	void reset() {
		// Size and scale for the train
		game->train->size = glm::vec3(1.0f);
		updateScale();

		// Train location itself
		game->train->x = 0;
		game->train->y = game->ground;
		game->train->z = 0.0f;

		// Bogie positions for the front and back of the train
		game->bogieFront->z = -1;
		game->bogieFront->currentTrack = 1;
		game->bogieBack->z = 9;
		game->bogieBack->currentTrack = 1;

		game->fallen = false;
	}

	void setup() {
		setupTrainModels();

		game->train = new Train();
		setTrainModel(DEFAULT);

		game->bogieFront = new Bogie();
		game->bogieBack = new Bogie();
		
		reset();
	}

	void update(float time, float deltaTime){
		
		Entity *train = game->train;
		Bogie *bogieFront = game->bogieFront;
		Bogie *bogieBack = game->bogieBack;
		
		Input input = platform->input;

		// if(input.a_pressed){
		//	bogieFront->x_vel = -5;
		// } else if(input.d_pressed) {
		//	bogieFront->x_vel = 5;
		// } else {
		//	bogieFront->x_vel = 0;
		// }

		// if(input.s_pressed){
		//	bogieFront->z_vel = 5;
		// } else if(input.w_pressed){
		//	bogieFront->z_vel = -5;
		// } else {
		//	bogieFront->z_vel = 0;
		// }

		float bogie_mid_x = (bogieBack->x + bogieFront->x)/2;
		float bogie_mid_z = (bogieBack->z + bogieFront->z)/2;

		float b_x_diff = bogieBack->x - bogieFront->x;
		float b_z_diff = bogieBack->z - bogieFront->z;

		if(bogieBack->x > bogie_mid_x + 1.5) bogieBack->x_vel = -2;
		if(bogieBack->x < bogie_mid_x - 1.5) bogieBack->x_vel = 2;
		
		if(bogieBack->z > bogie_mid_z + 1.5) bogieBack->z_vel = -2;
		if(bogieBack->z < bogie_mid_z - 1.5) bogieBack->z_vel = 2;

		// glm::vec3 v1(bogieBack->x, bogieBack->y, bogieBack->z);
		// glm::vec3 v2(bogieFront->x, bogieFront->y, bogieFront->z);
		// glm::vec3 dist_vec = glm::normalize(glm::vec3(v1 - v2));
		// dist_vec *= 7; //arbitrarily chosen distance between bogies
		// dist_vec += v2;

		// bogieBack->x = dist_vec.x;
		// bogieBack->y = dist_vec.y;
		// bogieBack->z = dist_vec.z;


		//Update bogie position
		// bogieFront->x += bogieFront->x_vel * deltaTime;
		// bogieFront->z += bogieFront->z_vel * deltaTime;

		// bogieBack->x += bogieBack->x_vel * deltaTime;
		// bogieBack->z += bogieBack->z_vel * deltaTime;

		// Track *t = Tracks::getSelectedTrack();
		// bogieFront->x = t->x;
		
		if(game->fallen) {
			train->z_rot = M_PI / 2;
			if(bogieFront->x > bogieBack->x){
				train->z_rot *= -1;	
			}
			
			if(game->speed > 0) game->speed -= 50 * deltaTime;
			if(game->speed < 0) game->speed = 0;
			return;
		}
		
		for(int i = 0; i < game->switchesCount; i++)
		{
			Switch *s = &game->switches[i];

			if(s->fromTrack == bogieFront->currentTrack)
			{
				if(s->z > bogieFront->z && abs(s->z - bogieFront->z) < 5){
					bogieFront->currentTrack = s->toTrack;
				}
			}

			if(s->fromTrack == bogieBack->currentTrack)
			{
				if(s->z > bogieBack->z && abs(s->z - bogieBack->z) < 5){
					bogieBack->currentTrack = s->toTrack;
				}
			}
		}

		bogieFront->x = Tracks::getTrack(bogieFront->currentTrack)->x;
		bogieBack->x = Tracks::getTrack(bogieBack->currentTrack)->x;


		b_x_diff = bogieBack->x - bogieFront->x;
		b_z_diff = bogieBack->z - bogieFront->z;

		if(b_z_diff == 0) b_z_diff = 0.00001;

		float bogie_dist = sqrt(b_z_diff*b_z_diff + b_x_diff*b_x_diff);

		float angle = 0;
		angle = atan(b_x_diff/b_z_diff);

		if(bogieBack->z > bogieFront->z) angle += M_PI;

		if(bogie_dist <= 12){
			train->y_rot = angle;
			train->x = (bogieBack->x + bogieFront->x)/2;
			train->z = (bogieBack->z + bogieFront->z)/2;
			train->z_rot = 0;
		} else {
			game->fallen = true;
		}

	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		
		Model *trainModel = game->trainModel;
		Train *train = game->train;

		Shader trainShader = Shaders::get("train");
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

		// Shader particleShader = Shaders::get("particle");
		// ID = particleShader.ID;

		// useShader(ID);
		// glBindVertexArray(game->Particle_VAO);

		// Entity *bogieFront = game->bogieFront;
		// model = glm::mat4();
		// model = glm::translate(model, glm::vec3(bogieFront->x, bogieFront->y, bogieFront->z));
		// model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
		// shaderSetMat4(ID, "model", model);
		// shaderSetVec3(ID, "color", glm::vec3(0,0.0,1.0));
		// shaderSetFloat(ID, "alpha", 0.5);
		
		// glDrawArrays(GL_TRIANGLES, 0, 36);


		// Entity *bogieBack = game->bogieBack;
		// model = glm::mat4();
		// model = glm::translate(model, glm::vec3(bogieBack->x, bogieBack->y, bogieBack->z));
		// model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
		// shaderSetMat4(ID, "model", model);
		// shaderSetVec3(ID, "color", glm::vec3(0,0.1,0.0));
		// shaderSetFloat(ID, "alpha", 0.5);

		// glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// glBindVertexArray(0);



	}

	void renderShadow(Shader &shader){
		
		Model *trainModel = game->trainModel;
		Train *train = game->train;

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
