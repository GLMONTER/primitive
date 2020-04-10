//
// Created by monter on 5/31/19.
//

#include "externalModel.hpp"
#include<assimp/version.h>

//delete all of the meshes allocated on the heap
void externalModel::cleanup()
{
    for (Mesh* mesh : abstractMeshes)
    {
        delete(mesh);
    }
}

void externalModel::deleteBuffers()
{
    for (Mesh* mesh : abstractMeshes)
    {
        mesh->cleanup();
    }
}

//iterate through all of the nodes and get the mesh objects from them
void externalModel::processNodes(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        meshes.push_back(mesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNodes(node->mChildren[i], scene);
    }
}

//loads a model from a file and uses the specified shader for rendering them
bool externalModel::loadModel(std::string file, Shader& vShader, Shader& fShader)
{
    Shader f;
    f.createShader(Shader::shaderTypes::FragmentShader, "rec/collision.frag");
    std::cout << aiGetVersionMajor() << aiGetVersionMinor();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_SortByPType | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ASSIMP ERROR : " << importer.GetErrorString() << std::endl;
        return false;
    }

    processNodes(scene->mRootNode, scene);

    std::cout << file << " loaded" << std::endl;

    //for each aiMesh make a new mesh and push it back into the meshes array for later drawing
    for (aiMesh* m : meshes)
    {
        Mesh* mesh = new Mesh();
        mesh = mesh->init(m, vShader, f, scene);

        mesh->nameH = m->mName.data;
        abstractMeshes.push_back(mesh);

        //give the model the indices of the meshes so we can used glDrawElements
        indicesH = mesh->indices;
        //to display the name of the model in a menu

        modelName = mesh->nameH;

        path = file;
    }
    return true;
}

//iterate through every mesh in the model and call the render function on the meshes
void externalModel::draw(glm::vec3 pos, glm::vec3 euler, glm::vec3 scale, Camera c)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (Mesh* mesh : abstractMeshes)
    {
        mesh->render(pos, euler, scale, c);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}
