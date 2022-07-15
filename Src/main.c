/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#ifdef __GNUC__
#include <sys/unistd.h>
#endif

#include "VXIS_Extern_Define.h"
#include "sw_i2c0.h" //TP2824
#include "sw_i2c1.h" //VS4210
#include "sw_i2c2.h" //IT66121
#include "VX_swi2c.h"
#include "ite66121.h"
#include "VS4210.h"
#include "IRdef.h"
#include "TP2802.h"
#include "VX_OSD_Simple.h"
#include "monitor.h"
#include "Printf.h"
#include "Etc_eep.h"
#include "audio.h"
#include "keyremo.h"
#include "VX8812.h"

#include "VS4210_param_Mode88.h"
#include "VS4210_param_Modex26.h"

#include "GT911.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RXBUFMAX 50
#define IR_Vendor_Code_ 0xFF00

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile tByte VS4210_ADDR = 0x40;
volatile tByte VS8812_ADDR = 0x12;

// RX
tByte ManVidRes;
tByte half_scaler = 0;
tByte DebugLevel;
tByte DayNightLevel = 0;
tWord LightSensor_ADC = 0;
BYTE AutoDayNightDelay = 0;
int iBackLightLevel = 4000;
BYTE RUN_DIMMER = _DIMMER_STOP;

DATA BYTE PWMLevel = 0xFF;
BYTE TimeStep;
BYTE PowerFlag = 1;
BYTE bytPowerDownMode = 0;

DATA BYTE keytic = 0, tptic = 0;
BYTE SelectModeType;
BYTE PreSelectModeType = 12; // VS4210_LOOP_MODE1
BYTE bytSelectCAMRD = 0;

BYTE Priority;
BYTE MenuTouchSel;
BYTE CAMA_M;
BYTE CAMB_M;
BYTE CAMC_M;
BYTE CAMR_M;
BYTE CAMD_M;

BYTE OSD_MIRROR[4] = {0, 0, 0, 0};
BYTE OSD_SPEAKER[4] = {0, 0, 0, 0};
BYTE BOOT_ISSUE = 1;
BYTE CURRENT_CH = 0;
BYTE LCD_PWR_FLAG = 0;
BYTE BOX_ISSUE = 0;
WORD BAT_VOLTAGE = 0, BAT_ADC = 0, BAT_StartupVOLT = 0;

BYTE bytAutoSleeping = 0;
BYTE bytStandByMode = 0;
BYTE bytStandByModeToTriggerMode = 0;
BYTE bytTripleScreenType = 1;
BYTE bytDistanceGaugePosition = 5;
BYTE bytDistanceGaugeWidth = 5;

BYTE bytGarbberLiner;
BYTE bytGarbberLinerPosition = 1;
WORD bytSetGarbberLinerPosition = 1;
BYTE bytGarbberLinerWidth = 1;
BYTE bytGarbberLinerMode = VS4210_LOOP_MODE_ERROR;
BYTE REV_ERROR = 0;

BYTE PWR_OFF_TO_ON = FALSE;

extern DATA BYTE keytic;
extern DATA BYTE Key;
extern bit KeyReady, RepeatKey;

extern BYTE updn_reg[4];
extern BYTE Time5ms;
extern bit ChangeKey;
extern tByte RS_buf[BUF_MAX], RS2_buf[BUF_MAX];
extern tByte RS_in, RS2_in;
extern BYTE tic02;
extern WORD tic03, tic04;
extern volatile WORD g_usTimerCounter;
extern volatile bit g_bNotifyTimer0Int;
extern BYTE AudioVol;
extern BYTE NowModeState;
extern BYTE Disp_Input;
extern BYTE OSD_LOCKF;
extern BYTE TriggerFlag;

extern BYTE MainSetSelectMode[13];
extern tByte gabVXIS_Input_mirrorH[4];
extern tByte gWindowNum;

extern BYTE TP_ACTION;
extern BYTE TP_PRES_DOWN;
extern BYTE TouchP;
extern BYTE Monitor_flag;
extern tByte count[4];
extern tByte VideoSave[4];
extern BYTE TriggerStatus;
#if (TPMSOSD == ON)
extern BYTE TP_HIGH_PSI[6];
extern BYTE TP_LOW_PSI[6];
extern BYTE TP_HIGH_TEMP[6];
#endif
extern int iBackLightLevel;
extern WORD standby_count;
extern BYTE bytTriggerTest;
extern bit AccessEnable;

extern BYTE SEQFlag;
extern tByte bSignal[4];

// Extern OSD 20200110-01
extern const unsigned char UserDefineFont1[(40 * (EXTERNOSDNUMBER))]; // extern OSD

tByte volatile gRxLoopFlg = 1;
T_VS4210_MutiWindows pVS4210_JointKind1;

static volatile tByte u8PowerFlg = 0;
static volatile tByte OSDSecCount = 0;
// static volatile
tByte OSDStateFlg = 1;
static volatile tByte IR_ShiftCount = 0;
static volatile tDWord IR_4BYTE = 0;
static volatile tWord u16Vendor;
static volatile tByte u8IRcode;
static volatile tByte u8IRCheckcode;
static volatile tByte PreIR_Code = 0;
static volatile tByte IR_Code = 0;
static volatile tByte IR_StartFlg = 0;
static volatile tByte IR_END_Flg = 0;
static volatile tByte IR_IRQFlg = 0;
static volatile tByte PreIR_IRQ_Index = 0;
static volatile tByte IR_IRQ_Index = 0;
static volatile tWord IR_PlusTimeVal = 0;
static volatile tDWord tim3count = 0; // 10us

static volatile tDWord TxiCount = 0;
static volatile tDWord RxiCount = 0;

static char Rx_data[2] /*, Rx_Buffer[RXBUFMAX]*/, Transfer_cplt;
static char Rx2_data[2];
static volatile int Rx_indx = 0;
static volatile int RxBufIndex = 0;
static char gRxbuf[RXBUFMAX];

static volatile int DebugFlg = 0;
extern BYTE Flisheye_CAM[4];

// static int OutMapLoopModeIndex = VS4210_LOOP_MODE_NUMBER ;
// static tByte OutMapLoopOutSizeIndex ;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

extern void MCUTimerInitialTimerEvent(void);
extern void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID);
extern BYTE RS_ready(void);
extern void SysTimerHandler(void);

/* USER CODE BEGIN PFP */
#ifdef __GNUC__

// With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf

//   set to 'Yes') calls __io_putchar()

int _write(int file, char *data, int len)
{
	if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
	{
		errno = EBADF;
		return -1;
	}
	// arbitrary timeout 1000
	HAL_StatusTypeDef status =
		HAL_UART_Transmit(&huart1, (uint8_t *)data, (uint16_t)len, 1000);
	// return # of bytes written - as best we can tell
	return (status == HAL_OK ? len : 0);
}
#else
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);
	return ch;
}
#endif

//=============================================================================
//		Serial TX
//=============================================================================
void RS2_tx(BYTE tx_buf)
{

	HAL_UART_Transmit(&huart2, &tx_buf, 1, 0xffff);
	// USART1->DR = tx_buf;
	// while(0 == (USART1->SR&(1<<6)));
}

// Sleep
/*
void msleep(unsigned int i )
{
	HAL_Delay(i);
}
*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void InitialTimerEvent(void)
{
	MCUTimerInitialTimerEvent(); // clear all of timer event
								 ///  MCUTimerActiveTimerEvent(SEC(10), _USER_TIMER_EVENT_KEY_REPEAT_ENABLE);//test
								 // MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_TEST);///for test UART3

	//	MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_CHECK_BAT_VOLTAGE);///for test UART3

#if (_DEBUG_MESSAGE_SysTimerEvent == ON)
	MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER); // for timer
#endif
	MCUTimerActiveTimerEvent(SEC(5 - 2 - 2), _SYSTEM_TIMER_EVENT_BOOT_VIDEO_DETECT); // for timer
	//	 MCUTimerActiveTimerEvent(SEC(2), _SYSTEM_TIMER_EVENT_LIGHT_SENSOR_CHECK);//for timer

	//   MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_KEY_CHECK);
	// mask ryan@20200908	 MCUTimerActiveTimerEvent(100, _SYSTEM_TIMER_EVENT_GRN_ON);

	// MCUTimerActiveTimerEvent(SEC(0.2), _SYSTEM_TIMER_EVENT_EQ_DETECT);
	// MCUTimerActiveTimerEvent(SEC(3), _SYSTEM_TIMER_EVENT_EQ_DETECT);

	// MCUTimerActiveTimerEvent(SEC(6-3),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
	//  MCUTimerActiveTimerEvent(SEC(3),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);

	MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_ADC_CHECK);
	// MCUTimerActiveTimerEvent(SEC(3),  _SYSTEM_TIMER_EVENT_VX8812_INIT);
}

void GetRS232_CMD(void)
{
	char achstr[3];
	char *endptr;
	long IrNum;
	if (Transfer_cplt == 1)
	{
		Transfer_cplt = 0;
		printf("RxBufIndex=%d,str=%s\r\n", RxBufIndex, gRxbuf);
		if (RxBufIndex >= 4)
		{
			if ((gRxbuf[0] == 'G') && (gRxbuf[3] == 'g'))
			{
				achstr[2] = '\0';
				memcpy(&achstr[0], &gRxbuf[1], 2);
				IrNum = strtol(achstr, &endptr, 16);
				printf("IrNum=0x%02x\r\n", (int)IrNum);
			}
			else if ((gRxbuf[0] == 'S') && (gRxbuf[5] == 's'))
			{
			}
		}
	}
}

// static
void setloopoutmode(int index)
{
	/*
		pVS4210_JointKind1.AudioChannel = HDMI_AUDIO_NONE ;
		  pVS4210_JointKind1.DrawBoxFlg =1 ;
		  pVS4210_JointKind1.DrawOSDFlg =0x03 ; //bits 0 ,1
	//	pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE1 ;
		  pVS4210_JointKind1.OutputSize = _OUTPUT_800x480;//_OUTPUT_1080P60 ;
	*/
	switch (index)
	{
	case VS4210_LOOP_MODE0_w0:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w0;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		OSD_MIRROR[0] = CAMA_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		printf("VS4210_MODE0_w0\r\n");
		OSD_SPEAKER[0] = ON;
		break;
	}
	case VS4210_LOOP_MODE0_w1:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w1;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		OSD_MIRROR[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		printf("VS4210_MODE0_w1\r\n");
		break;
	}
	case VS4210_LOOP_MODE0_w2:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w2;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		OSD_MIRROR[0] = CAMC_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		printf("VS4210_MODE0_w2\r\n");
		break;
	}
	case VS4210_LOOP_MODE0_w3:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w3;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		OSD_MIRROR[0] = CAMR_M;
		gabVXIS_Input_mirrorH[3] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		//	CAM_R_SW(ON);
		//	CAM_D_SW(OFF);
		printf("VS4210_MODE0_w3\r\n");
		break;
	}
	case VS4210_LOOP_MODE0_w4:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w4;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		OSD_MIRROR[0] = CAMD_M;
		gabVXIS_Input_mirrorH[3] = CAMD_M;
		OSD_SPEAKER[0] = ON;
		//	CAM_R_SW(OFF);
		//	CAM_D_SW(ON);

		printf("VS4210_MODE0_w4\r\n");
		break;
	}
	case VS4210_LOOP_MODE1:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE1;

		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		pVS4210_JointKind1.WindowMap.WMap3 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMC_M;
		OSD_MIRROR[3] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		gabVXIS_Input_mirrorH[3] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		OSD_SPEAKER[3] = ON;

		printf("VS4210_MODE1\r\n");
		break;
	}

	case VS4210_LOOP_MODE2:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE2;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		pVS4210_JointKind1.WindowMap.WMap3 = SV4;
		printf("VS4210_MODE2\r\n");
		break;
	}
	case VS4210_LOOP_MODE8:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		pVS4210_JointKind1.WindowMap.WMap3 = SV0;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMC_M;
		OSD_MIRROR[3] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		gabVXIS_Input_mirrorH[3] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE8 ch1 ch2 ch3\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_412:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV2;
		pVS4210_JointKind1.WindowMap.WMap3 = SV3;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;

		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch4 ch1 ch2\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_123:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;

		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch1 ch2 ch3\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_124:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = ON;

		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch1 ch2 ch4\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_134:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMC_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = ON;
		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch1 ch3 ch4\r\n");
		break;
	}
	//=============
	case VS4210_LOOP_MODE8_213:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;

		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch2 ch1 ch3\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_214:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = ON;

		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch2 ch1 ch4\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_234:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMC_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = ON;

		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch2 ch3 ch4\r\n");
		break;
	}

	//=============
	case VS4210_LOOP_MODE8_312:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV2;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch3 ch1 ch2\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_314:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = ON;
		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch3 ch1 ch4\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_324:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = ON;
		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch3 ch2 ch4\r\n");
		break;
	}
		//=============

	case VS4210_LOOP_MODE8_413:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch4 ch1 ch3\r\n");
		break;
	}
	case VS4210_LOOP_MODE8_423:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE8;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		//  pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE8 ch4 ch2 ch3\r\n");
		break;
	}

	//=============
	case VS4210_LOOP_MODE10_123:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;

		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		pVS4210_JointKind1.WindowMap.WMap3 = SV4;

		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMC_M;
		OSD_MIRROR[2] = CAMC_M;
		OSD_MIRROR[3] = CAMR_M;

		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		gabVXIS_Input_mirrorH[3] = CAMR_M;

		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		OSD_SPEAKER[3] = OFF;

		//   pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE10 ch1 ch2 ch3\r\n");
		break;
	}
	case VS4210_LOOP_MODE10_412:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV2;
		pVS4210_JointKind1.WindowMap.WMap3 = SV0;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		//	 pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE10 ch4 ch1 ch2\r\n");
		break;
	}
	case VS4210_LOOP_MODE10_413:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		//	 pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE10 ch4 ch1 ch3\r\n");
		break;
	}
	case VS4210_LOOP_MODE10_423:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		//	 pVS4210_JointKind1.WindowMap.WMap3 =4 ;
		printf("VS4210_MODE10 ch4 ch2 ch3\r\n");
		break;
	}

	case VS4210_LOOP_MODE9:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		pVS4210_JointKind1.WindowMap.WMap3 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMC_M;
		OSD_MIRROR[3] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		gabVXIS_Input_mirrorH[3] = CAMR_M;
		OSD_SPEAKER[0] = ON; // OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		OSD_SPEAKER[3] = OFF; // ON;
		printf("VS4210_MODE9\r\n");
		break;
	}
	case VS4210_LOOP_MODE9_412:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV2;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMB_M;
		OSD_SPEAKER[0] = ON; // OFF;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch4 ch1 ch2\r\n");
		break;
	}
	case VS4210_LOOP_MODE9_123:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch1 ch2 ch3\r\n");
		break;
	}
	break;
	case VS4210_LOOP_MODE9_124:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch1 ch2 ch4\r\n");
		break;
	}

	case VS4210_LOOP_MODE9_134:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMC_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch1 ch3 ch4\r\n");
		break;
	case VS4210_LOOP_MODE9_213:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch2 ch1 ch3\r\n");
		break;
	case VS4210_LOOP_MODE9_214:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch2 ch1 ch4\r\n");
		break;
	case VS4210_LOOP_MODE9_234:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMC_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch2 ch3 ch4\r\n");
		break;
	case VS4210_LOOP_MODE9_312:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV2;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch3 ch1 ch2\r\n");
		break;
	case VS4210_LOOP_MODE9_314:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch3 ch1 ch4\r\n");
		break;
	case VS4210_LOOP_MODE9_324:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE9;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE9 ch3 ch2 ch4\r\n");
		break;
		//	case VS4210_LOOP_MODE10_123 :
		//		break;
	case VS4210_LOOP_MODE10_124:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE10 ch1 ch2 ch4\r\n");
		break;
	case VS4210_LOOP_MODE10_134:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMC_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE10 ch1 ch3 ch4\r\n");
		break;
	case VS4210_LOOP_MODE10_213:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE10 ch2 ch1 ch3\r\n");
		break;
	case VS4210_LOOP_MODE10_214:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE10 ch2 ch1 ch4\r\n");
		break;
	case VS4210_LOOP_MODE10_234:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMC_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE10 ch2 ch3 ch4\r\n");
		break;
	case VS4210_LOOP_MODE10_312:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV2;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE10 ch3 ch1 ch2\r\n");
		break;
	case VS4210_LOOP_MODE10_314:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMA_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE10 ch3 ch1 ch4\r\n");
		break;
	case VS4210_LOOP_MODE10_324:
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE10;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV4;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[2] = OFF;
		printf("VS4210_MODE10 ch3 ch2 ch4\r\n");
		break;
	case VS4210_LOOP_MODE12_12:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		printf("VS4210_MODE12 ch1 ch2\r\n");
		break;
	}
	case VS4210_LOOP_MODE12_14:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV4;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = ON;
		printf("VS4210_MODE12 ch1 ch4\r\n");
		break;
	}
	case VS4210_LOOP_MODE12_41:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMA_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		printf("VS4210_MODE12 ch4 ch1\r\n");
		break;
	}
	case VS4210_LOOP_MODE12_42:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		printf("VS4210_MODE12 ch4 ch2\r\n");
		break;
	}
	case VS4210_LOOP_MODE12_43:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV4;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		OSD_MIRROR[0] = CAMR_M;
		OSD_MIRROR[1] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMR_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		printf("VS4210_MODE12 ch4 ch3\r\n");
		break;
	}
	//==============
	case VS4210_LOOP_MODE12_21:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMA_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMR_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		printf("VS4210_MODE12 ch2 ch1\r\n");
		break;
	}
	case VS4210_LOOP_MODE12_23:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		printf("VS4210_MODE12 ch2 ch3\r\n");
		break;
	}
	case VS4210_LOOP_MODE12_24:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV2;
		pVS4210_JointKind1.WindowMap.WMap1 = SV4;
		OSD_MIRROR[0] = CAMB_M;
		OSD_MIRROR[1] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMB_M;
		gabVXIS_Input_mirrorH[1] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = ON;
		printf("VS4210_MODE12 ch2 ch4\r\n");
		break;
	}

	//==============
	case VS4210_LOOP_MODE12_13:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV3;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;

		printf("VS4210_MODE12 ch1 ch3\r\n");
		break;
	}
	case VS4210_LOOP_MODE12_32:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMB_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;

		printf("VS4210_MODE12 ch3 ch2\r\n");
		break;
	}

	case VS4210_LOOP_MODE12_34:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV4;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMR_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMR_M;
		OSD_SPEAKER[0] = OFF;
		OSD_SPEAKER[1] = ON;

		printf("VS4210_MODE12 ch3 ch4\r\n");
		break;
	}
		//========================

	case VS4210_LOOP_MODE12_31:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE12;
		pVS4210_JointKind1.WindowMap.WMap0 = SV3;
		pVS4210_JointKind1.WindowMap.WMap1 = SV1;
		OSD_MIRROR[0] = CAMC_M;
		OSD_MIRROR[1] = CAMA_M;
		gabVXIS_Input_mirrorH[0] = CAMC_M;
		gabVXIS_Input_mirrorH[1] = CAMA_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;

		printf("VS4210_MODE12 ch3 ch1\r\n");
		break;
	}

		//=======================
	case VS4210_LOOP_MODE14:
	{
		pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE14;
		pVS4210_JointKind1.WindowMap.WMap0 = SV1;
		pVS4210_JointKind1.WindowMap.WMap1 = SV2;
		pVS4210_JointKind1.WindowMap.WMap2 = SV3;
		OSD_MIRROR[0] = CAMA_M;
		OSD_MIRROR[1] = CAMB_M;
		OSD_MIRROR[2] = CAMC_M;
		gabVXIS_Input_mirrorH[0] = CAMA_M;
		gabVXIS_Input_mirrorH[1] = CAMB_M;
		gabVXIS_Input_mirrorH[2] = CAMC_M;
		OSD_SPEAKER[0] = ON;
		OSD_SPEAKER[1] = OFF;
		OSD_SPEAKER[3] = OFF;

		printf("VS4210_MODE14 ch1 ch2\r\n");
		break;
	}
	}

	// if(index==VS4210_LOOP_MODE0_w4)
	//{
	//	CAM_R_SW(OFF);
	//	CAM_D_SW(ON);
	// }
	// else  ///default R
	//{
	//	CAM_R_SW(ON);
	//	CAM_D_SW(OFF);
	// }

	VS4210_SetMutiWindows(&pVS4210_JointKind1);

	// AUDIO_SELECT(index);
	// AudioMute(ON);

	Reset_OSD_SPEAKER();

	MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT);

	AUDIO_SELECT(index);

	if ((index == 4) && (TriggerFlag == 0))
		MCUTimerActiveTimerEvent(SEC(1 + 1), _SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT);
	else if (TriggerStatus == 5) /// Trigger CAM  D
		MCUTimerActiveTimerEvent(SEC(1 + 1 + 1), _SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT);
	else if (index <= 3)
		MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT);
	else
		MCUTimerActiveTimerEvent(SEC(1 - 0.5), _SYSTEM_TIMER_EVENT_AUDIO_CHANNEL_SELECT);

	CURRENT_CH = index;

	// ChangeVol(0);
}

/*
static int GetKet_State(void)
{
	int re = 0  ;

	#if 0
	if (HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin ) == 0)
	{
		re =1 ;
		  re =0 ;
	 //   printf("SW1 Key\r\n") ;
	}
	else if (HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin ) == 0)
	{
		re =2 ;
		 re =0 ;
	//    printf("SW2 Key\r\n") ;
	}
	else if (HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin ) == 0)
	{
		re =3 ;
		printf("SW3 Key\r\n") ;
	}
	else if (HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin ) == 0)
	{
		re =4 ;
		printf("SW4 Key\r\n") ;
	}
	else if (HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin ) == 0)
	{
	   // re =5 ;
	  //  printf("SW5 Key\r\n") ;
	  Monitor();
	}
	else if (HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin ) == 0)
	{
		re =6 ;
		printf("SW6 Key\r\n") ;
	}
	else if (HAL_GPIO_ReadPin(SW7_GPIO_Port, SW7_Pin ) == 0)
	{
		re =7 ;
		printf("SW7 Key\r\n") ;
	}
#endif

	return re ;
}
*/
//=============================================================================
//    Main Initialize Routine
//=============================================================================
void main_init(void)
{
	WORD rev;

	rev = GetFWRevEE();
	Printf("\r\nPrev.FW:A%01x.%02x.%04x\r\n", (WORD)FWVER1, (WORD)FWVER2, rev);

	if (GetFWRevEE() != FWVER) // exist EEPROM
	{
		Printf("\r\nCurr.FW:A%01x.%02x.%04x", (WORD)FWVER1, (WORD)FWVER2, (WORD)FWVER);
		SaveFWRevEE(FWVER);
		Printf("\r\nCurr.FW:A%01x.%02x.%04x", (WORD)FWVER1, (WORD)FWVER2, (WORD)GetFWRevEE());
	}

	if (GetFWRevEE() == FWVER)
	{ // exist EEPROM
		if (rev != FWVER)
		{
			REV_ERROR = 1;
			ClearBasicEE();
			REV_ERROR = 0;
			Printf("\r\n[MSG]main_init()=> rev != FWVER & call ClearBasicEE()\r\n"); // pinchi test
		}
		else
		{
			; // DebugLevel = GetDebugLevelEE();
		}
	}
	else
	{
		Printf("\r\nCannot access EEPROM!!!!");
		Printf("\r\nNo initialize");
		DebugLevel = 0xff;
		return;
	}

#if (TH0701024600NYR50S1 == ON)
	Printf("\r\n(Panel 1024X600 Parameter Installed)\r\n", rev);
#endif
	//	delay(100);

	//	SET_POWER_STATUS(_POWER_STATUS_OFF);
	//	SET_TARGET_POWER_STATUS(_POWER_STATUS_NORMAL);
}

void AdjustAutoDayNight(BYTE val)
{
	//	BYTE page;
	BYTE PWMLevelTemp = 0;
	// val=0xff-val;

#if 1 // ryan@20170928
	if (val >= 0 && val < (100))
		PWMLevelTemp = 0 + 10 + 5; /// 70
	else if (val >= (100) && val < 140)
		PWMLevelTemp = 20; // 20;
	else if (val >= (140) && val < 180)
		PWMLevelTemp = 40; // 50;
	else if (val >= 180 && val < 220)
		PWMLevelTemp = 60;			   // 65;
	else if (val >= 220 && val <= 255) // andy 20110414
	{
		//#ifdef AWT
		PWMLevelTemp = 75; // 80;
						   //#else
						   // PWMLevelTemp=115;
						   //#endif
						   //	PWMLevelTemp=128;
	}

#else
	if (val >= 0 && val < 100)
		PWMLevelTemp = 0;
	else if (val >= 100 && val < 140)
		PWMLevelTemp = 30; // 20;
	else if (val >= 140 && val < 180)
		PWMLevelTemp = 60; // 50;
	else if (val >= 180 && val < 220)
		PWMLevelTemp = 80;			   // 65;
	else if (val >= 220 && val <= 255) // andy 20110414
	{
		//#ifdef AWT
		PWMLevelTemp = 110; // 80;
							//#else
							// PWMLevelTemp=115;
							//#endif
	}
#endif
	// else
	//	PWMLevelTemp=0;

	if (DayNightLevel == 1)
	{
		// if(AutoDayNightDelay<(5+5+5))
		//		AutoDayNightDelay++;
		// else
		//{
		if (PWMLevel > PWMLevelTemp)
			PWMLevel -= 1;
		else if (PWMLevel < PWMLevelTemp)
			PWMLevel += 1;

		AutoDayNightDelay = 0;
		//}

		//	PWMLevel=PWMLevelTemp;

		//	Printf("(PWMLevel1=%02x %02x)",(WORD)PWMLevel,(WORD)PWMLevelTemp);
		if (PWMLevel >= 250)
			PWMLevel = 250;

		user_pwm_setvalue(PWMLevel * 16);
#if (_DEBUG_MESSAGE_LIGHTSENSOR == ON)
		Printf("(PWMLevel1=%02x)", (WORD)PWMLevel);
#endif
	}
	else
	{
		//	PWMLevel=PWMLevelTemp;
		// PWMLevel=255/*0x80*/;
		//		Printf("(PWMLevel2=%02x)",(WORD)PWMLevel);
		//	user_pwm_setvalue(_PWMLevelMax);
		//	if(PWMLevel>250)
		//		{
		//	PWMLevel=250; //0xFA
		//}

#if 0
	

	
		if((PWMLevel *16)> iBackLightLevel)
			{
			PWMLevel-=1;
			user_pwm_setvalue(PWMLevel*16);
#if (_DEBUG_MESSAGE_LIGHTSENSOR == ON)
			Printf("(D)");
#endif
			}
		/*
		else if((PWMLevel*16) < iBackLightLevel)
			{
			PWMLevel+=1;
			user_pwm_setvalue(PWMLevel*16);
			Printf("(U)");
			}
		*/
		else 
			{
			PWMLevel=(iBackLightLevel/16);
			user_pwm_setvalue(iBackLightLevel);
#if (_DEBUG_MESSAGE_LIGHTSENSOR == ON)
			Printf("(X)");
#endif
			}
#endif
		PWMLevel = (iBackLightLevel / 16);
		user_pwm_setvalue(iBackLightLevel);
#if (_DEBUG_MESSAGE_LIGHTSENSOR == ON)
		Printf("(PWMLevel2=%02x (%02x%02x))", (WORD)PWMLevel, (WORD)(iBackLightLevel >> 8), (WORD)(iBackLightLevel & 0xff));
#endif
	}

#if 0

	//Printf("\r\nLADC0=%02x",(WORD)val);
	page = ReadTW88( 0xff );
	WriteTW88( 0xff, 0x00 );		// set page 0
	WriteTW88( 0xbe, ((ReadTW88(0xbe) & 0xcf) | 0x10) );	// set active Low control, if want active high clear 
	//WriteTW88( 0xc4, ((ReadTW88(0xc4) & 0x80)|(val>>2)) );	// set pwm number
	WriteTW88( 0xc4, ((ReadTW88(0xc4) & 0x80)|(PWMLevel)) );	// set pwm number
	WriteTW88( 0xff, page );

#endif
	/*
	if(DayNightLevel==0)
	user_pwm_setvalue(4000);
	else
	user_pwm_setvalue(PWMLevel*15);
*/
}
void LCDPowerON(BYTE set)
{

	if (set == ON)
	{
#if (_DEBUG_MESSAGE_Panel == ON)
		Printf("(LCDPowerON1)");
#endif
		TTLI1_RES(OFF);
		PANEL_EN(ON);
		msleep(50);
		msleep(100);
		// msleep(200) ;///for new 7in panel  	LCM : TH070800480NYR40S1-L
		LCD_PWR(ON);

		msleep(50);

		//	MCUTimerDelayXms(50);
		BL_ENABLE(ON);
		HAL_GPIO_WritePin(PA7_GPIO_Port, PA7_Pin, 0);

		//	BL_PWM(ON);
	}
	else
	{
#if (_DEBUG_MESSAGE_Panel) == ON
		Printf("(LCDPowerON0)");
#endif
		TTLI1_RES(OFF);
		PANEL_EN(ON);
		msleep(25);
		// msleep(100) ;
		// msleep(200) ;///for new 7in panel 	LCM : TH070800480NYR40S1-L
		LCD_PWR(ON);

		msleep(25);

		//	MCUTimerDelayXms(50);
		//	BL_ENABLE(ON);
		HAL_GPIO_WritePin(PA7_GPIO_Port, PA7_Pin, 0);

		// BL_PWM(ON);
	}
	if (BOOT_ISSUE == 0)
		AudioMute(OFF);

	LCD_PWR_FLAG = ON;
}

void LCDPowerOFF(void)
{

	Printf("(LCDPowerOFF)");

#if 0

	TTLI1_RES(OFF);
	PANEL_EN(ON);
	msleep(50) ;
//	MCUTimerDelayXms(50);
	LCD_PWR(ON);

	msleep(50) ;

//	MCUTimerDelayXms(50);
	BL_ENABLE(ON);
//	BL_PWM(ON);

#else
	BL_ENABLE(OFF);
	HAL_GPIO_WritePin(PA7_GPIO_Port, PA7_Pin, 1);

	//	BL_PWM(OFF);
	//	msleep(50) ;
	//	MCUTimerDelayXms(50);
	TTLI1_RES(ON);
	PANEL_EN(OFF);
	//	MCUTimerDelayXms(50);
	//	msleep(50) ;

	LCD_PWR(OFF);
#endif

	AudioMute(ON);

	LCD_PWR_FLAG = OFF;
}

void BackLightON(void)
{
	BL_ENABLE(ON);

#if (StartPowerBlackScreen == ON)
	if (BOOT_ISSUE == 0)
		SETTW8812ON();
#else
	SETTW8812ON();
#endif
}

void BackLightOFF(void)
{
	AudioMute(ON);
	BL_ENABLE(OFF);
	SETTW8812OFF();
}
void SETTW8812OFF(void)
{
	VS8812Write(0x32, 0x00);
}

void SETTW8812ON(void)
{
	VS8812Write(0x32, 0x80);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */

void uComOnChipInitial(void)
{

	// power reset

	// power
	PCON3V3_TP(ON);
	PCON1V2_TP(ON);

	VS4210_RST(OFF);

#if 0

	Set_USART1();


	//Set_USART2();
	//Set_USB();
	//USB_Interrupts_Config();
	Set_GPIO_Port();

	//AUDIO_OUT_Flag=ReadEEP(0xae);
	MUTE_OUT(1);



	Set_Timer();
	Set_Init_GPIO();

	TIM4_PWM(0);
	Set_Battery_ADC();

	EXTI_Configuration();

	//Set_I2C_Port();
	//Set_IR_Port();
	//Set_ADC_Port();
	
GPIO_SetBits(GPIOA, GPIO_Pin_1);//TW8816 reset
GPIO_ResetBits(GPIOA, GPIO_Pin_0);//TW8816 reset

GPIO_SetBits(GPIOA, GPIO_Pin_12);//ryan@20150525
GPIO_SetBits(GPIOA, GPIO_Pin_13);//ryan@20150525
GPIO_SetBits(GPIOA, GPIO_Pin_14);//ryan@20150525

GPIO_SetBits(GPIOC, GPIO_Pin_9);//ryan@20150525
GPIO_SetBits(GPIOC, GPIO_Pin_13);//ryan@20150525
GPIO_SetBits(GPIOC, GPIO_Pin_14);//ryan@20150525
GPIO_SetBits(GPIOC, GPIO_Pin_15);//ryan@20150525

GPIO_SetBits(GPIOC, GPIO_Pin_7);//ryan@20150525

//GPIO_ResetBits(GPIOC, GPIO_Pin_8);//ryan@20150525
PC_SW(1);//MP board
DC_OUT(0);//MP board

#if __USE_SPI_FLASH__ == 1
	Reset_SPI();
	Set_SPI();
#endif

	//Set_USBClock();
	//USB_Interrupts_Config();
	//USB_Init();

#endif
}

void user_pwm_setvalue(uint16_t value)
{
	TIM_OC_InitTypeDef sConfigOC;

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = value;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}

void LoadEEPROM(void)
{
	PowerFlag = ReadEEP(EEP_Powerflag);
	SelectModeType = ReadEEP(EEP_SelectMode);

	OSD_LOCKF = ReadEEP(EEP_ONOFFChoice);
	Disp_Input = ReadEEP(EEP_COSDMode);

	TimeStep = ReadEEP(EEP_TimeStep);
	Priority = ReadEEP(EEP_PRIORITY);
	DayNightLevel = ReadEEP(EEP_AutoMode);
	NowModeState = ReadEEP(EEP_NowModeState);
	MenuTouchSel = ReadEEP(EEP_MenuTouch);

	AudioVol = GetAudioVolEE();
	CAMA_M = ReadEEP(EEP_CAMAMode);
	CAMB_M = ReadEEP(EEP_CAMBMode);
	CAMC_M = ReadEEP(EEP_CAMCMode);
	CAMR_M = ReadEEP(EEP_CAMRMode);
	CAMD_M = ReadEEP(EEP_CAMDMode);

	// AudioVol = GetAudioVolEE();

	bytAutoSleeping = ReadEEP(EEP_Auto_Sleeping);
	bytTripleScreenType = ReadEEP(EEP_Triple_Screen_Type);
	bytDistanceGaugePosition = ReadEEP(EEP_Distance_Gauge_Position);
	bytDistanceGaugeWidth = ReadEEP(EEP_Distance_Gauge_Width);

	bytGarbberLiner = ReadEEP(EEP_Grabber_Liner);
	bytGarbberLinerPosition = ReadEEP(EEP_Grabber_Liner_Position);
	bytGarbberLinerWidth = ReadEEP(EEP_Grabber_Liner_Width);

	bytSetGarbberLinerPosition = GetOSDStep(0x62, bytGarbberLinerPosition);

	VideoSave[0] = ReadEEP(EEP_TP2824_STD0 + (0 * 3) + 2);
	VideoSave[1] = ReadEEP(EEP_TP2824_STD0 + (1 * 3) + 2);
	VideoSave[2] = ReadEEP(EEP_TP2824_STD0 + (2 * 3) + 2);
	VideoSave[3] = ReadEEP(EEP_TP2824_STD0 + (3 * 3) + 2);

	/*
		if(SelectModeType==SelectCameraD)
			{
			VideoSave[3] = ReadEEP(EEP_TP2824_STD0+(4*3)+2);
			bytSelectCAMRD=SelectCameraD;
			CAM_R_SW(OFF);
			CAM_D_SW(ON);
			}
		else
			{
			VideoSave[3] = ReadEEP(EEP_TP2824_STD0+(3*3)+2);
			bytSelectCAMRD=SelectCameraR;
			CAM_R_SW(ON);
			CAM_D_SW(OFF);
			}
		*/
	Flisheye_CAM[0] = ReadEEP(EEP_FlishCAM1);
	Flisheye_CAM[1] = ReadEEP(EEP_FlishCAM2);
	Flisheye_CAM[2] = ReadEEP(EEP_FlishCAM3);
	Flisheye_CAM[3] = ReadEEP(EEP_FlishCAM4);

#if (TPMSOSD == ON)
	TP_HIGH_PSI[0] = ReadEEP(EEP_TIRE1_High_PSI);
	TP_HIGH_PSI[1] = ReadEEP(EEP_TIRE2_High_PSI);
	TP_HIGH_PSI[2] = ReadEEP(EEP_TIRE3_High_PSI);
	TP_HIGH_PSI[3] = ReadEEP(EEP_TIRE4_High_PSI);
	TP_HIGH_PSI[4] = ReadEEP(EEP_TIRE5_High_PSI);
	TP_HIGH_PSI[5] = ReadEEP(EEP_TIRE6_High_PSI);

	TP_LOW_PSI[0] = ReadEEP(EEP_TIRE1_Low_PSI);
	TP_LOW_PSI[1] = ReadEEP(EEP_TIRE2_Low_PSI);
	TP_LOW_PSI[2] = ReadEEP(EEP_TIRE3_Low_PSI);
	TP_LOW_PSI[3] = ReadEEP(EEP_TIRE4_Low_PSI);
	TP_LOW_PSI[4] = ReadEEP(EEP_TIRE5_Low_PSI);
	TP_LOW_PSI[5] = ReadEEP(EEP_TIRE6_Low_PSI);

	TP_HIGH_TEMP[0] = ReadEEP(EEP_TIRE1_High_Temp);
	TP_HIGH_TEMP[1] = ReadEEP(EEP_TIRE2_High_Temp);
	TP_HIGH_TEMP[2] = ReadEEP(EEP_TIRE3_High_Temp);
	TP_HIGH_TEMP[3] = ReadEEP(EEP_TIRE4_High_Temp);
	TP_HIGH_TEMP[4] = ReadEEP(EEP_TIRE5_High_Temp);
	TP_HIGH_TEMP[5] = ReadEEP(EEP_TIRE6_High_Temp);
#endif
}

void WaitPowerOn(void)
{
	BYTE val;

	if (TriggerFlag == 0)
	{
		PowerLED(_SYSTEM_TIMER_EVENT_RED_ON);

		LCDPowerOFF();

		CloseOSDMenu();

		MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
	}
	Printf("\r\n(Power off..)");

	RUN_DIMMER = _DIMMER_STOP;

	bytStandByModeToTriggerMode = OFF;

	while (1)
	{

		CheckKeyIn();
		if (PowerFlag == ON)
		{

			// PowerFlag=ON;
			WriteEEP(EEP_Powerflag, PowerFlag); // 2007.3.28 Justin
			break;								// poweron
		}

		val = CheckTrigger();

		//		  TriggerHandler(val);

		if (val != 0)
		{
			PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);
			TriggerHandler(val);
			//	AudioMute(OFF);
			// DisplayInput();		//william v0.2 20120223
			/*
			if(!(TriggerMode==0x08))
			{
				//Printf("\r\n(**PowerFlag=%02x)",(WORD)PowerFlag);
				LCDPowerON(1);//�ץ�power on����ܤӺC���D
				AudioMute(1);
			}
			*/
		}
		else
		{
			// Printf("\r\n(*NO-TRIG)");
			TriggerHandler(val); // Pinchi 20140922 add for AWT ML072Q

			if (TriggerFlag == 0)
			{
				// PowerOff();
				//	AudioMute(ON);

				// PowerLED(_SYSTEM_TIMER_EVENT_RED_ON);
				/*
				if(live_out_state==1)	////live out issue
				{
				live_out_state=0;
				SetNowMode();
				Panel_power=0;
				}
				*/
			}

			//	TriggerHandler(val);  //Pinchi 20140922 move to above for AWT ML072Q
		}

		SysTimerHandler();
		EQ_Detect();

		// if(GT9147_Scan(1)>=_TOUCH_COUNT_1)
		// PowerFlag=ON;
	}

	PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);
	SHOWOSDINFO();

	if ((NowModeState == JumpMode) && (TriggerFlag == 0))
		ShowJUMPOSD();

	MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_DISPLAY_DISABLE);
	MCUTimerActiveTimerEvent(SEC(2), _USER_TIMER_EVENT_DISPLAY_DISABLE);
	MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_GRABBER_SETUP);
	MCUTimerActiveTimerEvent(SEC(0.5), _SYSTEM_TIMER_EVENT_GRABBER_SETUP);

	MCUTimerActiveTimerEvent(SEC(_BACKLIGHT_ON_Time_out), _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
	// LCDPowerON();

	bytStandByMode = OFF;
	bytPowerDownMode = OFF;
	PWMLevel = (iBackLightLevel / 16);
	PWR_OFF_TO_ON = TRUE;
	Printf("\r\n(Power ON..)");
}

int main(void)
{
	/* USER CODE BEGIN 1 */
	tByte val;
	//  int keystate ;

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	InitialTimerEvent();

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();

	LCDPowerOFF();
	// SelectModeType = ReadEEP(EEP_SelectMode);
	// AUDIO_SELECT_MUTE(SelectModeType);

	// SET_PCA9554PW(_Shutter_SW,OFF);
	// SET_PCA9554PW(_CCD12_SW,ON);
	// SET_PCA9554PW(_PCON5V_AUDIO,ON);
	// SET_PCA9554PW(_COL_PW_5V,OFF);

	//  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin , GPIO_PIN_SET);
	//  HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin , GPIO_PIN_SET);

	// PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);

	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_ADC1_Init();

	// SelectModeType = ReadEEP(EEP_SelectMode);

	// AUDIO_SELECT_MUTE(SelectModeType);
	/* USER CODE BEGIN 2 */

	// HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);

	SW_I2C0_Init();
	SW_I2C1_Init();
	SW_I2C2_Init();
	SW_I2C3_Init();

	if (ReadEEP(EEP_Powerflag))
		PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);
	else
		PowerLED(_SYSTEM_TIMER_EVENT_RED_ON);

	AudioMute(ON);

	SelectModeType = ReadEEP(EEP_SelectMode);

	AUDIO_SELECT_MUTE(SelectModeType);

#if (StartPowerBlackScreen == OFF)
	user_pwm_setvalue(_PWMLevelMax);
#endif

	uComOnChipInitial();
	AudioVol = GetAudioVolEE();

	BAT_StartupVOLT = GET_LTC2451(0);

#if (BOOT_SET_VX8812_FIRST == ON)
	SET_VX8812_SD_Video_Out(SET_VX8812_Video);
#endif

	LCDPowerON(0);

#if (TOUCH_FUNCTION == ON)
	GT9147_Init();
#endif

	msleep(50);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)Rx_data, 1); // activate UART receive interrupt every time

	HAL_GPIO_WritePin(TP2824_RST_GPIO_Port, TP2824_RST_Pin, 0); // TP2824 RST
	HAL_GPIO_WritePin(TX_RST_GPIO_Port, VS_RST_Pin, 0);			// TX RST
	HAL_GPIO_WritePin(VS_RST_GPIO_Port, TX_RST_Pin, 0);			// VS4210 RST

	msleep(100);
	HAL_GPIO_WritePin(TP2824_RST_GPIO_Port, TP2824_RST_Pin, 1); // TP2824 RST
	HAL_GPIO_WritePin(TX_RST_GPIO_Port, VS_RST_Pin, 1);			// TX RST
	HAL_GPIO_WritePin(VS_RST_GPIO_Port, TX_RST_Pin, 1);			// VS4210 RST

	// SET_PCA9554PW(_COL_PW_5V,ON);

	// I2CWrite(VS9988_ADDR, 0x01, 0x5A);  //reset

	printf("\r\nVS4210 sdk2.0 Compiled %s %s !\r\n", __DATE__, __TIME__);
	val = VXISI2CRead(0x00);
	printf("VS4210 ID = 0x%02X !\r\n", (int)val);

	// Extern OSD 20200110-01
	VXOSD_UseFont_init(EXTERNOSDNUMBER, &UserDefineFont1[0]);

	main_init();

	// PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON);

	// load eeprom data
	LoadEEPROM();

	VS8812Write(0x32, 0x00);

	// RX
	ManVidRes = RES1920X1080; // 0x01
	// Init_TP280x_RegSet();

	// TX
	// VS9988_Set_Format(gbVXIS_InputMode, gbVXIS_OuputModeflg, gbVXIS_OuputChannel  )	;
	// gbVXIS_InputMode = _OSD_NO_SINGLE ;
	// VS9988_BlueScreen() ;
	// VXOSD_SetOnOff(0) ;

	// HDMI_Tx_Init() ;

	// VS4210 SW Reset@start
	VXISI2CWrite(0x01, 0x5A); // Reset

	// VS4210 SW Reset@end
	// VS4210_SwitchChip(0x40, 0 ) ;c8 cc8
	pVS4210_JointKind1.AudioChannel = HDMI_AUDIO_NONE;
	pVS4210_JointKind1.DrawBoxFlg = 1;
	pVS4210_JointKind1.DrawOSDFlg = 0x03; // bits 0 ,1
	// pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE0_w0 ;
	// pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE1 ;
	pVS4210_JointKind1.OutputMode = VS4210_1080P_MODE1;
	pVS4210_JointKind1.OutputSize = _OUTPUT_800x480; //_OUTPUT_1080P60 ;

	/*

	//SetNowMode();

	// if((MainSetSelectMode[SelectModeType]>VS4210_LOOP_MODE1)&&(MainSetSelectMode[SelectModeType]<VS4210_LOOP_MODE_NUMBER))
	  {
	 Printf("\r\nBoot setup...");
	 //MCUTimerCancelTimerEvent(  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);
	// MCUTimerActiveTimerEvent(SEC(6-1),  _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);


	 pVS4210_JointKind1.WindowMap.WMap0 =1;
	 pVS4210_JointKind1.WindowMap.WMap1 =2 ;
	 pVS4210_JointKind1.WindowMap.WMap2 =3 ;
	 pVS4210_JointKind1.WindowMap.WMap3 =4 ;

	   OSD_MIRROR[0]=CAMA_M;
	   OSD_MIRROR[1]=CAMB_M;
	   OSD_MIRROR[2]=CAMC_M;
	   OSD_MIRROR[3]=CAMR_M;
	 gabVXIS_Input_mirrorH[0]=CAMA_M;
	 gabVXIS_Input_mirrorH[1]=CAMB_M;
	 gabVXIS_Input_mirrorH[2]=CAMC_M;
	 gabVXIS_Input_mirrorH[3]=CAMR_M;
	   OSD_SPEAKER[0]=OFF;
	   OSD_SPEAKER[1]=OFF;
	   OSD_SPEAKER[2]=OFF;
	   OSD_SPEAKER[3]=ON;


   VS4210_SetMutiWindows(&pVS4210_JointKind1) ;
   }
   */
	// LCDPowerON();

	// VS4210_Rx_NoSignal(3) ;

	// setloopoutmode(MainSetSelectMode[SelectModeType]) ;
	// SetNowMode();
	/*
		VS4210_Rx_NoSignal(0) ;
	  VS4210_Rx_NoSignal(1) ;
	  VS4210_Rx_NoSignal(2) ;
	  VS4210_Rx_NoSignal(3) ;
	*/
	gbVXIS_InputMode[0] = _OSD_No_Signal;
	gbVXIS_InputMode[1] = _OSD_No_Signal;
	gbVXIS_InputMode[2] = _OSD_No_Signal;
	gbVXIS_InputMode[3] = _OSD_No_Signal;

	bSignal[0] = 0;
	bSignal[1] = 0;
	bSignal[2] = 0;
	bSignal[3] = 0;

	LoadVideoData();

	SetNowMode();

	// LCDPowerON();
// void VS4210_Rx_NoSignal(tByte ch )
// Init_TP280x_RegSet();

// EQ_Detect();
#if (StartPowerBlackScreen == OFF)
	if ((PowerFlag == ON) && (CheckTrigger() == 0)) /// power offf
		BackLightON();
#endif

	Init_TP280x_RegSet();

#if (BOOT_SET_VX8812_FIRST == OFF)
	SET_VX8812_SD_Video_Out(SET_VX8812_Video);
#endif

	// GT9147_Init();

	HAL_TIM_Base_Start_IT(&htim3);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

// SetNowMode();
#if (StartPowerBlackScreen == ON)
	MCUTimerDelayXms(500 - 250);

	user_pwm_setvalue(GetBackLightLevel());
	PWMLevel = (iBackLightLevel / 16);

	SETTW8812ON();

	//	LoadVideoData();
#endif
	// AudioMute(OFF);
	//  LCDPowerON();

	//	SET_VX8812_SD_Video_Out(_NTSC);
	//	SET_VX8812_SD_Video_Out(_PAL);

#if (StartPowerBlackScreen == ON)
	if ((PowerFlag == ON) && (CheckTrigger() == 0)) /// power offf
		BackLightON();
#endif

	while (1)
	{
		/* USER CODE END WHILE */

		if ((PowerFlag == ON)) /// power offf
			main_loop();

		WaitPowerOn();

		// user_pwm_setvalue(500);

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

void main_loop(void)
{
	//#if (_DEBUG_MESSAGE_Trigger==OFF)
	// BYTE val;
	//#endif
	BYTE val;

	while (1)
	{
#if (_DEBUG_MESSAGE_Monitor == ON)
		while (RS_ready())
			Monitor();
#endif

#if (HS_DEBUG == ON)
		if (AccessEnable == 1)
		{
#endif

			SysTimerHandler();
#if (HS_DEBUG == ON)
		}
#endif

// CheckKeyIn();

// CheckTouchP();
#if (HS_DEBUG == ON)
		if ((Monitor_flag == OFF) && (AccessEnable == 1))
#endif
			EQ_Detect();

#if (TOUCH_FUNCTION == ON)
		GT9147_Scan(1);
#endif

// COC function
#if (HS_DEBUG == ON)
		if ((AccessEnable == 1) && (SEQFlag == 0))
#endif
			Get_2824cIrq();

// if( GT9147_Scan(1)==_TOUCH_COUNT_4 ) break;
#if 0 // (_DEBUG_MESSAGE_Trigger==ON)
		if(bytTriggerTest==0)
		 CheckTrigger();

		if( BOOT_ISSUE==0)
		TriggerHandler(TriggerFlag);

#else

#if (HS_DEBUG == ON)
		if (AccessEnable == 1)
		{
#endif

			val = CheckTrigger();

			if (BOOT_ISSUE == 0)
				TriggerHandler(val);

#if (HS_DEBUG == ON)
		}
#endif

#endif

#if (HS_DEBUG == ON)
		if (AccessEnable == 1)
		{
#endif

			if (!CheckKeyIn())
				break; // POWEROFF

			if (bytStandByMode == ON) // auto sleep mode
			{
				PowerFlag = OFF;
				break;
			}

			if (bytPowerDownMode == ON)
			{
				PowerFlag = OFF;
				WriteEEP(EEP_Powerflag, PowerFlag);
				break;
			}

#if (HS_DEBUG == ON)
		}
#endif
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function.
 * @param None
 * @retval None
 */
#if 1
static void MX_ADC1_Init(void)
{

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */
	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_13;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	/*
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	*/
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/* USER CODE BEGIN ADC1_Init 2 */
	HAL_ADC_MspInit(&hadc1);

	/* USER CODE END ADC1_Init 2 */
}
#endif
/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */
	/// TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = (35 / 4);
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 4000;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	/*
	  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
		{
		  Error_Handler();
		}
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
		{
		  Error_Handler();
		}
	*/
	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	//  sConfigOC.Pulse = 4000;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */
	HAL_TIM_MspPostInit(&htim2);

	// TIMOCInitStructure.TIM_Pulse =(period);

	// TIMOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// TIMOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// TIM_OC3Init(TIM3, &TIMOCInitStructure);

	// TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 35;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 9;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	//  huart1.Init.BaudRate = 9600;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */

static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, TX_RST_Pin | TP2824_RST_Pin | VS_RST_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : TX_RST_Pin TP2824_RST_Pin VS_RST_Pin */
	GPIO_InitStruct.Pin = TX_RST_Pin | TP2824_RST_Pin | VS_RST_Pin | BL_ENABLE_Pin | PCON3V3_TP_Pin | PCON1V2_TP_Pin | VS4210_RST_Pin | PA7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : IR_IN_Pin */
	GPIO_InitStruct.Pin = IR_IN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(IR_IN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : RX1_Pin */
	GPIO_InitStruct.Pin = RX1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(RX1_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DOWN_KEY_Pin | UP_KEY_Pin | MENU_KEY_Pin | POWER_KEY_Pin /*|TOUCH_INT*/;
	// GPIO_InitStruct.Pin =MENU_KEY_Pin|POWER_KEY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = (PANEL_EN_Pin | TOUCH_RESET);
	//  GPIO_InitStruct.Pin =(PANEL_EN_Pin|BL_PWM_Pin);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; /// GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	/*Configure GPIO pins : SW1_Pin SW2_Pin SW3_Pin SW4_Pin
							 SW5_Pin SW6_Pin SW7_Pin */
	//  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin|SW3_Pin|SW4_Pin
	//                      |SW5_Pin|SW6_Pin|SW7_Pin|DOWN_KEY_Pin|UP_KEY_Pin|MENU_KEY_Pin|POWER_KEY_Pin;
	/*

	GPIO_InitStruct.Pin =DOWN_KEY_Pin|UP_KEY_Pin|MENU_KEY_Pin|POWER_KEY_Pin|TOUCH_INT;
   // GPIO_InitStruct.Pin =MENU_KEY_Pin|POWER_KEY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  */
	/*
	  GPIO_InitStruct.Pin =DOWN_KEY_Pin|UP_KEY_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	*/
	GPIO_InitStruct.Pin = LCD_PWR_Pin | TTLI1_RES_Pin | AUD_SEL2_Pin | GREEN_LED_Pin | RED_LED_Pin | INPUT_SEL1 | INPUT_SEL2;
	//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	//   GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;

	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	//  GPIO_InitStruct.Pin =GREEN_LED_Pin|RED_LED_Pin;
	// GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	// GPIO_InitStruct.Pull = GPIO_NOPULL;
	// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	// HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SOURCE_KEY_Pin | JUMP_KEY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = AUD_SEL1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	//  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
	//  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	BYTE TempKey = 0;

	// 10us
	if (htim->Instance == TIM3)
	{

		// Timer Counter
		tic02++;
		tic03++;
		tic04++;

		if (tic02 >= 100)
		{
			g_usTimerCounter += 1; /// per 10us  interrupt
			tic02 = 0;
			g_bNotifyTimer0Int = _TRUE;
		}

		tim3count++;
		if (tim3count > 100000)
		{
			tim3count = 0;
			// Test_tim3_printf_Flg  = 1;
			if (OSDStateFlg == 1)
			{
				// static tByte enOSD10secOff = 1;   //En16wNumberOSD=1
				OSDSecCount++;
			}
		}

		// Kane @HS 2007 0814 Ver1.31>>>>
		//	if(Time5ms)
		//	Time5ms--;
		// if(Time5ms==0)
		{
			// if(HAL_GPIO_ReadPin(UP_KEY_GPIO_Port,UP_KEY_Pin)==_FALSE)
			//	TempKey|=0x01;
			// if(HAL_GPIO_ReadPin(DOWN_KEY_GPIO_Port,DOWN_KEY_Pin)==_FALSE)
			//	TempKey|=0x02;
			// TempKey=(( (P2&0x60) >> 5) & 0x03);
			//			TempKey=((~GPIOB->IDR)&(UP_KEY_Pin|DOWN_KEY_Pin))>>14;
			TempKey = (((~GPIOB->IDR) & (UP_KEY_Pin | DOWN_KEY_Pin)) >> 14) & 0x03;

			if (updn_reg[3] != TempKey) // (( (P2&0x60) >> 5) & 0x03))    //Abel for HS SW key ......951102
			{
				updn_reg[0] = updn_reg[1];
				updn_reg[1] = updn_reg[2];
				updn_reg[2] = updn_reg[3];
				updn_reg[3] = TempKey; // ( (P2&0x60) >> 5) & 0x03;
				ChangeKey = 1;
				// else			//william-v1.42-961130
				// ChangeKey=0;	//william-v1.42-961130
				// Time5ms = (48-8);
				// Time5ms = (48+100);
			}
		}

		if (IR_IRQFlg == 1)
		{
			if (PreIR_IRQ_Index == IR_IRQ_Index)
			{
				IR_PlusTimeVal++;

				// 9.2xms + 4.5xms = 13.8ms
				if (IR_PlusTimeVal > 15000) // 10us
				{
					//                    IR_PlusTimeVal = 0  ;
					//                    PreIR_IRQ_Index = 0 ;
					//                    IR_IRQ_Index = 0 ;
					//                    IR_IRQFlg = 0 ;

					if (IR_ShiftCount >= 32)
					{
						u16Vendor = (tWord)(IR_4BYTE & 0x0000FFFF);
						if (u16Vendor == IR_Vendor_Code_)
						{
							u8IRcode = (tByte)((IR_4BYTE & 0x00FF0000) >> 16);
							u8IRCheckcode = 0xFF - (tByte)((IR_4BYTE & 0xFF000000) >> 24);
							IR_END_Flg = 1;
						}
					}

					IR_ShiftCount = 0;
					IR_PlusTimeVal = 0;
					PreIR_IRQ_Index = 0;
					IR_IRQ_Index = 0;
					IR_IRQFlg = 0;
					// memset(aIR_PlusTime , 0 , 40 )  ;
					IR_4BYTE = 0;
					IR_StartFlg = 0;
				}
			}
			else
			{
				if (PreIR_IRQ_Index <= 33)
				{
					// aIR_PlusTime[PreIR_IRQ_Index] = IR_PlusTimeVal ;
					if (IR_StartFlg == 1)
					{
						if (IR_PlusTimeVal > 90)
						{
							// IR_4BYTE = IR_4BYTE << 1 ;
							IR_4BYTE = IR_4BYTE >> 1;

							if (IR_PlusTimeVal > 160)
							{
								// IR_4BYTE = IR_4BYTE | 0x01 ;
								IR_4BYTE = IR_4BYTE | 0x80000000;
							}
							else
							{
								// IR_4BYTE = IR_4BYTE | 0x01 ;
							}
							IR_ShiftCount++;
						}
					}

					if ((IR_PlusTimeVal > 1250) && (IR_PlusTimeVal < 1450))
					{
						IR_StartFlg = 1;
					}
				}
				IR_PlusTimeVal = 0;
				PreIR_IRQ_Index = IR_IRQ_Index;
			}
		}

		///////////////////////////////////////////
		/*
			TempKey=0;
			if(HAL_GPIO_ReadPin(MENU_KEY_GPIO_Port,MENU_KEY_Pin)==_FALSE)
				TempKey|=0x08;
			if(HAL_GPIO_ReadPin(POWER_KEY_GPIO_Port,POWER_KEY_Pin)==_FALSE)
				TempKey|=0x04;
			if(HAL_GPIO_ReadPin(POWER_KEY_GPIO_Port,POWER_KEY_Pin)==_FALSE)
				TempKey|=0x04;
			if(HAL_GPIO_ReadPin(JUMP_KEY_GPIO_Port,JUMP_KEY_Pin)==_FALSE)
				TempKey|=0x10;
			if(HAL_GPIO_ReadPin(SOURCE_KEY_GPIO_Port,SOURCE_KEY_Pin)==_FALSE)
				Te			mpKey|=0x80;
	*/
		if (tic03 == 1000)
		{
			tic03 = 0;

			if (_ReadKey())
			{

				standby_count++;

				if (keytic == 3)
				{
					Key = _ReadKey();
					RepeatKey = 0;
					KeyReady = 1;
				}
				else if (keytic == 100)
				{
					Key = _ReadKey();
					KeyReady = 1;
					keytic = 80 - 70;
				}
				keytic++;
			}
			else
			{
				if ((RUN_DIMMER == _DIMMER_READY) && (standby_count >= 1) && (standby_count <= 86) && (PowerFlag == ON))
					RUN_DIMMER = _DIMMER_RUN1;
				else if ((RUN_DIMMER == _DIMMER_RUN2) && (standby_count >= 1) && (standby_count <= 86))
					RUN_DIMMER = _DIMMER_STOP;

				PWR_OFF_TO_ON = FALSE;
				keytic = 0;
				Key = 0;
				standby_count = 0;
			}
		}

		if (tic04 == 1000)
		{
			tic04 = 0;
			if (TP_PRES_DOWN)
			{

				if (tptic == 3)
				{
					TouchP = TP_PRES_DOWN;
				}
				else if (tptic == 150)
				{
					TouchP = TP_PRES_DOWN;

					tptic = 4;
				}
				tptic++;
			}
			else
			{
				tptic = 0;
				TouchP = 0;
			}
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) // current UART
	{

		/*
				if (Rx_indx==0)
				{
					memset(Rx_Buffer , 0  , RXBUFMAX ) ;
				}

				if (Rx_data[0] >= 0x30)
				{
					Rx_Buffer[Rx_indx]=Rx_data[0];	//add data to Rx_Buffer
					Rx_indx++ ;
					if (Rx_indx >= RXBUFMAX )
						Rx_indx = 0 ;
				}
				else if ((Rx_data[0] == 0x0d) || (Rx_data[0] == 0xa))//if received data = 13
				{
					RxBufIndex = Rx_indx ;
					memcpy(gRxbuf ,Rx_Buffer, Rx_indx ) ;
					Rx_indx=0;
					Transfer_cplt=1;//transfer complete, data is ready to read
				}
		*/

		RS_buf[RS_in++] = Rx_data[0]; // USART_ReceiveData(USART1);
		if (RS_in >= BUF_MAX)
			RS_in = 0;

		// RS_tx(Rx_data[0]);

		// HAL_UART_Receive_IT(&huart1,(uint8_t*)Rx_data, 1);	//activate UART receive interrupt every time
		HAL_UART_Receive_IT(&huart1, (uint8_t *)Rx_data, 1); // activate UART receive interrupt every time
	}

	if (huart->Instance == USART2) // current UART
	{

		/*
				if (Rx_indx==0)
				{
					memset(Rx_Buffer , 0  , RXBUFMAX ) ;
				}

				if (Rx_data[0] >= 0x30)
				{
					Rx_Buffer[Rx_indx]=Rx_data[0];	//add data to Rx_Buffer
					Rx_indx++ ;
					if (Rx_indx >= RXBUFMAX )
						Rx_indx = 0 ;
				}
				else if ((Rx_data[0] == 0x0d) || (Rx_data[0] == 0xa))//if received data = 13
				{
					RxBufIndex = Rx_indx ;
					memcpy(gRxbuf ,Rx_Buffer, Rx_indx ) ;
					Rx_indx=0;
					Transfer_cplt=1;//transfer complete, data is ready to read
				}
		*/

		RS2_buf[RS2_in++] = Rx2_data[0]; // USART_ReceiveData(USART1);
		if (RS2_in >= BUF_MAX)
			RS2_in = 0;

		// RS_tx(Rx_data[0]);

		// HAL_UART_Receive_IT(&huart1,(uint8_t*)Rx_data, 1);	//activate UART receive interrupt every time
		HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx2_data, 1); // activate UART receive interrupt every time
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == IR_IN_Pin)
	{
		IR_IRQFlg = 1;

		IR_IRQ_Index++;
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
