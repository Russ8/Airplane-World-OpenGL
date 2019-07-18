#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "Entity.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	SPEED_ADJ,
	SLOW,
	TOGGLECAM,
	UP
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	Entity * player;
	float distanceFromPlayer = 10;
	float angleAroundPlayer = 0;
	bool thirdCam = true;
	// Camera Attributes
	bool glFast;
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::mat4 lookat;
	// Euler Angles
	float Yaw;
	float Pitch;
	float velocity;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{

		
		velocity = 0;
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		velocity = 0;
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	//updates values of the 3rd person camera
	void updateThirdCam() {

		float distH = cos(glm::radians(Pitch)) * distanceFromPlayer;
		float distV = sin(glm::radians(Pitch)) * distanceFromPlayer;
		Position.y = player->position.y - distV + 4;

		float theta = player->right*(180/ 3.14159265358979323846) + angleAroundPlayer;
		float Xoffset = distH * sin(glm::radians(theta));
		float Zoffset = distH * cos(glm::radians(theta));

		Position.x = player->position.x - Xoffset;
		Position.z = player->position.z - Zoffset;
		Yaw = (90 - (player->right*(180 / 3.14159265358979323846) + angleAroundPlayer)); //sensitivity
	}

	// Pkeyboard input
	void ProcessKeyboard(Camera_Movement direction, float deltaTime, bool toggle = false)
	{
		
		if (direction == UP) {
			Pitch -= deltaTime*45;
		}

		if (direction == FORWARD) {
			player->velocity += Front * 0.1f;
		}
	
		if (direction == TOGGLECAM) {
			if (toggle == 0) {
				distanceFromPlayer = 0;
				updateThirdCam();
			}
			else {
				distanceFromPlayer = 10;
				updateThirdCam();
			}
			thirdCam = toggle;
		}
		if (true)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		if (thirdCam) updateThirdCam();
		updateCameraVectors();
	}

	// mouse input
	void ProcessMouseMovement(float xpos, float ypos, float xoffset, float yoffset, GLboolean constrainPitch = true)
	{

		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		angleAroundPlayer -= 1.5 * xoffset;
		Pitch += 1.5 * yoffset;

		if (thirdCam) {
			updateThirdCam();
		}
		else {
			Yaw += 1.5 * xoffset;
		}

		updateCameraVectors();
	}

	// 3rd person scroll feature
	void ProcessMouseScroll(float yoffset)
	{
		if (thirdCam) {
			distanceFromPlayer -= yoffset;
			updateThirdCam();
		}
		
	}

	//update 
	void update() {
		updateCameraVectors();
		velocity -= 0.001;
		if (velocity < 0.0) velocity = 0.0;
		Position += Front * velocity;
		if (Position.z < 0) Position.z = 0.0;
	}

	float sq(float a) { return a * a; }

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, WorldUp));  
		Up = glm::normalize(glm::cross(Right, Front));
		if (thirdCam == 0) {
			Position.y = player->position.y + 3;
		}

	}


};
#endif
