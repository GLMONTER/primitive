//
// Created by monter on 3/25/19.
//
#define STB_IMAGE_IMPLEMENTATION
#include<stb/stb_image.h>
#include "Texture.hpp"
#include<string>

unsigned int TextureC::loadTexture(const std::string& fileLocation)
{
    unsigned int texture;

    //generate a texture variable and bind it
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //give the texture parameters to follow
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;

    //load the texture
    unsigned char* data = stbi_load(fileLocation.c_str(), &width, &height, &nrChannels, 0);

    //if the image failed to load
    if(!data)
    {
        std::cerr<<"Texture Failed to Load : "<<fileLocation<<std::endl;
        stbi_image_free(data);
        return 0;
    }

    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    //actually giving opengl the texture data to the currently bound texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);


    //generate mip maps for the currently bound texture.
    glGenerateMipmap(GL_TEXTURE_2D);

    //delete the loaded image data from stb as it is no longer needed
    stbi_image_free(data);
    return texture;
}
