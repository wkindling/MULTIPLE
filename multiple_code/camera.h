#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include "glm.hpp"
#include "gtc\matrix_transform.hpp"
#include "gtx\string_cast.hpp"
#include "gtc\type_ptr.hpp"
#include <iomanip>     
#include "AABB.h"
#include <vector>

enum Camera_Movement {FORWARD,BACKWARD,LEFT,RIGHT,JUMP,};

class Camera
{
public:
	Camera(glm::vec3 pos = glm::vec3(0.0, 0.0, 2.0),glm::vec3 up = glm::vec3(0.0, 1.0, 0.0),GLfloat yaw = 180.0f, GLfloat pitch = 0.0f) 
		:position(pos), forward(0.0, 0.0, -1.0), viewUp(up),moveSpeed(80.0f), mouse_zoom(45.0f), mouse_sensitivity(0.05f),yawAngle(yaw), pitchAngle(pitch)
	{
		this->updateCameraVectors();
		aabbBox = AABB(-14.76269, 5.23731, 0, 45, -251.855f, -231.855f);
	}
public:
	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->forward, this->viewUp);
	}
	void handleKeyPress(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->moveSpeed * deltaTime;
		GLfloat radius;
		glm::vec3 predict;
		bool collosion_judge = true;
		switch (direction)
		{
		case FORWARD:
			predict.x = this->position.x + this->forward.x*velocity;
			predict.y = this->position.y;
			predict.z = this->position.z + this->forward.z*velocity;
			radius = predict.x*predict.x + predict.z * predict.z;
			aabbBox.max_position = predict + glm::vec3(10, 0, 10);
			aabbBox.min_position = predict - glm::vec3(10, 45, 10);
			if (radius < 60030&&!judgeCollision()) this->position = predict;
			else
			{
				aabbBox.max_position = this->position + glm::vec3(10, 0, 10);
				aabbBox.min_position = this->position - glm::vec3(10, 45, 10);
			}
			break;
		case BACKWARD:
			predict.x = this->position.x - this->forward.x*velocity;
			predict.y = this->position.y;
			predict.z = this->position.z - this->forward.z*velocity;
			radius = predict.x*predict.x + predict.z * predict.z;
			aabbBox.max_position = predict + glm::vec3(10, 0, 10);
			aabbBox.min_position = predict - glm::vec3(10, 45, 10);
			if (radius < 60030&&!judgeCollision()) this->position = predict;
			else
			{
				aabbBox.max_position = this->position + glm::vec3(10, 0, 10);
				aabbBox.min_position = this->position - glm::vec3(10, 45, 10);
			}
			break;
		case LEFT:
			predict.x = this->position.x - this->side.x*velocity;
			predict.y = this->position.y;
			predict.z = this->position.z - this->side.z*velocity;
			radius = predict.x*predict.x + predict.z * predict.z;
			aabbBox.max_position = predict + glm::vec3(10, 0, 10);
			aabbBox.min_position = predict - glm::vec3(10, 45, 10);
			if (radius < 60030&&!judgeCollision()) this->position = predict;
			else
			{
				aabbBox.max_position = this->position + glm::vec3(10, 0, 10);
				aabbBox.min_position = this->position - glm::vec3(10, 45, 10);
			}
			break;
		case RIGHT:
			predict.x = this->position.x + this->side.x*velocity;
			predict.y = this->position.y;
			predict.z = this->position.z + this->side.z*velocity;
			radius = predict.x*predict.x + predict.z * predict.z;
			aabbBox.max_position = predict + glm::vec3(10, 0, 10);
			aabbBox.min_position = predict - glm::vec3(10, 45, 10);
			if (radius < 60030&&!judgeCollision()) this->position = predict;
			else
			{
				aabbBox.max_position = this->position + glm::vec3(10, 0, 10);
				aabbBox.min_position = this->position - glm::vec3(10, 45, 10);
			}
			break;
		default:
			break;
		}
	}
	void handleMouseMove(GLfloat deltax, GLfloat deltay)
	{
		
		deltax *= this->mouse_sensitivity;
		deltay *= this->mouse_sensitivity;

		this->pitchAngle += deltay;
		this->yawAngle += deltax;

		this->normalizeAngle();
		this->updateCameraVectors();
	}
	void handleMouseScroll(GLfloat deltay)
	{
		if (this->mouse_zoom >= 1.0f && this->mouse_zoom <= 45.0f)
			this->mouse_zoom -= this->mouse_sensitivity * deltay;
		if (this->mouse_zoom <= 1.0f)
			this->mouse_zoom = 1.0f;
		if (this->mouse_zoom >= 45.0f)
			this->mouse_zoom = 45.0f;
	}
	void normalizeAngle()
	{
		if (this->pitchAngle > 89.0f)
			this->pitchAngle = 89.0f;
		if (this->pitchAngle < -89.0f)
			this->pitchAngle = -89.0f;
		if (this->yawAngle < 0.0f)
			this->yawAngle += 360.0f;
	}
	void updateCameraVectors()
	{
		glm::vec3 forward;
		forward.x = -sin(glm::radians(this->yawAngle)) * cos(glm::radians(this->pitchAngle));
		forward.y = sin(glm::radians(this->pitchAngle));
		forward.z = -cos(glm::radians(this->yawAngle)) * cos(glm::radians(this->pitchAngle));
		this->forward = glm::normalize(forward);
		
		glm::vec3 side;
		side.x = cos(glm::radians(this->yawAngle));
		side.y = 0;
		side.z = -sin(glm::radians(this->yawAngle));
		this->side = glm::normalize(side);
	}
	bool judgeCollision()
	{
		bool result = false;
		std::vector<AABB>::iterator i;
		for (i = detected_list.begin(); i != detected_list.end(); i++)
		{
			if (aabbBox.judgeCollision(*i))
			{
				result = true;
				break;
			}
		}
		return result;
	}
public:
	glm::vec3 forward,up, side, viewUp, position; 
	GLfloat yawAngle, pitchAngle; 
	GLfloat moveSpeed, mouse_sensitivity, mouse_zoom; 
	AABB aabbBox;
	std::vector<AABB> detected_list;
};

#endif