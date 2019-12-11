//
// Created by monter on 6/2/19.
//
#pragma once

#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
extern float deltaTime;


class Input
{
public:
    bool isKeyPressed(int keyCode);
    void init(GLFWwindow* window);
    void getMousePos();
private:
    GLFWwindow* win;
};