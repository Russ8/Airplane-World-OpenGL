#include "Model.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef ENTITY_H
#define ENTITY_H

//stores information for a single instance of a model. Some entities share the same model. 
class Entity {
public:
	glm::vec3 position;
	glm::vec3 velocity;
	Model * model;
	bool onground;
	float speed;
	float up;
	float right;
	float spin;


	float scaleFactor;
	std::vector<Model *> models;
	Entity(glm::vec3 Pos) {
		speed = 0.0f;
		up = 0.0f;
		float speed = 0.0f;
		right = 1.0f;
		spin = 0.0f;
		position = Pos;
		scaleFactor = 1;
		velocity = glm::vec3(0.0, 0.0, 0.0);
		onground = false;
	}

	//renders the models of the entity
	void render(Shader * shader) {
		for (int i = 0; i < models.size(); i++) {

			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, position);
			modelMatrix = glm::rotate(modelMatrix, up, glm::vec3(1.5 * cos(right), 0.0, -1.5 * sin(right)));
	
			
			modelMatrix = glm::rotate(modelMatrix, right, glm::vec3(0.0, 1.0, 0.0));
			modelMatrix = glm::rotate(modelMatrix, spin, glm::vec3(0.0, 0.0, 1.0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
			shader->setMat4("model", modelMatrix);
		
			shader->setFloat("reflectivity", 1.0);
			
			//setup texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, models.at(i)->textureID);

			//draw vao
			glBindVertexArray(models.at(i)->vaoID);
			glDrawArrays(GL_TRIANGLES, 0, models.at(i)->nVertex);
		}
	}

	//checks whether any model of the entity has collided with the terrain
	void checkCollisions(float y) {
		for (int i = 0; i < models.size(); i++) {
			
			if (models[i]->collision(position.y-y, scaleFactor)) {
				position.y = y+0.1;
				if((velocity.y!=0.0f) || (!onground))
					velocity = velocity * 0.986f;
				if (velocity.length() > 4.0f) {
					
					velocity.y = 0.0f;
				}
				
				onground = true;
			}
		}
	}

	//update model. The logic here is only used for the player
	void update() {
		right -= spin/10;
		if (spin > 0.003) {
			spin -= 0.01;
			
		}
		else if (spin < -0.003) {
			spin += 0.01;
		}

		//to keep the player on the map
		if ((position.x < 0) || (position.z < 0) || (position.x > 32000) || (position.z > 32000) || (position.y > 3000)) {
			position = glm::vec3(1000.0, 1000.0, 1000.0);
			std::cout << "out of map, teleporting back" << std::endl;

		}

		float horzspeed = glm::length(glm::vec2(velocity.x, velocity.z));
		position.x += horzspeed * sin(right);
		position.y += velocity.y - sin(up)*horzspeed;
		position.z += horzspeed * cos(right);

		//gravity
		velocity.y -= 0.025;
		onground = false;
	}
};



#endif
