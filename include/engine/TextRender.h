#include <engine/Font.h>
#include <engine/Transform.h>
#include <engine/GL/Camera.h>

namespace engine
{
	// TextRenderer component
	struct TextRenderer : ecs::Component
	{
		Font* font;
		std::string text = "";
		Vector3 offset;
		Vector3 rotation;
		Vector3 scale = Vector3(1);
		Vector3 color;
		bool uiElement = false;
	};

	// TextRenderSystem requires components TextRenderer component and transform component
	class TextRenderSystem : public ecs::System
	{
	public:
		// Constructor
		TextRenderSystem()
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

		void Update(Camera* cam)
		{
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

				//Create the model matrix
				glm::mat4 model = glm::mat4(1.0f);
				//Position
				model = glm::translate(model, transform.position.ToGlm());
				//X, Y, Z euler rotations
				model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
				//Scale
				model = glm::scale(model, transform.scale.ToGlm());

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
					//Give the shader a constant view matrix
					glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

					//Give the shader a constant projection matrix
					glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
				}

				glUniform3f(glGetUniformLocation(m_shader->ID, "textColor"), textRenderer.color.x, textRenderer.color.y, textRenderer.color.z);
				glActiveTexture(GL_TEXTURE0);
				glBindVertexArray(textRenderer.font->VAO);

				// Renders text one letter at a time
				string::const_iterator c;
				float x = textRenderer.offset.x;
				float y = textRenderer.offset.y;
				for (c = textRenderer.text.begin(); c != textRenderer.text.end(); ++c)
				{
					Character ch = textRenderer.font->characters[*c];

					float xpos = x + ch.Bearing.x * textRenderer.scale.x;
					float ypos = y - (ch.Size.y - ch.Bearing.y) * textRenderer.scale.y;

					float w = ch.Size.x * textRenderer.scale.x;
					float h = ch.Size.y * textRenderer.scale.y;

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
					x += (ch.Advance >> 6) * textRenderer.scale.x;
				}

				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

	private:
		Shader* m_shader;
	};
}