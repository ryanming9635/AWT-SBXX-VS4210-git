#ifndef _VX_OSD_Simple_H
#define _VX_OSD_Simple_H
#include "VXIS_Extern_Define.h"

// Type of MenuItem
/*
#define OSDMENU			1
#define CHOICEMENU		2
#define ACTION			3
#define BAR				4
#define NUMBER			5
#define CHOICE			6
#define ENUM_STRING		7 //YESNO			7
#define CHOICE1			8
#define TIME			9
#define NUMBER_STRING	10
#define TEXT_STR              11   // V1.1 02/28/07 simonsung
#define INFORMATION              12
#define TEXT_STR_exit	13
*/
#define PanelH 838
#define FontWidthX2 32
#define FontWidthSIZE 32  // 24
#define FontWidthSIZE2 16 // 24

#define TIRE_High_PSI_MAX 70
#define TIRE_High_PSI_MIN 50
#define TIRE_Low_PSI_MAX 40
#define TIRE_Low_PSI_MIN 20
#define TIRE_High_TEMP_MAX 60
#define TIRE_High_TEMP_MIN 10

extern tcByte aLineModeReg[];

extern tcByte aLineMode1080PMode1Lin1[];
extern tcByte aLineMode1080PMode1Lin2[];
extern tcByte aLineMode1080PMode2Lin1[];
extern tcByte aLineMode1080PMode2Lin2[];
extern tcByte aLineMode1080PMode2Lin3[];
extern tcByte aLineMode1080PMode8Lin1[];
extern tcByte aLineMode1080PMode8Lin2[];
extern tcByte aLineMode1080PMode9Lin1[];
extern tcByte aLineMode1080PMode9Lin2[];
extern tcByte aLineMode1080PMode12Lin1[];
extern tcByte aLineMode1080PMode14Lin1[];
extern tcByte aLineMode1080PMode14Lin2[];

extern tcByte aLineMode720PMode1Lin1[];
extern tcByte aLineMode720PMode1Lin2[];
extern tcByte aLineMode720PMode2Lin1[];
extern tcByte aLineMode720PMode2Lin2[];
extern tcByte aLineMode720PMode2Lin3[];
extern tcByte aLineMode720PMode8Lin1[];
extern tcByte aLineMode720PMode8Lin2[];
extern tcByte aLineMode720PMode9Lin1[];
extern tcByte aLineMode720PMode9Lin2[];
extern tcByte aLineMode720PMode12Lin1[];
extern tcByte aLineMode720PMode14Lin1[];
extern tcByte aLineMode720PMode14Lin2[];

#if (TH0701024600NYR50S1 == OFF)
extern tcByte aLineMode800x480Mode1Lin1[];
extern tcByte aLineMode800x480Mode1Lin2[];
extern tcByte aLineMode800x480Mode2Lin1[];
extern tcByte aLineMode800x480Mode2Lin2[];
extern tcByte aLineMode800x480Mode2Lin3[];
extern tcByte aLineMode800x480Mode8Lin1[];
extern tcByte aLineMode800x480Mode8Lin2[];
extern tcByte aLineMode800x480Mode10Lin1[];
extern tcByte aLineMode800x480Mode10Lin2[];
extern tcByte aLineMode800x480Mode9Lin1[];
extern tcByte aLineMode800x480Mode9Lin2[];
extern tcByte aLineMode800x480Mode12Lin1[];
extern tcByte aLineMode800x480Mode14Lin1[];
extern tcByte aLineMode800x480Mode14Lin2[];
#else
extern tcByte aLineMode1024x600Mode1Lin1[];
extern tcByte aLineMode1024x600Mode1Lin2[];
extern tcByte aLineMode1024x600Mode2Lin1[];
extern tcByte aLineMode1024x600Mode2Lin2[];
extern tcByte aLineMode1024x600Mode2Lin3[];
extern tcByte aLineMode1024x600Mode8Lin1[];
extern tcByte aLineMode1024x600Mode8Lin2[];
extern tcByte aLineMode1024x600Mode9Lin1[];
extern tcByte aLineMode1024x600Mode9Lin2[];
extern tcByte aLineMode1024x600Mode10Lin1[];
extern tcByte aLineMode1024x600Mode10Lin2[];
extern tcByte aLineMode1024x600Mode12Lin1[];
extern tcByte aLineMode1024x600Mode14Lin1[];
extern tcByte aLineMode1024x600Mode14Lin2[];

#endif

#if 1

struct DefineMenu
{
	BYTE MenuID;
	BYTE MenuNum;
	tcByte *Str_E;
	struct DefineMenu *Child;
	BYTE Type; // MENU, CHOICEMENU, BAR, NUMBER, TIME, ACTION, ENUM_STRING
	BYTE ActiveWithCursor;
	BYTE Id;
	tcByte *TypeData;
	BYTE TouchAddr;
};

#else
struct DefineMenu
{
	BYTE MenuID;
	BYTE MenuNum;
	tcByte *Str_E;
	struct DefineMenu *Child;
	BYTE Type; // MENU, CHOICEMENU, BAR, NUMBER, TIME, ACTION, ENUM_STRING
	BYTE ActiveWithCursor;
	BYTE Id;
	tcByte *TypeData;
};
#endif

//-----------------------------------------------------------------------------
// extern Function Prototypes
//-----------------------------------------------------------------------------
extern void VXIS_fSetOsdx2(bit OnOff);
extern void VXIS_fSetOsdOnOff(tByte ch, tByte block);
extern void VXIS_fShowInOutMode(tByte ch, tByte in_mode, tByte out_mode);
extern void VXIS_fShowInOutMode0OSD(tByte ch, tByte in_mode, tByte out_mode);
extern void OSD_Clear(tByte Win);
extern void OSD_Clear_34(void);
extern void OSD_Clear_234(void);
extern void OSD_Clear_4(void);
extern void OSD_Clear_All(void);
extern void SetDisplayedOSD(BYTE newd);
extern BYTE GetDisplayedOSD(void);
extern void setTransParentIndex(tByte index);
extern void ResetOSDValue(void);
extern void VS4210_Line_Draw(tByte mode);
extern void VXIS_fSetContentOsdOnOff(tByte block);
extern void user_pwm_setvalue(uint16_t value);

void VXOSD_UseFont_init(uint8_t Size, const uint8_t *UserDefineFontPtr);
void OpenOSDMenu(BYTE OSD_ID);
void OSDCursorMove(BYTE flag);
void ClearDisplayedOSD(BYTE newd);
void OSDValueUpDn(BYTE flag);
void OSDSelect(void);
void DisplayCursor(void);
void TouchDisplayCursor(BYTE index);
BYTE SHOWOSDINFO(void);
BYTE CloseOSDINFO(void);
BYTE CloseOSDMenu(void);
void LoadVideoData(void);
void ShowJUMPOSD(void);
void ShowGaugeOSD(void);
WORD GetItemValue(BYTE id);
void DisplayVol(void);
void ShowTRIGOSD(void);
void ShowCOCOSD(void);
void ShowTPMSOSD(void);
void SET_CROSS_LINE(BYTE sel);
int GetBackLightLevel(void);
void Reset_OSD_SPEAKER(void);
void DisplayOSDMenuOneItem(struct DefineMenu *DMp, BYTE x, BYTE y, BYTE NOTViewflag);
void ShowGrabberLinerOSD(void);
int GetOSDStep(BYTE id, BYTE newv);
void CLRGrabberMessage(void);
void GrabberModeTrigger(void);
void ShowOSDVersion(void);

// BYTE Mapping1( int fromValue, CODE_P struct RegisterInfo *fromRange,
//                               int * toValue, CODE_P struct RegisterInfo *toRange );

// extern void ClearAudioOSD(void) ;
// extern void Clear1ChAudioOSD(tByte ch ) ;

#ifdef OSD_CHANNEL_ALWAYS_ON
extern void OSD_ShowTitleOnly(void);
#endif

#endif
