//
// Created by monter on 5/31/19.
//
#pragma once

#include<Mesh.hpp>


class externalModel
{
public:
    void cleanup();
    void draw(glm::vec3 pos, glm::vec3 euler, glm::vec3 scale, Camera c);
    bool loadModel(std::string file, Shader& vShader, Shader& fShader);
    void processNodes(aiNode* node, const aiScene* scene);
    void deleteBuffers();
    std::vector<unsigned int> indicesH;
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    glm::quat Quatern;
    glm::vec3 EulerAngle = glm::vec3(0, 0, 0);
    std::string modelName = "Model";
    std::string path = "path";
    unsigned int id;

    std::vector<aiMesh*> meshes;
    std::vector<Mesh*> abstractMeshes;
};
