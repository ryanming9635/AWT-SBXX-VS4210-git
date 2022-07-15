/** @file VX_OSD_Simple.c
 * @brief OSD subtitle display program
 *
 * Message is displayed on the screen.
 *
 * @author Cunu-Yu
 * @date 2019/06/20
 */

#include <string.h>
#include "VXIS_Extern_Define.h"
#include "VX_OSD.h"
#include "VX_OSD_Simple.h"
#include "VX_swi2c.h"
#include "Printf.h"
#include "etc_eep.h"
#include "keyremo.h"
#include "Audio.h"

#include "tpinc/TP2802.h"

#include "GT911.h"

// struct MenuType   	CODE *MenuFormat;
struct RegisterInfo *BarRange;
struct DefineMenu *MenuTitle;
struct DefineMenu *MenuChild;
struct DefineMenu *MenuStack[4];
extern struct RegisterInfo UserRange;
extern struct RegisterInfo UserRangeBacklight;

// struct RegisterInfo ContrastRange={	0x11,0x99,128};
// struct RegisterInfo BrightnessRange={	0x51,0x99,0x7a};
struct RegisterInfo ContrastRange = {0, 255, 128};
struct RegisterInfo BrightnessRange = {0, 255, 128};

struct RegisterInfo SharpnessRange = {0, 0x3f, 0x1f};
struct RegisterInfo HueRange = {0, 63, 32};
struct RegisterInfo SaturationRange = {0, 255, 128};
struct RegisterInfo BacklightRange = {100, 4000, 4000};
// struct RegisterInfo AudioVolRange={	0,63,32};
struct RegisterInfo AudioVolRange = {0, 32, 16};
struct RegisterInfo AudioVolBarRange = {0, 126, 63};
// struct RegisterInfo GrabberLinerWinSize1={50,800,400};  //width max= 480
struct RegisterInfo GrabberLinerWinSize1 = {40 + 80 + 40 + 40, 800, 380}; // width max= 800
// struct RegisterInfo GrabberLinerWinSize2={67,400,200};  //width max= 240
struct RegisterInfo GrabberLinerWinSize2 = {20 + 80, 400, 190}; // width max= 400

extern tByte u8gDrawOSDFlg;
extern tByte gbVXIS_OuputSize;
extern tByte gbVXIS_OuputModeflg;
extern tByte gbVXIS_InputMode[4];
extern tByte gaHDMI_MapWindowIndex[4];
extern tByte gu8AudioMode;
extern BYTE OSDMenuLevel;
extern BYTE title_item, cursor_item, cursor_change, coc_cursor;
extern bit OnChangingValue;
extern tByte DayNightLevel;
extern BYTE OSD_LOCKF;
extern BYTE Disp_Input;
extern BYTE Priority;
extern BYTE CAMA_M;
extern BYTE CAMB_M;
extern BYTE CAMC_M;
extern BYTE CAMR_M;
extern BYTE CAMD_M;
extern BYTE SEQFlag;
extern BYTE NowModeState;
extern tByte DisplayedOSD;
extern BYTE OSD_MIRROR[4];
extern BYTE OSD_SPEAKER[4];
extern BYTE TriggerFlag;
extern BYTE TriggerStatus;
extern WORD OneTouchPosition;
extern BYTE MenuTouchMode;
extern BYTE bytDistanceGaugePosition;

static WORD OSDItemValue;
extern tByte newInRes[4];
extern tByte gWindowNum;
extern BYTE Flisheye_CAM[4];
extern BYTE Flisheye_COC;
extern WORD BAT_VOLTAGE, BAT_StartupVOLT;
extern BYTE bytAutoSleeping;
extern BYTE bytDistanceGaugePosition;
extern BYTE bytDistanceGaugeWidth;
extern BYTE bytSelectCAMRD;
extern BYTE bytGarbberLiner;
extern BYTE bytGarbberLinerPosition;
extern BYTE bytGarbberLinerWidth;
extern BYTE bytGarbberLinerMode;
extern tByte bSignal[4];
extern BYTE RUN_DIMMER;

BYTE PanelBrightnessRange = 10;
BYTE OSD_Diapley = 1;
BYTE OSD_DAY = 0;
BYTE SEQTime = 0;
BYTE SEQIndex = 0;
BYTE bytGaugeAdjust = 0;
BYTE bytGrabberAdjust = 0;
BYTE bytCOCAdjust = OFF;

// BYTE ResetCamMirror=0;
extern BYTE TimeStep;
extern BYTE SelectModeType;
extern BYTE MenuTouchFlag;
extern BYTE MenuTouchSel;
#if (TPMSOSD == ON)
extern BYTE TP_HIGH_PSI[6];
extern BYTE TP_LOW_PSI[6];
extern BYTE TP_HIGH_TEMP[6];
#endif

extern int iBackLightLevel;
extern BYTE bytTripleScreenType;
extern WORD bytSetGarbberLinerPosition;
extern BYTE CURRENT_CH;

extern T_VS4210_MutiWindows pVS4210_JointKind1;
#define _Color 0x07

#define CH_BLINK 0x80 // 0x08
#define CH_COLOR_MASK 0x0f
// OSD Level
#define _NO_OSD 0
#define _TITLE_OSD 1
#define _PICTURE_OSD 2
#define _INPUT_OSD 3
#define _VIDEO_OSD 4
#define _ULIT_OSD 5
#define _MIRROR_OSD 6
#define _DIR_IMAGE_OSD 7
#define _TRIGGER_DELAY_OSD 8
#define _RGB_GAIN_OSD 9
#define _TPMS_OSD 10
#define _TPMS_HIGH_PSI_OSD 11
#define _TPMS_LOW_PSI_OSD 12
#define _TPMS_HIGH_TEMP_OSD 13
#define _RESET_OSD 14
#define _DISTANCE_GAUGE_OSD 15

/// OSD NUMBER
#if (RGBGAINOSD == ON)
#define _TITLE_NUMBER 4 + 1 + 1
#elif (TPMSOSD == ON)
#define _TITLE_NUMBER 4 + 1 + 1
#else
#define _TITLE_NUMBER 4 + 1
#endif
#define _PICTURE_NUMBER 6 + 1
#if (TOUCH_FUNCTION == ON)
#define _INPUT_NUMBER (6 + 1)
#else
#define _INPUT_NUMBER (6 + 1)
#endif
#define _VIDEO_NUMBER (5 /*7+1*/)
#define _ULIT_NUMBER 2 + 1 + 1
#define _MIRROR_NUMBER 5 + 1
#define _TRIGGER_SET_NUMBER 6
#define _DIR_IMAGE_NUMBER 4
#define _TRIGGER_DELAY_NUMBER 5 + 1
#define _RGB_GAIN_NUMBER 7
#define _TPMS_NUMBER 4
#define _TPMS_HIGH_PSI_NUMBER 7
#define _TPMS_LOW_PSI_NUMBER 7
#define _TPMS_HIGH_TEMP_NUMBER 7
#define _RESET_OSD_NUMBER 2
#define _DISTANCE_GAUGE_NUMBER 4 + 2

static tcByte *ptcOSDbar;
#if 1
static tcByte OSD_720x480I60_param[] = {10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_720x480P60_param[] = {10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_720x576I50_param[] = {10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_720x576P50_param[] = {10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_1280x720P_param[] = {10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_1920x1080P_param[] = {10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_1920x1080I_param[] = {10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};

tByte OSD_CAM_C_VLOSS_CLR[] = {10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
#else
static tcByte OSD_720x480I60_param[] = {10, _7, _2, _0, _X, _4, _8, _0, _I, _spc, _spc};
static tcByte OSD_720x480P60_param[] = {10, _7, _2, _0, _X, _4, _8, _0, _P, _spc, _spc};
static tcByte OSD_720x576I50_param[] = {10, _7, _2, _0, _X, _5, _7, _6, _I, _spc, _spc};
static tcByte OSD_720x576P50_param[] = {10, _7, _2, _0, _X, _5, _7, _6, _P, _spc, _spc};
static tcByte OSD_1280x720P_param[] = {10, _1, _2, _8, _0, _X, _7, _2, _0, _P, _spc};
static tcByte OSD_1920x1080P_param[] = {10, _1, _9, _2, _0, _X, _1, _0, _8, _0, _P};
static tcByte OSD_1920x1080I_param[] = {10, _1, _9, _2, _0, _X, _1, _0, _8, _0, _I};
#endif
// static tcByte OSD_No_Signal_param[]=   {10 ,_N,_o,_spc,_S,_i,_g,_n,_a,_l,_spc};
static tcByte OSD_No_Signal_param[] = {10, _spc, _spc, _V, _L, _O, _S, _S, _spc, _spc, _spc};

// static tcByte OSD_CH0_param[]= {10,_C,_H,_spc,_1,_spc,_spc,0x80,0x81,0x82,0x83};
/*
static tcByte OSD_CH0_param[]= {6,_C,_H,_spc,_1,_spc,_spc};
static tcByte OSD_CH1_param[]= {6,_C,_H,_spc,_2,_spc,_spc};
static tcByte OSD_CH2_param[]= {6,_C,_H,_spc,_3,_spc,_spc};
static tcByte OSD_CH3_param[]= {6,_C,_H,_spc,_4,_spc,_spc};
*/
static tcByte OSD_CH0_param[] = {6, _A, _spc, _spc, _spc, _spc, _spc};
// static tcByte OSD_CH0_param[]= {6,_J,_spc,_spc,_A,_M,0x80};

static tcByte OSD_CH1_param[] = {6, _B, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_CH2_param[] = {6, _C, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_CH3_param[] = {6, _R, _spc, _spc, _spc, _spc, _spc};
static tcByte OSD_CH4_param[] = {6, _D, _spc, _spc, _spc, _spc, _spc};

static tcByte Str_OSD_SPEAKER[] = {2, _spc, 0x80};
static tcByte Str_OSD_SPEAKER_MUTE[] = {2, 0x80, _x};

static tcByte Str_OSD_MIRROR[] = {1, _M};
static tcByte Str_OSD_JUMP[] = {2, _spc, _J};
static tcByte Str_OSD_SEQ[] = {2, _spc, _S};
static tcByte Str_OSD_COC[] = {3, _C, _O, _C};
static tcByte Str_OSD_COC2[] = {33, _P, _R, _E, _S, _S, _spc, _M, _E, _N, _U, 0x53, _J, _U, _M, _P, _spc, _K, _E, _Y, _S, _spc, _T, _o, _spc, _E, _X, _I, _T, _spc, _C, _O, _C, 0x45};
static tcByte Str_OSD_COC_CLR[] = {33, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};

// static tcByte   Str_OSD_CLRJUMP[] ={1,_spc};

tcByte Str_OSD_TRI_LEFT[] = {1, 0x82};
tcByte Str_OSD_TRI_RIGHT[] = {1, 0x81};
tcByte Str_OSD_TRI_CAMC[] = {1, _T};
tcByte Str_OSD_TRI_CAMR[] = {1, 0x83};
tcByte Str_OSD_TRI_CAMD[] = {1, _T};
tcByte Str_OSD_Channel_CAMD[] = {1, _D};
tcByte Str_OSD_TRI_CLR[] = {1, _spc};

/*
static tcByte Str_Picture_E[] = {7,_P,_i,_c,_t,_u,_r,_e};
static tcByte Str_Input_E[] = {5,_I,_n,_P,_u,_t};
static tcByte Str_Camera_E[] = {6,_C,_a,_m,_e,_r,_a};
static tcByte Str_Exit_E[] = {4,_E,_x,_i,_t};
*/
// static tcByte Str_Picture[] = {20,_P,_i,_c,_t,_u,_r,_e,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_action};
static tcByte Str_Picture[] = {20, _V, _I, _D, _E, _O, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};
// static tcByte Str_Input[] = {20,_I,_n,_P,_u,_t,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_action};
static tcByte Str_Input[] = {20, _D, _I, _S, _P, _L, _A, _Y, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};
// static tcByte Str_Video[] = {20,_V,_i,_d,_e,_o,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_action};
static tcByte Str_Video[] = {20, _C, _A, _M, _E, _R, _A, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};
// static tcByte Str_Ulti[] = {20,_U,_l,_t,_i,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_action};
static tcByte Str_Ulti[] = {20, _S, _Y, _S, _T, _E, _M, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};
static tcByte Str_Exit[] = {20, _E, _X, _I, _T, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};

static tcByte Str_NO[] = {20, _N, _O, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};

static tcByte Str_Return[] = {20, _R, _E, _T, _U, _R, _N, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};
#if (RGBGAINOSD == ON)
static tcByte Str_RGBGain[] = {20, _R, _G, _B, _spc, _G, _A, _I, _N, _spc, _S, _E, _T, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};
#endif

#if (TPMSOSD == ON)
static tcByte Str_TPMS[] = {20, _T, _P, _M, _S, _spc, _S, _E, _T, _T, _I, _N, _G, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _action};
#endif

static tcByte Str_VolumeTXT[] = {20, _spc, _V, _O, _L, _U, _M, _E, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
static tcByte Str_VolumeBAR[] = {
	20,
	_bar0,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar1,
	_bar10,
	_spc,
	_1,
	_0,
	_0,
};
static tcByte Str_VolumeTXTArrow[] = {20, _spc, 0x54, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, 0x53, _spc, _spc, _spc, _spc, _spc};
static tcByte Str_VolumeDEC[] = {1, 0x54};
static tcByte Str_VolumeINC[] = {1, 0x53};
// static tcByte Str_ArrowLeft[]={1,0x56};
// static tcByte Str_ArrowRight[]={1,0x58};

static tcByte Str_OSD_LOCK2[] = {20, _O, _S, _D, _spc, _M, _E, _N, _U, _spc, _L, _O, _C, _K, _E, _D, _spc, _spc, _spc, _spc, _spc};
static tcByte Str_OSD_UNLOCK[] = {20, _O, _S, _D, _spc, _M, _E, _N, _U, _spc, _A, _C, _T, _I, _V, _E, _spc, _spc, _spc, _spc, _spc};

// static tcByte Str_GAUGE_OSD0[] = {20,_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_0};
// static tcByte Str_GAUGE_OSD0[] = {20,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,_7,_8,_9,_0,_1,_2,_3,_4,_5,_6,_7,_8,_9};

static tcByte Str_COC_LOSD[3] = {2, _AL0, _AL0 + 1};
static tcByte Str_COC_LOSD1[3] = {2, _AL0 + 2, _AL0 + 3};

static tcByte Str_COC_ROSD[3] = {2, _AR0, _AR0 + 1};
static tcByte Str_COC_ROSD1[3] = {2, _AR0 + 2, _AR0 + 3};

static tcByte Str_COC_UOSD[3] = {2, _AU0, _AU0 + 1};
static tcByte Str_COC_UOSD1[3] = {2, _AU0 + 2, _AU0 + 3};

static tcByte Str_COC_DOSD[3] = {2, _AD0, _AD0 + 1};
static tcByte Str_COC_DOSD1[3] = {2, _AD0 + 2, _AD0 + 3};

static tcByte Str_COC_MENUOSD[3] = {2, _E0, _E0 + 1};
static tcByte Str_COC_MENUOSD1[3] = {2, _E0 + 2, _E0 + 3};

/// GRABBER_1
/*
static tcByte Str_GRABBER_OSD10[] = {8,_spc,_spc,_spc,_M1G,_CLG,_M11G,_spc ,_spc };
static tcByte Str_GRABBER_OSD11[] = {8,_spc,_spc,_spc,_g02,_CLG,_g12 ,_spc ,_spc };
static tcByte Str_GRABBER_OSD12[] = {8,_spc,_spc,_M3G,_M4G,_CLG,_M13G,_M14G,_spc };
static tcByte Str_GRABBER_OSD13[] = {8,_spc,_spc,_g05,_spc,_CLG,_spc ,_g15 ,_spc };
static tcByte Str_GRABBER_OSD14[] = {8,_spc,_spc,_g06,_spc,_CLG,_spc ,_g16 ,_spc };
static tcByte Str_GRABBER_OSD15[] = {8,_spc,_g07,_g08,_spc,_CLG,_spc ,_g17 ,_g18 };
static tcByte Str_GRABBER_OSD16[] = {8,_spc,_M9G,_spc,_spc,_CLG,_spc ,_spc ,_M19G};
static tcByte Str_GRABBER_OSD17[] = {8,_spc,_g10,_spc,_spc,_CLG,_spc ,_spc ,_g20 };
*/
static tcByte Str_GRABBER_OSD10[] = {7, _spc, _spc, _M1G, _CLG, _M11G, _spc, _spc};
static tcByte Str_GRABBER_OSD11[] = {7, _spc, _spc, _g02, _CLG, _g12, _spc, _spc};
static tcByte Str_GRABBER_OSD12[] = {7, _spc, _M3G, _M4G, _CLG, _M13G, _M14G, _spc};
static tcByte Str_GRABBER_OSD13[] = {7, _spc, _g05, _spc, _CLG, _spc, _g15, _spc};
static tcByte Str_GRABBER_OSD14[] = {7, _spc, _g06, _spc, _CLG, _spc, _g16, _spc};
static tcByte Str_GRABBER_OSD15[] = {7, _g07, _g08, _spc, _CLG, _spc, _g17, _g18};
static tcByte Str_GRABBER_OSD16[] = {7, _M9G, _spc, _spc, _CLG, _spc, _spc, _M19G};
static tcByte Str_GRABBER_OSD17[] = {7, _g10, _spc, _spc, _CLG, _spc, _spc, _g20};

/// GRABBER_2
static tcByte Str_GRABBER_OSD20[] = {9, _spc, _spc, _M1G, _spc, _CLG, _spc, _M11G, _spc, _spc};
static tcByte Str_GRABBER_OSD21[] = {9, _spc, _spc, _g02, _spc, _CLG, _spc, _g12, _spc, _spc};
static tcByte Str_GRABBER_OSD22[] = {9, _spc, _M3G, _M4G, _spc, _CLG, _spc, _M13G, _M14G, _spc};
static tcByte Str_GRABBER_OSD23[] = {9, _spc, _g05, _spc, _spc, _CLG, _spc, _spc, _g15, _spc};
static tcByte Str_GRABBER_OSD24[] = {9, _spc, _g06, _spc, _spc, _CLG, _spc, _spc, _g16, _spc};
static tcByte Str_GRABBER_OSD25[] = {9, _g07, _g08, _spc, _spc, _CLG, _spc, _spc, _g17, _g18};
static tcByte Str_GRABBER_OSD26[] = {9, _M9G, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _M19G};
static tcByte Str_GRABBER_OSD27[] = {9, _g10, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _g20};

/// GRABBER_3
static tcByte Str_GRABBER_OSD30[] = {11, _spc, _spc, _M1G, _spc, _spc, _CLG, _spc, _spc, _M11G, _spc, _spc};
static tcByte Str_GRABBER_OSD31[] = {11, _spc, _spc, _g02, _spc, _spc, _CLG, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GRABBER_OSD32[] = {11, _spc, _M3G, _M4G, _spc, _spc, _CLG, _spc, _spc, _M13G, _M14G, _spc};
static tcByte Str_GRABBER_OSD33[] = {11, _spc, _g05, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GRABBER_OSD34[] = {11, _spc, _g06, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GRABBER_OSD35[] = {11, _g07, _g08, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GRABBER_OSD36[] = {11, _M9G, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _M19G};
static tcByte Str_GRABBER_OSD37[] = {11, _g10, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _g20};

/// GRABBER_4
static tcByte Str_GRABBER_OSD40[] = {13, _spc, _spc, _M1G, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _M11G, _spc, _spc};
static tcByte Str_GRABBER_OSD41[] = {13, _spc, _spc, _g02, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GRABBER_OSD42[] = {13, _spc, _M3G, _M4G, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _M13G, _M14G, _spc};
static tcByte Str_GRABBER_OSD43[] = {13, _spc, _g05, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GRABBER_OSD44[] = {13, _spc, _g06, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GRABBER_OSD45[] = {13, _g07, _g08, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GRABBER_OSD46[] = {13, _M9G, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _M19G};
static tcByte Str_GRABBER_OSD47[] = {13, _g10, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _g20};

/// GRABBER_5
static tcByte Str_GRABBER_OSD50[] = {15, _spc, _spc, _M1G, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _M11G, _spc, _spc};
static tcByte Str_GRABBER_OSD51[] = {15, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GRABBER_OSD52[] = {15, _spc, _M3G, _M4G, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _M13G, _M14G, _spc};
static tcByte Str_GRABBER_OSD53[] = {15, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GRABBER_OSD54[] = {15, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GRABBER_OSD55[] = {15, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GRABBER_OSD56[] = {15, _M9G, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _M19G};
static tcByte Str_GRABBER_OSD57[] = {15, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// GRABBER_6
static tcByte Str_GRABBER_OSD60[] = {17, _spc, _spc, _M1G, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _M11G, _spc, _spc};
static tcByte Str_GRABBER_OSD61[] = {17, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GRABBER_OSD62[] = {17, _spc, _M3G, _M4G, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _M13G, _M14G, _spc};
static tcByte Str_GRABBER_OSD63[] = {17, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GRABBER_OSD64[] = {17, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GRABBER_OSD65[] = {17, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GRABBER_OSD66[] = {17, _M9G, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _M19G};
static tcByte Str_GRABBER_OSD67[] = {17, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// GRABBER_7
static tcByte Str_GRABBER_OSD70[] = {19, _spc, _spc, _M1G, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _M11G, _spc, _spc};
static tcByte Str_GRABBER_OSD71[] = {19, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GRABBER_OSD72[] = {19, _spc, _M3G, _M4G, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _M13G, _M14G, _spc};
static tcByte Str_GRABBER_OSD73[] = {19, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GRABBER_OSD74[] = {19, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GRABBER_OSD75[] = {19, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GRABBER_OSD76[] = {19, _M9G, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _M19G};
static tcByte Str_GRABBER_OSD77[] = {19, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _CLG, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

static tcByte GrabberMessage[] = {22, _J, _U, _M, _P, 0x57, _M, _o, _v, _e, _spc, _S, _E, _L, _E, _C, _T, 0x57, _W, _i, _d, _t, _h};
static tcByte GrabberMessageCLR[] = {22, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};

static tcByte *Str_GRABBER_OSD1[8] = {Str_GRABBER_OSD10, Str_GRABBER_OSD11, Str_GRABBER_OSD12, Str_GRABBER_OSD13,
									  Str_GRABBER_OSD14, Str_GRABBER_OSD15, Str_GRABBER_OSD16, Str_GRABBER_OSD17};

static tcByte *Str_GRABBER_OSD2[8] = {Str_GRABBER_OSD20, Str_GRABBER_OSD21, Str_GRABBER_OSD22, Str_GRABBER_OSD23,
									  Str_GRABBER_OSD24, Str_GRABBER_OSD25, Str_GRABBER_OSD26, Str_GRABBER_OSD27};

static tcByte *Str_GRABBER_OSD3[8] = {Str_GRABBER_OSD30, Str_GRABBER_OSD31, Str_GRABBER_OSD32, Str_GRABBER_OSD33,
									  Str_GRABBER_OSD34, Str_GRABBER_OSD35, Str_GRABBER_OSD36, Str_GRABBER_OSD37};

static tcByte *Str_GRABBER_OSD4[8] = {Str_GRABBER_OSD40, Str_GRABBER_OSD41, Str_GRABBER_OSD42, Str_GRABBER_OSD43,
									  Str_GRABBER_OSD44, Str_GRABBER_OSD45, Str_GRABBER_OSD46, Str_GRABBER_OSD47};

static tcByte *Str_GRABBER_OSD5[8] = {Str_GRABBER_OSD50, Str_GRABBER_OSD51, Str_GRABBER_OSD52, Str_GRABBER_OSD53,
									  Str_GRABBER_OSD54, Str_GRABBER_OSD55, Str_GRABBER_OSD56, Str_GRABBER_OSD57};

static tcByte *Str_GRABBER_OSD6[8] = {Str_GRABBER_OSD60, Str_GRABBER_OSD61, Str_GRABBER_OSD62, Str_GRABBER_OSD63,
									  Str_GRABBER_OSD64, Str_GRABBER_OSD65, Str_GRABBER_OSD66, Str_GRABBER_OSD67};

static tcByte *Str_GRABBER_OSD7[8] = {Str_GRABBER_OSD70, Str_GRABBER_OSD71, Str_GRABBER_OSD72, Str_GRABBER_OSD73,
									  Str_GRABBER_OSD74, Str_GRABBER_OSD75, Str_GRABBER_OSD76, Str_GRABBER_OSD77};

static tcByte **Str_GRABBER_OSD[7] = {Str_GRABBER_OSD1, Str_GRABBER_OSD2, Str_GRABBER_OSD3, Str_GRABBER_OSD4,
									  Str_GRABBER_OSD5, Str_GRABBER_OSD6, Str_GRABBER_OSD7};

#if 1

static tcByte Str_GAUGE_OSD0[] = {18, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g11, _spc, _spc, _spc, _spc};
static tcByte Str_GAUGE_OSD1[] = {18, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc, _spc, _spc};
static tcByte Str_GAUGE_OSD2[] = {18, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g13, _g14, _spc, _spc, _spc};
static tcByte Str_GAUGE_OSD3[] = {18, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc, _spc, _spc};
static tcByte Str_GAUGE_OSD4[] = {18, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc, _spc, _spc};
static tcByte Str_GAUGE_OSD5[] = {18, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18, _spc, _spc};
static tcByte Str_GAUGE_OSD6[] = {18, _spc, _g09, _g23, _g23, _g23, _g23, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g23, _g23, _g23, _g23, _g19, _spc, _spc};
static tcByte Str_GAUGE_OSD7[] = {18, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20, _spc, _spc};

/*
static tcByte Str_GAUGE_OSD0[] = {20,_spc,_spc,_spc,_g01,_g21,_g21,_g21	,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc};
static tcByte Str_GAUGE_OSD1[] = {20,_spc,_spc,_spc,_g02,_spc,_spc,_spc	,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc};
static tcByte Str_GAUGE_OSD2[] = {20,_spc,_spc,_g03,_g04,_g21,_g21,_g21	,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc};
static tcByte Str_GAUGE_OSD3[] = {20,_spc,_spc,_g05,_spc,_spc,_spc,_spc	,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc};
static tcByte Str_GAUGE_OSD4[] = {20,_spc,_spc,_g06,_spc,_spc,_spc,_spc	,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc};
static tcByte Str_GAUGE_OSD5[] = {20,_spc,_g07,_g08,_spc,_spc,_spc,_spc	,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc};
static tcByte Str_GAUGE_OSD6[] = {20,_spc,_g09,_g23,_g23,_g23,_g23,_g23	,_g23,_g23,_g22,_g23,_g23,_g23,_g23,_g23,_g23,_g23,_g19,_spc,_spc};
static tcByte Str_GAUGE_OSD7[] = {20,_spc,_g10,_spc,_spc,_spc,_spc,_spc	,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc};
*/

#else
static tcByte Str_GAUGE_OSD1[] = {20, _1, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0};
static tcByte Str_GAUGE_OSD2[] = {20, _2, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0};
static tcByte Str_GAUGE_OSD3[] = {20, _3, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0};
static tcByte Str_GAUGE_OSD4[] = {20, _4, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0};
static tcByte Str_GAUGE_OSD5[] = {20, _5, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0};
static tcByte Str_GAUGE_OSD6[] = {20, _6, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0};
static tcByte Str_GAUGE_OSD7[] = {20, _7, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0};
#endif

#if 1
/// width 10
#if 0
static tcByte Str_GAUGE_Width10[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width11[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width12[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width13[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width14[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width15[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width16[] = {18,_spc,_g09,_g23,_g23,_g23,_g23,_g23,_g23,_g22,_g23,_g23,_g23,_g23,_g23,_g23,_g23,_g19,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width17[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc};   
///width 20                                                                                                                                     
static tcByte Str_GAUGE_Width20[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width21[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width22[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width23[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width24[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width25[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width26[] = {18,_spc,_g09,_g23,_g23,_g23,_g23,_g23,_g23,_g22,_g23,_g23,_g23,_g23,_g23,_g23,_g19,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width27[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc,_spc};   
                                                                                                                                                
///width 30                                                                                                                                     
static tcByte Str_GAUGE_Width30[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width31[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width32[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width33[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width34[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width35[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width36[] = {18,_spc,_g09,_g23,_g23,_g23,_g23,_g23,_g22,_g23,_g23,_g23,_g23,_g23,_g23,_g19,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width37[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc,_spc,_spc};   
                                                                                                                                                
                                                                                                                                                
///width 40                                                                                                                                     
static tcByte Str_GAUGE_Width40[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width41[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width42[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g21,_g21,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width43[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width44[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width45[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width46[] = {18,_spc,_g09,_g23,_g23,_g23,_g23,_g23,_g22,_g23,_g23,_g23,_g23,_g23,_g19,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width47[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc,_spc,_spc,_spc};   
                                                                                                                                                
///width 50                                                                                                                                     
static tcByte Str_GAUGE_Width50[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g21,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width51[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_spc,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width52[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g21,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width53[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width54[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width55[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width56[] = {18,_spc,_g09,_g23,_g23,_g23,_g23,_g22,_g23,_g23,_g23,_g23,_g23,_g19,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width57[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
                                                                                                                                                
///width 60                                                                                                                                     
static tcByte Str_GAUGE_Width60[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width61[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width62[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width63[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width64[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width65[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width66[] = {18,_spc,_g09,_g23,_g23,_g23,_g23,_g22,_g23,_g23,_g23,_g23,_g19,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width67[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
                                                                                                                                                
///width 70                                                                                                                                     
static tcByte Str_GAUGE_Width70[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width71[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width72[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width73[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width74[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width75[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width76[] = {18,_spc,_g09,_g23,_g23,_g23,_g22,_g23,_g23,_g23,_g23,_g19,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width77[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
                                                                                                                                                
                                                                                                                                                
///width 80                                                                                                                                     
static tcByte Str_GAUGE_Width80[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width81[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width82[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width83[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width84[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width85[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width86[] = {18,_spc,_g09,_g23,_g23,_g23,_g22,_g23,_g23,_g23,_g19,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width87[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
                                                                                                                                                
///width 90                                                                                                                                     

static tcByte Str_GAUGE_Width90[] = {18,_spc,_spc,_spc,_g01,_g21,_g21,_g11,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width91[] = {18,_spc,_spc,_spc,_g02,_spc,_spc,_g12,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width92[] = {18,_spc,_spc,_g03,_g04,_g21,_g21,_g13,_g14,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width93[] = {18,_spc,_spc,_g05,_spc,_spc,_spc,_spc,_g15,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width94[] = {18,_spc,_spc,_g06,_spc,_spc,_spc,_spc,_g16,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width95[] = {18,_spc,_g07,_g08,_spc,_spc,_spc,_spc,_g17,_g18,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width96[] = {18,_spc,_g09,_g23,_g23,_g22,_g23,_g23,_g23,_g19,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};   
static tcByte Str_GAUGE_Width97[] = {18,_spc,_g10,_spc,_spc,_spc,_spc,_spc,_spc,_g20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};
#else
/// width 10
static tcByte Str_GAUGE_Width10[] = {17, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width11[] = {17, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width12[] = {17, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width13[] = {17, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width14[] = {17, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width15[] = {17, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width16[] = {17, _spc, _g09, _g23, _g23, _g23, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g23, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width17[] = {17, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};
/// width 20
static tcByte Str_GAUGE_Width20[] = {16, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width21[] = {16, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width22[] = {16, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width23[] = {16, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width24[] = {16, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width25[] = {16, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width26[] = {16, _spc, _g09, _g23, _g23, _g23, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width27[] = {16, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// width 30
static tcByte Str_GAUGE_Width30[] = {15, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width31[] = {15, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width32[] = {15, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width33[] = {15, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width34[] = {15, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width35[] = {15, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width36[] = {15, _spc, _g09, _g23, _g23, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width37[] = {15, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// width 40
static tcByte Str_GAUGE_Width40[] = {14, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width41[] = {14, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width42[] = {14, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g21, _g21, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width43[] = {14, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width44[] = {14, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width45[] = {14, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width46[] = {14, _spc, _g09, _g23, _g23, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width47[] = {14, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// width 50
static tcByte Str_GAUGE_Width50[] = {13, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g21, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width51[] = {13, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width52[] = {13, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g21, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width53[] = {13, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width54[] = {13, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width55[] = {13, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width56[] = {13, _spc, _g09, _g23, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width57[] = {13, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// width 60
static tcByte Str_GAUGE_Width60[] = {12, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width61[] = {12, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width62[] = {12, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width63[] = {12, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width64[] = {12, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width65[] = {12, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width66[] = {12, _spc, _g09, _g23, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width67[] = {12, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// width 70
static tcByte Str_GAUGE_Width70[] = {11, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width71[] = {11, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width72[] = {11, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width73[] = {11, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width74[] = {11, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width75[] = {11, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width76[] = {11, _spc, _g09, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width77[] = {11, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// width 80
static tcByte Str_GAUGE_Width80[] = {10, _spc, _spc, _spc, _g01, _g21, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width81[] = {10, _spc, _spc, _spc, _g02, _spc, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width82[] = {10, _spc, _spc, _g03, _g04, _g21, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width83[] = {10, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width84[] = {10, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width85[] = {10, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width86[] = {10, _spc, _g09, _g23, _g23, _g23, _g22, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width87[] = {10, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _g20};

/// width 90
static tcByte Str_GAUGE_Width90[] = {9, _spc, _spc, _spc, _g01, _g21, _g21, _g11, _spc, _spc};
static tcByte Str_GAUGE_Width91[] = {9, _spc, _spc, _spc, _g02, _spc, _spc, _g12, _spc, _spc};
static tcByte Str_GAUGE_Width92[] = {9, _spc, _spc, _g03, _g04, _g21, _g21, _g13, _g14, _spc};
static tcByte Str_GAUGE_Width93[] = {9, _spc, _spc, _g05, _spc, _spc, _spc, _spc, _g15, _spc};
static tcByte Str_GAUGE_Width94[] = {9, _spc, _spc, _g06, _spc, _spc, _spc, _spc, _g16, _spc};
static tcByte Str_GAUGE_Width95[] = {9, _spc, _g07, _g08, _spc, _spc, _spc, _spc, _g17, _g18};
static tcByte Str_GAUGE_Width96[] = {9, _spc, _g09, _g23, _g23, _g22, _g23, _g23, _g23, _g19};
static tcByte Str_GAUGE_Width97[] = {9, _spc, _g10, _spc, _spc, _spc, _spc, _spc, _spc, _g20};
#endif
#endif

// default width
static tcByte *Str_GAUGE_OSD[8] = {Str_GAUGE_OSD0, Str_GAUGE_OSD1, Str_GAUGE_OSD2, Str_GAUGE_OSD3,
								   Str_GAUGE_OSD4, Str_GAUGE_OSD5, Str_GAUGE_OSD6, Str_GAUGE_OSD7};

// width 10
static tcByte *Str_GAUGE_Width1[8] = {Str_GAUGE_Width10, Str_GAUGE_Width11, Str_GAUGE_Width12, Str_GAUGE_Width13,
									  Str_GAUGE_Width14, Str_GAUGE_Width15, Str_GAUGE_Width16, Str_GAUGE_Width17};
// width 20
static tcByte *Str_GAUGE_Width2[8] = {Str_GAUGE_Width20, Str_GAUGE_Width21, Str_GAUGE_Width22, Str_GAUGE_Width23,
									  Str_GAUGE_Width24, Str_GAUGE_Width25, Str_GAUGE_Width26, Str_GAUGE_Width27};
// width 30
static tcByte *Str_GAUGE_Width3[8] = {
	Str_GAUGE_Width30,
	Str_GAUGE_Width31,
	Str_GAUGE_Width32,
	Str_GAUGE_Width33,
	Str_GAUGE_Width34,
	Str_GAUGE_Width35,
	Str_GAUGE_Width36,
	Str_GAUGE_Width37,
};

// width 40
static tcByte *Str_GAUGE_Width4[8] = {Str_GAUGE_Width40, Str_GAUGE_Width41, Str_GAUGE_Width42, Str_GAUGE_Width43,
									  Str_GAUGE_Width44, Str_GAUGE_Width45, Str_GAUGE_Width46, Str_GAUGE_Width47};
// width 50
static tcByte *Str_GAUGE_Width5[8] = {Str_GAUGE_Width50, Str_GAUGE_Width51, Str_GAUGE_Width52, Str_GAUGE_Width53,
									  Str_GAUGE_Width54, Str_GAUGE_Width55, Str_GAUGE_Width56, Str_GAUGE_Width57};
// width 60
static tcByte *Str_GAUGE_Width6[8] = {
	Str_GAUGE_Width60,
	Str_GAUGE_Width61,
	Str_GAUGE_Width62,
	Str_GAUGE_Width63,
	Str_GAUGE_Width64,
	Str_GAUGE_Width65,
	Str_GAUGE_Width66,
	Str_GAUGE_Width67,
};

// width 70
static tcByte *Str_GAUGE_Width7[8] = {Str_GAUGE_Width70, Str_GAUGE_Width71, Str_GAUGE_Width72, Str_GAUGE_Width73,
									  Str_GAUGE_Width74, Str_GAUGE_Width75, Str_GAUGE_Width76, Str_GAUGE_Width77};
// width 80
static tcByte *Str_GAUGE_Width8[8] = {Str_GAUGE_Width80, Str_GAUGE_Width81, Str_GAUGE_Width82, Str_GAUGE_Width83,
									  Str_GAUGE_Width84, Str_GAUGE_Width85, Str_GAUGE_Width86, Str_GAUGE_Width87};
// width90
static tcByte *Str_GAUGE_Width9[8] = {Str_GAUGE_Width90, Str_GAUGE_Width91, Str_GAUGE_Width92, Str_GAUGE_Width93,
									  Str_GAUGE_Width94, Str_GAUGE_Width95, Str_GAUGE_Width96, Str_GAUGE_Width97};

// static tcByte **Str_GAUGE_Width[9] = {Str_GAUGE_Width1,Str_GAUGE_Width2,Str_GAUGE_Width3,Str_GAUGE_Width4,
//	Str_GAUGE_Width5,Str_GAUGE_Width6,Str_GAUGE_Width7,Str_GAUGE_Width8,Str_GAUGE_Width9};
static tcByte **Str_GAUGE_Width[10] = {Str_GAUGE_Width9, Str_GAUGE_Width8, Str_GAUGE_Width7, Str_GAUGE_Width6,
									   Str_GAUGE_OSD, Str_GAUGE_Width5, Str_GAUGE_Width4, Str_GAUGE_Width3, Str_GAUGE_Width2, Str_GAUGE_Width1};

static tcByte *Str_COC_OSD[10] = {Str_COC_LOSD, Str_COC_LOSD1, Str_COC_ROSD, Str_COC_ROSD1, Str_COC_UOSD,
								  Str_COC_UOSD1, Str_COC_DOSD, Str_COC_DOSD1, Str_COC_MENUOSD, Str_COC_MENUOSD1};

/*
static tcByte Str_TPMS_OSD0[] = {20,_spc,_spc,_spc,_spc,_1,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_2,_spc,_spc,_spc,_spc};
static tcByte Str_TPMS_OSD1[] = {20,_spc,_1,_0,_0,_DIA_F,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_DIA_F,_1,_0,_0,_spc};
static tcByte Str_TPMS_OSD2[] = {20,_spc,_1,_0,_0,_DIA_F,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_DIA_F,_1,_0,_0,_spc};
static tcByte Str_TPMS_OSD3[] = {20,_spc,_spc,_spc,_spc,_spc,_spc,_spc	,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};
static tcByte Str_TPMS_OSD4[] = {20,_spc,_spc,_spc,_spc,_3,_4,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_5,_6,_spc,_spc,_spc,_spc};
static tcByte Str_TPMS_OSD5[] = {20,_spc,_1,_0,_0,_DIA_F,_DIA_F,_1,_0,_0,_spc,_spc,_1,_0,_0,_DIA_F,_DIA_F,_1,_0,_0,_spc};
static tcByte Str_TPMS_OSD6[] = {20,_spc,_1,_0,_0,_DIA_F,_DIA_F,_1,_0,_0,_spc,_spc,_1,_0,_0,_DIA_F,_DIA_F,_1,_0,_0,_spc};
static tcByte Str_TPMS_OSD7[] = {20,_spc,_spc,_spc,_spc,_spc,_spc,_spc	,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc};
*/
static tcByte Str_TPMS_OSD0[] = {20, 0x4a, _1, _spc, _P, _S, _I, 0x57, _1, _0, _0, _spc, _T, _E, _M, _P, 0x57, _1, _0, _0, _spc};
static tcByte Str_TPMS_OSD1[] = {20, 0x4a, _2, _spc, _P, _S, _I, 0x57, _1, _0, _0, _spc, _T, _E, _M, _P, 0x57, _1, _0, _0, _spc};
static tcByte Str_TPMS_OSD2[] = {20, 0x4a, _3, _spc, _P, _S, _I, 0x57, _1, _0, _0, _spc, _T, _E, _M, _P, 0x57, _1, _0, _0, _spc};
static tcByte Str_TPMS_OSD3[] = {20, 0x4a, _4, _spc, _P, _S, _I, 0x57, _1, _0, _0, _spc, _T, _E, _M, _P, 0x57, _1, _0, _0, _spc};
static tcByte Str_TPMS_OSD4[] = {20, 0x4a, _5, _spc, _P, _S, _I, 0x57, _1, _0, _0, _spc, _T, _E, _M, _P, 0x57, _1, _0, _0, _spc};
static tcByte Str_TPMS_OSD5[] = {20, 0x4a, _6, _spc, _P, _S, _I, 0x57, _1, _0, _0, _spc, _T, _E, _M, _P, 0x57, _1, _0, _0, _spc};
static tcByte Str_TPMS_OSD6[] = {20, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
static tcByte Str_TPMS_OSD7[] = {20, _B, _A, _T, 0x47, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};
// static tcByte Str_TPMS_OSD7[] = {20,_spc,_spc,_spc,0x4f,_T,_P,_M,_S,_spc,_S,_Y,_S,_T,_E,_M,0x50,_spc,_spc,_spc,_spc};

static tcByte Str_TPMS_NUM0[] = {2, _1, 0x4a};
static tcByte Str_TPMS_NUM1[] = {2, _2, 0x4a};
static tcByte Str_TPMS_NUM2[] = {2, _3, 0x4a};
static tcByte Str_TPMS_NUM3[] = {2, _4, 0x4a};
static tcByte Str_TPMS_NUM4[] = {2, _5, 0x4a};
static tcByte Str_TPMS_NUM5[] = {2, _6, 0x4a};

// static tcByte Str_action[] = {1,_action};
//{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc},

// osd menu
/*
static tcByte MENU1[4][6]= {
					{10,_T,_E,_S,_T,_spc,_1,_spc,_spc,_spc,_spc},
					{10,_T,_E,_S,_T,_spc,_2,_spc,_spc,_spc,_spc},
					{10,_T,_E,_S,_T,_spc,_3,_spc,_spc,_spc,_spc},
					{10,_T,_E,_S,_T,_spc,_4,_spc,_spc,_spc,_spc}
				};
*/
// static tcByte MENU1[]= {10,_T,_E,_S,_T,_1,_2,_3,_4,_5,_6};
/*
CODE BYTE Str_Brightness_E[] = {'B','r','i','g','h','t','n','e','s','s',0};
CODE BYTE Str_Contrast_E[] = {'C','o','n','t','r','a','s','t',0};
CODE BYTE Str_Saturation_E[] = {'S','a','t','u','r','a','t','i','o','n',0};
CODE BYTE Str_Hue_E[] = {'H','u','e',0};
CODE BYTE Str_Sharpness_E[] = {'S','h','a','r','p','n','e','s','s',0};
CODE BYTE Str_Reset_E[] = {'R','e','s','e','t',0};

CODE BYTE 	*Str_Brightness[]=	{Str_Brightness_E};
CODE BYTE 	*Str_Contrast[]=		{Str_Contrast_E};
CODE BYTE 	*Str_Saturation[]=	{Str_Saturation_E};
CODE BYTE 	*Str_Hue[]=			{Str_Hue_E};
CODE BYTE 	*Str_Sharpness[]=		{Str_Sharpness_E};
CODE BYTE 	*Str_Reset[]=	{Str_Reset_E};
*/
/*
	struct DefineMenu {
			BYTE						MenuID;
			BYTE						*Str_E;
			BYTE						Type;				// MENU, CHOICEMENU, BAR, NUMBER, TIME, ACTION, ENUM_STRING
			BYTE						ActiveWithCursor;
			BYTE						Id;

	} ;
*/
static tcByte ONOFFChoiceString[] = {
	0x02, // size
	4,	  // char  leng.
	3, _O, _F, _F,
	3, _O, _N, _spc};

static tcByte IMAGE_A_String[] = {
	0x07, // size
	5,	  // None by lang.
	4, _spc, _spc, _spc, _A,
	4, _spc, _A, _plus, _B,
	4, _spc, _A, _plus, _C,
	4, _spc, _A, _plus, _R,
	4, _A, _plus, _B, _C,
	4, _A, _plus, _B, _R,
	4, _A, _plus, _C, _R};

static tcByte IMAGE_B_String[] = {
	0x07,
	5,
	4, _spc, _spc, _spc, _B,
	4, _spc, _B, _plus, _A,
	4, _spc, _B, _plus, _C,
	4, _spc, _B, _plus, _R,
	4, _B, _plus, _A, _C,
	4, _B, _plus, _A, _R,
	4, _B, _plus, _C, _R};

static tcByte IMAGE_C_String[] = {
	0x07, // size
	5,
	4, _spc, _spc, _spc, _C,
	4, _spc, _C, _plus, _A,
	4, _spc, _C, _plus, _B,
	4, _spc, _C, _plus, _R,
	4, _C, _plus, _A, _B,
	4, _C, _plus, _A, _R,
	4, _C, _plus, _B, _R};

static tcByte REARString[] = {
	5, // size
	5,
	4, _spc, _spc, _spc, _R,
	4, _spc, _R, _plus, _A,
	4, _spc, _R, _plus, _B,
	4, _spc, _R, _plus, _C,
	// 4,_R,_slash,_A,_B,
	// 4,_R,_slash,_A,_C,
	//	 4,_R,_slash,_B,_C,
	4, _R, _plus, _A, _B
	//	4,_R,_plus,_A,_C,
	//	4,_R,_plus,_B,_C
};

static tcByte JUMPString[] = {
	15, // size
	5,
	4, _Q, _U, _A, _D,
	4, _spc, _S, _E, _Q,
	4, _spc, _A, _plus, _B,
	4, _spc, _C, _plus, _R,
	4, _spc, _A, _plus, _R,
	4, _spc, _R, _plus, _B,
	4, _spc, _A, _plus, _C,
	4, _spc, _C, _plus, _B,
	4, _R, _slash, _A, _B,
	4, _R, _plus, _A, _B,
	4, _spc, _spc, _spc, _A,
	4, _spc, _spc, _spc, _B,
	4, _spc, _spc, _spc, _C,
	4, _spc, _spc, _spc, _D,
	4, _spc, _spc, _spc, _R};
/*
static tcByte MChoiceString[] = {
			2, //size
			7,
		6,_N,_O,_R,_M,_A,_L,
		6,_M,_I,_R,_R,_O,_R
};
*/
// static tByte *Str_Main_OSD[4]={Str_Picture_E,Str_Input_E,Str_Camera_E,Str_Exit_E};
static tcByte Str_Brightness[] = {10, _B, _R, _I, _G, _H, _T, _N, _E, _S, _S};
static tcByte Str_Contrast[] = {8, _C, _O, _N, _T, _R, _A, _S, _T};
static tcByte Str_Saturation[] = {10, _S, _A, _T, _U, _R, _A, _T, _I, _O, _N};
static tcByte Str_Hue[] = {3, _H, _U, _E};
static tcByte Str_Sharpness[] = {9, _S, _H, _A, _R, _P, _N, _E, _S, _S};
static tcByte Str_Luminance[] = {9, _L, _U, _M, _I, _N, _A, _N, _C, _E};

// static tcByte Str_Brightness[] = {20,_B,_r,_i,_g,_h,_t,_n,_e,_s,_s,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_1,_0,_0};

// static tcByte Str_Auto_Day_Night[] = {20,_A,_u,_t,_o,_D,_a,_y,_A,_n,_d,_N,_i,_g,_h,_t,_spc,_spc,_O,_F,_F};
// static tcByte Str_DISPLAY[] = {20,_D,_i,_s,_p,_l,_a,_y,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_O,_F,_F};
// static tcByte Str_Auto_Day_Night[] = {14,_A,_u,_t,_o,_spc,_D,_a,_y,_and,_N,_i,_g,_h,_t};
static tcByte Str_Auto_Day_Night[] = {11, _A, _U, _T, _O, _spc, _D, _I, _M, _M, _E, _R};

// static tcByte Str_DISPLAY[] = {7,_D,_i,_s,_p,_l,_a,_y};
// static tcByte Str_DISPLAY[] = {5,_T,_i,_t,_l,_e};//TITLE
static tcByte Str_DISPLAY[] = {11, _I, _N, _F, _O, _R, _M, _A, _T, _I, _O, _N}; // info

static tcByte Str_DISTANCE[] = {14, _D, _I, _S, _T, _A, _N, _C, _E, _spc, _G, _A, _U, _G, _E};
static tcByte Str_OSD_LOCK[] = {8, _O, _S, _D, _spc, _L, _O, _C, _K};
// static tcByte  Str_PRIORITY[] = {8,_P,_R,_I,_O,_R,_I,_T,_Y};
static tcByte Str_PRIORITY[] = {8, _P, _R, _I, _O, _R, _I, _T, _Y};
#if (TOUCH_FUNCTION == ON)
static tcByte Str_Menu_Touch[] = {10, _M, _E, _N, _U, _spc, _T, _O, _U, _C, _H};
#endif
static tcByte Str_Trigger_Set[] = {11, _T, _R, _I, _G, _G, _E, _R, _spc, _S, _E, _T};
// static tcByte  Str_DIR_IMAGE[]={9,_D,_i,_r,_spc,_I,_m,_a,_g,_e};
static tcByte Str_DIR_IMAGE_CAMA[] = {11, _D, _I, _R, _spc, _I, _M, _A, _G, _E, _spc, _A};
static tcByte Str_DIR_IMAGE_CAMB[] = {11, _D, _I, _R, _spc, _I, _M, _A, _G, _E, _spc, _B};
static tcByte Str_DIR_IMAGE_CAMC[] = {11, _D, _I, _R, _spc, _I, _M, _A, _G, _E, _spc, _C};
static tcByte Str_DIR_IMAGE_CAMR[] = {11, _D, _I, _R, _spc, _I, _M, _A, _G, _E, _spc, _R};

// static tcByte  Str_REAR_SETUP[]={10,_R,_e,_a,_r,_spc,_S,_e,_t,_u,_p};
static tcByte Str_JUMP[] = {4, _J, _U, _M, _P};
static tcByte Str_Mirror[] = {6, _M, _I, _R, _R, _O, _R};
static tcByte Str_SEQ_Time_step[] = {13, _S, _E, _Q, _spc, _T, _I, _M, _E, _spc, _S, _T, _E, _P};
static tcByte Str_Trigger_Delay[] = {13, _T, _R, _I, _G, _G, _E, _R, _spc, _D, _E, _L, _A, _Y};
static tcByte Str_Auto_Sleeping[] = {13, _A, _U, _T, _O, _spc, _S, _L, _E, _E, _P, _I, _N, _G};
static tcByte Str_CAM_A[] = {5, _C, _A, _M, _spc, _A};
static tcByte Str_CAM_B[] = {5, _C, _A, _M, _spc, _B};
static tcByte Str_CAM_C[] = {5, _C, _A, _M, _spc, _C};
static tcByte Str_CAM_R[] = {5, _C, _A, _M, _spc, _R};
static tcByte Str_CAM_D[] = {5, _C, _A, _M, _spc, _D};

static tcByte Str_Triple_Screen_Type[] = {13, _T, _R, _I, _P, _L, _E, _spc, _S, _C, _R, _E, _E, _N};

#if (RGBGAINOSD == ON)
static tcByte Str_R_B_GAIN[] = {8, _R, _spc, _B, _spc, _G, _A, _I, _N};
static tcByte Str_G_B_GAIN[] = {8, _G, _spc, _B, _spc, _G, _A, _I, _N};
static tcByte Str_B_B_GAIN[] = {8, _B, _spc, _B, _spc, _G, _A, _I, _N};
static tcByte Str_R_C_GAIN[] = {8, _R, _spc, _C, _spc, _G, _A, _I, _N};
static tcByte Str_G_C_GAIN[] = {8, _G, _spc, _C, _spc, _G, _A, _I, _N};
static tcByte Str_B_C_GAIN[] = {8, _B, _spc, _C, _spc, _G, _A, _I, _N};
#endif

#if (TPMSOSD == ON)
static tcByte Str_High_PSI[] = {8, _H, _i, _g, _h, _spc, _P, _S, _I};
static tcByte Str_Low_PSI[] = {8, _L, _o, _w, _spc, _spc, _P, _S, _I};
static tcByte Str_High_Temp[] = {9, _H, _i, _g, _h, _spc, _T, _e, _m, _p};

static tcByte Str_TP1_High_PSI[] = {12, _T, _P, _1, _spc, _H, _i, _g, _h, _spc, _P, _S, _I};
static tcByte Str_TP2_High_PSI[] = {12, _T, _P, _2, _spc, _H, _i, _g, _h, _spc, _P, _S, _I};
static tcByte Str_TP3_High_PSI[] = {12, _T, _P, _3, _spc, _H, _i, _g, _h, _spc, _P, _S, _I};
static tcByte Str_TP4_High_PSI[] = {12, _T, _P, _4, _spc, _H, _i, _g, _h, _spc, _P, _S, _I};
static tcByte Str_TP5_High_PSI[] = {12, _T, _P, _5, _spc, _H, _i, _g, _h, _spc, _P, _S, _I};
static tcByte Str_TP6_High_PSI[] = {12, _T, _P, _6, _spc, _H, _i, _g, _h, _spc, _P, _S, _I};

static tcByte Str_TP1_Low_PSI[] = {11, _T, _P, _1, _spc, _L, _o, _w, _spc, _P, _S, _I};
static tcByte Str_TP2_Low_PSI[] = {11, _T, _P, _2, _spc, _L, _o, _w, _spc, _P, _S, _I};
static tcByte Str_TP3_Low_PSI[] = {11, _T, _P, _3, _spc, _L, _o, _w, _spc, _P, _S, _I};
static tcByte Str_TP4_Low_PSI[] = {11, _T, _P, _4, _spc, _L, _o, _w, _spc, _P, _S, _I};
static tcByte Str_TP5_Low_PSI[] = {11, _T, _P, _5, _spc, _L, _o, _w, _spc, _P, _S, _I};
static tcByte Str_TP6_Low_PSI[] = {11, _T, _P, _6, _spc, _L, _o, _w, _spc, _P, _S, _I};

static tcByte Str_TP1_High_Temp[] = {13, _T, _P, _1, _spc, _H, _i, _g, _h, _spc, _T, _e, _m, _p};
static tcByte Str_TP2_High_Temp[] = {13, _T, _P, _2, _spc, _H, _i, _g, _h, _spc, _T, _e, _m, _p};
static tcByte Str_TP3_High_Temp[] = {13, _T, _P, _3, _spc, _H, _i, _g, _h, _spc, _T, _e, _m, _p};
static tcByte Str_TP4_High_Temp[] = {13, _T, _P, _4, _spc, _H, _i, _g, _h, _spc, _T, _e, _m, _p};
static tcByte Str_TP5_High_Temp[] = {13, _T, _P, _5, _spc, _H, _i, _g, _h, _spc, _T, _e, _m, _p};
static tcByte Str_TP6_High_Temp[] = {13, _T, _P, _6, _spc, _H, _i, _g, _h, _spc, _T, _e, _m, _p};

#endif

// static tcByte Str_RESET[] = {20,_R,_E,_S,_E,_T,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_O,_F,_F};
static tcByte Str_RECALL[] = {6, _R, _E, _C, _A, _L, _L};
static tcByte Str_RESET[] = {3, _Y, _E, _S};

static tcByte Str_COCOSD[] = {3, _C, _O, _C};

static tcByte Str_Position[] = {8, _P, _O, _S, _I, _T, _I, _O, _N};
static tcByte Str_Width[] = {5, _W, _I, _D, _T, _H};
static tcByte Str_Setting[] = {7, _S, _E, _T, _T, _I, _N, _G};

static tcByte Str_GrabberLiner[] = {13, _G, _R, _A, _B, _B, _E, _R, _spc, _L, _I, _N, _E, _S};
static tcByte Str_GrabberLinerSet[] = {11, _G, _R, _A, _B, _B, _E, _R, _spc, _S, _E, _T};

///_PICTURE_OSD
struct DefineMenu Picture_Menu[] = {
	{_PICTURE_OSD, _PICTURE_NUMBER, Str_Brightness, 0, NUMBER, 0, VIDEO_BRIGHTNESS, 0, _TouchAddr0},
	{_PICTURE_OSD, _PICTURE_NUMBER, Str_Contrast, 0, NUMBER, 0, VIDEO_CONTRAST, 0, _TouchAddr1},
	{_PICTURE_OSD, _PICTURE_NUMBER, Str_Saturation, 0, NUMBER, 0, VIDEO_SATURATION, 0, _TouchAddr2},
	{_PICTURE_OSD, _PICTURE_NUMBER, Str_Hue, 0, NUMBER, 0, VIDEO_HUE, 0, _TouchAddr3},
	{_PICTURE_OSD, _PICTURE_NUMBER, Str_Sharpness, 0, NUMBER, 0, VIDEO_SHARPNESS, 0, _TouchAddr4},
	{_PICTURE_OSD, _PICTURE_NUMBER, Str_Luminance, 0, NUMBER, 0, VIDEO_BACKLIGHT, 0, _TouchAddr5},
	{_PICTURE_OSD, _PICTURE_NUMBER, Str_Return, 0, EXIT, 0, _TITLE_OSD, 0, _TouchAddr6}};

//_Distance_Gauge_OSD
struct DefineMenu Distance_Gauge_Menu[] = {
	{_DISTANCE_GAUGE_OSD, _DISTANCE_GAUGE_NUMBER, Str_Setting, 0, ENUM_STRING, 0, GAUGEChoice, ONOFFChoiceString, _TouchAddr0},
	{_DISTANCE_GAUGE_OSD, _DISTANCE_GAUGE_NUMBER, Str_Position, 0, NUMBER, 0, DistanceGaugePosition, 0, _TouchAddr1},
	{_DISTANCE_GAUGE_OSD, _DISTANCE_GAUGE_NUMBER, Str_Width, 0, NUMBER, 0, DistanceGaugeWidth, 0, _TouchAddr2},
	{_DISTANCE_GAUGE_OSD, _DISTANCE_GAUGE_NUMBER, Str_GrabberLiner, 0, ENUM_STRING, 0, Grabber_Liner, ONOFFChoiceString, _TouchAddr3},
	{_DISTANCE_GAUGE_OSD, _DISTANCE_GAUGE_NUMBER, Str_GrabberLinerSet, 0, NUMBER, 0, Grabber_Liner_Width, 0, _TouchAddr4},
	{_DISTANCE_GAUGE_OSD, _DISTANCE_GAUGE_NUMBER, Str_Return, 0, EXIT, 0, _INPUT_OSD, 0, _TouchAddr5}};

//_INPUT_OSD
struct DefineMenu Input_Menu[] = {
	{_INPUT_OSD, _INPUT_NUMBER, Str_Auto_Day_Night, 0, ENUM_STRING, 0, DayChoice, ONOFFChoiceString, _TouchAddr0},
	{_INPUT_OSD, _INPUT_NUMBER, Str_DISPLAY, 0, ENUM_STRING, 0, CAMOSDONOFF, ONOFFChoiceString, _TouchAddr1},
	{_INPUT_OSD, _INPUT_NUMBER, Str_Triple_Screen_Type, 0, NUMBER, 0, TripleScreenType, 0, _TouchAddr2},
	{_INPUT_OSD, _INPUT_NUMBER, Str_DISTANCE, Distance_Gauge_Menu, OSDMENU, 0, NO, 0, _TouchAddr3},
	{_INPUT_OSD, _INPUT_NUMBER, Str_SEQ_Time_step, 0, NUMBER, 0, STIME_STEP, 0, _TouchAddr4},
	{_INPUT_OSD, _INPUT_NUMBER, Str_Auto_Sleeping, 0, ENUM_STRING, 0, AutoSleeping, ONOFFChoiceString, _TouchAddr5},
//{_INPUT_OSD,_INPUT_NUMBER, Str_OSD_LOCK,0,ENUM_STRING,0,OsdLockChoice,ONOFFChoiceString,_TouchAddr3},
//{_INPUT_OSD,_INPUT_NUMBER, Str_PRIORITY,0,ENUM_STRING,0,PRIORITYChoice,ONOFFChoiceString,_TouchAddr4},
#if (TOUCH_FUNCTION == ON)
	{_INPUT_OSD, _INPUT_NUMBER, Str_Menu_Touch, 0, ENUM_STRING, 0, MenutouchChoice, ONOFFChoiceString, _TouchAddr5},
#endif
	{_INPUT_OSD, _INPUT_NUMBER, Str_Return, 0, EXIT, 0, _TITLE_OSD, 0, _TouchAddr6}};

//_MIRROR_OSD
struct DefineMenu Mirror_Menu[] = {
	{_MIRROR_OSD, _MIRROR_NUMBER, Str_CAM_A, 0, ENUM_STRING, 0, CAMAONOFF, ONOFFChoiceString, _TouchAddr0},
	{_MIRROR_OSD, _MIRROR_NUMBER, Str_CAM_B, 0, ENUM_STRING, 0, CAMBONOFF, ONOFFChoiceString, _TouchAddr1},
	{_MIRROR_OSD, _MIRROR_NUMBER, Str_CAM_C, 0, ENUM_STRING, 0, CAMCONOFF, ONOFFChoiceString, _TouchAddr2},
	{_MIRROR_OSD, _MIRROR_NUMBER, Str_CAM_D, 0, ENUM_STRING, 0, CAMDONOFF, ONOFFChoiceString, _TouchAddr3},
	{_MIRROR_OSD, _MIRROR_NUMBER, Str_CAM_R, 0, ENUM_STRING, 0, CAMRONOFF, ONOFFChoiceString, _TouchAddr4},
	{_MIRROR_OSD, _MIRROR_NUMBER, Str_Return, 0, EXIT, 0, _VIDEO_OSD, 0, _TouchAddr5}};
/*
//_DIR_IMAGE_OSD
struct DefineMenu Dir_Image_Menu[]={
{_DIR_IMAGE_OSD,_DIR_IMAGE_NUMBER, Str_DIR_IMAGE_CAMA,0,ENUM_STRING,0,IMAGE_A_Choice,IMAGE_A_String,_TouchAddr0},
{_DIR_IMAGE_OSD,_DIR_IMAGE_NUMBER, Str_DIR_IMAGE_CAMB,0,ENUM_STRING,0,IMAGE_B_Choice,IMAGE_B_String,_TouchAddr1},
{_DIR_IMAGE_OSD,_DIR_IMAGE_NUMBER, Str_DIR_IMAGE_CAMC,0,ENUM_STRING,0,IMAGE_C_Choice,IMAGE_C_String,_TouchAddr2},
{_DIR_IMAGE_OSD,_DIR_IMAGE_NUMBER, Str_Return,0,EXIT,0,_VIDEO_OSD,0,_TouchAddr3}
};
*/

//_TRIGGER_DELAY_OSD
struct DefineMenu Trigger_Delay_Menu[] = {
	{_TRIGGER_DELAY_OSD, _TRIGGER_DELAY_NUMBER, Str_CAM_A, 0, NUMBER, 0, DELAYCAMA, 0, _TouchAddr0},
	{_TRIGGER_DELAY_OSD, _TRIGGER_DELAY_NUMBER, Str_CAM_B, 0, NUMBER, 0, DELAYCAMB, 0, _TouchAddr1},
	{_TRIGGER_DELAY_OSD, _TRIGGER_DELAY_NUMBER, Str_CAM_C, 0, NUMBER, 0, DELAYCAMC, 0, _TouchAddr2},
	{_TRIGGER_DELAY_OSD, _TRIGGER_DELAY_NUMBER, Str_CAM_R, 0, NUMBER, 0, DELAYCAMR, 0, _TouchAddr3},
	{_TRIGGER_DELAY_OSD, _TRIGGER_DELAY_NUMBER, Str_CAM_D, 0, NUMBER, 0, DELAYCAMD, 0, _TouchAddr4},
	{_TRIGGER_DELAY_OSD, _TRIGGER_DELAY_NUMBER, Str_Return, 0, EXIT, 0, _DIR_IMAGE_OSD, 0, _TouchAddr5}};

//_TRIGGER_SET_OSD
struct DefineMenu Trigger_Set_Menu[] = {
	{_DIR_IMAGE_OSD, _TRIGGER_SET_NUMBER, Str_DIR_IMAGE_CAMA, 0, ENUM_STRING, 0, IMAGE_A_Choice, IMAGE_A_String, _TouchAddr0},
	{_DIR_IMAGE_OSD, _TRIGGER_SET_NUMBER, Str_DIR_IMAGE_CAMB, 0, ENUM_STRING, 0, IMAGE_B_Choice, IMAGE_B_String, _TouchAddr1},
	{_DIR_IMAGE_OSD, _TRIGGER_SET_NUMBER, Str_DIR_IMAGE_CAMC, 0, ENUM_STRING, 0, IMAGE_C_Choice, IMAGE_C_String, _TouchAddr2},
	{_DIR_IMAGE_OSD, _TRIGGER_SET_NUMBER, Str_DIR_IMAGE_CAMR, 0, ENUM_STRING, 0, REARChoice, REARString, _TouchAddr3},
	{_DIR_IMAGE_OSD, _TRIGGER_SET_NUMBER, Str_Trigger_Delay, Trigger_Delay_Menu, OSDMENU, 0, NO, 0, _TouchAddr4},
	{_DIR_IMAGE_OSD, _TRIGGER_SET_NUMBER, Str_Return, 0, EXIT, 0, _VIDEO_OSD, 0, _TouchAddr5}};

//_VIDEO_OSD
struct DefineMenu Video_Menu[] = {
	{_VIDEO_OSD, _VIDEO_NUMBER, Str_Mirror, Mirror_Menu, OSDMENU, 0, 0, ONOFFChoiceString, _TouchAddr0},
	{_VIDEO_OSD, _VIDEO_NUMBER, Str_JUMP, 0, ENUM_STRING, 0, JUMPChoice, JUMPString, _TouchAddr1},
	{_VIDEO_OSD, _VIDEO_NUMBER, Str_Trigger_Set, Trigger_Set_Menu, OSDMENU, 0, NO, 0, _TouchAddr2},
	//{_VIDEO_OSD,_VIDEO_NUMBER, Str_REAR_SETUP,0,ENUM_STRING,0,REARChoice,REARString,_TouchAddr2},
	//{_VIDEO_OSD,_VIDEO_NUMBER, Str_SEQ_Time_step,0,NUMBER,0,STIME_STEP,0,_TouchAddr3},
	//{_VIDEO_OSD,_VIDEO_NUMBER, Str_Trigger_Delay,Trigger_Delay_Menu,OSDMENU,0,NO,0,_TouchAddr3},
	//{_VIDEO_OSD,_VIDEO_NUMBER, Str_Auto_Sleeping,0,ENUM_STRING,0,AutoSleeping,ONOFFChoiceString,_TouchAddr6},
	{_VIDEO_OSD, _VIDEO_NUMBER, Str_PRIORITY, 0, ENUM_STRING, 0, PRIORITYChoice, ONOFFChoiceString, _TouchAddr3},
	{_VIDEO_OSD, _VIDEO_NUMBER, Str_Return, 0, EXIT, 0, _TITLE_OSD, 0, _TouchAddr4}};

//_RESET_OSD
struct DefineMenu Reset_Menu[] = {
	{_RESET_OSD, _RESET_OSD_NUMBER, Str_RESET, 0, ACTION, 0, RESET_VIDEOVALUE, 0, _TouchAddr0},
	{_RESET_OSD, _RESET_OSD_NUMBER, Str_NO, 0, EXIT, 0, _ULIT_OSD, 0, _TouchAddr1}

};

//_ULIT_OSD
struct DefineMenu Ulti_Menu[] = {
	{_ULIT_OSD, _ULIT_NUMBER, Str_OSD_LOCK, 0, ENUM_STRING, 0, OsdLockChoice, ONOFFChoiceString, _TouchAddr0},
	{_ULIT_OSD, _ULIT_NUMBER, Str_COCOSD, 0, ACTION, 0, COC_Action, 0, _TouchAddr1},
	{_ULIT_OSD, _ULIT_NUMBER, Str_RECALL, Reset_Menu, OSDMENU, 0, NO, 0, _TouchAddr2},
	//{_ULIT_OSD,_ULIT_NUMBER, Str_RESET,0,ACTION,0,RESET_VIDEOVALUE,0,_TouchAddr1},
	//{_ULIT_OSD,_ULIT_NUMBER, Str_Exit,0,EXIT,0,EXIT,0}
	{_ULIT_OSD, _ULIT_NUMBER, Str_Return, 0, EXIT, 0, _TITLE_OSD, 0, _TouchAddr3}

};

/// RGB GAIN OSD
#if (RGBGAINOSD == ON)
struct DefineMenu RGBGain_Menu[] = {
	{_RGB_GAIN_OSD, _RGB_GAIN_NUMBER, Str_R_B_GAIN, 0, NUMBER, 0, VIDEO_R_B_GAIN, 0, _TouchAddr0},
	{_RGB_GAIN_OSD, _RGB_GAIN_NUMBER, Str_G_B_GAIN, 0, NUMBER, 0, VIDEO_G_B_GAIN, 0, _TouchAddr1},
	{_RGB_GAIN_OSD, _RGB_GAIN_NUMBER, Str_B_B_GAIN, 0, NUMBER, 0, VIDEO_B_B_GAIN, 0, _TouchAddr2},
	{_RGB_GAIN_OSD, _RGB_GAIN_NUMBER, Str_R_C_GAIN, 0, NUMBER, 0, VIDEO_R_C_GAIN, 0, _TouchAddr3},
	{_RGB_GAIN_OSD, _RGB_GAIN_NUMBER, Str_G_C_GAIN, 0, NUMBER, 0, VIDEO_G_C_GAIN, 0, _TouchAddr4},
	{_RGB_GAIN_OSD, _RGB_GAIN_NUMBER, Str_B_C_GAIN, 0, NUMBER, 0, VIDEO_B_C_GAIN, 0, _TouchAddr5},
	{_RGB_GAIN_OSD, _RGB_GAIN_NUMBER, Str_Return, 0, EXIT, 0, _TITLE_OSD, 0, _TouchAddr6}};
#endif

/// TPMS OSD
#if (TPMSOSD == ON)
struct DefineMenu High_PSI_Menu[] = {
	{_TPMS_HIGH_PSI_OSD, _TPMS_HIGH_PSI_NUMBER, Str_TP1_High_PSI, 0, NUMBER, 0, TIRE1_High_PSI, 0, _TouchAddr0},
	{_TPMS_HIGH_PSI_OSD, _TPMS_HIGH_PSI_NUMBER, Str_TP2_High_PSI, 0, NUMBER, 0, TIRE2_High_PSI, 0, _TouchAddr1},
	{_TPMS_HIGH_PSI_OSD, _TPMS_HIGH_PSI_NUMBER, Str_TP3_High_PSI, 0, NUMBER, 0, TIRE3_High_PSI, 0, _TouchAddr2},
	{_TPMS_HIGH_PSI_OSD, _TPMS_HIGH_PSI_NUMBER, Str_TP4_High_PSI, 0, NUMBER, 0, TIRE4_High_PSI, 0, _TouchAddr3},
	{_TPMS_HIGH_PSI_OSD, _TPMS_HIGH_PSI_NUMBER, Str_TP5_High_PSI, 0, NUMBER, 0, TIRE5_High_PSI, 0, _TouchAddr4},
	{_TPMS_HIGH_PSI_OSD, _TPMS_HIGH_PSI_NUMBER, Str_TP6_High_PSI, 0, NUMBER, 0, TIRE6_High_PSI, 0, _TouchAddr5},
	{_TPMS_HIGH_PSI_OSD, _TPMS_HIGH_PSI_NUMBER, Str_Return, 0, EXIT, 0, _TPMS_OSD, 0, _TouchAddr6}};
// Low_PSI_Menu
struct DefineMenu Low_PSI_Menu[] = {
	{_TPMS_LOW_PSI_OSD, _TPMS_LOW_PSI_NUMBER, Str_TP1_Low_PSI, 0, NUMBER, 0, TIRE1_Low_PSI, 0, _TouchAddr0},
	{_TPMS_LOW_PSI_OSD, _TPMS_LOW_PSI_NUMBER, Str_TP2_Low_PSI, 0, NUMBER, 0, TIRE2_Low_PSI, 0, _TouchAddr1},
	{_TPMS_LOW_PSI_OSD, _TPMS_LOW_PSI_NUMBER, Str_TP3_Low_PSI, 0, NUMBER, 0, TIRE3_Low_PSI, 0, _TouchAddr2},
	{_TPMS_LOW_PSI_OSD, _TPMS_LOW_PSI_NUMBER, Str_TP4_Low_PSI, 0, NUMBER, 0, TIRE4_Low_PSI, 0, _TouchAddr3},
	{_TPMS_LOW_PSI_OSD, _TPMS_LOW_PSI_NUMBER, Str_TP5_Low_PSI, 0, NUMBER, 0, TIRE5_Low_PSI, 0, _TouchAddr4},
	{_TPMS_LOW_PSI_OSD, _TPMS_LOW_PSI_NUMBER, Str_TP6_Low_PSI, 0, NUMBER, 0, TIRE6_Low_PSI, 0, _TouchAddr5},
	{_TPMS_LOW_PSI_OSD, _TPMS_LOW_PSI_NUMBER, Str_Return, 0, EXIT, 0, _TPMS_OSD, 0, _TouchAddr6}};
// High_Temp_Menu
struct DefineMenu High_Temp_Menu[] = {
	{_TPMS_HIGH_TEMP_OSD, _TPMS_HIGH_TEMP_NUMBER, Str_TP1_High_Temp, 0, NUMBER, 0, TIRE1_High_Temp, 0, _TouchAddr0},
	{_TPMS_HIGH_TEMP_OSD, _TPMS_HIGH_TEMP_NUMBER, Str_TP2_High_Temp, 0, NUMBER, 0, TIRE2_High_Temp, 0, _TouchAddr1},
	{_TPMS_HIGH_TEMP_OSD, _TPMS_HIGH_TEMP_NUMBER, Str_TP3_High_Temp, 0, NUMBER, 0, TIRE3_High_Temp, 0, _TouchAddr2},
	{_TPMS_HIGH_TEMP_OSD, _TPMS_HIGH_TEMP_NUMBER, Str_TP4_High_Temp, 0, NUMBER, 0, TIRE4_High_Temp, 0, _TouchAddr3},
	{_TPMS_HIGH_TEMP_OSD, _TPMS_HIGH_TEMP_NUMBER, Str_TP5_High_Temp, 0, NUMBER, 0, TIRE5_High_Temp, 0, _TouchAddr4},
	{_TPMS_HIGH_TEMP_OSD, _TPMS_HIGH_TEMP_NUMBER, Str_TP6_High_Temp, 0, NUMBER, 0, TIRE6_High_Temp, 0, _TouchAddr5},
	{_TPMS_HIGH_TEMP_OSD, _TPMS_HIGH_TEMP_NUMBER, Str_Return, 0, EXIT, 0, _TPMS_OSD, 0, _TouchAddr6}};
// TPMS_Menu
struct DefineMenu TPMS_Menu[] = {
	{_TPMS_OSD, _TPMS_NUMBER, Str_High_PSI, High_PSI_Menu, OSDMENU, 0, 0, 0, _TouchAddr0},
	{_TPMS_OSD, _TPMS_NUMBER, Str_Low_PSI, Low_PSI_Menu, OSDMENU, 0, 0, 0, _TouchAddr1},
	{_TPMS_OSD, _TPMS_NUMBER, Str_High_Temp, High_Temp_Menu, OSDMENU, 0, 0, 0, _TouchAddr2},
	{_TPMS_OSD, _TPMS_NUMBER, Str_Return, 0, EXIT, 0, _TITLE_OSD, 0, _TouchAddr3}};
#endif

///_TITLE_OSD
struct DefineMenu Title_Menu[_TITLE_NUMBER] = {
	{_TITLE_OSD, _TITLE_NUMBER, Str_Picture, Picture_Menu, OSDMENU, 0, 0, 0, _TouchAddr0},
	{_TITLE_OSD, _TITLE_NUMBER, Str_Video, Video_Menu, OSDMENU, 0, 0, 0, _TouchAddr1},
	{_TITLE_OSD, _TITLE_NUMBER, Str_Input, Input_Menu, OSDMENU, 0, 0, 0, _TouchAddr2},
#if (RGBGAINOSD == ON)
	{_TITLE_OSD, _TITLE_NUMBER, Str_RGBGain, RGBGain_Menu, OSDMENU, 0, 0, 0, _TouchAddr3},
	{_TITLE_OSD, _TITLE_NUMBER, Str_Ulti, Ulti_Menu, OSDMENU, 0, 0, 0, _TouchAddr4},
	{_TITLE_OSD, _TITLE_NUMBER, Str_Exit, 0, EXIT, 0, EXIT, 0, _TouchAddr5}
#endif
#if (TPMSOSD == ON)
	{_TITLE_OSD, _TITLE_NUMBER, Str_TPMS, TPMS_Menu, OSDMENU, 0, 0, 0, _TouchAddr3},
	{_TITLE_OSD, _TITLE_NUMBER, Str_Ulti, Ulti_Menu, OSDMENU, 0, 0, 0, _TouchAddr4},
	{_TITLE_OSD, _TITLE_NUMBER, Str_Exit, 0, EXIT, 0, EXIT, 0, _TouchAddr5}

#else
	{_TITLE_OSD, _TITLE_NUMBER, Str_Ulti, Ulti_Menu, OSDMENU, 0, 0, 0, _TouchAddr3},
	{_TITLE_OSD, _TITLE_NUMBER, Str_Exit, 0, EXIT, 0, EXIT, 0, _TouchAddr4}
#endif
};

/*
static tcByte MENU1[8][21]={
						{20,_T,_E,_S,_T,_1,_2,_3,_4,_5,_1,_T,_E,_S,_T,_1,_2,_3,_4,_1,_1},
						{20,_T,_E,_S,_T,_2,_2,_3,_4,_5,_2,_T,_E,_S,_T,_1,_2,_3,_4,_2,_2},
						{20,_T,_E,_S,_T,_3,_2,_3,_4,_5,_3,_T,_E,_S,_T,_1,_2,_3,_4,_3,_3},
						{20,_T,_E,_S,_T,_4,_2,_3,_4,_5,_4,_T,_E,_S,_T,_1,_2,_3,_4,_1,_1},

						{20,_T,_E,_S,_T,_5,_2,_3,_4,_5,_5,_T,_E,_S,_T,_1,_2,_3,_4,_4,_4},
						{20,_T,_E,_S,_T,_6,_2,_3,_4,_5,_6,_T,_E,_S,_T,_1,_2,_3,_4,_5,_5},
						{20,_T,_E,_S,_T,_7,_2,_3,_4,_5,_7,_T,_E,_S,_T,_1,_2,_3,_4,_6,_6},
						{20,_T,_E,_S,_T,_8,_2,_3,_4,_5,_8,_T,_E,_S,_T,_1,_2,_3,_4,_7,_7}
					};
*/
/*
static tcByte   Str_Menu_Clr[8][21]={
							{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc},
							{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc},
							{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc},
							{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc},

							{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc},
							{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc},
							{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc},
							{20,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc,_spc}

						};
*/
static tcByte Str_Menu_Clr[21] = {20, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};

static tcByte Str_Menu_Cursor[17] = {16, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc, _spc};

/*
static tcByte MENU2[]= {10,_T,_E,_S,_T,_2,_2,_3,_4,_5,_6}
static tcByte MENU3[]= {10,_T,_E,_S,_T,_3,_2,_3,_4,_5,_6}
static tcByte MENU4[]= {10,_T,_E,_S,_T,_4,_2,_3,_4,_5,_6}
static tcByte MENU5[]= {10,_T,_E,_S,_T,_5,_2,_3,_4,_5,_6}
static tcByte MENU6[]= {10,_T,_E,_S,_T,_6,_2,_3,_4,_5,_6}
static tcByte MENU7[]= {10,_T,_E,_S,_T,_7,_2,_3,_4,_5,_6}
static tcByte MENU8[]= {10,_T,_E,_S,_T,_8,_2,_3,_4,_5,_6};
*/

// tByte  OSD_CHx_param[7] ;

tcWord Mode0_1080P_param[] = {180, 61};
tcWord Mode1_w0_1080P_param[] = {180, 61};
tcWord Mode1_w1_1080P_param[] = {1140, 61};
tcWord Mode1_w2_1080P_param[] = {180, 603};
tcWord Mode1_w3_1080P_param[] = {1140, 603};
tcWord Mode2_w0_1080P_param[] = {180, 61};
tcWord Mode2_w1_1080P_param[] = {664, 61};
tcWord Mode2_w2_1080P_param[] = {664, 603};
tcWord Mode2_w3_1080P_param[] = {1624, 61};
tcWord Mode8_w0_1080P_param[] = {180, 61};
tcWord Mode8_w1_1080P_param[] = {1140, 61};
tcWord Mode8_w2_1080P_param[] = {1140, 603};
tcWord Mode9_w0_1080P_param[] = {1140, 61};
tcWord Mode9_w1_1080P_param[] = {180, 61};
tcWord Mode9_w2_1080P_param[] = {180, 603};
tcWord Mode12_w0_1080P_param[] = {180, 61};
tcWord Mode12_w1_1080P_param[] = {1140, 61};
tcWord Mode14_w0_1080P_param[] = {180, 61};
tcWord Mode14_w1_1080P_param[] = {820, 61};
tcWord Mode14_w2_1080P_param[] = {1460, 61};

tcWord Mode10_w0_1080P_param[] = {180, 61};
tcWord Mode10_w1_1080P_param[] = {1140, 61};
tcWord Mode10_w2_1080P_param[] = {1140, 603};

tcWord Mode0_720P_param[] = {244, 37};
tcWord Mode1_w0_720P_param[] = {244, 37};
tcWord Mode1_w1_720P_param[] = {886, 37};
tcWord Mode1_w2_720P_param[] = {244, 401};
tcWord Mode1_w3_720P_param[] = {886, 401};
tcWord Mode2_w0_720P_param[] = {244, 37};
tcWord Mode2_w1_720P_param[] = {566, 37};
tcWord Mode2_w2_720P_param[] = {566, 401};
tcWord Mode2_w3_720P_param[] = {1206, 37};
tcWord Mode8_w0_720P_param[] = {244, 37};
tcWord Mode8_w1_720P_param[] = {886, 37};
tcWord Mode8_w2_720P_param[] = {886, 401};
tcWord Mode9_w0_720P_param[] = {886, 37};
tcWord Mode9_w1_720P_param[] = {244, 37};
tcWord Mode9_w2_720P_param[] = {244, 401};
tcWord Mode12_w0_720P_param[] = {244, 37};
tcWord Mode12_w1_720P_param[] = {886, 37};
tcWord Mode14_w0_720P_param[] = {244, 37};
tcWord Mode14_w1_720P_param[] = {670, 37};
tcWord Mode14_w2_720P_param[] = {1096, 37};

tcWord Mode10_w0_720P_param[] = {244, 37};
tcWord Mode10_w1_720P_param[] = {886, 37};
tcWord Mode10_w2_720P_param[] = {886, 401};
#if (TH0701024600NYR50S1 == OFF)
tcWord Mode0_800x480_param[] = {48 + 350 + 48, 24 + 3};
tcWord Mode1_w0_800x480_param[] = {48 + 200, 24 + 3};
tcWord Mode1_w1_800x480_param[] = {452 + 200, 24 + 3};
tcWord Mode1_w2_800x480_param[] = {48 + 200, 265 + 3};
tcWord Mode1_w3_800x480_param[] = {452 + 200, 265 + 3};

tcWord Mode2_w0_800x480_param[] = {48, 24};
tcWord Mode2_w1_800x480_param[] = {251, 24};
tcWord Mode2_w2_800x480_param[] = {251, 265};
tcWord Mode2_w3_800x480_param[] = {653, 24};
tcWord Mode8_w0_800x480_param[] = {48 + 200, 24 + 3};
tcWord Mode8_w1_800x480_param[] = {452 + 200, 24 + 3};
tcWord Mode8_w2_800x480_param[] = {452 + 200, 265 + 3};
tcWord Mode9_w0_800x480_param[] = {452 + 200, 24 + 3};
tcWord Mode9_w1_800x480_param[] = {48 + 200, 24 + 3};
tcWord Mode9_w2_800x480_param[] = {48 + 200, 265 + 3};
tcWord Mode12_w0_800x480_param[] = {48 + 200, 24 + 3};
tcWord Mode12_w1_800x480_param[] = {452 + 200, 24 + 3};
tcWord Mode14_w0_800x480_param[] = {48, 24};
tcWord Mode14_w1_800x480_param[] = {318, 24};
tcWord Mode14_w2_800x480_param[] = {588, 24};

tcWord Mode10_w0_800x480_param[] = {48 + 350 + 48, 24 + 3};
tcWord Mode10_w1_800x480_param[] = {48 + 200, 265 + 3};
tcWord Mode10_w2_800x480_param[] = {452 + 200, 265 + 3};
#else
tcWord Mode0_800x480_param[] = {48 + 350 + 48 + 224, 24 + 3};
tcWord Mode1_w0_800x480_param[] = {48 + 200 + 174, 24 + 3};
tcWord Mode1_w1_800x480_param[] = {452 + 200 + 274, 24 + 3};
tcWord Mode1_w2_800x480_param[] = {48 + 200 + 174, 265 + 3 + 60};
tcWord Mode1_w3_800x480_param[] = {452 + 200 + 274, 265 + 3 + 60};

tcWord Mode2_w0_800x480_param[] = {48, 24};
tcWord Mode2_w1_800x480_param[] = {251, 24};
tcWord Mode2_w2_800x480_param[] = {251, 265};
tcWord Mode2_w3_800x480_param[] = {653, 24};
tcWord Mode8_w0_800x480_param[] = {48 + 200 + 174, 24 + 3};
tcWord Mode8_w1_800x480_param[] = {452 + 200 + 274, 24 + 3};
tcWord Mode8_w2_800x480_param[] = {452 + 200 + 274, 265 + 3 + 60};
tcWord Mode9_w0_800x480_param[] = {452 + 200 + 274, 24 + 3};
tcWord Mode9_w1_800x480_param[] = {48 + 200 + 174, 24 + 3};
tcWord Mode9_w2_800x480_param[] = {48 + 200 + 174, 265 + 3 + 60};
tcWord Mode12_w0_800x480_param[] = {48 + 200 + 174, 24 + 3};
tcWord Mode12_w1_800x480_param[] = {452 + 200 + 274, 24 + 3};
tcWord Mode14_w0_800x480_param[] = {48, 24};
tcWord Mode14_w1_800x480_param[] = {318, 24};
tcWord Mode14_w2_800x480_param[] = {588, 24};

tcWord Mode10_w0_800x480_param[] = {48 + 350 + 48 + 224, 24 + 3};
tcWord Mode10_w1_800x480_param[] = {48 + 200 + 174, 265 + 3 + 60};
tcWord Mode10_w2_800x480_param[] = {452 + 200 + 274, 265 + 3 + 60};

#endif

#if (TH0701024600NYR50S1 == OFF)
// OSD Video Loss position
tcWord Mode0S_800x480_param[] = {48 + 324, 24 + 200};

tcWord Mode1S_w0_800x480_param[] = {48 + 125, 24 + 75};
tcWord Mode1S_w1_800x480_param[] = {452 + 125, 24 + 75};
tcWord Mode1S_w2_800x480_param[] = {48 + 125, 265 + 75};
tcWord Mode1S_w3_800x480_param[] = {452 + 125, 265 + 75};

tcWord Mode12S_w0_800x480_param[] = {48 + 125, 24 + 75 + 100 + 24};
tcWord Mode12S_w1_800x480_param[] = {452 + 125, 24 + 75 + 100 + 24};

tcWord Mode10S_w0_800x480_param[] = {48 + 324, 24 + 75};
tcWord Mode10S_w1_800x480_param[] = {48 + 125, 265 + 75};
tcWord Mode10S_w2_800x480_param[] = {452 + 125, 265 + 75};

tcWord Mode8S_w0_800x480_param[] = {48 + 125, 24 + 75 + 100 + 24};
tcWord Mode8S_w1_800x480_param[] = {452 + 125, 24 + 75};
tcWord Mode8S_w2_800x480_param[] = {452 + 125, 265 + 75};

tcWord Mode9S_w0_800x480_param[] = {452 + 125, 24 + 75 + 100 + 24};
tcWord Mode9S_w1_800x480_param[] = {48 + 125, 24 + 75};
tcWord Mode9S_w2_800x480_param[] = {48 + 125, 265 + 75};
#else
tcWord Mode0S_800x480_param[] = {48 + 324 + 224, 24 + 200 + 40};

tcWord Mode1S_w0_800x480_param[] = {48 + 125 + 174, 24 + 75 + 40};
tcWord Mode1S_w1_800x480_param[] = {452 + 125 + 274, 24 + 75 + 40};
tcWord Mode1S_w2_800x480_param[] = {48 + 125 + 174, 265 + 75 + 90};
tcWord Mode1S_w3_800x480_param[] = {452 + 125 + 274, 265 + 75 + 90};

tcWord Mode12S_w0_800x480_param[] = {48 + 125 + 177, 24 + 75 + 100 + 24 + 40};
tcWord Mode12S_w1_800x480_param[] = {452 + 125 + 274, 24 + 75 + 100 + 24 + 40};

tcWord Mode10S_w0_800x480_param[] = {48 + 324 + 224, 24 + 75 + 40};
tcWord Mode10S_w1_800x480_param[] = {48 + 125 + 174, 265 + 75 + 90};
tcWord Mode10S_w2_800x480_param[] = {452 + 125 + 274, 265 + 75 + 90};

tcWord Mode8S_w0_800x480_param[] = {48 + 125 + 174, 24 + 75 + 100 + 24 + 40};
tcWord Mode8S_w1_800x480_param[] = {452 + 125 + 274, 24 + 75 + 40};
tcWord Mode8S_w2_800x480_param[] = {452 + 125 + 274, 265 + 75 + 90};

tcWord Mode9S_w0_800x480_param[] = {452 + 125 + 274, 24 + 75 + 100 + 24 + 40};
tcWord Mode9S_w1_800x480_param[] = {48 + 125 + 174, 24 + 75 + 40};
tcWord Mode9S_w2_800x480_param[] = {48 + 125 + 174, 265 + 75 + 90};

#endif

tcByte aLineModeReg[] = {0xB8, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xB9};

tcByte aLineMode1080PMode1Lin1[] = {0x01, 0x04, 0x25, 0x5c, 0x04, 0x51, 0x83};
tcByte aLineMode1080PMode1Lin2[] = {0x02, 0x08, 0x94, 0x13, 0x82, 0x41, 0x83};
tcByte aLineMode1080PMode2Lin1[] = {0x01, 0x04, 0x25, 0x5C, 0x02, 0x72, 0x83};
tcByte aLineMode1080PMode2Lin2[] = {0x02, 0x04, 0x25, 0x5C, 0x06, 0x32, 0x83};
tcByte aLineMode1080PMode2Lin3[] = {0x03, 0x26, 0x74, 0x33, 0x82, 0x41, 0x83};
tcByte aLineMode1080PMode8Lin1[] = {0x01, 0x04, 0x25, 0x5A, 0x04, 0x53, 0x83};
tcByte aLineMode1080PMode8Lin2[] = {0x02, 0x48, 0x53, 0x14, 0x82, 0x40, 0x83};
tcByte aLineMode1080PMode9Lin1[] = {0x01, 0x04, 0x25, 0x5A, 0x04, 0x53, 0x83};
tcByte aLineMode1080PMode9Lin2[] = {0x02, 0x04, 0x93, 0x53, 0x82, 0x40, 0x83};
tcByte aLineMode1080PMode12Lin1[] = {0x01, 0x04, 0x25, 0x5C, 0x04, 0x51, 0x83};
tcByte aLineMode1080PMode14Lin1[] = {0x01, 0x04, 0x25, 0x5C, 0x03, 0x12, 0x83};
tcByte aLineMode1080PMode14Lin2[] = {0x02, 0x04, 0x25, 0x5C, 0x05, 0x92, 0x83};

tcByte aLineMode720PMode1Lin1[] = {0x01, 0x02, 0x15, 0xE4, 0x03, 0x5A, 0x83};
tcByte aLineMode720PMode1Lin2[] = {0x02, 0x05, 0xDC, 0xDB, 0x81, 0x7D, 0x83};
tcByte aLineMode720PMode2Lin1[] = {0x01, 0x02, 0x15, 0xE4, 0x02, 0x1A, 0x83};
tcByte aLineMode720PMode2Lin2[] = {0x02, 0x02, 0x15, 0xE4, 0x04, 0x9A, 0x83};
tcByte aLineMode720PMode2Lin3[] = {0x03, 0x24, 0x1B, 0x9B, 0x81, 0x7D, 0x83};
tcByte aLineMode720PMode8Lin1[] = {0x01, 0x02, 0x14, 0xE7, 0x03, 0x5A, 0x83};
tcByte aLineMode720PMode8Lin2[] = {0x02, 0x35, 0x5A, 0xDC, 0x81, 0x7E, 0x83};
tcByte aLineMode720PMode9Lin1[] = {0x01, 0x02, 0x15, 0xE6, 0x03, 0x5A, 0x83};
tcByte aLineMode720PMode9Lin2[] = {0x02, 0x03, 0x94, 0x5B, 0x81, 0x7E, 0x83};
tcByte aLineMode720PMode12Lin1[] = {0x01, 0x02, 0x15, 0xE4, 0x03, 0x5A, 0x83};
tcByte aLineMode720PMode14Lin1[] = {0x01, 0x02, 0x16, 0xE4, 0x02, 0x84, 0x83};
tcByte aLineMode720PMode14Lin2[] = {0x02, 0x02, 0x16, 0xE4, 0x04, 0x2E, 0x83};

#if (TH0701024600NYR50S1 == OFF)
tcByte aLineMode800x480Mode1Lin1[] = {0x01, 0x02, 0x00, 0xE4, 0x01, 0x97 + 1, 0x81 + 1};
tcByte aLineMode800x480Mode1Lin2[] = {0x02, 0x03, 0x00, 0x2A, 0x81, 0x05 - 1, 0x81 + 1};

tcByte aLineMode800x480Mode2Lin1[] = {0x01, 0x02, 0x00, 0xE4, 0x00, 0xCF, 0x81 + 1};
tcByte aLineMode800x480Mode2Lin2[] = {0x02, 0x02, 0xCF, 0x62, 0x81, 0x05, 0x81 + 1};
tcByte aLineMode800x480Mode2Lin3[] = {0x03, 0x02, 0x00, 0xE4, 0x02, 0x61, 0x81 + 1};

tcByte aLineMode800x480Mode10Lin1[] = {0x01, 0x02 | 0x10, 0x00 + 5, 0x98 + 1, 0x01, 0x97 + 1, 0x81 + 1};
tcByte aLineMode800x480Mode10Lin2[] = {0x02, /*0x13*/ 0x03, /*0x98*/ 0, 0x20 + 5, 0x81, 0x05 - 1, 0x81 + 1};

tcByte aLineMode800x480Mode8Lin1[] = {0x01, 0x02, 0x00, 0xE4, 0x01, 0x97, 0x81 + 1};
tcByte aLineMode800x480Mode8Lin2[] = {0x02, 0x13, 0x98, 0x20 + 5, 0x81, 0x05 - 1, 0x81 + 1};

tcByte aLineMode800x480Mode9Lin1[] = {0x01, 0x02, 0x00, 0xE4, 0x01, 0x97, 0x81 + 1};
tcByte aLineMode800x480Mode9Lin2[] = {0x02, 0x01, 0x00, 0x97, 0x81, 0x05, 0x81 + 1};

tcByte aLineMode800x480Mode12Lin1[] = {0x01, 0x02, 0x00, 0xE4, 0x01, 0x97 + 1, 0x81 + 1};

tcByte aLineMode800x480Mode14Lin1[] = {0x01, 0x02, 0x00, 0xE4, 0x01, 0x11, 0x81 + 1};
tcByte aLineMode800x480Mode14Lin2[] = {0x02, 0x02, 0x00, 0xE4, 0x02, 0x1C, 0x81 + 1};

#else
tcByte aLineMode1024x600Mode1Lin1[] = {0x01, 0x02, 0x00, 0x6F, 0x02, 0x9F + 2, 0x83};
tcByte aLineMode1024x600Mode1Lin2[] = {0x02, 0x04, 0x00, 0xB0, 0x81, 0x43, 0x83};
tcByte aLineMode1024x600Mode2Lin1[] = {0x01, 0x04, 0x00, 0xB0, 0x01, 0x9F, 0x83};
tcByte aLineMode1024x600Mode2Lin2[] = {0x02, 0x04, 0x00, 0xB0, 0x03, 0x9F, 0x83};
tcByte aLineMode1024x600Mode2Lin3[] = {0x03, 0x13, 0x9F, 0x9F, 0x81, 0x43, 0x83};
tcByte aLineMode1024x600Mode8Lin1[] = {0x01, 0x02, 0x00, 0x6F, 0x02, 0x9F + 1, 0x83};
tcByte aLineMode1024x600Mode8Lin2[] = {0x02, 0x24, 0x9F + 1, 0xB0, 0x81, 0x43, 0x83};
tcByte aLineMode1024x600Mode9Lin1[] = {0x01, 0x02, 0x00, 0x6F, 0x02, 0x9F, 0x83};
tcByte aLineMode1024x600Mode9Lin2[] = {0x02, 0x02, 0x00, 0x9F, 0x81, 0x43, 0x83};
tcByte aLineMode1024x600Mode10Lin1[] = {0x01, 0x12, 0x43, 0x6F, 0x02, 0x9F + 1, 0x83};
tcByte aLineMode1024x600Mode10Lin2[] = {0x02, 0x04, 0x00, 0xB0, 0x81, 0x43, 0x83};
tcByte aLineMode1024x600Mode12Lin1[] = {0x01, 0x02, 0x00, 0x6F, 0x02, 0x9F + 1, 0x83};
tcByte aLineMode1024x600Mode14Lin1[] = {0x01, 0x03, 0x00, 0x20, 0x01, 0xF4, 0x83};
tcByte aLineMode1024x600Mode14Lin2[] = {0x02, 0x03, 0x00, 0x20, 0x03, 0x4A, 0x83};
#endif

///////////////////////////////////////////////////////////////////////////////////////
BYTE Mapping1(int fromValue, CODE_P struct RegisterInfo *fromRange,
			  int *toValue, CODE_P struct RegisterInfo *toRange)
{

	// calculate intermediate values
	int a;
	int b;

	// perform mapping
	if (fromValue <= fromRange->Default)
	{
		a = toRange->Default - toRange->Min;
		b = fromRange->Default - fromRange->Min;
		// prevent divide by zero
		if (b == 0)
			return (FALSE);
		*toValue = (int)((DWORD)fromValue - (DWORD)fromRange->Min) * a / b + (DWORD)toRange->Min;
	}
	else
	{
		a = toRange->Max - toRange->Default;
		b = fromRange->Max - fromRange->Default;
		// prevent divide by zero
		if (b == 0)
			return (FALSE);
		*toValue = (int)((DWORD)fromValue - (DWORD)fromRange->Default) * a / b + (DWORD)toRange->Default;
	}

	return (TRUE);
}

WORD GetItemValue(BYTE id)
{
	WORD val = 0;

	switch (id)
	{
	case VIDEO_CONTRAST:
		val = ReadEEP(EEP_CONTRAST);
		break;
	case VIDEO_BRIGHTNESS:
		val = ReadEEP(EEP_BRIGHTNESS);
		break;
	case VIDEO_SATURATION:
		val = ReadEEP(EEP_SATURATION_U);
		break;
	case VIDEO_HUE:
		val = ReadEEP(EEP_HUE);
		break;
	case VIDEO_SHARPNESS:
		val = ReadEEP(EEP_SHARPNESS);
		break;
	case VIDEO_BACKLIGHT:
		val = ReadEEP(EEP_BACKLIGHT);
		break;
	case CAMOSDONOFF:
		val = ReadEEP(EEP_COSDMode);
		break;
	case DayChoice:
		val = ReadEEP(EEP_AutoMode);
		break;
	case GAUGEChoice:
		val = ReadEEP(EEP_GAUGEMode);
		break;
	case OsdLockChoice:
		val = ReadEEP(EEP_ONOFFChoice);
		break;
	case PRIORITYChoice:
		val = ReadEEP(EEP_PRIORITY);
		break;
	case AutoSleeping:
		val = ReadEEP(EEP_Auto_Sleeping);
		break;
	case TripleScreenType:
		val = ReadEEP(EEP_Triple_Screen_Type);
		break;
	case DistanceGaugePosition:
		val = ReadEEP(EEP_Distance_Gauge_Position);
		break;
	case DistanceGaugeWidth:
		val = ReadEEP(EEP_Distance_Gauge_Width);
		break;
	case MenutouchChoice:
		val = ReadEEP(EEP_MenuTouch);
		break;
	case CAMAONOFF:
		val = ReadEEP(EEP_CAMAMode);
		break;
	case CAMBONOFF:
		val = ReadEEP(EEP_CAMBMode);
		break;
	case CAMCONOFF:
		val = ReadEEP(EEP_CAMCMode);
		break;
	case CAMRONOFF:
		val = ReadEEP(EEP_CAMRMode);
		break;
	case CAMDONOFF:
		val = ReadEEP(EEP_CAMDMode);
		break;
	case IMAGE_A_Choice:
		val = ReadEEP(EEP_IMAGE_A_Mode);
		break;
	case IMAGE_B_Choice:
		val = ReadEEP(EEP_IMAGE_B_Mode);
		break;
	case IMAGE_C_Choice:
		val = ReadEEP(EEP_IMAGE_C_Mode);
		break;
	case REARChoice:
		val = ReadEEP(EEP_RearMode);
		break;
	case JUMPChoice:
		val = ReadEEP(EEP_JUMPMode);
		break;
	case STIME_STEP:
		val = ReadEEP(EEP_TimeStep);
		break;
	case DELAYCAMA:
		val = ReadEEP(EEP_DELAYCAMA);
		break;
	case DELAYCAMB:
		val = ReadEEP(EEP_DELAYCAMB);
		break;
	case DELAYCAMC:
		val = ReadEEP(EEP_DELAYCAMC);
		break;
	case DELAYCAMR:
		val = ReadEEP(EEP_DELAYCAMR);
		break;
	case DELAYCAMD:
		val = ReadEEP(EEP_DELAYCAMD);
		break;

#if (RGBGAINOSD == ON)
	case VIDEO_R_B_GAIN:
	case VIDEO_G_B_GAIN:
	case VIDEO_B_B_GAIN:
	case VIDEO_R_C_GAIN:
	case VIDEO_G_C_GAIN:
	case VIDEO_B_C_GAIN:
		val = VXISI2CRead(id);
		break;
#endif
#if (TPMSOSD == ON)

	case TIRE1_High_PSI:
		val = ReadEEP(EEP_TIRE1_High_PSI);
		break;
	case TIRE1_Low_PSI:
		val = ReadEEP(EEP_TIRE1_Low_PSI);
		break;
	case TIRE1_High_Temp:
		val = ReadEEP(EEP_TIRE1_High_Temp);
		break;
	case TIRE2_High_PSI:
		val = ReadEEP(EEP_TIRE2_High_PSI);
		break;
	case TIRE2_Low_PSI:
		val = ReadEEP(EEP_TIRE2_Low_PSI);
		break;
	case TIRE2_High_Temp:
		val = ReadEEP(EEP_TIRE2_High_Temp);
		break;
	case TIRE3_High_PSI:
		val = ReadEEP(EEP_TIRE3_High_PSI);
		break;
	case TIRE3_Low_PSI:
		val = ReadEEP(EEP_TIRE3_Low_PSI);
		break;
	case TIRE3_High_Temp:
		val = ReadEEP(EEP_TIRE3_High_Temp);
		break;
	case TIRE4_High_PSI:
		val = ReadEEP(EEP_TIRE4_High_PSI);
		break;
	case TIRE4_Low_PSI:
		val = ReadEEP(EEP_TIRE4_Low_PSI);
		break;
	case TIRE4_High_Temp:
		val = ReadEEP(EEP_TIRE4_High_Temp);
		break;
	case TIRE5_High_PSI:
		val = ReadEEP(EEP_TIRE5_High_PSI);
		break;
	case TIRE5_Low_PSI:
		val = ReadEEP(EEP_TIRE5_Low_PSI);
		break;
	case TIRE5_High_Temp:
		val = ReadEEP(EEP_TIRE5_High_Temp);
		break;
	case TIRE6_High_PSI:
		val = ReadEEP(EEP_TIRE6_High_PSI);
		break;
	case TIRE6_Low_PSI:
		val = ReadEEP(EEP_TIRE6_Low_PSI);
		break;
	case TIRE6_High_Temp:
		val = ReadEEP(EEP_TIRE6_High_Temp);
		break;

#endif
	case Grabber_Liner:
		val = ReadEEP(EEP_Grabber_Liner);
		break;
	case Grabber_Liner_Width:
		if (bytGrabberAdjust == Grabber_Liner_Width)
			val = ReadEEP(EEP_Grabber_Liner_Width);
		else
			val = ReadEEP(EEP_Grabber_Liner_Position);

		break;
	case Grabber_Liner_Position:
		val = ReadEEP(EEP_Grabber_Liner_Position);
		break;
	}

	return val;
}

BYTE SetOSDStep(BYTE id, BYTE newv)
{

	int regv;
	struct RegisterInfo ptr;

	switch (id)
	{
	case VIDEO_BRIGHTNESS:
		ptr = BrightnessRange;
		break;
	case VIDEO_CONTRAST:
		ptr = ContrastRange;
		break;
	case VIDEO_SATURATION:
		ptr = SaturationRange;
		break;
	case VIDEO_HUE:
		ptr = HueRange;
		break;
	case VIDEO_SHARPNESS:
		ptr = SharpnessRange;
		break;
	case VIDEO_BACKLIGHT:
		ptr = BacklightRange;
		break;
	case Grabber_Liner_Position:

		if ((bytGarbberLinerMode == VS4210_LOOP_MODE0_w2) || (bytGarbberLinerMode == VS4210_LOOP_MODE10_312) || (bytGarbberLinerMode == VS4210_LOOP_MODE10_314) || (bytGarbberLinerMode == VS4210_LOOP_MODE10_324))
			ptr = GrabberLinerWinSize1;
		else
			ptr = GrabberLinerWinSize2;
		break;
	}

	Mapping1(newv, &UserRange, &regv, &ptr);

	// if(id==Grabber_Liner_Position)
	//		bytSetGarbberLinerPosition=regv;

	// Printf("\r\nSETOSD id=%02x val=%02x",(WORD) id,(WORD) regv);
	////���蝺砍��蝺餃��隢�撖怠��������頦���凌�批��銴�摮����頦����
	//	WriteAD5110(AD5110_CMD_Write_Data_To_RDAC ,AD5110_setp[regv]);

	return regv;
}

int GetOSDStep(BYTE id, BYTE newv)
{
	int regv;
	struct RegisterInfo ptr;

	switch (id)
	{
	case Grabber_Liner_Position:
		if (bytGarbberLinerMode == VS4210_LOOP_MODE0_w2)
			ptr = GrabberLinerWinSize1;
		else
			ptr = GrabberLinerWinSize2;
		break;
	}

	Mapping1(newv, &UserRange, &regv, &ptr);

	return regv;
}

BYTE ChangeVideoData(BYTE id, BYTE flag)
{
	int newv, inc, reg;

	WORD pos;

	if (id == Grabber_Liner_Position)
		inc = (flag == UP ? 4 : -4);
	else
		inc = (flag == UP ? 1 : -1);

	if (id == Grabber_Liner_Position)
	{
		if (bytGarbberLinerPosition < 4)
		{
			pos = GetOSDStep(Grabber_Liner_Position, 0);
		}
		else
			pos = GetOSDStep(Grabber_Liner_Position, bytGarbberLinerPosition + inc);

		if (bytGarbberLinerMode == VS4210_LOOP_MODE0_w2)
		{
			//		(((((((val*2)+5)*FontWidthSIZE)/2)+pos)>800)||(((((val*2)+5)*FontWidthSIZE)/2)>pos))
			// if(((((bytGarbberLinerWidth*2)+5)*FontWidthSIZE)+pos)>800)
			if (((((((bytGarbberLinerWidth * 2) + 5) * FontWidthSIZE) / 2) + pos) > 800) || ((((((bytGarbberLinerWidth * 2) + 5) * FontWidthSIZE) / 2) + 100) > pos))
			{
				inc = 0;
				// Printf("(#PER1=%d)",(WORD)((((bytGarbberLinerWidth*2)+5)*FontWidthSIZE)+pos));
				Printf("(#PER1=%d)", (WORD)pos);
			}
			else
				Printf("(#PS1=%d)", (WORD)pos);
			// Printf("(#PS1=%d)",(WORD)((((bytGarbberLinerWidth*2)+5)*FontWidthSIZE)+pos));
		}
		else
		{
			if (((((bytGarbberLinerWidth * 2) + 5) * FontWidthSIZE2) + pos) > 400)
			{
				inc = 0;
				Printf("(#PER1=%d)", (WORD)((((bytGarbberLinerWidth * 2) + 5) * FontWidthSIZE2) + pos));
			}
			else
				Printf("(#PS1=%d)", (WORD)((((bytGarbberLinerWidth * 2) + 5) * FontWidthSIZE2) + pos));
		}
	}
	newv = OSDItemValue + inc;
	if (newv < UserRange.Min || newv > UserRange.Max)
	{
		return OSDItemValue;
	}

	reg = SetOSDStep(id, newv);

	switch (id)
	{
	case VIDEO_BRIGHTNESS:
		VXISI2CWrite(VS4210_BRIGHTNESS, reg);
		break;
	case VIDEO_CONTRAST:
		VXISI2CWrite(VS4210_CONTRAST, reg);
		break;
	case VIDEO_SATURATION:
		VXISI2CWrite(VS4210_SATURATION, reg);
		break;
	case VIDEO_HUE:
		VXISI2CWrite(VS4210_HUE, ((VXISI2CRead(VS4210_HUE) & 0xC0) | reg));
		break;
	case VIDEO_SHARPNESS:
		VXISI2CWrite(VS4210_SHARPNESS, reg);
		break;
	case VIDEO_BACKLIGHT:
		GetBackLightLevel();
		// Printf("(GetBackLightLevel()=%d)",(WORD)GetBackLightLevel());
		break;
	}

	OSDItemValue = newv;
	return newv;
}

int GetBackLightLevel(void)
{
	int regv;

	Mapping1(GetItemValue(VIDEO_BACKLIGHT), &UserRangeBacklight, &regv, &BacklightRange);
	iBackLightLevel = regv;

	return regv;
}

void LoadVideoData(void)
{
	BYTE reg;
	reg = SetOSDStep(VIDEO_BRIGHTNESS, GetItemValue(VIDEO_BRIGHTNESS));
	VXISI2CWrite(VS4210_BRIGHTNESS, reg);

	reg = SetOSDStep(VIDEO_SATURATION, GetItemValue(VIDEO_SATURATION));
	VXISI2CWrite(VS4210_SATURATION, reg);

	reg = SetOSDStep(VIDEO_CONTRAST, GetItemValue(VIDEO_CONTRAST));
	VXISI2CWrite(VS4210_CONTRAST, reg);

	reg = SetOSDStep(VIDEO_HUE, GetItemValue(VIDEO_HUE));
	VXISI2CWrite(VS4210_HUE, ((VXISI2CRead(VS4210_HUE) & 0xC0) | reg));

	reg = SetOSDStep(VIDEO_SHARPNESS, GetItemValue(VIDEO_SHARPNESS));
	VXISI2CWrite(0x4B, 0xA0); /// enable sharpness
	VXISI2CWrite(VS4210_SHARPNESS, reg);

#if (StartPowerBlackScreen == OFF)
	user_pwm_setvalue(GetBackLightLevel());
#endif

	/*
	gpio_i2c_write(TVI_I2C, TVI_BRIGHTNESS,0);
	gpio_i2c_write(TVI_I2C, TVI_CONTRAST,64);
	gpio_i2c_write(TVI_I2C, TVI_SATURATION,50);
	gpio_i2c_write(TVI_I2C, TVI_HUE,0);
	gpio_i2c_write(TVI_I2C,TVI_SHARPNESS,0);
	*/
}
void Reset_OSD_SPEAKER(void)
{
	// if((GetDisplayedOSD() & TVVOL)==TVVOL)

	if (OSD_SPEAKER[0] == ON)
	{
		if (AudioVol == 0)
		{
			OsdDumpString(0 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE);
			OsdDumpAttribute(0 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE, CH_COLOR_GREEN);
		}
		else
		{
			OsdDumpString(0 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER);
			OsdDumpAttribute(0 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER, CH_COLOR_GREEN);
		}
	}
	else if (OSD_SPEAKER[1] == ON)
	{
		if (AudioVol == 0)
		{
			OsdDumpString(6 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE);
			OsdDumpAttribute(6 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE, CH_COLOR_GREEN);
		}
		else
		{
			OsdDumpString(6 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER);
			OsdDumpAttribute(6 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER, CH_COLOR_GREEN);
		}
	}
	else if (OSD_SPEAKER[2] == ON)
	{
		if (AudioVol == 0)
		{
			OsdDumpString(12 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE);
			OsdDumpAttribute(12 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE, CH_COLOR_GREEN);
		}
		else
		{
			OsdDumpString(12 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER);
			OsdDumpAttribute(12 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER, CH_COLOR_GREEN);
		}
	}
	else if (OSD_SPEAKER[3] == ON)
	{
		if (AudioVol == 0)
		{
			OsdDumpString(18 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE);
			OsdDumpAttribute(18 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE, CH_COLOR_GREEN);
		}
		else
		{
			OsdDumpString(18 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER);
			OsdDumpAttribute(18 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER, CH_COLOR_GREEN);
		}
	}
}

BYTE CloseOSDMenu(void)
{
	BYTE x, fch[4] = {0, 2, 3, 1};

	Printf("(CloseOSDMenu..)");

	VXIS_fSetOsdOnOff(0, _TitleOn | _BottomOn);
	// VXIS_fSetOsdOnOff(1 , _TitleOn | _BottomOn);
	// VXIS_fSetOsdOnOff(2 , _TitleOn | _BottomOn);
	// VXIS_fSetOsdOnOff(3 , _TitleOn | _BottomOn);

	if (((GetDisplayedOSD() & MENU) == MENU))
	{
		for (x = 0; x < 8; x++)
		{
			OsdDumpString(MENU_START + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr);
			OsdDumpAttribute(MENU_START + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
		}
	}

	if (((GetDisplayedOSD() & _COC_OSD) == _COC_OSD))
	{
		//	SetCOCCom(_COCOSD_BOXONLINE,SelectModeType);
		//	Get_2824cIrq();

		if (Flisheye_CAM[fch[SelectModeType]] == TRUE)
		{
			// Flisheye_CAM[SelectModeType]=FALSE;
			SetCOCCom(_COCOSD_BOX_Sel, SelectModeType);
			SetCOCCom(_COC_FLISHEYE_MODE1, Fisheye_save); /// save data
			Get_2824cIrq();
		}
		if (Flisheye_COC)
		{
			switch (SelectModeType)
			{
			case CH1:
				CAMA_M = 1;
				VS4210_ResetCamMirror(CH1);
				break;

			case CH2:
				CAMB_M = 1;
				VS4210_ResetCamMirror(CH2);
				break;

			case CH3:
				CAMC_M = 1;
				VS4210_ResetCamMirror(CH3);
				break;

			case CH4:
				CAMR_M = 1;
				VS4210_ResetCamMirror(CH4);
				break;
			}
			Flisheye_COC = 0;
		}
	}

	Reset_OSD_SPEAKER();

	ClearDisplayedOSD((MENU | TVVOL | _GAUGE_OSD | _COC_OSD | _RUN_TPMS_OSD | _GRABBER_LINER_OSD));

	SHOWOSDINFO();

	// if(GetItemValue(CAMOSDONOFF)==_FALSE)
	//{
	MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_DISPLAY_DISABLE);
	MCUTimerActiveTimerEvent(SEC(2), _USER_TIMER_EVENT_DISPLAY_DISABLE);

	MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);

	bytGrabberAdjust = 0;

	//}

	SetNowMode();

	if ((NowModeState == JumpMode) && (TriggerFlag == 0))
		ShowJUMPOSD();

	MenuTouchMode = OFF;
	MenuTouchFlag = OFF;
	bytGaugeAdjust = OFF;
	bytCOCAdjust = OFF;

	return 0;
}
BYTE CloseOSDINFO(void)
{

	Printf("\r\nCloseOSDINFO");

	/*
	VS4210_1080P_MODE0_w0 = 0x00,
	VS4210_1080P_MODE0_w1 = 0x01,
	VS4210_1080P_MODE0_w2 = 0x02,
	VS4210_1080P_MODE0_w3 = 0x03,
	VS4210_1080P_MODE1	  = 0x10,
	 VS4210_1080P_MODE2    = 0x20,
	VS4210_1080P_MODE10    = 0x25,
	  VS4210_1080P_MODE8	= 0x30,
	VS4210_1080P_MODE9	  = 0x40,
	VS4210_1080P_MODE12   = 0x50,
	VS4210_1080P_MODE14   = 0x60,
	VS4210_1080P_MODE_NUMBER=0x90,*/

	switch (gbVXIS_OuputModeflg)
	{
	case VS4210_1080P_MODE0_w0:
	case VS4210_1080P_MODE0_w1:
	case VS4210_1080P_MODE0_w2:
	case VS4210_1080P_MODE0_w3:
	case VS4210_1080P_MODE0_w4:

		if (GetDisplayedOSD() & (MENU | TVVOL | _GRABBER_LINER_OSD | _COC_OSD))
		{
			VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn | WINZOOMx2);
		}
		else
		{
			if ((GetDisplayedOSD() & _GRABBER_LINER_OSD) == _GRABBER_LINER_OSD)
				VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn | WINZOOMx2);
			else
				VXIS_fSetOsdOnOff(0, _BottomOn);
		}
		break;

	case VS4210_1080P_MODE1:
		if (GetDisplayedOSD() & (MENU | TVVOL | _GRABBER_LINER_OSD | _COC_OSD))
		{
			VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn);
			VXIS_fSetOsdOnOff(1, _ContentOn | _BottomOn);
			VXIS_fSetOsdOnOff(2, _ContentOn | _BottomOn);
			VXIS_fSetOsdOnOff(3, _ContentOn | _BottomOn);
		}
		else
		{
			if ((GetDisplayedOSD() & _GRABBER_LINER_OSD) == _GRABBER_LINER_OSD)
			{
				VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn);
				VXIS_fSetOsdOnOff(1, _ContentOn | _BottomOn);
				VXIS_fSetOsdOnOff(2, _ContentOn | _BottomOn);
				VXIS_fSetOsdOnOff(3, _ContentOn | _BottomOn);
			}
			else
			{
				VXIS_fSetOsdOnOff(0, _BottomOn);
				VXIS_fSetOsdOnOff(1, _BottomOn);
				VXIS_fSetOsdOnOff(2, _BottomOn);
				VXIS_fSetOsdOnOff(3, _BottomOn);
			}
		}
		break;
	case VS4210_1080P_MODE12:
		if (GetDisplayedOSD() & (MENU | TVVOL | _GRABBER_LINER_OSD | _COC_OSD))
		{
			VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn);
			VXIS_fSetOsdOnOff(1, _ContentOn | _BottomOn);
			VXIS_fSetOsdOnOff(2, _ContentOn | _BottomOn);
			VXIS_fSetOsdOnOff(3, _ContentOn | _BottomOn);
			Printf("(*P1)");
		}
		else
		{
			Printf("(*P2)");
			if ((GetDisplayedOSD() & _GRABBER_LINER_OSD) == _GRABBER_LINER_OSD)
			{
				VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn);
				VXIS_fSetOsdOnOff(1, _ContentOn | _BottomOn);
				VXIS_fSetOsdOnOff(2, _ContentOn | _BottomOn);
				VXIS_fSetOsdOnOff(3, _ContentOn | _BottomOn);
			}
			else
			{
				VXIS_fSetOsdOnOff(0, _BottomOn);
				VXIS_fSetOsdOnOff(1, _BottomOn);
				VXIS_fSetOsdOnOff(2, _BottomOn);
				VXIS_fSetOsdOnOff(3, _BottomOn);
			}
		}
		break;

	case VS4210_1080P_MODE10:
	case VS4210_1080P_MODE8:
	case VS4210_1080P_MODE9:
		if (GetDisplayedOSD() & (MENU | TVVOL))
		{
			VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn);
			VXIS_fSetOsdOnOff(1, _ContentOn | _BottomOn);
			VXIS_fSetOsdOnOff(2, _ContentOn | _BottomOn);
			//	VXIS_fSetOsdOnOff(3 , _ContentOn|_BottomOn);
		}
		else
		{
			VXIS_fSetOsdOnOff(0, _BottomOn);
			VXIS_fSetOsdOnOff(1, _BottomOn);
			VXIS_fSetOsdOnOff(2, _BottomOn);
			//	VXIS_fSetOsdOnOff(3 , _BottomOn);
		}
		break;
	}
	/*
	if(	GetDisplayedOSD() & (MENU|TVVOL))
	{
		VXIS_fSetOsdOnOff(0 , _ContentOn|_BottomOn);
		VXIS_fSetOsdOnOff(1 , _ContentOn|_BottomOn);
		VXIS_fSetOsdOnOff(2 , _ContentOn|_BottomOn);
		VXIS_fSetOsdOnOff(3 , _ContentOn|_BottomOn);

	}
	else
	{
		VXIS_fSetOsdOnOff(0 , _BottomOn);
		VXIS_fSetOsdOnOff(1 , _BottomOn);
		VXIS_fSetOsdOnOff(2 , _BottomOn);
		VXIS_fSetOsdOnOff(3 , _BottomOn);
	}
	*/
	return 0;
}

BYTE SHOWOSDINFO(void)
{
	Printf("\r\nSHOWOSDINFO=%02x", (WORD)gbVXIS_OuputModeflg);
	/*
	VS4210_1080P_MODE0_w0 = 0x00,
	VS4210_1080P_MODE0_w1 = 0x01,
	VS4210_1080P_MODE0_w2 = 0x02,
	VS4210_1080P_MODE0_w3 = 0x03,
	VS4210_1080P_MODE1	  = 0x10,
	 VS4210_1080P_MODE2    = 0x20,
	VS4210_1080P_MODE10    = 0x25,
	  VS4210_1080P_MODE8	= 0x30,
	VS4210_1080P_MODE9	  = 0x40,
	VS4210_1080P_MODE12   = 0x50,
	VS4210_1080P_MODE14   = 0x60,
	VS4210_1080P_MODE_NUMBER=0x90,*/

	switch (gbVXIS_OuputModeflg)
	{
	case VS4210_1080P_MODE0_w0:
	case VS4210_1080P_MODE0_w1:
	case VS4210_1080P_MODE0_w2:
	case VS4210_1080P_MODE0_w3:
	case VS4210_1080P_MODE0_w4:

		VXIS_fSetOsdOnOff(0, _BottomOn | _TitleOn);
		break;

	case VS4210_1080P_MODE1:

		VXIS_fSetOsdOnOff(0, _BottomOn | _TitleOn);
		VXIS_fSetOsdOnOff(1, _BottomOn | _TitleOn);
		VXIS_fSetOsdOnOff(2, _BottomOn | _TitleOn);
		VXIS_fSetOsdOnOff(3, _BottomOn | _TitleOn);
		SET_CROSS_LINE(ON);
		break;
	case VS4210_1080P_MODE12:

		VXIS_fSetOsdOnOff(0, _BottomOn | _TitleOn);
		VXIS_fSetOsdOnOff(1, _BottomOn | _TitleOn);
		SET_CROSS_LINE(ON);

		break;

	case VS4210_1080P_MODE10:
	case VS4210_1080P_MODE8:
	case VS4210_1080P_MODE9:
		VXIS_fSetOsdOnOff(0, _BottomOn | _TitleOn);
		VXIS_fSetOsdOnOff(1, _BottomOn | _TitleOn);
		VXIS_fSetOsdOnOff(2, _BottomOn | _TitleOn);
		SET_CROSS_LINE(ON);
		break;
	}
	/*
	if(	GetDisplayedOSD() & (MENU|TVVOL))
	{
		VXIS_fSetOsdOnOff(0 , _ContentOn|_BottomOn);
		VXIS_fSetOsdOnOff(1 , _ContentOn|_BottomOn);
		VXIS_fSetOsdOnOff(2 , _ContentOn|_BottomOn);
		VXIS_fSetOsdOnOff(3 , _ContentOn|_BottomOn);

	}
	else
	{
		VXIS_fSetOsdOnOff(0 , _BottomOn);
		VXIS_fSetOsdOnOff(1 , _BottomOn);
		VXIS_fSetOsdOnOff(2 , _BottomOn);
		VXIS_fSetOsdOnOff(3 , _BottomOn);
	}
	*/

	return 0;
}

#define VOLBAR_LENGTH 16 //+3//ryan@21070919

void DisplayVolumebar(BYTE val)
{
#if 1
	int regv;

#if 1
	BYTE DEC_Number[10] = {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9};
	BYTE Str_Number[] = {4, _spc, _spc, _spc, _spc};
	BYTE i /*,temp*/;
	BYTE Bar_str2[VOLBAR_LENGTH] = {14, _bar9, _bar9, _bar9, _bar9, _bar9,
									_bar9, _bar9, _bar9, _bar9, _bar9,
									_bar9, _bar9, _bar9, _bar9};

	BYTE Bar_str[14] = {_bar9, _bar9, _bar9, _bar9, _bar9,
						_bar9, _bar9, _bar9, _bar9, _bar9,
						_bar9, _bar9, _bar9, _bar9};

	BYTE BAR_ROM_TABLE[] = {
		_bar1,
		_bar2,
		_bar3,
		_bar4,
		_bar5,
		_bar6,
		_bar7,
		_bar8,
		_bar9};

	Mapping1(val, &UserRange, &regv, &AudioVolBarRange);

	// Printf("\r\nAudioVolBarRange val=%d  regv=%d",(WORD) val,(WORD) regv);
	////���蝺砍��蝺餃��隢�撖怠��������頦���凌�批��銴�摮����頦����

	/*
	BYTE BAR_ROM_TABLE[] = {
		_bar1, // ...
		_bar3,
		_bar4, // |..
		_bar5,
		_bar6,
		_bar7, // ||.
		_bar8, // |
		_bar9 // |||
	};
	*/
	/*
	BYTE BAR_ROM_TABLE[] = {
		_bar1, // ...
		0x02,0x02, // |..
		0x05,0x05,0x04, // ||.
		0x03, // |
		0x04, // |||
	};
	*/

	// if(val <100)
	if (regv < 126)
	{
#if 1

		/*
			if(val<6)
		   {
			for(i = 1; i < (16); i++)
			   {
				Bar_str2[i] = _bar1;
			   }
		   }
			else
		   {
		*/
		//		Bar_str2[((val/6-1))] = BAR_ROM_TABLE[val%(6)];

		for (i = (regv / (9)); i < (16 - 1); i++)
		{
			Bar_str[i] = _bar1;
		}

		Bar_str[((regv / 9))] = BAR_ROM_TABLE[regv % (9)];
		/*
			if((val>95)&&(val<100))
				Bar_str2[14]= _bar8;
			if((val>1)&&(val<13))
				Bar_str2[1]= _bar2;
	*/
		// if((val>49)&&(val<54))
		//	Bar_str2[7]= _bar9;
		//	 }
	}

	for (i = 1; i < 15; i++)
		Bar_str2[i] = Bar_str[i - 1];

#else
		/*
			if(val<6)
		   {
			for(i = 1; i < (16); i++)
			   {
				Bar_str2[i] = _bar1;
			   }
		   }
			else
		   {
		*/
		Bar_str2[((val / 6 - 1))] = BAR_ROM_TABLE[val % (6)];

		for (i = ((val / (6))); i < (16 - 1); i++)
		{
			Bar_str2[i] = _bar1;
		}
		//   }
	}

	Bar_str2[0] = 14;
#endif
#else
	BYTE DEC_Number[10] = {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9};
	BYTE Str_Number[] = {3, _spc, _spc, _spc};
	BYTE i, temp;
	BYTE Bar_str2[VOLBAR_LENGTH] = {15, _bar9, _bar9, _bar9, _bar9, _bar9,
									_bar9, _bar9, _bar9, _bar9, _bar9,
									_bar9, _bar9, _bar9, _bar9, _bar9};
	BYTE BAR_ROM_TABLE[] = {
		_bar1,				 // ...
		_bar3, _bar4,		 // |..
		_bar5, _bar6, _bar7, // ||.
		_bar8,				 // |
		_bar9				 // |||
	};

	/*
	BYTE BAR_ROM_TABLE[] = {
		_bar1, // ...
		0x02,0x02, // |..
		0x05,0x05,0x04, // ||.
		0x03, // |
		0x04, // |||
	};
	*/

	if (val < 100)
	{
		if (val < 6)
		{
			for (i = 1; i < (16); i++)
			{
				Bar_str2[i] = _bar1;
			}
		}
		else
		{

			Bar_str2[val / 6] = BAR_ROM_TABLE[val % 6];

			for (i = val / 6; i < (16); i++)
			{
				Bar_str2[i] = _bar1;
			}
		}
	}
#endif

		// if( val==99 )
		//	 Bar_str2[16] = _bar7; // ||.

		// if( Bar_str2[0] == 0x01 )
		//	 Bar_str2[0] = 0x02;  // |..
/*
// Number
Bar_str[20] = val % 10 + '0';	  val /= 10;
Bar_str[19] =	val % 10 + '0';    val /= 10;
Bar_str[18] = val + '0';

if( Bar_str[18] == '0' )
	Bar_str[18] = ' ';

addr = OSDMENU_BARADDR  + VOLBAR_LENGTH;
*/
#else
	BYTE BAR_ROM_TABLE[] = {
		0x01,			  // ...
		0x02, 0x02,		  // |..
		0x05, 0x05, 0x04, // ||.
		0x03,			  // |
		0x04,			  // |||
	};

	BYTE i;
	BYTE Bar_str[VOLBAR_LENGTH] = {0x04, 0x04,
								   0x04, 0x04, 0x04, 0x04, 0x04,
								   0x04, 0x04, 0x04, 0x04, 0x04,
								   0x04, 0x04, 0x04, 0x04, 0x04, 0x03, ' ', ' ', ' '};

	WORD addr;

	if (val < 100)
	{
		Bar_str[val / 6] = BAR_ROM_TABLE[val % 6];
		for (i = val / 6 + 1; i < 17; i++)
			Bar_str[i] = BAR_ROM_TABLE[0];
	}
	else
	{
	}
#endif

	//	if( val==99 )
	//		Bar_str[16] = 0x05; // ||.

	// if( Bar_str[0] == 0x01 )
	// Bar_str[0] = 0x02;	// |..

	// Number
	//    Bar_str[20] = val % 10 + '0';    val /= 10;
	//  Bar_str[19] =  val % 10 + '0';    val /= 10;
	//  Bar_str[18] = val + '0';

	//	if( Bar_str[18] == '0' )
	//	ar_str[18] = ' ';

	//	addr = OSDMENU_BARADDR  + VOLBAR_LENGTH;
	// val=GetItemValue();
	if (val / 100)
	{
		Str_Number[1] = DEC_Number[(val / 100)];
		val = val % 100;
		Str_Number[2] = DEC_Number[(val / 10)];
		val = val % 10;
		Str_Number[3] = DEC_Number[(val)];
	}
	else if (val / 10)
	{
		Str_Number[2] = DEC_Number[(val / 10)];
		val = val % 10;
		Str_Number[3] = DEC_Number[(val)];
	}
	else
		Str_Number[3] = DEC_Number[(val)];
	OsdDumpString(OSDMENU_MAINADDR + (1 * MENU_WIDTH) + 1, (tByte *)Bar_str2);
	OsdDumpString(OSDMENU_MAINADDR + (1 * MENU_WIDTH) + 17 - 1, (tByte *)Str_Number);
}

void DisplayVol(void)
{
	//	BYTE CODE *Str ;
	//	BYTE len;
	// BYTE x;
	/*
		#ifdef Hs_debug
		Printf("\r\n++(DisplayVol)");
		#endif

		WriteDecoder(0xff, 0x01);
		Wait_ms(5);
		//	WriteDecoder(0x82, 0x0c);
		WriteDecoder(0x82, 0x01);
		Wait_ms(5);
		WriteDecoder(0xff, 0x00);
		*/

	if ((DisplayedOSD & TVVOL) == 0)
	{
		// Printf("\r\nDisplay Volume bar");

		VXIS_fSetOsdOnOff(0, 0);
		VXIS_fSetOsdOnOff(1, 0);
		VXIS_fSetOsdOnOff(2, 0);
		VXIS_fSetOsdOnOff(3, 0);

		SetOsdSize(_Content, MENU_WIDTH, 2 + 1);
		SetOsdStartAddress(_Content, MENU_START);
		//	 SetOsdPosition(_Content,48,24+40);
#if (TH0701024600NYR50S1 == OFF)
		SetOsdPosition(_Content, 128 /*MENU_OSD_X_ADDR*/, 360 /*MENU_OSD_Y_ADDR+250*/);
#else
		SetOsdPosition(_Content, 128 + 224 /*MENU_OSD_X_ADDR*/, 360 + 100 /*MENU_OSD_Y_ADDR+250*/);
#endif
		OsdDumpString(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_VolumeTXT);
		OsdDumpAttribute(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_VolumeTXT, DEFAULT_COLOR);

		OsdDumpString(MENU_START + (1 * MENU_WIDTH), (tByte *)Str_VolumeBAR);
		OsdDumpAttribute(MENU_START + (1 * MENU_WIDTH), (tByte *)Str_VolumeBAR, DEFAULT_COLOR);

		if (MenuTouchMode == 1)
		{
			OsdDumpString(MENU_START + (2 * MENU_WIDTH), (tByte *)Str_VolumeTXTArrow);
			OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH), (tByte *)Str_VolumeTXTArrow, DEFAULT_COLOR);
		}
		else
		{
			OsdDumpString(MENU_START + (2 * MENU_WIDTH), (tByte *)&Str_Menu_Clr);
			OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
		}

		/*
			ClearOSDInfo();
			#ifdef REFERENCE
			Change_OSDColorLookup();
			#endif
			delay(10);

			InitOSDWindow(Init_Osd_BarWindow);

			ClearDataCnt(OSDMENU_BARADDR, 50); // Total 25*2 Char.
			DrawAttrCnt(OSDMENU_BARADDR, BG_COLOR_CYAN | CH_COLOR_WHITE, 25 );
			#ifdef TW8816
			DrawAttrCnt(OSDMENU_BARADDR+21, BG_COLOR_WHITE | CH_COLOR_CYAN, 25 );
			#else
			DrawAttrCnt(OSDMENU_BARADDR+25, BG_COLOR_WHITE | CH_COLOR_CYAN, 25 );
			#endif
	#ifdef E_Wintek
			Str = StrVolume2[GetOSDLang()];
	#else
			Str = StrVolume[GetOSDLang()];
	#endif
			len=CStrlen(Str);
			WriteStringToAddr(OSDMENU_BARADDR, Str, len);

			ShowOSDWindow(OSDBARWINDOW,TRUE);
			ShowOSDWindow(DVRStatus_OSDWIN,FALSE);
			ShowOSDWindowAll(1);
			*/
		// VXISI2CWrite(0x58,0x94);
	}
	// if((NowModeState==SelectMode&&SelectModeType==VGAMode)||(NowModeState==JumpMode&&((ReadEEP(EEP_JUMPMode)==14)||SelectModeType==VGAMode)))		//william-v1.1-20110114
	// if(VGAAudio)
	//	DisplayVolumebar(PCAudioVol);
	// else

	// AudioVol=GetAudioVolEE();

	DisplayVolumebar(AudioVol);

	WriteEEP(EEP_AUDIOPVOL, AudioVol);

	SetDisplayedOSD(TVVOL);

	// VXIS_fSetOsdOnOff(0 , _BottomOn|_ContentOn );

	VXIS_fSetOsdOnOff(0, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(1, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(2, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(3, _ContentOn | WINZOOMx2);
	SET_CROSS_LINE(OFF);

	MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);
	MCUTimerActiveTimerEvent(SEC(Volume_Osd_Time_Out), _USER_TIMER_EVENT_OSD_DISABLE);
	//	OSDDisplayedTime = GetTime_ms();
}

void ShowOSDLOCK(void)
{
	//	BYTE CODE *Str ;
	//	BYTE len;
	//	BYTE x;
	if (GetItemValue(OsdLockChoice) == _TRUE)
	{

		SetOsdSize(_Content, MENU_WIDTH, 1);
		SetOsdStartAddress(_Content, MENU_START);
		SetOsdPosition(_Content, MENU_OSD_X_ADDR + 190 - 110, MENU_OSD_Y_ADDR + 250);
		// SetOsdPosition(_Content,48,24+40);

		OsdDumpString(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_OSD_LOCK2);
		OsdDumpAttribute(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_OSD_LOCK2, OSDLOCK_COLOR);
	}
	else
	{
		SetOsdSize(_Content, MENU_WIDTH, 1);
		SetOsdStartAddress(_Content, MENU_START);
		SetOsdPosition(_Content, MENU_OSD_X_ADDR + 170 - 90, MENU_OSD_Y_ADDR + 250);
		// SetOsdPosition(_Content,48,24+40);

		OsdDumpString(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_OSD_UNLOCK);
		OsdDumpAttribute(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_OSD_UNLOCK, DEFAULT_COLOR);
	}

	ClearDisplayedOSD(MENU | TVVOL);

	/*
if(GetItemValue(CAMOSDONOFF)==_FALSE)
VXIS_fSetOsdOnOff(0 ,_ContentOn|_BottomOn);
else
VXIS_fSetOsdOnOff(0 ,_ContentOn |_TitleOn|_BottomOn);
*/

	VXIS_fSetOsdOnOff(0, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(1, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(2, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(3, _ContentOn | WINZOOMx2);
	SET_CROSS_LINE(OFF);

	MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);
	MCUTimerActiveTimerEvent(SEC(Channel_Osd_Time_Out), _USER_TIMER_EVENT_OSD_DISABLE);

	//	OSDDisplayedTime = GetTime_ms();
}

void ShowOSDVersion(void)
{
	BYTE DEC_Number[] = {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _A, _B, _C, _D, _E, _F};
	BYTE Str_OSD_VERSION[] = {19, _V, _E, _R, _S, _I, _O, _N, 0x47, _spc, _CID, _0, _DOT, _0, _0, _DOT, _spc, _spc, _spc, _spc};

	Str_OSD_VERSION[11] = DEC_Number[0x0f & (FWVER1)];
	Str_OSD_VERSION[13] = DEC_Number[0x0f & (FWVER2 >> 4)];
	Str_OSD_VERSION[14] = DEC_Number[0x0f & (FWVER2)];
	Str_OSD_VERSION[16] = DEC_Number[0x0f & (FWVER >> 12)];
	Str_OSD_VERSION[17] = DEC_Number[0x0f & (FWVER >> 8)];
	Str_OSD_VERSION[18] = DEC_Number[0x0f & (FWVER >> 4)];
	Str_OSD_VERSION[19] = DEC_Number[0x0f & FWVER];

	SetOsdSize(_Content, MENU_WIDTH, 1);
	SetOsdStartAddress(_Content, MENU_START);
	SetOsdPosition(_Content, MENU_OSD_X_ADDR /*+170*/, MENU_OSD_Y_ADDR + 250);

	OsdDumpString(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_OSD_VERSION);
	OsdDumpAttribute(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_OSD_VERSION, DEFAULT_COLOR);

	ClearDisplayedOSD(MENU | TVVOL);

	SetDisplayedOSD(_VERSION_OSD);

	VXIS_fSetOsdOnOff(0, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(1, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(2, _ContentOn | WINZOOMx2);
	VXIS_fSetOsdOnOff(3, _ContentOn | WINZOOMx2);
	SET_CROSS_LINE(OFF);

	MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);
}

void ShowGaugeOSD(void)
{
	BYTE DEC_Number[10] = {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9};
	BYTE Str_Number[] = {3, _spc, _spc, _spc};
	BYTE val = 0;
	BYTE offsetMENU = 0;
	BYTE CWidth[11] = {0, 9, 8, 7, 6, 0, 5, 4, 3, 2, 1};
	// BYTE CODE *Str ;
	BYTE x;
	// WORD pos[10]={0,0,8*,50,75,112,140,168,168,196};

	if (/*(GetItemValue(GAUGEChoice)==_TRUE) &&*/ (ReadEEP(EEP_RearMode) == 0) && (Flisheye_CAM[CH4] == FALSE))
	{

		for (x = 0; x < 8; x++)
		{
			OsdDumpString(MENU_START + (x * (GAUGE_WIDTH /*-(bytDistanceGaugeWidth-1)*/)), (tByte *)&Str_Menu_Clr);
			OsdDumpAttribute(MENU_START + (x * (GAUGE_WIDTH) /*-(bytDistanceGaugeWidth-1)*/), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
		}

		// if((GetDisplayedOSD()&_GAUGE_OSD)==0)
		{

			//			 SetOsdSize(_Content,GAUGE_WIDTH-(bytDistanceGaugeWidth-1),MENU_HEIGHT);
			SetOsdSize(_Content, (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth])), MENU_HEIGHT);
			SetOsdStartAddress(_Content, MENU_START);
		}

#if 1
#if (TH0701024600NYR50S1 == OFF)
		if (bytDistanceGaugePosition == 5)
			SetOsdPosition(_Content, ((PanelH - (FontWidthX2 * 18)) / 2) + ((CWidth[bytDistanceGaugeWidth]) * (16)), 24 + 40);
		else
		{
			SetOsdPosition(_Content, ((bytDistanceGaugePosition)*25) + ((CWidth[bytDistanceGaugeWidth]) * 16), 24 + 40);
		}
#else
		if (bytDistanceGaugePosition == 5)
			SetOsdPosition(_Content, ((PanelH - (FontWidthX2 * 18)) / 2) + ((CWidth[bytDistanceGaugeWidth]) * (16)) + 224, 24 + 40 + 80);
		else
		{
			SetOsdPosition(_Content, ((bytDistanceGaugePosition)*25) + ((CWidth[bytDistanceGaugeWidth]) * 16) + 224, 24 + 40 + 80);
		}

#endif
#else
		if (bytDistanceGaugePosition == 5)
			SetOsdPosition(_Content, ((PanelH - (FontWidthX2 * 18)) / 2) + ((bytDistanceGaugeWidth - 1) * (16)), 24 + 40);
		else
		{
			SetOsdPosition(_Content, ((bytDistanceGaugePosition)*25) + ((bytDistanceGaugeWidth - 1) * 16), 24 + 40);
		}

#endif
#if 0
	 	{

			 for(x=0;x<8;x++)
			 {	 
			  OsdDumpString(MENU_START+(x*(GAUGE_WIDTH-(bytDistanceGaugeWidth-1))),(tByte *)&Str_Menu_Clr);
			  OsdDumpAttribute(MENU_START+(x*(GAUGE_WIDTH)-(bytDistanceGaugeWidth-1)),(tByte *)&Str_Menu_Clr,DEFAULT_COLOR);
			 }
	 	}
#endif

		if (bytDistanceGaugeWidth == 1)
		{

#if 0
			for(x=0;x<8;x++)
			{
			OsdDumpString(MENU_START+(x*(GAUGE_WIDTH)),(tByte *)Str_GAUGE_OSD[x]);
				if((x==2)||(x==3)||(x==4))
				OsdDumpAttribute(MENU_START+(x*(GAUGE_WIDTH)),(tByte *)Str_GAUGE_OSD[x],CH_COLOR_YELLOW);
				else if((x==5)||(x==6)||(x==7))
				OsdDumpAttribute(MENU_START+(x*(GAUGE_WIDTH)),(tByte *)Str_GAUGE_OSD[x],CH_COLOR_RED);
				else
				OsdDumpAttribute(MENU_START+(x*(GAUGE_WIDTH)),(tByte *)Str_GAUGE_OSD[x],DEFAULT_COLOR);

				
			}
#else
			for (x = 0; x < 8; x++)
			{
				OsdDumpString(MENU_START + (x * (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth]))), (tByte *)(Str_GAUGE_Width[0][x]));

				if ((x == 2) || (x == 3) || (x == 4))
					OsdDumpAttribute(MENU_START + (x * (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth]))), (tByte *)(Str_GAUGE_Width[0][x]), CH_COLOR_YELLOW);
				else if ((x == 5) || (x == 6) || (x == 7))
					OsdDumpAttribute(MENU_START + (x * (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth]))), (tByte *)(Str_GAUGE_Width[0][x]), CH_COLOR_RED);
				else
					OsdDumpAttribute(MENU_START + (x * (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth]))), (tByte *)(Str_GAUGE_Width[0][x]), DEFAULT_COLOR);
			}
#endif
		}
		else

		{
			for (x = 0; x < 8; x++)
			{
				OsdDumpString(MENU_START + (x * (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth]))), (tByte *)(Str_GAUGE_Width[bytDistanceGaugeWidth - 2 + 1][x]));

				if ((x == 2) || (x == 3) || (x == 4))
					OsdDumpAttribute(MENU_START + (x * (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth]))), (tByte *)(Str_GAUGE_Width[bytDistanceGaugeWidth - 2 + 1][x]), CH_COLOR_YELLOW);
				else if ((x == 5) || (x == 6) || (x == 7))
					OsdDumpAttribute(MENU_START + (x * (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth]))), (tByte *)(Str_GAUGE_Width[bytDistanceGaugeWidth - 2 + 1][x]), CH_COLOR_RED);
				else
					OsdDumpAttribute(MENU_START + (x * (GAUGE_WIDTH - (CWidth[bytDistanceGaugeWidth]))), (tByte *)(Str_GAUGE_Width[bytDistanceGaugeWidth - 2 + 1][x]), DEFAULT_COLOR);
			}
		}

		/*
			 OsdDumpString(MENU_START+(0*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[0]);
			 OsdDumpAttribute(MENU_START+(0*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[0],DEFAULT_COLOR);
			 OsdDumpString(MENU_START+(1*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[1]);
			 OsdDumpAttribute(MENU_START+(1*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[1],DEFAULT_COLOR);

			OsdDumpString(MENU_START+(2*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[2]);
			OsdDumpAttribute(MENU_START+(2*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[2],CH_COLOR_YELLOW);
			OsdDumpString(MENU_START+(3*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[3]);
			OsdDumpAttribute(MENU_START+(3*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[3],CH_COLOR_YELLOW);
			OsdDumpString(MENU_START+(4*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[4]);
			OsdDumpAttribute(MENU_START+(4*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[4],CH_COLOR_YELLOW);

			OsdDumpString(MENU_START+(5*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[5]);
			OsdDumpAttribute(MENU_START+(5*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[5],CH_COLOR_RED);
			OsdDumpString(MENU_START+(6*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[6]);
			OsdDumpAttribute(MENU_START+(6*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[6],CH_COLOR_RED);
			OsdDumpString(MENU_START+(7*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[7]);
			OsdDumpAttribute(MENU_START+(7*MENU_WIDTH),(tByte *)Str_GAUGE_OSD[7],CH_COLOR_RED);
	*/

		ClearDisplayedOSD(MENU | TVVOL);
		SetDisplayedOSD(_GAUGE_OSD);

		//	if((bytGaugeAdjust==DistanceGaugePosition)&&(TriggerFlag==0))
		//	 DisplayOSDMenuOneItem(MenuChild,0,1,0);
		//	else if((bytGaugeAdjust==DistanceGaugeWidth)&&(TriggerFlag==0))
		//	DisplayOSDMenuOneItem(MenuChild,0,2,0);
#if 1
		// BYTE *ptr;

		if (bytGaugeAdjust)
		{
			if (bytGaugeAdjust == DistanceGaugePosition)
				val = bytDistanceGaugePosition;
			else if (bytGaugeAdjust == DistanceGaugeWidth)
				val = bytDistanceGaugeWidth;

			if (val / 100)
			{
				Str_Number[1] = DEC_Number[(val / 100)];
				val = val % 100;
				Str_Number[2] = DEC_Number[(val / 10)];
				val = val % 10;
				Str_Number[3] = DEC_Number[(val)];
			}
			else if (val / 10)
			{
				Str_Number[2] = DEC_Number[(val / 10)];
				val = val % 10;
				Str_Number[3] = DEC_Number[(val)];
			}
			else
				Str_Number[3] = DEC_Number[(val)];

			OsdDumpAttribute(MENU_START + (0 * (MENU_WIDTH - offsetMENU)), (tByte *)Str_Number, OSDLOCK_COLOR);
			OsdDumpString(OSDMENU_MAINADDR, (tByte *)Str_Number);
		}

#endif

		if (bytGaugeAdjust)
			VXIS_fSetOsdOnOff(0, _ContentOn /* |_TitleOn|_BottomOn*/ | WINZOOMx2);
		else
			VXIS_fSetOsdOnOff(0, _ContentOn | _TitleOn /*|_BottomOn*/ | WINZOOMx2);
	}
}

void SetGarbberLinerMode(BYTE index)
{
	switch (index)
	{
	case VS4210_LOOP_MODE0_w0:
	case VS4210_LOOP_MODE0_w1:
	case VS4210_LOOP_MODE0_w3:
	case VS4210_LOOP_MODE0_w4:

	case VS4210_LOOP_MODE2:
	case VS4210_LOOP_MODE8:
	case VS4210_LOOP_MODE9:
	case VS4210_LOOP_MODE12_12:
	case VS4210_LOOP_MODE12_14:
	case VS4210_LOOP_MODE12_42:
	case VS4210_LOOP_MODE12_21:
	case VS4210_LOOP_MODE12_24:
	case VS4210_LOOP_MODE8_412:
	case VS4210_LOOP_MODE8_124:
	case VS4210_LOOP_MODE8_214:
	case VS4210_LOOP_MODE10_412:
	case VS4210_LOOP_MODE14:
	case VS4210_LOOP_MODE9_412:
	case VS4210_LOOP_MODE9_124:
	case VS4210_LOOP_MODE9_214:
	case VS4210_LOOP_MODE10_124:
	case VS4210_LOOP_MODE10_214:
		bytGarbberLinerMode = VS4210_LOOP_MODE_ERROR;
		VXIS_fSetOsdOnOff(0, _TitleOn | _BottomOn | WINZOOMx1);
		break;

	case VS4210_LOOP_MODE0_w2:
	case VS4210_LOOP_MODE12_34:
	case VS4210_LOOP_MODE12_31:
	case VS4210_LOOP_MODE12_32:
	case VS4210_LOOP_MODE12_43:
	case VS4210_LOOP_MODE12_13:
	case VS4210_LOOP_MODE8_123:
	case VS4210_LOOP_MODE9_123:
	case VS4210_LOOP_MODE10_123:
	case VS4210_LOOP_MODE8_134:
	case VS4210_LOOP_MODE9_134:
	case VS4210_LOOP_MODE10_134:

	case VS4210_LOOP_MODE12_23:
	case VS4210_LOOP_MODE8_213:
	case VS4210_LOOP_MODE8_312:
	case VS4210_LOOP_MODE8_314:
	case VS4210_LOOP_MODE8_324:
	case VS4210_LOOP_MODE8_234:
	case VS4210_LOOP_MODE10_413:
	case VS4210_LOOP_MODE10_423:
	case VS4210_LOOP_MODE8_413:
	case VS4210_LOOP_MODE8_423:
	case VS4210_LOOP_MODE9_312:
	case VS4210_LOOP_MODE9_314:
	case VS4210_LOOP_MODE9_324:
	case VS4210_LOOP_MODE9_213:
	case VS4210_LOOP_MODE9_234:
	case VS4210_LOOP_MODE10_213:
	case VS4210_LOOP_MODE10_234:
	case VS4210_LOOP_MODE10_312:
	case VS4210_LOOP_MODE10_314:
	case VS4210_LOOP_MODE10_324:
	case VS4210_LOOP_MODE1:
		bytGarbberLinerMode = index;
		break;
	}

	if ((bytGarbberLinerMode != VS4210_LOOP_MODE_ERROR) && (bytGarbberLiner == ON))
	{
		ShowGrabberLinerOSD();
	}
}

void ShowGrabberLinerOSD(void)
{

	//	BYTE DEC_Number[10]={_0,_1,_2,_3,_4,_5,_6,_7,_8,_9};
	BYTE OSD_Width[7] = {13, 11, 9, 7, 5, 3, 0};
	//	BYTE Str_Number[]={3,_spc,_spc,_spc};
	//	BYTE val=0;
	//	BYTE offsetMENU=0;
	BYTE x;

	// if(/*(GetItemValue(GAUGEChoice)==_TRUE) &&*/(ReadEEP(EEP_RearMode)==0)&&(Flisheye_CAM[CH4]==FALSE))
	// if((GetDisplayedOSD()&_GRABBER_LINER_OSD)==0)
	{
#if (_DEBUG_OSD == ON)
		GraphicsPrint(CYAN, "(ShowGrabberLinerOSD..)\r\n");
#endif
		if (bytGarbberLinerMode == VS4210_LOOP_MODE0_w2)
			bytSetGarbberLinerPosition = GetOSDStep(Grabber_Liner_Position, bytGarbberLinerPosition) - ((((bytGarbberLinerWidth * 2) + 5) * FontWidthSIZE) / 2);
		else
			bytSetGarbberLinerPosition = GetOSDStep(Grabber_Liner_Position, bytGarbberLinerPosition) - ((((bytGarbberLinerWidth * 2) + 5) * FontWidthSIZE2) / 2);

		// Printf("(#WER1=%d)",(WORD)((((val*2)+5)*FontWidthSIZE)+pos));
		// if(bytGrabberAdjust==Grabber_Liner_Width)
		//	{
		for (x = 0; x < 8; x++)
		{
			OsdDumpString(MENU_START + (x * (GRAABER_WIDTH - (OSD_Width[bytGarbberLinerWidth - 1]))), (tByte *)&Str_Menu_Clr);
			OsdDumpAttribute(MENU_START + (x * (GRAABER_WIDTH) - (OSD_Width[bytGarbberLinerWidth - 1])), (tByte *)&Str_Menu_Clr, CH_COLOR_WHITE /*DEFAULT_COLOR*/);
		}
		//}
		SetOsdSize(_Content, GRAABER_WIDTH - (OSD_Width[bytGarbberLinerWidth - 1]), MENU_HEIGHT);
		SetOsdStartAddress(_Content, MENU_START);

		//			SetOsdPosition(_Content,((((bytGarbberLinerWidth*2)+5)*FontWidthSIZE)/2)+bytSetGarbberLinerPosition,24+40);

		switch (bytGarbberLinerMode)
		{

		case VS4210_LOOP_MODE0_w2:
#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Content, bytSetGarbberLinerPosition, 24 + 40); // sizeX2 center	and size left up
#else
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 224, 24 + 40 + 80);						// sizeX2 center	and size left up
#endif
			break;
		case VS4210_LOOP_MODE9_134:
		case VS4210_LOOP_MODE9_312:
		case VS4210_LOOP_MODE9_314:
		case VS4210_LOOP_MODE9_324:
#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 24, 24 + 40); // sizeX2 center	and size left up
#else
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 24 + 168, 24 + 40 + 40);					// sizeX2 center	and size left up
#endif
			break;

		case VS4210_LOOP_MODE12_34:
		case VS4210_LOOP_MODE12_31:
		case VS4210_LOOP_MODE12_32:
		case VS4210_LOOP_MODE8_312:
		case VS4210_LOOP_MODE8_314:
		case VS4210_LOOP_MODE8_324:
#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 24, 24 + 40 + 120); // size1 left center
#else
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 24 + 168, 24 + 40 + 120);					// size1 left center
#endif
			break;

		case VS4210_LOOP_MODE12_43:
		case VS4210_LOOP_MODE12_13:
		case VS4210_LOOP_MODE12_23:
		case VS4210_LOOP_MODE9_213:

#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 400 + 24 + 4, 24 + 40 + 120); // size1 right center
#else
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 400 + 24 + 4 + 280, 24 + 40 + 120);		// size1 right center
#endif
			break;

		case VS4210_LOOP_MODE8_134:
		case VS4210_LOOP_MODE8_234:
		case VS4210_LOOP_MODE9_234:
#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 400 + 24, 24 + 40); // size1  right up
#else
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 400 + 24 + 280, 24 + 40 + 40);			// size1  right up
#endif
			break;

		case VS4210_LOOP_MODE8_123:
		case VS4210_LOOP_MODE10_123:
		case VS4210_LOOP_MODE8_213:
		case VS4210_LOOP_MODE10_413:
		case VS4210_LOOP_MODE10_423:
		case VS4210_LOOP_MODE8_413:
		case VS4210_LOOP_MODE8_423:
		case VS4210_LOOP_MODE10_213:
#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 400 + 24 + 4, 24 + 40 + 240); // size1 right down
#else
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 400 + 24 + 4 + 280, 24 + 40 + 240 + 100); // size1 right down
#endif
			break;

		case VS4210_LOOP_MODE9_123:
		case VS4210_LOOP_MODE10_134:
		case VS4210_LOOP_MODE10_234:
		case VS4210_LOOP_MODE1:
#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 24, 24 + 40 + 240); // size1 left down
#else
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 24 + 168, 24 + 40 + 240 + 100);			// size1 left down
#endif
			break;

		case VS4210_LOOP_MODE10_312:
		case VS4210_LOOP_MODE10_314:
		case VS4210_LOOP_MODE10_324:
#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 200 + 24, 24 + 40); // size1 up center
#else
			SetOsdPosition(_Content, bytSetGarbberLinerPosition + 200 + 24 + 224, 24 + 40 + 40);			// size1 up center
#endif
			break;
		}

#if (_DEBUG_OSD == ON)
		GraphicsPrint(CYAN, "(bytGarbberLinerWidth=%d bytGarbberLinerPosition=%d)\r\n", (WORD)bytGarbberLinerWidth, (WORD) /*((((bytGarbberLinerWidth*2)+5)*FontWidthSIZE)/2)*/ + bytSetGarbberLinerPosition);
#endif

		for (x = 0; x < 8; x++)
		{
			OsdDumpString(MENU_START + (x * (GRAABER_WIDTH - (OSD_Width[bytGarbberLinerWidth - 1]))), (tByte *)(Str_GRABBER_OSD[bytGarbberLinerWidth - 1][x]));

			if (bytGrabberAdjust)
				OsdDumpAttribute(MENU_START + (x * (GRAABER_WIDTH - (OSD_Width[bytGarbberLinerWidth - 1]))), (tByte *)(Str_GRABBER_OSD[bytGarbberLinerWidth - 1][x]), CH_COLOR_YELLOW);
			else
				OsdDumpAttribute(MENU_START + (x * (GRAABER_WIDTH - (OSD_Width[bytGarbberLinerWidth - 1]))), (tByte *)(Str_GRABBER_OSD[bytGarbberLinerWidth - 1][x]), CH_COLOR_RED);

			if ((bytGrabberAdjust == Grabber_Liner_Width) || (bytGrabberAdjust == 0))
				OsdDumpAttributeHS(MENU_START + (x * (GRAABER_WIDTH - (OSD_Width[bytGarbberLinerWidth - 1]))) + (bytGarbberLinerWidth + 2), (tByte *)(Str_GRABBER_OSD[bytGarbberLinerWidth - 1][x]), DEFAULT_COLOR, 1);
		}

		ClearDisplayedOSD(MENU | TVVOL | _COC_OSD | _GAUGE_OSD);

		// if(bytGrabberAdjust)
		// VXIS_fSetOsdOnOff(0 ,_ContentOn |_TitleOn/*|_BottomOn*/|WINZOOMx2);
		// else
		//	{
		if (bytGarbberLinerMode == VS4210_LOOP_MODE0_w2)
			VXIS_fSetOsdOnOff(0, _ContentOn | _TitleOn /*|_BottomOn*/ | WINZOOMx2);
		else
		{
			VXIS_fSetOsdOnOff(0, _ContentOn | _TitleOn | _BottomOn | WINZOOMx1);
		}

		if (bytGrabberAdjust)
		{
			// Printf("(*GrabberMessage)");
			SetOsdSize(_Bar0, GrabberMessageWidth, JUMP_HEIGHT);
			SetOsdStartAddress(_Bar0, 24);
			OsdDumpString(24, (tByte *)GrabberMessage);
			OsdDumpAttribute(24, (tByte *)GrabberMessage, CH_COLOR_RED);

#if (TH0701024600NYR50S1 == OFF)
			SetOsdPosition(_Bar0, 48 + 40, 400);
#else
			SetOsdPosition(_Bar0, 48 + 40 + 224, 400 + (200 - 20));
#endif

			VXIS_fSetOsdOnOff(0, _ContentOn | _TitleOn | _BottomOn | WINZOOMx2);
		}
		//	}
		SetDisplayedOSD(_GRABBER_LINER_OSD);
	}
}
void CLRGrabberMessage(void)
{
	Printf("(*CLRGrabberMessage)");
	// SetOsdSize(_Bar0,GrabberMessageWidth,JUMP_HEIGHT);
	//    SetOsdStartAddress(_Bar0,24);
	OsdDumpString(24, (tByte *)GrabberMessageCLR);
	// OsdDumpAttribute(24,(tByte *)GrabberMessageCLR,CH_COLOR_TRANS);
	// SetOsdPosition(_Bar0,48,400);
	// VXIS_fSetOsdOnOff(0 ,_ContentOn |_TitleOn|_BottomOn|WINZOOMx2);
}
void ShowCOCOSD2(void)
{

	//	BYTE CODE *Str ;
	//	BYTE len;
	BYTE x;
	// if((GetDisplayedOSD()&_JUMP_OSD)==0 )
	{
		Printf("\r\n(*ShowCOCOSD2)");

		SetOsdSize(_Content, COC_WIDTH, COC_HEIGHT);
		SetOsdStartAddress(_Content, MENU_START);

#if (TH0701024600NYR50S1 == OFF)
		SetOsdPosition(_Content, 48 + 20, 460);
#else
		SetOsdPosition(_Content, 48 + 20 + 112, 460 + 100);
#endif

		for (x = 0; x < 2; x++)
		{
			OsdDumpString(MENU_START + (x * COC_WIDTH), (tByte *)Str_OSD_COC_CLR);
			OsdDumpAttribute(MENU_START + (x * COC_WIDTH), (tByte *)Str_OSD_COC_CLR, DEFAULT_COLOR);
		}

		OsdDumpString(MENU_START, (tByte *)Str_OSD_COC);
		OsdDumpAttribute(MENU_START, (tByte *)Str_OSD_COC, OSDCOC_COLOR);

		OsdDumpString(MENU_START + COC_WIDTH, (tByte *)Str_OSD_COC2);
		OsdDumpAttribute(MENU_START + COC_WIDTH, (tByte *)Str_OSD_COC2, OSDCOC_COLOR);

		ClearDisplayedOSD((MENU | TVVOL | _GAUGE_OSD | _RUN_TPMS_OSD | _GRABBER_LINER_OSD));

		if (GetItemValue(CAMOSDONOFF) == _FALSE)
			VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn);
		else
			//	VXIS_fSetOsdOnOff(0 ,_ContentOn |_TitleOn|_BottomOn|WINZOOMx2);
			VXIS_fSetOsdOnOff(0, _ContentOn | _TitleOn | _BottomOn);
	}

	SetDisplayedOSD(_COC_OSD);
	bytCOCAdjust = ON;
}

void ShowTPMSOSD(void)
{
	// BYTE CODE *Str ;
	// BYTE len;
	BYTE x, val;
	BYTE DEC_Number[10] = {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9};
	BYTE Str_Number[] = {5, _spc, _spc, _DOT, _spc, _V};

	VXIS_fSetOsdOnOff(0, 0);
	VXIS_fSetOsdOnOff(1, 0);
	VXIS_fSetOsdOnOff(2, 0);
	VXIS_fSetOsdOnOff(3, 0);

	SET_CROSS_LINE(OFF);

	Printf("\r\n(ShowTPMSOSD)");

	// if((GetItemValue(GAUGEChoice)==_TRUE) &&(ReadEEP(EEP_RearMode)==0)&&(Flisheye_CAM[CH4]==FALSE))
	{

		SetOsdSize(_Content, MENU_WIDTH, MENU_HEIGHT);
		SetOsdStartAddress(_Content, MENU_START);
		SetOsdPosition(_Content, 48 + 48 + 20 + 16, 24 + 40);

		for (x = 0; x < 8; x++)
		{
			//			  OsdDumpString(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr[x][0]);
			//			  OsdDumpAttribute(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr[x][0],DEFAULT_COLOR);
			OsdDumpString(MENU_START + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr);
			OsdDumpAttribute(MENU_START + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
		}

		OsdDumpString(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_TPMS_OSD0);
		OsdDumpAttribute(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_TPMS_OSD0, DEFAULT_COLOR | BG_COLOR_Gray);
		OsdDumpAttribute(MENU_START + (0 * MENU_WIDTH), (tByte *)Str_TPMS_NUM0, CH_COLOR_WHITE | BG_COLOR_Gray);

		OsdDumpString(MENU_START + (1 * MENU_WIDTH), (tByte *)Str_TPMS_OSD1);
		OsdDumpAttribute(MENU_START + (1 * MENU_WIDTH), (tByte *)Str_TPMS_OSD1, DEFAULT_COLOR | BG_COLOR_Gray);
		OsdDumpAttribute(MENU_START + (1 * MENU_WIDTH), (tByte *)Str_TPMS_NUM1, CH_COLOR_WHITE | BG_COLOR_Gray);

		OsdDumpString(MENU_START + (2 * MENU_WIDTH), (tByte *)Str_TPMS_OSD2);
		OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH), (tByte *)Str_TPMS_OSD2, DEFAULT_COLOR | BG_COLOR_Gray);
		OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH), (tByte *)Str_TPMS_NUM2, CH_COLOR_WHITE | BG_COLOR_Gray);

		OsdDumpString(MENU_START + (3 * MENU_WIDTH), (tByte *)Str_TPMS_OSD3);
		OsdDumpAttribute(MENU_START + (3 * MENU_WIDTH), (tByte *)Str_TPMS_OSD3, DEFAULT_COLOR | BG_COLOR_Gray);
		OsdDumpAttribute(MENU_START + (3 * MENU_WIDTH), (tByte *)Str_TPMS_NUM3, CH_COLOR_WHITE | BG_COLOR_Gray);

		OsdDumpString(MENU_START + (4 * MENU_WIDTH), (tByte *)Str_TPMS_OSD4);
		OsdDumpAttribute(MENU_START + (4 * MENU_WIDTH), (tByte *)Str_TPMS_OSD4, DEFAULT_COLOR | BG_COLOR_Gray);
		OsdDumpAttribute(MENU_START + (4 * MENU_WIDTH), (tByte *)Str_TPMS_NUM4, CH_COLOR_WHITE | BG_COLOR_Gray);

		OsdDumpString(MENU_START + (5 * MENU_WIDTH), (tByte *)Str_TPMS_OSD5);
		OsdDumpAttribute(MENU_START + (5 * MENU_WIDTH), (tByte *)Str_TPMS_OSD5, DEFAULT_COLOR | BG_COLOR_Gray);
		OsdDumpAttribute(MENU_START + (5 * MENU_WIDTH), (tByte *)Str_TPMS_NUM5, CH_COLOR_WHITE | BG_COLOR_Gray);

		OsdDumpString(MENU_START + (6 * MENU_WIDTH), (tByte *)Str_TPMS_OSD6);
		OsdDumpAttribute(MENU_START + (6 * MENU_WIDTH), (tByte *)Str_TPMS_OSD6, DEFAULT_COLOR | BG_COLOR_Gray);

		OsdDumpString(MENU_START + (7 * MENU_WIDTH), (tByte *)Str_TPMS_OSD7);
		OsdDumpAttribute(MENU_START + (7 * MENU_WIDTH), (tByte *)Str_TPMS_OSD7, CH_COLOR_AQUA | BG_COLOR_Gray);

		val = (BAT_StartupVOLT / 10);
		if (val / 100)
		{
			Str_Number[1] = DEC_Number[(val / 100)];
			val = val % 100;
			Str_Number[2] = DEC_Number[(val / 10)];
			val = val % 10;
			Str_Number[4] = DEC_Number[(val)];
		}
		else if (val / 10)
		{
			Str_Number[2] = DEC_Number[(val / 10)];
			val = val % 10;
			Str_Number[4] = DEC_Number[(val)];
		}
		else
			Str_Number[4] = DEC_Number[(val)];

		OsdDumpString(MENU_START + (7 * MENU_WIDTH) + 4, (tByte *)Str_Number);
		if ((BAT_StartupVOLT / 10) < 100)
			OsdDumpAttribute(MENU_START + (7 * MENU_WIDTH) + 4, (tByte *)Str_Number, CH_COLOR_RED | BG_COLOR_Gray);
		else
			OsdDumpAttribute(MENU_START + (7 * MENU_WIDTH) + 4, (tByte *)Str_Number, DEFAULT_COLOR | BG_COLOR_Gray);

		val = (BAT_VOLTAGE / 10);
		if (val / 100)
		{
			Str_Number[1] = DEC_Number[(val / 100)];
			val = val % 100;
			Str_Number[2] = DEC_Number[(val / 10)];
			val = val % 10;
			Str_Number[4] = DEC_Number[(val)];
		}
		else if (val / 10)
		{
			Str_Number[2] = DEC_Number[(val / 10)];
			val = val % 10;
			Str_Number[4] = DEC_Number[(val)];
		}
		else
			Str_Number[4] = DEC_Number[(val)];

		OsdDumpString(MENU_START + (7 * MENU_WIDTH) + 6 + 4 + 1, (tByte *)Str_Number);

		if ((BAT_VOLTAGE / 10) < 100)
			OsdDumpAttribute(MENU_START + (7 * MENU_WIDTH) + 6 + 4 + 1, (tByte *)Str_Number, CH_COLOR_RED | BG_COLOR_Gray);
		else
			OsdDumpAttribute(MENU_START + (7 * MENU_WIDTH) + 6 + 4 + 1, (tByte *)Str_Number, DEFAULT_COLOR | BG_COLOR_Gray);

		ClearDisplayedOSD(MENU | TVVOL);
		SetDisplayedOSD(_RUN_TPMS_OSD);

		// if(GetItemValue(CAMOSDONOFF)==_FALSE)
		// VXIS_fSetOsdOnOff(0 ,_ContentOn|_BottomOn);
		// else
		//	VXIS_fSetOsdOnOff(0 ,_ContentOn |WINZOOMx2);
		VXIS_fSetOsdOnOff(0, _ContentOn /* |_TitleOn|_BottomOn*/ | WINZOOMx2);
	}
}

void ShowJUMPOSD(void)
{

	//	BYTE CODE *Str ;
	//	BYTE len;
	//	BYTE x;
	// if((GetDisplayedOSD()&_JUMP_OSD)==0 )

	if ((NowModeState == JumpMode) && ((bytGarbberLiner == OFF) || (bytGarbberLinerMode == VS4210_LOOP_MODE_ERROR)))
	{
#if (_DEBUG_MESSAGE_Source_Change == ON)
		Printf("\r\n(*ShowJUMPOSD)");
#endif

		SetOsdSize(_Content, JUMP_WIDTH, JUMP_HEIGHT);
		SetOsdStartAddress(_Content, MENU_START);
#if (TH0701024600NYR50S1 == ON)
		SetOsdPosition(_Content, 48 + 112, 24 + 3);
#else
		SetOsdPosition(_Content, 48, 24 + 3);
#endif

		/*
		 for(x=0;x<8;x++)
		 {
		  //OsdDumpString(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr[x][0]);
		  //OsdDumpAttribute(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr[x][0],DEFAULT_COLOR);
		  OsdDumpString(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr);
		  OsdDumpAttribute(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr,DEFAULT_COLOR);
		 }
		 */
		if (SEQFlag == _TRUE)
		{
			OsdDumpString(MENU_START, (tByte *)Str_OSD_SEQ);
			OsdDumpAttribute(MENU_START, (tByte *)Str_OSD_SEQ, DEFAULT_COLOR);
		}
		else
		{
			OsdDumpString(MENU_START, (tByte *)Str_OSD_JUMP);
			OsdDumpAttribute(MENU_START, (tByte *)Str_OSD_JUMP, DEFAULT_COLOR);
		}

		ClearDisplayedOSD(MENU | TVVOL);

		if (GetItemValue(CAMOSDONOFF) == _FALSE)
			VXIS_fSetOsdOnOff(0, _ContentOn | _BottomOn);
		else
			VXIS_fSetOsdOnOff(0, _ContentOn | _TitleOn | _BottomOn);

		SetDisplayedOSD(_JUMP_OSD);
	}
	else if (NowModeState == JumpMode)
	{
#if (_DEBUG_MESSAGE_Source_Change == ON)
		Printf("(*2ShowJUMPOSD)");
#endif

		SetOsdSize(_Bar2, JUMP_WIDTH, JUMP_HEIGHT);
		SetOsdStartAddress(_Bar2, 44);
		OsdDumpString(44, (tByte *)Str_OSD_JUMP);
		OsdDumpAttribute(44, (tByte *)Str_OSD_JUMP, DEFAULT_COLOR);
		SetOsdPosition(_Bar2, 48, 27);
		//   Printf("(#CC2)");
#if 1
		if ((TriggerFlag == 0) && (bSignal[0] == 0))
		{
			SetOsdSize(_Bar0, OSD_No_Signal_param[0], 1);
			SetOsdStartAddress(_Bar0, 24);
			OsdDumpString(24, OSD_No_Signal_param);
			OsdDumpAttribute(24, OSD_No_Signal_param, _Color);
			//       	OsdDumpAttribute(24,Str2,BG_COLOR_BLACK|CH_COLOR_YELLOW);//test
			SetOsdPosition(_Bar0, 48 + 125, 24 + 75 + 20);
		}
#endif

		/*
						SetOsdSize(_Bar2,JUMP_WIDTH,JUMP_HEIGHT);
					   SetOsdStartAddress(_Bar0,44);
					   OsdDumpString(44,(tByte *)Str_OSD_JUMP);
					   OsdDumpAttribute(44,(tByte *)Str_OSD_JUMP,DEFAULT_COLOR);
					   SetOsdPosition(_Bar2,48,27);
		*/

		/*
						SetOsdSize(_Bar1,Str2[0],1);
					SetOsdStartAddress(_Bar1,34);
					OsdDumpString(34,Str2);
					OsdDumpAttribute(34,Str2,_Color);
					SetOsdPosition(_Bar1,sx,sy + 20);
					   Printf("(#CC1)");
					   */
		// VXIS_fSetOsdOnOff(0 ,_ContentOn |_TitleOn|_BottomOn);
		// if(GetItemValue(CAMOSDONOFF)==_FALSE)
		//		VXIS_fSetOsdOnOff(0 ,_ContentOn|_BottomOn);
		//	else
	}

	// SetGarbberLinerMode(bytGarbberLinerMode);
}

void ShowTRIGOSD(void)
{
	if (TriggerFlag != 0)
	{
		if (TriggerStatus == 1)
		{
			OsdDumpString(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_LEFT);
			OsdDumpAttribute(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_LEFT, CH_COLOR_RED);
		}
		else if (TriggerStatus == 2)
		{
			OsdDumpString(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_RIGHT);
			OsdDumpAttribute(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_RIGHT, CH_COLOR_RED);
		}
		else if ((TriggerStatus == 3) /*||(TriggerStatus==5)*/)
		{
			OsdDumpString(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_CAMC);
			OsdDumpAttribute(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_CAMC, CH_COLOR_RED);
		}
		else if (TriggerStatus == 4)
		{
			OsdDumpString(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_CAMR);
			OsdDumpAttribute(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_CAMR, CH_COLOR_RED);
		}
		else if (TriggerStatus == 5)
		{
			OsdDumpString(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_CAMD);
			OsdDumpAttribute(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_CAMD, CH_COLOR_RED);
		}

#if (_DEBUG_MESSAGE_BOOT_NOISE == ON)
		Printf("(*ShowTRIGOSD=%02x)", (WORD)TriggerStatus);
#endif
	}
	else
	{

		OsdDumpString(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_CLR);
		OsdDumpAttribute(TRIGGER_OSD_START, (tByte *)Str_OSD_TRI_CLR, CH_COLOR_RED);

#if (_DEBUG_MESSAGE_BOOT_NOISE == ON)
		Printf("(*ShowTRIGOSD=CLR)");
#endif
	}
}

void ShowCOCOSD(void)
{
	// BYTE CODE *Str ;
	// BYTE len;
	BYTE x;

	VXIS_fSetOsdOnOff(0, 0);
	VXIS_fSetOsdOnOff(1, 0);
	VXIS_fSetOsdOnOff(2, 0);
	VXIS_fSetOsdOnOff(3, 0);

	SET_CROSS_LINE(OFF);

	// if((GetItemValue(GAUGEChoice)==_TRUE) &&(ReadEEP(EEP_RearMode)==0))
	{

		//	 SetOsdSize(_Content,COCOSD_WIDTH,COCOSD_HEIGHT);
		//	 SetOsdStartAddress(_Content,MENU_START);

		SetOsdSize(_Content, COCOSD_WIDTH, COCOSD_HEIGHT);
		SetOsdStartAddress(_Content, COCOSD_START);

		SetOsdPosition(_Content, 700 + 32, 24 + 40);
		/*
					 for(x=0;x<8;x++)
					 {
					  OsdDumpString(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr);
					  OsdDumpAttribute(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr,DEFAULT_COLOR);
					 }
			*/

		for (x = 0; x < 10; x++)
		{

			OsdDumpString(COCOSD_START + (x * COCOSD_WIDTH), (tByte *)Str_COC_OSD[x]);
			OsdDumpAttribute(COCOSD_START + (x * COCOSD_WIDTH), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
		}
		/*
			 OsdDumpString(COCOSD_START+(0*COCOSD_WIDTH),Str_COC_OSD[0]);
			 OsdDumpAttribute(COCOSD_START+(0*COCOSD_WIDTH),Str_COC_OSD[0],DEFAULT_COLOR);
			 OsdDumpString(COCOSD_START+(1*COCOSD_WIDTH),Str_COC_OSD[1]);
			 OsdDumpAttribute(COCOSD_START+(1*COCOSD_WIDTH),Str_COC_OSD[1],DEFAULT_COLOR);

			 OsdDumpString(COCOSD_START+(2*COCOSD_WIDTH),Str_COC_OSD[2]);
			 OsdDumpAttribute(COCOSD_START+(2*COCOSD_WIDTH),Str_COC_OSD[2],DEFAULT_COLOR);
			 OsdDumpString(COCOSD_START+(3*COCOSD_WIDTH),Str_COC_OSD[3]);
			 OsdDumpAttribute(COCOSD_START+(3*COCOSD_WIDTH),Str_COC_OSD[3],DEFAULT_COLOR);

			OsdDumpString(COCOSD_START+(4*COCOSD_WIDTH),Str_COC_OSD[4]);
			OsdDumpAttribute(COCOSD_START+(4*COCOSD_WIDTH),Str_COC_OSD[4],DEFAULT_COLOR);
			OsdDumpString(COCOSD_START+(5*COCOSD_WIDTH),Str_COC_OSD[5]);
			OsdDumpAttribute(COCOSD_START+(5*COCOSD_WIDTH),Str_COC_OSD[5],DEFAULT_COLOR);

			OsdDumpString(COCOSD_START+(6*COCOSD_WIDTH),Str_COC_OSD[6]);
			OsdDumpAttribute(COCOSD_START+(6*COCOSD_WIDTH),Str_COC_OSD[6],DEFAULT_COLOR);
			OsdDumpString(COCOSD_START+(7*COCOSD_WIDTH),Str_COC_OSD[7]);
			OsdDumpAttribute(COCOSD_START+(7*COCOSD_WIDTH),Str_COC_OSD[7],DEFAULT_COLOR);

			OsdDumpString(COCOSD_START+(8*COCOSD_WIDTH),Str_COC_OSD[8]);
			OsdDumpAttribute(COCOSD_START+(8*COCOSD_WIDTH),Str_COC_OSD[8],DEFAULT_COLOR);
			OsdDumpString(COCOSD_START+(9*COCOSD_WIDTH),Str_COC_OSD[9]);
			OsdDumpAttribute(COCOSD_START+(9*COCOSD_WIDTH),Str_COC_OSD[9],DEFAULT_COLOR);
	*/
		ClearDisplayedOSD(MENU | TVVOL);
		SetDisplayedOSD(_COC_OSD);

		// if(GetItemValue(CAMOSDONOFF)==_FALSE)
		// VXIS_fSetOsdOnOff(0 ,_ContentOn|_BottomOn);
		// else
		//	VXIS_fSetOsdOnOff(0 ,_ContentOn |WINZOOMx2);
		//	VXIS_fSetOsdOnOff(0 ,_ContentOn|WINZOOMx2);
		VXIS_fSetContentOsdOnOff(_ContentOn);
	}
}

BYTE DoAction(BYTE id)
{
	BYTE ret = 1;
	BYTE fch[4] = {0, 2, 3, 1};

	Printf("\r\nDoAction=%02x", (WORD)id);

	switch (id)
	{

	case RESET_VIDEOVALUE:

		if ((CURRENT_CH == 0) && (CAMA_M == 1))
		{
			CAMA_M = 0;
			VS4210_ResetCamMirror(CH1);
		}
		CloseOSDMenu();
		ResetOSDValue();
		break;
	// case Grabber_Liner_Width:
	//	bytBrabberAdjust=Grabber_Liner_Width;
	//	Printf("\r\nGrabber_Liner_Width..");
	//	ShowGrabberLinerOSD();
	//	break;
	case COC_Action:
		//���隢���寡�剖��頦���剖��嚙� CAM C & CAM D COC���蝛����頦����
		if ((CURRENT_CH < 4) && (CURRENT_CH != VS4210_LOOP_MODE0_w4) && (CURRENT_CH != VS4210_LOOP_MODE0_w2))
		{
			if ((SelectModeType < 4) && (Flisheye_CAM[fch[SelectModeType]] == TRUE))
			{
				switch (SelectModeType)
				{
				case CH1:
					if (CAMA_M)
					{
						Flisheye_COC = SelectModeType;
						CAMA_M = 0;
						VS4210_ResetCamMirror(CH1);
						SetNowMode();
					}
					break;

				case CH2:
					if (CAMB_M)
					{
						Flisheye_COC = SelectModeType;
						CAMB_M = 0;
						VS4210_ResetCamMirror(CH2);
						SetNowMode();
					}
					break;

				case CH3:
					if (CAMC_M)
					{
						Flisheye_COC = SelectModeType;
						CAMC_M = 0;
						VS4210_ResetCamMirror(CH3);
						SetNowMode();
					}
					break;

				case CH4:
					if (CAMR_M)
					{
						Flisheye_COC = SelectModeType;
						CAMR_M = 0;
						VS4210_ResetCamMirror(CH4);
						SetNowMode();
					}
					break;
				}

				// SetCOCCom(_COCOSD_BOX_Sel ,SelectModeType);
				// SetCOCCom(_COC_FLISHEYE_MODE1,Fisheye_no_save);
			}
			if ((CURRENT_CH == VS4210_LOOP_MODE0_w1) && (bSignal[2] == _TRUE))
			{
				Printf("\r\nCOC_Action B..");
				ShowCOCOSD2();
			}
			else if ((CURRENT_CH == VS4210_LOOP_MODE0_w0) && (bSignal[0] == _TRUE))
			{
				Printf("\r\nCOC_Action A..");
				ShowCOCOSD2();
			}
			else if ((CURRENT_CH == VS4210_LOOP_MODE0_w2) && (bSignal[3] == _TRUE))
			{
				Printf("\r\nCOC_Action C..");
				ShowCOCOSD2();
			}
			else if ((CURRENT_CH == VS4210_LOOP_MODE0_w3) && (bSignal[1] == _TRUE))
			{
				Printf("\r\nCOC_Action R..");
				ShowCOCOSD2();
			}
		}
		else
		{
			Printf("\r\nCOC_Error..");
		}
		break;
	}
	return ret;
}
WORD SetItemValue(BYTE id, BYTE flag)
{

	WORD val = 0;
	WORD pos;

	switch (id)
	{
	case VIDEO_CONTRAST:
		val = ReadEEP(EEP_CONTRAST);
		OSDItemValue = val;
		val = ChangeVideoData(VIDEO_CONTRAST, flag);
		WriteEEP(EEP_CONTRAST, val);
		break;
	case VIDEO_BRIGHTNESS:
		val = ReadEEP(EEP_BRIGHTNESS);
		OSDItemValue = val;
		val = ChangeVideoData(VIDEO_BRIGHTNESS, flag);
		WriteEEP(EEP_BRIGHTNESS, val);
		break;
	case VIDEO_SATURATION:
		val = ReadEEP(EEP_SATURATION_U);
		OSDItemValue = val;
		val = ChangeVideoData(VIDEO_SATURATION, flag);
		WriteEEP(EEP_SATURATION_U, val);
		break;
	case VIDEO_HUE:
		val = ReadEEP(EEP_HUE);
		OSDItemValue = val;
		val = ChangeVideoData(VIDEO_HUE, flag);
		WriteEEP(EEP_HUE, val);
		break;
	case VIDEO_SHARPNESS:
		val = ReadEEP(EEP_SHARPNESS);
		OSDItemValue = val;
		val = ChangeVideoData(VIDEO_SHARPNESS, flag);
		WriteEEP(EEP_SHARPNESS, val);
		break;
	case VIDEO_BACKLIGHT:
		val = ReadEEP(EEP_BACKLIGHT);
		OSDItemValue = val;
		val = ChangeVideoData(VIDEO_BACKLIGHT, flag);
		WriteEEP(EEP_BACKLIGHT, val);
		break;
	case CAMOSDONOFF:
		val = ReadEEP(EEP_COSDMode);
		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);
		WriteEEP(EEP_COSDMode, val);

		Disp_Input = val;
		//	if(val==0) SetNowMode();//ryan@20170922
		break;
	case AutoSleeping:
		val = ReadEEP(EEP_Auto_Sleeping);
		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);
		WriteEEP(EEP_Auto_Sleeping, val);
		bytAutoSleeping = val;

		if (bytAutoSleeping == ON)
		{
			MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_AUTO_SLEEPING_TIMEOUT);
			MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_AUTO_SLEEPING_TIMEOUT);
		}
		else
		{
			MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_AUTO_SLEEPING_TIMEOUT);
		}

		break;
	case TripleScreenType:
		val = ReadEEP(EEP_Triple_Screen_Type);
		if (flag == UP)
		{
			if (val < 3)
				val += 1;
			else
				val = 3;
		}
		else
		{
			if (val > 1)
				val -= 1;
			else
				val = 1;
		}

		WriteEEP(EEP_Triple_Screen_Type, val);
		bytTripleScreenType = val;
		break;
	case DistanceGaugePosition:
		val = ReadEEP(EEP_Distance_Gauge_Position);
		if (flag == UP)
		{
			if (val < 10)
				val += 1;
			else
				val = 10;
		}
		else
		{
			if (val > 1)
				val -= 1;
			else
				val = 1;
		}
		bytDistanceGaugePosition = val;
		WriteEEP(EEP_Distance_Gauge_Position, val);
		break;
	case DistanceGaugeWidth:
		val = ReadEEP(EEP_Distance_Gauge_Width);
		if (flag == UP)
		{
			if (val < 10)
				val += 1;
			else
				val = 10;
		}
		else
		{
			if (val > 1)
				val -= 1;
			else
				val = 1;
		}
		bytDistanceGaugeWidth = val;
		WriteEEP(EEP_Distance_Gauge_Width, val);
		break;
	case DayChoice:
		val = ReadEEP(EEP_AutoMode);

		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);

		DayNightLevel = val;
		WriteEEP(EEP_AutoMode, val);

		RUN_DIMMER = _DIMMER_STOP;
		break;
	case GAUGEChoice:
		val = ReadEEP(EEP_GAUGEMode);

		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);
		WriteEEP(EEP_GAUGEMode, val);
		break;
	case OsdLockChoice:
		val = ReadEEP(EEP_ONOFFChoice);

		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);
		WriteEEP(EEP_ONOFFChoice, val);

		OSD_LOCKF = val;
		break;
	case PRIORITYChoice:
		val = ReadEEP(EEP_PRIORITY);
		/*
				if(flag==UP)
					val = (!val ? 1 : 0);
				else
					val = (val ? 0 : 1);
		*/
		if (flag == UP)
			val = ((val == 0) ? 1 : 0);
		else
			val = ((val == 1) ? 0 : 1);

		Priority = val;

		if (Priority == ON)
		{
			WriteEEP(EEP_JUMPMode, 0);
			// WriteEEP(EEP_ONOFFChoice,OFF);     // OSD  lock....... on
			//  OSD_LOCKF=OFF;
		}
		else
		{

			WriteEEP(EEP_JUMPMode, 12);

			if (SelectModeType > CAM_D)
			{
				SelectModeType = CAM_A;
				SetNowMode();
				WriteEEP(EEP_SelectMode, SelectModeType);
			}

			WriteEEP(EEP_ONOFFChoice, ON); // OSD	lock....... off
			OSD_LOCKF = ON;
		}

		WriteEEP(EEP_PRIORITY, val);
		break;
	case CAMAONOFF:
		val = ReadEEP(EEP_CAMAMode);
		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);

		if (CAMA_M != val)
			VS4210_ResetCamMirror(CH1);

		CAMA_M = val;
		WriteEEP(EEP_CAMAMode, val);

		break;

	case MenutouchChoice:
		val = ReadEEP(EEP_MenuTouch);

		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);
		WriteEEP(EEP_MenuTouch, val);

		MenuTouchSel = val;
		break;

	case CAMBONOFF:
		val = ReadEEP(EEP_CAMBMode);
		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);

		if (CAMB_M != val)
			VS4210_ResetCamMirror(CH2);

		CAMB_M = val;
		WriteEEP(EEP_CAMBMode, val);

		break;
	case CAMCONOFF:
		val = ReadEEP(EEP_CAMCMode);
		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);

		if (CAMC_M != val)
			VS4210_ResetCamMirror(CH3);

		CAMC_M = val;
		WriteEEP(EEP_CAMCMode, val);

		break;
	case CAMRONOFF:
		val = ReadEEP(EEP_CAMRMode);
		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);

		if (CAMR_M != val)
			VS4210_ResetCamMirror(CH4);

		CAMR_M = val;
		WriteEEP(EEP_CAMRMode, val);

		break;
	case CAMDONOFF:
		val = ReadEEP(EEP_CAMDMode);
		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);

		if (CAMD_M != val)
			VS4210_ResetCamMirror(CH4);

		CAMD_M = val;
		WriteEEP(EEP_CAMDMode, val);

		break;
	case IMAGE_A_Choice:
		val = ReadEEP(EEP_IMAGE_A_Mode);
		if (flag == UP)
		{
			if (val < 6)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 6;
		}
		WriteEEP(EEP_IMAGE_A_Mode, val);
		break;
	case IMAGE_B_Choice:
		val = ReadEEP(EEP_IMAGE_B_Mode);
		if (flag == UP)
		{
			if (val < 6)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 6;
		}
		WriteEEP(EEP_IMAGE_B_Mode, val);
		break;
	case IMAGE_C_Choice:
		val = ReadEEP(EEP_IMAGE_C_Mode);
		if (flag == UP)
		{
			if (val < 6)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 6;
		}
		WriteEEP(EEP_IMAGE_C_Mode, val);
		break;
	case REARChoice:
		val = ReadEEP(EEP_RearMode);
		if (flag == UP)
		{
			if (val < 4 /*9*/)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 4 /*9*/;
		}
		WriteEEP(EEP_RearMode, val);
		break;

	case JUMPChoice:
		val = ReadEEP(EEP_JUMPMode);
		if (flag == UP)
		{
			if (val < (13 + 1))
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = (13 + 1);
		}

		WriteEEP(EEP_JUMPMode, val);
		/*
				if(val==0x01 && NowModeState==JumpMode)//if JUMP mode=SEQ
				{
					SEQIndex=0;
					//TW2835Cmd("\n\rmode 2\n\r");//set
					SEQMode();
					SEQFlag=1;
					SEQTime=0;
				}
				else if(NowModeState==JumpMode)///ryan@20170921
					{
					SetJumpMode();
					SEQFlag=0;//clear SEQ mode
					}
				*/
		if (SEQFlag && (val != 1))
			SEQFlag = _FALSE;

		break;
	case STIME_STEP:
		val = ReadEEP(EEP_TimeStep);
		if (flag == UP)
		{
			if (val < 100)
				val += 1;
			else
				val = 2;
		}
		else
		{
			if (val > 2)
				val -= 1;
			else
				val = 100;
		}

		WriteEEP(EEP_TimeStep, val);
		TimeStep = val;
		break;
	case DELAYCAMA:
		val = ReadEEP(EEP_DELAYCAMA);
		if (flag == UP)
		{
			if (val < 3)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 3;
		}
		WriteEEP(EEP_DELAYCAMA, val);

		break;
	case DELAYCAMB:
		val = ReadEEP(EEP_DELAYCAMB);
		if (flag == UP)
		{
			if (val < 3)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 3;
		}
		WriteEEP(EEP_DELAYCAMB, val);

		break;
	case DELAYCAMC:
		val = ReadEEP(EEP_DELAYCAMC);
		if (flag == UP)
		{
			if (val < 3)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 3;
		}
		WriteEEP(EEP_DELAYCAMC, val);

		break;
	case DELAYCAMR:
		val = ReadEEP(EEP_DELAYCAMR);
		if (flag == UP)
		{
			if (val < 3)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 3;
		}
		WriteEEP(EEP_DELAYCAMR, val);

		break;

	case DELAYCAMD:
		val = ReadEEP(EEP_DELAYCAMD);
		if (flag == UP)
		{
			if (val < 3)
				val += 1;
			else
				val = 0;
		}
		else
		{
			if (val > 0)
				val -= 1;
			else
				val = 3;
		}
		WriteEEP(EEP_DELAYCAMD, val);

		break;
#if (RGBGAINOSD == ON)
	case VIDEO_R_B_GAIN:
	case VIDEO_G_B_GAIN:
	case VIDEO_B_B_GAIN:
	case VIDEO_R_C_GAIN:
	case VIDEO_G_C_GAIN:
	case VIDEO_B_C_GAIN:
		val = VXISI2CRead(id);
		if (flag == UP)
		{
			if (val == 255)
				val = 0;
			else
				val++;
		}
		else
		{
			if (val == 0)
				val = 255;
			else
				val--;
		}

		VXISI2CWrite(id, val);
		break;
#endif
#if (TPMSOSD == ON)
		// TIRE1
	case TIRE1_High_PSI:
		val = ReadEEP(EEP_TIRE1_High_PSI);
		if (flag == UP)
		{
			if (val < TIRE_High_PSI_MAX)
				val += 1;
			else
				val = TIRE_High_PSI_MAX;
		}
		else
		{
			if (val > TIRE_High_PSI_MIN)
				val -= 1;
			else
				val = TIRE_High_PSI_MIN;
		}
		WriteEEP(EEP_TIRE1_High_PSI, val);
		TP_HIGH_PSI[0] = val;
		break;
	case TIRE1_Low_PSI:
		val = ReadEEP(EEP_TIRE1_Low_PSI);
		if (flag == UP)
		{
			if (val < TIRE_Low_PSI_MAX)
				val += 1;
			else
				val = TIRE_Low_PSI_MAX;
		}
		else
		{
			if (val > TIRE_Low_PSI_MIN)
				val -= 1;
			else
				val = TIRE_Low_PSI_MIN;
		}
		WriteEEP(EEP_TIRE1_Low_PSI, val);
		TP_LOW_PSI[0] = val;
		break;
	case TIRE1_High_Temp:
		val = ReadEEP(EEP_TIRE1_High_Temp);
		if (flag == UP)
		{
			if (val < TIRE_High_TEMP_MAX)
				val += 1;
			else
				val = TIRE_High_TEMP_MAX;
		}
		else
		{
			if (val > TIRE_High_TEMP_MIN)
				val -= 1;
			else
				val = TIRE_High_TEMP_MIN;
		}
		WriteEEP(EEP_TIRE1_High_Temp, val);
		TP_HIGH_TEMP[0] = val;
		break;
		// TIRE2
	case TIRE2_High_PSI:
		val = ReadEEP(EEP_TIRE2_High_PSI);
		if (flag == UP)
		{
			if (val < TIRE_High_PSI_MAX)
				val += 1;
			else
				val = TIRE_High_PSI_MAX;
		}
		else
		{
			if (val > TIRE_High_PSI_MIN)
				val -= 1;
			else
				val = TIRE_High_PSI_MIN;
		}
		WriteEEP(EEP_TIRE2_High_PSI, val);
		TP_HIGH_PSI[1] = val;
		break;
	case TIRE2_Low_PSI:
		val = ReadEEP(EEP_TIRE2_Low_PSI);
		if (flag == UP)
		{
			if (val < TIRE_Low_PSI_MAX)
				val += 1;
			else
				val = TIRE_Low_PSI_MAX;
		}
		else
		{
			if (val > TIRE_Low_PSI_MIN)
				val -= 1;
			else
				val = TIRE_Low_PSI_MIN;
		}
		WriteEEP(EEP_TIRE2_Low_PSI, val);
		TP_LOW_PSI[1] = val;
		break;
	case TIRE2_High_Temp:
		val = ReadEEP(EEP_TIRE2_High_Temp);
		if (flag == UP)
		{
			if (val < TIRE_High_TEMP_MAX)
				val += 1;
			else
				val = TIRE_High_TEMP_MAX;
		}
		else
		{
			if (val > TIRE_High_TEMP_MIN)
				val -= 1;
			else
				val = TIRE_High_TEMP_MIN;
		}
		WriteEEP(EEP_TIRE2_High_Temp, val);
		TP_HIGH_TEMP[1] = val;
		break;
		// TIRE3
	case TIRE3_High_PSI:
		val = ReadEEP(EEP_TIRE3_High_PSI);
		if (flag == UP)
		{
			if (val < TIRE_High_PSI_MAX)
				val += 1;
			else
				val = TIRE_High_PSI_MAX;
		}
		else
		{
			if (val > TIRE_High_PSI_MIN)
				val -= 1;
			else
				val = TIRE_High_PSI_MIN;
		}
		WriteEEP(EEP_TIRE3_High_PSI, val);
		TP_HIGH_PSI[2] = val;
		break;
	case TIRE3_Low_PSI:
		val = ReadEEP(EEP_TIRE3_Low_PSI);
		if (flag == UP)
		{
			if (val < TIRE_Low_PSI_MAX)
				val += 1;
			else
				val = TIRE_Low_PSI_MAX;
		}
		else
		{
			if (val > TIRE_Low_PSI_MIN)
				val -= 1;
			else
				val = TIRE_Low_PSI_MIN;
		}
		WriteEEP(EEP_TIRE3_Low_PSI, val);
		TP_LOW_PSI[2] = val;
		break;
	case TIRE3_High_Temp:
		val = ReadEEP(EEP_TIRE3_High_Temp);
		if (flag == UP)
		{
			if (val < TIRE_High_TEMP_MAX)
				val += 1;
			else
				val = TIRE_High_TEMP_MAX;
		}
		else
		{
			if (val > TIRE_High_TEMP_MIN)
				val -= 1;
			else
				val = TIRE_High_TEMP_MAX;
		}
		WriteEEP(EEP_TIRE3_High_Temp, val);
		TP_HIGH_TEMP[2] = val;
		break;
		// TIRE4
	case TIRE4_High_PSI:
		val = ReadEEP(EEP_TIRE4_High_PSI);
		if (flag == UP)
		{
			if (val < TIRE_High_PSI_MAX)
				val += 1;
			else
				val = TIRE_High_PSI_MAX;
		}
		else
		{
			if (val > TIRE_High_PSI_MIN)
				val -= 1;
			else
				val = TIRE_High_PSI_MIN;
		}
		WriteEEP(EEP_TIRE4_High_PSI, val);
		TP_HIGH_PSI[3] = val;
		break;
	case TIRE4_Low_PSI:
		val = ReadEEP(EEP_TIRE4_Low_PSI);
		if (flag == UP)
		{
			if (val < TIRE_Low_PSI_MAX)
				val += 1;
			else
				val = TIRE_Low_PSI_MAX;
		}
		else
		{
			if (val > TIRE_Low_PSI_MIN)
				val -= 1;
			else
				val = TIRE_Low_PSI_MIN;
		}
		WriteEEP(EEP_TIRE4_Low_PSI, val);
		TP_LOW_PSI[3] = val;
		break;
	case TIRE4_High_Temp:
		val = ReadEEP(EEP_TIRE4_High_Temp);
		if (flag == UP)
		{
			if (val < TIRE_High_TEMP_MAX)
				val += 1;
			else
				val = TIRE_High_TEMP_MAX;
		}
		else
		{
			if (val > TIRE_High_TEMP_MIN)
				val -= 1;
			else
				val = TIRE_High_TEMP_MIN;
		}
		WriteEEP(EEP_TIRE4_High_Temp, val);
		TP_HIGH_TEMP[3] = val;
		break;
		// TIRE5
	case TIRE5_High_PSI:
		val = ReadEEP(EEP_TIRE5_High_PSI);
		if (flag == UP)
		{
			if (val < TIRE_High_PSI_MAX)
				val += 1;
			else
				val = TIRE_High_PSI_MAX;
		}
		else
		{
			if (val > TIRE_High_PSI_MIN)
				val -= 1;
			else
				val = TIRE_High_PSI_MIN;
		}
		WriteEEP(EEP_TIRE5_High_PSI, val);
		TP_HIGH_PSI[4] = val;
		break;
	case TIRE5_Low_PSI:
		val = ReadEEP(EEP_TIRE5_Low_PSI);
		if (flag == UP)
		{
			if (val < TIRE_Low_PSI_MAX)
				val += 1;
			else
				val = TIRE_Low_PSI_MAX;
		}
		else
		{
			if (val > TIRE_Low_PSI_MIN)
				val -= 1;
			else
				val = TIRE_Low_PSI_MIN;
		}
		WriteEEP(EEP_TIRE5_Low_PSI, val);
		TP_LOW_PSI[4] = val;
		break;
	case TIRE5_High_Temp:
		val = ReadEEP(EEP_TIRE5_High_Temp);
		if (flag == UP)
		{
			if (val < TIRE_High_TEMP_MAX)
				val += 1;
			else
				val = TIRE_High_TEMP_MAX;
		}
		else
		{
			if (val > TIRE_High_TEMP_MIN)
				val -= 1;
			else
				val = TIRE_High_TEMP_MIN;
		}
		WriteEEP(EEP_TIRE5_High_Temp, val);
		TP_HIGH_TEMP[4] = val;
		break;
		// TIRE6
	case TIRE6_High_PSI:
		val = ReadEEP(EEP_TIRE6_High_PSI);
		if (flag == UP)
		{
			if (val < TIRE_High_PSI_MAX)
				val += 1;
			else
				val = TIRE_High_PSI_MAX;
		}
		else
		{
			if (val > TIRE_High_PSI_MIN)
				val -= 1;
			else
				val = TIRE_High_PSI_MIN;
		}
		WriteEEP(EEP_TIRE6_High_PSI, val);
		TP_HIGH_PSI[5] = val;
		break;
	case TIRE6_Low_PSI:
		val = ReadEEP(EEP_TIRE6_Low_PSI);
		if (flag == UP)
		{
			if (val < TIRE_Low_PSI_MAX)
				val += 1;
			else
				val = TIRE_Low_PSI_MAX;
		}
		else
		{
			if (val > TIRE_Low_PSI_MIN)
				val -= 1;
			else
				val = TIRE_Low_PSI_MIN;
		}
		WriteEEP(EEP_TIRE6_Low_PSI, val);
		TP_LOW_PSI[5] = val;
		break;
	case TIRE6_High_Temp:
		val = ReadEEP(EEP_TIRE6_High_Temp);
		if (flag == UP)
		{
			if (val < TIRE_High_TEMP_MAX)
				val += 1;
			else
				val = TIRE_High_TEMP_MAX;
		}
		else
		{
			if (val > TIRE_High_TEMP_MIN)
				val -= 1;
			else
				val = TIRE_High_TEMP_MIN;
		}
		WriteEEP(EEP_TIRE6_High_Temp, val);
		TP_HIGH_TEMP[5] = val;
		break;
#endif

	case Grabber_Liner:
		val = ReadEEP(EEP_Grabber_Liner);

		if (flag == UP)
			val = (!val ? 1 : 0);
		else
			val = (val ? 0 : 1);
		bytGarbberLiner = val;

		WriteEEP(EEP_Grabber_Liner, val);
		break;
	case Grabber_Liner_Width:

		if (bytGrabberAdjust == Grabber_Liner_Width)
		{

			val = ReadEEP(EEP_Grabber_Liner_Width);
			// OSDItemValue=val;
			// val=ChangeVideoData(VIDEO_CONTRAST,flag);
			//	pos=GetOSDStep(Grabber_Liner_Position,bytGarbberLinerPosition+inc);

			if (flag == UP)
			{
				if (val < 7)
					val += 1;
				else
					val = 7;
			}
			else
			{
				if (val > 1)
					val -= 1;
				else
					val = 1;
			}
			pos = GetOSDStep(Grabber_Liner_Position, bytGarbberLinerPosition);

			if (bytGarbberLinerMode == VS4210_LOOP_MODE0_w2)
			{
				//						if(((((val*2)+5)*FontWidthSIZE)+pos)>800)
				if (((((((val * 2) + 5) * FontWidthSIZE) / 2) + pos) > 800) || ((((((val * 2) + 5) * FontWidthSIZE) / 2) + 100) > pos))
				{
					Printf("(#WER1=%d)", (WORD)((((val * 2) + 5) * FontWidthSIZE) + pos));
					val = ReadEEP(EEP_Grabber_Liner_Width);
				}
				else
				{
					Printf("(#WS1=%d)", (WORD)((((val * 2) + 5) * FontWidthSIZE) + pos));
				}
			}
			else
			{
				if (((((val * 2) + 5) * FontWidthSIZE2) + pos) > 400)
				{
					Printf("(#2WER1=%d)", (WORD)((((val * 2) + 5) * FontWidthSIZE2) + pos));
					val = ReadEEP(EEP_Grabber_Liner_Width);
				}
				else
				{
					Printf("(#2WS1=%d)", (WORD)((((val * 2) + 5) * FontWidthSIZE2) + pos));
				}
			}

			Printf("(#W=%02x)", (WORD)val);
			bytGarbberLinerWidth = val;
			WriteEEP(EEP_Grabber_Liner_Width, val);
		}
		else if (bytGrabberAdjust == Grabber_Liner_Position)
		{

			val = ReadEEP(EEP_Grabber_Liner_Position);
			OSDItemValue = val;
			val = ChangeVideoData(Grabber_Liner_Position, flag);
			/*
			if(bytGarbberLinerWidth<3)
				{
						if(flag==UP)
						{
						   if(val<10) val+=1;
						   else val=10;
						}
						else
						{
						   if(val>1) val-=1;
						   else val=1;
						}

				}
				else
				{

				if(flag==UP)
				{
				   if(val<10) val+=1;
				   else val=10;
				}
				else
				{
				   if(val>1) val-=1;
				   else val=1;
				}

				}
				*/
			Printf("(#P=%02x)", (WORD)val);

			bytGarbberLinerPosition = val;
			WriteEEP(EEP_Grabber_Liner_Position, val);
		}
		break;
	case Grabber_Liner_Position:
		val = ReadEEP(EEP_Grabber_Liner_Position);
		bytGarbberLinerPosition = val;
		WriteEEP(EEP_Grabber_Liner_Position, val);
		break;
	}

	return val;
}

void DisplayOSDMenuOneItem(struct DefineMenu *DMp, BYTE x, BYTE y, BYTE NOTViewflag)
{
	BYTE DEC_Number[10] = {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9};
	BYTE Str_Number[] = {3, _spc, _spc, _spc};
	BYTE Str_action[] = {1, _action};
	BYTE Str_action2[] = {3, _spc, _spc, _action};

	BYTE val;
	// BYTE *ptr;

	switch (DMp[y].Type)
	{
	case BAR: // Draw Bar and Number value

		break;

	case NUMBER: // Draw Number value
				 // Printf("(NUM)");
		val = GetItemValue(DMp[y].Id);

		if (val / 100)
		{
			Str_Number[1] = DEC_Number[(val / 100)];
			val = val % 100;
			Str_Number[2] = DEC_Number[(val / 10)];
			val = val % 10;
			Str_Number[3] = DEC_Number[(val)];
		}
		else if (val / 10)
		{
			Str_Number[2] = DEC_Number[(val / 10)];
			val = val % 10;
			Str_Number[3] = DEC_Number[(val)];
		}
		else
			Str_Number[3] = DEC_Number[(val)];

		// if(bytGaugeAdjust)
		//	OsdDumpString(OSDMENU_MAINADDR+(y*MENU_WIDTH)+17-1,(tByte *)Str_Number);
		//	else
		if ((MenuChild[cursor_item].Id == Grabber_Liner_Width) || (MenuChild[cursor_item].Id == Grabber_Liner_Position))
			; // OsdDumpString(OSDMENU_MAINADDR+(y*MENU_WIDTH)+17,(tByte *)Str_action);
		else if ((MenuChild[4].Id == Grabber_Liner_Width) || (MenuChild[4].Id == Grabber_Liner_Position))
			OsdDumpString(OSDMENU_MAINADDR + (y * MENU_WIDTH) + 17, (tByte *)Str_action2);
		else
			OsdDumpString(OSDMENU_MAINADDR + (y * MENU_WIDTH) + 17, (tByte *)Str_Number);
		// OsdDumpAttribute(OSDMENU_MAINADDR+(y*MENU_WIDTH)+17,str_test,CH_COLOR_GREEN);
		break;

	case INFORMATION: // Draw Number value

		break;

	case ENUM_STRING:
		// Printf("(ENUM_STR)");
		val = GetItemValue(DMp[y].Id);
		// if(DMp[y].Id==CAMOSDONOFF)
		//	ptr=ONOFFChoiceString;
		//			OsdDumpString(OSDMENU_MAINADDR+(y*MENU_WIDTH)+17,&ONOFFChoiceString[2+(val*4)]);
		OsdDumpString(OSDMENU_MAINADDR + (y * MENU_WIDTH) + (MENU_WIDTH - (DMp[y].TypeData[1]) + 1), (tByte *)&DMp[y].TypeData[2 + (val * (DMp[y].TypeData[1]))]);
		break;
	case ACTION:
	case OSDMENU:
		OsdDumpString(OSDMENU_MAINADDR + (y * MENU_WIDTH) + 19, (tByte *)Str_action);
		break;

	case NUMBER_STRING:
	case CHOICEMENU:

		// Printf("(other)");

		//		 WriteStringToAddr(addr, RightSign_str, BARNUM_LENGTH);
		break;

	case TEXT_STR: // V1.1 02/28/07 simonsung
	case EXIT:
		//    WriteStringToAddr(addr, SpaceStr, BARNUM_LENGTH);
		break;
	}
}

void DrawMenulist(BYTE NOTViewflag)
{

	tByte x, y;
	// struct DefineMenu *MDpr=Title_Menu[0];
	// BYTE *ptr;

	// MenuChild=Title_Menu[0].Child;

	SetOsdSize(_Content, MENU_WIDTH, MENU_HEIGHT);
	SetOsdStartAddress(_Content, OSDMENU_MAINADDR);
	SetOsdPosition(_Content, MENU_OSD_X_ADDR, MENU_OSD_Y_ADDR);
	// SetOsdPosition(_Content,48,24+40);
	/*
		for(loop=0;loop<8;loop++)
			{
			 OsdDumpString(64+(loop*20),&MENU1[loop][0]);
			 OsdDumpAttribute(64+(loop*20),&MENU1[loop][0],CH_COLOR_GREEN);
			}
		*/
	for (x = 0; x < 8; x++)
	{
		//		 OsdDumpString(OSDMENU_MAINADDR+(x*MENU_WIDTH),&Str_Menu_Clr[x][0]);
		//		 OsdDumpAttribute(OSDMENU_MAINADDR+(x*MENU_WIDTH),&Str_Menu_Clr[x][0],DEFAULT_COLOR);

		OsdDumpString(OSDMENU_MAINADDR + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr);
		OsdDumpAttribute(OSDMENU_MAINADDR + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
	}
	// show osd main menu

	for (y = 0; y < MenuChild[0].MenuNum; y++)
	{
		// ptr = DMp->TypeData;
		// ptr=Title_Menu[x]->Str_E;
		OsdDumpString(OSDMENU_MAINADDR + (y * MENU_WIDTH), (tByte *)MenuChild[y].Str_E);
		OsdDumpAttribute(OSDMENU_MAINADDR + (y * MENU_WIDTH), (tByte *)MenuChild[y].Str_E, DEFAULT_COLOR);

		DisplayOSDMenuOneItem(MenuChild, 0, y, 0);
	}

	// if(MenuTouchMode==0)
	OsdDumpAttribute(OSDMENU_MAINADDR, (tByte *)Str_Menu_Cursor, CURSOR_COLOR);
	/*
	if(GetItemValue(CAMOSDONOFF)==_FALSE)
	VXIS_fSetOsdOnOff(0 ,_ContentOn|_BottomOn);
	else
	VXIS_fSetOsdOnOff(0 ,_ContentOn |_TitleOn|_BottomOn);
*/
	//	OSDMenuLevel=1;
	//	title_item=0;
	//	cursor_item=0;

	//	SetDisplayedOSD( MENU);
}
#if 0
void ClearCursor(void)
{
	//BYTE cursor_x;
	BYTE cursor_y;
	WORD addr;
/*
	if( cursor_item==NIL )
		return;

	//cursor_x = MenuFormat->CursorStart;
	cursor_y = cursor_item + ( MenuFormat->TitleColor ==NIL ? 0 : 1 );
	if( cur_osdwin == 4 ) addr = 0;
	else addr = 14; //weylis -@121008 modify
	//else addr = 10;
	addr += cursor_y*MenuFormat->width;
	DrawAttrCnt(addr, DEFAULT_COLOR, MenuFormat->width );
#ifdef HS_NEWOSDMENU_2
  // DrawAttrCnt(addr+46, DEFAULT_COLOR, (MenuFormat->width) );	//Weylis -@121008
#endif  
*/
}

void DisplayCursor(void)
{
	BYTE cursor_y=0,i ;
	WORD addr;
//#ifdef HS_NEWOSDMENU  //weylis -@121008
//	CODE_P BYTE 	*Str;
//#endif
//	if( cursor_item==NIL )
//		return;

//    for(i=0;i<8;i++)  //william-20091026
    for(i=0;i<(MENU_HEIGHT+1);i++)  //ryan@20170825
    {
     // cursor_y = i ;
     // addr = OSDMENU_SUBADDR;  
     // addr += cursor_y*MenuFormat->width; 
    //  DrawAttrCnt(addr, DEFAULT_COLOR, MenuFormat->width );  // ���������頦���剖��鞊�������蝺����頦���剖��嚙� 
      	 OsdDumpString(64+(i*20),&Str_Menu_Clr[i][0]);
	 OsdDumpAttribute(64+(i*20),&Str_Menu_Clr[i][0],CH_COLOR_GREEN);
    }
/*
	cursor_y = cursor_item + ( MenuFormat->TitleColor ==NIL ? 0 : 1 );
#ifdef HS_NEWOSDMENU_2
	if( OSDMenuLevel == 1 ) addr = OSDMENU_MAINADDR+HS_CHSPACE;
	//if( OSDMenuLevel == 1 ) addr = OSDMENU_MAINADDR;
	else addr = OSDMENU_SUBADDR+HS_CHSPACE;
#else
	if( OSDMenuLevel == 1 ) addr = OSDMENU_MAINADDR;
	else addr = OSDMENU_SUBADDR;
#endif
*/
	addr = OSDMENU_SUBADDR;

	//addr += cursor_y*MenuFormat->width;

	if( OSDMenuLevel == 1)
		//DrawAttrCnt(addr, MenuFormat->CursorColor, MenuFormat->width );	
	else
	{
		if(OnChangingValue)
		{
			GetItemValue(GetItemValue(MenuChild[cursor_item].Id));
			//DrawAttrCnt(addr, MenuFormat->CursorColor, MenuFormat->width );	
		}
		else
		{
			//DrawAttrCnt(addr, MenuFormat->CursorColor, MenuFormat->width-BARNUM_LENGTH );	
			//DrawAttrCnt(addr+MAX_DESC, DEFAULT_COLOR, BARNUM_LENGTH );	
		}
	}

}
#endif
// void DisplayOSDMenu(void)
//{

//}

void SET_CROSS_LINE(BYTE sel)
{

	if (sel == ON)
	{

		switch (gbVXIS_OuputModeflg)
		{
		case VS4210_1080P_MODE0_w0:
		case VS4210_1080P_MODE0_w1:
		case VS4210_1080P_MODE0_w2:
		case VS4210_1080P_MODE0_w3:
		case VS4210_1080P_MODE0_w4:
			break;

		case VS4210_1080P_MODE1:
		case VS4210_1080P_MODE10:
		case VS4210_1080P_MODE8:
		case VS4210_1080P_MODE9:
#if 0
/*
				VXISI2CWrite(0xB8,0x01);
				VXISI2CWrite(0xB9,0x80);
				VXISI2CWrite(0xB9,0x81);
				
				VXISI2CWrite(0xB8,0x02);
				VXISI2CWrite(0xB9,0x80);			
				VXISI2CWrite(0xB9,0x81);
				*/
				//VXISI2CWrite(0xB8,0x02);
				//VXISI2CWrite(0xB9,0x81);
				//VXISI2CWrite(0xB9,0x81);

#else
			/*
			VXISI2CWrite(0xB8,0x01);
			VXISI2CWrite(0xB9,0x81);
			VXISI2CWrite(0xB8,0x02);
			VXISI2CWrite(0xB9,0x81);
			*/
			VS4210_Line_Draw(gbVXIS_OuputModeflg);
#endif

			break;
		case VS4210_1080P_MODE12:
			/*
			VXISI2CWrite(0xB8,0x01);
			VXISI2CWrite(0xB9,0x81);
			*/
			VS4210_Line_Draw(gbVXIS_OuputModeflg);
			// VXISI2CWrite(0xB8,0x02);
			// VXISI2CWrite(0xB9,0x81);
			break;
		}
	}
	else
	{
		VXISI2CWrite(0xB8, 0x01);
		VXISI2CWrite(0xB9, 0x01);
		VXISI2CWrite(0xB8, 0x02);
		VXISI2CWrite(0xB9, 0x01);
	}
}

void OpenOSDMenu(BYTE OSD_ID)

{

	tByte x;
	// struct DefineMenu *MDpr=Title_Menu[0];
	// BYTE *ptr;

	VXIS_fSetOsdOnOff(0, 0);
	VXIS_fSetOsdOnOff(1, 0);
	VXIS_fSetOsdOnOff(2, 0);
	VXIS_fSetOsdOnOff(3, 0);

	SET_CROSS_LINE(OFF);

	Printf("\r\nOpenOSDMenu");

	MenuChild = Title_Menu;
	SetOsdSize(_Content, MENU_WIDTH, MENU_HEIGHT);
	SetOsdStartAddress(_Content, MENU_START);
	SetOsdPosition(_Content, MENU_OSD_X_ADDR, MENU_OSD_Y_ADDR);

	// SetOsdPosition(_Content,48,24+40);
	/*
		for(loop=0;loop<8;loop++)
			{
			 OsdDumpString(64+(loop*20),&MENU1[loop][0]);
			 OsdDumpAttribute(64+(loop*20),&MENU1[loop][0],CH_COLOR_GREEN);
			}
		*/

	// clr jump osd  Str_OSD_CLRJUMP
	//  OsdDumpString(MENU_START,Str_OSD_CLRJUMP);

	for (x = 0; x < 8; x++)
	{
		OsdDumpString(MENU_START + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr);
		OsdDumpAttribute(MENU_START + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
		// OsdDumpString(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr[x][0]);
		// OsdDumpAttribute(MENU_START+(x*MENU_WIDTH),&Str_Menu_Clr[x][0],DEFAULT_COLOR);
	}

	// show osd main menu
	VXISI2CWrite(0x51, 0x0A); //_ContentOn |WINZOOMx2
	msleep(100);
#if 1

	if (bytGaugeAdjust == 0)
	{
		for (x = 0; x < _TITLE_NUMBER; x++)
		{
			// ptr = DMp->TypeData;
			// ptr=Title_Menu[x]->Str_E;
			OsdDumpString(MENU_START + (x * MENU_WIDTH), (tByte *)MenuChild[x].Str_E);
			OsdDumpAttribute(MENU_START + (x * MENU_WIDTH), (tByte *)MenuChild[x].Str_E, DEFAULT_COLOR);
		}

#else

	for (x = 0; x < 4; x++)
	{
		OsdDumpString(64 + (x * 20), Str_Main_OSD[x]);
		OsdDumpAttribute(64 + (x * 20), Str_Main_OSD[x], CH_COLOR_GREEN);
	}
#endif
		// action
		// for(x=0;x<4;x++)
		//{
		//  OsdDumpString(84+(x*20),Str_action[x]);
		//  OsdDumpAttribute(84+(x*20),Str_action[x],CH_COLOR_GREEN);
		//	}

		// if(MenuTouchMode==0)
		OsdDumpAttribute(MENU_START, (tByte *)&Str_Menu_Cursor[0], CURSOR_COLOR);
	}

	// if(GetItemValue(CAMOSDONOFF)==_FALSE)
	// VXIS_fSetOsdOnOff(0 ,_ContentOn|_BottomOn);
	// else
	//{
	// VXIS_fSetOsdOnOff(0 ,_ContentOn |_TitleOn|_BottomOn|WINZOOMx2);
	/*
	VXIS_fSetOsdOnOff(0 ,0);
	VXIS_fSetOsdOnOff(1 ,0);
	VXIS_fSetOsdOnOff(2 ,0);
	VXIS_fSetOsdOnOff(3 ,0);
	*/
	//	msleep(100) ;
	/*
	msleep(1000) ;

	VXISI2CWrite(0x51,0x08);//_ContentOn
	//VXISI2CWrite(0x51,0x02);//_ContentOn
	msleep(1000) ;
	VXISI2CWrite(0x51,0x0A);//_ContentOn |WINZOOMx2
	*/
	//}

	// SET_CROSS_LINE(OFF);

	// OsdDumpAttribute(MENU_START,&Str_Menu_Cursor[0],CURSOR_COLOR);

	OSDMenuLevel = 1;
	OnChangingValue = 0;

	cursor_item = 0;
	// MenuChild=Title_Menu;

	SetDisplayedOSD(MENU);

	if (bytGaugeAdjust == DistanceGaugePosition)
	{
		MenuChild = Distance_Gauge_Menu;
		cursor_item = 1;
		DrawMenulist(0);
		OsdDumpAttribute(OSDMENU_MAINADDR + (0 * MENU_WIDTH), Str_Menu_Cursor, DEFAULT_COLOR);
		OsdDumpAttribute(OSDMENU_MAINADDR + (1 * MENU_WIDTH), Str_Menu_Cursor, CURSOR_COLOR);
		bytGaugeAdjust = 0;
	}
	else if (bytGaugeAdjust == DistanceGaugeWidth)
	{
		MenuChild = Distance_Gauge_Menu;
		cursor_item = 2;
		DrawMenulist(0);
		OsdDumpAttribute(OSDMENU_MAINADDR + (0 * MENU_WIDTH), Str_Menu_Cursor, DEFAULT_COLOR);
		OsdDumpAttribute(OSDMENU_MAINADDR + (2 * MENU_WIDTH), Str_Menu_Cursor, CURSOR_COLOR);
		bytGaugeAdjust = 0;
	}
}
#if 0
BYTE GetLowerMenuItem(  struct DefineMenu *DMp, BYTE itemno )
{
	if( itemno==NIL )
		return NIL;
	itemno++;
//	if( (DMp[itemno].IconColor==NIL) && (DMp[itemno].DescColor==NIL) )		// end
//		return NIL;
	return itemno;

}

BYTE GetUpperMenuItem(  BYTE itemno )
{
	//if( itemno==0 || 
	if(	itemno==NIL )
		return NIL;
	itemno--;
	return itemno;
}

BYTE GetNextCursorMenuItem( struct DefineMenu *DMp, BYTE itemno, BYTE flag)
{
	BYTE new_cursor;

	switch( flag ) {
	case UP:	new_cursor = GetUpperMenuItem( /*DMp,*/ itemno );		break;
	case DN:	new_cursor = GetLowerMenuItem( DMp, itemno );		break;
	}
	return new_cursor;
}
#endif
void OSDValueUpDn(BYTE flag)
{

	switch (MenuChild[cursor_item].Type)
	{
	case NUMBER:
		SetItemValue(MenuChild[cursor_item].Id, flag);
		if (bytGaugeAdjust == 0)
			DisplayOSDMenuOneItem(MenuChild, 0, cursor_item, 0);
		// DrawNum( addr, (CODE_P struct NumType *)ptr, val );

		break;
	case ENUM_STRING:
		// Printf("(OSDValUpDn)");
		SetItemValue(MenuChild[cursor_item].Id, flag);
		if (bytGaugeAdjust == 0)
			DisplayOSDMenuOneItem(MenuChild, 0, cursor_item, 0);

		break;
	}
}
void GetOSDMenu(void)
{
	//	if(OSDMenuLevel==2)
	//		{
	//		MenuChild=Title_Menu;
	//		}
	//	if(MenuChild[cursor_item].MenuID!=_TITLE_OSD)
	//			MenuChild=Title_Menu;
	switch (MenuChild[cursor_item].MenuID)
	{
	case _PICTURE_OSD:
	case _INPUT_OSD:
	case _VIDEO_OSD:
	case _RGB_GAIN_OSD:
	case _TPMS_OSD:
	case _ULIT_OSD:
		MenuChild = Title_Menu;
		break;
	case _RESET_OSD:
		MenuChild = Ulti_Menu;
		break;
	case _MIRROR_OSD:
	case _DIR_IMAGE_OSD:
		MenuChild = Video_Menu;
		break;
	case _TRIGGER_DELAY_OSD:
		MenuChild = Trigger_Set_Menu;
		break;
#if (TPMSOSD == ON)
	case _TPMS_HIGH_PSI_OSD:
	case _TPMS_LOW_PSI_OSD:
	case _TPMS_HIGH_TEMP_OSD:
		MenuChild = TPMS_Menu;
		break;
#endif

	case _DISTANCE_GAUGE_OSD:
		MenuChild = Input_Menu;
		break;
	}
}

void DisplayCursor(void)
{
	if (OnChangingValue)
	{
		// GetItemValue(GetItemValue(MenuChild[cursor_item].Id));
		// DrawAttrCnt(addr, MenuFormat->CursorColor, MenuFormat->width );
		//				OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Clr[0][0],DEFAULT_COLOR);
		//				OsdDumpAttribute((OSDMENU_MAINADDR)+(cursor_item*MENU_WIDTH),&Str_Menu_Cursor[0],CURSOR_COLOR);

		//			OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Clr[0][0],CURSOR_COLOR);
		if (MenuTouchMode == 0)
		{
			//					OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Clr,CURSOR_COLOR);
			TouchDisplayCursor(_MENU_CLR_TOUCH_CURSOR2);
		}
		else
		{
			TouchDisplayCursor(_MENU_SET_TOUCH_CURSOR);
		}

		OsdDumpAttribute(OSDMENU_MAINADDR + (cursor_item * MENU_WIDTH), (tByte *)&Str_Menu_Clr, CURSOR_COLOR);
	}
	else
	{
		//	DrawAttrCnt(addr, MenuFormat->CursorColor, MenuFormat->width-BARNUM_LENGTH );
		// DrawAttrCnt(addr+MAX_DESC, DEFAULT_COLOR, BARNUM_LENGTH );
		//			OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Clr[0][0],DEFAULT_COLOR);
		if (MenuTouchMode == 0)
		{

			TouchDisplayCursor(_MENU_CLR_TOUCH_CURSOR2);
			OsdDumpAttribute(OSDMENU_CURSORADDR + (cursor_item * MENU_WIDTH), (tByte *)&Str_Menu_Cursor[0], DEFAULT_COLOR);
		}
		else
		{
			// ClearCursor();
			// TouchDisplayCursor(_MENU_CLR_TOUCH_CURSOR);
			TouchDisplayCursor(_MENU_CLR_TOUCH_CURSOR2);
			OsdDumpAttribute(OSDMENU_CURSORADDR + (cursor_item * MENU_WIDTH), (tByte *)&Str_Menu_Cursor[0], DEFAULT_COLOR);
			// Printf("\r\n(44)");
		}
		// OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Cursor[0],CURSOR_COLOR);
	}
}
void TouchDisplayCursor(BYTE index)
{
	BYTE x, ret = 1;
#if (_DEBUG_MESSAGE_GT911 == ON)
	Printf("TDCursor index=%d", (WORD)index);
#endif

	if ((OneTouchPosition) /*&&(OnChangingValue==0)*/)
	{

		switch (index)
		{
		case _VOLUME_UP:
			OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH) + 14, (tByte *)Str_VolumeINC, TOUCH_ADJUST_COLOR);
			OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH) + 1, (tByte *)Str_VolumeDEC, DEFAULT_COLOR);
			break;
		case _VOLUME_DOWN:
			OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH) + 1, (tByte *)Str_VolumeDEC, TOUCH_ADJUST_COLOR);
			OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH) + 14, (tByte *)Str_VolumeINC, DEFAULT_COLOR);
			break;
		case _VOLUME_RELEASE:
			OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH) + 1, (tByte *)Str_VolumeDEC, DEFAULT_COLOR);
			OsdDumpAttribute(MENU_START + (2 * MENU_WIDTH) + 14, (tByte *)Str_VolumeINC, DEFAULT_COLOR);
			break;
		case _MENU_CURSOR:

			if (OnChangingValue == 0)
			{

				for (x = 0; x < (MenuChild[0].MenuNum); x++)
				{
					if ((OneTouchPosition >= MenuChild[x].TouchAddr) && (OneTouchPosition <= (MenuChild[x].TouchAddr + 19)))
					{
						OsdDumpAttribute(OSDMENU_MAINADDR + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, TOUCH_CURSOR_COLOR);

						// if(cursor_item!=x)
						// OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Clr,DEFAULT_COLOR);

						cursor_item = x;
						cursor_change = x;
#if (_DEBUG_MESSAGE_GT911 == ON)
						Printf("\r\ncursor_item=%02x", (WORD)cursor_item);
#endif
						// break;
						ret = 0;
					}
					else
					{
						OsdDumpAttribute(OSDMENU_MAINADDR + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
					}
				}

				if (ret == 1)
				{
					cursor_item = 0;
					// OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Cursor[0]/*&Str_Menu_Clr*/,CURSOR_COLOR);
				}
			}
			else
			{

				for (x = 0; x < (MenuChild[0].MenuNum); x++)
				{
					if ((OneTouchPosition >= MenuChild[x].TouchAddr) && (OneTouchPosition <= (MenuChild[x].TouchAddr + 19)) && (cursor_item != x))
					{
						OsdDumpAttribute(OSDMENU_MAINADDR + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, TOUCH_CURSOR_COLOR);

						if (cursor_item != x)
						{
							cursor_change = x;
#if (_DEBUG_MESSAGE_GT911 == ON)
							Printf("\r\ncursor_change=%02x", (WORD)cursor_change);
#endif
						}
						else
							cursor_change = cursor_item;
					}
					else if (cursor_item != x)
						OsdDumpAttribute(OSDMENU_MAINADDR + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
				}

				if (MenuTouchMode == 1)
				{
					if (TOUCH_ADDR_OFFSET(OneTouchPosition, _MenuDecAddr) == _TRUE)
						CheckTouchScreen(_MENU_DOWN, OneTouchPosition);
					else if (TOUCH_ADDR_OFFSET(OneTouchPosition, _MenuIncAddr) == _TRUE)
						CheckTouchScreen(_MENU_UP, OneTouchPosition);
					/*
					if(TOUCH_ADDR_OFFSET(OneTouchPosition,(MenuChild[cursor_item].TouchAddr+14))==_TRUE)
					CheckTouchScreen(_MENU_DOWN,OneTouchPosition);
					else if(TOUCH_ADDR_OFFSET(OneTouchPosition,(MenuChild[cursor_item].TouchAddr+19))==_TRUE)
					CheckTouchScreen(_MENU_UP,OneTouchPosition);
					*/
				}
			}

			break;
		case _MENU_UP:
			// OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+14,Str_VolumeDEC,CURSOR_COLOR);
			// OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+19,Str_VolumeINC,TOUCH_ADJUST_COLOR);

			OsdDumpAttribute(TOUCH_OSD_DEC_ADDR, (tByte *)Str_VolumeDEC, DEFAULT_COLOR);
			OsdDumpAttribute(TOUCH_OSD_INC_ADDR, (tByte *)Str_VolumeINC, TOUCH_ADJUST_COLOR);
			break;
		case _MENU_DOWN:
			// OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+14,Str_VolumeDEC,TOUCH_ADJUST_COLOR);
			// OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+19,Str_VolumeINC,CURSOR_COLOR);
			OsdDumpAttribute(TOUCH_OSD_DEC_ADDR, (tByte *)Str_VolumeDEC, TOUCH_ADJUST_COLOR);
			OsdDumpAttribute(TOUCH_OSD_INC_ADDR, (tByte *)Str_VolumeINC, DEFAULT_COLOR);

			break;
		case _MENU_RELEASE:
			if (OnChangingValue == 1)
			{
				// OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+14,Str_VolumeDEC,CURSOR_COLOR);
				// OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+19,Str_VolumeINC,CURSOR_COLOR);

				OsdDumpAttribute(TOUCH_OSD_DEC_ADDR, (tByte *)Str_VolumeDEC, DEFAULT_COLOR);
				OsdDumpAttribute(TOUCH_OSD_INC_ADDR, (tByte *)Str_VolumeINC, DEFAULT_COLOR);
			}
			break;
		case _MENU_SET_TOUCH_CURSOR:
			/*
			OsdDumpString(MENU_START+(cursor_item*MENU_WIDTH)+14,Str_VolumeDEC);
			OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+14,Str_VolumeDEC,TOUCH_CURSOR_COLOR);
			OsdDumpString(MENU_START+(cursor_item*MENU_WIDTH)+19,Str_VolumeINC);
			OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+19,Str_VolumeINC,TOUCH_CURSOR_COLOR);
			*/
			OsdDumpString(TOUCH_OSD_DEC_ADDR, (tByte *)Str_VolumeDEC);
			OsdDumpAttribute(TOUCH_OSD_DEC_ADDR, (tByte *)Str_VolumeDEC, DEFAULT_COLOR);
			OsdDumpString(TOUCH_OSD_INC_ADDR, (tByte *)Str_VolumeINC);
			OsdDumpAttribute(TOUCH_OSD_INC_ADDR, (tByte *)Str_VolumeINC, DEFAULT_COLOR);
#if (_DEBUG_MESSAGE_GT911 == ON)
			Printf("\r\n(_MENU_SET_TOUCH_CURSOR)");
#endif
			break;
		case _MENU_CLR_TOUCH_CURSOR:
			OsdDumpString(TOUCH_OSD_DEC_ADDR, (tByte *)Str_OSD_TRI_CLR);
			OsdDumpAttribute(TOUCH_OSD_DEC_ADDR, (tByte *)Str_OSD_TRI_CLR, DEFAULT_COLOR);
			OsdDumpString(TOUCH_OSD_INC_ADDR, (tByte *)Str_OSD_TRI_CLR);
			OsdDumpAttribute(TOUCH_OSD_INC_ADDR, (tByte *)Str_OSD_TRI_CLR, DEFAULT_COLOR);
#if (_DEBUG_MESSAGE_GT911 == ON)
			Printf("\r\n(_MENU_CLR_TOUCH_CURSOR)");
#endif
			break;

		case _MENU_CLR_TOUCH_CURSOR2:
			// OsdDumpString(MENU_START+(cursor_item*MENU_WIDTH)+14,Str_OSD_TRI_CLR);
			// OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+14,Str_OSD_TRI_CLR,CURSOR_COLOR);
			// OsdDumpString(MENU_START+(cursor_item*MENU_WIDTH)+19,Str_OSD_TRI_CLR);
			// OsdDumpAttribute(MENU_START+(cursor_item*MENU_WIDTH)+19,Str_OSD_TRI_CLR,CURSOR_COLOR);

			OsdDumpString(TOUCH_OSD_DEC_ADDR, (tByte *)Str_OSD_TRI_CLR);
			OsdDumpAttribute(TOUCH_OSD_DEC_ADDR, (tByte *)Str_OSD_TRI_CLR, DEFAULT_COLOR);
			OsdDumpString(TOUCH_OSD_INC_ADDR, (tByte *)Str_OSD_TRI_CLR);
			OsdDumpAttribute(TOUCH_OSD_INC_ADDR, (tByte *)Str_OSD_TRI_CLR, DEFAULT_COLOR);
#if (_DEBUG_MESSAGE_GT911 == ON)
			Printf("\r\n(_MENU_CLR_TOUCH_CURSOR2)");
#endif
			break;
		case _COCOSD_LEFT:
			for (x = 0; x < 5; x++)
			{
				if (coc_cursor == x)
					continue;
				OsdDumpAttribute(COCOSD_START + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
				OsdDumpAttribute(COCOSD_START + 2 + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
			}

			OsdDumpAttribute(COCOSD_START + (coc_cursor * 4), (tByte *)Str_COC_OSD[0], TOUCH_ADJUST_COLOR);
			OsdDumpAttribute(COCOSD_START + 2 + (coc_cursor * 4), (tByte *)Str_COC_OSD[0], TOUCH_ADJUST_COLOR);
			break;
		case _COCOSD_RIGHT:
			for (x = 0; x < 5; x++)
			{
				if (coc_cursor == x)
					continue;
				OsdDumpAttribute(COCOSD_START + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
				OsdDumpAttribute(COCOSD_START + 2 + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
			}
			OsdDumpAttribute(COCOSD_START + (coc_cursor * 4), (tByte *)Str_COC_OSD[1], TOUCH_ADJUST_COLOR);
			OsdDumpAttribute(COCOSD_START + 2 + (coc_cursor * 4), (tByte *)Str_COC_OSD[1], TOUCH_ADJUST_COLOR);
			break;
		case _COCOSD_UP:
			for (x = 0; x < 5; x++)
			{
				if (coc_cursor == x)
					continue;
				OsdDumpAttribute(COCOSD_START + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
				OsdDumpAttribute(COCOSD_START + 2 + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
			}
			OsdDumpAttribute(COCOSD_START + (coc_cursor * 4), (tByte *)Str_COC_OSD[2], TOUCH_ADJUST_COLOR);
			OsdDumpAttribute(COCOSD_START + 2 + (coc_cursor * 4), (tByte *)Str_COC_OSD[2], TOUCH_ADJUST_COLOR);
			break;
		case _COCOSD_DOWN:
			for (x = 0; x < 5; x++)
			{
				if (coc_cursor == x)
					continue;
				OsdDumpAttribute(COCOSD_START + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
				OsdDumpAttribute(COCOSD_START + 2 + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
			}
			OsdDumpAttribute(COCOSD_START + (coc_cursor * 4), (tByte *)Str_COC_OSD[3], TOUCH_ADJUST_COLOR);
			OsdDumpAttribute(COCOSD_START + 2 + (coc_cursor * 4), (tByte *)Str_COC_OSD[3], TOUCH_ADJUST_COLOR);
			break;

		case _COCOSD_ENTER:
			for (x = 0; x < 5; x++)
			{
				if (coc_cursor == x)
					continue;
				OsdDumpAttribute(COCOSD_START + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
				OsdDumpAttribute(COCOSD_START + 2 + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
			}
			OsdDumpAttribute(COCOSD_START + (coc_cursor * 4), (tByte *)Str_COC_OSD[4], TOUCH_ADJUST_COLOR);
			OsdDumpAttribute(COCOSD_START + 2 + (coc_cursor * 4), (tByte *)Str_COC_OSD[4], TOUCH_ADJUST_COLOR);

			break;

		case _COCOSD_RELEASE:

			for (x = 0; x < 5; x++)
			{
				OsdDumpAttribute(COCOSD_START + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
				OsdDumpAttribute(COCOSD_START + 2 + (x * 4), (tByte *)Str_COC_OSD[x], DEFAULT_COLOR);
			}
#if (_DEBUG_MESSAGE_GT911 == ON)
			Printf("\r\n(_COCOSD_RELEASE2)");
#endif

			break;
		}
	}
}
void OSDSelect(void)
{
	BYTE x, ret = 1;

#if (_DEBUG_OSD == ON)
	Printf("\r\nOSDSel1");
#endif

	// if(OSDMenuLevel==1)
	//{
	if ((OneTouchPosition) && (MenuTouchMode == 1) && (OnChangingValue == 0))
	{

		for (x = 0; x < (MenuChild[0].MenuNum); x++)
		{
			if ((OneTouchPosition >= MenuChild[x].TouchAddr) && (OneTouchPosition <= (MenuChild[x].TouchAddr + 19)))
			{
				cursor_item = x;
				Printf("\r\nAddr=%d OSDSel=%d", (WORD)OneTouchPosition, (WORD)x);
				ret = 0;
				// break;
				// continue;
			}
			else
				OsdDumpAttribute(OSDMENU_MAINADDR + (x * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
		}
		if ((ret == 1) && ((GetDisplayedOSD() & MENU) == MENU))
		{
			cursor_item = 0;
			/*
			for(x=0;x<(MenuChild[0].MenuNum);x++)
				OsdDumpAttribute(OSDMENU_MAINADDR+(x*MENU_WIDTH),&Str_Menu_Clr,DEFAULT_COLOR);
			*/
			// OsdDumpAttribute(OSDMENU_MAINADDR+(x*MENU_WIDTH),&Str_Menu_Clr,DEFAULT_COLOR);

			OsdDumpAttribute(OSDMENU_MAINADDR + (cursor_item * MENU_WIDTH), (tByte *)&Str_Menu_Cursor[0] /*&Str_Menu_Clr*/, CURSOR_COLOR);

			return;
		}
	}

	Printf("\r\nOSDSel=%02x", (WORD)MenuChild[cursor_item].Type);

	switch (MenuChild[cursor_item].Type)
	{
	case EXIT:
		//	if(MenuChild[cursor_item].MenuID==_ULIT_OSD)
		if (MenuChild[cursor_item].Id == EXIT)
		{
			MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DISABLE);
			CloseOSDMenu();
		}
		else /// if(OSDMenuLevel==_PICTURE_OSD)
		{
			GetOSDMenu();
			DrawMenulist(0);
			cursor_item = 0;
		}

		break;
	case OSDMENU:
		switch (MenuChild[cursor_item].MenuID)
		{
		case _TITLE_OSD:
			MenuChild = Title_Menu[cursor_item].Child;
			break;
		case _VIDEO_OSD:
			MenuChild = Video_Menu[cursor_item].Child;
			break;
#if (TPMSOSD == ON)
		case _TPMS_OSD:
			MenuChild = TPMS_Menu[cursor_item].Child;
			break;
#endif
		case _DIR_IMAGE_OSD:
			MenuChild = Trigger_Set_Menu[cursor_item].Child;
			break;
		case _ULIT_OSD:
			MenuChild = Ulti_Menu[cursor_item].Child;
			break;
		case _INPUT_OSD:
			MenuChild = Input_Menu[cursor_item].Child;
			break;
		}

		cursor_item = 0;
		DrawMenulist(0);

		break;
	case NUMBER:

		OnChangingValue = 1;
		// switch ( MenuChild[cursor_item].Id ) {
		// default:
		DisplayCursor();
		//		DrawNum( (CODE_P struct NumType *)ptr, GetItemValue(MenuChild[cursor_item].Id) );
		// break;
		//}
		// return;

		if (MenuChild[cursor_item].Id == DistanceGaugePosition)
		{
			if ((NowModeState == SelectMode) && (SelectModeType == VS4210_LOOP_MODE0_w3) && (TriggerFlag == 0))
			{
				bytGaugeAdjust = DistanceGaugePosition;
				Printf("(DistanceGaugePosition)");
				ShowGaugeOSD();
			}
			else
			{
				Printf("(1DistanceGauge_ERROR)");
				OnChangingValue = 0;
				DisplayCursor();
			}
		}
		else if (MenuChild[cursor_item].Id == DistanceGaugeWidth)
		{
			if ((NowModeState == SelectMode) && (SelectModeType == VS4210_LOOP_MODE0_w3))
			{
				bytGaugeAdjust = DistanceGaugeWidth;
				Printf("(DistanceGaugeWidth)");
				ShowGaugeOSD();
			}
			else
			{
				Printf("(2DistanceGauge_ERROR)");
				OnChangingValue = 0;
				DisplayCursor();
			}
		}
		else if (MenuChild[cursor_item].Id == Grabber_Liner_Width)
		{
			if ((bytGarbberLinerMode != VS4210_LOOP_MODE_ERROR) && (bytGarbberLinerMode == VS4210_LOOP_MODE0_w2) && (ReadEEP(EEP_Grabber_Liner) == ON))
			{

				bytGrabberAdjust = Grabber_Liner_Width;
				Printf("(Grabber_Liner_Width)");
				ShowGrabberLinerOSD();
			}
			else
			{
				Printf("(GrabberLiner_ERROR)");
				OnChangingValue = 0;
				DisplayCursor();
			}
		}

		break;
		break;
	case ENUM_STRING:
		OnChangingValue = 1; // Value changable label on
		DisplayCursor();	 // Display Setect Item cursor
		//	GetItemValue(MenuChild[cursor_item].Id);

		if (MenuChild[cursor_item].Id == Grabber_Liner)
		{
			Printf("(Cursor on  Grabber Lines)");
			OnChangingValue = 0;
			DisplayCursor();
		}
		break;
	case ACTION:
		DoAction(MenuChild[cursor_item].Id); // Action.
		return;
	}

#if 0
	if(MenuChild[cursor_item].Type==EXIT)
	 {
	 	if(MenuChild[cursor_item].MenuID==_ULIT_OSD)
	 		{
			CloseOSDMenu(); 
	 		}
		else ///if(OSDMenuLevel==_PICTURE_OSD)
			{
		   GetOSDMenu();
		   DrawMenulist(0);
		 //  OSDMenuLevel=_PICTURE_OSD;
		   cursor_item=0;
		   	}
	 }
	else	if(MenuChild[cursor_item].Type==OSDMENU)
	{
	MenuChild=Title_Menu[cursor_item].Child;
	cursor_item=0;
	//OSDMenuLevel=MenuChild[0].MenuID;
	DrawMenulist(0);
	}
#endif
	//}
}

void GrabberModeTrigger(void)
{

	if (MenuChild[cursor_item].Id == Grabber_Liner)
	{
		OnChangingValue = 1;
		DisplayCursor();

		if (ReadEEP(EEP_Grabber_Liner) == OFF)
		{
			WriteEEP(EEP_Grabber_Liner, ON);
			bytGarbberLiner = ON;
		}
		else
		{
			WriteEEP(EEP_Grabber_Liner, OFF);
			bytGarbberLiner = OFF;
		}

		DisplayOSDMenuOneItem(MenuChild, 0, cursor_item, 0);

		OnChangingValue = 0;
		DisplayCursor();
	}
}

void ClearCursor(void)
{
	//	OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Clr[0][0],DEFAULT_COLOR);
	OsdDumpAttribute(OSDMENU_MAINADDR + (cursor_item * MENU_WIDTH), (tByte *)&Str_Menu_Clr, DEFAULT_COLOR);
}

void OSDCursorMove(BYTE flag)
{
#if 0

	BYTE new_cursor, old_cursor;
		WORD dat;
		
		new_cursor = GetNextCursorMenuItem( MenuChild, cursor_item, flag ); // Get item to move


old_cursor = cursor_item;
	cursor_item = new_cursor;
	if(OSDMenuLevel == 1)
	{		
	OsdDumpAttribute(64+(cursor_item*20),&Str_Menu_Clr[0][0],CH_COLOR_GREEN|BG_COLOR_Gray);
	}
	else	
	{
		//if( MenuChild[ old_cursor ].Id == OSDLANG)
		//	DrawMenulist(0); // 	Recover Attr. and menu Data list
		DisplayCursor();
	}

#else
	if (flag == UP)
	{

		// Printf("Move Cursor up");

		//			if( title_item==3 && cur_osdwin==2

		// if(OSDMenuLevel == 1)///4
		{
			//	OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Clr[0][0],DEFAULT_COLOR);

			//	OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Cursor[0],DEFAULT_COLOR);
			OsdDumpAttribute(OSDMENU_MAINADDR + (cursor_item * MENU_WIDTH), Str_Menu_Cursor, DEFAULT_COLOR);
			//	ClearCursor();

			if (cursor_item == 0)
			{
				cursor_item = MenuChild[0].MenuNum;
			}

			cursor_item--;
			//	OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*CURSOR_WIDTH),&Str_Menu_Clr[0][0],CURSOR_COLOR);
			OsdDumpAttribute(OSDMENU_MAINADDR + (cursor_item * MENU_WIDTH), Str_Menu_Cursor, CURSOR_COLOR);
		}
	}
	else
	{
		//	Printf("Move Cursor down");
		// if(OSDMenuLevel == 1)///4
		{
			//	OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*MENU_WIDTH),&Str_Menu_Clr[0][0],DEFAULT_COLOR);

			OsdDumpAttribute(OSDMENU_MAINADDR + (cursor_item * MENU_WIDTH), Str_Menu_Cursor, DEFAULT_COLOR);

			// ClearCursor();
			cursor_item++;

			if (cursor_item >= MenuChild[0].MenuNum)
			{
				cursor_item = 0;
			}
			//	OsdDumpAttribute(OSDMENU_MAINADDR+(cursor_item*CURSOR_WIDTH),&Str_Menu_Clr[0][0],CURSOR_COLOR);
			OsdDumpAttribute(OSDMENU_MAINADDR + (cursor_item * MENU_WIDTH), Str_Menu_Cursor, CURSOR_COLOR);
		}
	}
#endif

	// cursor_item=title_item;
}

static void VX_Write_OSD_In_OutString(tByte ch, tByte *Str1, tByte *Str2, int out_mode)
{
	tiWord ix, iy, sx, sy;
	tByte TitleFlg;
	tByte BarFlg;

	//    tByte loop;
	TitleFlg = (u8gDrawOSDFlg & 0x02) >> 1;
	BarFlg = u8gDrawOSDFlg & 0x01;

	if (out_mode <= VS4210_1080P_MODE0_w4)
	{
		if (gbVXIS_OuputSize == _OUTPUT_1080P60)
		{
			ix = Mode0_1080P_param[0];
			iy = Mode0_1080P_param[1];
		}
		else if (gbVXIS_OuputSize == _OUTPUT_720P60)
		{
			ix = Mode0_720P_param[0];
			iy = Mode0_720P_param[1];
		}
		else
		{
			ix = Mode0_800x480_param[0];
			iy = Mode0_800x480_param[1];
			sx = Mode0S_800x480_param[0];
			sy = Mode0S_800x480_param[1];
		}
	}
	else if (out_mode == VS4210_1080P_MODE1)
	{
		if (ch == 0)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode1_w0_1080P_param[0];
				iy = Mode1_w0_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode1_w0_720P_param[0];
				iy = Mode1_w0_720P_param[1];
			}
			else
			{
				ix = Mode1_w0_800x480_param[0];
				iy = Mode1_w0_800x480_param[1];
				sx = Mode1S_w0_800x480_param[0];
				sy = Mode1S_w0_800x480_param[1];
			}
		}
		else if (ch == 1)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode1_w1_1080P_param[0];
				iy = Mode1_w1_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode1_w1_720P_param[0];
				iy = Mode1_w1_720P_param[1];
			}
			else
			{
				ix = Mode1_w1_800x480_param[0];
				iy = Mode1_w1_800x480_param[1];
				sx = Mode1S_w1_800x480_param[0];
				sy = Mode1S_w1_800x480_param[1];
			}
		}
		else if (ch == 2)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode1_w2_1080P_param[0];
				iy = Mode1_w2_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode1_w2_720P_param[0];
				iy = Mode1_w2_720P_param[1];
			}
			else
			{
				ix = Mode1_w2_800x480_param[0];
				iy = Mode1_w2_800x480_param[1];
				sx = Mode1S_w2_800x480_param[0];
				sy = Mode1S_w2_800x480_param[1];
			}
		}
		else // if (ch == 3 )
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode1_w3_1080P_param[0];
				iy = Mode1_w3_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode1_w3_720P_param[0];
				iy = Mode1_w3_720P_param[1];
			}
			else
			{
				ix = Mode1_w3_800x480_param[0];
				iy = Mode1_w3_800x480_param[1];
				sx = Mode1S_w3_800x480_param[0];
				sy = Mode1S_w3_800x480_param[1];
			}
		}
	}
#if 1
	else if (out_mode == VS4210_1080P_MODE10)
	{
		if (ch == 0)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode8_w0_1080P_param[0];
				iy = Mode8_w0_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode8_w0_720P_param[0];
				iy = Mode8_w0_720P_param[1];
			}
			else
			{
				ix = Mode10_w0_800x480_param[0];
				iy = Mode10_w0_800x480_param[1];
				sx = Mode10S_w0_800x480_param[0];
				sy = Mode10S_w0_800x480_param[1];
			}
		}
		else if (ch == 1)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode8_w1_1080P_param[0];
				iy = Mode8_w1_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode8_w1_720P_param[0];
				iy = Mode8_w1_720P_param[1];
			}
			else
			{
				ix = Mode10_w1_800x480_param[0];
				iy = Mode10_w1_800x480_param[1];
				sx = Mode10S_w1_800x480_param[0];
				sy = Mode10S_w1_800x480_param[1];
			}
		}
		else if (ch == 2)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode8_w2_1080P_param[0];
				iy = Mode8_w2_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode8_w2_720P_param[0];
				iy = Mode8_w2_720P_param[1];
			}
			else
			{
				ix = Mode10_w2_800x480_param[0];
				iy = Mode10_w2_800x480_param[1];
				sx = Mode10S_w2_800x480_param[0];
				sy = Mode10S_w2_800x480_param[1];
			}
		}
	}

#else
	else if (out_mode == VS4210_1080P_MODE2)
	{
		if (ch == 0)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode2_w0_1080P_param[0];
				iy = Mode2_w0_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode2_w0_720P_param[0];
				iy = Mode2_w0_720P_param[1];
			}
			else
			{
				ix = Mode2_w0_800x480_param[0];
				iy = Mode2_w0_800x480_param[1];
			}
		}
		else if (ch == 1)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode2_w1_1080P_param[0];
				iy = Mode2_w1_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode2_w1_720P_param[0];
				iy = Mode2_w1_720P_param[1];
			}
			else
			{
				ix = Mode2_w1_800x480_param[0];
				iy = Mode2_w1_800x480_param[1];
			}
		}
		else if (ch == 2)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode2_w2_1080P_param[0];
				iy = Mode2_w2_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode2_w2_720P_param[0];
				iy = Mode2_w2_720P_param[1];
			}
			else
			{
				ix = Mode2_w2_800x480_param[0];
				iy = Mode2_w2_800x480_param[1];
			}
		}
		else // if (ch == 3 )
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode2_w3_1080P_param[0];
				iy = Mode2_w3_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode2_w3_720P_param[0];
				iy = Mode2_w3_720P_param[1];
			}
			else
			{
				ix = Mode2_w3_800x480_param[0];
				iy = Mode2_w3_800x480_param[1];
			}
		}
	}
#endif
	else if (out_mode == VS4210_1080P_MODE8)
	{
		if (ch == 0)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode8_w0_1080P_param[0];
				iy = Mode8_w0_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode8_w0_720P_param[0];
				iy = Mode8_w0_720P_param[1];
			}
			else
			{
				ix = Mode8_w0_800x480_param[0];
				iy = Mode8_w0_800x480_param[1];
				sx = Mode8S_w0_800x480_param[0];
				sy = Mode8S_w0_800x480_param[1];
			}
		}
		else if (ch == 1)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode8_w1_1080P_param[0];
				iy = Mode8_w1_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode8_w1_720P_param[0];
				iy = Mode8_w1_720P_param[1];
			}
			else
			{
				ix = Mode8_w1_800x480_param[0];
				iy = Mode8_w1_800x480_param[1];
				sx = Mode8S_w1_800x480_param[0];
				sy = Mode8S_w1_800x480_param[1];
			}
		}
		else if (ch == 2)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode8_w2_1080P_param[0];
				iy = Mode8_w2_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode8_w2_720P_param[0];
				iy = Mode8_w2_720P_param[1];
			}
			else
			{
				ix = Mode8_w2_800x480_param[0];
				iy = Mode8_w2_800x480_param[1];
				sx = Mode8S_w2_800x480_param[0];
				sy = Mode8S_w2_800x480_param[1];
			}
		}
	}
	else if (out_mode == VS4210_1080P_MODE9)
	{
		if (ch == 0)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode9_w0_1080P_param[0];
				iy = Mode9_w0_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode9_w0_720P_param[0];
				iy = Mode9_w0_720P_param[1];
			}
			else
			{
				ix = Mode9_w0_800x480_param[0];
				iy = Mode9_w0_800x480_param[1];
				sx = Mode9S_w0_800x480_param[0];
				sy = Mode9S_w0_800x480_param[1];
			}
		}
		else if (ch == 1)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode9_w1_1080P_param[0];
				iy = Mode9_w1_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode9_w1_720P_param[0];
				iy = Mode9_w1_720P_param[1];
			}
			else
			{
				ix = Mode9_w1_800x480_param[0];
				iy = Mode9_w1_800x480_param[1];
				sx = Mode9S_w1_800x480_param[0];
				sy = Mode9S_w1_800x480_param[1];
			}
		}
		else if (ch == 2)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode9_w2_1080P_param[0];
				iy = Mode9_w2_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode9_w2_720P_param[0];
				iy = Mode9_w2_720P_param[1];
			}
			else
			{
				ix = Mode9_w2_800x480_param[0];
				iy = Mode9_w2_800x480_param[1];
				sx = Mode9S_w2_800x480_param[0];
				sy = Mode9S_w2_800x480_param[1];
			}
		}
	}
	else if (out_mode == VS4210_1080P_MODE12)
	{
		if (ch == 0)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode12_w0_1080P_param[0];
				iy = Mode12_w0_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode12_w0_720P_param[0];
				iy = Mode12_w0_720P_param[1];
			}
			else
			{
				ix = Mode12_w0_800x480_param[0];
				iy = Mode12_w0_800x480_param[1];
				sx = Mode12S_w0_800x480_param[0];
				sy = Mode12S_w0_800x480_param[1];
			}
		}
		else
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode12_w1_1080P_param[0];
				iy = Mode12_w1_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode12_w1_720P_param[0];
				iy = Mode12_w1_720P_param[1];
			}
			else
			{
				ix = Mode12_w1_800x480_param[0];
				iy = Mode12_w1_800x480_param[1];
				sx = Mode12S_w1_800x480_param[0];
				sy = Mode12S_w1_800x480_param[1];
			}
		}
	}
	else if (out_mode == VS4210_1080P_MODE14)
	{
		if (ch == 0)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode14_w0_1080P_param[0];
				iy = Mode14_w0_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode14_w0_720P_param[0];
				iy = Mode14_w0_720P_param[1];
			}
			else
			{
				ix = Mode14_w0_800x480_param[0];
				iy = Mode14_w0_800x480_param[1];
			}
		}
		else if (ch == 1)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode14_w1_1080P_param[0];
				iy = Mode14_w1_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode14_w1_720P_param[0];
				iy = Mode14_w1_720P_param[1];
			}
			else
			{
				ix = Mode14_w1_800x480_param[0];
				iy = Mode14_w1_800x480_param[1];
			}
		}
		else if (ch == 2)
		{
			if (gbVXIS_OuputSize == _OUTPUT_1080P60)
			{
				ix = Mode14_w2_1080P_param[0];
				iy = Mode14_w2_1080P_param[1];
			}
			else if (gbVXIS_OuputSize == _OUTPUT_720P60)
			{
				ix = Mode14_w2_720P_param[0];
				iy = Mode14_w2_720P_param[1];
			}
			else
			{
				ix = Mode14_w2_800x480_param[0];
				iy = Mode14_w2_800x480_param[1];
			}
		}
	}
	else
	{
		ix = 224;
		iy = 64;
	}

	if (ch == 0)
	{
#if (_DEBUG_MESSAGE_BOOT_NOISE == ON)
		Printf("(*CH0)");
#endif

		if (TitleFlg == 1)
		{
			SetOsdSize(_Title0, Str1[0], 1);
			SetOsdStartAddress(_Title0, 0x00);
			OsdDumpString(0x00, (tByte *)Str1);
			OsdDumpAttribute(0x00, (tByte *)Str1, DEFAULT_COLOR);
			//     	OsdDumpAttribute(0x00,Str1,BG_COLOR_Royal_Blue|CH_COLOR_RED);//test
			SetOsdPosition(_Title0, ix, iy);
		}

		// Printf("(CURRENT=%02x)",(WORD)CURRENT_CH);

		if (OSD_MIRROR[0] == _TRUE)
		{
			OsdDumpString(0 + MIRROR_OSD_START, Str_OSD_MIRROR);
			OsdDumpAttribute(0 + MIRROR_OSD_START, Str_OSD_MIRROR, CH_COLOR_YELLOW);
		}
		if (OSD_SPEAKER[0] == ON)
		{
			if (AudioVol == 0)
			{
				OsdDumpString(0 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE);
				OsdDumpAttribute(0 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE, CH_COLOR_GREEN);
			}
			else
			{
				OsdDumpString(0 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER);
				OsdDumpAttribute(0 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER, CH_COLOR_GREEN);
			}
		}

		ShowTRIGOSD();

		if (BarFlg == 1)
		{
			if ((pVS4210_JointKind1.WindowMap.WMap0 == 4) && (bytGarbberLiner == ON) && (bSignal[3] == 0) && (bytGarbberLinerMode != VS4210_LOOP_MODE_ERROR))
				Str2 = &OSD_CAM_C_VLOSS_CLR[0];

			SetOsdSize(_Bar0, Str2[0], 1);
			SetOsdStartAddress(_Bar0, 24);
			OsdDumpString(24, Str2);
			OsdDumpAttribute(24, Str2, _Color);
			//       	OsdDumpAttribute(24,Str2,BG_COLOR_BLACK|CH_COLOR_YELLOW);//test
			SetOsdPosition(_Bar0, sx, sy + 20);
		}
	}
	else if (ch == 1)
	{

#if (_DEBUG_MESSAGE_BOOT_NOISE == ON)
		Printf("(*CH1)");
#endif

		if (TitleFlg == 1)
		{
			SetOsdSize(_Title1, Str1[0], 1);
			SetOsdStartAddress(_Title1, 6);
			OsdDumpString(6, Str1);
			OsdDumpAttribute(6, Str1, DEFAULT_COLOR);
			SetOsdPosition(_Title1, ix, iy);
		}

		if (OSD_MIRROR[1] == _TRUE)
		{
			OsdDumpString(6 + MIRROR_OSD_START, (tByte *)Str_OSD_MIRROR);
			OsdDumpAttribute(6 + MIRROR_OSD_START, (tByte *)Str_OSD_MIRROR, CH_COLOR_YELLOW);
		}

		if (OSD_SPEAKER[1] == ON)
		{
			if (AudioVol == 0)
			{
				OsdDumpString(6 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE);
				OsdDumpAttribute(6 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE, CH_COLOR_GREEN);
			}
			else
			{
				OsdDumpString(6 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER);
				OsdDumpAttribute(6 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER, CH_COLOR_GREEN);
			}
		}

		ShowTRIGOSD();

		if (BarFlg == 1)
		{

			if ((pVS4210_JointKind1.WindowMap.WMap1 == 4) && (bytGarbberLiner == ON) && (bSignal[3] == 0) && (bytGarbberLinerMode != VS4210_LOOP_MODE_ERROR))
				Str2 = &OSD_CAM_C_VLOSS_CLR[0];

			SetOsdSize(_Bar1, Str2[0], 1);
			SetOsdStartAddress(_Bar1, 34);
			OsdDumpString(34, Str2);
			OsdDumpAttribute(34, Str2, _Color);
			SetOsdPosition(_Bar1, sx, sy + 20);
		}
	}
	else if (ch == 2)
	{

#if (_DEBUG_MESSAGE_BOOT_NOISE == ON)
		Printf("(*CH2)");
#endif

		if (TitleFlg == 1)
		{
			SetOsdSize(_Title2, Str1[0], 1);
			SetOsdStartAddress(_Title1, 12);
			OsdDumpString(12, Str1);
			OsdDumpAttribute(12, Str1, DEFAULT_COLOR);
			SetOsdPosition(_Title2, ix, iy);
		}

		if (OSD_MIRROR[2] == _TRUE)
		{
			OsdDumpString(12 + MIRROR_OSD_START, (tByte *)Str_OSD_MIRROR);
			OsdDumpAttribute(12 + MIRROR_OSD_START, (tByte *)Str_OSD_MIRROR, CH_COLOR_YELLOW);
		}
		if (OSD_SPEAKER[2] == ON)
		{
			if (AudioVol == 0)
			{
				OsdDumpString(12 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE);
				OsdDumpAttribute(12 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE, CH_COLOR_GREEN);
			}
			else
			{
				OsdDumpString(12 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER);
				OsdDumpAttribute(12 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER, CH_COLOR_GREEN);
			}
		}
		ShowTRIGOSD();

		if (BarFlg == 1)
		{

			if ((pVS4210_JointKind1.WindowMap.WMap2 == 4) && (bytGarbberLiner == ON) && (bSignal[3] == 0) && (bytGarbberLinerMode != VS4210_LOOP_MODE_ERROR))
				Str2 = &OSD_CAM_C_VLOSS_CLR[0];

			SetOsdSize(_Bar2, Str2[0], 1);
			SetOsdStartAddress(_Bar2, 44);
			OsdDumpString(44, Str2);
			OsdDumpAttribute(44, Str2, _Color);
			SetOsdPosition(_Bar2, sx, sy + 20);

			//}
		}
	}
	else if (ch == 3)
	{

#if (_DEBUG_MESSAGE_BOOT_NOISE == ON)
		Printf("(*CH3)");
#endif

		if (TitleFlg == 1)
		{
			SetOsdSize(_Title3, Str1[0], 1);
			SetOsdStartAddress(_Title3, 18);
			OsdDumpString(18, Str1);
			OsdDumpAttribute(18, Str1, DEFAULT_COLOR);
			SetOsdPosition(_Title3, ix, iy);
		}

		if (OSD_MIRROR[3] == _TRUE)
		{
			OsdDumpString(18 + MIRROR_OSD_START, (tByte *)Str_OSD_MIRROR);
			OsdDumpAttribute(18 + MIRROR_OSD_START, (tByte *)Str_OSD_MIRROR, CH_COLOR_YELLOW);
		}

		if (OSD_SPEAKER[3] == ON)
		{
			if (AudioVol == 0)
			{
				OsdDumpString(18 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE);
				OsdDumpAttribute(18 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER_MUTE, CH_COLOR_GREEN);
			}
			else
			{
				OsdDumpString(18 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER);
				OsdDumpAttribute(18 + SPEAKER_OSD_START, (tByte *)Str_OSD_SPEAKER, CH_COLOR_GREEN);
			}
		}

		ShowTRIGOSD();

		if (BarFlg == 1)
		{

			if ((pVS4210_JointKind1.WindowMap.WMap3 == 4) && (bytGarbberLiner == ON) && (bSignal[3] == 0) && (bytGarbberLinerMode != VS4210_LOOP_MODE_ERROR))
				Str2 = &OSD_CAM_C_VLOSS_CLR[0];

			SetOsdSize(_Bar3, Str2[0], 1);
			SetOsdStartAddress(_Bar3, 54);
			OsdDumpString(54, Str2);
			OsdDumpAttribute(54, Str2, _Color);
			SetOsdPosition(_Bar3, sx, sy + 20);
		}
	}

	setTransParentIndex(8);

	if (u8gDrawOSDFlg == 3)
	{
		if ((TriggerStatus == 4) && (TriggerFlag == REVERSE_T) && (GetItemValue(GAUGEChoice) == _TRUE) && (ReadEEP(EEP_RearMode) == 0))
			ShowGaugeOSD();
		else
		{
#if (TH0701024600NYR50S1 == OFF)
			VXIS_fSetOsdOnOff(ch, _TitleOn | _BottomOn);
#else
			if (SEQFlag == OFF)
				VXIS_fSetOsdOnOff(ch, _TitleOn | _BottomOn);
			else
				VXIS_fSetOsdOnOff(ch, _BottomOn);
#endif
		}
		//         VXIS_fSetOsdOnOff(ch , _TitleOn | _BottomOn|_ContentOn|WINZOOMx2 );
	}
	else if (TitleFlg == 1)
	{
		VXIS_fSetOsdOnOff(ch, _TitleOn);
	}
	else if (BarFlg == 1)
	{
		VXIS_fSetOsdOnOff(ch, _BottomOn);
	}
	else
	{
		VXIS_fSetOsdOnOff(ch, _TurnoffAll);
	}

	if (GetItemValue(CAMOSDONOFF) == _FALSE)
	{

		MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_DISPLAY_DISABLE);
		MCUTimerActiveTimerEvent(SEC(2), _USER_TIMER_EVENT_DISPLAY_DISABLE);
	}

	//	if((TriggerStatus==4)&&((TriggerFlag&REVERSE_T)==REVERSE_T))
	/*
		if((TriggerStatus==4)&&	(TriggerFlag&REVERSE_T==REVERSE_T))
		{
		ShowGaugeOSD();
		}
		*/
	MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_GRABBER_SETUP);
	MCUTimerActiveTimerEvent(SEC(0.5), _SYSTEM_TIMER_EVENT_GRABBER_SETUP);

	// SetGarbberLinerMode(bytGarbberLinerMode);

	// mask ryan@20210803 	if(bytGarbberLinerMode!=VS4210_LOOP_MODE_ERROR)
	//	ShowJUMPOSD();
}
static void GetconvertBarStr(tByte in_mode)
{
	switch (in_mode)
	{
		// HDMI
	case _OSD_720X480I60:
		ptcOSDbar = &OSD_720x480I60_param[0];
		break;
	case _OSD_720X576I50:
		ptcOSDbar = &OSD_720x576I50_param[0];
		break;
	case _OSD_720X480P60:
		ptcOSDbar = &OSD_720x480P60_param[0];
		break;
	case _OSD_720X576P50:
		ptcOSDbar = &OSD_720x576P50_param[0];
		break;
	case _OSD_1920X1080I:
		ptcOSDbar = &OSD_1920x1080I_param[0];
		break;
	case _OSD_1280X720P:
		ptcOSDbar = &OSD_1280x720P_param[0];
		break;
	case _OSD_1920X1080P:
		ptcOSDbar = &OSD_1920x1080P_param[0];
		break;
	case _OSD_No_Signal:
		ptcOSDbar = &OSD_No_Signal_param[0];
		break;

	default:
		ptcOSDbar = &OSD_No_Signal_param[0];
		break;
	}
}

/**
 * @brief Input resolution
 *
 * Input resolution on the screen display
 *
 * @param in_mode Input resolution
 * @param out_mode Output resolution
 * @return none
 */
void VXIS_fShowInOutMode(tByte ch, tByte in_mode, tByte out_mode)
{

	tcByte *ptr1;
	tByte OSD_CHx_param[7 + 4];
	tByte pu8aOSDbar[11];
	tByte chFix;

	chFix = gaHDMI_MapWindowIndex[ch];
	ptr1 = &OSD_CH1_param[0];

	if (out_mode == VS4210_1080P_MODE0_w1)
	{
		if (ch == 1)
		{
			ptr1 = OSD_CH1_param;
			chFix = 0;
		}
	}
	else if (out_mode == VS4210_1080P_MODE0_w2)
	{
		if (ch == 4)
		{
			ptr1 = OSD_CH2_param;
			chFix = 0;
		}
	}
	else if (out_mode == VS4210_1080P_MODE0_w3)
	{
		if (ch == 3)
		{
			ptr1 = OSD_CH3_param;
			chFix = 0;
		}
	}
	else if (out_mode == VS4210_1080P_MODE0_w4)
	{
		if (ch == 2)
		{
			ptr1 = OSD_CH4_param;

			chFix = 0;
		}
	}
	else
	{
		if (chFix == 0)
		{
			ptr1 = OSD_CH0_param;
		}
		else if (chFix == 2)
		{
			ptr1 = OSD_CH1_param;
		}
		else if (chFix == 3)
		{
			ptr1 = OSD_CH2_param;
		}
		else
		{
			ptr1 = OSD_CH3_param;
		}
	}

	if (gu8AudioMode == chFix)
	{
		memcpy(OSD_CHx_param, ptr1, 6);
		OSD_CHx_param[6] = 0x6C;
	}
	else
	{
		memcpy(OSD_CHx_param, ptr1, 7 + 4); /// test
	}

	GetconvertBarStr(in_mode);
	memcpy(pu8aOSDbar, ptcOSDbar, 11);
	VX_Write_OSD_In_OutString(ch, OSD_CHx_param, pu8aOSDbar, out_mode);
}

void VXIS_fShowInOutMode0OSD(tByte ch, tByte in_mode, tByte out_mode)
{

	tcByte *ptr1;
	tByte OSD_CHx_param[7];
	tByte pu8aOSDbar[11];
	tByte chFix;

	if (ch == 0)
	{
		ptr1 = OSD_CH0_param;
	}
	else if (ch == 2)
	{
		ptr1 = OSD_CH1_param;
	}
	else if (ch == 3)
	{
		if (((TriggerStatus == 5) && (TriggerFlag)) || ((bytSelectCAMRD == SelectCameraD) && (TriggerFlag == 0)))
			ptr1 = OSD_CH4_param;
		else
			ptr1 = OSD_CH2_param;
	}
	else
	{

		if ((TriggerStatus == 5) || ((bytSelectCAMRD == SelectCameraD) && (TriggerFlag == 0)))
			ptr1 = OSD_CH4_param;
		else
		{
			if ((SelectModeType == SelectCameraR) || (TriggerStatus == 4) || (bytSelectCAMRD == SelectCameraR))
				ptr1 = OSD_CH3_param;
			else
				ptr1 = OSD_CH4_param;
		}
	}

	if (gu8AudioMode == ch)
	{
		memcpy(OSD_CHx_param, ptr1, 6);
		OSD_CHx_param[6] = 0x6C;
	}
	else
	{
		memcpy(OSD_CHx_param, ptr1, 7);
	}

	chFix = 0;
	GetconvertBarStr(in_mode);
	memcpy(pu8aOSDbar, ptcOSDbar, 11);
	VX_Write_OSD_In_OutString(chFix, OSD_CHx_param, pu8aOSDbar, out_mode);
}

void OSD_Clear(tByte Win)
{
	tByte temp53;
	tByte temp5E;
	tByte temp2;

	if (Win == 0)
	{
		SetOsdSize(_Title0, 0, 1);
		SetOsdSize(_Bar0, 0, 1);
	}
	else
	{
		temp2 = VXISI2CRead(0x53);
		temp53 = temp2 & 0x3f;
		temp2 = VXISI2CRead(0x5E);
		temp5E = temp2 & 0x3f;

		if (Win == 1)
		{
			VXISI2CWrite(0x53, 0x40 | temp53);
			VXISI2CWrite(0x5E, 0x40 | temp5E);
		}
		else if (Win == 2)
		{
			VXISI2CWrite(0x53, 0x80 | temp53);
			VXISI2CWrite(0x5E, 0x80 | temp5E);
		}
		else if (Win == 3)
		{
			VXISI2CWrite(0x53, 0xC0 | temp53);
			VXISI2CWrite(0x5E, 0xC0 | temp5E);
		}

		VXISI2CWrite(0x51, 0x00);
	}
}

void OSD_Clear_4(void)
{
	OSD_Clear(3);
}

void OSD_Clear_34(void)
{
	tByte i;
	for (i = 2; i < 4; i++)
	{
		OSD_Clear(i);
	}
}

void OSD_Clear_234(void)
{
	tByte i;
	for (i = 1; i < 4; i++)
	{
		OSD_Clear(i);
	}
}

void OSD_Clear_All(void)
{
	tByte i;
	for (i = 0; i < 4; i++)
	{
		OSD_Clear(i);
	}
}

/*
void ClearAudioOSD(void)
{
	tByte i ;
	for (i = 0  ; i < HDMI_IN_NUMBER ; i++ )
	{
		VXIS_fShowInOutMode(gaHDMI_MapWindowIndex[i]  ,  gbVXIS_InputMode[i] , gbVXIS_OuputModeflg )  ;
	}
}

void Clear1ChAudioOSD(tByte ch )
{
	VXIS_fShowInOutMode(gaHDMI_MapWindowIndex[ch]  ,  gbVXIS_InputMode[ch] , gbVXIS_OuputModeflg )  ;
}

*/
/**
 * @brief Initialize the custom font function
 *
 * Initialize the custom font data.
 *
 * @param Size, Number of custom fonts.
 * @param *UserDefineFont, Font data start position.
 * @return none
 */
#if 1
void VXOSD_UseFont_init(tByte Size, tcByte *UserDefineFontPtr)
{ // Extern OSD
	// 16*20
	uint16_t FontIndex;
	uint8_t bytecommand, Haddr, Laddr;
	uint16_t i;
	uint16_t FontAddr;
	uint8_t Size2;

	Size2 = Size * 2;
	if (Size < 64)
	{
		for (i = 0; i < Size2; i++) //
		{
			FontAddr = i * 10;	// WORD
			FontIndex = i * 20; // BYTE
			Haddr = (uint8_t)((FontAddr & 0x0F00) >> 8);
			Laddr = (uint8_t)(FontAddr & 0x00FF);
			bytecommand = 0x20 | Haddr;
			osd_cont_write(bytecommand, Laddr, 20, &UserDefineFontPtr[FontIndex]); // user define font
		}
	}
	else
	{
		printf("Error ,  Size >64\r\n");
	}
}

#else
void VXOSD_UseFont_init(uint8_t Size, const uint8_t *UserDefineFontPtr) // 20200110-01
{																		// Extern OSD
																		// 16*20
	uint16_t size1;
	uint8_t bytecommand, Hsize, Lsize;
	uint8_t i;
	uint16_t FontIndex;
	uint8_t OSDarray[80];
	uint8_t Size2;
	Size2 = Size * 2;

	size1 = 0;
	for (i = 0; i < Size2; i++)
	{
		OSDarray[i] = 0x80 + i;
		size1 = i * 20;
		Hsize = (uint8_t)((size1 & 0x0700) >> 8);
		Lsize = (uint8_t)(size1 & 0x00FF);
		bytecommand = 0x20 | Hsize;
		FontIndex = 40 * i;
		osd_cont_write(bytecommand, Lsize, 40, &UserDefineFontPtr[FontIndex]); // user define font
																			   // Printf("\r\n(bytecommand=%04x  Lsize=%04x FontIndex=%d)",(WORD)bytecommand,(WORD)Lsize,(WORD)FontIndex);
	}
	osd_cont_write(0x10, 0x80, Size, &OSDarray[0]);
}

#endif
