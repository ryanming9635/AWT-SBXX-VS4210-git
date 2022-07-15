/*****************************************************************************/
/*                                                                           										*/
/*  VS4210   MCU                    													*/
/*                                                                           										*/
/* Etc_eep.h                                                           										*/
/*                                                                           										*/
/*****************************************************************************/
#ifndef __ETC_EEP__
#define __ETC_EEP__

////eeprom.h
//#define  EEP_Powerflag          			0x10
//#define  EEP_CameraVolt          			0x11
//#define	EEP_DC12_PWR_START		0x12

#define EEP_FWREV_MAJOR 0x04  // BYTE	1	F/W Rev.-major(Hex)
#define EEP_FWREV_MINOR 0x05  // BYTE	1	F/W Rev.-minor(Hex)
#define EEP_FWREV_MINOR2 0x06 // BYTE	1	F/W Rev.-minor(Hex)
#define EEP_DEBUGLEVEL 0x07   // BYTE	1	DebugLevel

#define EEP_AUTODETECT 0x07     // BYTE	1	Flag for Input Auto Detect	-0:Auto, 1:NTSC,....
#define EEP_AUTODETECTTYTE 0x08 // BYTE	1	Type of Auto-detect(will be value of register 0x1d)
//			---------------------------------------------------------------
#define EEP_WIDEMODE 0x09 // BYTE	1   Wide Mode

#define EEP_AUTORECOGNITION 0x0a // BYTE  1	Auto Recognition
#define EEP_VIDEOMODE 0x0b       // BYTE  1   Video Mode

#define EEP_OSDLANG 0x0c // BYTE  1   OSDLang						// 0

#define EEP_OSDPOSITIONMODE 0x0d // BYTE	1	OSD Position Mode
#define EEP_CCD 0x0e             // BYTE	1	Closed Caption-	0: off, 1:on

#define EEP_INPUTSELECTION 0x0f // BYTE	1	InputSelection

//
#define EEP_PC_CONTRAST 0x10
#define EEP_PC_BRIGHT 0x11

#define EEP_CONTRAST 0x12     // BYTE	1	Contrast
#define EEP_BRIGHTNESS 0x13   // BYTE	1	Brightness
#define EEP_SATURATION_U 0x14 // BYTE	1	Saturation :U
#define EEP_SATURATION_V 0x15 // BYTE	1	Saturation :V
#define EEP_SHARPNESS 0x16    // BYTE	1	Sharpness
#define EEP_HUE 0x17          // BYTE	1	Hue
#define EEP_BACKLIGHT 0x18    // BYTE	1	BACKLIGHT

//====================================HS AWT 981001
#define EEP_AutoMode 0xa0
#define EEP_IMAGEMode 0xa1
#define EEP_RearMode 0xa2
#define EEP_COSDMode 0xa3
#define EEP_GAUGEMode 0xa4
#define EEP_CAMAMode 0xa5
#define EEP_CAMBMode 0xa6
#define EEP_CAMCMode 0xa7
#define EEP_CAMRMode 0xa8
#define EEP_CAMDMode 0x19

#define EEP_CAMOutMode 0xa9
#define EEP_TimeStep 0xaa
#define EEP_Powerflag 0xab
#define EEP_ONOFFChoice 0xac

#define EEP_PRIORITY 0xad
#define EEP_MenuTouch 0x20

#define EEP_JUMPMode 0xae
#define EEP_NowModeState 0xaf
#define EEP_SelectMode 0xb0
#define EEP_PriorityOffSelectMode 0xb1
#define EEP_AudioRecord 0xb2

#define EEP_FirstNumber 0xb3
#define EEP_SecondNumber 0xb4
#define EEP_ThridNumber 0xb5
#define EEP_FourthNumber 0xb6

#define EEP_CAMNUMBER 0xb7
#define EEP_PELCO 0xb8
#define EEP_Baud_rate 0xb9
#define EEP_TriggerVGA 0xba
#define EEP_DVRDisplay 0xbb
#define EEP_IMAGE_A_Mode 0xbc // andy A1.4 20100113
#define EEP_IMAGE_B_Mode 0xbd // andy A1.4 20100113
#define EEP_IMAGE_C_Mode 0xbe // andy A1.4 20100113

#define EEP_Auto_Sleeping 0xbf      // ryan@20210304
#define EEP_Triple_Screen_Type 0x20 // ryan@20210304

#define EEP_Distance_Gauge_Position 0x21 // ryan@20210304
#define EEP_Distance_Gauge_Width 0x22    // ryan@20210304

#define EEP_Grabber_Liner 0x23
#define EEP_Grabber_Liner_Width 0x24
#define EEP_Grabber_Liner_Position 0x25

//#ifdef HTRV
//#define  EEP_Gauge_VStart  0x211   //WILLIAM-970318
// static  BYTE GaugeVStart;
//#endif

#define EEP_DELAYCAMA 0xC0
#define EEP_DELAYCAMB 0xC1
#define EEP_DELAYCAMC 0xC2
#define EEP_DELAYCAMR 0xC3
#define EEP_DELAYCAMD 0xC4
//#ifdef USE_CAMD
//#define  EEP_DELAYCAMD           0xC4
//#endif
//#define  EEP_SelectModeBuf         0xb2
#define EEP_ReverseMode 0xc5
#define EEP_JUMPAV_TIMESET 0xc6

#define EEP_TP2824_STD0 0xC7
#define EEP_TP2824_MODE0 0xC8
#define EEP_TP2824_SAVE0 0xC9

#define EEP_TP2824_STD1 0xCA
#define EEP_TP2824_MODE1 0xCB
#define EEP_TP2824_SAVE1 0xCC

#define EEP_TP2824_STD2 0xCD
#define EEP_TP2824_MODE2 0xCE
#define EEP_TP2824_SAVE2 0xCF

#define EEP_TP2824_STD3 0xD0
#define EEP_TP2824_MODE3 0xD1
#define EEP_TP2824_SAVE3 0xD2

#define EEP_TP2824_STD4 0xD3
#define EEP_TP2824_MODE4 0xD4
#define EEP_TP2824_SAVE4 0xD5

#define EEP_FlishCAM1 0xD8
#define EEP_FlishCAM2 0xD9
#define EEP_FlishCAM3 0xDA
#define EEP_FlishCAM4 0xDB

#define EEP_TIRE1_High_PSI 0xDC
#define EEP_TIRE1_Low_PSI 0xDD

#define EEP_TIRE2_High_PSI 0xDE
#define EEP_TIRE2_Low_PSI 0xDF

#define EEP_TIRE3_High_PSI 0xE0
#define EEP_TIRE3_Low_PSI 0xE1

#define EEP_TIRE4_High_PSI 0xE2
#define EEP_TIRE4_Low_PSI 0xE3

#define EEP_TIRE5_High_PSI 0xE4
#define EEP_TIRE5_Low_PSI 0xE5

#define EEP_TIRE6_High_PSI 0xE6
#define EEP_TIRE6_Low_PSI 0xE7

#define EEP_TIRE1_High_Temp 0xE8
#define EEP_TIRE2_High_Temp 0xE9
#define EEP_TIRE3_High_Temp 0xEA
#define EEP_TIRE4_High_Temp 0xEB
#define EEP_TIRE5_High_Temp 0xEC
#define EEP_TIRE6_High_Temp 0xED

#if 0
#define EEP_Resolution 0xc0
#define EEP_FPS 0xc1
#define EEP_Quality 0xc2
#define EEP_RecordMode 0xc3
#define EEP_RecordLength 0xc4
#define EEP_PreAlarmLength 0xc5
#define EEP_PostAlarmLength 0xc6
#endif
//====================================HS AWT 981001

#define EEP_PCAUDIOPVOL 0x37  // BYTE	1   AudioVol
#define EEP_AUDIOPVOL 0x38    // BYTE	1   AudioVol
#define EEP_AUDIOBALANCE 0x39 // BYTE	1   AudioBalance

//-----------------------------------------------------------------------------
//		I2C Device Access Address Mapping
//-----------------------------------------------------------------------------

#define EEPROMI2CAddress 0xa0 // EEPROM (24C16)
//#define TW88I2CAddress		0x8a	// TW880x
//#define  AD5110I2CAddress    	0x5E//0x58//0x5E//0x58
extern void WriteEEP(WORD index, BYTE dat);
extern BYTE ReadEEP(WORD index);

WORD GetFWRevEE(void);
void SaveFWRevEE(WORD);

void SaveDebugLevelEE(BYTE);
void ClearBasicEE(void);
void ResetOSDValue(void);

extern void LoadEEPROM(void);
extern BYTE ChangeVol(char amount);
extern void LoadVideoData(void);
extern BYTE CloseOSDMenu(void);
extern void TPMS_Default_Setting(void);
extern void VS4210_Rx_NoSignal(tByte ch);
extern void Set_VidRes(tByte ch);
extern void Init_TP280x_RegSet(void);
extern void CheckCAMDInputVideo(BYTE index);

extern void SETChannelReset(BYTE index);

#endif // __ETC_EEP__
