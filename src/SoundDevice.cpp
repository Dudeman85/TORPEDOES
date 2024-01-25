#include <engine/AL/SoundDevice.h>
#include <AL/al.h>
#include <stdio.h>
#include <vector>

static SoundDevice* _instance = nullptr;

SoundDevice* SoundDevice::getDevice()
{
	static SoundDevice* snd_device = new SoundDevice();
	return snd_device;
}

void SoundDevice::Init()
{
	if (_instance == nullptr)
		_instance = new SoundDevice();
}

void SoundDevice::GetLocation(float& x, float& y, float& z)
{
	alGetListener3f(AL_POSITION, &x, &y, &z);
}

void SoundDevice::GetSourceLocation(const SoundSource& source, float& x, float& y, float& z)
{
	alGetSource3f(source.getSourceId(), AL_POSITION, &x, &y, &z);
	
}

void SoundDevice::GetOrientation(float& ori)
{
	alGetListenerfv(AL_ORIENTATION, &ori);
}

float SoundDevice::GetGain()
{
	float curr_gain;
	alGetListenerf(AL_GAIN, &curr_gain);
	return curr_gain;
}

///#define AL_INVERSE_DISTANCE                      0xD001
///#define AL_INVERSE_DISTANCE_CLAMPED              0xD002
///#define AL_LINEAR_DISTANCE                       0xD003
///#define AL_LINEAR_DISTANCE_CLAMPED               0xD004
///#define AL_EXPONENT_DISTANCE                     0xD005
///#define AL_EXPONENT_DISTANCE_CLAMPED             0xD006


void SoundDevice::SetLocation(const float& x, const float& y, const float& z)
{
	alListener3f(AL_POSITION, x, y, z);
}

void SoundDevice::SetSourceLocation(const SoundSource& source, const float& x, const float& y, const float& z)
{
	alSource3f(source.getSourceId(), AL_POSITION, x, y, z);
}

void SoundDevice::SetOrientation(const float& horx, const float& hory, const float& horz,
	const float& verx, const float& very, const float& verz)
{
	std::vector<float> ori;
	ori.push_back(horx);
	ori.push_back(hory);
	ori.push_back(horz);
	ori.push_back(verx);
	ori.push_back(very);
	ori.push_back(verz);
	alListenerfv(AL_ORIENTATION, ori.data());
}

void SoundDevice::SetGain(const float& val)
{
	float newVol = val;
	if (newVol < 0.f)
		newVol = 0.f;
	else if (newVol > 5.f)
		newVol = 5.f;

	alListenerf(AL_GAIN, newVol);
}


SoundDevice::SoundDevice()
{
	p_ALCDevice = alcOpenDevice(nullptr); //nullptr = default sound device
	if (!p_ALCDevice)
		throw("failed to get sound device");

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr); //create context
	if (!p_ALCContext)
		throw("Failed to set sound context");

	if (!alcMakeContextCurrent(p_ALCContext))   // make context current
		throw("failed to make context current");

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
		name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
	printf("Opened \"%s\"\n", name);
}

SoundDevice::~SoundDevice()
{
	if (!alcMakeContextCurrent(nullptr))
		printf("failed to set context to nullptr");

	alcDestroyContext(p_ALCContext);
	if (p_ALCContext)
		printf("failed to unset during close");

	if (!alcCloseDevice(p_ALCDevice))
		printf("failed to close sound device");
}