#include <chrono>
#include <functional>
#include <engine/ECS.h>

namespace engine
{
	//Global timers
	double deltaTime = 0;
	double programTime = 0;
	unsigned long frameCount = 0;

	std::chrono::time_point<std::chrono::high_resolution_clock> _lastFrame;

	//Get the time since last call in seconds
	double CalculateDeltaTime()
	{
		std::chrono::time_point thisFrame = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = thisFrame - _lastFrame;
		deltaTime = duration.count();
		_lastFrame = thisFrame;
		programTime += deltaTime;
		return deltaTime;
	}

	/*
	struct Timer
	{
		std::function<>
	};
	*/
}