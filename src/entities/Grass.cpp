#include "Grass.h"

namespace Grasses {

    void resetPos(State *state, Grass *p) {
		p->alpha = 0.8;
		
        p->x = (std::rand()%200)/10.0 + 6;
        if (std::rand()%2 == 0) p->x *= -1;
		p->y = state->game_state.ground;
		p->z = -50 - std::rand()%50;
		p->x_vel = 0.0;
		p->y_vel = 0.0;
		p->z_vel = state->game_state.speed;
        p->y_rot = std::rand()%5;
    }

    Model* randomModel() {
        switch(rand()%4) {
            case 0: return new Model("grass1", "models/rocks/rock1.obj");
            case 1: return new Model("grass2", "models/rocks/rock2.obj");
            case 2: return new Model("grass3", "models/rocks/rock3.obj");
            case 3: return new Model("grass4", "models/rocks/rock4.obj");
        }
    }

	void setup(State *state) {
        printf("SETUP GRASS\n");

		GameState *game = &state->game_state;

		state->game_state.grass_count = 10;
        
		game->grassModel = randomModel();
		game->grassShader = loadShader("grass", "src/shaders/grass.vs", "src/shaders/grass.fs");
		unsigned int ID = game->grassShader.ID;
		useShader(ID);
		shaderSetVec3(ID, "objectColor", 0.4f, 0.4f, 0.4f);
		shaderSetVec3(ID, "lightColor", 1.0f, 1.0f, 1.0f);
		shaderSetVec3(ID, "viewPos", game->camera.Position);
		shaderSetVec3(ID, "material.ambient",  0.5f, 0.5f, 0.31f);
		shaderSetVec3(ID, "material.diffuse",  0.5f, 0.5f, 0.31f);
		shaderSetVec3(ID, "material.specular", 0.5f, 0.5f, 0.5f);
		shaderSetFloat(ID, "material.shininess", 5.0f); 

		glm::vec3 lightColor;
		lightColor.x = 1;
		lightColor.y = 1;
		lightColor.z = 1;

		glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5f); // low influence

		shaderSetVec3(ID, "light.ambient", ambientColor);
		shaderSetVec3(ID, "light.diffuse", diffuseColor);

		shaderSetVec3(ID, "light.position", game->sun->x, game->sun->y, game->sun->z); 
		shaderSetVec3(ID, "light.specular", 1.0f, 1.0f, 1.0f); 

        int grass_count = game->grass_count;
        game->grass = (Grass*)malloc(grass_count*sizeof(Grass));

		for(int i = 0; i < grass_count; i++)
		{
			Grass *p = new Grass();
            resetPos(state, p);
            p->alpha = 1.0;
            p->z += 50 - std::rand()%100; // Initial offset
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
			p->alpha += 0.025 * deltaTime;

			if (p->z > 18) {
                resetPos(state, p);
			}
		}
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){
		GameState *game = &state->game_state;
        Model *grassModel = game->grassModel;
		Grass *grass = game->grass;
		int grass_count = game->grass_count;

		Shader grassShader = game->grassShader;
		unsigned int ID = grassShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		shaderSetVec3(ID, "objectColor", 1.0f, 1.0f, 1.0f);
		shaderSetVec3(ID, "lightColor", 1.0f, 0.0f, 0.0f);
		shaderSetVec3(ID, "viewPos", game->camera.Position);
		shaderSetVec3(ID, "material.ambient",  1.0f, 1.0f, 1.0f);
		shaderSetVec3(ID, "material.diffuse",  0.5f, 0.5f, 0.31f);
		shaderSetVec3(ID, "material.specular", 0.25f, 0.25f, 0.25f);
		shaderSetFloat(ID, "material.shininess", 64.0f); 

		for(int i = 0; i < grass_count; i++)
		{

			Grass *p = grass+i;
		    
            // render the loaded model
		    glm::mat4 model;
			model = glm::translate(model, glm::vec3(p->x,p->y,p->z));
		    model = glm::scale(model, glm::vec3(0.8));

		    model = glm::rotate(model, p->y_rot, glm::vec3(0.0, 1.0, 0.0));
		    shaderSetMat4(ID, "model", model);
		    shaderSetMat4(ID, "inverseModel", glm::inverse(model));
			
			grassModel->Draw(grassShader);
        }
		
		glBindVertexArray(0);
		useShader(0);
	}

	void renderShadow(State *state, Shader &shader){
		GameState *game = &state->game_state;
        Model *grassModel = game->grassModel;
		Grass *grass = game->grass;
		int grass_count = game->grass_count;

		unsigned int ID = shader.ID;

		for(int i = 0; i < grass_count; i++)
		{

			Grass *p = grass+i;
		    
            // render the loaded model
		    glm::mat4 model;
			model = glm::translate(model, glm::vec3(p->x,p->y,p->z));
		    model = glm::scale(model, glm::vec3(0.8));

		    model = glm::rotate(model, p->y_rot, glm::vec3(0.0, 1.0, 0.0));
		    shaderSetMat4(ID, "model", model);
		    
		    grassModel->Draw(shader);
        }

	}

}
