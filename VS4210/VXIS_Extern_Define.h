/**
 * @brief VX889x configuration file
 *
 * Define the input / output / ISP / MCU and other profiles.
 *
 * @author Cunu-Yu
 * @date 2018/07/06 */

#ifndef _VXIS_EXTERN_DEFINE_H
#define _VXIS_EXTERN_DEFINE_H
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

/// debug message
#if ON
#define HS_DEBUG ON
#else
#define HS_DEBUG OFF
#endif
// Customer ID
#define _CID _A // AWT
#define TH0701024600NYR50S1 OFF

/// FW version
#if (TH0701024600NYR50S1 == ON)
#define FWVER 0x1001
#define FWVER1 0x01
#define FWVER2 0x00
#else
#define FWVER 0x0013
#define FWVER1 0x01
#define FWVER2 0x00
#endif

// ISP init params
#define _EN_VS4210_Modex26
#define _EN_VS4210_Mode88
#define HDMI_IN_NUMBER 4
#define RGBGAINOSD OFF
#define TPMSOSD OFF
#define TOUCH_FUNCTION OFF
#define Menu_Osd_Time_Out 15
#define Volume_Osd_Time_Out 3 + 3
#define Channel_Osd_Time_Out 2

#define StartPowerBlackScreen ON
#define BOOT_SET_VX8812_FIRST ON
#define RECALL_SORUCE_RESET OFF

//#define OSD_10SEC_OFF
/// RUN_DIMMER
#define _DIMMER_STOP 0
#define _DIMMER_READY 1
#define _DIMMER_RUN1 2
#define _DIMMER_RUN2 3

#define _BACKLIGHT_ON_Time_out 0.5
#define _ONLY_BACKLIGHT_ON_Time_out 0.01 // 1

#define COC_RX OFF // ON//OFF

// MCU def
#define EEPROM_24C02_addr 0xA0

#define VS4210_INIT_NUMBER 88
#define VS4210_INIT_MIN_NUM 26

// source video of TP2824
#define SV1 1
#define SV2 3
#define SV3 4
#define SV4 2
#define SV0 0

#ifndef TRUE
#define TRUE 1
#define _TRUE 1

#define MUTE 1
#define MENU 2
#define TVVOL 4
#define _JUMP_OSD 8
#define _GAUGE_OSD 0x10
#define _COC_OSD 0x20
#define _RUN_TPMS_OSD 0x40
#define _GRABBER_LINER_OSD 0x80
#define _VERSION_OSD 0x40
#define _ALL_OSD 0xFF

#define UP 1
#define DN 2
#define RIGHT 3
#define LEFT 4

// Type of MenuItem
#define OSDMENU 1
#define CHOICEMENU 2
#define ACTION 3
#define BAR 4
#define NUMBER 5
#define CHOICE 6
#define ENUM_STRING 7 // YESNO			7
#define CHOICE1 8
#define TIME 9
#define NUMBER_STRING 10
#define TEXT_STR 11 // V1.1 02/28/07 simonsung
#define INFORMATION 12
#define EXIT 13

#define _TouchAddr0 54
#define _TouchAddr1 79
#define _TouchAddr2 104
#define _TouchAddr3 129

#define _TouchAddr4 154
#define _TouchAddr5 179
#define _TouchAddr6 204
#define _TouchAddr7 229

#define _VolDecAddr 255 // 270
#define _VolIncAddr 268 // 273

#define _MenuDecAddr 233
#define _MenuIncAddr 243

// COC OSD font address
#define COC_LeftAddr 48
#define COC_RightAddr (COC_LeftAddr + 50)
#define COC_UpAddr (COC_RightAddr + 50)
#define COC_DownAddr (COC_UpAddr + 50)
#define COC_EnterAddr (COC_DownAddr + 50)

#define BG_COLOR_BLACK 0x00 // Transparent //For background color of window, border, and char.
#define BG_COLOR_Orange 0x40
#define BG_COLOR_Med_AQUA 0x20
#define BG_COLOR_Silver 0x60
#define BG_COLOR_Royal_Blue 0x10
#define BG_COLOR_Hot_Pink 0x50
#define BG_COLOR_Light_Green 0x30
#define BG_COLOR_Gray 0x70

#define BG_COLOR_MASK 0xf0

#define CH_COLOR_BLACK 0x00 // For color of char.
#define CH_COLOR_RED 0x04
#define CH_COLOR_GREEN 0x02
#define CH_COLOR_YELLOW 0x06
#define CH_COLOR_BLUE 0x01
#define CH_COLOR_FUCHSIA 0x05
#define CH_COLOR_AQUA 0x03
#define CH_COLOR_WHITE 0x07
#define CH_COLOR_TRANS 0x08

#define VS4210_CONTRAST 0x32
#define VS4210_BRIGHTNESS 0x31
#define VS4210_SATURATION 0x33
#define VS4210_SHARPNESS 0x4E
#define VS4210_HUE 0x34

#define VIN1 0x01
#define VIN2 0x02
#define VIN3 0x04
#define VIN4 0x08

#endif

#ifndef FALSE
#define FALSE 0
#define _FALSE 0
#endif

#define Fisheye_save 1
#define Fisheye_no_save 0
#define SelectCameraR 3
#define SelectCameraD 4

//#define _EN_EEPROM_Save

//#define VS4210_ADDR 0x40
#define VS4210_INIT_NUMBER 88
#define VS4210_INIT_MIN_NUM 26

// OSD-------------------------------------------------------------------------
#define _TitleOn 0x04    // xxxxx100
#define _ContentOn 0x02  // xxxxx010
#define _BottomOn 0x01   // xxxxx001
#define _TurnoffAll 0x00 // xxxxx000

#define _EN_DRAWBOX_COLOR_BLACK 0x70
#define _EN_DRAWBOX_COLOR_Blue 0x60
#define _EN_DRAWBOX_COLOR_Green 0x30
#define _EN_DRAWBOX_COLOR_Aqua 0x20
#define _EN_DRAWBOX_COLOR_Red 0x50
#define _EN_DRAWBOX_COLOR_Fuchsia 0x40
#define _EN_DRAWBOX_COLOR_Yellow 0x10
#define _EN_DRAWBOX_COLOR_White 0x00

// COMMAND OSD
#define CMD_FW 1
#define CMD_OSD_MENU 2
#define CMD_OSD_UP 3
#define CMD_OSD_DOWN 4
#define CMD_OSD_LEFT 5
#define CMD_OSD_RIGHT 6
#define CMD_OSD_SELECT 7
#define CMD_OSD_JUMP 8

#ifdef __GNUC__
// typedef unsigned char bit;
typedef int bit;
// typedef bool BOOL ;
typedef unsigned char tByte;
typedef unsigned short tWord;
typedef unsigned long tLong;
typedef unsigned char tiByte;
typedef unsigned short tiWord;
typedef unsigned long tiLong;
typedef const unsigned char tcByte;
typedef const unsigned short tcWord;
typedef unsigned char txByte;
typedef unsigned short txWord;
typedef unsigned int tDWord;
typedef unsigned long txLong;

typedef unsigned char Register;
typedef unsigned char BYTE;
// typedef	unsigned short	WORD;
typedef unsigned int WORD;
typedef unsigned long DWORD;

#else
// Type Define------------------------------------------------------------------------
typedef unsigned char tByte;
typedef unsigned int tWord;
typedef unsigned long tLong;
typedef unsigned char idata tiByte;
typedef unsigned int idata tiWord;
typedef unsigned long idata tiLong;
typedef unsigned char code tcByte;
typedef unsigned int code tcWord;
typedef unsigned char xdata txByte;
typedef unsigned int xdata txWord;
typedef unsigned long xdata txLong;

typedef unsigned char Register;
typedef unsigned char BYTE;
// typedef	unsigned short	WORD;
typedef unsigned int WORD;

typedef unsigned long DWORD;

#endif

//--------------------------------------------------
// Enumerations of Timer Events
//--------------------------------------------------
#define SEC(x) (1000 * (x))

typedef struct
{
    BYTE b4PowerAction : 4;
    BYTE b4PowerStatus : 4;
    BYTE b1GDIIsolationFlag : 1;
    BYTE b1TopIsolationFlag : 1;
    BYTE b1AnalogIsolationFlag : 1;
} StructPowerInfoType;

typedef struct
{
    BYTE b1EventValid : 1;
    BYTE b7EventID : 7;
    WORD usTime;
} StructTimerEventTableType;

typedef enum
{
    _USER_TIMER_EVENT_START,
    _USER_TIMER_EVENT_KEY_REPEAT_ENABLE = _USER_TIMER_EVENT_START,
    _USER_TIMER_EVENT_KEY_SCAN_READY,
    _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_INC,
    _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_DEC,
    _USER_TIMER_EVENT_OSD_GET_MCU_VERSION,
    _USER_TIMER_EVENT_OSD_MCU_ENTRY_SLEEP_MODE,
    _USER_TIMER_EVENT_OSD_DISTANCE_RESET,
    _USER_TIMER_EVENT_OSD_DVR_REBOOT,
    _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN,
    _USER_TIMER_EVENT_OSD_ENCODER_DEVIATION_SETTING,
    //
    _USER_TIMER_EVENT_OSD_SHOW_NO_CABLE,
    _USER_TIMER_EVENT_OSD_SHOW_NO_SIGNAL,
    _USER_TIMER_EVENT_OSD_SHOW_POWER_SAVING,
    _USER_TIMER_EVENT_GO_TO_POWER_SAVING,
    _USER_TIMER_EVENT_OSD_DISABLE,
    _USER_TIMER_EVENT_DISPLAY_DISABLE,
    _USER_TIMER_EVENT_Encoder_Loop,
    _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON,
    _USER_TIMER_EVENT_PANEL_POWER_OFF,
    _USER_TIMER_EVENT_BACKLIGHT_ON,
    _USER_TIMER_EVENT_SEQ_CHECK,
    _USER_TIMER_EVENT_DO_AUTO_COLOR,
    _USER_TIMER_EVENT_PANEL_USED_TIMER,
    _USER_TIMER_EVENT_HLWIN_TYPE5_MOVE,
    _USER_TIMER_EVENT_LED_POWER_SAVING,
    _USER_TIMER_EVENT_LIGHT_SENSER_ON,
    _USER_TIMER_EVENT_SHOW_BURN_IN, // ryan@20151230
    _USER_TIMER_EVENT_BURN_IN_ON,   // ryan@20151230
    _USER_TIMER_EVENT_CHECK,        // ryan@20151231
    _USER_TIMER_EVENT_DISPLAY_INFO, // ryan@20151231
    _USER_TIMER_EVENT_STANDBY_MODE,
    _USER_TIMER_EVENT_FACTORY_MODE, // ryan@20160113
    _USER_TIMER_EVENT_Monitor_ON,
    _USER_TIMER_EVENT_END,
    _USER_TIMER_EVENT_COUNT = _USER_TIMER_EVENT_END,
} EnumUserTimerEventID;

typedef enum
{
    _SYSTEM_TIMER_EVENT_START = _USER_TIMER_EVENT_COUNT,
    _SYSTEM_TIMER_EVENT_SOURCE_CLEAR_DETECTING = _SYSTEM_TIMER_EVENT_START,
    _SYSTEM_TIMER_EVENT_TEST,
    _SYSTEM_TIMER_CHECK_BAT_VOLTAGE,
    _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_POWER_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE,
    _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE,
    _SYSTEM_TIMER_EVENT_JUDGE_DVR_COMM,
    _SYSTEM_TIMER_EVENT_JUDGE_TW8836_COMM,
    _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO,
    _SYSTEM_TIMER_EVENT_GRN_ON,
    _SYSTEM_TIMER_EVENT_GRN_OFF,
    _SYSTEM_TIMER_EVENT_RED_ON,
    _SYSTEM_TIMER_EVENT_RED_OFF,
    _SYSTEM_TIMER_EVENT_GRN_BLINK,
    _SYSTEM_TIMER_EVENT_RED_BLINK,
    _SYSTEM_TIMER_EVENT_GRN_RED_BLINK,
    _SYSTEM_TIMER_EVENT_GRN_RED_ON,
    _SYSTEM_TIMER_EVENT_GRN_RED_OFF,
    _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK,
    _SYSTEM_TIMER_EVENT_LIGHT_SENSOR_CHECK,
    _SYSTEM_TIMER_EVENT_EQ_DETECT,
    _SYSTEM_TIMER_EVENT_KEY_CHECK,
    _SYSTEM_TIMER_EVENT_ADC_CHECK,
    _SYSTEM_TIMER_EVENT_TRIGGER_DELAY_RELEASE,
    _SYSTEM_TIMER_EVENT_BOOT_SETUP,
    _SYSTEM_TIMER_EVENT_CAMRD_CHANGE,
    _SYSTEM_TIMER_EVENT_BOOT_VIDEO_DETECT,
    _SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT,
    _SYSTEM_TIMER_EVENT_SET_VX8812_SD_Video_Out,
    _SYSTEM_TIMER_EVENT_TOUCH_ACTION_0,
    _SYSTEM_TIMER_EVENT_TOUCH_ACTION_1,
    _SYSTEM_TIMER_EVENT_TOUCH_ACTION_2,
    _SYSTEM_TIMER_EVENT_TOUCH_ACTION_3,
    _SYSTEM_TIMER_EVENT_TOUCH_ACTION_4,
    _SYSTEM_TIMER_EVENT_TOUCH_ACTION_5,
    _SYSTEM_TIMER_EVENT_TOUCH_SLIDER_TIME_OUT,
    _SYSTEM_TIMER_EVENT_VX8812_INIT,
    _SYSTEM_TIMER_EVENT_TVI_BOX_Set,
    _SYSTEM_TIMER_EVENT_GRABBER_SETUP,

    /*
    _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE,
    _SYSTEM_TIMER_EVENT_BATT_HIGH_CHARGE,
    _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE,
   */
    _SYSTEM_TIMER_EVENT_TP_PRES_DOWN,
    _SYSTEM_TIMER_EVENT_RESET_TOUCH_PANEL,
    _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER,
    _SYSTEM_TIMER_EVENT_AUTO_SLEEPING_TIMEOUT,

    _SYSTEM_TIMER_EVENT_END,
    _SYSTEM_TIMER_EVENT_COUNT,
} EnumSystemTimerEventID;

typedef enum
{
    _SCALER_TIMER_EVENT_START = _SYSTEM_TIMER_EVENT_COUNT,
    _SCALER_TIMER_EVENT_TMDS_HDMI_PACKET_DETECT = _SCALER_TIMER_EVENT_START,
    _SCALER_TIMER_EVENT_TMDS_VIDEO_DETECT,
    _SCALER_TIMER_EVENT_AUDIO_LOAD_STABLE_I_CODE,
    _SCALER_TIMER_EVENT_DP_HOTPLUG_ASSERTED,
    _SCALER_TIMER_EVENT_DP_HDCP_LONG_HOTPLUG_EVENT,
    _SCALER_TIMER_EVENT_DP_LINK_STATUS_IRQ,
    _SCALER_TIMER_EVENT_D0_PORT_DIGITAL_SWITCH,
    _SCALER_TIMER_EVENT_D1_PORT_DIGITAL_SWITCH,
    _SCALER_TIMER_EVENT_NR_DETECTION_FINISHED,
    _SCALER_TIMER_EVENT_PHASE_CALIBRATION,
    _SCALER_TIMER_EVENT_PHASE_CNT_CHECK,
    _SCALER_TIMER_EVENT_MHL_D0_READY_TO_TRANSMIT,
    _SCALER_TIMER_EVENT_MHL_D0_READY_TO_RESEND,
    _SCALER_TIMER_EVENT_MHL_D0_RECONNECT_1K,
    _SCALER_TIMER_EVENT_MHL_D0_ACTIVE_VBUS,
    _SCALER_TIMER_EVENT_MHL_D1_READY_TO_TRANSMIT,
    _SCALER_TIMER_EVENT_MHL_D1_READY_TO_RESEND,
    _SCALER_TIMER_EVENT_MHL_D1_RECONNECT_1K,
    _SCALER_TIMER_EVENT_MHL_D1_ACTIVE_VBUS,
    _SCALER_TIMER_EVENT_DCC_HISTOGRAM_THD_CHANGE,
    _SCALER_TIMER_EVENT_END,
    _SCALER_TIMER_EVENT_COUNT = _SCALER_TIMER_EVENT_END,
} EnumMCUTimerEventID;

//#define _SIMULTANEOUS_SCALER_EVENT_COUNT       0//     (5 + (_D0_INPUT_PORT_TYPE == _D0_MHL_PORT) + (_D1_INPUT_PORT_TYPE == _D1_MHL_PORT))
#define _SIMULTANEOUS_USER_EVENT_COUNT 5
#define _SIMULTANEOUS_SYSTEM_EVENT_COUNT 5

//#define _TIMER_EVENT_COUNT    (_SIMULTANEOUS_USER_EVENT_COUNT + _SIMULTANEOUS_SYSTEM_EVENT_COUNT + _SIMULTANEOUS_SCALER_EVENT_COUNT)
#define _TIMER_EVENT_COUNT (_SIMULTANEOUS_USER_EVENT_COUNT + _SIMULTANEOUS_SYSTEM_EVENT_COUNT)

typedef enum
{

#if 1
    VS4210_1080P_MODE0_w0 = 0x00,
    VS4210_1080P_MODE0_w1 = 0x01,
    VS4210_1080P_MODE0_w2 = 0x02,
    VS4210_1080P_MODE0_w3 = 0x03,
    VS4210_1080P_MODE0_w4 = 0x04,
    VS4210_1080P_MODE1 = 0x10,
    VS4210_1080P_MODE2 = 0x20,
    VS4210_1080P_MODE8 = 0x30,
    VS4210_1080P_MODE9 = 0x40,
    VS4210_1080P_MODE10 = 0x50,
    VS4210_1080P_MODE12 = 0x60,
    VS4210_1080P_MODE14 = 0x70,
    VS4210_1080P_MODE_NUMBER = 0x90,
    VS4210_1080P_MODE_SystemPowerDown = 0xF0,
    VS4210_1080P_MODE_SystemPowerResume = 0xF1

#else
    VS4210_1080P_MODE0_w0 = 0x00,
    VS4210_1080P_MODE0_w1 = 0x01,
    VS4210_1080P_MODE0_w2 = 0x02,
    VS4210_1080P_MODE0_w3 = 0x03,
    VS4210_1080P_MODE1 = 0x10,
    //     VS4210_1080P_MODE2    = 0x20,
    VS4210_1080P_MODE10 = 0x25,
    VS4210_1080P_MODE8 = 0x30,
    VS4210_1080P_MODE9 = 0x40,
    VS4210_1080P_MODE12 = 0x50,
    VS4210_1080P_MODE14 = 0x60,
    VS4210_1080P_MODE_NUMBER = 0x90,
    VS4210_1080P_MODE_SystemPowerDown = 0xF0,
    VS4210_1080P_MODE_SystemPowerResume = 0xF1
#endif
} HDMI_OUTPUT_MODE;

typedef enum
{
    VS4210_LOOP_MODE0_w0 = 0,
    VS4210_LOOP_MODE0_w1,   /// 1
    VS4210_LOOP_MODE0_w2,   /// 2
    VS4210_LOOP_MODE0_w3,   /// 3
    VS4210_LOOP_MODE0_w4,   // 4///4
    VS4210_LOOP_MODE1,      /// 5
    VS4210_LOOP_MODE2,      ///
    VS4210_LOOP_MODE10_123, ///
    VS4210_LOOP_MODE8,      ///
    VS4210_LOOP_MODE9,      ///
    VS4210_LOOP_MODE12_12,  /// 10
    VS4210_LOOP_MODE12_34,  ///
    VS4210_LOOP_MODE12_14,  ///
    VS4210_LOOP_MODE12_41,  ///
    VS4210_LOOP_MODE12_42,  ///
    VS4210_LOOP_MODE12_43,  ///
    VS4210_LOOP_MODE12_21,  ///
    VS4210_LOOP_MODE12_23,  ///
    VS4210_LOOP_MODE12_24,  ///
    VS4210_LOOP_MODE12_13,  ///
    VS4210_LOOP_MODE12_31,  ///
    VS4210_LOOP_MODE12_32,  /// 20
    VS4210_LOOP_MODE8_412,  /// 20
    VS4210_LOOP_MODE8_123,  /// 21
    VS4210_LOOP_MODE8_124,  /// 22
    VS4210_LOOP_MODE8_134,  /// 16
    VS4210_LOOP_MODE8_213,  /// 16
    VS4210_LOOP_MODE8_214,  /// 16
    VS4210_LOOP_MODE8_234,  /// 16
    VS4210_LOOP_MODE8_312,  /// 16
    VS4210_LOOP_MODE8_314,  /// 16
    VS4210_LOOP_MODE8_324,  /// 16
    VS4210_LOOP_MODE10_412, /// 6
    VS4210_LOOP_MODE10_413, /// 6
    VS4210_LOOP_MODE10_423, /// 6
    VS4210_LOOP_MODE8_413,  /// 16
    VS4210_LOOP_MODE8_423,  /// 16
    VS4210_LOOP_MODE14,
    VS4210_LOOP_MODE9_412,
    VS4210_LOOP_MODE9_123,
    VS4210_LOOP_MODE9_124,
    VS4210_LOOP_MODE9_134,
    VS4210_LOOP_MODE9_213,
    VS4210_LOOP_MODE9_214,
    VS4210_LOOP_MODE9_234,
    VS4210_LOOP_MODE9_312,
    VS4210_LOOP_MODE9_314,
    VS4210_LOOP_MODE9_324,
    //		VS4210_LOOP_MODE10_123 ,
    VS4210_LOOP_MODE10_124,
    VS4210_LOOP_MODE10_134,
    VS4210_LOOP_MODE10_213,
    VS4210_LOOP_MODE10_214,
    VS4210_LOOP_MODE10_234,
    VS4210_LOOP_MODE10_312,
    VS4210_LOOP_MODE10_314,
    VS4210_LOOP_MODE10_324,
    VS4210_LOOP_MODE_ERROR,
    VS4210_LOOP_MODE_NUMBER
} VS4210_OUTPUT_MODE_LOOP;

typedef enum
{

#if 1
    VS4210_FULL_MODE0 = 0x00,
    VS4210_FULL_MODE1 = 0x10,
    VS4210_FULL_MODE2 = 0x20,
    VS4210_FULL_MODE8 = 0x30,
    VS4210_FULL_MODE9 = 0x40,
    VS4210_FULL_MODE10 = 0x50,
    VS4210_FULL_MODE12 = 0x60,
    VS4210_FULL_MODE14 = 0x70,
    VS4210_FULL_MODE_NUMBER = 0x90,
    VS4210_SystemPowerDown = 0xF0,
    VS4210_SystemPowerResume = 0xF1

#else
    VS4210_FULL_MODE0 = 0x00,
    VS4210_FULL_MODE1 = 0x10,
    //   VS4210_FULL_MODE2 =0x20,
    VS4210_FULL_MODE10 = 0x20,
    VS4210_FULL_MODE8 = 0x30,
    VS4210_FULL_MODE9 = 0x40,
    VS4210_FULL_MODE12 = 0x50,
    VS4210_FULL_MODE14 = 0x60,
    VS4210_FULL_MODE_NUMBER = 0x90,
    VS4210_SystemPowerDown = 0xF0,
    VS4210_SystemPowerResume = 0xF1
#endif
} HDMI_FULL_OUTPUT_MODE;

typedef enum
{
    HDMI_AUDIO_0 = 0,
    HDMI_AUDIO_1,
    HDMI_AUDIO_2,
    HDMI_AUDIO_3,
    HDMI_AUDIO_NONE,
    HDMI_AUDIO_NUMBER
} HDMI_AUDIO_SELECT;

typedef enum
{
    _OSD_720X480I60 = 0, // HDMI
    _OSD_720X576I50,
    _OSD_720X480P60,
    _OSD_720X576P50,
    _OSD_1920X1080I,
    _OSD_1280X720P, // 5
    _OSD_1920X1080P,
    _OSD_No_Signal,
    _OSD_OUT_OF_RANGE
} HDMI_INPUT_SIZE;

typedef enum
{
    _OUTPUT_1080P60 = 0,
    _OUTPUT_720P60,
    _OUTPUT_800x480,
    _OUTPUT_NUMBER
} OUTPUT_SIZE;

typedef struct
{
    tByte WMap0;
    tByte WMap1;
    tByte WMap2;
    tByte WMap3;
} tWindowMap;

typedef struct
{
    tByte OutputMode;
    tWindowMap WindowMap;
    tByte AudioChannel;
    tByte OutputSize;
    tByte DrawOSDFlg;
    tByte DrawBoxFlg;
    tByte DrawBoxColor;
} T_VS4210_MutiWindows;

#endif
