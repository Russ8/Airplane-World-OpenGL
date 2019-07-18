#include "Model.h"
#include "shader.h"
#include "helper.h"
#include <string>

#ifndef SKYBOX_H
#define SKYBOX_H

class Skybox {
public:
	float SIZE = 50000.0f;
	Model * model;

	//files for this application
	std::string files[6] = {
		"skybox/sahara_rt.tga",
		"skybox/sahara_lf.tga",
		"skybox/sahara_up.tga",
		"skybox/sahara_dn.tga",
		"skybox/sahara_bk.tga",
		"skybox/sahara_ft.tga"
	};

	//vao for skybox
	float box[108] = {
		-SIZE,  SIZE, -SIZE,
		-SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		-SIZE,  SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE,  SIZE
	};

	Skybox() {
		model = loadModelFromVertices(box, 108);
		model->textureID = loadCubeMap(files);
	}

	void render(Shader * shader) {
		shader->use();
		//setup texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, model->textureID);

		//draw
		glBindVertexArray(model->vaoID);
		glDrawArrays(GL_TRIANGLES, 0, model->nVertex);
	}
};

#endif