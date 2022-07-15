#ifndef _VS4210_H_
#define _VS4210_H_

#include "VXIS_Extern_Define.h"

#define REG_SC1_A1 0xa1
#define REG_PRES3_2D 0x2D

#if (TH0701024600NYR50S1 == OFF)
#define No_Sgnal_Blue_Screen (0x08 | 0x02)
#else
#define No_Sgnal_Blue_Screen (0x08 | 0x01)
#endif
extern tByte gbVXIS_OuputSize;
// extern tByte gaOpenWindowflg[4];
// extern tByte bSignal[4] ;
// extern txByte xchMapl[4] ;

extern tByte g4210VinIndex;
extern tByte gbVXIS_OuputModeflg;
extern tByte gbVXIS_InputMode[4];
extern tByte gaChannel_MapWindowIndex[4];

extern tByte u8gDrawOSDFlg; //= 0x03 ; //Title+Bar
extern tByte u8DrawBoxFlg;
extern tByte u8DrawBoxColor;
extern tByte gu8AudioMode;

extern tcByte Str_OSD_TRI_LEFT[];
extern tcByte Str_OSD_TRI_RIGHT[];
extern tcByte Str_OSD_TRI_CAMC[];
extern tcByte Str_OSD_TRI_CAMR[];

// extern void VS4210_SetInputSizeHV(tByte ch ,tWord HActive , tWord VActive , tWord HTotal, tWord VTotal  ) ;

extern tByte VS4210_SetInModeOutMode(tByte ch, tByte inmode, tByte outmode);
extern void VS4210_Rx_NoSignal(tByte ch);
extern bit VS4210_SetMutiWindows(T_VS4210_MutiWindows *pVS4210_MutiWindows);
void VS4210_SetCamMirror(tByte ch, tByte win);
void VS4210_CLRCamMirror(tByte ch, tByte win);
void VS4210_ResetCamMirror(tByte ch);

#endif
