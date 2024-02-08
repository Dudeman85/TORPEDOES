#pragma once
//OpenGL
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STL
#include <vector>
#include <set>

//Engine
#include <engine/ECS.h>
#include <engine/Transform.h>
#include <engine/GL/Shader.h>
#include <engine/GL/Texture.h>
#include <engine/GL/Camera.h>
#include <engine/Tilemap.h>


namespace engine
{
	///2D Sprite Renderer component
	ECS_REGISTER_COMPONENT(SpriteRenderer)
	struct SpriteRenderer
	{
		///Abstraction class for textures
		Texture* texture = nullptr;
		///Abstraction class for shaders
		Shader* shader = nullptr;
		///Bool to turn on the sprite renderer
		bool enabled = true;
		///Bool to turn on the ui elements
		bool uiElement = false;
	};

	///Animation struct. Not a component
	struct Animation
	{
		Animation() {};
		Animation(std::vector<Texture*> animationTextures, std::vector<int> animationDelays)
		{
			assert(animationTextures.size() == animationDelays.size() && "Failed to create animation! Number of frames and delays do not match!");
			
			textures = animationTextures;
			
			delays = animationDelays;
			
			length = animationDelays.size();
		};
		std::vector<Texture*> textures;
		std::vector<int> delays;
		unsigned int length = 0;
	};

	///Animator component
	ECS_REGISTER_COMPONENT(Animator)
	struct Animator
	{
		std::map<std::string, Animation> animations;

		std::string currentAnimation;
		int animationFrame = 0;
		bool repeatAnimation = false;
		bool playingAnimation = false;

		float animationTimer = 0;
	};

	///2D Sprite Render system, Requires SpriteRenderer and Transform
	ECS_REGISTER_SYSTEM(SpriteRenderSystem, SpriteRenderer, Transform)
	class SpriteRenderSystem : public ecs::System
	{
	public:
		~SpriteRenderSystem()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteVertexArrays(1, &VBO);
			glDeleteVertexArrays(1, &EBO);
		}
		///Initialize the shaders and clear the screen
		void Init()
		{
			//Set the screen clear color to black
			glClearColor(0, 0, 0, 1.0f);

			//Enable transparency
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			//Enable Depth buffering
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			//Create the default sprite shader
			defaultShader = new Shader(
				R"(
				#version 330 core
				layout (location = 0) in vec3 aPos;
				layout(location = 1) in vec2 aTexCoord;
				out vec2 TexCoord;
				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;
				void main()
				{
					gl_Position = projection * view * model * vec4(aPos, 1.0f);
					TexCoord = vec2(aTexCoord.x, aTexCoord.y);
				}
				)",
				R"(
				#version 330 core
				out vec4 FragColor;
				in vec2 TexCoord;
				uniform sampler2D texture1;
				void main()
				{
				   FragColor = texture(texture1, TexCoord);
				}
				)", false);

			//Rectangle vertices start at top left and go clockwise to bottom left
			float vertices[] = {
				//Positions		  Texture Coords
				 1.f,  1.f, 0.0f, 1.0f, 1.0f, // top right
				 1.f, -1.f, 0.0f, 1.0f, 0.0f, // bottom right
				-1.f, -1.f, 0.0f, 0.0f, 0.0f, // bottom left
				-1.f,  1.f, 0.0f, 0.0f, 1.0f, // top left 
			};
			//Indices to draw a rectangle from two triangles
			unsigned int indices[] = {
				0, 1, 2, //1st trangle
				0, 2, 3, //2nd triangle
			};

			//Make the Vertex Array Object, Vertex Buffer Object, and Element Buffer Object
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			//Bind the Vertex Array Object
			glBindVertexArray(VAO);

			//Bind the Vertex Bufer Object and set vertices
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			//Bind and set indices to EBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			//Configure Vertex attribute at location 0 aka position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			//Configure Vertex attribute at location 1 aka texture coords
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			//Unbind all buffers and arrays
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		///Renders everything. Call this every frame
		void Update(Camera* cam)
		{
			//Sort the entities and tilemap by Z
			std::set<float> layersToDraw;
			std::map<float, std::vector<ecs::Entity>> sortedEntities;
			if (tilemap)
				layersToDraw.insert(tilemap->zLayers.begin(), tilemap->zLayers.end());

			//UI elements are sorted seperately
			std::set<float> uiLayersToDraw;
			std::map<float, std::vector<ecs::Entity>> sortedUIElements;

			//Sort the entities into sprite and UI layers
			for (auto itr = entities.begin(); itr != entities.end();)
			{
				ecs::Entity entity = *itr++;
				Transform& transform = ecs::GetComponent<Transform>(entity);
				SpriteRenderer& renderer = ecs::GetComponent<SpriteRenderer>(entity);

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

			//Draw all sprites and tilemap by layer
			for (const float& layer : layersToDraw)
			{
				if (tilemap)
					tilemap->draw(layer);

				//Bind the right VAO after tilemap
				glBindVertexArray(VAO);

				//Draw entities for this layer
				for (const ecs::Entity& entity : sortedEntities[layer])
				{
					DrawEntity(entity, cam);
				}
			}

			//Draw all UI elements by layer
			for (const float& layer : uiLayersToDraw)
			{
				//Draw entities for this layer
				for (const ecs::Entity& entity : sortedUIElements[layer])
				{
					DrawEntity(entity, cam);
				}
			}

			//Unbind vertex array
			glBindVertexArray(0);
		}

		///Draw an entity to the screen
		void DrawEntity(ecs::Entity entity, Camera* cam)
		{
			//Get relevant components
			SpriteRenderer& sprite = ecs::GetComponent<SpriteRenderer>(entity);
			Transform& transform = ecs::GetComponent<Transform>(entity);

			if (!sprite.enabled)
				return;

			//If a shader has been specified for this sprite use it, else use the default
			Shader* shader = defaultShader;
			if (sprite.shader)
				shader = sprite.shader;
			shader->use();

			//Create the model matrix
			glm::mat4 model = TransformSystem::GetGlobalTransformMatrix(entity);

			//Give the shader the model matrix
			unsigned int modelLoc = glGetUniformLocation(shader->ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//Get the view and projection locations
			unsigned int projLoc = glGetUniformLocation(shader->ID, "projection");
			unsigned int viewLoc = glGetUniformLocation(shader->ID, "view");
			
			if (!sprite.uiElement)
			{
				//Give the shader the camera's view matrix
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));

				//Give the shader the camera's projection matrix
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
			}
			else
			{
				//Clear the depth buffer to always draw UI elements on top
				glClear(GL_DEPTH_BUFFER_BIT);

				//Give the shader a constant view matrix
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));

				//Give the shader a constant projection matrix
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
			}

			//Bind the texture
			glActiveTexture(GL_TEXTURE0);
			if (sprite.texture)
				sprite.texture->Use();

			//Draw the sprite
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			//Unbind the texture
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		///Set the screens clear color to given rgb
		static void SetBackgroundColor(float r, float g, float b)
		{
			glClearColor(r / 255, g / 255, b / 255, 1.0f);
		}

		///Set a tilemap to render
		void SetTilemap(Tilemap* map)
		{
			tilemap = map;
		}
		//Remove a tilemap from rendering
		void RemoveTilemap()
		{
			tilemap = nullptr;
		}

	private:
		unsigned int VAO, VBO, EBO;
		Shader* defaultShader;
		Tilemap* tilemap = nullptr;
	};

	///Animator system, Requires Animator and SpriteRenderer
	ECS_REGISTER_SYSTEM(AnimationSystem, Animator, SpriteRenderer)
	class AnimationSystem : public ecs::System
	{
	public:
		//Update every entity with relevant components
		void Update(float deltaTime)
		{
			//Delta time in milliseconds
			deltaTime *= 1000;

			//For each entity that has the required components
			for (auto const& entity : entities)
			{
				//Get the relevant components from entity
				Animator& animator = ecs::GetComponent<Animator>(entity);

				//If the entity is currently playing an animation
				if (animator.playingAnimation)
				{
					animator.animationTimer += deltaTime;

					//If enough time (defined by animation) has passed advance the animation frame
					if (animator.animationTimer >= animator.animations[animator.currentAnimation].delays[animator.animationFrame])
					{
						AdvanceFrame(entity);
					}
				}
			}
		}

		///Advance to the next animation frame of current animation
		static void AdvanceFrame(ecs::Entity entity)
		{
			//Get the relevant components from entity
			Animator& animator = ecs::GetComponent<Animator>(entity);
			SpriteRenderer& sprite = ecs::GetComponent<SpriteRenderer>(entity);

			//Change Sprites texture
			sprite.texture = animator.animations[animator.currentAnimation].textures[animator.animationFrame];

			animator.animationFrame++;
			animator.animationTimer = 0;

			//If end of animation has been reached go to start or end animation
			if (animator.animationFrame >= animator.animations[animator.currentAnimation].length)
			{
				animator.animationFrame = 0;
				animator.animationTimer = 0;

				//End the animation if it is not set to repeat
				if (!animator.repeatAnimation)
				{
					animator.playingAnimation = false;
					animator.currentAnimation = "";
					return;
				}
			}
		}

		///Add animations to entity, they will be accessible by given names
		static void AddAnimations(ecs::Entity entity, std::vector<Animation> animations, std::vector<std::string> names)
		{
			if (animations.size() > names.size())
				throw("Not enough names given for each animation!");

			Animator& animator = ecs::GetComponent<Animator>(entity);

			//For each animation to add
			for (size_t i = 0; i < animations.size(); i++)
			{
				animator.animations.insert({ names[i], animations[i] });
			}
		}

		///Add an animation to entity, it will be accessibl by given name
		static void AddAnimation(ecs::Entity entity, Animation animation, std::string name)
		{
			Animator& animator = ecs::GetComponent<Animator>(entity);

			//Add the animation indexed by given name
			animator.animations.insert({ name, animation });
		}

		///Play an animation, optionally set it to repeat, if the animation is currently playing don't do anything
		static void PlayAnimation(ecs::Entity entity, std::string animation, bool repeat = false)
		{
			Animator& animator = ecs::GetComponent<Animator>(entity);

			if (animator.animations.find(animation) == animator.animations.end())
			{
				std::cout << "Warning: No animation named \"" << animation << "\" was found in this entity." << std::endl;
				return;
			}

			if (animator.currentAnimation == animation)
				return;

			animator.currentAnimation = animation;
			animator.animationFrame = 0;
			animator.repeatAnimation = repeat;
			animator.playingAnimation = true;
			animator.animationTimer = 0;

			//Immediately advance to 1st frame of animation
			AdvanceFrame(entity);
		}

		///Stop an animation, optionally provide the specific animation to stop
		static void StopAnimation(ecs::Entity entity, std::string animation = "")
		{
			Animator& animator = ecs::GetComponent<Animator>(entity);

			//If trying to stop animation that is not playing, return without doing anything
			if (animation != "")
				if (animator.currentAnimation != animation)
					return;

			animator.currentAnimation = "";
			animator.animationFrame = 0;
			animator.animationTimer = 0;
			animator.playingAnimation = false;
		}
	};
}