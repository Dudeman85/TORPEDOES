#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <engine/AL/SoundSource.h>
class SoundDevice
{
public:
	static SoundDevice* getDevice();
	static void Init();

	void GetLocation(float& x, float& y, float& z);
	void GetSourceLocation(const SoundSource& source, float& x, float& y, float& z);
	void GetOrientation(float& ori);
	float GetGain();

	void SetLocation(const float& x, const float& y, const float& z);
	void SetSourceLocation(const SoundSource& source, const float& x, const float& y, const float& z);
	void SetOrientation(const float& horx, const float& hory, const float& horz,
		const float& verx, const float& very, const float& verz);
	void SetGain(const float& val);

private:
	SoundDevice();
	~SoundDevice();

	ALCdevice* p_ALCDevice;
	ALCcontext* p_ALCContext;
	ALuint sourceID;

}; 