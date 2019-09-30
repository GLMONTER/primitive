//
// Created by monter on 3/24/19.
//
#pragma once

#include<GL/glew.h>
#include<fstream>
#include<iostream>

class Shader
{
public:
    bool createShader(const GLenum ShaderType, const char* shaderLoc);
    void useShader(const GLuint shaderProgram);
    enum shaderTypes {VertexShader = 0, FragmentShader = 1, GeometryShader = 2};
	GLuint shaderID;
};
