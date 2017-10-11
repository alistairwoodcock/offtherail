#include "Grass.h"

namespace Grasses {

    void resetPos(Grass *p) {
		p->alpha = 0.8;
		
        p->x = (std::rand()%150)/10.0 + 6;
        if (std::rand()%2 == 0) p->x *= -1;
		p->y = game->ground;
		p->z = -150 - std::rand()%150;
		p->x_vel = 0.0;
		p->y_vel = 0.0;
		p->z_vel = game->speed;
        p->y_rot = std::rand()%5;
    }

    Model* randomModel() {
        switch(rand()%7) {
            case 0: return new Model("grass1", "models/rocks/rock1.obj", glm::vec3(0.2f));
            case 1: return new Model("grass2", "models/rocks/rock2.obj", glm::vec3(0.2f));
            case 2: return new Model("grass3", "models/rocks/rock3.obj", glm::vec3(0.2f));
            case 3: return new Model("grass4", "models/rocks/rock4.obj", glm::vec3(0.2f));
            case 4: return new Model("grass5", "models/grass/grass1.obj", glm::vec3(0.05f));
            case 5: return new Model("grass6", "models/grass/grass2.obj", glm::vec3(0.05f));
            case 6: return new Model("grass7", "models/grass/grass3.obj", glm::vec3(0.05f));
            case 7: return new Model("grass7", "models/grass/grass4.obj", glm::vec3(0.05f));
        }

        return new Model("grass7", "models/grass/grass4.obj", glm::vec3(0.05f));
    }

	//we have to take (void *state) because of the dependency 
	//structure of our includes. Requires casting when receiving
    void grassShaderSetup(void *state){
    	GameState *game = &((State*)state)->game_state;
    	
    	Shader grassShader = Shaders::get("grass");
		
		unsigned int ID = grassShader.ID;

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
    }

	void setup() {
        printf("SETUP GRASS\n");

		game->grass_count = 15;
        
		//setting a reload callback because we have shader vals set on startup
		//these will need to be reinitialised
		grassShaderSetup((void*)GlobalState);
		Shaders::reloadCallback("grass", *grassShaderSetup);

        int grass_count = game->grass_count;
        game->grass = (Grass*)malloc(grass_count*sizeof(Grass));

        for(int i = 0; i < grass_count; i++)
		{
			Grass *p = new Grass();
            p->model = randomModel();
            resetPos(p);
            p->alpha = 1.0;
            game->grass[i] = *p;
		}

        printf("SETUP GRASS - 6\n");
	}

	void update(float time, float deltaTime){
		
		int grass_count = game->grass_count;
		Grass *grass = game->grass;

		for(int i = 0; i < grass_count; i++)
		{
			Grass *p = grass+i;

			p->x += p->x_vel * deltaTime;
			p->y += p->y_vel * deltaTime;
			p->z += p->z_vel * deltaTime;
			p->alpha += 0.025 * deltaTime;

			if (p->z > 18) {
                resetPos(p);
			}
		}
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		
		Grass *grass = game->grass;
		int grass_count = game->grass_count;

		Shader grassShader = Shaders::get("grass");
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
		shaderSetFloat(ID, "material.shininess", 128.0f);

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
			
			p->model->Draw(grassShader);
        }
		
		glBindVertexArray(0);
		useShader(0);
	}

	void renderShadow(Shader &shader){
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
		    
		    p->model->Draw(shader);
        }

	}

}
