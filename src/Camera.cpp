//
// Created by monter on 6/2/19.
//

#include "Camera.hpp"
#include<iostream>
void Camera::calc()
{
	view = glm::lookAt(position, position + camFront, camUp);
    projection = glm::perspective(glm::radians(fov), static_cast<float>(widthH)/static_cast<float>(heightH), 0.01f, 10000.0f);
}
