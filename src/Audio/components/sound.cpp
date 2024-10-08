#include <GameEngineFramework/Audio/components/sound.h>

extern bool isAudioDeviceActive;


Sound::Sound() : 
    mBuffer(0),
    mSource(0),
    mIsBufferConstructed(false)
{
}

Sound::~Sound() {
    
    if (mIsBufferConstructed) {
        alDeleteBuffers(1, &mBuffer);
        alDeleteSources(1, &mSource);
    }
    
    return;
}

void Sound::Play(void) {
    
    if (!mIsBufferConstructed) 
        return;
    
    alSourcei(mSource, AL_BUFFER, mBuffer);
    
    alSourcePlay(mSource);
    
    return;
}

void Sound::Stop(void) {
    
    if (!mIsBufferConstructed) 
        return;
    
    alSourceStop(mSource);
    
    return;
}

void Sound::SetVolume(float volume) {
    
    if (!mIsBufferConstructed) 
        return;
    
    alSourcef(mSource, AL_GAIN, volume);
    
    return;
}

void Sound::SetPitch(float pitch) {
    
    if (!mIsBufferConstructed) 
        return;
    
    alSourcef(mSource, AL_PITCH, pitch);
    
    return;
}

bool Sound::IsSamplePlaying(void) {
    
    if (!mIsBufferConstructed) 
        return false;
    
    ALint state;
    
    alGetSourcei(mSource, AL_SOURCE_STATE, &state);
    
    return state == AL_PLAYING;
}

bool Sound::LoadSample(AudioSample* samplePtr) {
    
    if (!isAudioDeviceActive) 
        return false;
    
    if (!mIsBufferConstructed) {
        
        alGenBuffers(1, &mBuffer);
        alGenSources(1, &mSource);
        
        mIsBufferConstructed = true;
    }
    
    unsigned int bufferSize = samplePtr->sampleBuffer.size() * sizeof(ALshort);
    
    alBufferData(mBuffer, AL_FORMAT_MONO16, samplePtr->sampleBuffer.data(), bufferSize, samplePtr->sample_rate);
    
    return true;
}

