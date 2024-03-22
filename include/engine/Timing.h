#include <chrono>
#include <vector>
#include <functional>
#include <engine/ECS.h>

namespace engine
{
	//Global timers, these are updated in TimerSystem::Update()
	//How many seconds the last frame took
	double deltaTime = 0;
	//How many seconds the program has been running
	double programTime = 0;
	//How many frames the program has been through
	unsigned long frameCount = 0;

	std::chrono::time_point<std::chrono::high_resolution_clock> _lastFrame;

	//Get the time since last call in seconds
	double CalculateDeltaTime()
	{
		std::chrono::time_point thisFrame = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = thisFrame - _lastFrame;
		deltaTime = duration.count();
		_lastFrame = thisFrame;
		return deltaTime;
	}

	//A function to be called sometime in the future
	struct ScheduledFunction
	{
		enum class Type { frames, seconds };
		//Should the duration be treated as frames or seconds
		Type type;
		//How long the delay before calling is
		double duration;
		bool repeat = false;
		std::function<void()> function;
		double timePassed;
	};

	//Timer Component
	ECS_REGISTER_COMPONENT(Timer)
	struct Timer
	{
		//Duration of the timer in seconds
		double duration = 0;
		//Seconds passed
		double timePassed = 0;
		//Restart the timer after completion
		bool repeat = false;
		//Function to call after timer is done
		std::function<void(ecs::Entity)> callback;
		//Is the timer currently running
		bool running = false;
	};

	//Timer System, Requires Timer
	ECS_REGISTER_SYSTEM(TimerSystem, Timer)
	class TimerSystem : public ecs::System
	{
	private:
		static std::vector<ScheduledFunction> schedule;

	public:
		void Init()
		{
			//Start Time
			_lastFrame = chrono::high_resolution_clock::now();
		}

		void Update()
		{
			// Iterate through scheduled functions
			for (auto itr = schedule.begin(); itr != schedule.end();)
			{
				// Get the entity and increment the iterator
				ScheduledFunction& future = *itr;

				if (future.type == ScheduledFunction::Type::seconds)
				{
					// Timer is realtime, add deltaTime
					future.timePassed += deltaTime;
				}
				else
				{
					// Timer is in frames, add one frame
					future.timePassed++;
				}

				// Whether timer is done
				while (future.timePassed >= future.duration)
				{
					future.timePassed -= future.duration;

					future.function();

					// If not repeating, delete the event
					if (!future.repeat)
					{
						itr = schedule.erase(itr);
						break;
					}
				}

				// Iterator can get a new value so make sure it is not at end
				if (itr != schedule.end())
				{
					itr++;
				}
			}

			//Iterate through entities
			for (ecs::Entity entity : entities)
			{
				Timer& timer = ecs::GetComponent<Timer>(entity);

				//If timer is not done
				if (timer.timePassed < timer.duration)
				{
					timer.timePassed += deltaTime;
				}
				else
				{
					timer.timePassed = 0;

					//Call callback if applicable
					if (timer.callback)
						timer.callback(entity);

					//Stop timer if not repeating
					if (!timer.repeat)
						timer.running = false;
				}
			}

			//Update all the time stuff
			CalculateDeltaTime();
			frameCount++;
			programTime += deltaTime;
		}

		//Shorthand for starting an entity's timer
		static inline void StartTimer(ecs::Entity entity, double duration, bool repeat = false, std::function<void(ecs::Entity)> callback = nullptr)
		{
			Timer& timer = ecs::GetComponent<Timer>(entity);

			timer.duration = duration;
			timer.timePassed = 0;
			timer.repeat = repeat;
			timer.callback = callback;
			timer.running = true;
		}

		//Schedule a function to be executed in n seconds or frames, returns a handle to that event
		static ScheduledFunction& ScheduleFunction(std::function<void()> function, double time, bool repeat = false, ScheduledFunction::Type durationType = ScheduledFunction::Type::seconds)
		{
			ScheduledFunction future = ScheduledFunction{ .type = durationType, .duration = time, .repeat = repeat, .function = function };
			schedule.push_back(future);
			return schedule.back();
		}
	};
}

std::vector<ScheduledFunction> TimerSystem::schedule = schedule;