#include "Particles.h"

namespace Particles {

	void setup(State *state){
		printf("SETUP PARTICLES\n");
		int particle_count = state->game_state.particle_count;
		
		state->game_state.particles = (Particle*)malloc(particle_count*sizeof(Particle));


		for(int i = 0; i < particle_count; i++)
		{
			Particle *p = new Particle();
			p->alpha = 1.0;

			p->x_vel = ((std::rand()%100) - (std::rand()%100))/1000.0;
			p->y_vel = ((std::rand()%100))/50.0;
			p->z_vel = ((std::rand()%100) - (std::rand()%100))/1000.0;
			
			state->game_state.particles[i] = *p;
		}


	}

	void update(State *state, float time, float deltaTime){
		GameState *game  = &state->game_state;
		int particle_count = state->game_state.particle_count;
		Particle *particles = state->game_state.particles;

		for(int i = 0; i < particle_count; i++)
		{
			Particle *p = particles+i;

			p->x += p->x_vel * deltaTime;
			p->y += p->y_vel * deltaTime;
			p->z += p->z_vel * deltaTime;

			p->y_vel -= 0.98 * deltaTime;

			p->z_rot += (p->y_vel/std::abs(p->y_vel)) * 10 * deltaTime;

			p->alpha -= 0.6 * deltaTime;

			if(p->y < -2)
			{
				Entity *train = game->train;
				p->y = -2;
				p->x = train->x;
				p->z = 0;
				p->x_vel = ((std::rand()%100) - (std::rand()%100))/300.0;
				p->y_vel = ((std::rand()%100))/50.0;
				p->z_vel = ((std::rand()%100) - (std::rand()%100))/300.0;

				if(p->y_vel >= 2) p->y_vel = 1;

				p->alpha = 0.8;
			}
		}
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){
		Particle *particles = state->game_state.particles;
		int particle_count = state->game_state.particle_count;

		glBindVertexArray(state->game_state.Particle_VAO);

		Shader shader = state->game_state.particleShader;
		unsigned int ID = shader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		for(int i = 0; i < particle_count; i++)
		{

			Particle *p = particles+i;

			glm::mat4 model;
			model = glm::translate(model, glm::vec3(p->x,p->y,p->z));
			model = glm::scale(model, glm::vec3(0.02f));
			model = glm::rotate(model, glm::radians(p->z_rot), glm::vec3(0.0, 0.0, 1.0));
			
			shaderSetMat4(ID, "model", model);
			shaderSetVec3(ID, "color", glm::vec3(1,0.0,0.0));
			shaderSetFloat(ID, "alpha", p->alpha);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glBindVertexArray(0);
	}

}
