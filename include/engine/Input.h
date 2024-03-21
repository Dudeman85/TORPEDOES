#pragma once

#include <algorithm>
#include <thread>
#include <optional>
#include <map>
#include <vector>
#include <list>
#include <cmath>

#include "engine/Vector.h"
#include "engine/Application.h"

namespace input
{ 
	using inputKey = int;	// GLFW keyboard key
	using inputButton = int;// GLFW gamepad button
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

		std::list<InputSample> inputSamples;

		float lastUpdatedTime; // Last time the inputValue was updated

		InputValue(float startingValue = 0)
		{
			// First sample is 0, we assume the value is not changed
			inputSamples.push_back({ 0,startingValue });
			lastUpdatedTime = engine::deltaTime;
		}

		void update()
		{
			clearInputSamples();
			lastUpdatedTime = engine::deltaTime;
		}

		// Called from a callback function
		void addInputSample(float addValue)
		{
			inputSamples.push_back((engine::deltaTime - lastUpdatedTime, addValue));
		};

		void clearInputSamples()
		{
			if (inputSamples.size() <= 0)
			{
				return;
			}
			// Keep the latest sample
			InputSample latestSample = *inputSamples.end();
			// Set the latest sample to be before our 0 time, as it's from the last update iteration
			latestSample.x = (engine::deltaTime - lastUpdatedTime) - latestSample.x;

			inputSamples.clear();

			inputSamples.push_back(latestSample);
		};

		const bool isValue(float targetValue)
		{
			// We need at least 2 samples
			if (inputSamples.size() > 1)
			{
				// Loop throgh all inputValues to check if ANY have met the specified value
				for (std::list<InputSample>::iterator it = inputSamples.begin(); it != inputSamples.end();)
				{
					if (findIntersectionTime(*it, *(++it), targetValue).has_value())
					{
						return true;
					}
				}
			}
			return false;
		}

		const bool isRange(float minimumValue, float maximumValue)
		{
			// We need at least 2 samples
			if (inputSamples.size() > 1)
			{
				// Loop throgh all inputValues to check if ANY have met the specified value
				for (std::list<InputSample>::iterator it = inputSamples.begin(); it != inputSamples.end();)
				{
					InputSample current = *it;
					InputSample previous = *(++it);

					// TODO: Unnecessarily expensive, we can just check InputSamples directly
					if (findIntersectionTime(current, previous, minimumValue).has_value())
					{
						return true;
					}
					if(findIntersectionTime(current, previous, minimumValue).has_value())
					{
						return true;
					}
				}
			}

			return false;
		}

		const float findTimeSpent(float targetValue)
		{
			// We need at least 2 samples
			if (inputSamples.size() > 1)
			{
				return 0;
			}

			float timeSpent = 0;

			// Loop throgh all inputValues and add their spent times together
			for (std::list<InputSample>::iterator it = inputSamples.begin(); it != inputSamples.end();)
			{
				std::optional<float> time = findIntersectionTime(*it, *(++it), targetValue);

				if (time.has_value())
				{
					timeSpent += time.value();
				}
			}

			return timeSpent;
		}

		const static std::optional<float> findIntersectionTime(InputSample previousSample, InputSample currentSample, float targetValue)
		{
			float slope = (currentSample.y - previousSample.y) / (currentSample.x - previousSample.x);

			float yIntercept = previousSample.y - (previousSample.x * slope);

			float intersectionX = (targetValue - yIntercept) / slope;

			// Value never reaches targetValue, or intersection happened before 0, meaning in the previous update iteration
			if (!std::isfinite(intersectionX) || intersectionX < 0)
			{
				return std::nullopt;
			}

			return intersectionX;
		}
		const static std::optional<float> findIntersectionTimeSpent(InputSample previousSample, InputSample currentSample, float targetValue)
		{
			std::optional<float> targetX = findIntersectionTime(previousSample, currentSample, targetValue);
			if (!targetX.has_value())
			{
				return std::nullopt;
			}

			return Vector2(std::abs(currentSample.x - targetX.value()), std::abs(currentSample.y - targetValue)).Length();
		}
	};
	
	class AnalogInputEvent;
	class DigitalInputEvent;
	
	std::map<std::string, AnalogInputEvent*> nameToAnalogInputEvent;	// All AnalogInputEvents
	std::map<std::string, DigitalInputEvent*> nameToDigitalInputEvent;	// All DigitalInputEvents

	// Base InputEvent
	class InputEvent
	{
	public:
		InputEvent(std::string inputName) : name(inputName)
		{

		}

		std::string name;
	};

	struct MinMaxDefault
	{
		float Min;
		float Max;
		float Default;
	};

	MinMaxDefault controllerMixedInput = {-1, 1, 0};			// Values for default mixed positive & negative input
	MinMaxDefault controllerMixedInputFlipped = {1, -1, 0};		// Flipped values for default mixed positive & negative input
	MinMaxDefault digitalPositiveInput = {0, 1, 0};				// Values for default keyboard positive input
	MinMaxDefault digitalNegativeInput = {0, -1, 0};			// Values for default keyboard negative input

	struct InputEventData
	{
		MinMaxDefault minMaxDefault;
		int axis;

		// Define hash function for InputEventData
		struct Hash 
		{
			std::size_t operator()(const InputEventData& eventData) const 
			{
				// Just hash axis
				return std::hash<int>()(eventData.axis);
			}
		};

		// Define equality comparison function for InputEventData
		struct Equal 
		{
			bool operator()(const InputEventData& eventData1, const InputEventData& eventData2) const 
			{
				// Just compare axis
				return eventData1.axis == eventData2.axis;
			}
		};
	};

	static float map_value(float value, float from_min, float from_max, float to_min, float to_max)
	{
		return (value - from_min) / (from_max - from_min) * (to_max - to_min) + to_min;
	}

	class AnalogInputEvent : public InputEvent
	{
	public:
		AnalogInputEvent(std::string inputName) : InputEvent(inputName)
		{
			nameToAnalogInputEvent[name] = this;
		}

		std::map<InputState*, MinMaxDefault> OutputStatesToMinMaxDefaultValues;
		std::map<float*, InputState*> OutputValuesToOutputStates;

		std::unordered_multimap<InputEventData, std::pair<float*, MinMaxDefault>, InputEventData::Hash, InputEventData::Equal> InputEventDataToOutputValues;

		std::map<int, float> totalValues;
		bool totalValuesUpdated = false;

		// Turns all outputStates into outputValues
		// Call after updating all inputs, before handling the input axes
		void turnOutputStatesToOutputValues()
		{
			for (auto OutputValueToOutputState : OutputValuesToOutputStates)
			{
				// Find the min, max and default values for this output
				auto it = OutputStatesToMinMaxDefaultValues.find(OutputValueToOutputState.second);
				if (it != OutputStatesToMinMaxDefaultValues.end())
				{
					// Set as default value to start
					*OutputValueToOutputState.first = it->second.Default;

					switch (*OutputValueToOutputState.second)
					{
					case InputState::Released:
					case InputState::NewRelease:
					case InputState::NewPressNewRelease:
						// Released, set as min value
						*OutputValueToOutputState.first += it->second.Min;
						break;
					case InputState::Pressed:
					case InputState::NewPress:
					case InputState::NewReleaseNewPress:
						// Released, set as max value
						*OutputValueToOutputState.first += it->second.Max;
						break;
					case InputState::None:
					default:
						// State isn't valid, do nothing
						break;
					}
				}
			}
		}

		void inputPollingFinished()
		{
			// We are not guaranteed to keep the same totalValue, as we've finished input polling
			totalValuesUpdated = false;
		}

		const std::map<int, float> getValues()
		{
			if (!totalValuesUpdated)
			{
				// Update total value
				std::map<int, float> output;
				
				for (auto it = InputEventDataToOutputValues.begin(); it != InputEventDataToOutputValues.end(); ++it)
				{
					output[it->first.axis] += input::map_value(*it->second.first, it->second.second.Min, it->second.second.Max, it->first.minMaxDefault.Min, it->first.minMaxDefault.Max);
				}
				totalValues = output;
				totalValuesUpdated = true;
			}
			return totalValues;
		}

		const float getValue(int axis)
		{
			auto temp = getValues();
			return temp[axis];
		}
	};

	class DigitalInputEvent : public InputEvent
	{
	public:
		DigitalInputEvent(std::string inputName) : InputEvent(inputName)
		{
			nameToDigitalInputEvent[name] = this;
		}

		// Every output state of InputButton this is binded to
		std::vector<InputState*> outputStates;

		const bool isPressed() const
		{
			return pressed != eventStates::Not;
		}
		const bool isReleased() const
		{
			return released != eventStates::Not;
		}
		const bool isNewPress() const
		{
			return (pressed == eventStates::NewOn);
		}
		const bool isNewRelease() const
		{
			return (released == eventStates::NewOn);
		}
		const bool isChanged() const
		{
			return (isNewPress() || isNewRelease());
		}

		// Before handling new events, we should refresh the event's state
		void refreshState()
		{
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
		}
	 

	protected:
		eventStates pressed = eventStates::Not;		// Whether key is pressed the update before this poll
		eventStates released = eventStates::Not;	// Whether key is released the update before this poll
	};

	class DigitalInput;
	std::map<inputKey, DigitalInput*> inputKeytoDigitalInput;	// All DigitalInputs

	class DigitalInput
	{
	public:
		// Don't call
		DigitalInput()
		{

		}

		DigitalInput(inputKey key)
		{
			//inputButtons.push_back(this);
			inputKeytoDigitalInput[key] = this;
		}

		InputState buttonInputState = InputState::Released;		// Callback inputted state of key
		InputState buttonOutputState = InputState::Released;	// Pollable state of key

		// Check and handle the change in state
		void checkStateChange()
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
		void callback(int pressed)
		{
			switch(pressed)
			{
				case GLFW_PRESS:
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
					break;
				case GLFW_RELEASE:
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
					break;
				default:
					// Do not handle GLFW_REPEAT
					break;
			}
		}
	};

	class DigitalControllerInput;
	std::map<std::pair<int, int>, DigitalControllerInput*> inputButtonToDigitalControllerInput;	// All DigitalControllerInputs

	class DigitalControllerInput : public DigitalInput
	{
	public:
		int joystick;
		inputButton button;

		DigitalControllerInput(inputButton setButton, int setJoystick)
		{
			joystick = setJoystick;
			button = setButton;
			inputButtonToDigitalControllerInput[{joystick, setButton}] = this;
		}

		void test()
		{
			if (!glfwJoystickPresent(joystick))
			{
				return;
			}

			// Update joystick analog events
			if (glfwJoystickIsGamepad(joystick))
			{
				GLFWgamepadstate state;

				// If joystick is not connected, return
				if (glfwGetGamepadState(joystick, &state) != GLFW_TRUE)
				{
					return;
				}

				if (state.buttons[button])
				{
					pressState(GLFW_PRESS);
				}
				else
				{
					pressState(GLFW_RELEASE);
				}
			}
			else
			{
				int count;

				const unsigned char* buttonsStart = glfwGetJoystickButtons(joystick, &count);

				// If button is not on the controller, return
				if (count <= button)
				{
					std::cout << "WARNING: Controller" << joystick << "'s button " << button << " has been set to an invalid button. The maximum button for this controller is:" << (count - 1) << "\n";
					return;
				}

				// If controller disconnected, return
				if (!buttonsStart)
				{
					return;
				}
				// There is a cosmic chance that at point, the disconnect happens and game crashes. But oh well

				// Set button state
				bool a = !(*(buttonsStart + button) == GLFW_PRESS);

				pressState(a);
			}
		}

		void pressState(int pressed)
		{
			switch (pressed)
			{
			case GLFW_PRESS:
				// Button is currently being pressed, what about other inputs this update cycle?
				switch (buttonInputState)
				{
				case input::InputState::None:				// No input before
				case input::InputState::Released:			// Not pressed last update cycle
				case input::InputState::NewRelease:			// Not pressed last update cycle
				case input::InputState::NewPressNewRelease:	// Not pressed last update cycle
					// There were no conflicting inputs: Set as new press:
					buttonInputState = InputState::NewPress;
					break;
				case input::InputState::NewPress:			// Same input
				case input::InputState::NewReleaseNewPress:	// Same input
					// The input is same as last frame: Downgrade
					buttonInputState = InputState::Pressed;
					break;
				case input::InputState::Pressed:			// Lesser input
					// There were lesser inputs: Keep that input
				default:
					break;
				}
				break;
			case GLFW_RELEASE:
				// Button is currently not being pressed, what about other inputs this update cycle?
				switch (buttonInputState)
				{
				case input::InputState::None:				// No input before
				case input::InputState::Pressed:			// Pressed last update cycle
				case input::InputState::NewPress:			// Pressed last update cycle
				case input::InputState::NewReleaseNewPress:	// Pressed last update cycle
					// There were no conflicting inputs: Set as new release:
					buttonInputState = InputState::NewRelease;
					break;
				case input::InputState::NewRelease:			// Same input
				case input::InputState::NewPressNewRelease:	// Same input
					// The input is same as last frame: Downgrade
					buttonInputState = InputState::Released;
					break;
				case input::InputState::Released:			// Lesser input
					// There were lesser inputs: Keep that input
				default:
					break;
				}
				break;
			default:
				// Do not handle GLFW_REPEAT
				break;
			}

			// Set outputState to be InputState
			buttonOutputState = buttonInputState;
			// Do not reset inputState
		}
	};

	static void unbindInput(DigitalInput* inputButton)
	{
		// TODO:

		// Find all events that use the button's buttonOutputState.
		// Remove the button from them

		// If inputButton has no uses, delete it (not necessary, if engine restarts, the button will not be initialized anymore)
	}

	static void bindDigitalInput(DigitalInput* inputButton, std::vector<DigitalInputEvent*> inputEvents)
	{
		// Bind all specified events to DigitalInput
		for (auto inputEvent : inputEvents)
		{
			inputEvent->outputStates.push_back(&inputButton->buttonOutputState);
		}
	}
	static void bindDigitalInput(inputKey key, std::vector<DigitalInputEvent*> inputEvents)
	{
		// Find inputKey
		auto it = inputKeytoDigitalInput.find(key);

		// Check if the key exists in the map
		if (it != inputKeytoDigitalInput.end())
		{
			bindDigitalInput(it->second, inputEvents);
		}
		else
		{
			// Construct a new inputButton to bind
			bindDigitalInput(new DigitalInput(key), inputEvents);
		}
	}
	static void bindDigitalInput(inputKey key, std::vector<std::string> digitalInputEventNames)
	{
		std::vector<DigitalInputEvent*> digitalInputEventsToBind;

		// Find by DigitalInputEvent name
		for (const auto& digitalInputEventName : digitalInputEventNames)
		{
			auto it = nameToDigitalInputEvent.find(digitalInputEventName);

			// Check if the DigitalInputEvent's name exists in the map
			if (it != nameToDigitalInputEvent.end())
			{
				digitalInputEventsToBind.push_back(it->second);
			}
			else 
			{
				std::cout << "WARNING: No matching DigitalInputEvent found for " << digitalInputEventName << std::endl;
			}
		}

		bindDigitalInput(key, digitalInputEventsToBind);
	}

	static void bindDigitalControllerInput(int joystick, DigitalControllerInput* inputButton, std::vector<DigitalInputEvent*> inputEvents)
	{
		// Bind all specified events to DigitalInput
		for (auto inputEvent : inputEvents)
		{
			inputEvent->outputStates.push_back(&inputButton->buttonOutputState);
		}
	}
	static void bindDigitalControllerInput(int joystick, inputButton button, std::vector<DigitalInputEvent*> inputEvents)
	{
		// Find inputKey
		auto it = inputButtonToDigitalControllerInput.find({joystick, button});

		// Check if the key exists in the map
		if (it != inputButtonToDigitalControllerInput.end())
		{
			bindDigitalControllerInput(joystick, it->second, inputEvents);
		}
		else
		{
			// Construct a new DigitalControllerInput to bind
			bindDigitalControllerInput(joystick, new DigitalControllerInput(button, joystick), inputEvents);
		}
	}
	static void bindDigitalControllerInput(int joystick, inputButton button, std::vector<std::string> digitalInputEventNames)
	{
		std::vector<DigitalInputEvent*> digitalInputEventsToBind;

		// Find by DigitalInputEvent name
		for (const auto& digitalInputEventName : digitalInputEventNames)
		{
			auto it = nameToDigitalInputEvent.find(digitalInputEventName);

			// Check if the DigitalInputEvent's name exists in the map
			if (it != nameToDigitalInputEvent.end())
			{
				digitalInputEventsToBind.push_back(it->second);
			}
			else
			{
				std::cout << "WARNING: No matching DigitalInputEvent found for " << digitalInputEventName << std::endl;
			}
		}

		bindDigitalControllerInput(joystick, button, digitalInputEventsToBind);
	}

	// List that saves all outputvalues for deletion
	std::vector<float*> bindedOutputValues;

	static void bindAnalogInput(DigitalInput* inputButton, MinMaxDefault inputMinMaxDefault, std::map<AnalogInputEvent*, InputEventData> inputEvents)
	{
		// Bind all specified events to DigitalInput
		for (auto& inputEvent : inputEvents)
		{
			// Bind the output state
			inputEvent.first->OutputStatesToMinMaxDefaultValues.insert({&inputButton->buttonOutputState, inputEvent.second.minMaxDefault});
			
			// Create a new output value
			float* outputValue = new float;
			bindedOutputValues.push_back(outputValue);		

			// Add output value to it's axis
			inputEvent.first->InputEventDataToOutputValues.insert({ {inputEvent.second.minMaxDefault, inputEvent.second.axis}, { outputValue, inputMinMaxDefault } });

			// Bind the output value to the output state
			inputEvent.first->OutputValuesToOutputStates.insert({outputValue, &inputButton->buttonOutputState});
		}
	}
	static void bindAnalogInput(inputKey key, MinMaxDefault inputMinMaxDefault, std::map<AnalogInputEvent*, InputEventData> inputEvents)
	{
		// Find inputKey
		auto it = inputKeytoDigitalInput.find(key);

		// Check if the key exists in the map
		if (it != inputKeytoDigitalInput.end())
		{
			bindAnalogInput(it->second, inputMinMaxDefault, inputEvents);
		}
		else
		{
			// Construct a new inputButton to bind
			bindAnalogInput(new DigitalInput(key), inputMinMaxDefault, inputEvents);
		}
	}
	static void bindAnalogInput(inputKey key, MinMaxDefault inputMinMaxDefault, std::vector<std::string> analogInputEventName, int axis, float maxValue = 1, float minValue = 0, float defaultValue = 0)
	{
		std::map<AnalogInputEvent*, InputEventData> AnalogInputEventsToBind;

		// Find by AnalogInputEvent name
		for (const auto& analogInputEventName : analogInputEventName)
		{
			auto it = nameToAnalogInputEvent.find(analogInputEventName);

			// Check if the AnalogInputEvent's name exists in the map
			if (it != nameToAnalogInputEvent.end())
			{
				AnalogInputEventsToBind.insert({it->second, {{minValue, maxValue, defaultValue}, axis}});
			}
			else
			{
				std::cout << "WARNING: No matching AnalogInputEvent found for " << analogInputEventName << "\n";
			}
		}

		bindAnalogInput(key, inputMinMaxDefault, AnalogInputEventsToBind);
	}

	class AnalogControllerInput;
	std::map<int, AnalogControllerInput*> joystickToAnalogInput;	// All AnalogInputs

	class AnalogControllerInput
	{
	public:
		// n-dimensional analog input
		std::map<int, float> axisToValue;

		float min = -1;
		float max = 1;

		AnalogControllerInput(float joystick)
		{
			joystickToAnalogInput[joystick] = this;
		}

		void setAxis(int axis, float value)
		{
			axisToValue[axis] = std::clamp(value, min, max);
		}
		void setAxis(std::vector<float> values)
		{
			int i = 0;
			for (float value : values)
			{
				setAxis(i, value);
				i++;
			}
		}
		void setAxis(float values[6])
		{
			for (size_t i = 0; i < 6; i++)
			{
				setAxis(i, values[i]);
			}
		}
		const float getAxisValue(int axis)
		{
			return axisToValue[axis];
		}
	};

	void bindAnalogControllerInput(AnalogControllerInput* inputButton, std::vector<InputEventData> axisEventDatas, std::vector<AnalogInputEvent*> inputEvents)
	{
		// Bind all specified events to AnalogInput
		for (auto inputEvent : inputEvents)
		{
			// Bind all specified axes to event
			for (auto& axisEventData : axisEventDatas)
			{
				inputEvent->InputEventDataToOutputValues.insert({ {axisEventData.minMaxDefault, axisEventData.axis}, {&inputButton->axisToValue[axisEventData.axis], controllerMixedInput} });
			}
		}
	}
	static void bindAnalogControllerInput(int joystick, std::vector<InputEventData> axisEventDatas, std::vector<AnalogInputEvent*> inputEvents)
	{
		// Find inputKey
		auto it = joystickToAnalogInput.find(joystick);

		// Check if the key exists in the map
		if (it != joystickToAnalogInput.end())
		{
			bindAnalogControllerInput(it->second, axisEventDatas, inputEvents);
		}
		else
		{
			// Construct a new AnalogControllerInput to bind
			bindAnalogControllerInput(new AnalogControllerInput(joystick), axisEventDatas, inputEvents);
		}
	}
	static void bindAnalogControllerInput(int joystick, std::vector<InputEventData> axisEventDatas, std::vector<std::string> analogInputEventNames)
	{
		std::vector<AnalogInputEvent*> digitalInputEventsToBind;

		// Find by AnalogInputEvent's name
		for (const auto& analogInputEventName : analogInputEventNames)
		{
			auto it = nameToAnalogInputEvent.find(analogInputEventName);

			// Check if the AnalogInputEvent's name exists in the map
			if (it != nameToAnalogInputEvent.end())
			{
				digitalInputEventsToBind.push_back(it->second);
			}
			else
			{
				std::cout << "WARNING: No matching AnalogInputEvent found for " << analogInputEventName << std::endl;
			}
		}

		bindAnalogControllerInput(joystick, axisEventDatas, digitalInputEventsToBind);
	}

	// Function prototype for the key callback
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
		// Find InputButton is O(logN), bruteforce checking through each individually is O(N).
		// Futhermore, find InputButton happens only when there is a press, bruteforce happens every frame
		auto it = inputKeytoDigitalInput.find(key);

		// Call input button's callback logic
		if (it != inputKeytoDigitalInput.end())
		{
			it->second->callback(action);
		}
	}

	static void gamePadAxisTest(int joystick)
	{
		int count;

		// Find joystick
		auto it = joystickToAnalogInput.find(joystick);
		if (it == joystickToAnalogInput.end())
		{
			return;
		}
		// Get joystick state
		GLFWgamepadstate state;
		if (glfwGetGamepadState(joystick, &state) != GLFW_TRUE)
		{
			return;
		}

		// Set axis values
		it->second->setAxis(state.axes);
	}

	static void customAxisTest(int joystick, int offset = 1)
	{			
		int count;

		// Find joystick
		auto it = joystickToAnalogInput.find(joystick);
		if (it == joystickToAnalogInput.end())
		{
			return;
		}

		const float* axesStart = glfwGetJoystickAxes(joystick, &count);

		// If controller disconnected, return
		if (!axesStart)
		{
			return;
		}
		// There is a cosmic chance that at point, the disconnect happens and game crashes. But oh well

		// Set axis values
		it->second->setAxis({ *axesStart, *(axesStart + offset) });
	}

	// Sets window with key callbacks
	// Place as late into initialization as possible to increase load times while pressing keys
	// Should be a singleton, lazy-evaluation calls this late as possible
	static void initialize(GLFWwindow* window)
	{
		// Set the key callback function
		glfwSetKeyCallback(window, keyCallback);
	}
	// Updates all inputs. Call BEFORE polling input events (such as before gameloop)
	static void update()
	{
		// Update DigitalInputEvents
		for (auto it = nameToDigitalInputEvent.begin(); it != nameToDigitalInputEvent.end(); ++it)
		{
			it->second->refreshState();
		}

		// Read all new events
		glfwPollEvents();

		// Update joysticks
		for (size_t i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			if (glfwJoystickPresent(i))
			{
				// Update joystick analog events
				if (glfwJoystickIsGamepad(i))
				{
					gamePadAxisTest(i);
				}
				else
				{
					customAxisTest(i);
				}
			}
		}

		// Update binded DigitalControllerInput
		for (auto it = inputButtonToDigitalControllerInput.begin(); it != inputButtonToDigitalControllerInput.end(); ++it)
		{
			it->second->test();
		}

		// Update binded DigitalInputs
		for (auto it = inputKeytoDigitalInput.begin(); it != inputKeytoDigitalInput.end(); ++it)
		{
			it->second->checkStateChange();
		}
		// Update binded AnalogInputEvents
		for (auto it = nameToAnalogInputEvent.begin(); it != nameToAnalogInputEvent.end(); ++it)
		{
			it->second->inputPollingFinished();
			// Update values
			it->second->turnOutputStatesToOutputValues();
		}
	}
	// Frees memory used by the input system
	static void uninitialize()
	{
		// Free DigitalInputEvents
		for (auto it = nameToDigitalInputEvent.begin(); it != nameToDigitalInputEvent.end(); ++it)
		{
			delete it->second;
		}
		nameToDigitalInputEvent.clear();

		// Free AnalogInputEvents
		for (auto it = nameToAnalogInputEvent.begin(); it != nameToAnalogInputEvent.end(); ++it)
		{
			delete it->second;
		}
		nameToAnalogInputEvent.clear();

		// Free DigitalInputs
		for (auto it = inputKeytoDigitalInput.begin(); it != inputKeytoDigitalInput.end(); ++it)
		{
			delete it->second;
		}

		// Free AnalogInputs
		for (auto it = joystickToAnalogInput.begin(); it != joystickToAnalogInput.end(); ++it)
		{
			delete it->second;
		}

		// Free bindedOutputValues
		{
			for (auto outputValue : bindedOutputValues)
			{
				delete outputValue;
			}
		}
	}

	static void ConstructAnalogEvent(std::string name)
	{
		new input::AnalogInputEvent(name);	// memory is freed in uninitialize()
	}
	static void ConstructDigitalEvent(std::string name)
	{
		new input::DigitalInputEvent(name);	// memory is freed in uninitialize()
	}

	static void ConstructAnalogEvents(std::vector<std::string> Eventnames)
	{
		for (auto& eventName : Eventnames)
		{
			ConstructAnalogEvent(eventName);
		}
	}
	static void ConstructDigitalEvents(std::vector<std::string> Eventnames)
	{
		for (auto& eventName : Eventnames)
		{
			ConstructDigitalEvent(eventName);
		}
	}

	static const std::optional<std::map<int, float>> GetInputValues(std::string eventName)
	{
		for (auto& it : nameToAnalogInputEvent)
		{
			if (it.second->name == eventName)
			{
				return it.second->getValues();
			}
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return std::nullopt;
	}
	static const float GetInputValue(std::string eventName, int axis)
	{
		for (auto& it : nameToAnalogInputEvent)
		{
			if (it.second->name == eventName)
			{
				return it.second->getValue(axis);
			}
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return 0;
	}
	static const float GetTotalInputValue(std::string eventName, std::vector<int> axes)
	{
		float inputValue = 0;

		for (auto& it : nameToAnalogInputEvent)
		{
			if (it.second->name == eventName)
			{
				for (int axis : axes)
				{
					inputValue += it.second->getValue(axis);
				}
				return inputValue;
			}
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return 0;
	}
	static const float GetTotalInputValue(std::string eventName)
	{
		float totalValue = 0;
		for (auto& it : nameToAnalogInputEvent)
		{
			if (it.second->name == eventName)
			{
				for (auto& axisToValue : it.second->getValues())
				{
					totalValue += axisToValue.second;
				}
				return totalValue;
			}
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return 0;
	}

	static bool GetNewPress(std::string eventName)
	{
		for (auto& it : nameToDigitalInputEvent)
		{
			if (it.second->name == eventName)
			{
				return it.second->isNewPress();
			}
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return false;
	}
	static bool GetPressed(std::string eventName)
	{
		for (auto& it : nameToDigitalInputEvent)
		{
			if (it.second->name != eventName)
			{
				continue;
			}
			return it.second->isPressed();
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return false;
	}
	static bool GetReleased(std::string eventName)
	{
		for (auto& it : nameToDigitalInputEvent)
		{
			if (it.second->name != eventName)
			{
				continue;
			}
			return it.second->isReleased();
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return false;
	}
	static bool GetNewRelease(std::string eventName)
	{
		for (auto& it : nameToDigitalInputEvent)
		{
			if (it.second->name != eventName)
			{
				continue;
			}
			return it.second->isNewRelease();
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return false;
	}
	static bool GetChanged(std::string eventName)
	{
		for (auto& it : nameToDigitalInputEvent)
		{
			if (it.second->name != eventName)
			{
				continue;
			}
			return it.second->isChanged();
		}
		std::cout << "WARNING: Attempting to test state of unknown event: " << eventName << "\n";

		return false;
	}
}
