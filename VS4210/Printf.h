/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Printf.h		                                            										*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef __PRINTF__
#define __PRINTF__
#define DATA  //_data
#define PDATA //		_pdat
#define IDATA //	idat
#define CODE  //_rom
#define PDATA_P
#define CODE_P

#if 1
//#define _outchar(c)   //while(1) { if( !RS_Xbusy ) { SBUF = c; RS_Xbusy=1; break; } }
#define _outchar(c) RS_tx(c)

#define Putch(c) _outchar(c)

#define BLACK PRINT_COLOR_BLACK
#define RED PRINT_COLOR_RED
#define GREEN PRINT_COLOR_GREEN
#define YELLOW PRINT_COLOR_YELLOW
#define BLUE PRINT_COLOR_BLUE
#define MAGENTA PRINT_COLOR_MAGENTA
#define CYAN PRINT_COLOR_CYAN
#define GRAY PRINT_COLOR_GRAY

#if (HS_DEBUG == OFF)
#define _DEBUG_MESSAGE_BatteryBTH OFF
#define _DEBUG_MESSAGE_BatteryVoltage OFF
#define _DEBUG_MESSAGE_WDT_KICK_CHECK OFF
#define _DEBUG_MESSAGE_WORKING_TIME OFF
#define _DEBUG_MESSAGE_SysJudgeHandler OFF
#define _DEBUG_MESSAGE_UART_Protocol OFF
#define _DEBUG_MESSAGE_Scankey_CHECK OFF
#define _DEBUG_MESSAGE_COLOR_MESSAGE OFF
#define _DEBUG_MESSAGE_UserInterfaceTimerEvent OFF
#define _DEBUG_MESSAGE_SysTimerEvent OFF
#define _DEBUG_MESSAGE_PowerTimerEvent OFF
#define _DEBUG_MESSAGE_Monitor ON
#define _DEBUG_EncorderHandler OFF
#define _DEBUG_CheckKeyIn OFF
#define _DEBUG_OSD OFF
#define _DEBUG_MESSAGE_VX8812 OFF
#define _DEBUG_MESSAGE_GT911 OFF
#define _DEBUG_MESSAGE_Get_2824cIrq OFF
#define _DEBUG_MESSAGE_TP2824 OFF
#define _DEBUG_MESSAGE_Trigger OFF
#define _DEBUG_MESSAGE_Source_Change OFF
#define _DEBUG_MESSAGE_Panel OFF
#define _DEBUG_MESSAGE_Audio OFF
#define _DEBUG_MESSAGE_BOOT_NOISE OFF
#define _DEBUG_MESSAGE_LIGHTSENSOR OFF
#define _DEBUG_MESSAGE_CONSOLE_TEST OFF

#else
//#warning DEBUG is defined
#define _DEBUG_MESSAGE_BatteryBTH ON
#define _DEBUG_MESSAGE_BatteryVoltage ON
#define _DEBUG_MESSAGE_WDT_KICK_CHECK ON
#define _DEBUG_MESSAGE_WORKING_TIME ON
#define _DEBUG_MESSAGE_SysJudgeHandler ON
#define _DEBUG_MESSAGE_UART_Protocol ON
#define _DEBUG_MESSAGE_Scankey_CHECK ON
#define _DEBUG_MESSAGE_COLOR_MESSAGE ON
#define _DEBUG_MESSAGE_UserInterfaceTimerEvent ON
#define _DEBUG_MESSAGE_SysTimerEvent ON
#define _DEBUG_MESSAGE_PowerTimerEvent ON
#define _DEBUG_MESSAGE_Monitor ON
#define _DEBUG_EncorderHandler ON
#define _DEBUG_CheckKeyIn ON
#define _DEBUG_OSD ON
#define _DEBUG_MESSAGE_VX8812 ON
#define _DEBUG_MESSAGE_GT911 ON
#define _DEBUG_MESSAGE_Get_2824cIrq ON
#define _DEBUG_MESSAGE_TP2824 ON
#define _DEBUG_MESSAGE_Trigger ON
#define _DEBUG_MESSAGE_Source_Change ON
#define _DEBUG_MESSAGE_Panel ON
#define _DEBUG_MESSAGE_Audio ON
#define _DEBUG_MESSAGE_BOOT_NOISE ON
#define _DEBUG_MESSAGE_LIGHTSENSOR OFF
#define _DEBUG_MESSAGE_CONSOLE_TEST ON

#endif

typedef enum _PRINT_COLOR_e_
{
    PRINT_COLOR_BLACK = 0,
    PRINT_COLOR_RED,
    PRINT_COLOR_GREEN,
    PRINT_COLOR_YELLOW,
    PRINT_COLOR_BLUE,
    PRINT_COLOR_MAGENTA,
    PRINT_COLOR_CYAN,
    PRINT_COLOR_GRAY,
    PRINT_NUM_COLOR
} PRINT_COLOR_e;

#define KEILC 1

//#ifdef SERIAL

void Printf(const char CODE_P *fmt, ...);
void Puts(CODE_P char *ptr);

#define TW2835Cmd(a) Puts(a)

//#else

// #undef  DEBUG
// #define Printf(a,b)	//
// #define Puts(a)   		//

//#endif

void DEBUG_MESSAGE(const char CODE_P *fmt, ...);
extern void RS_tx(BYTE tx_buf);
extern void RS2_tx(BYTE tx_buf);

void GraphicsPrint(PRINT_COLOR_e mode, const char *Message, ...);

#ifndef DEBUG

#define dPuts(a) //
#define wPuts(a) //
#define ePuts(a) //

#else

void dPrintf(const char CODE_P *fmt, ...);
void wPrintf(const char CODE_P *fmt, ...);
void ePrintf(const char CODE_P *fmt, ...);

void dPuts(CODE_P char *ptr);
void wPuts(CODE_P char *ptr);
void ePuts(CODE_P char *ptr);

#endif // DEBUG

BYTE Getch(void);
BYTE Getche(void);
#endif

#endif //__PRINTF__
