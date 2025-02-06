#include <engine/Font.h>
#include <engine/Transform.h>
#include <engine/GL/Camera.h>
// Include Window.h
#include "../include/engine/GL/Window.h"

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

				// Renders text one letter at a time
				std::string::const_iterator c;
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

				glEnable(GL_DEPTH_BUFFER_BIT);
			}
		}

	private:
		Shader* m_shader;
<<<<<<< HEAD
		// Temporal Window variables
		GLFWwindow* window;
		int width = 1920;
		int height = 1080;
=======
		// Window size variables
<<<<<<< HEAD
	
=======
>>>>>>> d07ae8b4c17cddcb3cd93e0d51f1e25e97f3de57
>>>>>>> 3dd2c92a582637f775b10104b390cf044ae66fa7
	};
}