#include "Grass.h"

namespace Grasses {

    void resetPos(Grass *p) {
		p->alpha = 1.0;
		
        p->x = (std::rand()%250)/10.0 + 5;
        if (std::rand()%2 == 0) p->x *= -1;
		p->y = -2.0;
		p->z = -50 - std::rand()%50;
		p->x_vel = 0.0;
		p->y_vel = 0.0;
		p->z_vel = 100.0;
    }

	void setup(State *state) {

        printf("SETUP GRASS\n");
		GameState *game = &state->game_state;

		//game->grassModel = new Model("grass", "models/teapot/teapot.obj");
		game->grassModel = new Model("grass", "models/train/locomotive/Locomotive C36.obj");
		game->grassShader = loadShader("grass", "src/shaders/grass.vs", "src/shaders/grass.fs");
        
        int grass_count = game->grass_count;
        game->grass = (Grass*)malloc(grass_count*sizeof(Grass));

		for(int i = 0; i < grass_count; i++)
		{
			Grass *p = new Grass();
            resetPos(p);
            p->z -= std::rand()%100; // Initial offset
			game->grass[i] = *p;
		}

	}

	void update(State *state, float time, float deltaTime){
		GameState *game  = &state->game_state;
		int grass_count = state->game_state.grass_count;
		Grass *grass = state->game_state.grass;

		for(int i = 0; i < grass_count; i++)
		{
			Grass *p = grass+i;

			p->x += p->x_vel * deltaTime;
			p->y += p->y_vel * deltaTime;
			p->z += p->z_vel * deltaTime;

            //printf("x%f y%f z%f\n", p->x, p->y, p->z);

			p->alpha -= 0.025 * deltaTime;

			if (p->z > 18 || p->alpha < 0.05) {
                resetPos(p);
			}
		}
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){

        Model *grassModel = state->game_state.grassModel;
		Grass *grass = state->game_state.grass;
		int grass_count = state->game_state.grass_count;

		Shader grassShader = state->game_state.grassShader;
		unsigned int ID = grassShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		for(int i = 0; i < grass_count; i++)
		{

			Grass *p = grass+i;
		    
            // render the loaded model
		    glm::mat4 model;
			model = glm::translate(model, glm::vec3(p->x,p->y,p->z));
		    model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.00f));

		    //model = glm::rotate(model, p->y_rot, glm::vec3(0.0, 1.0, 0.0));
		    shaderSetMat4(ID, "model", model);
			shaderSetVec3(ID, "color", glm::vec3(1.0, std::rand()%100/100.0, 0.5));
			shaderSetFloat(ID, "alpha", p->alpha);
		    grassModel->Draw(grassShader);

    		Shader particleShader = state->game_state.particleShader;
    		ID = particleShader.ID;

    		useShader(ID);
    		glBindVertexArray(state->game_state.Particle_VAO);
        }
	}

}
