//Engine
#include <engine/ECS.h>
#include <engine/Transform.h>
#include <engine/GL/Model.h>

namespace engine
{
	///3D Model Renderer component
	ECS_REGISTER_COMPONENT(ModelRenderer)
	struct ModelRenderer
	{
		///Stores vertex data
		Model* model;
		///Stores shader data
		Shader* shader;
		///Stores texture data
		Model* Texture;
	};

	///3D Model Render System, requires Transform and ModelRenderer
	ECS_REGISTER_SYSTEM(ModelRenderSystem, Transform, ModelRenderer)
	class ModelRenderSystem : public ecs::System
	{
	public:
		///Initialize the shaders
		void Init()
		{
			//The default 3D model shader with bling-phong lighting
			defaultShader = new Shader(
				R"(
				#version 330 core
				layout(location = 0) in vec3 aPos;
				layout(location = 1) in vec3 aNormal;
				layout(location = 2) in vec2 aTexCoords;
				
                out vec3 FragPos;
                out vec3 Normal;
                out vec2 TexCoords;

				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;

				void main()
				{
					TexCoords = aTexCoords;
                    FragPos = vec3 (model * vec4(aPos,1.0));
                    Normal = mat3(transpose(inverse(model))) *aNormal;
 
					gl_Position = projection * view * model * vec4(aPos, 1.0);
				}
				)",
				R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 TexCoords;
                in vec3 Normal;
                in vec3 FragPos;

                uniform sampler2D texture_diffuse1;
				uniform vec3 lightPos;
                uniform vec3 lightColor;
                uniform vec3 viewPos;
 
				void main()
				{    
					vec3 objectColor = texture(texture_diffuse1, TexCoords).xyz;
					float ambientStrength = 0.3;
					vec3 ambient = ambientStrength * lightColor;
                    
                    vec3 norm = normalize(Normal);
                    vec3 lightDir = normalize(lightPos - FragPos);
                    float diff = max(dot(norm , lightDir), 0.0);
                    vec3 diffuse = diff * lightColor;

                    float specularStrength = 0.5;
                    vec3 viewDir = normalize(viewPos - FragPos);
                    vec3 reflectDir = reflect(-lightDir, norm);  
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                    vec3 specular = specularStrength * spec * lightColor; 

                   vec3 result = (ambient + diffuse + specular ) * objectColor;
                   FragColor = vec4(result, 1.0);  
				}
				)", false);
		}
		///Call this every frame
		void Update(Camera* cam)
		{
			//For each entity
			for (auto itr = entities.begin(); itr != entities.end();)
			{
				ecs::Entity entity = *itr++;

				//Get relevant components
				Transform& transform = ecs::GetComponent<Transform>(entity);
				ModelRenderer& modelRenderer = ecs::GetComponent<ModelRenderer>(entity);

				//If a shader has been specified for this sprite use it, else use the default
				Shader* shader = defaultShader;
				if (modelRenderer.shader)
					shader = modelRenderer.shader;
				shader->use();

				//Create the model matrix, this is the same for each mesh so it only needs to be done once
				glm::mat4 model = TransformSystem::GetGlobalTransformMatrix(entity);

				//Model matrix
				unsigned int modelLoc = glGetUniformLocation(shader->ID, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				//View matrix
				unsigned int viewLoc = glGetUniformLocation(shader->ID, "view");
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));
				//Projection matrix
				unsigned int projLoc = glGetUniformLocation(shader->ID, "projection");
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
				//Light Color
				unsigned int colorLoc = glGetUniformLocation(shader->ID, "lightColor");
				glUniform3f(colorLoc, lightColor.x / 255, lightColor.y / 255, lightColor.z / 255);
				//Light Position
				unsigned int lightPosLoc = glGetUniformLocation(shader->ID, "lightPos");
				glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
				// camara pposition
				unsigned int viewPosLoc = glGetUniformLocation(shader->ID, "viewPos");
				glUniform3fv(viewPosLoc, 1, glm::value_ptr(cam->position));


				//For each mesh in the model
				for (unsigned int i = 0; i < modelRenderer.model->meshes.size(); i++)
				{
					Mesh mesh = modelRenderer.model->meshes[i];

					//Texture uniforms are named: uniform sampler2D texture_diffuseN, or texture_specularN
					//We can support up to 8 textures which have to be defined in the shader
					unsigned int diffuseNr = 1;
					unsigned int specularNr = 1;

					//For each Texture in the mesh
					for (unsigned int i = 0; i < mesh.textures.size(); i++)
					{
						//Activate proper texture unit before binding
						glActiveTexture(GL_TEXTURE0 + i);

						//Retrieve texture number and type (the N in texture_{type}N)
						std::string number;
						std::string name = mesh.textures[i]->type;
						if (name == "texture_diffuse")
							number = std::to_string(diffuseNr++);
						else if (name == "texture_specular")
							number = std::to_string(specularNr++);

						//Set the uniform for the material texture
						glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);

						glBindTexture(GL_TEXTURE_2D, mesh.textures[i]->ID());
					}

					//Unbind texture
					glActiveTexture(GL_TEXTURE0);

					//Draw mesh
					glBindVertexArray(mesh.VAO);
					glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}
		}
		///Set light position and color
		void SetLight(Vector3 _lightPos, Vector3 _lightColor)
		{
			lightPos = _lightPos;
			lightColor = _lightColor;
		}

	private:
		Vector3 lightPos;
		Vector3 lightColor = Vector3(255);

		Shader* defaultShader;
	};
}