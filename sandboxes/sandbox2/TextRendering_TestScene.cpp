//// Text Rendering includes
//#include <ft2build.h>
//#include FT_FREETYPE_H
//
//#include <engine/Application.h>
//#include <engine/GL/Shader.h>
//#include <map>
//
//void RenderText(engine::Shader* m_shader, string text, float x, float y, float scale, glm::vec3 colour);
//
//using namespace std;
//using namespace engine;
//
//ECS ecs;
//
//// Holds all state information relevant to a character as loaded using FreeType
//struct Character
//{
//	unsigned int TextureID; // ID handle of the glyph texture
//	glm::ivec2 Size;		// Size of glyph
//	glm::ivec2 Bearing;		// Offset from baseline to left/top of glyph
//	unsigned int Advance;	// Horizontal offset to advance to next glyph
//};
//
//// don't know what difference is with GLchar and char
//map<char, Character> Characters;
//
//unsigned int VAO, VBO;
//
//int main()
//{
//	// Create the window and OpenGL context before creating EngineLib
//	GLFWwindow* window = CreateWindow(800, 600, "Text rendering");
//
//	// Camera
//	Camera cam = Camera(800, 600);
//
//	// Initialize the default engine library
//	EngineLib engine;
//
//	// Window background colour
//	RenderSystem::SetBackgroundColor(68, 154, 141);
//
//	// OpenGL state
//	glEnable(GL_CULL_FACE);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	// compile and setup the shader
//	engine::Shader* m_shader = new engine::Shader("textVertexShader.glsl", "textFragmentShader.glsl");
//	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
//	m_shader->use();
//	glUniformMatrix4fv(glGetUniformLocation(m_shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//
//
//	// Initialize FreeType library
//	FT_Library ft;
//	if (FT_Init_FreeType(&ft))
//	{
//		cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
//		return -1;
//	}
//
//	// Font load
//	FT_Face face;
//	if (FT_New_Face(ft, "assets/fonts/ARIAL.TTF", 0, &face))
//	{
//		cout << "ERROR::FREETYPE: Failed to load font" << endl;
//		return -1;
//	}
//	else
//	{
//		// set size to load glyphs as
//		FT_Set_Pixel_Sizes(face, 0, 48);
//
//		// disable byte-alignment restriction
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//		// load first 128 characters of ASCII set
//		for (unsigned char c = 0; c < 128; c++)
//		{
//			// Load character glyph
//			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
//			{
//				cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
//				continue;
//			}
//
//			// generate texture
//			unsigned int texture;
//			glGenTextures(1, &texture);
//			glBindTexture(GL_TEXTURE_2D, texture);
//			glTexImage2D(
//				GL_TEXTURE_2D,
//				0,
//				GL_RED,
//				face->glyph->bitmap.width,
//				face->glyph->bitmap.rows,
//				0,
//				GL_RED,
//				GL_UNSIGNED_BYTE,
//				face->glyph->bitmap.buffer
//			);
//
//			// set texture options
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//			// now store character for later use
//			Character character = {
//				texture,
//				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
//				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
//				static_cast<unsigned int>(face->glyph->advance.x)
//			};
//
//			Characters.insert(pair<char, Character>(c, character));
//		}
//
//		glBindTexture(GL_TEXTURE_2D, 0);
//	}
//
//	// destroy FreeType once we're finished
//	FT_Done_Face(face);
//	FT_Done_FreeType(ft);
//
//	// Configure VAO/VBO for texture quads
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glBindVertexArray(VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	// Game loop
//	while (!glfwWindowShouldClose(window))
//	{
//		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		{
//			glfwSetWindowShouldClose(window, true);
//		}
//		// Update all engine systems, this usually should go last in the game loop
//		// For greater control of system execution, you can update each one manually
//		engine.Update(&cam);
//
//		RenderText(m_shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
//		RenderText(m_shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
//
//		// OpenGL stuff, goes very last
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	glfwTerminate();
//
//	return 0;
//}
//
//void RenderText(engine::Shader* m_shader, string text, float x, float y, float scale, glm::vec3 colour)
//{
//	// activate corresponding render state
//	m_shader->use();
//	glUniform3f(glGetUniformLocation(m_shader->ID, "textColour"), colour.x, colour.y, colour.z);
//	glActiveTexture(GL_TEXTURE0);
//	glBindVertexArray(VAO);
//
//	// iterate through all characters
//	string::const_iterator c;
//	for (c = text.begin(); c != text.end(); c++)
//	{
//		Character ch = Characters[*c];
//
//		float xpos = x + ch.Bearing.x * scale;
//		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
//
//		float w = ch.Size.x * scale;
//		float h = ch.Size.y * scale;
//
//		// update VBO for each character
//		float vertices[6][4] = {
//			{xpos, ypos + h, 0.0f , 0.0f },
//			{xpos, ypos, 0.0f, 1.0f },
//			{xpos + w, ypos, 1.0f, 1.0f },
//
//			{xpos, ypos + h, 0.0f, 0.0f },
//			{xpos + w, ypos, 1.0f, 1.0f },
//			{xpos + w, ypos + h, 1.0f, 0.0f}
//		};
//
//		// render glyph texture over quad
//		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//
//		// update content of VBO memory
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
//
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//		// render quad
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//
//		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//	}
//
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//}