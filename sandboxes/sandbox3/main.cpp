#include <engine/Application.h>
#include <engine/AudioEngine.h>

int main()
{
	AudioEngine ae;

	Audio* testAudio = ae.createAudio("bang_09.wav", true, 1, Vector3(10, 0, 0));
	Audio* testAudio2 = ae.createAudio("engine_heavy_loop.wav", true, 1, Vector3(-10, 0, 0));

	testAudio->play();
	testAudio2->play();

	while (true)
	{

	}

	return 0;
}
