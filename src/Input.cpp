//
// Created by monter on 6/2/19.
//

#include "Input.hpp"
#include<iostream>

extern bool editorEnable;

bool Input::isKeyPressed(int keyCode)
{
    int state = glfwGetKey(win, keyCode);

    if (!(state == GLFW_PRESS || state == GLFW_REPEAT))
        return false;
    else
        return true;
}

void Input::init(GLFWwindow* window)
{
    win = window;
}

void Input::getMousePos()
{
    double xPos, yPos;
    glfwGetCursorPos(win, &xPos, &yPos);
    std::cout << xPos << std::endl;
}
