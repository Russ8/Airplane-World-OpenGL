#include "QuadTree.h"
#include "Camera.h"
#include "GeometricCull.h"
#include "shader.h"
#include "Model.h"
#include "Entity.h"
#include "Light.h"
#include "helper.h"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

//world options
static int worldSeed = 323445;
static int worldHeight = 100;
static float worldmaxheight = 0.07;
static int worldSize = 15; //keep this <16
static int detail = 10; //lower is faster

//utility
static int counter = 0;
static int fcounter = 0;
static int indices[6] = { 0, 2, 3, 3, 1, 0 };
int randomNumber = 0;

//returns random number based on input. used for terrain.
float gen_y(float, float, int);

//renders terrain, constructs terrain if necessary.
void traverse(Node * , Camera * , Shader * , Light * , Node *, QuadTree *, FrustrumCuller *, bool);

//models
std::vector<Model *> pineTree;
std::vector<Model *> stump;

//images
int sandImg;

//constructor for quadtree. generates objs
QuadTree::QuadTree()
{
	pineTree = loadModelFromObj("PineTree03.obj");
	stump = loadModelFromObj("TreeStump03.obj");
	sandImg = loadRGBATexture("sand.png");
}

//sets up the quadtree
void QuadTree::construct_tree(glm::vec3 pos) {

	//creates a new face - each representing a terrain face
	Node * node = new Node();
	counter++;
	node->size = worldSize;
	node->length = pow(2, node->size);
	node->posX = pos.x;
	node->posZ = pos.z;
	node->nwy = 0;

	//generate 5 poins of a face
	fcounter++;
	this->y_values.push_back(gen_y(pos.x, pos.z, node->size)+worldHeight); //(0,0)
	node->ney = 1;
	fcounter++;
	this->y_values.push_back(gen_y(pos.x + float(node->length), pos.z, node->size)+worldHeight); //(1024,0)
	node->swy = 2;
	fcounter++;
	this->y_values.push_back(gen_y(pos.x, pos.z + float(node->length), node->size)+worldHeight); // (0, 1024)
	node->sey = 3;
	fcounter++;
	this->y_values.push_back(gen_y(pos.x + float(node->length), pos.z + float(node->length), node->size)+worldHeight); //(1024, 1024)
	node->cy = 4;
	fcounter++;
	this->y_values.push_back(gen_y(pos.x + (float(node->length) / 2), pos.z + (float(node->length) / 2), node->size)+worldHeight); //(512, 512)
	node->textureID = sandImg;

	build_tree(node, this);
	this->root = node;
}

//builds the current node of a given quadtree
void QuadTree::build_tree(Node * node, QuadTree * tree) {
	if (node->size == 0) return;
	//gen random number for object placement
	srand(randomNumber);
	randomNumber = rand() % 100;

	//spawn pine trees
	if (node->size == 4) {
		if (fcounter%40 == 1) {
			Entity * tree = new Entity(glm::vec3(node->posX + node->length / 2, y_values.at(node->cy), node->posZ + node->length / 2));
			tree->models = pineTree;
			tree->scaleFactor = randomNumber%4;
			entityList.push_back(tree);
		}
	}

	//spawn stumps
	if(node->size == 5) {
		if ((fcounter%35==1)) {
			Entity * treeStump = new Entity(glm::vec3(node->posX + node->length / 2, y_values.at(node->cy), node->posZ + node->length / 2));
			treeStump->models = stump;
			treeStump->scaleFactor = 3;
			entityList.push_back(treeStump);
		}
	}

	
	
	
	//the resut of this function implements the diamond square algorithm. 

	//geneate random heights for the children of this node
	float N = gen_y(node->posX + (node->length/ 2) , node->posZ, node->size) + ((tree->y_values.at(node->nwy) + tree->y_values.at(node->ney)) / 2); // (0.5, 0.0)
	float W = gen_y(node->posX, node->posZ + (node->length / 2), node->size) + ((tree->y_values.at(node->nwy) + tree->y_values.at(node->swy)) / 2); // (0.0, 0.5)
	float E = gen_y(node->posX + node->length, node->posZ + (node->length / 2), node->size) + ((tree->y_values.at(node->ney) + tree->y_values.at(node->sey)) / 2); // (1.0, 0.5)
	float S = gen_y(node->posX + (node->length / 2), node->posZ + node->length, node->size) + ((tree->y_values.at(node->swy) + tree->y_values.at(node->sey)) / 2); // (0.5, 1.0)

	node->textureID = sandImg;

	//create children and give them their new x, y, z values
	if (node->nw == NULL) {
		Node * new_node_nw = new Node();
		counter++;
		new_node_nw->size = node->size - 1;
		new_node_nw->length = pow(2, new_node_nw->size);
		new_node_nw->parent = node;
		new_node_nw->posX = node->posX;
		new_node_nw->posZ = node->posZ;
		new_node_nw->nwy = node->nwy;
		new_node_nw->sey = node->cy;
		

		new_node_nw->swy = fcounter;
		tree->y_values.push_back(W);
		fcounter++;
		new_node_nw->ney = fcounter;
		tree->y_values.push_back(N);
		fcounter++;
		new_node_nw->cy = fcounter;
		tree->y_values.push_back(gen_y(node->posX + (node->length / 4), node->posZ + (node->length / 4), new_node_nw->size) + ((tree->y_values.at(node->nwy) + tree->y_values.at(node->cy)) / 2)); // (0.25, 0.25)
		fcounter++;
		node->nw = new_node_nw;
	}
	if (node->ne == NULL) {
		Node * new_node_ne = new Node();
		counter++;

		new_node_ne->size = node->size - 1;
		new_node_ne->length = pow(2, new_node_ne->size);
		new_node_ne->parent = node;
		new_node_ne->posX = node->posX + new_node_ne->length;
		new_node_ne->posZ = node->posZ;

		new_node_ne->nwy = fcounter;
		tree->y_values.push_back(N);
		fcounter++;
		new_node_ne->sey = fcounter;
		tree->y_values.push_back(E);
		fcounter++;
		new_node_ne->swy = node->cy;
		new_node_ne->ney = node->ney;
		new_node_ne->cy = fcounter;
		tree->y_values.push_back(gen_y(node->posX + (node->length / (4/3)), node->posZ + (node->length / 4), new_node_ne->size) + ((N + E) / 2)); // (0.75, 0.25)
		fcounter++;
		node->ne = new_node_ne;
	}
	if (node->sw == NULL) {
		Node * new_node_sw = new Node();
		counter++;
		new_node_sw->size = node->size - 1;
		new_node_sw->length = pow(2, new_node_sw->size);
		new_node_sw->parent = node;
		new_node_sw->posX = node->posX;
		new_node_sw->posZ = node->posZ + new_node_sw->length;

		new_node_sw->nwy = fcounter;
		tree->y_values.push_back(W);
		fcounter++;
		new_node_sw->sey = fcounter;
		tree->y_values.push_back(S);
		fcounter++;
		new_node_sw->swy = node->swy;
		new_node_sw->ney = node->cy;
		new_node_sw->cy = fcounter;
		tree->y_values.push_back(gen_y(node->posX + (node->length / 4), node->posZ + (node->length / (4 / 3)), new_node_sw->size) + ((W + S) / 2)); // (0.25, 0.75)
		fcounter++;
		node->sw = new_node_sw;
	}
	if (node->se == NULL) {
		Node * new_node_se = new Node();
		counter++;
		new_node_se->size = node->size - 1;
		new_node_se->length = pow(2, new_node_se->size);
		new_node_se->parent = node;
		new_node_se->posX = node->posX + new_node_se->length;
		new_node_se->posZ = node->posZ + new_node_se->length;
		new_node_se->nwy = node->cy;
		new_node_se->sey = node->sey;

		new_node_se->swy = fcounter;
		tree->y_values.push_back(S);
		fcounter++;
		new_node_se->ney = fcounter;
		tree->y_values.push_back(E);
		fcounter++;
		new_node_se->cy = fcounter;
		tree->y_values.push_back(gen_y(node->posX + (node->length / (4 / 3)), node->posZ + (node->length / (4 / 3)), new_node_se->size) + ((tree->y_values.at(node->cy) + tree->y_values.at(node->sey)) / 2)); // (0.75, 0.75)
		fcounter++;
		node->se = new_node_se;
	}
}

//function that renders the quadtree
void QuadTree::render(Shader * shader, Light * light, QuadTree * tree, FrustrumCuller * culler, bool cullFaces) {
	traverse(this->root, this->camera, shader, light, this->root, tree, culler, cullFaces);
}

//euclidean distance
int dist(int x1, int x2, int y1, int y2, int z1, int z2) {
	return int(sqrt(((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1)) + ((z2 - z1)*(z2 - z1))));
}

//gets minimum distance from camera to node of quadtree
int getDistance(Node * node, Camera * camera, QuadTree * tree) {
	int v1 = dist(node->posX, camera->Position.x, tree->y_values[node->nwy], camera->Position.y, node->posZ, camera->Position.z);
	int v2 = dist(node->posX + node->length, camera->Position.x, tree->y_values[node->ney], camera->Position.y, node->posZ, camera->Position.z);
	int v3 = dist(node->posX, camera->Position.x, tree->y_values[node->swy], camera->Position.y, node->posZ + node->length, camera->Position.z);
	int v4 = dist(node->posX + node->length, camera->Position.x, tree->y_values[node->sey], camera->Position.y, node->posZ + node->length, camera->Position.z);
	int d = v1;
	if (v2 < d) d = v2;
	if (v3 < d) d = v3;
	if (v4 < d) d = v4;
	return d;
}

//finds a node in a quadtree given x, z, size values. Returns pointer the node otherwise returns null
Node * find(Node * node, int x, int z, int size, QuadTree * tree) {

	if (node == NULL) {
		return NULL;
	}
	if ( (node->nw == NULL)) {
		tree->build_tree(node, tree);
	}
	if ((node->size == size) && (node->posX == x) && (node->posZ == z)) {
		return node;
	}
	if (size == node->size) {
		return NULL;
	}
	int length = node->length / 2;
	if ((x < (node->posX + length)) && (z < (node->posZ + length))) { //NW
		return find(node->nw, x, z, size, tree);
	}
	if ((x >=(node->posX + length)) && (z < (node->posZ + length))) { //Ne
		return find(node->ne, x, z, size, tree);
	}
	if ((x < (node->posX + length)) && (z >=(node->posZ + length))) { //SW
		return find(node->sw, x, z, size, tree);
	}
	if ((x >=(node->posX + length)) && (z >=(node->posZ + length))) { //SE
		return find(node->se, x, z, size, tree);
	}
	else {
		return NULL;
	}
}

//similar to find, but returns the node's y value
Node * gety(Node * node, int x, int z) {

	if (node == NULL) {
		return NULL;
	}
	if ((node->size == 1) || (node->nw == NULL)) {
		return node;
	}

	int length = node->length / 2;
	if ((x < (node->posX + length)) && (z < (node->posZ + length))) { //NW
																	  
		return gety(node->nw, x, z);
	}
	if ((x >= (node->posX + length)) && (z < (node->posZ + length))) { //Ne
		return gety(node->ne, x, z);
	}
	if ((x < (node->posX + length)) && (z >= (node->posZ + length))) { //SW
		return gety(node->sw, x, z);
	}
	if ((x >= (node->posX + length)) && (z >= (node->posZ + length))) { //SE
		return gety(node->se, x, z);
	}
	else {
		return NULL;
	}
}

//function that gets the closest y value of a given x and z value
float QuadTree::findClosestY(float x, float z) {
	return y_values.at(gety(root, x, z)->cy);
}

//calculates a normal for a node using its dimensions
glm::vec3 calculateNormal(Node * node, QuadTree * tree) {
	if (node == NULL) {
		return glm::vec3(0.0, 10.0, 0.0);
	}

	glm::vec3 a = glm::vec3(0, tree->y_values[node->nwy], 0);
	glm::vec3 b = glm::vec3(node->length, tree->y_values[node->ney], 0);
	glm::vec3 c = glm::vec3(0, tree->y_values[node->swy], node->length);

	return normalize(glm::cross(c - a, b - a));
}

//functions that traverses a quadtree, rendering faces and constructing them on the fly.
void traverse(Node * node, Camera * camera, Shader * shader, Light * light, Node * root, QuadTree * tree, FrustrumCuller * culler, bool cullFaces) {
	
	if (node == NULL) {
		std::cout << "parent is null" << std::endl;
		return;

	}

	//if we heave reached the minimum sized node, go no further
	if (node->size == 0) return;

	//for culling faces, 5 points of each terrain face are considered
	glm::vec3 c1 = glm::vec3(node->posX, tree->y_values[node->nwy], node->posZ);
	glm::vec3 c2 = glm::vec3(node->posX+node->length, tree->y_values[node->ney], node->posZ);
	glm::vec3 c3 = glm::vec3(node->posX, tree->y_values[node->swy], node->posZ + node->length);
	glm::vec3 c4 = glm::vec3(node->posX + node->length, tree->y_values[node->sey], node->posZ + node->length);
	glm::vec3 c5 = glm::vec3(node->posX, tree->y_values[node->nwy], node->posZ);
	//frustrum culling
	if (cullFaces && (!culler->pointInFrustum(c1)) && (!culler->pointInFrustum(c2)) && (!culler->pointInFrustum(c3)) && (!culler->pointInFrustum(c4)) && (!culler->pointInFrustum(c5))) {
		return;
	}
	//if node's childrend not built, build them
	if (node->nw == NULL) {
		tree->build_tree(node, tree);
	}

	//for the dynamic level of detail, ( eg if the terrain is close, render the nodes children, is the node is far render this node and not its children)
	if (!((getDistance(node, camera, tree) > detail * node->length) || (node->size == 1))) {
		traverse(node->nw, camera, shader, light, root, tree, culler, cullFaces);
		traverse(node->ne, camera, shader, light, root, tree, culler, cullFaces);
		traverse(node->sw, camera, shader, light, root, tree, culler, cullFaces);
		traverse(node->se, camera, shader, light, root, tree, culler, cullFaces);
	}
	else {
		//create the vao for this terrain node if it does not exist
		if (node->face == 99999999) {
			glm::vec3 a = glm::vec3(0, tree->y_values[node->nwy], 0);
			glm::vec3 b = glm::vec3(0 + node->length, tree->y_values[node->ney], 0);
			glm::vec3 c = glm::vec3(0, tree->y_values[node->swy], node->length);
			glm::vec3 d = glm::vec3(0 + node->length, tree->y_values[node->sey], node->length);

			glm::vec3 normal_face = normalize(glm::cross(c - a, b - a));


			//calculate normals based on the faces of nearby nodes
			glm::vec3 normal_nw = calculateNormal(find(root, node->posX - node->length, node->posZ - node->length, node->size, tree), tree);
			glm::vec3 normal_n = calculateNormal(find(root, node->posX, node->posZ - node->length, node->size, tree), tree);
			glm::vec3 normal_ne = calculateNormal(find(root, node->posX + node->length, node->posZ - node->length, node->size, tree), tree);
			glm::vec3 normal_e = calculateNormal(find(root, node->posX + node->length, node->posZ, node->size, tree), tree);
			glm::vec3 normal_se = calculateNormal(find(root, node->posX + node->length, node->posZ + node->length, node->size, tree), tree);
			glm::vec3 normal_s = calculateNormal(find(root, node->posX, node->posZ + node->length, node->size, tree), tree);
			glm::vec3 normal_sw = calculateNormal(find(root, node->posX - node->length, node->posZ + node->length, node->size, tree), tree);
			glm::vec3 normal_w = calculateNormal(find(root, node->posX - node->length, node->posZ, node->size, tree), tree);

			float point_nw_x = (normal_face.x + normal_nw.x + normal_n.x + normal_w.x) / 4;
			float point_nw_y = (normal_face.y + normal_nw.y + normal_n.y + normal_w.y) / 4;
			float point_nw_z = (normal_face.z + normal_nw.z + normal_n.z + normal_w.z) / 4;

			float point_sw_x = (normal_face.x + normal_w.x + normal_sw.x + normal_s.x) / 4;
			float point_sw_y = (normal_face.y + normal_w.y + normal_sw.y + normal_s.y) / 4;
			float point_sw_z = (normal_face.z + normal_w.z + normal_sw.z + normal_s.z) / 4;

			float point_ne_x = (normal_face.x + normal_n.x + normal_ne.x + normal_e.x) / 4;
			float point_ne_y = (normal_face.y + normal_n.y + normal_ne.y + normal_e.y) / 4;
			float point_ne_z = (normal_face.z + normal_n.z + normal_ne.z + normal_e.z) / 4;

			float point_se_x = (normal_face.x + normal_e.x + normal_se.x + normal_s.x) / 4;
			float point_se_y = (normal_face.y + normal_e.y + normal_se.y + normal_s.y) / 4;
			float point_se_z = (normal_face.z + normal_e.z + normal_se.z + normal_s.z) / 4;

			float point_c_x = (point_nw_x + point_sw_x + point_ne_x + point_se_x) / 4;
			float point_c_y = (point_nw_y + point_sw_y + point_ne_y + point_se_x) / 4;
			float point_c_z = (point_nw_z + point_sw_z + point_ne_z + point_se_x) / 4;

			//image quality
			float image_quality = pow(2, 6);

			float detail = pow(2, node->size - 1);

			//each node has 5 points for 4 triangles
			float data[40];
			data[0] = node->posX; // nwx  //vertices
			data[1] = tree->y_values[node->nwy]; // nwy
			data[2] = node->posZ; // nwz
			data[3] = 0.0; //tex coords x
			data[3] = 0.0; //tex coords y
			data[5] = point_nw_x; // normals     
			data[6] = point_nw_y;
			data[7] = point_nw_z;

			data[8] = node->posX + node->length; // nex  //vertices
			data[9] = tree->y_values[node->ney]; // nwey
			data[10] = node->posZ; // nez
			data[11] = detail; //tex coords x
			data[12] = 0.0; //tex coords y
			data[13] = point_ne_x; // normals a   
			data[14] = point_ne_y;
			data[15] = point_ne_z;

			data[16] = node->posX; // swx  //vertices
			data[17] = tree->y_values[node->swy]; // swy
			data[18] = node->posZ + node->length; // swz
			data[19] = 0.0; //tex coords x
			data[20] = detail; //tex coords y
			data[21] = point_sw_x; // normals 
			data[22] = point_sw_y;
			data[23] = point_sw_z;

			data[24] = node->posX + node->length; // nwx  //vertices
			data[25] = tree->y_values[node->sey]; // nwy
			data[26] = node->posZ + node->length; // nwz
			data[27] = detail; //tex coords x
			data[28] = detail; //tex coords y
			data[29] = point_se_x; // normals 
			data[30] = point_se_y;
			data[31] = point_se_z;

			data[32] = node->posX + node->length/2; // nwx  //vertices
			data[33] = tree->y_values[node->cy]; // nwy
			data[34] = node->posZ + node->length/2; // nwz
			data[35] = detail/2; //tex coords x
			data[36] = detail/2; //tex coords y
			data[37] = point_c_x; // normals 
			data[38] = point_c_y;
			data[39] = point_c_z;

			//build vao , vbo, ect
			unsigned int VBO, VAO, EBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// texture coord attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
			glEnableVertexAttribArray(2);
			node->vboHandle = VBO;
			node->face = VAO;
		}
		//finally rendering the terrain 
//std::cout <<tree->y_values.size() << std::endl;
		glm::mat4 model;
		
		model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		shader->setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, node->textureID);
		glBindVertexArray(node->face);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		return;

	}
	
}



QuadTree::~QuadTree()
{
}

float max(float a, float b) {
	if (a > b) return b;
}

float min(float a, float b) {
	if (a < b) return b;
}

//generates a random number given three inputs
float gen_y(float x, float z, int size) { 
	int seed = (x * 1339) + (z * 2477) + worldSeed;
	srand(seed);
	float temp = max(float(rand()/ 10000000000.0), worldmaxheight );
	//std::cout << "rand: " <<temp << std::endl;
	float random = temp * pow(2, float(size)*1.0);


	if (int(random) % 2 == 0) random *= -1.0;


	if(random<0) return 0.0;

	//the randomness is only affected by x and z. size affects the value's size
	return random;
}
