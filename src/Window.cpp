//
// Created by monter on 6/2/19.
//

#include "Window.hpp"

void GLFWerrorCallback(int error, const char* descrip)
{
    std::cerr<<"GLFW ERROR : "<<descrip<<std::endl;
}

void Window::createWindow(bool fullscreen)
{
    glfwSetErrorCallback(GLFWerrorCallback);
    if(!glfwInit())
        throw std::runtime_error("GLFW FAILED TO INIT");

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    this->height = mode->height;
    this->width = mode->width;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//start window in maxamized mode
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	//for msaa
    glfwWindowHint(GLFW_SAMPLES, 4);

    if(fullscreen)
        window = glfwCreateWindow(static_cast<int>(mode->width), static_cast<int>(mode->height), "primative", glfwGetPrimaryMonitor(), nullptr);
    else
        window = glfwCreateWindow(static_cast<int>(mode->width), static_cast<int>(mode->height), "primative", nullptr, nullptr);


    if(!window)
        throw std::runtime_error("Window Creation Failed.");

    glfwMakeContextCurrent(window);
}
