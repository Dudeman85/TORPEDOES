# Audio Reference

***This engine uses MiniAudio.io.**

***All audio functions are included through AudioEngine.h. No other includes needed***
```cpp
#include <engine/AudioEngine.h>
```

## Loading and playing sound effects

To load and play sound effects, you must first create an audioengine. You can think of audioengines as the sound bars in a video game settings menu: You can individually change the volume of different audioengines.

```cpp
AudioEngine musicEngine(1);
// Initialize dialogue with ambient volume of 0.4 
AudioEngine dialogueEngine(0.4);
AudioEngine sfxEngine(1);

// Set the relative volume of all sounds played by sxfEngine
sfxEngine.setAmbientVolume(0.2);
```

Second, you need to load a sound. There is only support for .wav files.

```cpp
// Create a new audio for sfxEngine
Audio jumpAudio = sfxEngine.createAudio("assets/jump.wav")
// Create audio with full initializer list: name, loop, volume, location
Audio walkAudio = sfxEngine.createAudio("assets/walk.wav", true, 0.8, Vector3(1,0,0))
```

In order to play a loaded sound, you call the appropirate function:

```cpp
// Play the jump sound effect
jumpAudio.play()
```

Audio also has functions for pausing, unpausing, stopping, loop control, volume control and direction control.

Audio is not thread-safe, and is not guaranteed to stay valid for duration of the program.

## Implementing 3D sounds

By default, listener location is 0,0,0.

The audio direction uses same global coordinates, and is not relative to a listener location.

```cpp
// Set jump audio to play from location 1,0,0
jumpAudio->setDirection(Vector3(1,0,0));
