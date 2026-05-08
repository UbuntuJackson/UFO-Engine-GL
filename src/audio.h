#pragma once

namespace ufo{

class Audio{
public:
    MIX_Audio* audio = nullptr;

    void Play(){
        MIX_SetTrackAudio(track, audio);

        MIX_PlayTrack(track, 0)
    }
};

}
