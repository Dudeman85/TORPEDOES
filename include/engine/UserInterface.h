#pragma once

#error Dont use this!

namespace engine
{
	
	ECS_REGISTER_COMPONENT(UIElement)
	///UI Element struct
	struct UIElement
	{
		bool relativeToCamera = true;
		bool enabled = true;
	};

	///2D UI element system, Requires UIElement, and Transform
	ECS_REGISTER_SYSTEM(UISystem, Transform, UIElement)
	class UISystem : public ecs::System
	{
	public:
		///Update the UI system, call this once per frame
		void Update(Camera* cam)
		{
			for (auto const& entity : entities)
			{
				//Get relevant components
				UIElement& uiElement = ecs::GetComponent<UIElement>(entity);
				Transform& transform = ecs::GetComponent<Transform>(entity);

				//If the element should be relative to the camera move it correctly
				if (uiElement.relativeToCamera)
				{
					//TODO refactor camera to use ECS and fix this
					transform.position = Vector3(cam->position.x + transform.position.x, cam->position.y + transform.position.y, cam->position.z + transform.position.z);
				}

				//UI Elements are always drawn on top of everything
				transform.position.z = cam->position.z - 0.01f;
			}
		}
		///A pointer to place the shader in
		Shader* defaultShader;
	};
}