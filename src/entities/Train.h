#ifndef TRAIN_H
#define TRAIN_H

#include <glm/glm.hpp>
#include "Entity.h"

const unsigned int TRAIN_MODEL_NUM = 3;

enum TrainTypes {
	DEFAULT,
	ROCK,
	OUR_BOY_THOMAS
};


class Train: public Entity {
public:
    glm::vec3 size;
    glm::vec3 scale;
};

#endif
