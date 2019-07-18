#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <map>
#include <cstdio>
#include <ctime>
#include "Entity.h"
#include "GeometricCull.h"
#include "skybox.h"
#include "Light.h"
#include "File.h"
#include "QuadTree.h"
#include "helper.h"
#include "Particle.h"
#include "shader.h"
#include "Camera.h"


void MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
bool cullFaces = false;
float mouseX = 0;
float mouseY = 0;

//Player
Entity * player = new Entity(glm::vec3(0000.0, 0.0, 0000.0));


// camera
Camera * camera = new Camera(glm::vec3(2000.0, 80.0f, 2000.0));
bool thirdCam = true;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
int renderDistance = 1000;

//sun
Light * light = new Light(glm::vec3(0.8, 0.8, 0.8), glm::vec3(100.0, 400000.0, 100.0));

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
bool toggle = 0;
int timer = 0;

//skyColor
float skyR = 0.8;
float skyG = 0.8;
float skyB = 0.8;

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	//Models
	std::vector<Model *> plane;
	try {
		plane = loadModelFromObj("plane (4).obj", "Texture.tga");
	}
	catch (int x) {
			std::cout << "cant load obj\n";
	}


	//setting up objects
	//player
	player->models = plane;
	player->scaleFactor = 0.005;
	//smoke 
	std::vector<Particle * > particleList;
	float smoke[] = { -0.1, -0.1, 0.0, 0.1, -0.1, 0.0, -0.1, 0.1, 0.0,  0.1, -0.1, 0.0, 0.1, 0.1, 0.0, -0.1, 0.1, 0.0};
	Model * smoke_model = loadModelFromVertices(smoke, 18);
	//frustrum culler
	FrustrumCuller * culler = new FrustrumCuller();
	//other stuff
	camera->player = player;
	Skybox * skybox = new Skybox();
	glm::vec3 lightPos(0.0f, 80000.0f, 0.0f);

	//setup shaders
	Shader * shader = new Shader("vertex.vs", "fragment.fs");
	Shader * particleShader = new Shader("particle.vs", "particle.fs");
	Shader * skyboxShader = new Shader("skybox.vs", "skybox.fs");

	//setup a chunk of terrain
	QuadTree * chunk = new QuadTree();
	chunk->construct_tree(glm::vec3(0.0, 0.0, 0.0));
	chunk->camera = camera;

	//gl options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback((GLDEBUGPROC)MessageCallback, 0);

	while (!glfwWindowShouldClose(window))
	{
		//std::cout << player
		glEnable(GL_DEPTH_TEST);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glClearColor(skyR, skyG, skyB, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//timer
		if (timer != 0) timer--;
		//input
		processInput(window);
		
		//update logic
		player->checkCollisions(chunk->findClosestY(player->position.x, player->position.z));
		player->update();
		camera->updateThirdCam();

		//update particles
		for (int i = 0; i < particleList.size(); i++) {
			if (particleList[i]->update()) {
				Particle * particle = particleList[i];
				particleList.erase(particleList.begin() + i);
				particle->~Particle();
			}
		}
		
		//set face culler
		glm::vec3 temp = (camera->Position) + camera->Front;
		culler->setCamInternals((camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100000.0f);
		culler->setCamDef(camera->Position,  temp , camera->Up);
		
		//generate smoke
		for (int i = 0; i < 40; i++) {
			Particle * smokeParticle = new Particle(player->position + glm::vec3(0.5, 1.3, 0.8), (player->velocity * 0.8f) + glm::vec3(float(rand() % 10) / 50, float(rand() % 10) / 50, float(rand() % 10) / 50), 0.0, 50, 0.1, camera);
			smokeParticle->model = smoke_model;
			particleList.push_back(smokeParticle);
		}
		
		//setup shader (general use shader)
		shader->use();
		shader->setVec3("skyColor", glm::vec3(skyR, skyG, skyB));
		shader->setVec3("lightColor", light->color);
		shader->setVec3("lightPosition", lightPos);
		shader->setFloat("shineDamper", 1);
		shader->setFloat("reflectivity", 0.5);
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100000.0f);
		shader->setMat4("projection", projection);
		camera->update();
		
		glm::mat4 view = camera->GetViewMatrix();
		shader->setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, chunk->root->textureID);
		

		chunk->render(shader, light, chunk, culler, cullFaces);

		//draw pine trees, stumps
		for (int i = 0; i < chunk->entityList.size(); i++) {
			if (glm::length(chunk->entityList.at(i)->position - camera->Position) < renderDistance) {
				chunk->entityList.at(i)->render(shader);
			}
		}

		//render player
		player->render(shader);

		//render smoke particles
		particleShader->use();
		particleShader->setMat4("projectionMatrix", projection);
		skyboxShader->use();
		skyboxShader->setMat4("projectionMatrix", projection);
		view[3][0] = 0.0f;
		view[3][1] = 0.0f;
		view[3][2] = 0.0f;


		//render skybox
		skyboxShader->setMat4("viewMatrix", view);
		skybox->render(skyboxShader);

		for (int i = 0; i < particleList.size(); i++) {
			particleList[i]->render(particleShader);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
	}
		glfwTerminate();
		return 0;
	
}

//setup input
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
		//close program
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//up
		camera->ProcessKeyboard(UP, 0.05);
		player->up += 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//down
		camera->ProcessKeyboard(UP, -0.05);
		player->up -= 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//left
		player->spin -= 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//right
		player->spin += 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		//speed
		player->speed += 0.01;
		camera->ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		//face culling
		if (timer == 0) {
			cullFaces = !cullFaces;
			timer = 5;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		//toggle cam
		if (timer == 0) {

			if (thirdCam == 0) {
				thirdCam = 1;
				camera->ProcessKeyboard(TOGGLECAM, 0.0, true);
			}
			else if (thirdCam == 1) {
				thirdCam = 0;
				camera->ProcessKeyboard(TOGGLECAM, 0.0, false);
			}
			timer = 5;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		//toggle wirefram mode
		if (timer == 0) {
			toggle = !toggle;
			timer = 10;
		}
		if (toggle == 1) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (toggle == 0) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	//mouse input
	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		mouseX = xpos;
		mouseY = ypos;
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; 

		lastX = xpos;
		lastY = ypos;

		camera->ProcessMouseMovement(xpos, ypos, xoffset, yoffset);
	}

	//scroll wheel input
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera->ProcessMouseScroll(yoffset);
	}
