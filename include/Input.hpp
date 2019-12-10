//
// Created by monter on 6/2/19.
//
#pragma once

#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
class Input
{
public:
    void init(GLFWwindow* window);
    void getMousePos();
private:
    GLFWwindow* win;
};