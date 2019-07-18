#pragma once
#ifndef MODEL_H
#define MODEL_H

//holds data to render a single model
class Model {
public:
	unsigned int vaoID;
	int nVertex;
	float shineDamper;
	float reflectivity;

	//bounding box used for collision
	float x1, x2, y1, y2, z1, z2;
	int textureID;
	Model();
	bool collision(float y1, float scale);
	~Model();

};


#endif
