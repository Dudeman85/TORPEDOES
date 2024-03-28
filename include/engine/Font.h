#pragma once

// FreeType includes
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <engine/GL/Shader.h>
#include <engine/GL/Camera.h>
#include <map>
#include <exception>

///Struct to store data about the characters
struct Character
{
	///Automatic ID for every character
	unsigned int TextureID;
	///The size of the characters
	glm::ivec2 Size;
	///The rotation of the characters
	glm::ivec2 Bearing;
	unsigned int Advance;
};

namespace engine
{
	///Class to create and store data for the font
	class Font
	{
	public:
		/// Constructor
		Font(std::string filepathname, FT_Long face_index, FT_UInt pixel_width, FT_UInt pixel_height);

		// Destructor
		~Font();

		unsigned int VAO, VBO;
		///A map of all the characters
		std::map<GLchar, Character> characters;

	private:
		// VAO & VBO function
		void config();
		// Loading function
		void load();
		
		unsigned int texture;

		FT_Library ft;
		FT_Face face;
	};
}