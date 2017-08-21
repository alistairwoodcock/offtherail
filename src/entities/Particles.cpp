

namespace Particles {

	class Particle: public Entity {
	public:
		float alpha;
	};

	float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	};

	GLuint VAO;
	GLuint VBO;

	Shader *shader;

	int particle_count = 1000;
	Particle* particles;

	glm::vec3 defaultColor(1,0.3,0.3);

	void setup(State &state){
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0*sizeof(float)));
		glEnableVertexAttribArray(0);

		shader = new Shader("src/shaders/particle.vs","src/shaders/particle.fs");
	
		particles = (Particle*)malloc(particle_count*sizeof(Particle));

		for(int i = 0; i < particle_count; i++)
		{
			Particle *p = new Particle();
			p->alpha = 1.0;

			p->x_vel = ((std::rand()%100) - (std::rand()%100))/1000.0;
			p->y_vel = ((std::rand()%100))/50.0;
			p->z_vel = ((std::rand()%100) - (std::rand()%100))/1000.0;
			
			particles[i] = *p;
		}
	}

	void update(State &state, float time, float deltaTime){
		
		for(int i = 0; i < particle_count; i++)
		{
			Particle *p = particles+i;

			p->x += p->x_vel * deltaTime;
			p->y += p->y_vel * deltaTime;
			p->z += p->z_vel * deltaTime;

			p->y_vel -= 0.98 * deltaTime;

			p->z_rot += (p->y_vel/abs(p->y_vel)) * 10 * deltaTime;

			p->alpha -= 0.8 * deltaTime;

			if(p->y < -2)
			{
				p->y = 1;
				p->x = 0;
				p->z = 4;
				p->x_vel = ((std::rand()%100) - (std::rand()%100))/300.0;
				p->y_vel = ((std::rand()%100))/50.0;
				p->z_vel = ((std::rand()%100) - (std::rand()%100))/300.0;

				if(p->y_vel >= 2) p->y_vel = 1;

				p->alpha = 0.8;
			}
		}
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		glBindVertexArray(VAO);

		shader->use();
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		for(int i = 0; i < particle_count; i++)
		{
			Particle *p = particles+i;
			
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(p->x,p->y,p->z));
			model = glm::scale(model, glm::vec3(0.02f));
			model = glm::rotate(model, glm::radians(p->z_rot), glm::vec3(0.0, 0.0, 1.0));
			
			shader->setMat4("model", model);
			shader->setVec3("color", defaultColor);
			shader->setFloat("alpha", p->alpha);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glBindVertexArray(0);
	}

}
