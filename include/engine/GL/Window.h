#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <engine/Vector.h>
#include <iostream>

namespace engine
{
	static bool OPENGL_INITIALIZED = false;

	static GLFWwindow* mainWindow;

	///Create OpenGL window and context
	GLFWwindow* CreateGLWindow(int width, int height, const char* name, bool fullscreen = false)
	{
		//Initialize GLFW and set it to require OpenGL 3
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//glfwWindowHint(GLFW_SAMPLES, 4);

		//Create window object
		mainWindow = glfwCreateWindow(width, height, name, NULL, NULL);
		if (mainWindow == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}
		glfwMakeContextCurrent(mainWindow);
		if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
		}

		//Set the resize window callback function
		glfwSetFramebufferSizeCallback(mainWindow, [](GLFWwindow* window, int width, int height)
			{
				glViewport(0, 0, width, height);
			}
		);

		//If specified set the window to fullsreen and monitors resolution
		if (fullscreen)
		{
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			int x, y, w, h;
			glfwGetMonitorWorkarea(primaryMonitor, &x, &y, &w, &h);
			glfwSetWindowMonitor(mainWindow, primaryMonitor, x, y, w, h, 60);
		}

		//Enable 4xMSAA
		glfwWindowHint(GLFW_SAMPLES, 4);
		glEnable(GL_MULTISAMPLE);

		OPENGL_INITIALIZED = true;

		return mainWindow;
	}

	Vector2 GetMainWindowSize()
	{
		int w, h;
		glfwGetWindowSize(mainWindow, &w, &h);
		return { (float)w, (float)h };
	}
}
