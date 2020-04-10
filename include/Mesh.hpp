//
// Created by monter on 3/25/19.
//
#pragma once

#include<GL/glew.h>

#include<string>
#include<vector>

#include<Camera.hpp>
#include"Shader.hpp"
#include"Texture.hpp"

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


//dont modify
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TextureCoords;
    glm::vec3 staticPosition;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
    Mesh* init(aiMesh* m, Shader &vShader, Shader &fShader, const aiScene* s);
    void loadVec(aiMesh* mesh, const aiScene* scene);
    void render(glm::vec3 pos, glm::vec3 euler, glm::vec3 scale, Camera c);
	void cleanup();
    std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
    std::string nameH = "Mesh";
    unsigned int faceCount = 0;
    glm::vec3 offset;
    glm::vec3 Scale = glm::vec3(1, 1, 1);
private:
    std::vector<float> verts;
    std::vector<aiMesh*> meshes;
    GLuint shaderProgram;
    GLuint VAO, VBO;
    GLuint matrixID;
};