//
// Created by monter on 6/2/19.
//

#include "Camera.hpp"
#include<iostream>

void Camera::calc(glm::vec3* pos)
{
	if ((pos == nullptr) && !cameraGameEnable)
	{
		view = glm::lookAt(position, position + camFront, camUp);
		projection = glm::perspective(glm::radians(fov), static_cast<float>(widthH) / static_cast<float>(heightH), 0.01f, 1000.0f);

	}
	else
	{
		if (pos != nullptr)
		{
			cameraGameEnable = true;
			view = glm::lookAt(position, *pos, camUp);
			projection = glm::perspective(glm::radians(fov), static_cast<float>(widthH) / static_cast<float>(heightH), 0.01f, 1000.0f);
		}

	}

}
