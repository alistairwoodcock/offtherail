#include "Switch.h"
#include "../libs/mesh.h"

namespace Switches {
	void generateCurveSegments(Vertex p0, Vertex p1, Vertex p2, Vertex p3);
	void loadModel();
	Vertex getTangentAt(unsigned int curveIndex, GLfloat t);
	glm::mat4 getModelMatrix(Vertex direction, glm::vec3 position);
	void bufferData(GameState* game);

	float START_X = 0.0;
	float START_Z = 0.0;
	const float DIST_BETWEEN_TRACKS = 3.4;
	const float SWITCH_LENGTH = 20;
	const float CP_LENGTH = SWITCH_LENGTH / 2;
	int numberOfSegments = 25;

	void setup() {
		
		std::cout << "Setting Up Switch" << std::endl;
		game->curvedSwitches = new Switch();
		loadModel();
		
		float XDIFF = DIST_BETWEEN_TRACKS;
		
		bool left;		//Is the track switching to the left?
		if (left) {
			XDIFF = -XDIFF;
		}

		Vertex p0 = Vertex({ START_X, 0, START_Z });
		Vertex p1 = Vertex({ START_X, 0, START_Z - CP_LENGTH });
		Vertex p2 = Vertex({ START_X + XDIFF, 0, START_Z - CP_LENGTH });
		Vertex p3 = Vertex({ START_X + XDIFF, 0, START_Z - SWITCH_LENGTH });

		generateCurveSegments(p0, p1, p2, p3);

	}
	void loadModel()
	{
		game->switchModel = new Model("switch", "models/track/singlerailtexturedJoined.obj");

		glGenBuffers(1, &game->curvedSwitches->switchVBO);
		glBindBuffer(GL_ARRAY_BUFFER, game->curvedSwitches->switchVBO);

		for (unsigned int i = 0; i < game->trackModel->meshes.size(); i++)
		{
			unsigned int VAO = game->switchModel->meshes[i].VAO;
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
	}


	void update(float time, float deltaTime) {

		//update tracks and also get the furthest behind
		int lastTrack = 0;
		Switch* s = game->curvedSwitches;
		float deltaPos = game->speed * deltaTime;
		//s->z += deltaPos;

		//Update the control points so the getPointAt method still works.
		game->curvedSwitches->controlPoints[0][2] += deltaPos;
		game->curvedSwitches->controlPoints[1][2] += deltaPos;
		game->curvedSwitches->controlPoints[2][2] += deltaPos;
		game->curvedSwitches->controlPoints[3][2] += deltaPos;
		
	}

	void render(glm::mat4 &projection, glm::mat4 &view)
	{
		
		std::cout << "Render Switch" << game->curvedSwitches->trackPieceTransforms.size() << std::endl;
		Model* switchModel = game->switchModel;
		Switch* s = game->curvedSwitches;

		Shader switchShader = Shaders::get("switch");
		unsigned int ID = switchShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		//Create model matrix to translate the switch to the correct position
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(s->x, s->y, s->z));
		shaderSetMat4(ID, "model", model);

		//Loop through each mesh of the model and draw an instance of the track	
		for (unsigned int i = 0; i < switchModel->meshes.size(); i++)
		{
			//Bind the meshes texture
			//trackShader->setInt("texture_diffuse1", 0);
			shaderSetInt(ID, "texture_diffuse1", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, switchModel->meshes[i].textures[0].id);

			//Bind the vertex array
			glBindVertexArray(switchModel->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, switchModel->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, game->curvedSwitches->trackPieceTransforms.size());
			glBindVertexArray(0);
		}
		

	}

	void generateCurveSegments(Vertex p0, Vertex p1, Vertex p2, Vertex p3)
	{
		//Add the vertices to the control points vector
		game->curvedSwitches->controlPoints.push_back({ p0.Position.x, p0.Position.y, p0.Position.z });
		game->curvedSwitches->controlPoints.push_back({ p1.Position.x, p1.Position.y, p1.Position.z });
		game->curvedSwitches->controlPoints.push_back({ p2.Position.x, p2.Position.y, p2.Position.z });
		game->curvedSwitches->controlPoints.push_back({ p3.Position.x, p3.Position.y, p3.Position.z });

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
			Vertex tangent = getTangentAt(0, t);
			//Normalize the tangent here to speed things up later
			tangent.Position = glm::normalize(glm::vec4(tangent.Position, 1));

			//Form Transofrm matrix here and put it into the vector
			glm::vec3 position = glm::vec3(x, y, z);
			game->curvedSwitches->trackPieceTransforms.push_back(getModelMatrix(tangent, position));
		}
		//Buffer the data
		bufferData(game);
		//Save the current end and ends control point
		//game->track->currentEnd = p3;
		//game->track->currentEndCP = p2;
		//game->track->numberOfCurves++;
	}

	void bufferData(GameState* game)
	{
		std::cout << "Buffering Data" << game->curvedSwitches->trackPieceTransforms.size() << std::endl;
		//Generate and bind a new buffer
		glGenBuffers(1, &game->curvedSwitches->switchVBO);
		glBindBuffer(GL_ARRAY_BUFFER, game->curvedSwitches->switchVBO);
		glBufferData(GL_ARRAY_BUFFER, game->curvedSwitches->trackPieceTransforms.size() * sizeof(glm::mat4), &game->curvedSwitches->trackPieceTransforms[0], GL_STATIC_DRAW);

		std::cout << "Loop Over Meshes" << std::endl;
		for (unsigned int i = 0; i < game->switchModel->meshes.size(); i++)
		{
			unsigned int VAO = game->switchModel->meshes[i].VAO;
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

	Vertex getPointAt(State *state, unsigned int curveIndex, GLfloat t)
	{
		GameState *game = &state->game_state;

		if (t < 0 || t > 1) {
			//Maybe they forgot to convert?
			//curveIndex = timeToCurveIndex(t);
			//t = t - curveIndex;
			//if (t < 0 || t > 1) {
				//Nope they're idiots, Invalid t, Exception?
				//Return empty Vertex for now, so it doesn't crash the program
			//	cout << "Error finding point at t with t = " << t << endl;
			//	return Vertex();
			//}
		}

		//Skip through the cotrol points until we get to the current curve
		unsigned int index = 0; //curveIndex * 4;
		vector<GLfloat> p0 = game->curvedSwitches->controlPoints[index];
		vector<GLfloat> p1 = game->curvedSwitches->controlPoints[index + 1];
		vector<GLfloat> p2 = game->curvedSwitches->controlPoints[index + 2];
		vector<GLfloat> p3 = game->curvedSwitches->controlPoints[index + 3];

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

		//Shouldnt it be -v t?????
		Vertex v;
		v.Position = glm::vec3(x, y, z);
		return v;
	}

	Vertex getTangentAt(unsigned int curveIndex, GLfloat t)
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
		vector<float> p0 = game->curvedSwitches->controlPoints[index];
		vector<float> p1 = game->curvedSwitches->controlPoints[index + 1];
		vector<float> p2 = game->curvedSwitches->controlPoints[index + 2];
		vector<float> p3 = game->curvedSwitches->controlPoints[index + 3];

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
		//glm::mat4 rotation = glm::transpose(glm::lookAt(glm::normalize(glm::vec3(x, y, z)), direction.Position, glm::vec3(0, 1, 0)));
		//model =  rotation * model;

		glm::vec3 start = glm::normalize(glm::vec3(0, 0, -1));
		//glm::vec3 start = glm::vec3(0, 1, 0);
		glm::vec3 dest = glm::normalize(glm::vec3(direction.Position));

		float cosTheta = glm::dot(start, dest);
		float theta = glm::acos(cosTheta);
		glm::vec3 rotationAxis = glm::cross(start, dest);

		model = glm::translate(model, position);

		model = glm::scale(model, glm::vec3(0.26f, 0.26f, 0.26f));

		//model = glm::rotate(model, cosTheta, rotationAxis);
		model = glm::rotate(model, theta, rotationAxis);

		return model;
	}
}