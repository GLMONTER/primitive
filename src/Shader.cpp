//
// Created by monter on 3/24/19.
//

#include "Shader.hpp"
#include<string>
bool Shader::createShader(const GLenum ShaderType, const char *shaderLoc)
{
    std::ifstream File(shaderLoc);
    if(!File) {
        std::cerr << "Shader file failed to load : " << shaderLoc << std::endl;
        return false;
    }

    std::string temp;
    std::string final;
    const char* cFinal;
    while(std::getline(File, temp))
    {
        final += temp + "\n";
    }
    File.close();

    cFinal = final.c_str();

    if(ShaderType == shaderTypes::VertexShader)
        shaderID = glCreateShader(GL_VERTEX_SHADER);
    else if(ShaderType == shaderTypes::FragmentShader)
        shaderID = glCreateShader(GL_FRAGMENT_SHADER);
    else if(ShaderType == shaderTypes::GeometryShader)
        shaderID = glCreateShader(GL_GEOMETRY_SHADER);

    glShaderSource(shaderID, 1, &cFinal, nullptr);
    glCompileShader(shaderID);

    GLint shaderCompiled;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(shaderID, 1024, &log_length, message);
        if (ShaderType == shaderTypes::VertexShader)
        {
            std::cerr << "Vertex Shader Failed to Compile : " << message << std::endl;
        }
        else
        if (ShaderType == shaderTypes::FragmentShader)
        {
                std::cerr << "Fragment Shader Failed to Compile : " << message << std::endl;
        }
        else
        if (ShaderType == shaderTypes::GeometryShader)
        {
            std::cerr << "Geometry Shader Failed to Compile : " << message << std::endl;
        }
        
        return false;
    }
    return true;
}

void Shader::useShader(const GLuint shaderProgram)
{
    glAttachShader(shaderProgram, shaderID);
}
