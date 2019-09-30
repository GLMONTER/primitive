//
// Created by monter on 3/25/19.
//
#pragma once

#include<GL/glew.h>
#include<iostream>

class TextureC
{
public:
    static unsigned int loadTexture(const std::string& fileLocation);
};
