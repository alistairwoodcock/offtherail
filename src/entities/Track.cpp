#ifndef TRACK_CPP
#define TRACK_CPP

#include "Track.h"
#include "../libs/model.h"

namespace Tracks {

	static float MAX_CP_LENGTH = 5;
	static float MIN_CP_LENGTH = 2;

	static float MIN_Z_DISTANCE_BETWEEN_ANCHOR_CP = 2;
	static float MAX_Z_DISTANCE_BETWEEN_ANCHOR_CP = 5;
	static float MIN_X_DISTANCE_BETWEEN_ANCHOR_CP = 0;
	static float MAX_X_DISTANCE_BETWEEN_ANCHOR_CP = 8;

	static float MIN_Z_DISTANCE_BETWEEN_ANCHOR_POINTS = 8;
	static float MAX_Z_DISTANCE_BETWEEN_ANCHOR_POINTS = 15;
	static float MIN_X_DISTANCE_BETWEEN_ANCHOR_POINTS = 0;
	static float MAX_X_DISTANCE_BETWEEN_ANCHOR_POINTS = 8;

	void loadModel(GameState* game);
	void generateCurveSegments(GameState* game, Vertex p0, Vertex p1, Vertex p2, Vertex p3);
	Vertex getTangentAt(GameState* game, unsigned int curveIndex, GLfloat t);
	glm::mat4 getModelMatrix(Vertex direction, glm::vec3 position);
	void bufferData(GameState* game);
	unsigned int timeToCurveIndex(GLfloat time);
	Vertex getPointAt(State *state, unsigned int curveIndex, GLfloat t);
	void extend(State *state);
	float getRandomNumber(float min, float max);
	void normalize(vector<GLfloat>& vec);

	void setup(State *state) {
		GameState *game = &state->game_state;
		std::cout << "SETTING UP MyModel" << std::endl;
		//game->mymodel = new Model("mymodel", "models/teapot/teapot.obj");

		
		//Model* trackModel = new Model("track", "models/track/longstraight.obj");
		//game->trackModel = new Model("track", "models/track/longstraight.obj");
		game->track = new Track();
		game->track->numberOfCurves = 0;
		
		loadModel(game);
		
		//game->track->x = 0;
		//game->track->y = game->ground;
		//game->track->z = 0.0f;

		//Set up first piece of track
		//Can we do this in one line???
		Vertex p0 = Vertex({ 0, 0, 0 });
		Vertex p1 = Vertex({ -5,0,-3 });
		Vertex p2 = Vertex({ 1,0,-8 });
		Vertex p3 = Vertex({ -1,0,-12 });

		Tracks::generateCurveSegments(game, p0, p1, p2, p3);

		//Add a 2nd piece
		extend(state);
		
	}

	//glm::mat4 model
	void render(State *state, glm::mat4 &projection, glm::mat4 &view) 
	{
		std::cout << "Rednder Track" << std::endl;
		Model* trackModel = state->game_state.trackModel;

		Shader trackShader = Shaders::get(state, "track");
		unsigned int ID = trackShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		//Loop through each mesh of the model and draw an instance of the track	
		for (unsigned int i = 0; i < trackModel->meshes.size(); i++)
		{
			//Bind the meshes texture
			//trackShader->setInt("texture_diffuse1", 0);
			shaderSetInt(ID, "texture_diffuse1", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, trackModel->meshes[i].textures[0].id);

			//Bind the vertex array
			glBindVertexArray(trackModel->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, trackModel->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, state->game_state.track->trackPieceTransforms.size());
			glBindVertexArray(0);
		}
		
	}

	void loadModel(GameState* game)
	{
		game->trackModel = new Model("track", "models/track/singlerailtexturedJoined.obj");

		glGenBuffers(1, &game->track->trackVBO);
		glBindBuffer(GL_ARRAY_BUFFER, game->track->trackVBO);

		for (unsigned int i = 0; i < game->trackModel->meshes.size(); i++)
		{
			unsigned int VAO = game->trackModel->meshes[i].VAO;
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

	void extend(State *state)
	{
		GameState *game = &state->game_state;
		//The first control point must be co linear with currentEnd and currentEndCP G1 Continuity
		//Calculate the line between them
		//Find the direction vector between the two points
		vector<GLfloat> dir = {
			game->track->currentEnd.Position.x - game->track->currentEndCP.Position.x,
			game->track->currentEnd.Position.y - game->track->currentEndCP.Position.y,
			game->track->currentEnd.Position.z - game->track->currentEndCP.Position.z,
		};
		//To make calculating t (z value) easy normalize the vector
		normalize(dir);

		//Use current End as the Anchor Point: Line = Anchor Point + dir * scalar
		//Radnomly generate the scalar (z value) (distance along the line to travel)
		float max = MAX_CP_LENGTH;
		float min = MIN_CP_LENGTH;
		float t = getRandomNumber(min, max);

		//Calculate the point on the line z units along the direction vector from the anchor point
		float x = game->track->currentEnd.Position.x + (dir[0] * t);
		//Want the curve on the ground so y will always be zero.
		float y = 0;
		float z = game->track->currentEnd.Position.z + (dir[2] * t);

		vector<GLfloat> controlPointPosition = { x, y, z };
		Vertex p1 = Vertex(controlPointPosition); //P1

		//Randomly generate the P2 (CP) and P3 (Anchor Point)
		//Control Points Z value must always be bigger than the anchor point.
		//Start with P3 (anchor point)
		min = game->track->currentEnd.Position.x - MAX_X_DISTANCE_BETWEEN_ANCHOR_POINTS;
		max = game->track->currentEnd.Position.x + MAX_X_DISTANCE_BETWEEN_ANCHOR_POINTS;
		x = getRandomNumber(min, max);
		//Make sure it is over the minimum distance	
		while (abs(game->track->currentEnd.Position.x - x) < MIN_X_DISTANCE_BETWEEN_ANCHOR_POINTS)
		{
			x = getRandomNumber(min, max);
		}

		min = game->track->currentEnd.Position.z - MAX_Z_DISTANCE_BETWEEN_ANCHOR_POINTS;
		max = game->track->currentEnd.Position.z - MIN_Z_DISTANCE_BETWEEN_ANCHOR_POINTS;
		z = getRandomNumber(min, max);

		vector<GLfloat> anchorPosition = { x,y,z };
		Vertex p3 = Vertex(anchorPosition);		//P3

		//Find a good control point for the new end point
		min = p3.Position.x - MAX_X_DISTANCE_BETWEEN_ANCHOR_CP;
		max = p3.Position.x + MAX_X_DISTANCE_BETWEEN_ANCHOR_CP;
		x = getRandomNumber(min, max);
		//Make sure it is above the minimum
		while (abs(p3.Position.x - x) < MIN_X_DISTANCE_BETWEEN_ANCHOR_CP) {
			x = getRandomNumber(min, max);
		}

		min = p3.Position.z;
		max = p3.Position.z + MAX_Z_DISTANCE_BETWEEN_ANCHOR_CP;
		z = getRandomNumber(min, max);
		while (abs(p3.Position.z - z) < MIN_Z_DISTANCE_BETWEEN_ANCHOR_CP) {
			z = getRandomNumber(min, max);
		}

		vector<GLfloat> anchorCPPosition = { x, y, z };
		Vertex p2 = Vertex(anchorCPPosition);			//P2

		generateCurveSegments(game, game->track->currentEnd, p1, p2, p3);
	}

	void generateCurveSegments(GameState* game, Vertex p0, Vertex p1, Vertex p2, Vertex p3)
	{
		//Add the vertices to the control points vector
		game->track->controlPoints.push_back({ p0.Position.x, p0.Position.y, p0.Position.z });
		game->track->controlPoints.push_back({ p1.Position.x, p1.Position.y, p1.Position.z });
		game->track->controlPoints.push_back({ p2.Position.x, p2.Position.y, p2.Position.z });
		game->track->controlPoints.push_back({ p3.Position.x, p3.Position.y, p3.Position.z });

		vector<GLfloat> subPointsVector;
		for (int i = 0; i != game->track->numberOfSegments; ++i) {

			// use the parametric time value 0 to 1
			float t = (float)i / (game->track->numberOfSegments - 1);

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
			Vertex tangent = getTangentAt(game, game->track->numberOfCurves, t);
			//Normalize the tangent here to speed things up later
			tangent.Position = glm::normalize(glm::vec4(tangent.Position, 1));

			//Form Transofrm matrix here and put it into the vector
			glm::vec3 position = glm::vec3(x, y, z);
			game->track->trackPieceTransforms.push_back(getModelMatrix(tangent, position));
		}
		//Buffer the data
		bufferData(game);
		//Save the current end and ends control point
		game->track->currentEnd = p3;
		game->track->currentEndCP = p2;
		game->track->numberOfCurves++;
	}

	Vertex getPointAt(State *state, unsigned int curveIndex, GLfloat t)
	{
		GameState *game = &state->game_state;

		if (t < 0 || t > 1) {
			//Maybe they forgot to convert?
			curveIndex = timeToCurveIndex(t);
			t = t - curveIndex;
			if (t < 0 || t > 1) {
				//Nope they're idiots, Invalid t, Exception?
				//Return empty Vertex for now, so it doesn't crash the program
				cout << "Error finding point at t with t = " << t << endl;
				return Vertex();
			}
		}

		//Skip through the cotrol points until we get to the current curve
		unsigned int index = curveIndex * 4;
		vector<GLfloat> p0 = game->track->controlPoints[index];
		vector<GLfloat> p1 = game->track->controlPoints[index + 1];
		vector<GLfloat> p2 = game->track->controlPoints[index + 2];
		vector<GLfloat> p3 = game->track->controlPoints[index + 3];

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

	Vertex getTangentAt(GameState* game, unsigned int curveIndex, GLfloat t)
	{
		if (t < 0 || t > 1) {
			//Maybe they forgot to convert?
			curveIndex = timeToCurveIndex(t);
			if (curveIndex < 0 || curveIndex > 1) {
				//Nope they're idiots, Invalid t, Exception?
				//Return empty Vertex for now, so it doesn't crash the program
				cout << "Error finding point at t with t = " << t << endl;
				return Vertex();
			}
		}
		// nice to pre-calculate 1.0f-t because we will need it frequently
		float it = 1.0f - t;

		// calculate blending functions of First derivative of bezier curve function
		// -3(1-t)^2 P0 + (3(1-t)^2 - 6t(1-t)) P1 + (6t(1-t) - 3t^2) P2 + 3t^2 P3
		float b0 = 3 * t * t;
		float b1 = (6 * t * it) - (3 * t * t);
		float b2 = (3 * it * it) - (6 * t * it);
		float b3 = -3 * it * it;

		unsigned int index = curveIndex * 4;
		vector<GLfloat> p0 = game->track->controlPoints[index];
		vector<GLfloat> p1 = game->track->controlPoints[index + 1];
		vector<GLfloat> p2 = game->track->controlPoints[index + 2];
		vector<GLfloat> p3 = game->track->controlPoints[index + 3];

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

		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

		//model = glm::rotate(model, cosTheta, rotationAxis);
		model = glm::rotate(model, theta, rotationAxis);

		return model;
	}

	void bufferData(GameState* game)
	{
		//Generate and bind a new buffer
		glGenBuffers(1, &game->track->trackVBO);
		glBindBuffer(GL_ARRAY_BUFFER, game->track->trackVBO);
		glBufferData(GL_ARRAY_BUFFER, game->track->trackPieceTransforms.size() * sizeof(glm::mat4), &game->track->trackPieceTransforms[0], GL_STATIC_DRAW);

		for (unsigned int i = 0; i < game->trackModel->meshes.size(); i++)
		{
			unsigned int VAO = game->trackModel->meshes[i].VAO;
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

	//Curves are from 0 - 1 so if the time is greater than 1 we need to 
	//get the index of the curve at that time
	unsigned int timeToCurveIndex(GLfloat time) {
		//Although this is one line Im leaving it as an independant method
		//because I predict we will need to actually do some maths here eventually
		unsigned int index = floor(time);
		return index;
	}

	void normalize(vector<GLfloat>& vec) {
		//Calulate length
		float length = 0;
		for (GLfloat num : vec) {
			length += num * num;
		}
		length = sqrt(length);
		for (int i = 0; i < vec.size(); i++) {
			vec[i] = vec[i] / length;
		}
	}

	float getRandomNumber(float min, float max)
	{
		return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
	}
	/*
	


	void update(State *state, float time, float deltaTime) {
		if (state->game_state.track->z < 200) {
			state->game_state.track->z++;
		}
		else {
			state->game_state.track->z = 0;
		}
	}

	void render(State *state, glm::mat4 &projection, glm::mat4 &view) {

		Model *trackModel = state->game_state.trackModel;
		Track *track = state->game_state.track;

		Shader trackShader = Shaders::get(state, "track");
		unsigned int ID = trackShader.ID;

		useShader(ID);
		shaderSetMat4(ID, "projection", projection);
		shaderSetMat4(ID, "view", view);

		// render the loaded model
		glm::mat4 model;

		//model = glm::translate(model, glm::vec3(train->x, train->y, train->z));
		//model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));

		//model = glm::rotate(model, train->y_rot, glm::vec3(0.0, 1.0, 0.0));
		//model = glm::rotate(model, train->z_rot, glm::vec3(0.0, 0.0, 1.0));

		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		//model = glm::translate(model, glm::vec3(track->x, track->y, track->z));
		model = glm::translate(model, glm::vec3(track->x, -0.6, track->z));

		shaderSetMat4(ID, "model", model);
		trackModel->Draw(trackShader);

	}

	*/
}

#endif

