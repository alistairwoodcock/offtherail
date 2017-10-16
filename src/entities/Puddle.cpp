#include "Puddle.h"

namespace Puddles {

    void resetPos(Puddle *p) {
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
        switch(rand()%4) {
            case 0: return new Model("grass1", "models/rocks/rock1.obj", glm::vec3(0.2f));
            case 1: return new Model("grass2", "models/rocks/rock2.obj", glm::vec3(0.2f));
            case 2: return new Model("grass3", "models/rocks/rock3.obj", glm::vec3(0.2f));
            case 3: return new Model("grass4", "models/rocks/rock4.obj", glm::vec3(0.2f));
        }
    }

	void setup() {
        printf("SETUP PUDDLE\n");

		game->puddle_count = 20;
        
        int puddle_count = game->puddle_count;
        game->puddle = (Puddle*)malloc(puddle_count*sizeof(Puddle));

        for(int i = 0; i < puddle_count; i++)
		{
			Puddle *p = new Puddle();
            p->model = randomModel();
            resetPos(p);
            p->alpha = 1.0;
            game->puddle[i] = *p;
		}
	}

	void update(float time, float deltaTime){
		
		int puddle_count = game->puddle_count;
		Puddle *puddle = game->puddle;

		for(int i = 0; i < puddle_count; i++)
		{
			Puddle *p = puddle+i;
			p->z_vel = game->speed;

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
		
		Puddle *puddle = game->puddle;
		int puddle_count = game->puddle_count;

		Shader trackShader = Shaders::get("track");
		unsigned int ID = trackShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);
		shaderSetVec3(ID, "colour", glm::vec3(0.7f, 0.7f, 0.95f));

        // ENABLE STENCIL
		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);

		glDepthMask(GL_FALSE);
		glClear(GL_STENCIL_BUFFER_BIT);

        for(int i = 0; i < puddle_count; i++) {

			Puddle *p = puddle+i;
		    
            // render the loaded model
		    glm::mat4 model;
			model = glm::translate(model, glm::vec3(p->x,p->y,p->z));
		    model = glm::scale(model, glm::vec3(10.0, 0.01, 10.0));

		    model = glm::rotate(model, p->y_rot, glm::vec3(0.0, 1.0, 0.0));
		    shaderSetMat4(ID, "model", model);
			
			p->model->Draw(trackShader);
        }


	    glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);

        // Draw reflected grasses
        Grasses::renderReflections(projection, view);
		
		glDepthMask(GL_TRUE);
		glDisable(GL_STENCIL_TEST);
		
        glBindVertexArray(0);
		useShader(0);
	}

}
