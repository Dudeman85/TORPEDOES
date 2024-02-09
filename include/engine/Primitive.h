#pragma once
//OpenGL
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STL
#include <vector>
#include <math.h>

//Engine
#include <engine/ECS.h>
#include <engine/Transform.h>
#include <engine/GL/Shader.h>
#include <engine/GL/Texture.h>
#include <engine/GL/Camera.h>


namespace engine
{
	///A class to create and store a primitive shape
	class Primitive
	{
	private:
		Primitive(std::vector<float> vertices, std::vector<unsigned int> indices)
		{
			numVertices = indices.size();

			//Make the Vertex Array Object, Vertex Buffer Object, and Element Buffer Object
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			//Bind the Vertex Array Object
			glBindVertexArray(VAO);

			//Bind the Vertex Bufer Object and set vertices
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

			//Bind and set indices to EBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

			//Configure Vertex attribute at location 0 aka position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			//Unbind all buffers and arrays
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

	public:
		~Primitive()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}

		///A primitive can be drawn without being an entity
		void Draw(Camera* cam, Vector3 color = Vector3(100.0f), Transform transform = Transform())
		{
			defaultShader->use();

			glBindVertexArray(VAO);

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
			unsigned int modelLoc = glGetUniformLocation(defaultShader->ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			//Give the shader the primitive's color
			unsigned int colorLoc = glGetUniformLocation(defaultShader->ID, "color");
			glUniform4f(colorLoc, color.x / 255, color.y / 255, color.z / 255, 1);

			//Get the view and projection locations
			unsigned int viewLoc = glGetUniformLocation(defaultShader->ID, "view");
			unsigned int projLoc = glGetUniformLocation(defaultShader->ID, "projection");

			//Give the shader the camera's view matrix
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));
			//Give the shader the camera's projection matrix
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));


			glDrawElements(GL_LINE_LOOP, numVertices, GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
		}

		///Create a line starting at p1 and ending at p2
		static Primitive Line(Vector3 p1 = Vector3(-1, 0, 0), Vector3 p2 = Vector3(1, 0, 0))
		{
			//Rectangle vertices start at top left and go clockwise to bottom left
			std::vector<float> vertices
			{
				//Positions		
				p1.x, p1.y, p1.z,
				p2.x, p2.y, p2.z,
			};
			//Indices to draw a line
			std::vector<unsigned int> indices
			{
				0, 1
			};

			//Create the primitive object from vertice data
			return Primitive(vertices, indices);
		}

		///Create a triangle from three vertices, Defaults to equilateral triangle
		static Primitive Triangle(Vector3 v1 = Vector3(-1, -1, 0), Vector3 v2 = Vector3(1, -1, 0), Vector3 v3 = Vector3(0, 1, 0))
		{
			//Rectangle vertices start at bottom left and go clockwise to bottom right
			std::vector<float> vertices
			{
				//Positions	
				v1.x, v1.y, v1.z,
				v2.x, v2.y, v2.z,
				v3.x, v3.y, v3.z,
			};
			//Indices to draw a triangle
			std::vector<unsigned int> indices
			{
				0, 1, 2,
			};

			//Create the primitive object from vertice data
			return Primitive(vertices, indices);
		}

		///Create a rectangle from four vertices going clockwise, Defaults to square
		static Primitive Rectangle(Vector3 v1 = Vector3(-1, -1, 0), Vector3 v2 = Vector3(-1, 1, 0), Vector3 v3 = Vector3(1, 1, 0), Vector3 v4 = Vector3(1, -1, 0))
		{
			//Rectangle vertices start at bottom left and go clockwise to bottom right
			std::vector<float> vertices
			{
				//Positions		
				v1.x, v1.y, v1.z,
				v2.x, v2.y, v2.z,
				v3.x, v3.y, v3.z,
				v4.x, v4.y, v4.z,
			};
			//Indices to draw a rectangle
			std::vector<unsigned int> indices
			{
				0, 1, 2,
				2, 3, 0,
			};

			//Create the primitive object from vertice data
			return Primitive(vertices, indices);
		}

		///Create a polygon from provided 3D vertices, going clockwise
		static Primitive Polygon(std::vector<Vector3> verts)
		{
			//Move all Vector3 vertices to a simple float vector
			std::vector<float> vertices;
			//Automaticaly create indices to draw triangles
			std::vector<unsigned int> indices;
			for (int i = 0; i < verts.size(); i++)
			{
				vertices.push_back(verts[i].x);
				vertices.push_back(verts[i].y);
				vertices.push_back(verts[i].z);
				indices.push_back(i);
			}

			//Create the primitive object from vertice data
			return Primitive(vertices, indices);
		}
		///Create a polygon from provided 2D vertices, going clockwise
		static Primitive Polygon(std::vector<Vector2> verts)
		{
			//Move all Vector2 vertices to a simple float vector
			std::vector<float> vertices;
			//Automaticaly create indices to draw triangles
			std::vector<unsigned int> indices;
			for (int i = 0; i < verts.size(); i++)
			{
				vertices.push_back(verts[i].x);
				vertices.push_back(verts[i].y);
				vertices.push_back(0);
				indices.push_back(i);
			}

			//Create the primitive object from vertice data
			return Primitive(vertices, indices);
		}
		///Number of vertices
		unsigned int numVertices = 0;
		unsigned int VAO, VBO, EBO;
		static Shader* defaultShader;
	};

	Shader* Primitive::defaultShader = nullptr;

	
	ECS_REGISTER_COMPONENT(PrimitiveRenderer)
	///Primitive Renderer Component, They consist of only a primitive shape and a color, no texture
	struct PrimitiveRenderer
	{
		///Pointer
		Primitive* primitive = nullptr;
		///Color of the shapes
		Vector3 color;
		///Draw wireframes on objects
		bool wireframe = false;
		///Lock the X and Y of the shapes
		bool lockXYRotation = false;
		///Bool to turn primitive render on and off
		bool enabled = true;
		///Bool to turn ui elements on and off
		bool uiElement = false;
	};


	ECS_REGISTER_SYSTEM(PrimitiveRenderSystem, Transform, PrimitiveRenderer)
	///Primitive Render system, Requires PrimitiveRenderer and Transform
	class PrimitiveRenderSystem : public ecs::System
	{
	public:
		///Initialize the shaders
		void Init()
		{
			//The default 3D model shader
			defaultShader = new Shader(
				R"(
				#version 330 core
				layout(location = 0) in vec3 aPos;

				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;
				uniform vec4 color;

				out vec4 vertexColor;			

				void main()
				{
					vertexColor = color;
					gl_Position = projection * view * model * vec4(aPos, 1.0);
				}
				)",
				R"(
				#version 330 core
				out vec4 FragColor;

				in vec4 vertexColor;

				void main()
				{    
					FragColor = vertexColor;
				}
				)", false);

			Primitive::defaultShader = defaultShader;
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
				PrimitiveRenderer& primitiveRenderer = ecs::GetComponent<PrimitiveRenderer>(entity);

				if (!primitiveRenderer.enabled)
					continue;

				defaultShader->use();

				//Bind the right VAO after tilemap
				glBindVertexArray(primitiveRenderer.primitive->VAO);

				//Create the model matrix
				glm::mat4 model = glm::mat4(1.0f);
				//Position
				model = glm::translate(model, transform.position.ToGlm());
				//X, Y, Z euler rotations
				if (!primitiveRenderer.lockXYRotation)
				{
					model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
				//Scale
				model = glm::scale(model, transform.scale.ToGlm());

				//Give the shader the model matrix
				unsigned int modelLoc = glGetUniformLocation(defaultShader->ID, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				//Give the shader the primitive's color
				unsigned int colorLoc = glGetUniformLocation(defaultShader->ID, "color");
				glUniform4f(colorLoc, primitiveRenderer.color.x / 255, primitiveRenderer.color.y / 255, primitiveRenderer.color.z / 255, 1);

				//Get the view and projection locations
				unsigned int viewLoc = glGetUniformLocation(defaultShader->ID, "view");
				unsigned int projLoc = glGetUniformLocation(defaultShader->ID, "projection");

				if (!primitiveRenderer.uiElement)
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
					glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

					//Give the shader a constant projection matrix
					glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
				}

				//Render either as lines or triangles
				if (primitiveRenderer.wireframe || primitiveRenderer.primitive->numVertices < 3)
					glDrawElements(GL_LINE_LOOP, primitiveRenderer.primitive->numVertices, GL_UNSIGNED_INT, 0);
				else
					glDrawElements(GL_TRIANGLES, primitiveRenderer.primitive->numVertices, GL_UNSIGNED_INT, 0);

				int a = 0;
			}

			//Unbind vertex array
			glBindVertexArray(0);
		}
		///Pointer to store the shader
		Shader* defaultShader;
	};
}