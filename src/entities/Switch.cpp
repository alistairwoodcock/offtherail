#include "Switch.h"
#include "../libs/mesh.h"

namespace Switches {
	void generateCurveSegments(bool left, Vertex p0, Vertex p1, Vertex p2, Vertex p3);
	Vertex getTangentAt(bool left, unsigned int curveIndex, GLfloat t);
	glm::mat4 getModelMatrix(Vertex direction, glm::vec3 position);
	void bufferData(bool left);

	const float DIST_BETWEEN_TRACKS = 4;
	const float SWITCH_LENGTH = 20;
	const float CP_LENGTH = SWITCH_LENGTH / 2;

	void setup() {
		printf("SETTING UP SWITCH\n");
		
		game->switchesCount = 0;
		game->maxSwitches = 20;
		game->selectedTrack = 0;

		//Load the Track Model and setup the vertex buffer
		game->leftSwitchModel = new Model("switch", "models/track/singlerailtexturedJoined.obj");
		game->rightSwitchModel = new Model("switch", "models/track/singlerailtexturedJoined.obj");
		
		float XDIFF = -DIST_BETWEEN_TRACKS;
		
		float x = 0;
		float z = 0;

		Vertex p0 = Vertex({ x, 0, z });
		Vertex p1 = Vertex({ x, 0, z - CP_LENGTH });
		Vertex p2 = Vertex({ x + XDIFF, 0, z - CP_LENGTH });
		Vertex p3 = Vertex({ x + XDIFF, 0, z - SWITCH_LENGTH });

		generateCurveSegments(true, p0, p1, p2, p3);
		
		XDIFF = -XDIFF;
		p0 = Vertex({ x, 0, z });
		p1 = Vertex({ x, 0, z - CP_LENGTH });
		p2 = Vertex({ x + XDIFF, 0, z - CP_LENGTH });
		p3 = Vertex({ x + XDIFF, 0, z - SWITCH_LENGTH });
	
		generateCurveSegments(false, p0, p1, p2, p3);


		
	}

	void toggleSwitch(int i){
		Switch *s = &game->switches[i];

		int from = s->fromTrack;
		int to = s->toTrack;

		switch(from){
			case 0: {
				if(to == 0) to = 1;
				else to = 0;
			} break;

			case 1: {
				if(s->left)
				{
					if(to == 1) to == 0;
					else to = 1;
				}
				else
				{
					if(to == 1) to = 2;
					else to = 1;
				}
			} break;

			case 2: {
				if(to == 2) to = 1;
				else to = 0;
			} break;
		}

		s->toTrack = to;
	}

	void update(float time, float deltaTime){
		//update switches and set the current one
		int closestSwitch = -1;

		float deltaPos = game->speed * deltaTime;

		for(int i = 0; i < game->switchesCount; i++){
			Switch *s = &game->switches[i];

			s->z += deltaPos;

			if(s->z > 400){
				for(int f = 0; f < game->switchesCount; f++){
					game->switches[f] = game->switches[f+1];
				}
				game->switchesCount--;
			}

			bool onSelectedTrack = s->fromTrack == game->selectedTrack;
			
			if(onSelectedTrack && s->z < 10)
			{
				if(closestSwitch == -1) closestSwitch = i;
				
				if(s->z > game->switches[closestSwitch].z) closestSwitch = i;
			}
		}

		game->selectedSwitch = closestSwitch;
		
		Input input = platform->input;

		if(input.space_pressed)
		{
			toggleSwitch(game->selectedSwitch);
			game->input_timeout = 0.1;
		}
	}


	void render(glm::mat4 &projection, glm::mat4 &view)
	{	

		Shader switchShader = Shaders::get("switch");
		unsigned int ID = switchShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		for(int i = 0; i < game->switchesCount; i++){
			Model* switchModel;
			unsigned int trackPieceTransformsSize;

			Switch *s = &game->switches[i];
			
			if(s->left){
				switchModel = game->leftSwitchModel;
				trackPieceTransformsSize = game->leftTrackPieceTransforms.size();
			} else {
				switchModel = game->rightSwitchModel;
				trackPieceTransformsSize = game->rightTrackPieceTransforms.size();
			}


			if(i == game->selectedSwitch){
				shaderSetVec3(ID, "colour", glm::vec3(0.5f, 1.0f, 1.0f));
			} else {
				shaderSetVec3(ID, "colour", glm::vec3(0.0f, 0.0f, 0.0f));
			}

			glm::mat4 model;
			model = glm::translate(model, glm::vec3(s->x, s->y, s->z));
			shaderSetMat4(ID, "model", model);
			
			//Loop through each mesh of the model and draw an instance of the track	
			for (unsigned int i = 0; i < switchModel->meshes.size(); i++)
			{
				//Bind the meshes texture
				//trackShader->setInt("texture_diffuse1", 0);
				shaderSetInt(ID, "texture_diffuse1", 0);
				// shaderSetFloat(ID, "zOffset", s->z);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, switchModel->meshes[i].textures[0].id);

				//Bind the vertex array
				glBindVertexArray(switchModel->meshes[i].VAO);
				glDrawElementsInstanced(GL_TRIANGLES, switchModel->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, trackPieceTransformsSize);
				glBindVertexArray(0);
			}
			
		}

	}

	
	void addSwitch(float x, float y, float z, int from, int to){
		if(game->switchesCount == game->maxSwitches) return;

		bool left = false;
		if(from-to > 0) left = true;

		int i = game->switchesCount;

		Switch *s = &game->switches[i];

		s->x = x;
		s->y = y;
		s->z = z;
		s->fromTrack = from;
		s->toTrack = to;
		s->left = left;

		game->switchesCount++;
	}

	int numberOfSegments = 25;

	void generateCurveSegments(bool left, Vertex p0, Vertex p1, Vertex p2, Vertex p3)
	{

		vector<vector<GLfloat>> controlPoints;
		vector<glm::mat4> trackPieceTransforms;

		//Add the vertices to the control points vector
		controlPoints.push_back({ p0.Position.x, p0.Position.y, p0.Position.z });
		controlPoints.push_back({ p1.Position.x, p1.Position.y, p1.Position.z });
		controlPoints.push_back({ p2.Position.x, p2.Position.y, p2.Position.z });
		controlPoints.push_back({ p3.Position.x, p3.Position.y, p3.Position.z });

		if(left){
			game->leftControlPoints = controlPoints;
		} else {
			game->rightControlPoints = controlPoints;
		}

		//Using the constants in the header create the Control points.
		std::cout << "Generating Curve Segments" <<std::endl;
		vector<float> subPointsVector;
		for (int i = 0; i != numberOfSegments; ++i) {

			// use the parametric time value 0 to 1
			float t = (float)i / (numberOfSegments - 1);

			// nice to pre-calculate 1.0f-t because we will need it frequently
			float it = 1.0f - t;

			// calculate blending functions
			float b0 = t*t*t;
			float b1 = 3 * t*t*it;
			float b2 = 3 * t*it*it;
			float b3 = it*it*it;

			// calculate the x,y and z of the curve point by summing
			// the Control vertices weighted by their respective blending
			// functions
			float x = b3*p0.Position.x +
				b2*p1.Position.x +
				b1*p2.Position.x +
				b0*p3.Position.x;

			float y = b3*p0.Position.y +
				b2*p1.Position.y +
				b1*p2.Position.y +
				b0*p3.Position.y;

			float z = b3*p0.Position.z +
				b2*p1.Position.z +
				b1*p2.Position.z +
				b0*p3.Position.z;

			subPointsVector.push_back(x);
			subPointsVector.push_back(y);
			subPointsVector.push_back(z);

			//Get the tangent at this point
			Vertex tangent = getTangentAt(left, 0, t);
			//Normalize the tangent here to speed things up later
			tangent.Position = glm::normalize(glm::vec4(tangent.Position, 1));

			//Form Transofrm matrix here and put it into the vector
			glm::vec3 position = glm::vec3(x, y, z);
			trackPieceTransforms.push_back(getModelMatrix(tangent, position));
		}

		if(left){
			game->leftTrackPieceTransforms = trackPieceTransforms;
		} else {
			game->rightTrackPieceTransforms = trackPieceTransforms;
		}

		//Buffer the data
		bufferData(left);
	}

	void bufferData(bool left){

		if(left)
		{
			/* -- DO ONE FOR LEFT -- */

			std::cout << "Buffering Data" << game->leftTrackPieceTransforms.size() << std::endl;
			//Generate and bind a new buffer
			glGenBuffers(1, &game->leftSwitchVBO);
			glBindBuffer(GL_ARRAY_BUFFER, game->leftSwitchVBO);
			glBufferData(GL_ARRAY_BUFFER, game->leftTrackPieceTransforms.size() * sizeof(glm::mat4), &game->leftTrackPieceTransforms[0], GL_STATIC_DRAW);

			std::cout << "Loop Over Meshes" << std::endl;
			for (unsigned int i = 0; i < game->leftSwitchModel->meshes.size(); i++)
			{
				unsigned int VAO = game->leftSwitchModel->meshes[i].VAO;
				glBindVertexArray(VAO);
				// set attribute pointers for matrix (4 times vec4)
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

				glVertexAttribDivisor(3, 1);
				glVertexAttribDivisor(4, 1);
				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);

				glBindVertexArray(0);
			}
			std::cout << "Buffered Data" << std::endl;

		}
		else
		{
			/* -- DO ONE FOR RIGHT -- */

			std::cout << "Buffering Data" << game->rightTrackPieceTransforms.size() << std::endl;
			//Generate and bind a new buffer
			glGenBuffers(1, &game->rightSwitchVBO);
			glBindBuffer(GL_ARRAY_BUFFER, game->rightSwitchVBO);
			glBufferData(GL_ARRAY_BUFFER, game->rightTrackPieceTransforms.size() * sizeof(glm::mat4), &game->rightTrackPieceTransforms[0], GL_STATIC_DRAW);

			std::cout << "Loop Over Meshes" << std::endl;
			for (unsigned int i = 0; i < game->rightSwitchModel->meshes.size(); i++)
			{
				unsigned int VAO = game->rightSwitchModel->meshes[i].VAO;
				glBindVertexArray(VAO);
				// set attribute pointers for matrix (4 times vec4)
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

				glVertexAttribDivisor(3, 1);
				glVertexAttribDivisor(4, 1);
				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);

				glBindVertexArray(0);
			}
			std::cout << "Buffered Data" << std::endl;
		}

	}
	Vertex getTangentAt(bool left, unsigned int curveIndex, GLfloat t)
	{
		if (t < 0 || t > 1) {
			//Maybe they forgot to convert?
			//curveIndex = timeToCurveIndex(t);
			//if (curveIndex < 0 || curveIndex > 1) {
				//Nope they're idiots, Invalid t, Exception?
				//Return empty Vertex for now, so it doesn't crash the program
			//	cout << "Error finding point at t with t = " << t << endl;
			//	return Vertex();
			//}
		}
		// nice to pre-calculate 1.0f-t because we will need it frequently
		float it = 1.0f - t;

		// calculate blending functions of First derivative of bezier curve function
		// -3(1-t)^2 P0 + (3(1-t)^2 - 6t(1-t)) P1 + (6t(1-t) - 3t^2) P2 + 3t^2 P3
		float b0 = 3 * t * t;
		float b1 = (6 * t * it) - (3 * t * t);
		float b2 = (3 * it * it) - (6 * t * it);
		float b3 = -3 * it * it;

		unsigned int index = 0;  //curveIndex * 4;
		vector<float> p0, p1, p2, p3;
		
		if(left){
		 	p0 = game->leftControlPoints[index];
			p1 = game->leftControlPoints[index + 1];
			p2 = game->leftControlPoints[index + 2];
			p3 = game->leftControlPoints[index + 3];
		} else {
		 	p0 = game->rightControlPoints[index];
			p1 = game->rightControlPoints[index + 1];
			p2 = game->rightControlPoints[index + 2];
			p3 = game->rightControlPoints[index + 3];
		}

		// calculate the x,y and z of the curve point by summing
		// the Control vertices weighted by their respective blending
		// functions
		float x = b3*p0[0] +
			b2*p1[0] +
			b1*p2[0] +
			b0*p3[0];

		//This should always be zero
		float y = b3*p0[1] +
			b2*p1[1] +
			b1*p2[1] +
			b0*p3[1];

		float z = b3*p0[2] +
			b2*p1[2] +
			b1*p2[2] +
			b0*p3[2];

		Vertex v;
		v.Position = glm::vec3(x, y, z);
		return v;
	}

	glm::mat4 getModelMatrix(Vertex direction, glm::vec3 position)
	{
		glm::mat4 model;
		glm::vec3 start = glm::normalize(glm::vec3(0, 0, -1));
		glm::vec3 dest = glm::normalize(glm::vec3(direction.Position));

		float cosTheta = glm::dot(start, dest);
		float theta = glm::acos(cosTheta);
		glm::vec3 rotationAxis = glm::cross(start, dest);

		model = glm::translate(model, position);

		model = glm::scale(model, glm::vec3(0.26f, 0.26f, 0.26f));

		model = glm::rotate(model, theta, rotationAxis);

		return model;
	}
}