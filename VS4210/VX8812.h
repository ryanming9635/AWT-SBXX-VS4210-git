#ifndef VX8812_H
#define VX8812_H
#include "VXIS_Extern_Define.h"

//-----------------------------------------------------------------------------
// extern Function Prototypes
//-----------------------------------------------------------------------------
extern void VXIS_fVX8812_Set_HDMI_in_SD_Video_Out(tByte tbMode);
extern void VX8812_BlueScreen(void);
void SET_VX8812_SD_Video_Out(tByte tbMode);
extern void GraphicsPrint(PRINT_COLOR_e mode, const char *Message, ...);

#endif
