//
// Created by monter on 6/2/19.
//
#pragma once

#include<GLFW/glfw3.h>
#include<stdexcept>
#include<iostream>
#include<assert.h>

class Window
{
public:
    void createWindow(bool fullscreen);
    GLFWwindow* window;
    unsigned int width, height;
private:
    void glfwFramebufferSizeCallback(GLFWwindow* window, int w, int h);
};
