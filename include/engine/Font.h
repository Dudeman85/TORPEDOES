#pragma once

// FreeType includes
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <engine/GL/Shader.h>
#include <engine/GL/Camera.h>
#include <engine/Constants.h>
#include <map>
#include <exception>

using namespace std;
using namespace engine;

struct Character
{
	unsigned int TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

namespace engine
{
	class Font
	{
	public:
		// Constructor
		Font(std::string filepathname, FT_Long face_index, FT_UInt pixel_width, FT_UInt pixel_height);

		// Destructor
		~Font();

		unsigned int VAO, VBO;
		map<GLchar, Character> characters;

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