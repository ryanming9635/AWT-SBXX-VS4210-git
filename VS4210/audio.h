
#define MAX_SOUND_VOLUME 50
#define CAMA_AUD 0
#define CAMB_AUD 1
#define CAMC_AUD 2
#define CAMR_AUD 3

extern BYTE AudioVol;
extern BYTE PCAudioVol;

struct RegisterInfo
{
   int Min;
   int Max;
   int Default;
};

// BYTE ResetMSP(void);
// void SetMSPSCARTPrescale(void);
// void SetMSPSourceSel(void);
// void SetMSPVol(void);

// BYTE AudioAutoStdDetection(void);
// BYTE SteroMonoDetection(void);

// BYTE TunerAudio(void);
// void ExtAudio(void);
// id PCAudio(void);
// id MuteOn(void);
// BYTE GetAudioVol(void);
BYTE ChangeVol(char);
void AUDIO_SELECT(BYTE sel);
void AUDIO_SELECT_MUTE(BYTE sel);

// BYTE GetAudioBalance(void);
// BYTE ChangeBalance(char);
// BYTE GetAudioBass(void);
// BYTE ChangeBass(char);
// BYTE GetAudioTreble(void);
// BYTE ChangeTreble(char);
// BYTE GetAudioEffect(void);
// BYTE ChangeAudioEffect(char amount);
// void ToggleAudioMute(void);
// void CheckAndClearMuteOn(void);

// BYTE IsAudioMuteOn(void);
// void SetAudioMute(BYTE on );
// void ChangeMTS(BYTE type);
// void AudioOff(void);
void AudioMute(bit mute);
void SetAudioVolEE(BYTE vol);
extern void Wait_ms(WORD Tms);
extern void SET_ALC122(BYTE addr, WORD val);
extern BYTE Mapping1(int fromValue, CODE_P struct RegisterInfo *fromRange, int *toValue, CODE_P struct RegisterInfo *toRange);

//#define AudioOn()			SetMSPVol()
// void ResetAudioValue();
