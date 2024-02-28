#pragma once
#ifndef ENGINE_USE_VULKAN

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace engine
{
	bool OPENGL_INITIALIZED = false;

	//Create OpenGL window and context
	GLFWwindow* CreateGLWindow(int width, int height, const char* name)
	{
		//Initialize GLFW and set it to require OpenGL 3
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//Create window object
		GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);
		if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
		}

		//Set the resize window callback function
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				glViewport(0, 0, width, height);
			}
		);

		OPENGL_INITIALIZED = true;

		return window;
	}
}
#else

//#define GLFW_INCLUDE_VULKAN
#include <engine/GL/VulkanHelpers.h>

//GLFWAPI VkResult glfwCreateWindowSurface(VkInstance instance,
//	GLFWwindow* handle,
//	const VkAllocationCallbacks* allocator,
//	VkSurfaceKHR* surface);

//#include <vulkan/vulkan.h>
//#include <GLFW/glfw3.h>

#include <iostream>

namespace engine
{
	bool OPENGL_INITIALIZED = false;

	void GLFWErrorCallback(int, const char* err_str)
	{
		std::cout << "GLFW Error: " << err_str << std::endl;
	}

	//Create OpenGL window and context
	GLFWwindow* CreateGLWindow(int width, int height, const char* name)
	{
		//Initialize GLFW and set it to require OpenGL 3
		glfwSetErrorCallback(GLFWErrorCallback);
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		//Create window object
		GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}

		//Set the resize window callback function
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				engine::vulkan::framebufferResized = true;
			}
		);

		vulkan::window = window;

		//Vulkan stuff
		vulkan::InitVulkan();
		
		OPENGL_INITIALIZED = true;

		return window;
	}
}
#endif