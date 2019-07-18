#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Light {
public:
	Light(glm::vec3 color, glm::vec3 position) {
		this->color = color;
		this->position = position;
	}
	glm::vec3 color;
	glm::vec3 position;
};



#endif