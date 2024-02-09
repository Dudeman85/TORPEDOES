#pragma once
#include <engine/Vector.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine
{
	///Class for creating and storing data for the camera
	class Camera
	{
	public:
		///Constructor
		Camera(float w, float h, glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f), glm::vec3 rot = glm::vec3(0.f, 0.f, 0.f))
		{
			width = w;
			height = h;
			position = pos;
			rotation = rot;
		}

		///Sets the camera position in world space
		void SetPosition(Vector3 pos)
		{
			//I really have no idea why these have to be divided by 2, but otherwise the coordinates don't match the sprites
			position.x = pos.x / 2;
			position.y = pos.y / 2;
			position.z = pos.z / 2;
		}

		///Move the camera in world space. Camera's origin is in its center
		void Translate(Vector3 dPos = 0.f)
		{
			//I really have no idea why these have to be divided by 2, but otherwise the coordinates don't match the sprites
			position.x += dPos.x / 2;
			position.y += dPos.y / 2;
			position.z += dPos.z / 2;
		}

		///Set the rotation of the camera the camera in world space
		void SetRotation(Vector3 rot)
		{
			rotation.x = rot.x;
			rotation.y = rot.y;
			rotation.z = rot.z;
		}

		///Rotate the camera in world space
		void Rotate(Vector3 dRot)
		{
			rotation.x += dRot.x;
			rotation.y += dRot.y;
			rotation.z += dRot.z;
		}

		///Sets the width and height of the camera
		void SetDimensions(float w, float h)
		{
			width = w;
			height = h;
		}

		///Get the projection matrix for this camera. is perspective is set to true, use perspective projection instead of orthographic
		glm::mat4 GetProjectionMatrix()
		{
			if (perspective)
				return glm::perspective(glm::radians(fov), width / height, 0.01f, farPlane);
			else
				return glm::ortho(position.x - width / 2, position.x + width / 2, position.y - height / 2, position.y + height / 2, 0.01f, farPlane);
		}

		///Get the model matrix for this camera, aka the view matrix
		glm::mat4 GetViewMatrix()
		{
			glm::mat4 model = glm::mat4(1.0f);
			//Position
			model = glm::translate(model, -position);
			//X, Y, Z euler rotations
			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			return model;
		}

		///Position of the camera
		glm::vec3 position;
		///Rotation of the camera
		glm::vec3 rotation;
		///Bool to turn on and of different perspective modes
		bool perspective = false;
		///Width of the camera
		float width;
		///Height of the camera
		float height;
		///Farplane is a location in the final fantasy mmo i guess but also a datapoint for our 3rd perspective camera
		float farPlane = 10000;
		///Field of view of the camera
		float fov = 45;
	};
}