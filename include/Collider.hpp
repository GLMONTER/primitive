#pragma once
#include<glm/glm.hpp>
#include<externalModel.hpp>

class Collider
{
public:
	enum colliderType {COL_BOX_COLLIDER, COL_SPH_COLLIDER};

	glm::vec3 colPosition = glm::vec3(0, 0, 0);
	glm::vec3 colScale = glm::vec3(1, 1, 1);
	glm::vec3 EulerAngle = glm::vec3(0, 0, 0);
	externalModel collisionModel;
	int id = 0;
	bool isNull = true;

};

