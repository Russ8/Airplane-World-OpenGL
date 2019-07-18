#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "external/tiny_obj_loader.h"
#include "external/stb_image.h"
#include "Model.h"

#ifndef HELPER_H
#define HELPER_H

//returns id to a loaded cube map
inline unsigned int loadCubeMap(std::string files[]) {

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char *data = stbi_load(files[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << files[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

//returns id to a loaded rgb texture
inline int loadTexture(std::string file) {
	unsigned int texture1;

	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1); //make sure glfw is already set up
											// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0); //contains pixel data

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR READING IMAGE" << std::endl;
	}


	stbi_image_free(data);

	return texture1;
}

//returns id to a loaded rgba image
inline int loadRGBATexture(std::string file) {
	unsigned int texture1;

	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1); //make sure glfw is already set up
											// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0); //contains pixel data

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR READING IMAGE" << std::endl;
	}

	stbi_image_free(data);

	return texture1;
}

//function that loads all shapes of a given file and places each in a model object and returns a vector to these objects
inline std::vector<Model *> loadModelFromObj(std::string inputfile, std::string useTexture = "") {

	//-----------------  load obj file --------------------



	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	//default material values
	glm::vec3 m_ambient = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 m_diffuse = glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 m_specular = glm::vec3(1.0, 1.0, 1.0);
	float shininess = 1.0;

	std::vector<Model *> models;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		Model * model = new Model();

		if (useTexture == "") {
			model->textureID = loadTexture(materials.at(shapes[s].mesh.material_ids[0]).diffuse_texname);
		}
		else {
			model->textureID = loadTexture(useTexture);
		}
		std::vector<float> shapeVertices;
		std::vector<float> shapeNormals;
		std::vector<float> shapeTextures;
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				shapeVertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
				shapeVertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
				shapeVertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
				if (attrib.normals.size() > 0) {
					shapeNormals.push_back(attrib.normals[3 * idx.normal_index + 0]);
					shapeNormals.push_back(attrib.normals[3 * idx.normal_index + 1]);
					shapeNormals.push_back(attrib.normals[3 * idx.normal_index + 2]);
				}
				else {

					shapeNormals.push_back(1.0);
					shapeNormals.push_back(1.0);
					shapeNormals.push_back(1.0);
				}
				if (attrib.texcoords.size() > 0) {
					shapeTextures.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
					shapeTextures.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
				}

			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
		
		//copy data to GPU
		unsigned int VBO, VAO, EBO, VBOt, VBOn;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &VBOt);
		glGenBuffers(1, &VBOn);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, shapeVertices.size() * sizeof(float), shapeVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		if (attrib.texcoords.size() > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, VBOt);
			glBufferData(GL_ARRAY_BUFFER, shapeTextures.size() * sizeof(float), shapeTextures.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0 * sizeof(float)));
			glEnableVertexAttribArray(1);
		}


		glBindBuffer(GL_ARRAY_BUFFER, VBOn);
		glBufferData(GL_ARRAY_BUFFER, shapeNormals.size() * sizeof(float), shapeNormals.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//get min, max values for bounding box
		float max_width = 0.0f;
		float min_x = 99999999.0;
		float max_x = -9999999.0;
		float min_y = 99999999.0;
		float max_y = -99999999.0;
		float min_z = 9999999.0;
		float max_z = -9999999.0;

		for (int i = 0; i < shapeVertices.size(); i += 3) {
			float wx = shapeVertices[i];
			if (wx < min_x) {
				min_x = wx;
			}
			if (wx > max_x) {
				max_x = wx;
			}
			wx = shapeVertices[i + 1];
			if (wx < min_y) {
				min_y = wx;
			}
			if (wx > max_y) {
				max_y = wx;
			}
			wx = shapeVertices[i + 2];
			if (wx < min_z) {
				min_z = wx;
			}
			if (wx > max_z) {
				max_z = wx;
			}
		}
		model->x1 = min_x;
		model->x2 = max_x;
		model->y1 = min_y;
		model->y2 = max_y;
		model->z1 = min_z;
		model->z2 = max_z;
		model->nVertex = shapeVertices.size() / 3;
		model->vaoID = VAO;
		models.push_back(model);
	}

	return models;
}

//function that returns a model from a list of given vertices
inline Model * loadModelFromVertices(float * array, int size) {
	Model * model = new Model();
	//copy data to GPU
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), array, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	model->vaoID = VAO;
	model->nVertex = size;
	return model;
}

#endif
