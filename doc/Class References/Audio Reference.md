# Audio Reference

***This engine uses OpenAL. You might need to download OpenAL 1.1 Windows Installer from their website***

***All audio functions are included through Application.h. No other includes needed***
```cpp
#include <engine/Application.h>
```

## Loading and playing sound effects

To play sound effects in your program, you need to create new sound source. If you want to play multiple sound effects simultaniously, you need to create multiple sources.

```cpp
static SoundSource mySpeaker1;
static SoundSource mySpeaker2;
static SoundSource mySpeaker3;
static SoundSource mySpeaker4;
	
```

To load sound files, use following functions:
```cpp
//load sound from file
uint32_t sound1 = SoundBuffer::getFile()->addSoundEffect("assets/jump.wav");
uint32_t sound2 = SoundBuffer::getFile()->addSoundEffect("assets/sound100.wav");
```
Engine only supports .wav files.

To play the sound, use Soundsource play function. You can set sound to loop with set looping function.
```cpp
//play sound files
mySpeaker1.Play(sound1);
//sets sound to loop, value 1=true
mySpeaker1.SetLooping(1);
```


## Loading and playing Music

Music system is very similar to sound effects:
```cpp
//Load music from file
MusicBuffer myMusic("assets/forest.wav");
//play music
myMusic.Play();
```
To set music volume:
```cpp
//music volume
myMusic.SetVolume(0.5f);
```

music file is divided into buffers, to play whole audio file, you need to add following into the game loop:
```cpp
myMusic.updateBufferStream();
```


## Implementing 3D sounds

Engine supports full 3D sounds. However, .wav files need to be in mono in order to work.
First you need to set listener location and orientation:
```cpp
//sets listener location on map, x,y,z
engine.soundDevice->SetLocation(0, 0, 0);
//sets listener orientation, horizontal x,y,z, vertical x,y,z
engine.soundDevice->SetOrientation(0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
```
If you want to bind listener location to player character, you need to make definition inside game loop, for example:
```cpp
Transform playerTransform = ecs.getComponent<Transform>(player);
engine.soundDevice->SetLocation(playerTransform.x, playerTransform.y, playerTransform.z);
engine.soundDevice->SetOrientation(0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
```



Engine has 3 different distance models, Linear, Inverse and exponential.
Clamped versions are also available.
Please note you can only use one model in your application.
```cpp
// different attenuation models, only use 1
//Linear models
mySpeaker1.setLinearDistance(1.f, 100.f, 600.f, 1.f);
mySpeaker1.setLinearDistanceClamped(1.f, 100.f, 600.f, 1.f);
//Inverse models
mySpeaker1.setInverseDistance(1.f, 100.f, 600.f, 1.f);
mySpeaker1.setInverseDistanceClamped(1.f, 100.f, 600.f, 1.f);
//Exponential models
mySpeaker1.setExponentialDistance(1.f, 100.f, 600.f, 1.f);
mySpeaker1.setExponentialDistanceClamped(1.f, 100.f, 600.f, 1.f);
```
Parameters given in the example are following:
sourceID, volume, reference distance, max distance, rolloff factor
Reference distance is the distance in pixels where sound volume is 1.
In clamped models volume stays at 1 inside reference area, while in unclamped models volume keeps increasing.
Max distance is the limit for roll off factor to affect sound volume.
Roll off dictates how fast volume decreases, larger value = faster drop.


To set sound location in your application, use following:
```cpp
//sets source location.
engine.soundDevice->SetSourceLocation(mySpeaker1, 0.f, 0.f, 0.f);
```
Given parameters are: SourceId, x, y, z values.
Source id needs to be same as soundSource in order to work properly.
If you want to bind sound to a moving object in application, you need to make definition inside game loop, for example:
```cpp
//sets source location to sprite2
Transform sprite2Transform = ecs.getComponent<Transform>(sprite2);
engine.soundDevice->SetSourceLocation(mySpeaker1, sprite2Transform.x, sprite2Transform.y, 20.f);
```
Adding small z value balances sound orientation changes in very close ranges.



## Additional functions

Music and sound effects support pause and resume functions. use these in game loop.
```cpp
myMusic.Pause();
mySpeaker1.Pause();

myMusic.Resume();
mySpeaker1.Resume();
```

You can create basic volume control with following commands
```cpp
//initialize volume at the beginning of main function
float volume =0.2f;

//inside game loop
if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)//volume down when 'n' is pressed
	{
		volume -= 0.01f;
		if (volume < 0.0f) volume = 0.0f; // Clamp the volume to a minimum of 0.0f
		myMusic.SetVolume(volume);
	}
if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)//volume up when 'm' is pressed
	{
		volume += 0.01f;
		if (volume > 1.0f) volume = 1.0f; // Clamp the volume to a max 1.0f
		myMusic.SetVolume(volume);
	}
```
