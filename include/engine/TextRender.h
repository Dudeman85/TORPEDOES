#include <engine/Font.h>
#include <engine/Transform.h>
#include <engine/GL/Camera.h>
// Include Window.h
#include "../include/engine/GL/Window.h"
#include <stdio.h>

namespace engine
{
	/// TextRenderer component
	ECS_REGISTER_COMPONENT(TextRenderer)
	struct TextRenderer
	{
		///The font of the text
		Font* font;
		///The text that is printed
		std::string text = "";
		///Location of the text on the screen
		Vector3 offset;
		///Rotation of the text
		Vector3 rotation;
		///Size of the text
		//Vector2 charRes;
		// Old
		Vector3 scale = Vector3(1);
		///Color of the text
		Vector3 color = Vector3(0);
		///Bool to turn on the ui elements
		bool uiElement = false;
	};

	/// TextRenderSystem requires components TextRenderer component and transform component
	ECS_REGISTER_SYSTEM(TextRenderSystem, Transform, TextRenderer)
	class TextRenderSystem : public ecs::System
	{
	public:
		///Initialize the shaders
		void Init()
		{
			m_shader = new Shader(
				R"(
					#version 330 core
					layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
					out vec2 TexCoords;
					uniform mat4 projection;
					uniform mat4 view;
					uniform mat4 model;
					
					void main()
					{
						gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
						TexCoords = vertex.zw;
					})",
				R"(
					#version 330 core
					in vec2 TexCoords;
					out vec4 color;
					uniform sampler2D text;
					uniform vec3 textColor;

					void main()
					{
						vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
						color = vec4(textColor, 1.0) * sampled;
					})",
				false);
		}

		///Call this every frame
		void Update(Camera* cam)
		{
			// Main windows size variable.
			windowSize = engine::GetMainWindowSize();

			for (auto const& entity : entities)
			{
				Transform& transform = ecs::GetComponent<Transform>(entity);
				TextRenderer& textRenderer = ecs::GetComponent<TextRenderer>(entity);
				if (!textRenderer.font)
				{
					printf("ERROR: No font given!");
					continue;
				}
				m_shader->use();

				//Create the model matrix, this is the same for each mesh so it only needs to be done once
				glm::mat4 model = TransformSystem::GetGlobalTransformMatrix(entity);

				//Give the shader the model matrix
				unsigned int modelLoc = glGetUniformLocation(m_shader->ID, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				//Get the view and projection locations
				unsigned int viewLoc = glGetUniformLocation(m_shader->ID, "view");
				unsigned int projLoc = glGetUniformLocation(m_shader->ID, "projection");

				if (!textRenderer.uiElement)
				{

					//Give the shader the camera's view matrix
					glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));

					//Give the shader the camera's projection matrix
					glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
				}
				else
				{
					glDisable(GL_DEPTH_BUFFER_BIT);
					//Give the shader a constant view matrix
					glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

					//Give the shader a constant projection matrix
					glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
				}

				glUniform3f(glGetUniformLocation(m_shader->ID, "textColor"), textRenderer.color.x, textRenderer.color.y, textRenderer.color.z);
				glActiveTexture(GL_TEXTURE0);
				glBindVertexArray(textRenderer.font->VAO);

				// Position character resolution values to the font
				// might not be a good idea to set in update...
				/*
				printf("Resolution X-value: %.2f Resolution Y-value: %.2f\n", textRenderer.charRes.x, textRenderer.charRes.y);
  				if (textRenderer.charRes.x == oldCharRes.x && textRenderer.charRes.y == oldCharRes.y)
				{
					printf("X value: %.2f Y value: %.2f \n", textRenderer.charRes.x, textRenderer.charRes.y);
					break;
					
				}
				else
				{
					oldCharRes = textRenderer.charRes;
					textRenderer.font->SetResolution(textRenderer.charRes.x, textRenderer.charRes.y);
					printf("Character resolution: %.2f x %.2f \n", textRenderer.charRes.x, textRenderer.charRes.y);
				}
				*/
				
				

				// Scale text relative to window size
				scaleFactor = std::min(windowSize.x, windowSize.y) / resolution;
				scaledTextSize = textRenderer.scale * scaleFactor / 10;

				// Renders text one letter at a time
				std::string::const_iterator c;
				float x = textRenderer.offset.x;
				float y = textRenderer.offset.y;
				for (c = textRenderer.text.begin(); c != textRenderer.text.end(); ++c)
				{
					Character ch = textRenderer.font->characters[*c];
					
					
					// Old rendering code
					//float xpos = x + ch.Bearing.x * textRenderer.scale.x;
					//float ypos = y - (ch.Size.y - ch.Bearing.y) * textRenderer.scale.y;

					// New rendering code
					float xpos = x + ch.Bearing.x * scaledTextSize.x;
					float ypos = y - (ch.Size.y - ch.Bearing.y) * scaledTextSize.y;
					/*
					* // New rendering code
				float xpos = x + ch.Bearing.x * transform.scale.x;
				float ypos = y - (ch.Size.y - ch.Bearing.y) * transform.scale.y;
					*/
					

					// Old renderin code
					//float w = ch.Size.x * textRenderer.scale.x;
					//float h = ch.Size.y * textRenderer.scale.y;

					// New rendering code
					float w = ch.Size.x * scaledTextSize.x;
					float h = ch.Size.y * scaledTextSize.y;
					/*
					* // New rendering code
				float w = ch.Size.x * transform.scale.x;
				float h = ch.Size.y * transform.scale.y;
					*/

					float vertices[6][4] = {
						{xpos, ypos + h, 0.0f, 0.0f},
						{xpos, ypos, 0.0f, 1.0f},
						{xpos + w, ypos, 1.0f, 1.0f},

						{xpos, ypos + h, 0.0f, 0.0f},
						{xpos + w, ypos, 1.0f, 1.0f},
						{xpos + w, ypos + h, 1.0f, 0.0f}
					};

					glBindTexture(GL_TEXTURE_2D, ch.TextureID);
					glBindBuffer(GL_ARRAY_BUFFER, textRenderer.font->VBO);
					glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					// Old rendering code
					//x += (ch.Advance >> 6) * textRenderer.scale.x;
					// New rendering code
					x += (ch.Advance >> 6) * scaledTextSize.x;
					//x += (ch.Advance >> 6) * transform.scale.x;
					
				}

				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);

				glEnable(GL_DEPTH_BUFFER_BIT);
			}
		}

	private:
		Shader* m_shader;
		// Window variable
		Vector2 windowSize;
		// Text scaleFactor variable
		float scaleFactor = 0.0f;
		// Text scaled based on the window size variable
		Vector3 scaledTextSize = Vector3(0);
		// Window resolution
		float resolution = 800.0f;
		// Compare value for charRes
		Vector2 oldCharRes = Vector2(0.0f, 0.0f);
	};
}