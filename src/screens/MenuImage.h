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
