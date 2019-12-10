//
// Created by monter on 6/2/19.
//

#include "Input.hpp"
#include<iostream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        std::cout << "yes";
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
