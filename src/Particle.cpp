#include "Particle.h"
#include "shader.h"

//constructor
Particle::Particle(glm::vec3 position, glm::vec3 velocity, float gravityResistance, float lifeTime, float scale, Camera * cam)
{
	this->cam = cam;
	this->position = position;
	this->velocity = velocity;
	this->gravity = gravity;
	this->lifeTime = lifeTime;
	this->scale = scale;
	this->color = glm::vec4(0.0, 0.0, 0.0, 1.0);
}
//updates transparency, scale and position
bool Particle::update() {
	color[3] -= 0.1;
	scale += 0.1;
	velocity.y += gravity;
	position += velocity;
	currentTime++;
	return currentTime > lifeTime;
}
//renders the particle
void Particle::render(Shader * shader) {
	shader->use();
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, position);
	glm::mat4 view = cam->GetViewMatrix();

	//modified modelMatrix so that the particles do not rotate
	modelMatrix[0][0] = view[0][0];
	modelMatrix[0][1] = view[1][0];
	modelMatrix[0][2] = view[2][0];
	modelMatrix[1][0] = view[0][1];
	modelMatrix[1][1] = view[1][1];
	modelMatrix[1][2] = view[2][1];
	modelMatrix[2][0] = view[0][2];
	modelMatrix[2][1] = view[1][2];
	modelMatrix[2][2] = view[2][2];
	
	//drawing dvao
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, scale));
	glm::mat4 modelViewMatrix = view * modelMatrix;
	shader->setVec4("color", color);
	shader->setMat4("modelViewMatrix", modelViewMatrix);

	glBindVertexArray(model->vaoID);
	glDrawArrays(GL_TRIANGLES, 0, model->nVertex);

}

Particle::~Particle()
{
}
