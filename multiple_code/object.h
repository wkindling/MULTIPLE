#pragma once
#include <GL/glew.h>  
#include "model.h"
#include "AABB.h"
#include "mesh.h"

class Object
{
public:
	Model * pModel;
	Mesh pMesh;
	AABB aabbBox;

	Object(){}
	~Object(){}
};

class Scene: public Object
{
public:
	double hp;

	Scene(){}
	~Scene(){}

	Scene(Mesh mesh,glm::vec3 scale)
	{
		pMesh = mesh;
		aabbBox = AABB(mesh, scale);
		hp = 100.0f;
	}
};

class Enemy: public Object
{
public:
	int a[100];
	int count = 0;
	int move_ctrl=0;
	glm::vec3 center;
	double speed=0.01;

	Enemy() {}

	Enemy(Model* pModel) 
	{
		this->pModel = pModel;
		aabbBox = AABB(pModel->meshes[0],glm::vec3(30.0,23.0,30.0));

		for (int i = 0; i <= 99; i++) a[i] = i;
		for (int i = 99; i >= 1; i--)
		{
			int temp=rand()%i;
			a[i] = a[i] + a[temp];
			a[temp] = a[i] - a[temp];
			a[i] = a[i] - a[temp];
		}
	}

	void random_move()
	{
		for (std::vector<Vertex>::iterator i = pModel->meshes[0].vertData.begin(); i != pModel->meshes[0].vertData.end(); i++)
		{
			center += (*i).position;
		}
		center /= pModel->meshes[0].vertData.size(); //计算模型的重心

		std::vector<Vertex>::iterator i;
		glm::vec3 predict;
		GLfloat radius;

		int forward = a[count] % 6;
		if (move_ctrl == 100)
		{
			count++;
			move_ctrl = 0;
		}
		else move_ctrl++;
		if (count == 99) count = 0; //随机确定方向
		
		for (i = pModel->meshes[0].vertData.begin(); i != pModel->meshes[0].vertData.end(); i++) //开始运动
		{
			switch (forward)
			{
				case 0:
					predict = center + glm::vec3(speed, 0, 0);
					radius = predict.x*predict.x + predict.z*predict.z;
					if (predict.y > 3.9 && radius < 50 && predict.y < 10)
					{
						(*i).position += glm::vec3(speed, 0, 0);
						predict = predict * glm::vec3(30, 23, 30);
						aabbBox.max_position = predict + glm::vec3(20, 10, 20);
						aabbBox.min_position = predict - glm::vec3(20, 10, 20);
					}
					else move_ctrl = 100;
					break;
				case 1:
					predict = center + glm::vec3(-speed, 0, 0);
					radius = predict.x*predict.x + predict.z*predict.z;
					if (predict.y > 3.9 && radius < 50 && predict.y < 10)
					{
						(*i).position += glm::vec3(-speed, 0, 0);
						predict = predict * glm::vec3(30, 23, 30);
						aabbBox.max_position = predict + glm::vec3(20, 10, 20);
						aabbBox.min_position = predict - glm::vec3(20, 10, 20);
					}
					else move_ctrl = 100;
					break;
				case 2:
					predict = center + glm::vec3(0, speed, 0);
					radius = predict.x*predict.x + predict.z*predict.z;
					if (predict.y > 3.9 && radius < 50 && predict.y < 10)
					{
						(*i).position += glm::vec3(0, speed, 0);
						predict = predict * glm::vec3(30, 23, 30);
						aabbBox.max_position = predict + glm::vec3(20, 10, 20);
						aabbBox.min_position = predict - glm::vec3(20, 10, 20);
					}
					else move_ctrl = 100;
					break;
				case 3:
					predict = center + glm::vec3(0, -speed, 0);
					radius = predict.x*predict.x + predict.z*predict.z;
					if (predict.y > 3.9 && radius < 50 && predict.y < 10)
					{
						(*i).position += glm::vec3(0, -speed, 0);
						predict = predict * glm::vec3(30, 23, 30);
						aabbBox.max_position = predict + glm::vec3(20, 10, 20);
						aabbBox.min_position = predict - glm::vec3(20, 10, 20);
					}
					else move_ctrl = 100;
					break;
				case 4:
					predict = center + glm::vec3(0, 0, speed);
					radius = predict.x*predict.x + predict.z*predict.z;
					if (predict.y > 3.9 && radius < 50 && predict.y < 10)
					{
						(*i).position += glm::vec3(0, 0, speed);
						predict = predict * glm::vec3(30, 23, 30);
						aabbBox.max_position = predict + glm::vec3(20, 10, 20);
						aabbBox.min_position = predict - glm::vec3(20, 10, 20);
					}
					else move_ctrl = 100;
					break;
				case 5:
					predict = center + glm::vec3(0, 0, -speed);
					radius = predict.x*predict.x + predict.z*predict.z;
					if (predict.y > 3.9 && radius < 50 && predict.y < 10)
					{
						(*i).position += glm::vec3(0, 0, -speed);
						predict = predict * glm::vec3(30, 23, 30);
						aabbBox.max_position = predict + glm::vec3(20, 10, 20);
						aabbBox.min_position = predict - glm::vec3(20, 10, 20);
					}
					else move_ctrl = 100;
					break;
			}
		}

	}
};
