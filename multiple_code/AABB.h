#pragma once
#include "mesh.h"

class AABB
{
public:
	friend class object;
	AABB()
	{
		min_position = glm::vec3(0.0, 0.0, 0.0);
		max_position = glm::vec3(0.0, 0.0, 0.0);
	}
	~AABB(){}
	
	AABB(Mesh mesh,glm::vec3 scale)
	{
		double xMin = mesh.vertData[0].position.x;
		double xMax = mesh.vertData[0].position.x;
		double yMin = mesh.vertData[0].position.y;
		double yMax = mesh.vertData[0].position.y;
		double zMin = mesh.vertData[0].position.z;
		double zMax = mesh.vertData[0].position.z;
		std::vector<Vertex>::const_iterator it;
		for (it = mesh.vertData.begin(); it != mesh.vertData.end(); it++)
		{
			if (xMin>(*it).position.x) xMin = (*it).position.x;
			if (xMax<(*it).position.x) xMax = (*it).position.x;
			
			if (yMin>(*it).position.y) yMin = (*it).position.y;
			if (yMax<(*it).position.y) yMax = (*it).position.y;
			
			if (zMin>(*it).position.z) zMin = (*it).position.z;
			if (zMax<(*it).position.z) zMax = (*it).position.z;
		}

		min_position = glm::vec3(xMin*scale.x-5, yMin*scale.y, zMin*scale.z-5);
		max_position = glm::vec3(xMax*scale.x+5, yMax*scale.y, zMax*scale.z+5);
	}

	AABB(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
	{
		min_position = glm::vec3(xmin, ymin, zmin);
		max_position = glm::vec3(xmax, ymax, zmax);
	}

	bool judgeCollision(AABB& box2)
	{
		if (min_position.x > box2.max_position.x) return false;
		else if (max_position.x < box2.min_position.x) return false;
		else if (min_position.y > box2.max_position.y) return false;
		else if (max_position.y < box2.min_position.y) return false;
		else if (min_position.z > box2.max_position.z) return false;
		else if (max_position.z < box2.min_position.z) return false;
		else return true;
	}
public:
	glm::vec3 min_position, max_position;
};