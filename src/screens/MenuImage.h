#ifndef MENU_IMAGE
#define MENU_IMAGE

class MenuImage: public Entity {
public:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int glTexture;

	int width;
	int height;
	int nChannels;

	glm::vec3 scale;
	float scale_vel;
};

#endif