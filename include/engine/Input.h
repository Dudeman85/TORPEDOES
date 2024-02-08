#pragma once

#include <optional>
#include <map>
#include <vector>
#include <list>
#include <cmath>

#include <GLFW/glfw3.h>
#include "engine/Vector.h"

#include <iostream>

namespace input
{ 
	using inputKey = int;	// GLFW key
	using keyState = int;	// Whether key is pressed

	enum class InputState
	{
		None = 0,			// Input has not been changed
		Released,			// Input is not being pressed
		Pressed,			// Input is being pressed
		NewRelease,			// Input was released
		NewPress,			// Input was pressed
		NewReleaseNewPress,	// Input was released, then pressed
		NewPressNewRelease	// Input was pressed, then released
		// Each update, only one press and one release input is reported; It is unnecessary to report more
	};

	enum class eventStates
	{
		Not,	// This staate is (still not on
		On,		// This state is (still) on
		NewOn	// This state was turned on
	};

	using engine::Vector2;

	// Handles a specified input's values by saving all values and timestamps in InputSamples.
	// From this, we can calculate whether input has been in a specified value range within an update iteration.
	// We can also calculate the time an input has spent in the specified value range.
	// The InputSamples are written to from input callbacks.
	// The old update iteration's InputSamples are cleared upon an update iteration start.
	struct InputValue
	{
		// X is timestamp in relation to latest update iteration
		// Y is value
		// X starts out as 0 for first InputValue.
		// X is set to the moment a callback to this input is reached. Y is set to value
		using InputSample = Vector2;

		std::list<Vector2> inputSamples;

		double lastUpdatedTime; // last time the inputValue was updated

		void addInputSample(float value)
		{
			// Called from callback function, add currrent time and value into inputSamples

			// Get current time (DeltaTime - lastUpdatedTime)
		};

		void clearInputSamples(InputSample)
		{
			if (inputSamples.size() <= 0)
			{
				return;
			}
			// Add last sample
			InputSample latestSample = *inputSamples.end();

			inputSamples.clear();

			inputSamples.push_back(latestSample);
		};

		const bool findValue(float value)
		{
			// Loop throgh all inputValues to check if ANY have met this value:

			//return findIntersectionTime(inputSamples[0], inputSamples[1], value).has_value();
		}

		const bool findRange(float minimumValue, float maximumValue)
		{
			// Loop throgh all inputValues to check if ANY fall within this range:

			return (findValue(minimumValue) || findValue(maximumValue));
		}

		const std::optional<float> findTimeSpent(float value)
		{
			// Loop throgh all inputValues and add their time together:

			//return findIntersectionTimeSpent(inputSamples[0], inputSamples[1], value);
		}

		const static std::optional<float> findIntersectionTime(InputSample previousSample, InputSample currentSample, float targetValue)
		{
			float slope = (currentSample.y - previousSample.y) / (currentSample.x - previousSample.x);

			float yIntercept = previousSample.y - (previousSample.x * slope);

			float intersectionX = (targetValue - yIntercept) / slope;

			// Value never reaches targetValue
			if (!std::isfinite(intersectionX))
			{
				return std::nullopt;
			}

			return intersectionX;
		}
		const static std::optional<float> findIntersectionTimeSpent(InputSample previousSample, InputSample currentSample, float targetValue)
		{
			std::optional<float> targetX = findIntersectionTime(previousSample, currentSample, targetValue);
			if (!targetX)
			{
				return std::nullopt;
			}

			Vector2 delta = { std::abs(currentSample.x - targetX.value()), std::abs(currentSample.y - targetValue) };

			float distance = delta.Length();

			return distance;
		}
	};

	class InputEvent;
	// All inputEvents
	std::map<std::string, InputEvent*> nameToInputEvent;

	class InputEvent
	{
	public:
		InputEvent(std::string inputName) : name(inputName)
		{
			nameToInputEvent[name] = this;
		}

		std::string name;

		// Every output state of InputButton this is binded to
		std::vector<InputState*> outputStates;

		const bool isPressed()
		{
			return pressed != eventStates::Not;
		}
		const bool isReleased()
		{
			return released != eventStates::Not;
		}
		const bool isNewPress()
		{
			return (pressed == eventStates::NewOn);
		}
		const bool isNewRelease()
		{
			return (released == eventStates::NewOn);
		}
		const bool isChanged()
		{
			return (isNewPress() || isNewRelease());
		}

		void update()
		{
			InputState newState = InputState::None;
			pressed = eventStates::Not;
			released = eventStates::Not;

			for (auto outputState : outputStates)
			{
				switch (*outputState)
				{
				case input::InputState::Released:
					if (released != eventStates::NewOn)
					{ 
						released = eventStates::On;
					}
					break;
				case input::InputState::Pressed:
					if (pressed != eventStates::NewOn)
					{
						pressed = eventStates::On;
					}
					break;
				case input::InputState::NewRelease:
					released = eventStates::NewOn;
					if (pressed == eventStates::NewOn)
					{
						return; // All states are on, testing cannot wield any changes
					}
					break;
				case input::InputState::NewPress:
					pressed = eventStates::NewOn;
					if (released == eventStates::NewOn)
					{
						return; // All states are on, testing cannot wield any changes
					}
					break;
				case input::InputState::NewReleaseNewPress:
				case input::InputState::NewPressNewRelease:
					// Key was newly released and newly pressed last update
					pressed = eventStates::NewOn;
					released = eventStates::NewOn;
					return; // All states are on, testing cannot wield any changes
					break;
				default:
					break;
				}
			}

			std::cout << "button state is: " << (int)pressed << ": " << (int)released << "\n";
		}

	protected:
		eventStates pressed = eventStates::Not;		// Whether key is pressed the update before this poll
		eventStates released = eventStates::Not;	// Whether key is released the update before this poll
	};

	class InputButton;	
	std::map<inputKey, InputButton*> inputKeyToInputButton;	// All inputButtons

	class InputButton
	{
	public:
		InputButton(inputKey key)
		{
			//inputButtons.push_back(this);
			inputKeyToInputButton[key] = this;
		}

		InputState buttonInputState = InputState::Released;		// Callback inputted state of key
		InputState buttonOutputState = InputState::Released;	// Pollable state of key

		// Checks the change in the key
		void update()
		{
			if (buttonInputState == InputState::None)
			{
				switch (buttonOutputState)
				{
				case input::InputState::Released:
				case input::InputState::Pressed:
					// Key was not updated, keep the same state
					break;
				case input::InputState::NewPress:
				case input::InputState::NewReleaseNewPress:
					// Key was newly pressed last update, now it's just pressed
					buttonOutputState = InputState::Pressed;
					break;
				case input::InputState::NewRelease:
				case input::InputState::NewPressNewRelease:
					// Key was newly released last update, now it's just released
					buttonOutputState = InputState::Released;
					break;
				default:
					break;
				}
			}
			else
			{ 
				// Set outputState to be InputState
				buttonOutputState = buttonInputState;
				// Reset inputState
				buttonInputState = InputState::None;
			}
		}

		// Pressed/released logic
		void callback(bool pressed)
		{
			if (pressed)
			{
				// Button is currently being pressed, what about other inputs this update cycle?
				switch (buttonInputState)
				{
				case input::InputState::None:		// No input before: ignore
				case input::InputState::Released:	// Should not be possible: ignore
				case input::InputState::Pressed:	// Should not be possible: ignore
				case input::InputState::NewPress:	// Same input: ignore
					// There were no conflicting inputs: Set as new press:
					buttonInputState = InputState::NewPress;
					break;
				case input::InputState::NewRelease:
				case input::InputState::NewReleaseNewPress:
				case input::InputState::NewPressNewRelease:
					// Both inputs: Set as new new release, then new press:
					buttonInputState = InputState::NewReleaseNewPress;
					break;
				default:
					break;
				}
			}
			else
			{
				// Button is currently not being pressed, what about other inputs this update cycle?
				switch (buttonInputState)
				{
				case input::InputState::None:		// No input before: ignore
				case input::InputState::Released:	// Should not be possible: ignore
				case input::InputState::Pressed:	// Should not be possible: ignore
				case input::InputState::NewRelease:	// Same input: ignore
					// There were no conflicting inputs: Set as new release:
					buttonInputState = InputState::NewRelease;
					break;
				case input::InputState::NewPress:
				case input::InputState::NewReleaseNewPress:
				case input::InputState::NewPressNewRelease:
					// Both inputs: Set as new new press, then new release:
					buttonInputState = InputState::NewPressNewRelease;
					break;
				default:
					break;
				}
			}
		}
	};

	static void unbindInput(InputButton* inputButton)
	{
		// TODO:

		// Find all events that use the button's buttonOutputState.
		// Remove the button from them

		// If inputButton has no uses, delete it (not necessary, if engine restarts, the button will not be initialized anymore)
	}

	static void bindInput(InputButton* inputButton, std::vector<InputEvent*> inputEvents)
	{
		for (auto inputEvent : inputEvents)
		{
			// Bind event to button
			inputEvent->outputStates.push_back(&inputButton->buttonOutputState);
		}
	}
	static void bindInput(inputKey key, std::vector<InputEvent*> inputEvents)
	{
		// Find inputKey
		auto it = inputKeyToInputButton.find(key);

		// Check if the key exists in the map
		if (it != inputKeyToInputButton.end()) 
		{
			bindInput(it->second, inputEvents);
		}
		else
		{
			// Construct a new inputButton to bind
			bindInput(new InputButton(key), inputEvents);
		}
	}
	static void bindInput(inputKey key, std::vector<std::string> inputEventNames)
	{
		std::vector<InputEvent*> inputEventsToBind;

		// Find by InputEvent name
		for (const auto& inputEventName : inputEventNames)
		{
			auto it = nameToInputEvent.find(inputEventName);

			// Check if the inputEvent's name exists in the map
			if (it != nameToInputEvent.end()) 
			{
				inputEventsToBind.push_back(it->second);
			}
			else 
			{
				std::cout << "WARNING: No matching InputEvent found for " << inputEventName << std::endl;
			}
		}

		bindInput(key, inputEventsToBind);
	}

	// Function prototype for the key callback
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
		// Find input button (Yes, this is slow. Yes, it's only way to have buttons pressed and released on same update)
		auto it = inputKeyToInputButton.find(key);

		// Call input button's callback logic
		if (it != inputKeyToInputButton.end())
		{
			it->second->callback(action != GLFW_RELEASE);
		}
	}

	// Sets window with key callbacks
	// Place as late into initialization as possible to increase load times while pressing keys
	// Should be a singleton, lazy-evaluation calls this late as possible
	static void initialize(GLFWwindow* window)
	{
		// Set the key callback function
		glfwSetKeyCallback(window, keyCallback);
	}
	// Updates all inputs. Call AFTER polling GLFW events, but BEFORE polling input events (such as before gameloop)
	static void update(GLFWwindow* window)
	{
		for (auto it = inputKeyToInputButton.begin(); it != inputKeyToInputButton.end(); ++it)
		{
			it->second->update();
		}

		for (auto it = nameToInputEvent.begin(); it != nameToInputEvent.end(); ++it)
		{
			it->second->update();
		}
	}
	// Frees memory used by the input system
	static void uninitialize()
	{
		// Free inputButtons
		for (auto it = inputKeyToInputButton.begin(); it != inputKeyToInputButton.end(); ++it)
		{
			delete it->second;
		}

		// Free inputEvents
		for (auto it = nameToInputEvent.begin(); it != nameToInputEvent.end(); ++it) 
		{
			delete it->second;
		}
		nameToInputEvent.clear();
	}
}
