#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <engine/GL/Shader.h>
#include <glm/gtc/type_ptr.hpp>
#include <engine/GL/Texture.h>

namespace engine
{
	///Class for creating and storing meshes
	class Mesh
	{
	public:
		///Struct to store vertex data
		struct Vertex
		{
			///Store the vertexes position
			glm::vec3 Position;
			///Store the vertexes normals
			glm::vec3 Normal;
			///Store the vertexes texture coordinates
			glm::vec2 TexCoords;
		};
		///Vector to store mesh data
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);

		//Mesh data
		///Store the meshes vertices
		std::vector<Vertex> vertices;
		///Store the meshes indices
		std::vector<unsigned int> indices;
		///Store the meshes textures
		std::vector<Texture*> textures;

		//Buffer Data
		unsigned int VAO, VBO, EBO;
	};
}