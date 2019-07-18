#pragma once
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "Entity.h"
#include "GeometricCull.h"
#include "shader.h"
#include <vector>
#ifndef QUADTREE_H
#define QUADTREE_H


//holds the data for each terrain face
struct Node
{
	float posX, posZ;
	int size, length;
	Node* nw, *ne, *sw, *se, *c, *parent;
	int nwy;
	int ney;
	int swy;
	int sey;
	int cy;
	unsigned int vboHandle;
	unsigned int face = 99999999;
	unsigned int textureID;
};

//holds data to display all the nodes
class QuadTree
{
public:
	QuadTree();
	std::vector<Entity*> entityList;
	std::vector<float> y_values;
	std::vector<Node > nodes;
	Camera * camera;
	Node * root;
	void construct_tree(glm::vec3 pos);
	void build_tree(Node *, QuadTree *);
	void render(Shader *, Light *, QuadTree *, FrustrumCuller *, bool);
	float findClosestY(float, float);
	~QuadTree();
};
#endif
