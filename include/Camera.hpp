//
// Created by monter on 6/2/19.
//
#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
//for glm::value_ptr when using uniforms
#include<glm/gtc/type_ptr.hpp>

class Camera {
public:
    unsigned int widthH = 1;
    unsigned int heightH = 1;

    float fov = 60.0f;
    glm::vec3 DcamFront = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 20.0f);
	glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 projection = glm::perspective(fov, (float)widthH/(float)heightH, 0.01f, 1000.0f);
    //position, where to look(the front of the camera), the up vector
    glm::mat4 view = glm::lookAt(position, position + camFront, camUp);

    void calc();
    
};
