#include <GameEngineFramework/Audio/AudioSystem.h>

#include <GameEngineFramework/Logging/Logging.h>

#include <Audioclient.h>
#include <Audiopolicy.h>

#include <mmdeviceapi.h>

extern Logger Log;


// Audio thread
bool isAudioThreadActive = true;
void AudioThreadMain(void);


void AudioSystem::Initiate(void) {
    
    // Launch the audio thread
    audioThread = new std::thread( AudioThreadMain );
    
    Log.Write( " >> Starting thread audio" );
    
    return;
}

void AudioSystem::Shutdown(void) {
    
    isAudioThreadActive = false;
    
    audioThread->join();
    
    return;
}



//
// Audio thread entry point
//

void AudioThreadMain(void) {
    
    while (isAudioThreadActive) {
        
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(2) );
    Log.Write(" >> Shutting down on thread audio");
    
    return;
}


