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

		bool uiElement = false;

		bool enabled = true;

		///Alternate textures, will override default ones from model
		std::vector<Texture*> textures;
	};

	///3D Model Render System, requires Transform and ModelRenderer
	ECS_REGISTER_SYSTEM(ModelRenderSystem, Transform, ModelRenderer)
		class ModelRenderSystem : public ecs::System
	{
	public:
		///Initialize the Model Render System
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
			//Sort the entities and tilemap by Z
			std::set<float> layersToDraw;
			std::map<float, std::vector<ecs::Entity>> sortedEntities;

			//UI elements are sorted seperately
			std::set<float> uiLayersToDraw;
			std::map<float, std::vector<ecs::Entity>> sortedUIElements;

			//Sort the entities into sprite and UI layers
			for (ecs::Entity entity : entities)
			{
				Transform& transform = ecs::GetComponent<Transform>(entity);
				ModelRenderer& renderer = ecs::GetComponent<ModelRenderer>(entity);

				//Seperate sprites and UI elements
				if (!renderer.uiElement)
				{
					sortedEntities[transform.position.z].push_back(entity);
					layersToDraw.insert(transform.position.z);
				}
				else
				{
					sortedUIElements[transform.position.z].push_back(entity);
					uiLayersToDraw.insert(transform.position.z);
				}
			}

			//Draw all models by layer
			for (const float& layer : layersToDraw)
			{
				//Draw entities for this layer
				for (const ecs::Entity& entity : sortedEntities[layer])
				{
					DrawEntity(entity, cam);
				}
			}

			//Draw all UI elements by layer
			for (const float& layer : uiLayersToDraw)
			{
				//Clear the depth buffer to always draw UI elements on top
				glDisable(GL_DEPTH_BUFFER_BIT);

				//Draw entities for this layer
				for (const ecs::Entity& entity : sortedUIElements[layer])
				{
					DrawEntity(entity, cam);
				}
			}
			glEnable(GL_DEPTH_BUFFER_BIT);
		}

		///Draw an entity to the screen
		void DrawEntity(ecs::Entity entity, Camera* cam)
		{
			//Get relevant components
			Transform& transform = ecs::GetComponent<Transform>(entity);
			ModelRenderer& modelRenderer = ecs::GetComponent<ModelRenderer>(entity);

			if (!modelRenderer.enabled)
				return;

			if (!modelRenderer.model)
			{
				std::cout << "WARNING: No Model set!" << std::endl;
				return;
			}

			//If a shader has been specified for this model use it, else use the default
			Shader* shader = defaultShader;
			if (modelRenderer.shader)
				shader = modelRenderer.shader;
			shader->use();

			//Create the model matrix, this is the same for each mesh so it only needs to be done once
			glm::mat4 model = TransformSystem::GetGlobalTransformMatrix(entity);


			unsigned int viewLoc = glGetUniformLocation(shader->ID, "view");
			unsigned int projLoc = glGetUniformLocation(shader->ID, "projection");

			if (!modelRenderer.uiElement)
			{
				//Give the shader the camera's view matrix
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));

				//Give the shader the camera's projection matrix
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
			}
			else
			{
				//Give the shader a constant view matrix
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));

				//Give the shader a constant projection matrix
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
			}

			//Model matrix
			unsigned int modelLoc = glGetUniformLocation(shader->ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//Light Color
			unsigned int colorLoc = glGetUniformLocation(shader->ID, "lightColor");
			glUniform3f(colorLoc, lightColor.x / 255, lightColor.y / 255, lightColor.z / 255);
			//Light Position
			unsigned int lightPosLoc = glGetUniformLocation(shader->ID, "lightPos");
			glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
			//Camera Position
			unsigned int viewPosLoc = glGetUniformLocation(shader->ID, "viewPos");
			glUniform3fv(viewPosLoc, 1, glm::value_ptr(cam->position));

			//For each mesh in the model
			for (unsigned int i = 0; i < modelRenderer.model->meshes.size(); i++)
			{
				unsigned int diffuseNr = 1;
				unsigned int specularNr = 1;

				Mesh mesh = modelRenderer.model->meshes[i];

				//For each Texture in the mesh
				for (unsigned int j = 0; j < mesh.textures.size(); j++)
				{
					//Use default texture if no specific texture is assigned
					glActiveTexture(GL_TEXTURE0 + j);

					//Retrieve texture number and type (the N in texture_{type}N)
					std::string number;
					std::string name;

					//Check if we have an override texture for this mesh
					if (j < modelRenderer.textures.size() && modelRenderer.textures[j])
					{
						name = modelRenderer.textures[j]->type;

						//Bind override texture
						glBindTexture(GL_TEXTURE_2D, modelRenderer.textures[j]->ID());
					}
					//Use default texture
					else
					{
						name = mesh.textures[j]->type;

						//Bind default texture
						glBindTexture(GL_TEXTURE_2D, mesh.textures[j]->ID());
					}

					if (name == "texture_diffuse")
						number = std::to_string(diffuseNr++);
					else if (name == "texture_specular")
						number = std::to_string(specularNr++);

					//Set the uniform for the material texture
					glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), j);
				}

				//Unbind texture
				glActiveTexture(GL_TEXTURE0);

				//Draw mesh
				glBindVertexArray(mesh.VAO);
				glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
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

