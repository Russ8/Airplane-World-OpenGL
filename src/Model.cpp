#include <iostream>
#include "Model.h"


Model::Model() {

}
//returns true if y in model's bounding box
bool Model::collision(float y, float scale) {
	if (y < (y1*scale)) {
		return true;
	}
	return false;
}

Model::~Model()
{

}
