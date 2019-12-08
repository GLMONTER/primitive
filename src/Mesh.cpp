//
// Created by monter on 3/25/19.
//

#include "Mesh.hpp"


static std::string dir;
std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		std::string final = "rec/" + dir + str.data;
        size_t position;
        while((position = final.find("\\")) != std::string::npos)
        {
            final.replace(position, 1, "/");
        }

		std::string FINAL = final;
        texture.id = TextureC::loadTexture(FINAL);


		texture.type = typeName;
		texture.path = str.C_Str();
		textures.push_back(texture);
	}
	return textures;
}
void Mesh::loadVec(aiMesh* mesh, const aiScene* scene)
{
    //get all of the vertices from the mesh
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
		Vertex vertex;
		glm::vec3 vector;

		//set the position for the vertex.
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		//set the normals for the vertex
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		else
			std::cout << "no normals" << std::endl;

		//set the texture coords for the vertex
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TextureCoords = vec;
		}
		else
        {
			vertex.TextureCoords = glm::vec2(0.0f, 0.0f);
        }
		vertices.push_back(vertex);
    }
    //get all of the indices for the mesh
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
		//for every face push back all of the indices for indexed rendering.
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

	if (mesh->HasTextureCoords(0))
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}
}

Mesh* Mesh::init(aiMesh* m, Shader &vShader, Shader &fShader, const aiScene* s)
{
    loadVec(m, s);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //load a buffer with verts
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    //for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);

	//for normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    //for normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoords));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    shaderProgram = glCreateProgram();

    vShader.useShader(shaderProgram);
    fShader.useShader(shaderProgram);

    glLinkProgram(shaderProgram);

    GLint program_linked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(shaderProgram, 1024, &log_length, message);
        std::cerr<<"shader program failed to link : "<<message<<std::endl;
    }
	else
	{
		//will use this if and when each mesh has it's own shader.

		/*detach and delete the shaders that were used to create the shader program as they are not needed any longer.*/
		/*
		glDetachShader(shaderProgram, vShader.shaderID);
		glDetachShader(shaderProgram, fShader.shaderID);

		glDeleteShader(vShader.shaderID);
		glDeleteShader(fShader.shaderID);
		*/
	}
    matrixID = static_cast<GLuint>(glGetUniformLocation(shaderProgram, "MVP"));
    return this;
}

void Mesh::cleanup()
{
	glDeleteProgram(shaderProgram);

	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &VAO);

	//delete all of the texture objects associated with the mesh
	for (Texture t : textures)
	{
		glDeleteTextures(1, &t.id);
	}
}

void Mesh::render(glm::vec3 pos, glm::vec3 euler, glm::vec3 scale, Camera c)
{
	glBindVertexArray(VAO);

    glUseProgram(shaderProgram);

	//I don't think we need glActiveTexture because there is only 1 texture per mesh at the moment
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        //glActiveTexture(GL_TEXTURE0 + i);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse1"), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

   // glActiveTexture(GL_TEXTURE0);

    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(euler.x), glm::vec3(1,0,0));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(euler.y), glm::vec3(0,1,0));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(euler.z), glm::vec3(0,0,1));

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), pos);

    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

    glm::mat4 model = scaleMatrix * rotationMatrix * translationMatrix;

	
    glm::mat4 mvp = c.projection * c.view * model;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(mvp));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glBindVertexArray(0);
}
