#include "SkyDome.h"
//INclude mesh for the vertex struct
//TODO Probs make the vertex struct a separate class
#include "../libs/mesh.h"

namespace SkyDomes {
	int numberOfVertices;
	Vertex* vertices;

	void setup(State *state) {
		GameState *game = &state->game_state;
		std::cout << "SetUP Sky" << std::endl;

		//Create Skydome Model

		//SkyDome Shaders here too probably

		/*game->skydome = new SkyDome();

		game->skydome->m_vBaseDayAmbient = glm::vec3(1, 1, 1);
		game->skydome->m_vBaseNightAmbient = glm::vec3(0, 0, 0);
		game->skydome->m_vBaseDayColor = glm::vec3(0, 0, 1);
		game->skydome->m_vBaseNightColor = glm::vec3(0, 0, 0);
		*/
		//game->trainModel = new Model("train", "models/train/locomotive/Locomotive C36.obj");
		//game->trainShader = loadShader("train", "src/shaders/train.vs", "src/shaders/train.fs");
		//game->train = new Train();


		//game->train->x = 0;
		//game->train->y = -2.0f;
		//game->train->z = 0.0f;

		//game->bogieFront = new Entity();
		//game->bogieFront->z = -1;
		//game->bogieBack = new Entity();
		//game->bogieBack->z = 9;
	}

	void Initialise() {
		std::cout << "Init" << std::endl;

		
	}


}


