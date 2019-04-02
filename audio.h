/*************************************************************************
 *
 *
 *    File name   : audio.h
 *    Description : audio function header
 *                  this modules are wraped whith st's library
 *                  so you need to access only this function.
 *    $Revision: 39 $
 **************************************************************************/
#ifndef __AUDIO_H
#define __AUDIO_H

#include "includes.h"

#define SOUND_VOL_STEPS       10
#define SOUND_VOL_MIN         (WM8731_LHO_LHPVOL_Min-1)
#define SOUND_VOL_MAX         (WM8731_LHO_LHPVOL_Max)
#define SOUND_VOL_STEP       ((SOUND_VOL_MAX-SOUND_VOL_MIN)/SOUND_VOL_STEPS)
#define WM8731_VOL_CONV(x)    (SOUND_VOL_MIN + SOUND_VOL_STEP*x)
#define SINE_BUFFER_SIZE      441
#define PI                    3.14159265
#define REC_BUFFER_TIME       1
#define REC_BUFFER_SIZE       (44100*REC_BUFFER_TIME)
#define REC_BUFFER_ALLOW_PLAY (REC_BUFFER_SIZE-100)

typedef int16_t AudioChannelSample16b_t;

#pragma pack(1)
typedef struct 
{
  AudioChannelSample16b_t leftChannel;
  AudioChannelSample16b_t rightChannel;
} AudioSample16b_t;
#pragma pack()

typedef enum 
{
  stLoopbackMic,
  stLoopbackLineIn,
  stGenerateSine,
  NumberOfPlaybackStates
} PlaybackState_t;


void AudioInit(void);

void GenerateSineWave(void);
AudioChannelSample16b_t GetSineSample(void);
AudioChannelSample16b_t GetRecSample(void);
uint32_t SaveRecSample(AudioChannelSample16b_t sample);
void InitPlaybackState(PlaybackState_t state);
void Save_Audio_Rx(Int16U *  Audio_Rx_Buf, Int8U *I2S_int_cnt);
void I2S2Handler(void);

#endif