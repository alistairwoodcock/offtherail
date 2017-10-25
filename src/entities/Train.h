#ifndef TRAIN_H
#define TRAIN_H

#include <glm/glm.hpp>
#include "Entity.h"

const unsigned int TRAIN_MODEL_NUM = 2;

enum TrainTypes {
	DEFAULT = 0,
	TAXI
};


class Train: public Entity {
public:
	glm::vec3 size;
	glm::vec3 scale;
	glm::vec3 offset;
};

class Bogie: public Entity {
public:
	int currentTrack;
};

#endif
