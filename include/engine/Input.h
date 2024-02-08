#pragma once

#include <optional>
#include <map>
#include <vector>

#include <GLFW/glfw3.h>
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

	class InputEvent
	{
	public:
		std::string name;

		// Every output state of InputButton this is binded to
		std::vector<InputState*> outputStates;

		const bool isPressed()
		{
			return pressed;
		}
		const bool isReleased()
		{
			return released;
		}
		const bool isNewPress()
		{
			return (isPressed() && newChange);
		}
		const bool isNewRelease()
		{
			return (isReleased() && newChange);
		}
		const bool isChanged()
		{
			return newChange;
		}

		void update()
		{
			InputState newState = InputState::None;
			pressed = false;
			released = false;
			newChange = false;

			for (auto outputState : outputStates)
			{
				switch (*outputState)
				{
				case input::InputState::Released:
					pressed = true;
					break;
				case input::InputState::Pressed:
					released = true;
					break;
				case input::InputState::NewRelease:
					released = true;
					newChange = true;
				case input::InputState::NewPress:
					pressed = true;
					newChange = true;
				break;
				case input::InputState::NewReleaseNewPress:
				case input::InputState::NewPressNewRelease:
					// Key was newly released and newly pressed last update
					pressed = true;
					released = true;
					newChange = true;
					break;
				default:
					break;
				}
				if (newChange && pressed && released)
				{
					return; // All states are true, testing cannot wield any changes
				}	
			}
		}

	protected:
		bool pressed = false;	// Whether key is pressed the update before this poll
		bool released = false;	// Whether key is released the update before this poll
		bool newChange = false;	// Whether key was changed the update before this poll
	};

	std::map<std::string, InputEvent*> nameToInputEvent;

	class InputButton;
	static std::vector<InputButton*> inputButtons;	// All inputButtons

	class InputButton
	{
	public:
		InputButton()
		{
			inputButtons.push_back(this);
		}

		InputState buttonInputState = InputState::Released;		// Callback inputted state of key
		InputState buttonOutputState = InputState::Released;	// Pollable state of key

		// Polls the change in the key, returns whether to poll next update
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

	std::map<inputKey, InputButton*> inputKeyToInputButton;

	static void unbindInput(InputButton* inputButton)
	{
		// TODO:

		// Find all events that use the button's buttonOutputState.
		// Remove the button from them
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
				std::cout << "Found matching InputEvent for " << inputEventName << std::endl;
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
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// Your custom key callback function
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
		// Find input button (Yes, this is slow. Yes, it's only way to have buttons pressed and released on same update)
		auto it = inputKeyToInputButton.find(key);

		// Call input button's callback logic
		if (it != inputKeyToInputButton.end())
		{
			it->second->callback(action != GLFW_RELEASE);
		}

		std::cout << "Key event: " << key << std::endl;
	}

	// Sets window with key callbacks, place as late into initialization as possible to increase load times while pressing keys
	static void initialize(GLFWwindow* window)
	{
		// Set the key callback function
		glfwSetKeyCallback(window, keyCallback);
	}
	// Updates all inputs. Call AFTER polling GLFW events, but BEFORE polling input events (such as before gameloop)
	static void update(GLFWwindow* window)
	{
		for (auto UpdatableinputButton : inputButtons)
		{
			UpdatableinputButton->update();
		}
	}
	// Frees up  memory used by the input system
	static void uninitialize()
	{
		for (int i = inputButtons.size() - 1; i >= 0; --i)
		{
			delete inputButtons[i];
			inputButtons.erase(inputButtons.begin() + i);
		}
	}
}
