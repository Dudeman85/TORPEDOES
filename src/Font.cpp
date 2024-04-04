#include <engine/Font.h>
#include <engine/Constants.h>

using namespace std;
using namespace engine;

Font::Font(std::string filepathname, FT_Long face_index, FT_UInt pixel_width, FT_UInt pixel_height)
{
	try
	{
		if (FT_Init_FreeType(&ft))
		{
			throw runtime_error("Could not init FreeType Library");
		}
		if (FT_New_Face(ft, (assetPath + filepathname).c_str(), face_index, &face))
		{
			throw runtime_error("Failed to load font from: " + assetPath + filepathname);
		}

		else
		{
			FT_Set_Pixel_Sizes(face, pixel_width, pixel_height);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			load();
		}
	}
	catch (const exception& e)
	{
		cerr << "ERROR::FREETYPE: " << e.what() << endl;
	}
}

Font::~Font()
{
}

void Font::config()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Font::load()
{
	for (unsigned char c = 0; c < 128; ++c)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
			continue;
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};

		characters.insert(pair<char, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	glBindTexture(GL_TEXTURE_2D, 0);

	config();
}
