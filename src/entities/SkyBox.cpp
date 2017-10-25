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

#include "SkyBox.h"
//INclude mesh for the vertex struct
//TODO Probs make the vertex struct a separate class
//#include "../libs/mesh.h"

#include <vector>
//#include "../libs/shader.h"

namespace SkyBoxes {
	unsigned int loadCubemap(std::vector<std::string> faces);
	void setup() {
		std::cout << "SetUP Sky" << std::endl;

		game->skybox = new SkyBox();

		float skyboxVertices[] = {
			// positions		  
			-1.0f,	1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,	1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,	1.0f, -1.0f,
			-1.0f,	1.0f, -1.0f,
			-1.0f,	1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,	1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,	1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,	1.0f,  1.0f,
			-1.0f,	1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};;

		
		glGenVertexArrays(1, &game->skybox->skyboxVAO);
		glGenBuffers(1, &game->skybox->skyboxVBO);
		glBindVertexArray(game->skybox->skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, game->skybox->skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		std::vector<std::string> faces
		{
			
			"images/skybox/right.jpg",
			"images/skybox/left.jpg",
			"images/skybox/top.jpg",
			"images/skybox/bottom.jpg",
			"images/skybox/back.jpg",
			"images/skybox/front.jpg"
		};

		game->skybox->cubemapTexture = loadCubemap(faces);
	}

	void render(glm::mat4 &projection, glm::mat4 &view) {
		// draw skybox as last
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		SkyBox* skybox = game->skybox;
		Shader skyboxShader = Shaders::get("skybox");
		unsigned int ID = skyboxShader.ID;
		useShader(ID);

		glm::mat4 tempView = glm::mat4(glm::mat3(view));
		shaderSetMat4(ID,"view", tempView);
		shaderSetMat4(ID, "projection", projection);
		
		// skybox cube		
		glBindVertexArray(skybox->skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
		glDepthMask(GL_TRUE);
	}

	// loads a cubemap texture from 6 individual texture faces
	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)
	// -------------------------------------------------------
	unsigned int loadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	void Initialise() {
		std::cout << "Init" << std::endl;

		
	}


}


