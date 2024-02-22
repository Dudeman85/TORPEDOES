#include <engine/GL/Model.h>

namespace engine
{
	void Model::LoadModel(std::string path)
	{
		//Load model with Assimp, convert all primitives to triangles and flip texture UVs for OpenGL
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		
		//Make sure the model scene is not null and it imported properly
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}

		//Save the original model's directory
		directory = path.substr(0, path.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);
	}
	
	//Call ProcessNode recursively on every child node of root node
	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		//Process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		//Then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	//Call ProcessMesh on every mesh originating from node
	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Mesh::Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture*> textures;

		//For each vertex in the mesh
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Mesh::Vertex vertex;
			//Process vertex positions
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			//Normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			//Texture Coordinates
			//If the mesh contains texture coordinates
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			//Add the processed vertex to the list
			vertices.push_back(vertex);
		}

		//For each face in the mesh
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			//For each indice in the face
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		//If the mesh has a material
		if (mesh->mMaterialIndex >= 0)
		{
			//Get the scene material vector
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<Texture*> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Texture*> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	//Loads every texture of specific type in a material
	std::vector<Texture*> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture*> textures;
		//For each material texture
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			//Get the location of the texture
			aiString textureLoc;
			mat->GetTexture(type, i, &textureLoc);

			//Check if the texture has already been loaded
			bool skip = false; 
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j]->path.data(), textureLoc.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}

			//If the texture has not been loaded, load it
			if (!skip)
			{
				//Load the texture from location relative to model
				Texture* texture = new Texture((directory + "/" + textureLoc.C_Str()).c_str(), GL_LINEAR, false, false);
				texture->type = typeName;
				texture->path = textureLoc.C_Str();
				textures.push_back(texture);
			}
		}
		return textures;
	}
}