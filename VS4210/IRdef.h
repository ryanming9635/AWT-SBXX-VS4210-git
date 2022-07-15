#ifndef _IR_DEF_H
#define _IR_DEF_H

#include "VXIS_Extern_Define.h"

#define kCustomCodeH 0x00
#define kCustomCodeL 0xFF

#define _IR_KEY_1CH_Show        0x19  // 1
#define _IR_KEY_2CH_Show        0x1B  // 2
#define _IR_KEY_3CH_Show        0x11  // 3
#define _IR_KEY_4CH_Show        0x15  // 4
#define _IR_KEY_xCH_PREVIOUS    0x50  // <
#define _IR_KEY_xCH_LAST        0x4A  // >
#define _IR_KEY_AUDIO_MODE      0x47  // 
#define _IR_KEY_MODE            0x07  // 4Windows->H-Windows->M6-W(1234)->M6-W(2341)->M6-W(3412)->M6-W(4123)->M12(12)->M12(34) 
#define _IR_KEY_CHANGE_OUTSIZE  0x02  // 1080/720
#define _IR_KEY_4_Split         0x40  // Quad-view

#define _IR_KEY_CheckCode  0xFE

//-----------------------------------------------------------------------------
// extern Function Prototypes
//-----------------------------------------------------------------------------
extern void IR_Process(tByte IRcode) ; 


#endif
