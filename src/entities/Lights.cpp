namespace Lights {
	
	void setup(State *state){
		GameState *game = &state->game_state;

		game->sun = new Entity();
		game->sun->x = 20;
		game->sun->y = 13;
		game->sun->z = 0;
		game->sun->z_rot = 0;


		game->lightShader = loadShader("light", "src/shaders/particle.vs","src/shaders/particle.fs");
	}

	void update(State *state, float time, float deltaTime){
		GameState *game = &state->game_state;

		game->sun->z_rot += 1 * deltaTime;
		game->sun->x = 15;
		game->sun->z = -40;
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view){
		glBindVertexArray(state->game_state.Particle_VAO);
		
		GameState *game = &state->game_state;

		Shader shader = state->game_state.lightShader;
		unsigned int ID = shader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(game->sun->x,game->sun->y,game->sun->z));
		model = glm::scale(model, glm::vec3(2.0f));
		shaderSetVec3(ID, "color", glm::vec3(0.97,0.97,1.0));
		shaderSetFloat(ID, "alpha", 1.0f);

		for(float i = 0; i < 1; i+= 0.3){
			model = glm::rotate(model, glm::radians(game->sun->z_rot + i), glm::vec3(0.0, 0.0, 1.0));
				
			shaderSetMat4(ID, "model", model);
			

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);

	}

}