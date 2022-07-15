#ifndef DEF_ite66121_h_
#define DEF_ite66121_h_

#include <VXIS_Extern_Define.h>

extern void HDMI_Tx_Init(void ) ; 
extern void HDMI_Tx_SetAudioFs(tByte AudioSampleFreq ) ;
extern void HDMI_Tx_SetAudioMute( bit Mute ) ; 
extern void HDMITX_DevLoopProc();

#endif 

