/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  SysTimer.c                                          											*/
/*                                                                           										*/
/*****************************************************************************/

#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"
#include "VX_OSD_Simple.h"

//#include "config.h"

//#ifdef SERIAL

//#include "typedefs.h"
#include "main.h"
//#include "i2c.h"
//#include "reg.h"
#include "printf.h"
//#include "etc_eep.h"
#include "monitor.h"
//#include "cpu.h"
//#include "Adc.h"

#include <math.h>
#include <stdio.h>
#include "keyremo.h"
#include "GT911.h"

/*
#include "Config.h"

#include "reg.h"
#include "typedefs.h"
#include "main.h"
//#include "tw88.h"
#include "i2c.h"
#include "adc.h"
//#include "osdbasic.h"
//#include "dispinfo.h"
//#include "debug.h"
#include "etc_eep.h"
//#include "measure.h"
#include "Printf.h"
//#include "audio.h"
//#include "osdmenu.h"
//#include "pc_eep.h"
#include "KeyRemo.h"
#include "Monitor.h"
#include "CPU.h"
//#include "HS_keyremo.h"
//#include "HS_DisplayOSD.h"
//#include "rgbmix.h"
//#include "panel.h"
//#include "HS_IOdef.h"
#include "HS_DVRProtocol.h"
*/

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define GET_EVENT_VALID(x) (g_stTimerEvent[x].b1EventValid)
#define SET_EVENT_VALID(x) (g_stTimerEvent[x].b1EventValid = _TRUE)
#define CLR_EVENT_VALID(x) (g_stTimerEvent[x].b1EventValid = _FALSE)

#define GET_EVENT_ID(x) (g_stTimerEvent[x].b7EventID)
#define SET_EVENT_ID(x, y) (g_stTimerEvent[x].b7EventID = y)

#define GET_EVENT_TIME(x) (g_stTimerEvent[x].usTime)
#define SET_EVENT_TIME(x, y) (g_stTimerEvent[x].usTime = y)

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
volatile bit g_bNotifyTimer0Int = _FALSE;
BYTE tic02 = 0;
WORD tic03 = 0, tic04 = 0;
WORD standby_count = 0;
volatile WORD g_usTimerCounter = 0;
StructTimerEventTableType g_stTimerEvent[_TIMER_EVENT_COUNT];
WORD BatteryBTH, BatteryVoltage;
BYTE PSW_Flag = 0;
BYTE WDT_Echo_Count = 0;
BYTE CameraVolt;
BYTE year1 = 0, year2 = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
BYTE BOX_ISSUE;
extern BYTE Monitor_flag;
extern tByte DayNightLevel;
extern void EQ_Detect();
extern tWord LightSensor_ADC;
extern ADC_HandleTypeDef hadc1;
extern volatile BYTE Encoder_flag;
extern tByte DisplayedOSD;
extern BYTE bytAutoSleeping;
extern tByte bytVideoInput;
extern BYTE bytStandByMode;
extern BYTE bytStandByModeToTriggerMode;

extern WORD GetItemValue(BYTE id);
extern BYTE TimeStep;
extern BYTE TriggerFlag;
extern BYTE PowerFlag;
extern BYTE SEQFlag;
extern BYTE PWMLevel;
extern BYTE TriggerStatus;
extern volatile BYTE MainSetSelectMode[13];
extern BYTE SelectModeType;
extern BYTE NowModeState;
extern tByte gaHDMI_MapWindowIndex[4];
extern BYTE BOOT_ISSUE;
extern BYTE CURRENT_CH;
extern BYTE TP_ACTION;
extern BYTE slider_event;
extern BYTE Flisheye_CAM[4];
extern WORD BAT_VOLTAGE, BAT_ADC, BAT_StartupVOLT;
extern BYTE bytSelectCAMRD;
extern BYTE bytGarbberLinerMode;

extern BYTE bytResetOSDValue;
extern BYTE RUN_DIMMER;

// extern StructBatteryInfoType g_stBatteryInfo;
/*
extern BYTE EncorderCountPN;
extern BYTE EncorderLenint;
extern short EncorderCount;
extern WORD EncorderLenfloaat;
*/
// StructPowerInfoType idata g_stPowerInfo;
StructPowerInfoType g_stPowerInfo;
//--------------------------------------------------
// Definitions of Timer2
//--------------------------------------------------
#define _EVENT_TIME_MAX 61000 // Please do not modify this value.

//****************************************************************************
// CODE TABLES
//****************************************************************************

//****************************************************************************
// FUNCTION DECLARATIONS
//****************************************************************************
void SysTimerHandler(void);
void SysTimerEventProc(EnumSystemTimerEventID enumEventID);
void SysTimerIntProc0(void);
void SysTimerIntProc1(void);
void SysTimerIntProc2(void);
void MCUTimerInitialTimerEvent(void);
void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID);
void MCUTimerCancelTimerEvent(BYTE ucEventID);
void MCUTimerDelayXms(WORD usNum);
void PowerLED(EnumSystemTimerEventID flag);

extern void ShowWorkingTimer(void);
extern void GraphicsPrint(PRINT_COLOR_e mode, const char *Message, ...);
extern void AdjustAutoDayNight(BYTE val);
extern BYTE CheckKeyIn(void);
extern BYTE GET_PCA9554PW(BYTE reg);
extern void AudioMute(bit mute);
extern void SET_VX8812_SD_Video_Out(tByte tbMode);
extern void BackLightON(void);
extern void TVI_BOX_Set(void);

/*
extern WORD GetBatteryBTH(void);
extern WORD GetBatteryVoltage(void);
extern void SetAD5110Step(BYTE newv) ;
extern void UserInterfacePowerSwitch(EnumPowerAction enumSwitch);
*/

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : Timer Delay
// Input Value  : usNum --> Delay in ms
// Output Value : None
//--------------------------------------------------
void MCUTimerDelayXms(WORD usNum)
{
	if (usNum)
	{
		g_bNotifyTimer0Int = _FALSE;

		while (_TRUE)
		{
			if (g_bNotifyTimer0Int)
			{
				g_bNotifyTimer0Int = _FALSE;

				if (--usNum)
				{
				}
				else
				{
					return;
				}
			}
		}
	}
}

//--------------------------------------------------
// Description  : Get Event Time
// Input Value  : ucEventIndex --> Event which time is needed
// Output Value : Event Time
//--------------------------------------------------
WORD MCUTimerGetEventTime(BYTE ucEventIndex)
{
	return GET_EVENT_TIME(ucEventIndex);
}

//--------------------------------------------------
// Description  : Get Event ID
// Input Value  : Referenced Event Index
// Output Value : Stored Event ID
//--------------------------------------------------
BYTE MCUTimerGetEventID(BYTE ucEventIndex)
{
	return GET_EVENT_ID(ucEventIndex);
}

//--------------------------------------------------
// Description  : Clear Event Time
// Input Value  : ucEventIndex --> Event which time is needed to be cleared
// Output Value : None
//--------------------------------------------------

void MCUTimerClrEventValid(BYTE ucEventIndex)
{
	CLR_EVENT_VALID(ucEventIndex);
}

//--------------------------------------------------
// Description  : Check if Event is Valid
// Input Value  : ucEventIndex --> Event to be checked
// Output Value : Validity(_TRUE/_FALSE)
//--------------------------------------------------
bit MCUTimerCheckEventValid(BYTE ucEventIndex)
{
	return GET_EVENT_VALID(ucEventIndex);
	;
}

//--------------------------------------------------
// Description  : Get Timer Counter.
// Input Value  : None
// Output Value : Timer Counter
//--------------------------------------------------
WORD MCUTimerGetTimerCounter(void)
{
	WORD usPresentTime = g_usTimerCounter;

	// When get counter,timer interrupt maybe change counter.
	// Get twice to avoid it.
	if (abs(g_usTimerCounter - usPresentTime) > 0x7F)
	{
		usPresentTime = g_usTimerCounter;
	}

	return usPresentTime;
}
//--------------------------------------------------
// Description  : Check timer and events. We have to run this function when setting up a timer for an event.
// Input Value  : usTime    --> Unit in 1ms, range in 0.001 ~ 61sec
// Output Value : Return usPresentTime
//--------------------------------------------------
WORD MCUTimerCheckTimerEvent(WORD usTime)
{
	BYTE ucTimerEventCnt = 0;
	WORD usPresentTime = 0;

	usPresentTime = MCUTimerGetTimerCounter();

	if (usTime > _EVENT_TIME_MAX)
	{
		usTime = _EVENT_TIME_MAX;
	}

	if (((usPresentTime + usTime) > _EVENT_TIME_MAX) || // Size Limit of Timer Counter
		((usPresentTime + usTime) < usPresentTime))		// Timer Counter Overflow
	{
		g_usTimerCounter = 0;

		// Reset Timer Event Counter
		for (ucTimerEventCnt = 0; ucTimerEventCnt < _TIMER_EVENT_COUNT; ucTimerEventCnt++)
		{
			if (GET_EVENT_VALID(ucTimerEventCnt) == _TRUE)
			{
				if (GET_EVENT_TIME(ucTimerEventCnt) > usPresentTime)
				{
					SET_EVENT_TIME(ucTimerEventCnt, (GET_EVENT_TIME(ucTimerEventCnt) - usPresentTime));
				}
				else
				{
					SET_EVENT_TIME(ucTimerEventCnt, 0);
				}
			}
		}

		usPresentTime = 0;
	}

	return usPresentTime;
}

//--------------------------------------------------
// Description  : Initial timer and events. We have to run this function at firmware startup
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void MCUTimerInitialTimerEvent(void)
{
	BYTE ucTimerEventCnt = 0;

	for (ucTimerEventCnt = 0; ucTimerEventCnt < _TIMER_EVENT_COUNT; ucTimerEventCnt++)
	{
		CLR_EVENT_VALID(ucTimerEventCnt);
	}

	// TR2 = 1;
}

//--------------------------------------------------
// Description  : Active Timer Event
// Input Value  : usTime --> Time to active Timer Event
//				  ucEventID --> Event to be actived
// Output Value : None
//--------------------------------------------------
void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID)
{
	BYTE ucEventIndex = 0;

	// Search for Existing Event with the same Event ID
	for (ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
	{
		if (GET_EVENT_VALID(ucEventIndex) == _TRUE)
		{
			if (GET_EVENT_ID(ucEventIndex) == ucEventID)
			{
				return;
			}
		}
	}

	// Choose any Vacant Storage for Event
	for (ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
	{
		if (GET_EVENT_VALID(ucEventIndex) == _FALSE)
		{
			SET_EVENT_VALID(ucEventIndex);
			SET_EVENT_ID(ucEventIndex, ucEventID);
			SET_EVENT_TIME(ucEventIndex, (WORD)((DWORD)(MCUTimerCheckTimerEvent(usTime)) + usTime));
			return;
		}
	}

	Printf("No More Available Storage for Simultaneous Event!!!!!!");
}

//--------------------------------------------------
// Description  : Cancel an event
// Input Value  : Event     --> Event which we want to cancel
// Output Value : None
//--------------------------------------------------
void MCUTimerCancelTimerEvent(BYTE ucEventID)
{
	BYTE ucEventIndex = 0;

	// Search for Existing Event with the same Event ID
	for (ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
	{
		if (GET_EVENT_VALID(ucEventIndex) == _TRUE)
		{
			if (GET_EVENT_ID(ucEventIndex) == ucEventID)
			{
				CLR_EVENT_VALID(ucEventIndex);
				return;
			}
		}
	}
}

//--------------------------------------------------
// Description  : Reactive a timer for an event. If the event is exist,
//                this function will reset the executing time and restart.
// Input Value  : usTime    --> Unit in 1ms, range in 0.01 ~ 61 sec
//                Event     --> Execute Event while timeup
// Output Value : None
//--------------------------------------------------
void MCUTimerReactiveTimerEvent(WORD usTime, BYTE ucEventID)
{
	BYTE ucEventIndex = 0;

	// Search for Existing Event with the same Event ID
	for (ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
	{
		if (GET_EVENT_VALID(ucEventIndex) == _TRUE)
		{
			if (GET_EVENT_ID(ucEventIndex) == ucEventID)
			{
				SET_EVENT_TIME(ucEventIndex, (WORD)((DWORD)(MCUTimerCheckTimerEvent(usTime)) + usTime));
				return;
			}
		}
	}

	// Choose any Vacant Storage for Event
	for (ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
	{
		if (GET_EVENT_VALID(ucEventIndex) == _FALSE)
		{
			SET_EVENT_VALID(ucEventIndex);
			SET_EVENT_ID(ucEventIndex, ucEventID);
			SET_EVENT_TIME(ucEventIndex, (WORD)((DWORD)(MCUTimerCheckTimerEvent(usTime)) + usTime));
			return;
		}
	}
}

//=============================================================================
//                            Power LED ON / OFF
//=============================================================================
void PowerLED(EnumSystemTimerEventID flag)
{

	// SET_LED_STATUS(flag);

	switch (flag)
	{
	case _SYSTEM_TIMER_EVENT_GRN_ON:
		HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
		break;
	case _SYSTEM_TIMER_EVENT_RED_ON:
		HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
		break;
	case _SYSTEM_TIMER_EVENT_GRN_RED_OFF:
		HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
		break;

		//    HAL_GPIO_WritePin(GPIOA, TX_RST_Pin|TP2824_RST_Pin|VS_RST_Pin, GPIO_PIN_SET);
	/*
	case _SYSTEM_TIMER_EVENT_GRN_ON:
	if((P4&0x01))
	  P4=P4&0xFE;	   //GREEN_LED = 0; 	   // ON GREEN
	 break;
	case _SYSTEM_TIMER_EVENT_GRN_OFF:
	  if((P4&0x01)==0)
	 P4=P4|0x01;		//GREEN_LED = 1;		// OFF GREEN
	 break;
	case _SYSTEM_TIMER_EVENT_RED_ON:
   if((P4&0x02))
	 P4=P4&0xFD;		//RED_LED = 0;		// ON RED
	 break;
	case _SYSTEM_TIMER_EVENT_RED_OFF:
	if((P4&0x02)==0)
	 P4=P4|0x02;		//RED_LED = 1;		// OFF RED
	 break;
	case _SYSTEM_TIMER_EVENT_GRN_RED_ON:
		if((P4&0x01))
		  P4=P4&0xFE;	   //GREEN_LED = 0; 	   // ON GREEN
		if((P4&0x02))
		 P4=P4&0xFC; 	//RED_LED = 0;		// ON RED
	 break;
	case _SYSTEM_TIMER_EVENT_GRN_BLINK:
		  if((P4&0x01))
		 {
		  P4=P4&0xFE;	   //GREEN_LED = 0; 	   // ON GREEN
		 }
		 else
		 {
		 P4=P4|0x01; 	//GREEN_LED = 1;		// OFF GREEN
		 }
	 break;
	case _SYSTEM_TIMER_EVENT_RED_BLINK:
		  if((P4&0x02))
		 {
		  P4=P4&0xFD;		//RED_LED = 0;		// ON RED
		 }
		 else
		 {
		 P4=P4|0x02;		//RED_LED = 1;		// OFF RED
		 }
	 break;
   case _SYSTEM_TIMER_EVENT_GRN_RED_BLINK:
		  if((P4&0x01))
		 {
		  P4=P4&0xFC;	   //GREEN_LED = 0; 	   // ON RED& GREEN
		 }
		 else
		 {
		 P4=P4|0x03; 	//GREEN_LED = 1;		// OFF GREEN&RED
		 }
	  break;
	case _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK:
		  if((P4&0x02))
		 {
		  P4=P4&0xFC;	   //GREEN_LED = 0; 	   // ON RED
		 }
		 else
		 {
		 P4=P4|0x02; 	//GREEN_LED = 1;		// OFF RED
		 }
	  break;
	  */
	default:
		break;
	}
}

void UserInterfaceTimerEventProc(BYTE ucEventID)
{

	switch (ucEventID)
	{
	case _USER_TIMER_EVENT_KEY_REPEAT_ENABLE:
		// SET_KEYREPEATSTART();
		DEBUG_MESSAGE(">");
		MCUTimerActiveTimerEvent(SEC(10), _USER_TIMER_EVENT_KEY_REPEAT_ENABLE);
		break;
	case _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_INC:
		if (CameraVolt < 100)
		{
			CameraVolt++;
		}
		else
		{
			CameraVolt = 100;
		}
		// SetAD5110Step(CameraVolt);
		// WriteEEP(EEP_CameraVolt, CameraVolt);
		Printf("(CAM_INC %02x)", (WORD)CameraVolt);
		break;
	case _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_DEC:

		if (CameraVolt)
		{
			CameraVolt--;
		}
		else
		{
			CameraVolt = 0;
		}
		// SetAD5110Step(CameraVolt);
		// WriteEEP(EEP_CameraVolt, CameraVolt);
		Printf("(CAM_DEC %02x)", (WORD)CameraVolt);
		break;
	case _USER_TIMER_EVENT_OSD_GET_MCU_VERSION:
		// MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_REPLY_MCU_FW);
		Printf("(GET_MCU_VERSION)");
		break;
	case _USER_TIMER_EVENT_OSD_MCU_ENTRY_SLEEP_MODE:
		Printf("(MCU_ENTRY_SLEEP_MODE)");
		PSW_Flag = 1;
		break;
	case _USER_TIMER_EVENT_OSD_DISTANCE_RESET:
		/*
		EncorderCount=0;
		EncorderLenint=0;
		EncorderLenfloaat=0;
		EncorderCountPN=0;
		*/
		// MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_REGULAR_DATA);
		// param[0]=EncorderLenint;
		// param[1]=EncorderLenfloaat;
		// param[2]=EncorderCountPN;
		// protocol_send_cmd_with_param_num(0x0a, (BYTE *)param, 3);
		Printf("\r\n ClearEncorder ", 0);

		// Printf("(DISTANCE_RESET)");
		break;
	case _USER_TIMER_EVENT_OSD_DVR_REBOOT:
		Printf("(DVR_REBOOT)");
		break;
	case _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN:
		Printf("(DVR_SHUTDOWN)");
		break;
	case _USER_TIMER_EVENT_OSD_ENCODER_DEVIATION_SETTING:
		Printf("(ENCODER_SET)");
		break;
#if (_DEBUG_MESSAGE_Monitor == ON)
	case _USER_TIMER_EVENT_Monitor_ON:
		Monitor_flag = _FALSE;
		break;
#endif
	case _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON:
		LCDPowerON(1);
		SETTW8812ON();
		break;
	case _USER_TIMER_EVENT_BACKLIGHT_ON:
		BackLightON();
		AudioMute(OFF);
		bytResetOSDValue = 0;
		break;
	case _USER_TIMER_EVENT_OSD_DISABLE:
		CloseOSDMenu();
		break;
	case _USER_TIMER_EVENT_DISPLAY_DISABLE:

		if ((GetItemValue(0xb9) == _FALSE) && (TriggerStatus == OFF) && ((GetDisplayedOSD() & (MENU | TVVOL)) == 0) && (SEQFlag == OFF))
			CloseOSDINFO();

		break;
	case _USER_TIMER_EVENT_Encoder_Loop:
		Encoder_flag = ON;

		break;
	case _USER_TIMER_EVENT_SEQ_CHECK:
		if ((SEQFlag == ON) && (TriggerFlag == 0) && (PowerFlag == ON) && ((GetDisplayedOSD() & (MENU | TVVOL)) == 0))
		{
			if (BOOT_ISSUE == 0)
				SEQMode();

			MCUTimerActiveTimerEvent(SEC(TimeStep), _USER_TIMER_EVENT_SEQ_CHECK);
		}
		break;
	default:
		break;
	}
#if 0
    switch(ucEventID)
    {
        case _USER_TIMER_EVENT_KEY_REPEAT_ENABLE:
            SET_KEYREPEATSTART();
            break;

        case _USER_TIMER_EVENT_KEY_SCAN_READY:
            SET_KEYSCANREADY();
            break;

        case _USER_TIMER_EVENT_OSD_DISABLE_OSD_LOGO:
            RTDOsdDisplayDisableOsd();
            SET_OSD_LOGO_ON(_OFF);

#if (_INSTANT_TIMER_EVENT_0 == _ON)
            MCUTimer0SetTimerCount(_TIMER_OFF);
#endif

#if (_PANEL_POW_SEQ_FAST_MODE == _ON)
            UserInterfacePanelPowerAction(_BACKLIGHT_OFF);
#else
            UserInterfacePanelPowerAction(_PANEL_OFF);
#endif
            ScalerDDomainBackgroundEnable(_ENABLE);
            ScalerDDomainPatternGenEnable(_DISABLE);
            break;

        case _USER_TIMER_EVENT_OSD_SHOW_NO_CABLE:
      SET_OSD_EVENT_MESSAGE(_OSDEVENT_SHOW_NOCABLE_MSG);
            break;

        case _USER_TIMER_EVENT_OSD_SHOW_NO_SIGNAL:
       SET_OSD_EVENT_MESSAGE(_OSDEVENT_SHOW_NOSIGNAL_MSG);
            break;

        case _USER_TIMER_EVENT_OSD_SHOW_POWER_SAVING:

#ifdef debug_mode1
					DebugMessageOsd("p6",0);
#endif
       SET_OSD_EVENT_MESSAGE(_OSDEVENT_SHOW_POWER_SAVING_EVENT_MSG);
            break;

        case _USER_TIMER_EVENT_GO_TO_POWER_SAVING:
          SET_OSD_EVENT_MESSAGE(_OSDEVENT_GO_TO_POWER_SAVING_EVENT_MSG);
            break;

        case _USER_TIMER_EVENT_OSD_DISABLE:
					
        	
						RTDOsdDisplayDisableOsd();
				  if(GET_CHANNEL_INFO_STATUS()==_TRUE) MCUTimerReactiveTimerEvent(SEC(3),_USER_TIMER_EVENT_DISPLAY_INFO);
            break;

        case _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON:
            SET_PANELPOWERSTATUS(_READY_FOR_BACKLIGHT);
            break;
		 case _USER_TIMER_EVENT_LIGHT_SENSER_ON	:
		 	if(GET_OSD_LIGHTSENSER()<=0x30) 
		 		{
				 UserInterfacePanelPowerAction(_BACKLIGHT_OFF);
				DebugMessageOsd("LIGHT_SENSER BACKLIGHT_OFF==",1);
		 		}
			  break;
				 case _USER_TIMER_EVENT_SHOW_BURN_IN://ryan@20141230		
				 	SET_OSD_EVENT_MESSAGE(_OSDEVENT_SHOW_BURNIN_MSG);	
				 //SET_OSD_EVENT_MESSAGE(_OSDEVENT_FACTORY_BURNIN_MODE_MSG);
				break;
				
	  case _USER_TIMER_EVENT_BURN_IN_ON://ryan@20141230		
		 		 SET_OSD_EVENT_MESSAGE(_OSDEVENT_FACTORY_BURNIN_MODE_MSG);	
				 //SET_OSD_EVENT_MESSAGE(_OSDEVENT_FACTORY_BURNIN_MODE_MSG);
				break;
	  case _USER_TIMER_EVENT_CHECK://ryan@20141230		
			 SET_OSD_EVENT_MESSAGE(_OSDEVENT_CHECK_MSG);
				break;
	
		 case _USER_TIMER_EVENT_DISPLAY_INFO://ryan@20141230		
			 SET_OSD_EVENT_MESSAGE(_OSDEVENT_DISPLAY_INFO_MSG);
				break;	
	  case _USER_TIMER_EVENT_STANDBY_MODE:
	 SET_OSD_EVENT_MESSAGE(_OSDEVENT_STANDBY_MODE_MSG);
			break;
		  case _USER_TIMER_EVENT_FACTORY_MODE:
	 SET_OSD_EVENT_MESSAGE(_OSDEVENT_FACTORY_MODE_MSG);
			break;		
        case _USER_TIMER_EVENT_PANEL_POWER_OFF:
            SET_PANELPOWERSTATUS(_PANEL_POWER_OFF_STATE);
            break;

	case _USER_TIMER_EVENT_LED_POWER_SAVING:
		
		PCB_LED_IDLE();
		break;

#if (_VGA_SUPPORT == _ON)
        case _USER_TIMER_EVENT_DO_AUTO_COLOR:
            RTDOsdDisplayAutoBalanceProc();
            break;
#endif

        case _USER_TIMER_EVENT_PANEL_USED_TIMER:
            RTDFactoryPanelUsedTimer();
            break;

#if (_HIGHLIGHT_WINDOW_SUPPORT == _ON)
        case _USER_TIMER_EVENT_HLWIN_TYPE5_MOVE:
            UserAdjustHLWinType5Move();
            break;
#endif

#if (_ENERGY_STAR_SUPPORT == _ON)
        case _USER_TIMER_EVENT_ES_ADJUST:
            UserCommonAdjustESDcrAdjustPWM();
            break;
#endif

#if defined(_UART_ENABLE) // uart [micon.sam.0524.2013]
        case _USER_TIMER_EVENT_UART_RESET:          //for uart timeout  //walter.hsiao 20121108
            g_bUartCommandStart = _FALSE;
            g_ucUartCommandNum = 1;
            break;
#endif

        default:
            break;
    }
#endif
}

//=============================================================================//
//                     ShowWorkTimer                                                 										//
//=============================================================================//
void ShowWorkingTimer(void)
{
	if (day)
	{
		GraphicsPrint(YELLOW, "\r\nSystem has been working for %d days, %d hours, %d minutes and %d seconds", (WORD)day, (WORD)hour, (WORD)minute, (WORD)second);
	}
	else if (hour)
	{
		GraphicsPrint(YELLOW, "\r\nSystem has been working for %d hours, %d minutes and %d seconds", (WORD)hour, (WORD)minute, (WORD)second);
	}
	else if (minute)
	{
		GraphicsPrint(YELLOW, "\r\nSystem has been working for %d minutes and %d seconds", (WORD)minute, (WORD)second);
	}
	else
	{
		GraphicsPrint(YELLOW, "\r\nSystem has been working for %d seconds", (WORD)second);
	}

	/*
		GraphicsPrint(GREEN,"\r\n(BatteryBTH=%d(%d.%dV))",(WORD)BatteryBTH\
				,(WORD)((BatteryBTH*32)/10000)\
			,(WORD)((BatteryBTH*32)%10000));

		if(GET_BTH_STATE()==_BATT_STATUS_TEMP_NORMAL)
			GraphicsPrint(GREEN,"(TEMP_NORMAL)");
		else if(GET_BTH_STATE()==_BATT_STATUS_TEMP_HIGH)
			GraphicsPrint(RED,"(TEMP_HIGH)");
		else
			GraphicsPrint(BLUE,"(TEMP_LOW)");

	GraphicsPrint(MAGENTA,"\r\n(BatteryCAP=%d(%d.%dV))",(WORD)GET_BATTERY_CAPACITY()\
			,(WORD)((GET_BATTERY_CAPACITY()*32)/10000)\
			,(WORD)((GET_BATTERY_CAPACITY()*32)%10000));

		if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_HIGH_CHARGE)
			GraphicsPrint(GREEN,"(HIGH_CHARGE)");
		else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_LOW_CHARGE)
			GraphicsPrint(YELLOW,"(LOW_CHARGE)");
		else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_STOP_CHARGE)
			GraphicsPrint(RED,"(STOP_CHARGE)");
		else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)
			GraphicsPrint(RED,"(NO_BATT)");

	   if(GET_BATTERY_STATE()==4)
		GraphicsPrint(GREEN,"(BATT_STATUS_AC_MODE)");
	else if(GET_BATTERY_STATE()==3)
		GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL1)");
	else if(GET_BATTERY_STATE()==2)
		GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL2)");
	else if(GET_BATTERY_STATE()==1)
		GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL3)");
	else	 if(GET_BATTERY_STATE()==0)
		GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL4)");
	else	 if(GET_BATTERY_STATE()==9)
		GraphicsPrint(RED,"(_BATT_STATUS_NO_BATT)");

	GraphicsPrint(CYAN,"\r\n(POWER_STATUS=%d)",(WORD)GET_POWER_STATUS());
	*/
}

void MCUTimerEventProc(EnumMCUTimerEventID enumEventID)
{

	switch (enumEventID)
	{

	default:
		break;
	}

#if 0
    switch(enumEventID)
    {
#if ((_DVI_SUPPORT == _ON) || (_HDMI_SUPPORT == _ON))
        case _SCALER_TIMER_EVENT_TMDS_VIDEO_DETECT:
        
            ScalerSyncTmdsVideoDetectEvent();
            break;

#if (_HDMI_SUPPORT == _ON)
        case _SCALER_TIMER_EVENT_TMDS_HDMI_PACKET_DETECT:
        
            ScalerTmdsHdmiPacketDetectEvent();
            break;
#endif // End of #if(_HDMI_SUPPORT == _ON)

#endif // End of #if((_DVI_SUPPORT == _ON) || (_HDMI_SUPPORT == _ON))

#if (_AUDIO_SUPPORT == _ON)
        case _SCALER_TIMER_EVENT_AUDIO_LOAD_STABLE_I_CODE:

            ScalerAudioLoadStableICodeEvent();
            break;
#endif // End of #if(_AUDIO_SUPPORT == _ON)

#if (_DP_SUPPORT == _ON)
        case _SCALER_TIMER_EVENT_DP_HOTPLUG_ASSERTED:
            ScalerDpHotPlugEvent(_DP_HPD_ASSERTED);
            break;

        case _SCALER_TIMER_EVENT_DP_HDCP_LONG_HOTPLUG_EVENT:
            ScalerDpHotPlugEvent(_DP_HDCP_LONG_HPD_EVENT);
            break;

        case _SCALER_TIMER_EVENT_DP_LINK_STATUS_IRQ:
            ScalerDpLinkStatusIRQ();
            break;
#endif

#if ((_D2_INPUT_PORT_SWITCH_FROM == _SWITCH_FROM_D0) || (_D3_INPUT_PORT_SWITCH_FROM == _SWITCH_FROM_D0))
        case _SCALER_TIMER_EVENT_D0_PORT_DIGITAL_SWITCH:
            SET_D0_PORT_DIGITAL_SWITCH();
            break;
#endif

#if ((_D2_INPUT_PORT_SWITCH_FROM == _SWITCH_FROM_D1) || (_D3_INPUT_PORT_SWITCH_FROM == _SWITCH_FROM_D1))
        case _SCALER_TIMER_EVENT_D1_PORT_DIGITAL_SWITCH:
            SET_D1_PORT_DIGITAL_SWITCH();
            break;
#endif

#if ((_VGA_SUPPORT == _ON) && (_ADCNR_BY_HISTOGRAM_SUPPORT == _ON))
        case _SCALER_TIMER_EVENT_NR_DETECTION_FINISHED:
            ScalerNrTimerEvent();
            break;
#endif

#if (_SDRAM_PHASE_CALIBRATION == _ON)
        case _SCALER_TIMER_EVENT_PHASE_CALIBRATION: 
            ScalerSDRAMPhaseOnLineTimer();
            break;

        case _SCALER_TIMER_EVENT_PHASE_CNT_CHECK:

            // Check phase count every 60 minute
            ScalerSDRAMPhaseCntCheck(60);
            break;
#endif

#if (_D0_INPUT_PORT_TYPE == _D0_MHL_PORT)
        case _SCALER_TIMER_EVENT_MHL_D0_READY_TO_TRANSMIT:

            SET_MHL_READY_TO_TRANSMIT(0, _TRUE);
            break;

        case _SCALER_TIMER_EVENT_MHL_D0_READY_TO_RESEND:

            SET_MHL_READY_TO_RESEND(0, _TRUE);
            break;

        case _SCALER_TIMER_EVENT_MHL_D0_RECONNECT_1K:

#if (_D0_CBUS_CHANNEL_SEL == _CBUS0)
            if(((ScalerGetByte(P28_A7_CBUS0_CTRL_07) & 0x38) >> 3) == 0x02)
            {
                ScalerSetBit(P28_A8_CBUS0_CTRL_08, ~_BIT5, 0x00);
                MCUTimerDelayXms(52);
                ScalerSetBit(P28_A8_CBUS0_CTRL_08, ~_BIT5, _BIT5);
            }
#elif (_D0_CBUS_CHANNEL_SEL == _CBUS1)
            if(((ScalerGetByte(P29_A7_CBUS1_CTRL_07) & 0x38) >> 3) == 0x02)
            {
                ScalerSetBit(P29_A8_CBUS1_CTRL_08, ~_BIT5, 0x00);
                MCUTimerDelayXms(52);
                ScalerSetBit(P29_A8_CBUS1_CTRL_08, ~_BIT5, _BIT5);
            }
#endif

            break;

        case _SCALER_TIMER_EVENT_MHL_D0_ACTIVE_VBUS:

            if(GET_D0_TMDS_MODE() == _TMDS_MHL_MODE)
            {
                // GPIO Select to Hot Plug
#if (_D0_CBUS_CHANNEL_SEL == _CBUS0)
                ScalerSetBit(P27_A0_CBUS_EN, ~_BIT4, 0x00);
#elif (_D0_CBUS_CHANNEL_SEL == _CBUS1)
                ScalerSetBit(P27_A0_CBUS_EN, ~_BIT5, 0x00);
#endif
                SET_D0_MHL_VBUS_SWITCH(_MHL_VBUS_ON);
            }
            
            break;
#endif // End of #if(_D0_INPUT_PORT_TYPE == _D0_MHL_PORT)

#if (_D1_INPUT_PORT_TYPE == _D1_MHL_PORT)
        case _SCALER_TIMER_EVENT_MHL_D1_READY_TO_TRANSMIT:

#if (_D0_INPUT_PORT_TYPE == _D0_MHL_PORT)
            SET_MHL_READY_TO_TRANSMIT(1, _TRUE);
#else
            SET_MHL_READY_TO_TRANSMIT(0, _TRUE);
#endif

            break;

        case _SCALER_TIMER_EVENT_MHL_D1_READY_TO_RESEND:

#if (_D0_INPUT_PORT_TYPE == _D0_MHL_PORT)
            SET_MHL_READY_TO_RESEND(1, _TRUE);
#else
            SET_MHL_READY_TO_RESEND(0, _TRUE);
#endif

            break;
     
        case _SCALER_TIMER_EVENT_MHL_D1_RECONNECT_1K:

#if (_D1_CBUS_CHANNEL_SEL == _CBUS0)
            if(((ScalerGetByte(P28_A7_CBUS0_CTRL_07) & 0x38) >> 3) == 0x02)
            {
                ScalerSetBit(P28_A8_CBUS0_CTRL_08, ~_BIT5, 0x00);
                MCUTimerDelayXms(52);
                ScalerSetBit(P28_A8_CBUS0_CTRL_08, ~_BIT5, _BIT5);
            }
#elif (_D1_CBUS_CHANNEL_SEL == _CBUS1)
            if(((ScalerGetByte(P29_A7_CBUS1_CTRL_07) & 0x38) >> 3) == 0x02)
            {
                ScalerSetBit(P29_A8_CBUS1_CTRL_08, ~_BIT5, 0x00);
                MCUTimerDelayXms(52);
                ScalerSetBit(P29_A8_CBUS1_CTRL_08, ~_BIT5, _BIT5);
            }
#endif

            break;

        case _SCALER_TIMER_EVENT_MHL_D1_ACTIVE_VBUS:

            if(GET_D1_TMDS_MODE() == _TMDS_MHL_MODE)
            {
                // GPIO Select to Hot Plug
#if (_D1_CBUS_CHANNEL_SEL == _CBUS0)
                ScalerSetBit(P27_A0_CBUS_EN, ~_BIT4, 0x00);
#elif (_D1_CBUS_CHANNEL_SEL == _CBUS1)
                ScalerSetBit(P27_A0_CBUS_EN, ~_BIT5, 0x00);
#endif
 
                SET_D1_MHL_VBUS_SWITCH(_MHL_VBUS_ON);
            }
            
            break;
#endif // End of #if(_D1_INPUT_PORT_TYPE == _D1_MHL_PORT)

#if ((_DCC_HISTOGRM_INFO == _ON) && (_DCC_HISTOGRAM_THD_CHANGE == _ON))
        case _SCALER_TIMER_EVENT_DCC_HISTOGRAM_THD_CHANGE:
            ScalerColorDCCChangeHistogramThd();            
            MCUTimerReactiveTimerEvent((WORD)(((DWORD)_PANEL_DH_TOTAL * _PANEL_DV_TOTAL * 2) / GET_D_CLK_FREQ()), _SCALER_TIMER_EVENT_DCC_HISTOGRAM_THD_CHANGE);

            break;
#endif

        default:
            break;
    }
#endif
}

//--------------------------------------------------
// Description  : Timer Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysTimerHandler(void)
{
	BYTE ucTimerEventIndex = 0;
	BYTE ucTimerEventID = 0;
	WORD usPresentTime = 0;

	for (ucTimerEventIndex = 0; ucTimerEventIndex < _TIMER_EVENT_COUNT; ucTimerEventIndex++)
	{
		usPresentTime = MCUTimerGetTimerCounter();

		if (MCUTimerCheckEventValid(ucTimerEventIndex) == _FALSE)
		{
			continue;
		}

		if (usPresentTime > MCUTimerGetEventTime(ucTimerEventIndex))
		{
			MCUTimerClrEventValid(ucTimerEventIndex);

			// Get Event ID From Event Index
			ucTimerEventID = MCUTimerGetEventID(ucTimerEventIndex);

			if (ucTimerEventID < _USER_TIMER_EVENT_END)
			{
				UserInterfaceTimerEventProc(ucTimerEventID);

				continue;
			}

			if (ucTimerEventID < _SYSTEM_TIMER_EVENT_END)
			{
				SysTimerEventProc(ucTimerEventID);

				continue;
			}

			//   MCUTimerEventProc(ucTimerEventID);
		}
	}
}

//--------------------------------------------------
// Description  : Timer Event Process
// Input Value  : enumEventID --> Event to be processed
// Output Value : None
//--------------------------------------------------
BYTE led_Flag = 0;
void SysTimerEventProc(EnumSystemTimerEventID enumEventID)
{
	// WORD val1,val2;
	BYTE cam_change[4] = {0, 2, 3, 1};
#if (_DEBUG_MESSAGE_LIGHTSENSOR == ON)
// BYTE ADCL,ADCH;
#endif
	switch (enumEventID)
	{
	case _SYSTEM_TIMER_EVENT_TEST:
		RS2_tx('>');
		RS_tx('>');
		MCUTimerActiveTimerEvent(SEC(0.5), _SYSTEM_TIMER_EVENT_TEST);
		break;
	case _SYSTEM_TIMER_CHECK_BAT_VOLTAGE:
		GET_LTC2451(0);
/*
BAT_ADC=GET_LTC2451(0);
val1=((BAT_ADC/10)*5)/10;
val2=((BAT_ADC%10)*5)/100;
val1=((val1+val2)*(75))/100;
BAT_VOLTAGE=val1;
*/
#if 0
	val=((((BAT_VOLTAGE*59)/*-800*/)/100))+((((BAT_VOLTAGE*59)/*-800*/)/100))+((((BAT_VOLTAGE*59)/*-800*/)/100));
	
	if((val/1000))
		val=(val/10)+1;
		else 
		val=(val/10);
#endif

#if 0	
	GraphicsPrint(MAGENTA,"(BatteryCAP=%d(%d.%04dV))",(WORD)BAT_VOLTAGE\
			,(WORD)(((BAT_VOLTAGE/10)*(5/*32+1*/))/10000)\
			,(WORD)((((BAT_VOLTAGE/10)*(5/*32+1*/))/*-200*/)%10000));
#endif

		GraphicsPrint(GREEN, "(StartupBatteryCAP=(%d.%02dV))\r\n", (WORD)(BAT_StartupVOLT / 100), (WORD)((BAT_StartupVOLT % 100)));

		GraphicsPrint(MAGENTA, "(BatteryCAP=%d(%d.%02dV))\r\n", (WORD)BAT_ADC, (WORD)(BAT_VOLTAGE / 100), (WORD)((BAT_VOLTAGE % 100)));

		MCUTimerActiveTimerEvent(SEC(3), _SYSTEM_TIMER_CHECK_BAT_VOLTAGE);
		break;

	case _SYSTEM_TIMER_EVENT_GRN_ON:
		PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);
		break;
	case _SYSTEM_TIMER_EVENT_GRN_OFF:
		PowerLED(_SYSTEM_TIMER_EVENT_GRN_OFF);
		break;
	case _SYSTEM_TIMER_EVENT_RED_ON:
		PowerLED(_SYSTEM_TIMER_EVENT_RED_ON);
		break;
	case _SYSTEM_TIMER_EVENT_RED_OFF:
		PowerLED(_SYSTEM_TIMER_EVENT_RED_OFF);
		break;
	case _SYSTEM_TIMER_EVENT_GRN_RED_ON:
		PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_ON);
		break;
	case _SYSTEM_TIMER_EVENT_GRN_BLINK:
		PowerLED(_SYSTEM_TIMER_EVENT_GRN_BLINK);
		MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_GRN_BLINK);

		break;
	case _SYSTEM_TIMER_EVENT_RED_BLINK:
		PowerLED(_SYSTEM_TIMER_EVENT_RED_BLINK);
		MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_RED_BLINK);

		break;
	case _SYSTEM_TIMER_EVENT_GRN_RED_BLINK:
		PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_BLINK);
		MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);

		break;
	case _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK:
		PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK);
		MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK);

		break;
	case _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE:
		/*
	SET_BATTERY_CAPACITY(GetBatteryVoltage());
		   if(GET_BATTERY_CAPACITY()<10)
			SET_BATTERY_STATE(_BATT_STATUS_AC_MODE);
		else if(GET_BATTERY_CAPACITY()<256)
			SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL1);
		else if((GET_BATTERY_CAPACITY()>256)&&(GET_BATTERY_CAPACITY()<512))
			SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL2);
		else if((GET_BATTERY_CAPACITY()>512)&&(GET_BATTERY_CAPACITY()<768))
			SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL3);
		else
			SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL4);

		//BATERY_STAT=GET_BATTERY_STATE();
		MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_REGULAR_DATA);
		*/
		MCUTimerActiveTimerEvent(SEC(5), _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE);
		break;
	case _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE:
		DEBUG_MESSAGE("\r\nPOWER key pressed");
		break;
	case _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE:
		// Printf("(BTH)");
		/*
		BatteryBTH=GetBatteryBTH();

			if(BatteryBTH<50)
			{
			SET_BTH_STATE(_BATT_STATUS_TEMP_HIGH);

			if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_STOP_CHARGE)
			MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE);

			}
		else if(BatteryBTH>768)
			SET_BTH_STATE(_BATT_STATUS_TEMP_LOW);
		else
			{
			SET_BTH_STATE(_BATT_STATUS_TEMP_NORMAL);
			}

			if(GET_BTH_STATE()!=_BATT_STATUS_TEMP_NORMAL)
			SET_BTH_STATE(_BATT_STATUS_TEMP_NORMAL);//for test
	*/
		MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE);
		break;
	case _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO:
		DEBUG_MESSAGE("\r\n(WDT kick over 10Second, Reboot DVR)");
		MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);
		break;
	case _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE:
	{
		/*
						DEBUG_MESSAGE("(AC_MODE)");
						if((GET_ADAP_12())&&(GET_AC_PLUG()==0))
							{
								SET_AC_PLUG();

								if(GET_POWER_STATUS()==_POWER_STATUS_NORMAL)
								{
									if(GET_BTH_STATE()!=_BATT_STATUS_TEMP_HIGH)
									MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE);
									else
									MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE);
								}
								else if(GET_POWER_STATUS()==_POWER_STATUS_SAVING)
									{
										if(GET_BTH_STATE()!=_BATT_STATUS_TEMP_HIGH)
										MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_BATT_HIGH_CHARGE);
										else
										MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE);
									}
						MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE);
							}
						else if((GET_ADAP_12()==0)&&(GET_AC_PLUG()))
							{
							CLR_AC_PLUG();
							MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE);
							MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE);
							}

		*/
		// if(GET_AC_PLUG())
		// DEBUG_MESSAGE("(AON)");
		// else
		// DEBUG_MESSAGE("(AOFF)");
	}
	break;
	case _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE:

		DEBUG_MESSAGE("(BATT_MODE)");
		/*
		if((GET_ADAP_12())&&(GET_AC_PLUG()==0))
				{
				SET_AC_PLUG();
			MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE);
			}
			else
				{
				  MCUTimerActiveTimerEvent(SEC(5), _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE);
				}
			*/
		break;

	/*
case _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE:
	//	if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_LOW_CHARGE)
	//	UserInterfacePowerSwitch(_POWER_ACTION_BATT_TO_LOW_CURRENT_CHARGE);
break;
case  _SYSTEM_TIMER_EVENT_BATT_HIGH_CHARGE:
	//	if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_HIGH_CHARGE)
	//	UserInterfacePowerSwitch(_POWER_ACTION_BATT_TO_HIGH_CURRENT_CHARGE);
break;
case  _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE:
	//	if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_STOP_CHARGE)
	//	UserInterfacePowerSwitch(_POWER_ACTION_BATT_TO_NO_CHARGE);
	break;
	*/
	case _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER:

		second += 10;

		if (second == 60)
		{
			second = 0;
			minute++;
		}

		if (minute == 60)
		{
			minute = 0;
			hour++;
		}

		if (hour == 24)
		{
			hour = 0;
			day++;
		}

		ShowWorkingTimer();
		MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER);
		break;
	case _SYSTEM_TIMER_EVENT_AUTO_SLEEPING_TIMEOUT:

#if (_DEBUG_MESSAGE_UserInterfaceTimerEvent == ON)
		GraphicsPrint(RED, "(AUTO_SLEEPING_TIMEOUT)");
#endif

		if ((bytAutoSleeping == ON) && (SEQFlag == 0))
		{
#if (_DEBUG_MESSAGE_UserInterfaceTimerEvent == ON)
			GraphicsPrint(GREEN, "(enter standbye mode)");
#endif
			bytStandByMode = ON;
		}
		else
		{
			MCUTimerActiveTimerEvent(SEC(10 - (bytStandByModeToTriggerMode * 5)), _SYSTEM_TIMER_EVENT_AUTO_SLEEPING_TIMEOUT);
		}
#if 0				
			switch(CURRENT_CH)
				{
					case VS4210_LOOP_MODE0_w0: 
									if(((bytVideoInput&VIN1)==0)&&(bytAutoSleeping==ON))
									{
#if (_DEBUG_MESSAGE_UserInterfaceTimerEvent == ON)
									   	GraphicsPrint(GREEN,"(enter standbye mode)");
#endif
									bytStandByMode=ON;
												

									}
							break;
					case VS4210_LOOP_MODE0_w1: 
							break;
					case VS4210_LOOP_MODE0_w2: 
								break;
					case VS4210_LOOP_MODE0_w3: 
							break;
					case VS4210_LOOP_MODE12_12: 
					case VS4210_LOOP_MODE12_21: 		
					break;
					case VS4210_LOOP_MODE12_14:
					break;
				case VS4210_LOOP_MODE10_123:
				case VS4210_LOOP_MODE8: 	
				case VS4210_LOOP_MODE8_123:
				case VS4210_LOOP_MODE8_213:
				case VS4210_LOOP_MODE8_312:
				case VS4210_LOOP_MODE9: 	
					break;
			
				case VS4210_LOOP_MODE12_23:
				case VS4210_LOOP_MODE12_32:
								break;
				case VS4210_LOOP_MODE12_34:
				case VS4210_LOOP_MODE12_43:
							break;
				case VS4210_LOOP_MODE12_31:
				case VS4210_LOOP_MODE12_13: 
								break;
				case VS4210_LOOP_MODE1:
				case VS4210_LOOP_MODE2:
					break;
				case VS4210_LOOP_MODE12_42:
				case VS4210_LOOP_MODE12_24:
					break;
				case VS4210_LOOP_MODE8_412: 
				case VS4210_LOOP_MODE8_124:
				case VS4210_LOOP_MODE8_214: 
				case VS4210_LOOP_MODE10_412:
					break;
				case VS4210_LOOP_MODE8_134:
				case VS4210_LOOP_MODE8_314:
				case VS4210_LOOP_MODE10_413:
				case VS4210_LOOP_MODE8_413: 
					break;
				case VS4210_LOOP_MODE8_234:
				case VS4210_LOOP_MODE8_324: 
				case VS4210_LOOP_MODE10_423:
				case VS4210_LOOP_MODE8_423: 		
					break;
				default:
					break;
				}
#endif
		break;
	case _SYSTEM_TIMER_EVENT_RESET_TOUCH_PANEL:

		TP_ACTION = ON;
		//#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
		// GraphicsPrint(YELLOW,"(_SYSTEM_TIMER_EVENT_RESET_TOUCH_PANEL)");
		//#endif
		break;
	case _SYSTEM_TIMER_EVENT_TP_PRES_DOWN:
		MCUTimerActiveTimerEvent(SEC(3), _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER);
		break;

	case _SYSTEM_TIMER_EVENT_BOOT_VIDEO_DETECT:
		BOOT_ISSUE = 0;
#if (_DEBUG_MESSAGE_UserInterfaceTimerEvent == ON)
		GraphicsPrint(YELLOW, "(BOOT_ISSUE=0)\r\n");
#endif
		break;
	case _SYSTEM_TIMER_EVENT_SET_VX8812_SD_Video_Out:
		SET_VX8812_SD_Video_Out(_NTSC);
		break;
	case _SYSTEM_TIMER_EVENT_GRABBER_SETUP:
		SetGarbberLinerMode(bytGarbberLinerMode);
		break;
	case _SYSTEM_TIMER_EVENT_TVI_BOX_Set:
		TVI_BOX_Set();
		BOX_ISSUE++;
		if (BOX_ISSUE < 2)
			MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_TVI_BOX_Set);
		break;
	case _SYSTEM_TIMER_EVENT_LIGHT_SENSOR_CHECK:

#if (_DEBUG_MESSAGE_UserInterfaceTimerEvent == ON)
// GraphicsPrint(GREEN,"(Trig=%02x)(OSD=%02x)",(WORD)GET_PCA9554PW(PCA9554PW_reg0),(WORD)DisplayedOSD);
// GET_PCA9554PW(PCA9554PW_reg1);
#endif
		/*
					  if (ReadEEP(0xa0)==ON)  //ON=Auto
					  {
						//val = ReadDecoder(LADC0);
						AdjustAutoDayNight(LightSensor_ADC);
					  }
					  else	//OFF=Day
					  {
					  AdjustAutoDayNight(0x80);
					  }
		*/
		//	HDMITX_DevLoopProc() ;

		MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_LIGHT_SENSOR_CHECK);

		break;
	case _SYSTEM_TIMER_EVENT_KEY_CHECK:

#if (_DEBUG_MESSAGE_SysTimerEvent == ON)
		GraphicsPrint(CYAN, "(KEYC)");
#endif
		CheckKeyIn();
		MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_KEY_CHECK);
		break;
	case _SYSTEM_TIMER_EVENT_EQ_DETECT:
		EQ_Detect();
// Encoder_flag=ON;
#if (_DEBUG_MESSAGE_SysTimerEvent == ON)
		GraphicsPrint(YELLOW, "(EQ)");
#endif
		MCUTimerActiveTimerEvent(SEC(0.5), _SYSTEM_TIMER_EVENT_EQ_DETECT);
		break;
	case _SYSTEM_TIMER_EVENT_ADC_CHECK:
		HAL_ADC_Start(&hadc1);

		HAL_ADC_PollForConversion(&hadc1, 50);

		if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
		{
			LightSensor_ADC = (HAL_ADC_GetValue(&hadc1) / 14); // max 0xDFF =3583

#if (_DEBUG_MESSAGE_LIGHTSENSOR == ON)
/*
ADCH=(LightSensor_ADC>>8);
ADCL=(LightSensor_ADC&0xff);
GraphicsPrint(RED,"(ADCLH=%02x)",(WORD)(ADCH));
GraphicsPrint(CYAN,"(ADCL=%02x)",(WORD)(ADCL));
GraphicsPrint(CYAN,"(PWMLevel=%02x)\r\n",(WORD)PWMLevel);
*/
#endif
		}

		if (DayNightLevel) // ON=Auto
		{
			// val = ReadDecoder(LADC0);
			AdjustAutoDayNight(LightSensor_ADC);
		}
		else if ((RUN_DIMMER == _DIMMER_RUN1) || (RUN_DIMMER == _DIMMER_RUN2))
		{

			user_pwm_setvalue(15 * 15);
			// user_pwm_setvalue(15*15*3);
			// user_pwm_setvalue(600);
		}
		else // OFF=Day
		{
			AdjustAutoDayNight(0x80);
		}

		MCUTimerActiveTimerEvent(SEC(/*0.2-0.1*/ 0.2), _SYSTEM_TIMER_EVENT_ADC_CHECK);

		break;
	case _SYSTEM_TIMER_EVENT_TRIGGER_DELAY_RELEASE:

		if (bytStandByMode == ON)
		{
			PowerFlag = ON;
			bytStandByModeToTriggerMode = ON;
			Printf("(bytStandByModeToTriggerMode)");
		}
		else if (PowerFlag == OFF)
			LCDPowerOFF();

		ShowTRIGOSD();

		if (PowerFlag == ON)
		{

			// AudioMute(ON);
			/*
			if(BOOT_ISSUE==ON)
				{
				//BL_ENABLE(ON);
				//LCDPowerON(0);
				MCUTimerActiveTimerEvent(SEC(0.1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
				}
			else
				*/
			if (TriggerStatus)
			{
				LCDPowerOFF();
				SETTW8812OFF();
				MCUTimerActiveTimerEvent(SEC(_BACKLIGHT_ON_Time_out), _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
			}

			if (bytAutoSleeping == ON)
			{
				MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_AUTO_SLEEPING_TIMEOUT);
				MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_AUTO_SLEEPING_TIMEOUT);
			}
		}
		else
		{
			AudioMute(ON);
			PowerLED(_SYSTEM_TIMER_EVENT_RED_ON);
		}

		if (TriggerStatus)
		{
			if (GetDisplayedOSD() & _GAUGE_OSD)
				CloseOSDMenu();
			else
			{
				SetNowMode();
				CloseOSDMenu();
			}

			if (Flisheye_CAM[cam_change[TriggerStatus - 1]] == TRUE)
			{
				SetCOCCom(_COCOSD_BOX_Sel, (TriggerStatus - 1));
				SetCOCCom(_COC_FLISHEYE_MODE1, Fisheye_no_save);
				Get_2824cIrq();
			}

			TriggerStatus = 0;
		}
		// MCUTimerActiveTimerEvent(SEC(5),  _SYSTEM_TIMER_EVENT_NOW_DISPLAY);

		break;
	case _SYSTEM_TIMER_EVENT_BOOT_SETUP:
		// SetNowMode();
		//	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[0],0);
		//	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[1],1);
		//	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[2],2);
		//	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[3],3);
		/*
						gaHDMI_MapWindowIndex[i]

						switch(MainSetSelectMode[SelectModeType])
							{
							case VS4210_LOOP_MODE8_412 :
							VS4210_SwitchChannel(3,0);
							 printf("\r\n*VS4210_SwitchChannel(3,0)");
							break;
							}
						*/
		if ((NowModeState == JumpMode) && (TriggerFlag == 0))
			ShowJUMPOSD();

		// if((TriggerStatus==4)&& (TriggerFlag&REVERSE_T==REVERSE_T))
		//{
		// ShowGaugeOSD();
		// }

		break;
	case _SYSTEM_TIMER_EVENT_CAMRD_CHANGE:
		SetSelectCAMRD(bytSelectCAMRD);

#if (_DEBUG_MESSAGE_SysTimerEvent == ON)
		GraphicsPrint(GREEN, "(_SYSTEM_TIMER_EVENT_CAMRD_CHANGE)");
#endif

		break;

	case _SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT:
		Printf("\r\n(AudioMute(OFF))\r\n");
		AudioMute(OFF);
		break;
	case _SYSTEM_TIMER_EVENT_TOUCH_ACTION_0:
#if (_DEBUG_MESSAGE_GT911 == ON)
		GraphicsPrint(GREEN, "(TOUCH_ACTION_0)");
#endif
		Touch_Screen_Handler(_TOUCH_COUNT_0);
		break;
	case _SYSTEM_TIMER_EVENT_TOUCH_ACTION_1:
#if (_DEBUG_MESSAGE_GT911 == ON)
		GraphicsPrint(GREEN, "(TOUCH_ACTION_1)");
#endif
		if (((NowModeState == SelectMode) && (TriggerFlag == 0)) || (GetDisplayedOSD() & (MENU | TVVOL)))
			Touch_Screen_Handler(_TOUCH_COUNT_1);

		break;
	case _SYSTEM_TIMER_EVENT_TOUCH_ACTION_2:
#if (_DEBUG_MESSAGE_GT911 == ON)
		GraphicsPrint(GREEN, "(TOUCH_ACTION_2)");
#endif

		//	if( GetDisplayedOSD()==0)
		Touch_Screen_Handler(_TOUCH_COUNT_2);
		break;
	case _SYSTEM_TIMER_EVENT_TOUCH_ACTION_3:
#if (_DEBUG_MESSAGE_GT911 == ON)
		GraphicsPrint(GREEN, "(TOUCH_ACTION_3)");
#endif
		Touch_Screen_Handler(_TOUCH_COUNT_3);
		break;
	case _SYSTEM_TIMER_EVENT_VX8812_INIT:
		SET_VX8812_SD_Video_Out(_NTSC);
		//	SET_VX8812_SD_Video_Out(_PAL);
		break;
	case _SYSTEM_TIMER_EVENT_TOUCH_SLIDER_TIME_OUT:
		slider_event = _FALSE;
#if (_DEBUG_MESSAGE_GT911 == ON)
		GraphicsPrint(RED, "(slider_event=_FALSE)");
#endif
		break;

	default:
		break;
	}
#if 0
    switch(enumEventID)
    {
        case _SYSTEM_TIMER_EVENT_SOURCE_CLEAR_DETECTING:            
            SysSourceClearDetectingEvent();            
            break;

        case _SYSTEM_TIMER_EVENT_JUDGE_ACTIVE_STATE:
            SET_MODE_ACTIVE_READY();
            break;

        case _SYSTEM_TIMER_EVENT_JUDGE_NOSUPPORT_STATE:
            SET_MODE_RESET_TARGET(_MODE_ACTION_RESET_TO_NOSUPPORT);
            break;

        case _SYSTEM_TIMER_EVENT_JUDGE_NOSIGNAL_STATE:
					
            SET_MODE_RESET_TARGET(_MODE_ACTION_RESET_TO_NOSIGNAL);      
            break;

#if (_EMBEDDED_DAC_SUPPORT == _ON)
        case _SYSTEM_TIMER_EVENT_AUDIO_DAC_WAIT_VCM_INITIAL:
            ScalerAudioDACSetState(_AUDIO_DAC_CHARGE);
            break;
            
        case _SYSTEM_TIMER_EVENT_AUDIO_DAC_WAIT_VCM_CHARGE:
            ScalerAudioDACSetState(_AUDIO_DAC_DEPOP);
            break;

        case _SYSTEM_TIMER_EVENT_AUDIO_DAC_WAIT_DEPOP_TO_NORMAL:
            ScalerAudioDACDepopToNormal();
            break;
#endif // End of #if(_EMBEDDED_DAC_SUPPORT == _ON)

#if (_HP_SOURCE_SCAN_SUPPORT == _ON)
        case _SYSTEM_TIMER_EVENT_SOURCE_HP_CANCEL_MANUAL_SWITCH_PORT:
            SET_SOURCE_HP_PORT_SWITCH_TARGET((_INPUT_PORT_VALID + 1));
            break;
#endif                        
        default:
            break;
    }
#endif
}

#if 0
//--------------------------------------------------
// Description  : Timer0 Int Function
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysTimerIntProc0(void) interrupt 1
{

#if (_INSTANT_TIMER_EVENT_0 == _ON)
    // Clear Timer0 flag
    TF0 = 0;

    TL0 = g_ucTimerCountLByte0;
    TH0 = g_ucTimerCountHByte0;

    TR0 = _ON;

    UserInterfaceTimer0InstantAccess_TIMER0();
#endif

}

//--------------------------------------------------
// Description  : Timer1 Int Function
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysTimerIntProc1(void) interrupt 3
{

#if (_INSTANT_TIMER_EVENT_1 == _ON)
    // Clear Timer1 flag
    TF1 = 0;

    TL1 = g_ucTimerCountLByte1;
    TH1 = g_ucTimerCountHByte1;

    TR1 = _ON;

    UserInterfaceTimer1InstantAccess_TIMER1();
#endif

}

//--------------------------------------------------
// Description  : Timer2 Int Function
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysTimerIntProc2(void) interrupt 5
{
    // Clear Timer2 flag
    TF2 = 0;

    // Clear watchdog
    CLR_MCU_WATCH_DOG();

#if (_SYSTEM_TIMER_QUARTER_RESOLUTION == _OFF)

    g_bNotifyTimer2Int = _TRUE;

    // Timer Counter 
    g_usTimerCounter += 1;

#else

    if(g_ucTimerQuarterCount >= 3)
    {
        g_ucTimerQuarterCount = 0;
            
        g_bNotifyTimer2Int = _TRUE;

        // Timer Counter 
        g_usTimerCounter += 1;   
    }
    else
    {
        g_ucTimerQuarterCount++;
    }

    UserInterfaceTimer2InstantAccess_TIMER2();

#endif

}

#endif
