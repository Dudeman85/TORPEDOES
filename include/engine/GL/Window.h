#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <engine/Vector.h>
#include <iostream>

namespace engine
{
	static bool OPENGL_INITIALIZED = false;

	static GLFWwindow* mainWindow;

	static int mainWindowWidth = 0;
	static int mainWindowHeight = 0;
	static int mainWindowX = 15;
	static int mainWindowY = 30;
	static bool mainWindowFullscreen = false;

	//Return the monitor with greatest window overlap
	//https://stackoverflow.com/questions/21421074/how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
	GLFWmonitor* GetOptimalMonitor(GLFWwindow* window)
	{
		int nmonitors, i;
		int wx, wy, ww, wh;
		int mx, my, mw, mh;
		int overlap, bestoverlap;
		GLFWmonitor* bestmonitor;
		GLFWmonitor** monitors;
		const GLFWvidmode* mode;

		bestoverlap = 0;
		bestmonitor = NULL;

		glfwGetWindowPos(window, &wx, &wy);
		glfwGetWindowSize(window, &ww, &wh);
		monitors = glfwGetMonitors(&nmonitors);

		for (i = 0; i < nmonitors; i++) {
			mode = glfwGetVideoMode(monitors[i]);
			glfwGetMonitorPos(monitors[i], &mx, &my);
			mw = mode->width;
			mh = mode->height;

			overlap =
				std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) *
				std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

			if (bestoverlap < overlap) {
				bestoverlap = overlap;
				bestmonitor = monitors[i];
			}
		}

		return bestmonitor;
	}

	///Create OpenGL window and context
	GLFWwindow* CreateGLWindow(int width, int height, const char* name, bool fullscreen = false)
	{
		mainWindowWidth = width;
		mainWindowHeight = height;
		mainWindowFullscreen = fullscreen;

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
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
			int x, y, w, h;
			glfwGetMonitorWorkarea(primaryMonitor, &x, &y, &w, &h);
			glfwSetWindowMonitor(mainWindow, primaryMonitor, x, y, w, h, mode->refreshRate);
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

	void SetFullscreen() 
	{
		//Save windowed pos
		glfwGetWindowPos(mainWindow, &mainWindowX, &mainWindowY);

		mainWindowFullscreen = true;
		GLFWmonitor* monitor = GetOptimalMonitor(mainWindow);
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		int x, y, w, h;
		glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);
		glfwSetWindowMonitor(mainWindow, monitor, x, y, w, h, mode->refreshRate);
	}

	void SetWindowed() 
	{
		mainWindowFullscreen = false;
		glfwSetWindowMonitor(mainWindow, nullptr, mainWindowX, mainWindowY, mainWindowWidth, mainWindowHeight, 60);
	}

	void ToggleFullscreen()
	{
		if (mainWindowFullscreen)
			SetWindowed();
		else
			SetFullscreen();
	}
}
