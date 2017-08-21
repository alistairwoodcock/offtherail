
namespace Train {

	Model* trainModel;
	Shader* trainShader;

	Entity* train;
	
	void setup(State &state){
		trainShader = new Shader("src/shaders/train.vs", "src/shaders/train.fs");
		trainModel = new Model("models/train/locomotive/Locomotive C36.obj");

		train = new Entity();
		train->x = 0;
		train->y = -2.0f;
		train->z = 0.0f;
	}

	void update(State &state, float time, float deltaTime){

		train->y_rot += 10 * deltaTime;
	}

	void render(glm::mat4 &projection, glm::mat4 &view){
		
		trainShader->use();
		trainShader->setMat4("projection", projection);
		trainShader->setMat4("view", view);

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(train->x, train->y, train->z));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, glm::radians(train->y_rot), glm::vec3(0.0, 1.0, 0.0));
		trainShader->setMat4("model", model);
		trainModel->Draw(*trainShader);

	}

}