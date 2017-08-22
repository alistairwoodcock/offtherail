namespace Game {
	void setup();
	void updateDimensions(unsigned int width, unsigned int height);
	void update(float deltaTime);
	void switchScreen(Screens newScreen);
	void handleInput(GLFWwindow *window) ;
	glm::vec3 getBackgroundColor();
	bool shouldClose();
}