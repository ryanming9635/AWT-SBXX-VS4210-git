/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Printf.c		                                            										*/
/*                                                                           										*/
/*****************************************************************************/

#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"

//#include "Config.h"

//#ifdef SERIAL

//#include "reg.h"
//#include "Typedefs.h"
//#include "debug.h"
//#include "main.h"
#include "printf.h"
#include "monitor.h"

#include <stdarg.h>

#if (_DEBUG_MESSAGE_COLOR_MESSAGE == ON)

static const char *_PrintColor[] = {
	"\033[0m",
	"\033[0;31m",
	"\033[0;32m",
	"\033[0;33m",
	"\033[0;34m",
	"\033[0;35m",
	"\033[0;36m",
	"\033[0;37m",
};
#endif
CODE tByte Hex[] = "0123456789ABCDEF";
extern bit RS_Xbusy; // bit RS_Xbusy=0;
extern tByte DebugLevel;
extern BYTE Monitor_flag;

void DoPrint(const char CODE_P *fmt, va_list ap)
{
	char ch;
	char i;
	long value = _FALSE;
	bit fl_zero;
	//	bit   fl_num;
	BYTE fl_len;
	BYTE cnt;
	DWORD mask = 1;

#ifdef KEILC
	char *ptr;
#endif

	while (1)
	{

		//----- Find Formatter % -----

		switch (ch = *fmt++)
		{
		case 0:
			return;
		case '%':
			if (*fmt != '%')
				break;
			fmt++;
		default:
			_outchar(ch);
			continue;
		}

		//----- Get Count -------------

		fl_zero = 0;
		// fl_num = 0;
		cnt = 0;

		ch = *fmt++;

		if (ch == '0')
		{
			fl_zero = 1;
			ch = *fmt++;
			cnt = ch - '0';
			ch = *fmt++;
		}
		else if (ch >= '0' && ch <= '9')
		{
			cnt = ch - '0';
			ch = *fmt++;
		}

		//----- Get char(B) / int / long(L) ----------------

		fl_len = 2;

		switch (ch)
		{
		case 'l':
		case 'L':
			ch = *fmt++;
			fl_len = 4;
			break;
		case 'b':
		case 'B':
			ch = *fmt++;
			fl_len = 1;
			break;
		}

		//----- Get Type Discriptor -----

		switch (ch)
		{

		case 'd':
		case 'u':

			switch (fl_len)
			{
			case 1:
#if 1 // ryan@20200511
				if (ch == 'd')
					value = (int)va_arg(ap, int);
				else
					value = (int)va_arg(ap, int);

#else
				if (ch == 'd')
					value = (char)va_arg(ap, char);
				else
					value = (BYTE)va_arg(ap, BYTE);
#endif
				break;

			case 2:
				if (ch == 'd')
					value = (int)va_arg(ap, int);
				else
					value = (WORD)va_arg(ap, WORD);
				break;

			case 4:
				if (ch == 'd')
					value = (long)va_arg(ap, long);
				else
					value = (DWORD)va_arg(ap, DWORD);
				break;
			}

			if (value < 0)
			{
				_outchar('-');
				value = value * (-1);
			}

			if (cnt == 0)
			{
				if (value == 0)
				{
					_outchar('0');
					continue;
				}

				for (cnt = 0, mask = 1; cnt < 10; cnt++)
				{
					if ((value / mask) == 0)
						break;
					mask = mask * 10;
				}
			}

			for (i = 0, mask = 1; i < cnt - 1; i++)
				mask = mask * 10;

			while (1)
			{
				ch = (value / mask) + '0';
				if (ch == '0' && fl_zero == 0 && mask != 1)
					ch = ' ';
				else
					fl_zero = 1;
				_outchar(ch);

				value = value % (mask);
				mask = mask / 10;

				if (mask == 0)
					break;
			}
			continue;

		case 'x':
		case 'X':

			switch (fl_len)
			{
#if 1 // ryan@20200511
			case 1:
				value = (int)va_arg(ap, int);
				break;
#else
			case 1:
				value = (BYTE)va_arg(ap, BYTE);
				break;
#endif
			case 2:
				value = (WORD)va_arg(ap, WORD);
				break;
			case 4:
				value = (DWORD)va_arg(ap, DWORD);
				break;
			}

			if (cnt == 0)
				cnt = fl_len * 2;

			for (i = 0; i < cnt; i++)
			{
				_outchar(Hex[(value >> (cnt - i - 1) * 4) & 0x000f]);
			}
			continue;

		case 's':

#ifdef TASKINGC

			value = (WORD)va_arg(ap, WORD);
			while (*(char CODE_P *)value != '\0')
				_outchar(*(char CODE_P *)value++);
			continue;

#elif defined KEILC

			ptr = (char *)va_arg(ap, char *);
			while (*ptr != '\0')
				_outchar(*ptr++);
			continue;

#endif

		case 'c':
			value = va_arg(ap, int);
			_outchar((BYTE)value);
			continue;

		default:
			value = (WORD)va_arg(ap, int);
			continue;
		}
	}
}

//===========================================================================//
//                                                                           //
//===========================================================================//
// va_list ap;

void Printf(const char CODE_P *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	DoPrint(fmt, ap);
	va_end(ap);
}

void DEBUG_MESSAGE(const char CODE_P *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
#if (HS_DEBUG == ON)
	DoPrint(fmt, ap);
#endif
	va_end(ap);
}

void Puts(CODE_P char *ptr)
{
	while (*ptr != '\0')
		RS_tx(*ptr++);
}

//===========================================================================//
//                                                                           //
//===========================================================================//
#ifdef DEBUG

void dPrintf(const char CODE_P *fmt, ...)
{
	va_list ap;

	if (DebugLevel >= DEBUG_INFO)
	{
		va_start(ap, fmt);
		DoPrint(fmt, ap);
		va_end(ap);
	}
}
/*
void wPrintf( const char CODE_P *fmt, ... )
{
	va_list ap;

	if( DebugLevel >= DEBUG_WARN ) {
		va_start(ap, fmt);
		DoPrint( fmt, ap );
		va_end( ap );
	}
}
*/

/*

void ePrintf( const char CODE_P *fmt, ... )
{
	va_list ap;

	if( DebugLevel >= DEBUG_ERR ) {
		va_start(ap, fmt);
		DoPrint( fmt, ap );
		va_end( ap );
	}
}
*/
//===========================================================================//
//                                                                           //
//===========================================================================//
void dPuts(CODE_P char *ptr)
{
	if (DebugLevel >= DEBUG_INFO)
	{
		while (*ptr != '\0')
			RS_tx(*ptr++);
	}
}

#if defined(SUPPORT_PC) || defined(SUPPORT_DTV) || defined(DEBUG_SETPANEL)
void wPuts(CODE_P char *ptr)
{
	if (DebugLevel >= DEBUG_WARN)
	{
		while (*ptr != '\0')
			RS_tx(*ptr++);
	}
}

void ePuts(CODE_P char *ptr)
{
	if (DebugLevel >= DEBUG_ERR)
	{
		while (*ptr != '\0')
			RS_tx(*ptr++);
	}
}
#endif

#endif // DEBUG

//===========================================================================//
//                                                                          GraphicsPrint
//===========================================================================//

void GraphicsPrint(PRINT_COLOR_e mode, const char *Message, ...)
{
#if (_DEBUG_MESSAGE_COLOR_MESSAGE == ON)

	va_list ap;

#if (_DEBUG_MESSAGE_Monitor == ON)
	if (Monitor_flag == _TRUE)
		;
	else
#endif
	{

		DEBUG_MESSAGE(_PrintColor[mode]);
		va_start(ap, Message);
		DoPrint(Message, ap);
		va_end(ap);
		DEBUG_MESSAGE(_PrintColor[PRINT_COLOR_BLACK]);
	}
#else
	va_list ap;
	va_start(ap, Message);
	DoPrint(Message, ap);
	va_end(ap);
	mode = 1;

#endif
}

//===========================================================================//
//                                                                           //
//===========================================================================//
/*
#ifdef DEBUG
BYTE Getch(void)
{
	while(!RS_ready());
	return RS_rx();
}
#endif
*/
#if 0
BYTE Getche(void)
{
	BYTE ch;

	while(!RS_ready());
	ch = RS_rx();
	RS_tx(ch);

	return ch;
}
#endif

//#endif	// SERIAL
