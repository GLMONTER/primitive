//
// Created by monter on 5/31/19.
//

#include "Model.hpp"

//delete all of the meshes allocated on the heap
void Model::cleanup()
{
    for(Mesh* m : mm)
    {
        delete(m);
    }
}


void Model::deleteBuffers()
{
	for (Mesh* m : mm)
	{
		m->cleanup();
	}
}

//iterate through all of the nodes and get the mesh objects from them
void Model::processNodes(aiNode* node, const aiScene* scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        
        meshes.push_back(mesh);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNodes(node->mChildren[i], scene);
    }
}

//loads a model from a file and uses the specified shader for rendering them
bool Model::loadModel(std::string file, Shader &vShader, Shader &fShader)
{
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
    for(aiMesh* m : meshes)
    {
        Mesh* mesh = new Mesh();
        mesh = mesh->init(m, vShader, fShader, scene);

        mesh->nameH = m->mName.data;
        mm.push_back(mesh);

        //give the model the indices of the meshes so we can used glDrawElements
        indicesH = mesh->indices;
        //to display the name of the model in a menu

        modelName = mesh->nameH;

		path = file;
    }
    return true;
}

//iterate through every mesh in the model and call the render function on the meshes
void Model::draw(glm::vec3 pos, glm::vec3 euler, glm::vec3 scale, Camera c)
{
    for(Mesh* mmm : mm)
    {
        mmm->render(pos, euler, scale, c);
    }
}
