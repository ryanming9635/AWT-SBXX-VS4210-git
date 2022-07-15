/*****************************************************************************/
/*                                                                           										*/
/*  VS4210   MCU                    													*/
/*                                                                           										*/
/*  Etc_eep.c                                                            									*/
/*                                                                           										*/
/*****************************************************************************/

// eeprom.c
#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"

//#include "Config.h"
//#include "typedefs.h"
#include "main.h"
//#include "i2c.h"
#include "printf.h"
#include "etc_eep.h"
#include "keyremo.h"

BYTE bytResetOSDValue = 0;

extern tByte DayNightLevel;
extern BYTE AudioVol;
extern BYTE Priority;
extern BYTE OSD_LOCKF;
extern BYTE SEQFlag;
extern BYTE NowModeState;
extern BYTE SelectModeType;
extern BYTE MenuTouchSel;
extern BYTE PreSelectModeType; // VS4210_LOOP_MODE1
extern tByte VideoSave[4];
extern BYTE Flisheye_CAM[4];
#if (TPMSOSD == ON)
extern BYTE TP_HIGH_PSI[6];
extern BYTE TP_LOW_PSI[6];
extern BYTE TP_HIGH_TEMP[6];
#endif

extern BYTE bytGarbberLinerPosition;
extern WORD bytSetGarbberLinerPosition;
extern BYTE bytGarbberLinerWidth;
extern BYTE mode[4];
extern BYTE state[4];
extern BYTE count[4];
extern BYTE VideoChangeFlag;
extern BYTE bytSelectCAMRD;
extern BYTE bytDistanceGaugePosition;
extern BYTE bytDistanceGaugeWidth;
extern BYTE RUN_DIMMER;
extern tByte bSignal[4];
extern BYTE REV_ERROR;

#if 0
void WriteEEP(WORD index, BYTE dat)
{
//	BYTE addr;

//	addr = (EEPROMI2CAddress | (index>>7)) & 0xfe;
/*
	I2CStart();
	I2CWriteData(addr);
	I2CWriteData((BYTE)index);
	I2CWriteData(dat);
	I2CStop();
*/
//	#ifdef DEBUG_EEP
//		dPrintf("Write %02xh to [Adrs(%02xh)Index(%02xh)\r\n", (WORD)dat, (WORD)addr, (WORD)index );
//	#endif

	delay(2);
}

BYTE ReadEEP(WORD index)
{
	BYTE val=_FALSE;
//	BYTE addr;

//	addr = (EEPROMI2CAddress | (index>>7)) & 0xfe;
/*
	val = ReadI2C(addr,(BYTE)index);
*/
//	#ifdef DEBUG_EEP
//		dPrintf("Read [Adrs:%02xh,Index:%02xh] %02xh\r\n", (WORD)addr, (WORD)index, (WORD)val);
//	#endif
	delay(2);
	return val;
}
#endif
WORD GetFWRevEE()
{
	WORD rev = 0;

	if (ReadEEP(0) != '0')
		return 0;
	if (ReadEEP(1) != '7')
		return 0;
	if (ReadEEP(2) != '2')
		return 0;
	if (ReadEEP(3) != 'Q')
		return 0;

	rev = (ReadEEP(EEP_FWREV_MAJOR)) << 8;
	rev |= ReadEEP(EEP_FWREV_MINOR);

	return rev;
	// return 0x0001;
}

void SaveFWRevEE(WORD rev)
{
	WriteEEP(0, '0');
	WriteEEP(1, '7');
	WriteEEP(2, '2');
	WriteEEP(3, 'Q');
	WriteEEP(EEP_FWREV_MAJOR, (BYTE)(rev >> 8));
	WriteEEP(EEP_FWREV_MINOR, (BYTE)rev);
}

#if 0 /// (_DEBUG_MESSAGE_Monitor==ON)

void SaveDebugLevelEE(BYTE dl)
{
	WriteEEP(EEP_DEBUGLEVEL, dl);
}
#endif

BYTE GetDebugLevelEE(void)
{
	return ReadEEP(EEP_DEBUGLEVEL);
}

//===================================================================
void ResetOSDValue(void)
{
	bytResetOSDValue = 1;

	Printf("\r\nReset OSD Data..");

	// BackLightOFF();

	WriteEEP(EEP_AutoMode, 0x00); // Auto Day&Night...... OFF

	WriteEEP(EEP_CONTRAST, 50);
	WriteEEP(EEP_BRIGHTNESS, 50);
	WriteEEP(EEP_SATURATION_U, 50);
	WriteEEP(EEP_SHARPNESS, 50);
	WriteEEP(EEP_HUE, 50);
	WriteEEP(EEP_BACKLIGHT, 100);

	// WriteEEP(EEP_PRIORITY,0);
	DayNightLevel = ReadEEP(EEP_AutoMode);
	WriteEEP(EEP_IMAGEMode, 0x00);	// IMAGEMode..... SINGLE
	WriteEEP(EEP_RearMode, 0x00);	// RearMode...... R
	WriteEEP(EEP_TriggerVGA, 0x00); // trigger vga...... on
	WriteEEP(EEP_GAUGEMode, 0x01);	// ­Ë¨®OSD....... ON
	WriteEEP(EEP_Powerflag, 0x01);	// POWER FLAG....... ON
	WriteEEP(EEP_COSDMode, 0x01);	// Display OSD... ON
	WriteEEP(EEP_DVRDisplay, 0x00); // DVR Display OSD... ON
	WriteEEP(EEP_CAMAMode, 0x00);	// MirrorA....... OFF
	WriteEEP(EEP_CAMBMode, 0x00);	// MirrorB....... OFF
	WriteEEP(EEP_CAMCMode, 0x00);	// MirrorC....... OFF
	WriteEEP(EEP_CAMRMode, 0x01);	// MirrorR....... ON
	WriteEEP(EEP_CAMDMode, 0x01);	// MirrorD....... ON

	WriteEEP(EEP_AUDIOPVOL, 50); // AudioVol

	WriteEEP(EEP_ONOFFChoice, 0x00); // osd lock oFF
	WriteEEP(EEP_MenuTouch, 0x00);	 // Menu Touch select....... OFF

	WriteEEP(EEP_Auto_Sleeping, 0x00); // Auto sleeping=OFF
	WriteEEP(EEP_Triple_Screen_Type, 1);

	WriteEEP(EEP_Distance_Gauge_Position, 5);
	WriteEEP(EEP_Distance_Gauge_Width, 5);
	bytDistanceGaugePosition = 5;
	bytDistanceGaugeWidth = 5;

	WriteEEP(EEP_Grabber_Liner, OFF); // default OFF
	WriteEEP(EEP_Grabber_Liner_Position, 55 + 2 /*44-4*/);
	WriteEEP(EEP_Grabber_Liner_Width, 1);

	bytGarbberLinerWidth = 1;
	bytGarbberLinerPosition = 40;

	MenuTouchSel = 0;

	RUN_DIMMER = 0;

	AudioVol = 50;
	//  ChangeVol(0);
	WriteEEP(EEP_PCAUDIOPVOL, 50);
	WriteEEP(EEP_AudioRecord, 0x03); // dvr audio channel

	WriteEEP(EEP_IMAGE_A_Mode, 0x00);
	WriteEEP(EEP_IMAGE_B_Mode, 0x00);
	WriteEEP(EEP_IMAGE_C_Mode, 0x00);

	WriteEEP(EEP_DELAYCAMA, 0x02); // DELAY CAM A....... 2
	WriteEEP(EEP_DELAYCAMB, 0x02); // DELAY CAM B....... 2
	WriteEEP(EEP_DELAYCAMC, 0x02); // DELAY CAM C....... 2
	WriteEEP(EEP_DELAYCAMR, 0x02); // DELAY CAM R....... 2
	WriteEEP(EEP_DELAYCAMD, 0x02); // DELAY CAM D....... 2

	if (Priority == ON) // on
	{
		WriteEEP(EEP_JUMPMode, 0);		 // JUMPMode...... QUAL
		WriteEEP(EEP_ONOFFChoice, 0x00); // OSD	lock....... off
		OSD_LOCKF = 0;
	}
	else
	{
		WriteEEP(EEP_JUMPMode, 13);		 // JUMPMode...... CAM R
		WriteEEP(EEP_ONOFFChoice, 0x01); // OSD	lock....... ON
		OSD_LOCKF = 1;
	}
#if (RECALL_SORUCE_RESET == ON)
	// TP2824 video data reset
	WriteEEP(EEP_TP2824_STD0, 0xFF);
	WriteEEP(EEP_TP2824_MODE0, 0xFF);
	WriteEEP(EEP_TP2824_STD1, 0xFF);
	WriteEEP(EEP_TP2824_MODE1, 0xFF);
	WriteEEP(EEP_TP2824_STD2, 0xFF);
	WriteEEP(EEP_TP2824_MODE2, 0xFF);
	WriteEEP(EEP_TP2824_STD3, 0xFF);
	WriteEEP(EEP_TP2824_MODE3, 0xFF);
	WriteEEP(EEP_TP2824_STD4, 0xFF);
	WriteEEP(EEP_TP2824_MODE4, 0xFF);

	// WriteEEP(EEP_JUMPMode,0);

	WriteEEP(EEP_TP2824_STD0 + (0 * 3) + 2, 0);
	WriteEEP(EEP_TP2824_STD0 + (1 * 3) + 2, 0);
	WriteEEP(EEP_TP2824_STD0 + (2 * 3) + 2, 0);
	WriteEEP(EEP_TP2824_STD0 + (3 * 3) + 2, 0);
	WriteEEP(EEP_TP2824_STD0 + (4 * 3) + 2, 0);

	VideoSave[0] = 0;
	VideoSave[1] = 0;
	VideoSave[2] = 0;
	VideoSave[3] = 0;
#endif

	WriteEEP(EEP_FlishCAM1, 0);
	WriteEEP(EEP_FlishCAM2, 0);
	WriteEEP(EEP_FlishCAM3, 0);
	WriteEEP(EEP_FlishCAM4, 0);

	Flisheye_CAM[0] = 0;
	Flisheye_CAM[1] = 0;
	Flisheye_CAM[2] = 0;
	Flisheye_CAM[3] = 0;

#if (RECALL_SORUCE_RESET == ON)

	mode[0] = 0x07;
	mode[1] = 0x07;
	mode[2] = 0x07;
	mode[3] = 0x07;

	state[0] = 3;
	state[1] = 3;
	state[2] = 3;
	state[3] = 3;

	count[0] = 0;
	count[1] = 0;
	count[2] = 0;
	count[3] = 0;
#endif
	//	 Set_VidRes(0);
	//	 Set_VidRes(1);
	//	 Set_VidRes(2);
	//	 Set_VidRes(3);
	BackLightOFF();
/*
	 VS4210_Rx_NoSignal(0) ;
	 VS4210_Rx_NoSignal(1) ;
	 VS4210_Rx_NoSignal(2) ;
	 VS4210_Rx_NoSignal(3) ;
*/
#if (RECALL_SORUCE_RESET == ON)

	gbVXIS_InputMode[0] = _OSD_No_Signal;
	gbVXIS_InputMode[1] = _OSD_No_Signal;
	gbVXIS_InputMode[2] = _OSD_No_Signal;
	gbVXIS_InputMode[3] = _OSD_No_Signal;

	bSignal[0] = 0;
	bSignal[1] = 0;
	bSignal[2] = 0;
	bSignal[3] = 0;
#endif

#if (TPMSOSD == ON)

	TPMS_Default_Setting();
/*
	   WriteEEP(EEP_TIRE1_High_PSI,TIRE_HIGH_PSI);
	   WriteEEP(EEP_TIRE1_Low_PSI,TIRE_LOW_PSI);
	   WriteEEP(EEP_TIRE1_High_Temp,TIRE_HIGH_TEMP);

	   WriteEEP(EEP_TIRE2_High_PSI,TIRE_HIGH_PSI);
	   WriteEEP(EEP_TIRE2_Low_PSI,TIRE_LOW_PSI);
	   WriteEEP(EEP_TIRE2_High_Temp,TIRE_HIGH_TEMP);

	   WriteEEP(EEP_TIRE3_High_PSI,TIRE_HIGH_PSI);
	   WriteEEP(EEP_TIRE3_Low_PSI,TIRE_LOW_PSI);
	   WriteEEP(EEP_TIRE3_High_Temp,TIRE_HIGH_TEMP);

	   WriteEEP(EEP_TIRE4_High_PSI,TIRE_HIGH_PSI);
	   WriteEEP(EEP_TIRE4_Low_PSI,TIRE_LOW_PSI);
	   WriteEEP(EEP_TIRE4_High_Temp,TIRE_HIGH_TEMP);

	   WriteEEP(EEP_TIRE5_High_PSI,TIRE_HIGH_PSI);
	   WriteEEP(EEP_TIRE5_Low_PSI,TIRE_LOW_PSI);
	   WriteEEP(EEP_TIRE5_High_Temp,TIRE_HIGH_TEMP);

	   WriteEEP(EEP_TIRE6_High_PSI,TIRE_HIGH_PSI);
	   WriteEEP(EEP_TIRE6_Low_PSI,TIRE_LOW_PSI);
	   WriteEEP(EEP_TIRE6_High_Temp,TIRE_HIGH_TEMP);
*/
/*
	   TP_HIGH_PSI[0]=TIRE_HIGH_PSI;
	   TP_HIGH_PSI[1]=TIRE_HIGH_PSI;
	   TP_HIGH_PSI[2]=TIRE_HIGH_PSI;
	   TP_HIGH_PSI[3]=TIRE_HIGH_PSI;
	   TP_HIGH_PSI[4]=TIRE_HIGH_PSI;
	   TP_HIGH_PSI[5]=TIRE_HIGH_PSI;

	   TP_LOW_PSI[0]=TIRE_LOW_PSI;
	   TP_LOW_PSI[1]=TIRE_LOW_PSI;
	   TP_LOW_PSI[2]=TIRE_LOW_PSI;
	   TP_LOW_PSI[3]=TIRE_LOW_PSI;
	   TP_LOW_PSI[4]=TIRE_LOW_PSI;
	   TP_LOW_PSI[5]=TIRE_LOW_PSI;

	   TP_HIGH_TEMP[0]=TIRE_HIGH_TEMP;
	   TP_HIGH_TEMP[1]=TIRE_HIGH_TEMP;
	   TP_HIGH_TEMP[2]=TIRE_HIGH_TEMP;
	   TP_HIGH_TEMP[3]=TIRE_HIGH_TEMP;
	   TP_HIGH_TEMP[4]=TIRE_HIGH_TEMP;
	   TP_HIGH_TEMP[5]=TIRE_HIGH_TEMP;
	   */
#endif
	SEQFlag = 0;
	//#ifdef AWT_ML072S
	//	   WriteEEP(EEP_TimeStep,0x03); 	 // TimerStep.....02
	//#else
	WriteEEP(EEP_TimeStep, 0x02);	// TimerStep.....02
									//#endif
	WriteEEP(EEP_CAMOutMode, 0x00); // CAMOut........ QUAL
									//	   CAMOutDSel( 0x00 );	///V1.1 02/27/07 simonsung
									//	   TW2835Cmd("\n\rmode 0\n\r");

	//	   ClearDataCnt(INPUTINFO_ADDR, 51);   //william-980416

	LoadEEPROM();

	NowModeState = SelectMode;
	SelectModeType = CAM_A;

	WriteEEP(EEP_NowModeState, SelectMode);

	WriteEEP(EEP_SelectMode, SelectModeType);

	CloseOSDMenu();

	//	 setloopoutmode(SelectModeType) ;

	LoadVideoData();

#if (StartPowerBlackScreen == ON)
	user_pwm_setvalue(GetBackLightLevel());
#endif

	PreSelectModeType = 12;

#if (RECALL_SORUCE_RESET == ON)
	Init_TP280x_RegSet();
#endif

#if (RECALL_SORUCE_RESET == OFF)
	/// Only Reset CAM D video..
	// Printf("\r\n(CAM D Reset START)");
	WriteEEP(EEP_TP2824_STD4, 0xFF);
	WriteEEP(EEP_TP2824_MODE4, 0xFF);
	WriteEEP(EEP_TP2824_SAVE4, 0xFF);
	if (REV_ERROR == 0)
	{
		SETChannelReset(3);

		CheckCAMDInputVideo(4);
	}
	// Printf("\r\n(CAM D Reset END)");
#endif
	// setloopoutmode(VS4210_LOOP_MODE0_w4);  //display CAM D
	SetNowMode();

#if (RECALL_SORUCE_RESET == ON)
	CheckCAMDInputVideo(4);

	mode[3] = 0x07;
	state[3] = 0;
	count[3] = 0;
	VideoSave[3] = 0;
	SETChannelReset(3);

	CheckCAMDInputVideo(3);

#endif
	MCUTimerActiveTimerEvent(SEC(0.5), _USER_TIMER_EVENT_BACKLIGHT_ON);

	Printf("\r\nEnd Reset OSD Data");
}

void ClearBasicEE(void)
{

	Printf("\r\nClearBasicEE...");

	WriteEEP(EEP_PRIORITY, 0x01); // PRIORITY.........OFF	Andy-980602
	Priority = 1;

	//	SaveVInputStdDetectModeEE(0);	// AUTO
	//	WriteEEP(EEP_CCD, 0);	// CC = OFF

	ResetOSDValue(); // Pinchi 20150212 enable

	//	ResetVideoValue();

	/*
		WriteEEP(EEP_Powerflag,ON);

		WriteEEP(EEP_CameraVolt, 14);  //7.6V

		WriteEEP(EEP_DEBUGLEVEL,OFF);

		WriteEEP(EEP_DC12_PWR_START, OFF);
	*/
}

//=========  Audio =================================================
BYTE GetAudioVolEE(void)
{
	return ReadEEP(EEP_AUDIOPVOL);
}

void SetAudioVolEE(BYTE vol)
{
	WriteEEP(EEP_AUDIOPVOL, vol);
}

BYTE GetPCAudioVolEE(void)
{
	return ReadEEP(EEP_PCAUDIOPVOL);
}
void SetPCAudioVolEE(BYTE vol)
{
	WriteEEP(EEP_PCAUDIOPVOL, vol);
}
