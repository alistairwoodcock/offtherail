
namespace Ground {

    void setup(State *state) {
    	GameState *game = &state->game_state;

        game->groundShader = loadShader("ground", "src/shaders/ground.vs", "src/shaders/ground.fs");

        float planeVertices[] = {
        // positions            // normals         // texcoords
        	60.0f, 0.0f,  40.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
        	-60.0f, 0.0f,  40.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        	-60.0f, 0.0f, -110.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,

        	60.0f, 0.0f,  40.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
        	-60.0f, 0.0f, -110.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,
        	60.0f, 0.0f, -110.0f,  0.0f, 1.0f, 0.0f,  50.0f, 50.0f
        };
    	
    	// plane VAO
        unsigned int planeVAO;
        unsigned int planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);

        game->Plane_VAO = planeVAO;
        game->Plane_VBO = planeVBO;

	}

	void update(State *state, float time, float deltaTime){

	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view, glm::mat4 &lightSpaceMatrix){
		GameState *game = &state->game_state;
		
		Shader shader = game->groundShader;
		unsigned int ID = shader.ID;

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0, game->ground, 0));

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);
		shaderSetMat4(ID, "model", model);
		shaderSetMat4(ID, "lightSpaceMatrix", lightSpaceMatrix);
		shaderSetVec3(ID, "color", glm::vec3(0.48,0.625,0.2656));

		shaderSetInt(ID, "diffuseTexture", 0);
		shaderSetInt(ID, "shadowMap", 1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, game->shadowDepthMap);

		glBindVertexArray(game->Plane_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
	}


}
