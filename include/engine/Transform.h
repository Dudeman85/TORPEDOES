#pragma once
#include <engine/ECS.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <engine/Vector.h>
#include <vector>
#include <set>


namespace engine
{
	//Transform component
	ECS_REGISTER_COMPONENT(Transform)
	struct Transform : ecs::Component
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale = Vector3(1.0f);

		//The parent entity of this entity
		//If set this transform will be relative to the parent
		ecs::Entity parent;
		//All the children of this entity
		std::set<ecs::Entity> children;

		//If true updates all transform based caches, reverts to false next frame
		//This will not update if transform is manually changed
		bool staleCache = true;
	};

	//Transform system
	//Requires Transform component
	ECS_REGISTER_SYSTEM(TransformSystem, Transform)
	class TransformSystem : public ecs::System
	{
	public:
		void Update()
		{
			for (auto const& entity : entities)
			{
				Transform& transform = ecs::GetComponent<Transform>(entity);
				transform.staleCache = false;
			}
		}

		//Translate an entity by dx, dy, and dz
		static void Translate(ecs::Entity entity, float dx, float dy, float dz = 0)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.position.x += dx;
			transform.position.y += dy;
			transform.position.z += dz;

			transform.staleCache = true;
		}
		//Translate an entity by dt
		static void Translate(ecs::Entity entity, Vector3 dt)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.position += dt;

			transform.staleCache = true;
		}
		//Set the world position of entity
		static void SetPosition(ecs::Entity entity, float x, float y, float z = 0)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.position.x = x;
			transform.position.y = y;
			transform.position.z = z;

			transform.staleCache = true;
		}
		//Set the world position of entity
		static void SetPosition(ecs::Entity entity, Vector3 position)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.position = position;

			transform.staleCache = true;
		}

		//Rotate an entity by euler angles dx, dy, and dz
		static void Rotate(ecs::Entity entity, float dx, float dy, float dz = 0)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.rotation.x += dx;
			transform.rotation.y += dy;
			transform.rotation.z += dz;

			transform.staleCache = true;
		}
		//Rotate an entity by euler angles dr
		static void Rotate(ecs::Entity entity, Vector3 dr)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.rotation += dr;

			transform.staleCache = true;
		}
		//Set the euler rotation of an entity
		static void SetRotation(ecs::Entity entity, float x, float y, float z = 0)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.rotation.x = x;
			transform.rotation.y = y;
			transform.rotation.z = z;

			transform.staleCache = true;
		}
		//Set the euler rotation of an entity
		static void SetRotation(ecs::Entity entity, Vector3 rotation)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.rotation = rotation;

			transform.staleCache = true;
		}

		//Scale an entity by dx, dy, and dz
		static void Scale(ecs::Entity entity, float dx, float dy, float dz = 0)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.scale.x = dx;
			transform.scale.y = dy;
			transform.scale.z = dz;

			transform.staleCache = true;
		}
		//Scale an entity by ds
		static void Scale(ecs::Entity entity, Vector3 ds)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.scale += ds;

			transform.staleCache = true;
		}
		//Set the scale of an entity
		static void SetScale(ecs::Entity entity, float x, float y, float z = 0)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.scale.x = x;
			transform.scale.y = y;
			transform.scale.z = z;

			transform.staleCache = true;
		}
		//Set the scale of an entity
		static void SetScale(ecs::Entity entity, Vector3 scale)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.scale = scale;

			transform.staleCache = true;
		}

		//Get the right (x) vector of a transform
		static Vector3 RightVector(ecs::Entity entity)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);

			//Create the rotation matrix
			glm::mat4 rotation = glm::mat4(1.0f);
			//X, Y, Z euler rotations
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			//Right vector is x
			glm::vec4 right(1, 0, 0, 0);

			right = right * rotation;

			return Vector3(right.x, right.y, right.z);
		}
		//Get the up (y) vector of a transform
		static Vector3 UpVector(ecs::Entity entity)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);

			//Create the rotation matrix
			glm::mat4 rotation = glm::mat4(1.0f);
			//X, Y, Z euler rotations
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			//Up vector is y
			glm::vec4 up(0, 1, 0, 0);

			up = up * rotation;

			return Vector3(up.x, up.y, up.z);
		}
		//Get the forward (z) vector of a transform
		static Vector3 ForwardVector(ecs::Entity entity)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);

			//Create the rotation matrix
			glm::mat4 rotation = glm::mat4(1.0f);
			//X, Y, Z euler rotations
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rotation = glm::rotate(rotation, glm::radians(-transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			//Forward vector is z
			glm::vec4 forward(0, 0, 1, 0);

			forward = forward * rotation;

			return Vector3(forward.x, forward.y, forward.z);
		}

		//Get the distance between two entities
		static float Distance(ecs::Entity a, ecs:: Entity b)
		{
			Transform& aTransform = ecs::GetComponent<Transform>(a);
			Transform& bTransform = ecs::GetComponent<Transform>(b);

			return sqrt(pow(bTransform.position.x - aTransform.position.x, 2) + pow(bTransform.position.y - aTransform.position.y, 2) + pow(bTransform.position.z - aTransform.position.z, 2));
		}

		//Get the angle of b with a as the origin
		//TODO Fix this to work in 3D
		static float Angle(ecs::Entity a, ecs::Entity b)
		{
			Transform& aTransform = ecs::GetComponent<Transform>(a);
			Transform& bTransform = ecs::GetComponent<Transform>(b);

			return glm::degrees(atan2(bTransform.position.y - aTransform.position.y, bTransform.position.x - aTransform.position.x));
		}

		static glm::mat4 GetLocalTranformMatrix(ecs::Entity entity)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);

			//Create the transform matrix
			glm::mat4 transformMatrix = glm::mat4(1.0f);
			//Position
			transformMatrix = glm::translate(transformMatrix, transform.position.ToGlm());
			//X, Y, Z euler rotations
			transformMatrix = glm::rotate(transformMatrix, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			transformMatrix = glm::rotate(transformMatrix, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transformMatrix = glm::rotate(transformMatrix, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			//Scale
			transformMatrix = glm::scale(transformMatrix, transform.scale.ToGlm());

			return transformMatrix;
		}

		//Get the global Transform matrix of an entity after all parent transforms have been applied
		static glm::mat4 GetGlobalTransformMatrix(ecs::Entity entity)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);

			//Go through all the parents and add them to a list
			std::vector<ecs::Entity> parents;
			parents.push_back(entity);
			ecs::Entity currentParent = transform.parent;
			while (currentParent != 0)
			{
				parents.push_back(currentParent);

				currentParent = ecs::GetComponent<Transform>(currentParent).parent;
			}

			//Create the transform matrix
			glm::mat4 transformMatrix = GetLocalTranformMatrix(parents.back());

			//Go through each parent from root up
			for (int i = parents.size() - 2; i >= 0; i--)
			{
				transformMatrix *= GetLocalTranformMatrix(parents[i]);
			}

			return transformMatrix;
		}

		//Applies transforms to vertices and returns the transformed vertices, takes rotation in degrees
		static std::vector<Vector3> ApplyTransforms(std::vector<Vector3> vertices, Vector3 rotation, Vector3 scale, Vector3 position)
		{
			//Create the transform matrix
			glm::mat4 transform = glm::mat4(1.0f);
			//Position
			transform = glm::translate(transform, position.ToGlm());
			//X, Y, Z euler rotations
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			//Scale
			transform = glm::scale(transform, scale.ToGlm());

			std::vector<Vector3> transformedVerts;
			//For each vertice apply the transforms
			for (int i = 0; i < vertices.size(); i++)
			{
				//Many type conversions later we have applied transform matrix
				glm::vec3 glmVert = glm::vec3(glm::vec4(vertices[i].ToGlm(), 0) * transform);
				Vector3 vert(glmVert.x, glmVert.y, glmVert.z);
				transformedVerts.push_back(vert);
			}

			return transformedVerts;
		}

		//Applies transforms to 2D vertices and returns the transformed vertices, takes rotation in degrees
		static std::vector<Vector2> ApplyTransforms2D(std::vector<Vector2> vertices, float rotation, Vector2 scale, Vector2 position)
		{
			std::vector<Vector2> transformedVerts;
			//For each vertice apply scale and rotation
			for (int i = 0; i < vertices.size(); i++)
			{
				//Apply transform to the polygon collider
				Vector2 transformedVert = vertices[i];
				//Rotate
				float angle = Radians(rotation);
				transformedVert.x = vertices[i].x * cos(angle) - vertices[i].y * sin(angle);
				transformedVert.y = vertices[i].x * sin(angle) + vertices[i].y * cos(angle);
				//Scale
				transformedVert *= scale;
				//Move
				transformedVert += position;

				transformedVerts.push_back(transformedVert);
			}
			return transformedVerts;
		}
	};
}