#ifndef TTYD_SOUND_H
#define TTYD_SOUND_H

#include <cstdint>

// Partially taken from the TTYD doldecomp repo:
// https://github.com/doldecomp/ttyd/blob/main/include/sound.h

extern "C"
{
    // SoundAIDMACallback
    // SoundSSMainInt
    // SoundSSMain
    // SoundSSMuteOff
    // SoundSSMuteOn
    // SoundSSCheck
    // SoundSSSetPlayFreqCh
    // SoundSSSetSrndPanCh
    // SoundSSGetVolCh
    // SoundSSSetVolCh
    // SoundSSSetPanCh
    // SoundSSContinueCh
    // SoundSSFadeoutCh
    void SoundSSStopCh(int32_t chan);
    // SoundSSPlayChEx_main
    // SoundSSPlayChEx
    // SoundSSPlayCh
    // _sscallback
    // ssDecodeDPCM
    // _ssDVDReadAsync_cache_next
    // _ssDVDReadAsync_cache_aram
    // cache_flush
    // _ssDVDReadAsync_activeChk
    // SoundDVDMain
    // SoundEfxMain
    // SoundEfxCheck
    // SoundEfxSetLPF
    // SoundEfxSetSrndPan
    // SoundEfxSetPan
    // SoundEfxSetAux1
    // SoundEfxGetVolume
    // SoundEfxSetVolume
    // SoundEfxSetPitch
    void SoundEfxStop(int32_t chan);
    int32_t SoundEfxPlayEx(int32_t soundId, uint8_t a2, uint8_t vol, uint8_t pan);
    // SoundSongCheck
    // SoundSongGetVolCh
    // SoundSongSetVolCh
    // SoundSongFadeinCh
    // SoundSongFadeoutCh
    void SoundSongStopCh(int32_t chan);
    // SoundSongContinueCh
    // SoundSongPlayCh
    // SoundCloseCover
    // SoundOpenCover
    // SoundSetOutputMode
    // SoundDropData
    // SoundSLibLoadDVD
    // SoundLoadDVD2Free
    // SoundLoadDVD2PushGroup
    // SoundLoadDVD2
    // loadDVD_callback
    // SoundSetFadeTime
    // SoundMainInt
    // SoundMain
    // SoundInit
    // sndFree
    // sndMalloc
}

#endif
