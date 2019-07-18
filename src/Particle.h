#include <glm/glm.hpp>
#include "Model.h"
#include "Camera.h"
#include "shader.h"
#ifndef PARTICLE_H
#define PARTICLE_H

//contains info the render and update a single particle
class Particle
{
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;
	Camera * cam;
	float rotation = 0;
	float gravity;
	float lifeTime;
	float scale;
	Model * model;
	int currentTime = 0;
	Particle(glm::vec3 position, glm::vec3 velocity, float gravity, float lifeTime, float scale, Camera * cam);
	bool update();
	void render(Shader * shader);
	
	~Particle();
};

#endif

