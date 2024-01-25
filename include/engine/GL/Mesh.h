#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <engine/GL/Shader.h>
#include <glm/gtc/type_ptr.hpp>
#include <engine/GL/Texture.h>

namespace engine
{
	class Mesh
	{
	public:
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec2 TexCoords;
		};

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);

		//Mesh data
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture*> textures;

		//Buffer Data
		unsigned int VAO, VBO, EBO;
	};
}