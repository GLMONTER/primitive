//
// Created by monter on 6/2/19.
//

#include "Input.hpp"
#include<iostream>

bool Input::isKeyPressed(int keyCode)
{
    auto state = glfwGetKey(win, keyCode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

void Input::init(GLFWwindow *window)
{
    win = window;
}

void Input::getMousePos()
{
    double xPos, yPos;
    glfwGetCursorPos(win, &xPos, &yPos);
    std::cout<<xPos<<std::endl;
}
