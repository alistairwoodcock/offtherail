/* 
 * Course: COMP3320
 * Project: Off The Rail
 * Members: 
 *	Alistair Woodcock	(c3138738)
 *	Lachlan Meyer		(c3203676)
 *	Joshua Crompton		(c3165877)
 *	Scott Walker		(c3232582)
 *	Timothy Pitts		(c3220826)
 *
 */

#include "Particles.h"

namespace Particles {

	void setup(){
		printf("SETUP PARTICLES\n");
		float particle_vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,	0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,	0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,	0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		};

		GLuint VAO;
		GLuint VBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0*sizeof(float)));
		glEnableVertexAttribArray(0);

		game->Particle_VAO = VAO;
		game->Particle_VBO = VBO;

		game->particle_count = 5000;

		int particle_count = game->particle_count;
		
		game->particles = (Particle*)malloc(particle_count*sizeof(Particle));


		for(int i = 0; i < particle_count; i++)
		{
			Particle *p = new Particle();
			p->alpha = 1.0;

			p->x_vel = ((std::rand()%100) - (std::rand()%100))/1000.0;
			p->y_vel = ((std::rand()%100))/50.0;
			p->z_vel = game->speed;//((std::rand()%100) - (std::rand()%100))/1000.0;
			p->render = false;
			
			auto color = glm::vec3(0.95,0.9,0.5);
			float r = std::rand()%2;
			if(r != 0) color = glm::vec3(0.95,0.4,0.4);
			
			p->r = color.r;
			p->g = color.g;
			p->b = color.b;
			
			game->particles[i] = *p;


		}


	}

	void reset(Particle *p, bool sparkTime) {
		float r = std::rand()%100/12.3f;
		// printf("%f\n", r);
		p->y = -1.7;
		p->x = game->bogieBack->x + (std::rand()%2 == 0 ? 0.7 : -0.7);
		p->z = 8;

		p->x_vel = ((std::rand()%100) - (std::rand()%100))/73.2f;
		p->y_vel = 0 + std::rand()%100/103.2;
		p->z_vel = game->speed/16.2f + r;//((std::rand()%100) - (std::rand()%100))/300.0;
		p->scale = r/81.3f;
		if(p->y_vel >= 2) p->y_vel = 1;

		p->alpha = 0.5 + r/50.0f;
				
		p->render = sparkTime;

		auto color = glm::vec3(0.95,0.9,0.5);
		r = std::rand()%2;
		if(r != 0) color = glm::vec3(0.95,0.4,0.4);
		
		p->r = color.r;
		p->g = color.g;
		p->b = color.b;
	}

	void update(float time, float deltaTime){
		int particle_count = game->particle_count;
		Particle *particles = game->particles;

		bool sparkTime = (game->bogieBack->currentTrack != game->bogieFront->currentTrack);
		sparkTime = sparkTime && !game->fallen;

		for(int i = 0; i < particle_count; i++)
		{	
			Particle *p = particles+i;

			if(!p->render && !sparkTime) continue;
			
			p->x += p->x_vel * deltaTime;
			p->y += p->y_vel * deltaTime;
			p->z += p->z_vel * deltaTime;

			p->y_vel += 0.98 * deltaTime;

			p->z_rot += (std::rand()%100/123.4f) * deltaTime;
			
			static float sad = 0;
			sad += 0.01;
			p->x_rot = 0.01;

			p->alpha -= 0.1 * deltaTime;

			if(p->y < -2 || p->z > 30) {
				reset(p, sparkTime);
			}
		}
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		Particle *particles = game->particles;
		int particle_count = game->particle_count;

		glBindVertexArray(game->Particle_VAO);

		Shader shader = Shaders::get("particle");
		unsigned int ID = shader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);


		for(int i = 0; i < particle_count; i++)
		{
			Particle *p = particles+i;
			
			if(!p->render) continue;


			glm::mat4 model;
			model = glm::translate(model, glm::vec3(p->x,p->y,p->z));
			model = glm::scale(model, glm::vec3(p->scale));
			
			shaderSetMat4(ID, "model", model);
			shaderSetFloat(ID, "alpha", p->alpha);
			shaderSetVec3(ID, "color", glm::vec3(p->r,p->g,p->b));

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glBindVertexArray(0);
	}

}
