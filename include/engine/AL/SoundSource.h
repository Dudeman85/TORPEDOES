#pragma once
#include <AL/al.h>
class SoundSource
{
public:
	SoundSource();
	~SoundSource();

	void Play(const ALuint& buffer_to_play);
	void Stop();
	void Pause();
	void Resume();

	void SetBufferToPlay(const ALuint& buffer_to_play);
	void SetLooping(const bool& loop);

	void setPitch(float pitch);
	void setVolume(float volume);
	void setVelocity(float x, float y, float z);
	void setInverseDistance(/*int sourceID, */float volume, float refdistance, float maxdistance, float rolloff);
	void setLinearDistance(/*int sourceID, */float volume, float refdistance, float maxdistance, float rolloff);
	void setExponentialDistance(/*int sourceID, */float volume, float refdistance, float maxdistance, float rolloff);
	void setInverseDistanceClamped(/*int sourceID, */float volume, float refdistance, float maxdistance, float rolloff);
	void setExponentialDistanceClamped(/*int sourceID, */ float volume, float refdistance, float maxdistance, float rolloff);
	void setLinearDistanceClamped(/*int sourceID, */ float volume, float refdistance, float maxdistance, float rolloff);
	bool isPlaying();

	ALuint getSourceId() const {
		return p_Source;
	}

private:
	SoundSource(const SoundSource&);
	SoundSource& operator=(const SoundSource&);

	ALuint p_Source;
	ALuint p_Buffer = 0;
	float Volume;
	float RollOff;
	float refDistance;
	float maxDistance;
	ALuint sourceID;
};