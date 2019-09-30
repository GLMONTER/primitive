//
// Created by monter on 6/2/19.
//
#pragma once

#include<GL/glew.h>

#include<vector>
#include<thread>

#include<Camera.hpp>
#include<Model.hpp>
#include<Sound.hpp>
#include<Window.hpp>
#include<Input.hpp>
#include<Shader.hpp>

#include<glm/gtx/quaternion.hpp>
#include<glm/gtc/quaternion.hpp>

#include<imgui/imgui.h>
#include<imgui/examples/imgui_impl_glfw.h>
#include<imgui/examples/imgui_impl_opengl3.h>

class Core
{
public:
    void Init();
    void renderLoop();
    void updateCamera(unsigned int wid, unsigned int hei);
    void drawMenu();

    unsigned int idCounter = 0;
private:
    void loadOrUnloadModel(float (&selectedPos)[3], float (&selectedRot)[3], float (&selectedScl)[3], bool load);
    void loadClear(char (&cur)[1024]);
	static void glfwFramebufferSizeCallback(GLFWwindow* wind, int w, int h);

	//framebuffer object
	GLuint FBO;

	GLuint depthrenderbuffer;
	
	//framebuffer texture
	GLuint FBTexture;

    Input input;
    SoundSystem soundSystem;
    Window window;

    Shader defaultVert;
    Shader defaultFrag;

    std::vector<Shader> shaders;
    Camera mainCamera;
    std::vector<Model> models;
    std::vector<std::string> modelNames;
};