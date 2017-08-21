
#include "entities/Camera.cpp"
#include "entities/Particles.cpp"
#include "entities/Train.cpp"

namespace Game {

	//Game state object
	State state = {0};
	
	Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
	
	unsigned int screenWidth;
	unsigned int screenHeight;

	void setup(){

		state.game_started = false;

		Train::setup(state);
		Particles::setup(state);
	}

	void updateDimensions(unsigned int width, unsigned int height){
		screenWidth = width;
		screenHeight = height;
	}

	float time = 0;

	//gets called from the main loop
	void update(float deltaTime){
		time += deltaTime;
		
		//update camera based on state
		if(state.up_pressed) camera.ProcessKeyboard(FORWARD, deltaTime);
		if(state.down_pressed) camera.ProcessKeyboard(BACKWARD, deltaTime);
		if(state.left_pressed) camera.ProcessKeyboard(LEFT, deltaTime);
		if(state.right_pressed) camera.ProcessKeyboard(RIGHT, deltaTime);
		
		// view/projection transformations to pass to render functions
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		Train::update(state, time, deltaTime);
		Particles::update(state, time, deltaTime);
	

		Train::render(projection, view);
		Particles::render(projection, view);
	}

	void handleInput(GLFWwindow *window) {
		state.escape_pressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
		state.up_pressed = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
		state.down_pressed = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
		state.left_pressed = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
		state.right_pressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
	}

	glm::vec3 getBackgroundColor(){
		return glm::vec3(0,0,0);
	}

	bool shouldClose(){
		return state.escape_pressed;
	}
}
