#pragma once
#include<glm/glm.hpp>

class Collider
{
	enum colliderType {COL_BOX_COLLIDER, COL_SPH_COLLIDER};

	glm::vec3 colPosition = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	glm::vec3 EulerAngle = glm::vec3(0, 0, 0);
public:
	bool isNull = true;
};

