/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Monitor.c		                                            										*/
/*                                                                           										*/
/*****************************************************************************/
#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"

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
#include "etc_eep.h"

IDATA BYTE MonAddress = 0x8a; // initialize value should be placed to IDATA area
BYTE MonIndex;
BYTE MonRdata, MonWdata;
BYTE monstr[30]; // buffer for input string
				 // volatile  BYTE *argv[7];
BYTE *argv[7];

// char *argv[7];
BYTE argc = 0;
bit echo = 1;

BYTE Monitor_flag = OFF;
BYTE Monitor_I2C = 0x40; /// 0x88;//0x29;//0x40;///0x88;///0x40;//0x28;//0x12;//EEPROM_24C02_addr;

DATA BYTE RS_buf[BUF_MAX];
DATA BYTE RS2_buf[BUF_MAX];
DATA BYTE DVR_buf[DVR_BUF_MAX];

DATA BYTE RS_in = 0, RS_out = 0;
DATA BYTE RS2_in = 0, RS2_out = 0;
bit RS_Xbusy = 0;  // bit RS_Xbusy=0;
bit RS2_Xbusy = 0; // bit RS_Xbusy=0;

static bit indirect = 0;
extern BYTE TriggerFlag;

// extern		  bit	AutoDetect;
extern BYTE InputSelection;
extern BYTE DebugLevel;
// extern  CODE  struct struct_IdName struct_InputSelection[];
extern bit NoInitAccess;
bit AccessEnable = 1;
extern BYTE PSW_Flag;
extern BYTE year1, year2, month, day, hour, minute, second;

extern UART_HandleTypeDef huart1;
extern tByte count[4];
extern tByte state[4];
extern tByte count[4];
extern tByte mode[4];
extern tByte std[4];
extern tByte egain[4][4];
extern BYTE Flisheye_CAM[4];
extern BYTE QuadBoxId;
extern BYTE bytTriggerTest;
extern BYTE PtzTxChNum;
extern BYTE VideoChangeFlag;

tByte bytVideoInput;

// char * asctime(const struct tm * timeptr);
// char *ctime(const time_t *timep);

//=============================================================================
//		Serial RX Check
//=============================================================================
BYTE RS_ready(void)
{
	if (RS_in == RS_out)
		return 0;
	else
		return 1;
}
//#ifdef Hs_debug
//=============================================================================
//		Serial RX
//=============================================================================
BYTE RS_rx(void)
{
	BYTE ret;

	//	ES = 0;
	ret = RS_buf[RS_out];
	//	ret = DVR_buf[RS_out];
	RS_out++;
	if (RS_out >= BUF_MAX)
		RS_out = 0;
	//	ES = 1;

	return ret;
}

//=============================================================================
//		Serial TX
//=============================================================================
void RS_tx(BYTE tx_buf)
{

	// HAL_UART_Transmit(&huart1, &tx_buf, 1, 0xffff);
	USART1->DR = tx_buf;
	while (0 == (USART1->SR & (1 << 6)))
		;
}

// void Loader(BYTE);

//=============================================================================
//
//=============================================================================
void Prompt(void)
{
	if (day)
	{
		Printf("\r\nMCU_I2C[%02x][%d:%d:%d:%d]>", (WORD)Monitor_I2C, (WORD)day, (WORD)hour, (WORD)minute, (WORD)second);
	}
	else if (hour)
	{
		Printf("\r\nMCU_I2C[%02x][%d:%d:%d]>", (WORD)Monitor_I2C, (WORD)hour, (WORD)minute, (WORD)second);
	}
	else if (minute)
	{
		Printf("\r\nMCU_I2C[%02x][%d:%d]>", (WORD)Monitor_I2C, (WORD)minute, (WORD)second);
	}
	else
	{
		Printf("\r\nMCU_I2C[%02x][%d]>", (WORD)Monitor_I2C, (WORD)second);
	}
}

void Mon_tx(BYTE ch)
{
	RS_tx(ch);
}

//=============================================================================
//		Convert ASCII to Binery
//=============================================================================
DWORD a2i(BYTE *str)
{
	DWORD num = 0;
	BYTE i;

	for (i = 0;; i++, str++)
	{
		if (*str == '\0' || *str == ' ')
			break;
		num = num * 10 + *str - '0';
	}
	return num;
}

BYTE Asc1Bin(BYTE asc)
{
	if (asc >= '0' && asc <= '9')
		return (asc - '0');
	if (asc >= 'a' && asc <= 'f')
		return (asc - 'a' + 0x0a);
	if (asc >= 'A' && asc <= 'F')
		return (asc - 'A' + 0x0a);

	return 0;
}

BYTE Asc2Bin(PDATA_P BYTE *s)
{
	WORD bin;

	bin = 0;
	while (*s != '\0' && *s != ' ')
	{
		bin = bin << 4;
		bin = bin + Asc1Bin(*s);
		s++;
	}
	return (bin);
}

//=============================================================================
BYTE hs_toupper(BYTE ch)
{
	if (ch >= 'a' && ch <= 'z')
		return (ch - 'a' + 'A');

	return ch;
}

int stricmp(BYTE *ptr1, BYTE *ptr2)
{
	int i;
	int ret;

	for (i = 0; *ptr1; i++)
	{
		ret = hs_toupper(*ptr1++) - hs_toupper(*ptr2++);
		if (ret)
			return ret;
	}
	return 0;
}

//=============================================================================
//
//=============================================================================
/*
void SetMonAddress(BYTE addr)
{
	MonAddress = addr;
}
*/
void MonReadI2C(void)
{
	/*
		if( argc>=2 ) MonIndex = Asc2Bin( argv[1] );
		else	{
			Printf("   --> Missing parameter !!!");
			return;
		}

		if ( MonAddress == TW88I2CAddress )
			MonRdata = ReadTW88(MonIndex);
		else
			MonRdata = ReadI2C(MonAddress, MonIndex);

		if( echo )
			Printf("\r\nRead %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);

		MonWdata = MonRdata;
		*/
}

void MonWriteI2C(void)
{
	/*
		if( argc<3 ) {
			Printf("   --> Missing parameter !!!");
			return;
		}

		MonIndex = Asc2Bin( argv[1] );
		MonWdata = Asc2Bin( argv[2] );

		if( echo ) {
			Printf("\r\nWrite %2xh:%2xh ", (tWord)MonIndex, (tWord)MonWdata);
			if ( MonAddress == TW88I2CAddress ) {
				WriteTW88(MonIndex, MonWdata);
				MonRdata = ReadTW88(MonIndex);
			}
			else {
				WriteI2C(MonAddress, MonIndex, MonWdata);
				MonRdata = ReadI2C(MonAddress, MonIndex);
			}
			Printf("==> Read %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);
		}
		else {
			if ( MonAddress == TW88I2CAddress ) {
				WriteTW88(MonIndex, MonWdata);
			}
			else {
				WriteI2C(MonAddress, MonIndex, MonWdata);
			}
		}
		*/
}

void MonIncDecI2C(BYTE inc)
{
	BYTE ret;

	switch (inc)
	{
	case 0:
		MonWdata--;
		break;
	case 1:
		MonWdata++;
		break;
	case 10:
		MonWdata -= 0x10;
		break;
	case 11:
		MonWdata += 0x10;
		break;
	}
	/*


	if ( MonAddress == TW88I2CAddress ) {
		WriteTW88(MonIndex, MonWdata);
		MonRdata = ReadTW88(MonIndex);
	}
	else {
		WriteI2C(MonAddress, MonIndex, MonWdata);
		MonRdata = ReadI2C(MonAddress, MonIndex);
	}

	if( echo ) {
		Printf("Write %2xh:%2xh ", (tWord)MonIndex, (tWord)MonWdata);
		Printf("==> Read %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);
	}

*/

	if (Monitor_I2C == 0xA0)
	{
		WriteEEP(MonIndex, MonWdata);
		ret = ReadEEP(MonIndex);
		Printf("\r\nEEPROM Write addr=%02x  data=%02x", (WORD)MonIndex, (WORD)ret);
	}
	else if (Monitor_I2C == 0x40)
	{
		VXISI2CWrite(MonIndex, MonWdata);
		ret = VXISI2CRead(MonIndex);
		Printf("\r\nVS4210 Write addr=%02x  data=%02x", (WORD)MonIndex, (WORD)ret);
	}
	else if (Monitor_I2C == 0x12)
	{
		VS8812Write(MonIndex, MonWdata);
		ret = VS8812Read(MonIndex);
		Printf("\r\nVS8812 Write addr=%02x  data=%02x", (WORD)MonIndex, (WORD)ret);
	}
	else if (Monitor_I2C == 0x88)
	{
		tp28xx_byte_write(MonIndex, MonWdata);
		ret = tp28xx_byte_read(MonIndex);
		Printf("\r\nTP2824  Read addr=%02x  data=%02x", (WORD)MonIndex, (WORD)ret);
	}
	/*
	else if( Monitor_I2C==0x90)
		{
		gHDMI_Index=0;
		HDMIRX_WriteI2C_Byte( MonIndex, MonWdata );
		ret= HDMIRX_ReadI2C_Byte(MonIndex);
		 Printf("\r\nIT66021  Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
		}
	*/
	Prompt();
}

void MonDumpI2C(void)
{
	/*
		BYTE ToMonIndex;
		int  cnt=8;

		if( argc>=2 ) MonIndex = Asc2Bin(argv[1]);
		if( argc>=3 ) ToMonIndex = Asc2Bin(argv[2]);
		else ToMonIndex = MonIndex+cnt;
		if ( ToMonIndex < MonIndex ) ToMonIndex = 0xFF;
		cnt = ToMonIndex - MonIndex + 1;

		if( echo ) {
			if ( MonAddress == TW88I2CAddress ) {
				for ( ; cnt > 0; cnt-- ) {
					MonRdata = ReadTW88(MonIndex);
					Printf("\r\n==> Read %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);
					MonIndex++;
				}
			}
			else {
				for ( ; cnt > 0; cnt-- ) {
					MonRdata = ReadI2C(MonAddress, MonIndex);
					Printf("\r\n==> Read %2xh:%2xh", (tWord)MonIndex, (tWord)MonRdata);
					MonIndex++;
				}
			}
		}
		else {
			if ( MonAddress == TW88I2CAddress ) {
				for ( ; cnt > 0; cnt-- ) {
					MonRdata = ReadTW88(MonIndex);
					MonIndex++;
				}
			}
			else {
				for ( ; cnt > 0; cnt-- ) {
					MonRdata = ReadI2C(MonAddress, MonIndex);
					MonIndex++;
				}
			}
		}
		*/
}

//-----------------------------------------------------------------------------

void MonNewReadI2C(void)
{
	/*
		BYTE Slave;

		if( argc>=3 ) MonIndex = Asc2Bin( argv[2] );
		else	{
			Printf("   --> Missing parameter !!!");
			return;
		}
		Slave = Asc2Bin(argv[1]);

		if ( Slave == TW88I2CAddress )
			MonRdata = ReadTW88(MonIndex);
		else
			MonRdata = ReadI2C(Slave, MonIndex);

		if( echo )
			Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)MonIndex, (tWord)MonRdata);

		MonWdata = MonRdata;
		*/
}

void MonNewWriteI2C(void)
{
	/*
		BYTE Slave;

		if( argc<4 ) {
			Printf("   --> Missing parameter !!!");
			return;
		}

		MonIndex = Asc2Bin( argv[2] );
		MonWdata = Asc2Bin( argv[3] );
		Slave = Asc2Bin(argv[1]);

		if ( Slave == TW88I2CAddress ) {
			WriteTW88(MonIndex, MonWdata);
			MonRdata = ReadTW88(MonIndex);
		}
		else {
			WriteI2C(Slave, MonIndex, MonWdata);
			MonRdata = ReadI2C(Slave, MonIndex);
		}

		if( echo )
			Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)MonIndex, (tWord)MonRdata);
	*/
}

void MonNewDumpI2C(void)
{
	/*
		BYTE 	ToMonIndex, Slave;
		tWord	i;

		if( argc>=2 ) MonIndex = Asc2Bin(argv[2]);
		if( argc>=3 ) ToMonIndex = Asc2Bin(argv[3]);
		Slave = Asc2Bin(argv[1]);

		if( echo ) {
			if ( Slave == TW88I2CAddress ) {
				for(i=MonIndex; i<=ToMonIndex; i++) {
					MonRdata = ReadTW88(i);
					Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)i, (tWord)MonRdata);
				}
			}
			else {
				for(i=MonIndex; i<=ToMonIndex; i++) {
					MonRdata = ReadI2C(Slave, i);
					Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)i, (tWord)MonRdata);
				}
			}
		}
		else {
			if ( Slave == TW88I2CAddress ) {
				for(i=MonIndex; i<=ToMonIndex; i++) {
					MonRdata = ReadTW88(i);
				}
			}
			else {
				for(i=MonIndex; i<=ToMonIndex; i++) {
					MonRdata = ReadI2C(Slave, i);
				}
			}
		}
		*/
}

void MonWriteBit(void)
{
	/*
		BYTE mask, i, FromBit, ToBit,  MonMask, val;
		BYTE Slave;
		// b 88 index startbit|endbit data

		if( argc<5 ) {
			Printf("   --> Missing parameter !!!");
			return;
		}
		Slave = Asc2Bin(argv[1]);

		MonIndex = Asc2Bin( argv[2] );
		FromBit  = (Asc2Bin( argv[3] ) >> 4) & 0x0f;
		ToBit  = Asc2Bin( argv[3] )  & 0x0f;
		MonMask  = Asc2Bin( argv[4] );

		if( FromBit<ToBit || FromBit>7 || ToBit>7) {
			Printf("\r\n   --> Wrong range of bit operation !!!");
			return;
		}

		mask = 0xff;
		val=0x7f;
		for(i=7; i>FromBit; i--) {
			mask &= val;
			val = val>>1;
		}

		val=0xfe;
		for(i=0; i<ToBit; i++) {
			mask &= val;
			val = val<<1;
		}

		if ( Slave == TW88I2CAddress ) {
			MonRdata = ReadTW88(MonIndex);
			MonWdata = (MonRdata & (~mask)) | (MonMask & mask);

			WriteTW88(MonIndex, MonWdata);
			MonRdata = ReadTW88(MonIndex);
		}
		else {
			MonRdata = ReadI2C(Slave, MonIndex);
			MonWdata = (MonRdata & (~mask)) | (MonMask & mask);

			WriteI2C(Slave, MonIndex, MonWdata);
			MonRdata = ReadI2C(Slave, MonIndex);
		}

		if( echo )
			Printf("\r\n<R>%2x[%2x]=%2x", (tWord)Slave, (tWord)MonIndex, (tWord)MonRdata);
	*/
}

#if 0 // defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
void PclkAccess(void)
{
	DtWord pclk;

	if( argc > 2 ) {
		Printf("   --> Too many parameter !!!");
		return;
	}

	if( argc==2 ) {
		pclk = (DtWord)a2i(argv[1]) * 1000000;
		Printf("\r\n --> Set Pclk:  %ld Hz ", pclk);
//		Printf("\r\n Not Suuport ");
		ChangeInternPLL(pclk);
	}
	
	if( argc==1)   GetPPF();
	
}
#endif

void MonTxDump(void)
{
	/*
		tWord	count;

		if ( argc < 2 ) {
			Puts("\r\nInsufficient parameters...");
			return;
		}
		count = (tWord)a2i(argv[1]);
		do {
			RS_tx((count&0x1F)+0x20);
			count--;
		} while ( count != 0 );
		*/
}

//=============================================================================
//			Help Message
//=============================================================================
void MonHelp(void)
{
	Printf("\r\n=======================================================");
	Printf("\r\n>>>     Welcome to ML076HQ     Rev 1.00     <<<");
	Printf("\r\n=======================================================");
	Printf("\r\n   R ii             	; Read TP2824 data");
	Printf("\r\n   W ii dd          	; Write TP2824 data");
	Printf("\r\n   VR ii             	; Read VS4210 data");
	Printf("\r\n   VW ii dd          	; Write VS4210 data");
	Printf("\r\n   D [ii] [cc]      	; Dump");
	Printf("\r\n   C aa             	; Change I2C address");
	Printf("\r\n   M ?              	; Current Input");
	Printf("\r\n   M dd             	; Change Input");
	Printf("\r\n   Access [0,1]     	; Program TW88xx Access OFF/ON");
	Printf("\r\n   AutoDetect [0,1] 	; PC Automode OFF/ON");
	Printf("\r\n   PCAuto		    	; RUN PC Auto Adjust");
	Printf("\r\n   Debug [0..255]   	; Debuging display level");
	Printf("\r\n   Echo On/Off      	; Terminal Echoing On/Off");
	Printf("\r\n   Delta            	; Init DeltaRGB Panel");
	Printf("\r\n   Delta w ii dh dl; SPI Write to DeltaRGB Panel");
	Printf("\r\n");
	Printf("\r\n=======================================================");
	Printf("\r\n");
}

//=============================================================================
//
//=============================================================================
BYTE MonGetCommand(void)
{
	static BYTE comment = 0;
	static BYTE incnt = 0, last_argc = 0;
	BYTE i, ch;
	BYTE ret = 0;

	if (!RS_ready())
		return 0;
	//#ifdef HS_DEBUG
	ch = RS_rx();
	//#endif

#if (_DEBUG_MESSAGE_Monitor == ON)
	Monitor_flag = _TRUE;
#endif
	MCUTimerReactiveTimerEvent(SEC(30), _USER_TIMER_EVENT_Monitor_ON);

	//----- if comment, echo back and ignore -----
	if (comment)
	{
		if (ch == '\r' || ch == 0x1b)
			comment = 0;
		else
		{
			Mon_tx(ch);
			return 0;
		}
	}
	else if (ch == ';')
	{
		comment = 1;
		Mon_tx(ch);
		return 0;
	}

	//=====================================
	switch (ch)
	{

	case 0x1b:
		argc = 0;
		incnt = 0;
		comment = 0;
		Prompt();
		return 0;

	//--- end of string
	case '\r':

		if (incnt == 0)
		{
			Prompt();
			break;
		}

		monstr[incnt++] = '\0';
		argc = 0;

		for (i = 0; i < incnt; i++)
			if (monstr[i] != ' ')
				break;

		if (!monstr[i])
		{
			incnt = 0;
			comment = 0;
			Prompt();
			return 0;
		}
		argv[0] = &monstr[i];
		for (; i < incnt; i++)
		{
			if (monstr[i] == ' ' || monstr[i] == '\0')
			{
				monstr[i] = '\0';
				// Printf("(%s) ",  argv[argc]);
				i++;
				while (monstr[i] == ' ')
					i++;
				argc++;
				if (monstr[i])
				{
					argv[argc] = &monstr[i];
				}
			}
		}

		ret = 1;
		last_argc = argc;
		incnt = 0;

		break;

	//--- repeat command
	case '/':
		argc = last_argc;
		ret = 1;
		break;

	//--- back space
	case 0x08:
		if (incnt)
		{
			incnt--;
			Mon_tx(ch);
			Mon_tx(' ');
			Mon_tx(ch);
		}
		break;

	//--- decreamental write
	case ',':
		if (incnt)
		{
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(0);
		break;

	case '<':
		if (incnt)
		{
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(10);
		break;
	//--- increamental write
	case '.':
		if (incnt)
		{
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(1);
		break;

	case '>':
		if (incnt)
		{
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(11);
		break;

	default:
		Mon_tx(ch);
		monstr[incnt++] = ch;
		break;
	}

	if (ret)
	{
		comment = 0;
		last_argc = argc;
		return ret;
	}
	else
	{
		return ret;
	}
}

//*****************************************************************************
//				Monitoring Command
//*****************************************************************************
//#ifdef  HS_DEBUG
void Monitor(void)
{
#if (HS_DEBUG == ON)
	BYTE ret, temp;
	WORD addr, retw;
#endif

#if (HS_DEBUG == ON)
	BYTE ascii[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	BYTE ReadAddr;
#endif

	if (!MonGetCommand())
		return;

	//--------------------------------------------------
	// Check Indirect command
	/*
	if( !stricmp( argv[0], "I" ) ) 			indirect=1;
	else if( !stricmp( argv[0], "O" ) ) 	indirect=1;

	else if( !stricmp( argv[0], "," ) ) 	indirect=0;
	else if( !stricmp( argv[0], ">" ) ) 	indirect=0;
	else if( !stricmp( argv[0], "<" ) ) 	indirect=0;
	else if( !stricmp( argv[0], "." ) ) 	indirect=0;
	else  indirect=0;
	*/
	indirect = 0;

//---------------- Write Register -------------------
#if (HS_DEBUG == ON)
	if (!stricmp((BYTE *)argv[0], (BYTE *)"W"))
	{

		MonIndex = Asc2Bin(argv[1]);
		MonWdata = Asc2Bin(argv[2]);

#if 0

VXISI2CWrite( Asc2Bin(argv[1]), Asc2Bin(argv[2]) );
		 ret= VXISI2CRead(Asc2Bin(argv[1]));	
	   Printf("\r\nVS4210 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);

#else
		if (Monitor_I2C == 0xA0)
		{
			WriteEEP(Asc2Bin(argv[1]), Asc2Bin(argv[2]));
			// tp28xx_byte_write(Asc2Bin(argv[1]), Asc2Bin(argv[2]));
			ret = ReadEEP(Asc2Bin(argv[1]));
			Printf("\r\nEEPROM Write addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
		}
		else if (Monitor_I2C == 0x40)
		{
			VXISI2CWrite(Asc2Bin(argv[1]), Asc2Bin(argv[2]));
			ret = VXISI2CRead(Asc2Bin(argv[1]));
			Printf("\r\nVS4210 Write addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
		}
		else if (Monitor_I2C == 0x12)
		{
			VS8812Write(Asc2Bin(argv[1]), Asc2Bin(argv[2]));
			ret = VS8812Read(Asc2Bin(argv[1]));
			Printf("\r\nVS8812 Write addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
		}
		else if (Monitor_I2C == 0x28)
		{

			addr = Asc2Bin(argv[1]);
			addr <<= 8;
			addr |= Asc2Bin(argv[2]);

			temp = Asc2Bin(argv[3]);
			SET_nGT911(addr, &temp, 1);

			Printf("\r\nGT911  Write addr=%02x%02x data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)Asc2Bin(argv[2]), (WORD)temp);

			GET_nGT911(addr, &ret, 1);

			Printf("\r\nGT911  Read addr=%02x%02x data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)Asc2Bin(argv[2]), (WORD)ret);
		}
		else if (Monitor_I2C == 0x88)
		{
			tp28xx_byte_write(Asc2Bin(argv[1]), Asc2Bin(argv[2]));
			ret = tp28xx_byte_read(Asc2Bin(argv[1]));
			Printf("\r\nTP2824  Read addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
		}

		else
		{
			Printf("\r\nI2C addr. error");
		}

#endif
		/*
				if(Monitor_I2C==EEPROM_24C02_addr)
				{
				 SW_I2C1Write( Monitor_I2C , Asc2Bin(argv[1]), Asc2Bin(argv[2]) );
				//tp28xx_byte_write(Asc2Bin(argv[1]), Asc2Bin(argv[2]));
				  ret= SW_I2C1Read(Monitor_I2C,Asc2Bin(argv[1]));
				Printf("\r\nEEPROM Write addr=%02x	data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
				}
				else
					{
						tp28xx_byte_write(Asc2Bin(argv[1]), Asc2Bin(argv[2]));
						//ret= tp28xx_byte_read(0x2a);
					  ret= tp28xx_byte_read(Asc2Bin(argv[1]));
					Printf("\r\nTP2824 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
					}
				*/
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)")"))
	{
		indirect = 0;
		//	MonNewWriteI2C();
	}
	/*
	else if( !stricmp( argv[0], "O" ) ) {			// inderect addressing for 8806
		MonIndex = Asc2Bin( argv[1] );
		MonWdata = Asc2Bin( argv[2] );
		WriteI2C(MonAddress, 0xc5, MonIndex);
		WriteI2C(MonAddress, 0xc6, MonWdata);
		MonRdata = ReadI2C(MonAddress, 0xc6);
		Printf("\r\n==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);
	}
	*/
	//---------------- Read Register --------------------
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"R"))
	{
		// MonReadI2C();

		MonIndex = Asc2Bin(argv[1]);

#if 0
			   ret= VXISI2CRead(Asc2Bin(argv[1]));			 
			Printf("\r\nVS4210	Read addr=%02x	data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);

#else
		if (Monitor_I2C == 0xA0)
		{
			ret = ReadEEP(Asc2Bin(argv[1]));
			Printf("\r\nEEPROM  Read addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
		}
		else if (Monitor_I2C == 0x40)
		{
			ret = VXISI2CRead(Asc2Bin(argv[1]));
			Printf("\r\nVS4210  Read addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
		}
		else if (Monitor_I2C == 0x12)
		{
			ret = VS8812Read(Asc2Bin(argv[1]));
			Printf("\r\nVS8812  Read addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
		}
		else if (Monitor_I2C == 0x28)
		{

			addr = Asc2Bin(argv[1]);
			addr <<= 8;
			addr |= Asc2Bin(argv[2]);

			GET_nGT911(addr, &temp, 1);
			Printf("\r\nGT911  Read addr=%02x%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)Asc2Bin(argv[2]), (WORD)temp);
		}
		else if (Monitor_I2C == 0x88)
		{
			ret = tp28xx_byte_read(Asc2Bin(argv[1]));
			Printf("\r\nTP2824  Read addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
		}
		else if (Monitor_I2C == 0x29)
		{
			retw = GET_LTC2451(Asc2Bin(argv[1]));
			Printf("\r\nLTC2451  Read addr=%02x  data=%04x", (WORD)Asc2Bin(argv[1]), (WORD)retw);
		}

		else
		{
			Printf("\r\nI2C addr. error");
		}

		MonWdata = ret;

#endif
		/*
		if(Monitor_I2C==EEPROM_24C02_addr)
				{
			   ret= SW_I2C1Read(Monitor_I2C,Asc2Bin(argv[1]));
			Printf("\r\nEEPROM Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
			}
		else
			{
			   ret= tp28xx_byte_read(Asc2Bin(argv[1]));
			Printf("\r\nTP2824 Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);
			}
		*/
	}

	else if (!stricmp((BYTE *)argv[0], (BYTE *)"VR"))
	{
		// MonReadI2C();
		ret = VXISI2CRead(Asc2Bin(argv[1]));
		Printf("\r\nVS4210 Read addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"VW"))
	{
		VXISI2CWrite(Asc2Bin(argv[1]), Asc2Bin(argv[2]));
		ret = VXISI2CRead(Asc2Bin(argv[1]));
		Printf("\r\nVS4210 Write addr=%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)ret);
	}
	else if (!stricmp(argv[0], (BYTE *)"("))
	{
		indirect = 0;
		// MonNewReadI2C();
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"I2C"))
	{
		ret = Asc2Bin(argv[1]);
		Printf("\r\nChange I2C addr. from %02x to %02x", (WORD)Monitor_I2C, (WORD)ret);
		Monitor_I2C = ret;
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"TR"))
	{
		HAL_GPIO_WritePin(TP2824_RST_GPIO_Port, TP2824_RST_Pin, 0); // TP2824 RST
		msleep(300);
		HAL_GPIO_WritePin(TP2824_RST_GPIO_Port, TP2824_RST_Pin, 1); // TP2824 RST
		msleep(300);
		Init_TP280x_RegSet();
	}
#endif
#if (HS_DEBUG == ON)
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"MENU"))
#else
	if (!stricmp((BYTE *)argv[0], (BYTE *)"MENU"))
#endif
	{
		COMMAND_OSD(CMD_OSD_MENU);
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"FW"))
	{
		COMMAND_OSD(CMD_FW);
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"UP"))
	{
		COMMAND_OSD(CMD_OSD_UP);
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"DOWN"))
	{
		COMMAND_OSD(CMD_OSD_DOWN);
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"SELECT"))
	{
		COMMAND_OSD(CMD_OSD_SELECT);
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"JUMP"))
	{
		COMMAND_OSD(CMD_OSD_JUMP);
	}
#if (_DEBUG_MESSAGE_CONSOLE_TEST == ON)
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"CMDR"))
	{
		// BYTE ascii[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

		Monitor_I2C = Asc2Bin(argv[1]);
		ReadAddr = Asc2Bin(argv[2]);
		if (Monitor_I2C == 0xA0)
			ret = ReadEEP(Asc2Bin(argv[2]));
		else if (Monitor_I2C == 0x40)
			ret = VXISI2CRead(Asc2Bin(argv[2]));
		else if (Monitor_I2C == 0x12)
			ret = VS8812Read(Asc2Bin(argv[2]));
		else if (Monitor_I2C == 0x88)
			ret = tp28xx_byte_read(Asc2Bin(argv[2]));
		else
			Printf("\r\nI2C addr. error");

		Printf("STX");
		RS_tx(ascii[ReadAddr >> 4]);
		RS_tx(ascii[ReadAddr & 0x0F]);
		RS_tx(ascii[ret >> 4]);
		RS_tx(ascii[ret & 0x0F]);
		RS_tx(ascii[0x56 >> 4]);
		RS_tx(ascii[0x56 & 0x0F]);
		RS_tx(ascii[(ReadAddr ^ ret ^ 0x56) >> 4]);
		RS_tx(ascii[(ReadAddr ^ ret ^ 0x56) & 0x0F]);
		Printf("ETX");
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"CMDW"))
	{
		// BYTE ascii[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

		Monitor_I2C = Asc2Bin(argv[1]);
		// VXISI2CWrite(Asc2Bin(argv[2]), Asc2Bin(argv[3]));
		// ret = VXISI2CRead(Asc2Bin(argv[2]));
		if (Monitor_I2C == 0xA0)
		{
			WriteEEP(Asc2Bin(argv[2]), Asc2Bin(argv[3]));
			ret = ReadEEP(Asc2Bin(argv[2]));
		}
		else if (Monitor_I2C == 0x40)
		{
			VXISI2CWrite(Asc2Bin(argv[2]), Asc2Bin(argv[3]));
			ret = VXISI2CRead(Asc2Bin(argv[2]));
		}
		else if (Monitor_I2C == 0x12)
		{
			VS8812Write(Asc2Bin(argv[2]), Asc2Bin(argv[3]));
			ret = VS8812Read(Asc2Bin(argv[2]));
		}
		else if (Monitor_I2C == 0x88)
		{
			tp28xx_byte_write(Asc2Bin(argv[2]), Asc2Bin(argv[3]));
			ret = tp28xx_byte_read(Asc2Bin(argv[2]));
		}
		else
		{
			Printf("\r\nI2C addr. error");
		}

		ReadAddr = Asc2Bin(argv[2]);
		Printf("STX");
		RS_tx(ascii[ReadAddr >> 4]);
		RS_tx(ascii[ReadAddr & 0x0F]);
		RS_tx(ascii[ret >> 4]);
		RS_tx(ascii[ret & 0x0F]);
		RS_tx(ascii[0x56 >> 4]);
		RS_tx(ascii[0x56 & 0x0F]);
		RS_tx(ascii[(ReadAddr ^ ret ^ 0x56) >> 4]);
		RS_tx(ascii[(ReadAddr ^ ret ^ 0x56) & 0x0F]);
		Printf("ETX");
	}
#endif
#if (HS_DEBUG == ON)
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"Q"))
	{
		Monitor_flag = _FALSE;
	}
	else if (!stricmp(argv[0], (BYTE *)"BS"))
	{ // HDMI input
		extern tByte bSignal[4];
		Printf("\r\nbSignal[0] = %d state[0]=%d", (WORD)bSignal[0], (WORD)state[0]);
		Printf("\r\nbSignal[1] = %d state[1]=%d", (WORD)bSignal[1], (WORD)state[1]);
		Printf("\r\nbSignal[2] = %d state[2]=%d", (WORD)bSignal[2], (WORD)state[2]);
		Printf("\r\nbSignal[3] = %d state[3]=%d", (WORD)bSignal[3], (WORD)state[3]);

		Printf("\r\nVideoChangeFlag=%d", (WORD)VideoChangeFlag);

		Monitor_flag = _FALSE;
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"DUMP"))
	{
		WORD s, e;

		s = Asc2Bin(argv[1]);
		e = Asc2Bin(argv[2]);

		if (Monitor_I2C == 0xA0)
			Printf("\r\nDUMP EEPROM");
		else if (Monitor_I2C == 0x40)
			Printf("\r\nDUMP VS4210");
		else if (Monitor_I2C == 0x12)
			Printf("\r\nDUMP VS8812");
		else if (Monitor_I2C == 0x88)
			Printf("\r\nDUMP TP2824");
		else
			Printf("\r\nDUMP REG.");

		for (/*s*/; s <= e; s++)
		{
			if (Monitor_I2C == 0xA0)
			{
				ret = ReadEEP(s);
				Printf("\r\nA0,%02x,%02x", (WORD)s, (WORD)ret);
			}
			else if (Monitor_I2C == 0x40)
			{
				ret = VXISI2CRead(s);
				// Printf("\r\nVS4210  Read addr=%02x  data=%02x",(WORD)s,(WORD)ret);
				Printf("\r\n40,%02x,%02x", (WORD)s, (WORD)ret);
			}
			else if (Monitor_I2C == 0x12)
			{
				ret = VS8812Read(s);
				//			  Printf("\r\nVS8812  Read addr=%02x  data=%02x",(WORD)s,(WORD)ret);
				Printf("\r\n12,%02x,%02x", (WORD)s, (WORD)ret);
			}
			else if (Monitor_I2C == 0x88)
			{
				ret = tp28xx_byte_read(s);
				Printf("\r\n88,%02x,%02x", (WORD)s, (WORD)ret);
			}
			else if (Monitor_I2C == 0x28)
			{
				addr = Asc2Bin(argv[1]);
				addr <<= 8;
				addr |= Asc2Bin(argv[2]);

				GET_nGT911(addr, &temp, 1);
				Printf("\r\nGT911  Read addr=%02x%02x  data=%02x", (WORD)Asc2Bin(argv[1]), (WORD)Asc2Bin(argv[2]), (WORD)temp);
			}
			else
			{
				Printf("\r\nI2C addr. error");
			}
		}
	}
	else if (!stricmp(argv[0], (BYTE *)"ECLEAN"))
	{ // EEPROM Clear to 0xFF

		Printf("\r\nEEPROM CLEAN_START");

		WriteEEP(0, 0xFF);
		WriteEEP(1, 0xFF);
		WriteEEP(2, 0xFF);
		WriteEEP(3, 0xFF);
		WriteEEP(EEP_FWREV_MAJOR, 0xFF);
		WriteEEP(EEP_FWREV_MINOR, 0xFF);

		WriteEEP(EEP_AutoMode, 0xFF); // Auto Day&Night...... OFF
		WriteEEP(EEP_CONTRAST, 0xFF);
		WriteEEP(EEP_BRIGHTNESS, 0xFF);
		WriteEEP(EEP_SATURATION_U, 0xFF);
		WriteEEP(EEP_SHARPNESS, 0xFF);
		WriteEEP(EEP_HUE, 0xFF);
		WriteEEP(EEP_BACKLIGHT, 0xFF);

		WriteEEP(EEP_IMAGEMode, 0xFF);	// IMAGEMode..... SINGLE
		WriteEEP(EEP_RearMode, 0xFF);	// RearMode...... R
		WriteEEP(EEP_TriggerVGA, 0xFF); // trigger vga...... on
		WriteEEP(EEP_GAUGEMode, 0xFF);	// �˨�OSD....... ON
		WriteEEP(EEP_Powerflag, 0xFF);	// POWER FLAG....... ON
		WriteEEP(EEP_COSDMode, 0xFF);	// Display OSD... ON
		WriteEEP(EEP_DVRDisplay, 0xFF); // DVR Display OSD... ON
		WriteEEP(EEP_CAMAMode, 0xFF);	// MirrorA....... OFF
		WriteEEP(EEP_CAMBMode, 0xFF);	// MirrorB....... OFF
		WriteEEP(EEP_CAMCMode, 0xFF);	// MirrorC....... OFF
		WriteEEP(EEP_CAMRMode, 0xFF);	// MirrorR....... ON
		WriteEEP(EEP_CAMDMode, 0xFF);	// MirrorD....... ON

		WriteEEP(EEP_AUDIOPVOL, 0xFF); // AudioVol

		WriteEEP(EEP_GAUGEMode, 0xFF);

		WriteEEP(EEP_ONOFFChoice, 0xFF); // osd lock oFF
		WriteEEP(EEP_MenuTouch, 0xFF);	 // Menu Touch select....... OFF

		WriteEEP(EEP_Auto_Sleeping, 0xFF); // Auto sleeping=OFF
		WriteEEP(EEP_Triple_Screen_Type, 0xFF);

		WriteEEP(EEP_Distance_Gauge_Position, 0xFF);
		WriteEEP(EEP_Distance_Gauge_Width, 0xFF);

		WriteEEP(EEP_Grabber_Liner, 0xFF); // default OFF
		WriteEEP(EEP_Grabber_Liner_Position, 0xFF);
		WriteEEP(EEP_Grabber_Liner_Width, 0xFF);

		//  ChangeVol(0);
		WriteEEP(EEP_PCAUDIOPVOL, 0xFF);
		WriteEEP(EEP_AudioRecord, 0xFF); // dvr audio channel

		WriteEEP(EEP_IMAGE_A_Mode, 0xFF);
		WriteEEP(EEP_IMAGE_B_Mode, 0xFF);
		WriteEEP(EEP_IMAGE_C_Mode, 0xFF);

		WriteEEP(EEP_DELAYCAMA, 0xFF); // DELAY CAM A....... 2
		WriteEEP(EEP_DELAYCAMB, 0xFF); // DELAY CAM B....... 2
		WriteEEP(EEP_DELAYCAMC, 0xFF); // DELAY CAM C....... 2
		WriteEEP(EEP_DELAYCAMR, 0xFF); // DELAY CAM R....... 2
		WriteEEP(EEP_DELAYCAMD, 0xFF); // DELAY CAM D....... 2

		WriteEEP(EEP_JUMPMode, 0xFF);	 // JUMPMode...... QUAL
		WriteEEP(EEP_ONOFFChoice, 0xFF); // OSD	lock....... off
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

		WriteEEP(EEP_TP2824_STD0 + (0 * 3) + 2, 0xFF);
		WriteEEP(EEP_TP2824_STD0 + (1 * 3) + 2, 0xFF);
		WriteEEP(EEP_TP2824_STD0 + (2 * 3) + 2, 0xFF);
		WriteEEP(EEP_TP2824_STD0 + (3 * 3) + 2, 0xFF);
		WriteEEP(EEP_TP2824_STD0 + (4 * 3) + 2, 0xFF);

		WriteEEP(EEP_FlishCAM1, 0xFF);
		WriteEEP(EEP_FlishCAM2, 0xFF);
		WriteEEP(EEP_FlishCAM3, 0xFF);
		WriteEEP(EEP_FlishCAM4, 0xFF);
		WriteEEP(EEP_TimeStep, 0xFF);	// TimerStep.....02
		WriteEEP(EEP_CAMOutMode, 0xFF); // CAMOut........ QUAL

		WriteEEP(EEP_NowModeState, 0xFF);

		WriteEEP(EEP_SelectMode, 0xFF);

		Printf("\r\nEEPROM CLEAN_END");
	}
#if 0
	else if( !stricmp( (BYTE *)argv[0],(BYTE *) "INC" ))
		{
		BYTE a,d,b0,b1,b2,b3;
		b0= tp28xx_byte_read(0x30);
		b1= tp28xx_byte_read(0x31);
		b2= tp28xx_byte_read(0x32);
		b3= tp28xx_byte_read(0x33);	
	
		// a=Asc2Bin(argv[1]); 
		// d=Asc2Bin(argv[2]); 
		 
	 for(b1;b0<240;b1+=4)
	 {
		if(b1>240)
			{
			b1=0;
			b0++;
			}
		
		
//	        tp28xx_byte_write(0x33,b3);	 
//	 	 tp28xx_byte_write(0x32,b2);	 
	 	 tp28xx_byte_write(0x31,b1);	 
	 	tp28xx_byte_write(0x30,b0);	 
	 
  Printf("\r\nTP2824  Read addr=0x30313233  data=%02x%02x%02x%02x" ,(WORD)tp28xx_byte_read(0x30),(WORD)tp28xx_byte_read(0x31)\
  	,(WORD)tp28xx_byte_read(0x32),(WORD)tp28xx_byte_read(0x33));

		msleep(1000);
	 }

		}
#endif
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"DEC"))
	{
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"DTP"))
	{
		WORD n, i;

		addr = Asc2Bin(argv[1]);
		addr <<= 8;
		addr |= Asc2Bin(argv[2]);

		n = Asc2Bin(argv[3]);

		Printf("\r\nDUMP GT911 Reg..");

		for (i = 0; i < n; i++)
		{
			GET_nGT911(addr + i, &temp, 1);
			Printf("\r\nAddr=%04x Data=%02x", (WORD)addr + i, (WORD)temp);
		}
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"SETBOX"))
	{
		SetCOCCom(25, Asc2Bin(argv[1]));
		SetCOCCom(21, 0);
	}

	else if (!stricmp((BYTE *)argv[0], (BYTE *)"DF"))
	{
		BYTE i;
		for (i = 0; i < 4; i++)
		{
			Printf("\r\nFlisheye_CAM[%02x]=%02x", (WORD)i, (WORD)Flisheye_CAM[i]);
		}
		Printf("\r\nQuadBoxId=%02x", (WORD)QuadBoxId);
	}
	/*
	else if( !stricmp( argv[0], "i" ) ) {
		MonIndex = Asc2Bin( argv[1] );
		WriteI2C(MonAddress, 0xc5, MonIndex);
		MonRdata = ReadI2C(MonAddress, 0xc6);
		Printf("\r\n==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);

	}
	*/
	/*
	//---------------- Dump Register --------------------
	else if( !stricmp( argv[0], "D" ) ) {
		Puts("\r\ndump start");
		MonDumpI2C();
	}
	else if( !stricmp( argv[0], "&" ) ) {
		indirect=0;
		MonNewDumpI2C();
	}

	//---------------- Bit Operation --------------------
	else if( !stricmp( argv[0], "B" ) ) {// Write bits - B AA II bb DD
		MonWriteBit();
	}
	//---------------- AD5110 CMD --------------------
	else if( !stricmp( argv[0], "PSW" ) ) {
		PSW_Flag=1;
		Printf("\r\n(PSW_Flag =1)");
	}
	//---------------- AD5110 CMD --------------------
	else if( !stricmp( argv[0], "ADW" ) ) {
		WriteAD5110(Asc2Bin(argv[1]),Asc2Bin(argv[2]));
		Printf("\r\nAD5110 Write addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)Asc2Bin(argv[2]));
	}
	else if( !stricmp( argv[0], "ADR" ) ) {
		ReadAD5110(Asc2Bin(argv[1]));
		Printf("\r\nAD5110 Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ReadAD5110(Asc2Bin(argv[1])));
	}
*/
	/*	no indirect addressing in TW8816
	//---------------------------------------------------
	else if( !stricmp( argv[0], "@" ) ) { // Indirect address bit access

					BYTE Slave, mask, FromBit, ToBit,  MonMask, val,i;
					// @ 8a iaddr idata index  startbit|endbit  data
					// 0  1   2    3     4     5                 6

					if( argc<7 ) {
						Printf("   --> Missing parameter !!!");
						return;
					}

					Slave = Asc2Bin(argv[1]);

					FromBit = Asc2Bin( argv[5] );
					FromBit  = ( FromBit >> 4) & 0x0f;
					ToBit = Asc2Bin( argv[5] );
					ToBit  =  ToBit & 0x0f;
					MonMask  = Asc2Bin( argv[6] );

					if( FromBit<ToBit || FromBit>7 || ToBit>7) {
						Printf("\r\n   --> Wrong range of bit operation !!!");
						return;
					}

					mask = 0xff;
					val=0x7f;
					for(i=7; i>FromBit; i--) {
						mask &= val;
						val = val>>1;
					}

					val=0xfe;
					for(i=0; i<ToBit; i++) {
						mask &= val;
						val = val<<1;
					}

					// @ 8a iaddr idata index  startbit|endbit  data
					// 0  1   2    3     4     5                 6
					MonIndex = Asc2Bin(argv[2]);
					MonWdata = Asc2Bin(argv[4]);
					WriteI2C( Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, Asc2Bin( argv[3]));
					MonWdata = (MonRdata & (~mask)) | (MonMask & mask);
					MonIndex = Asc2Bin(argv[3]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, MonIndex);

					Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);

	}
	//------------------------------------------------------------------
	else if( !stricmp( argv[0], "!" ) ) //  Indirect address write access
	{
					BYTE Slave;
					// @ 8a iaddr idata index data
					if( argc<6 ) {
						Printf("   --> Missing parameter !!!");
						return;
					}

					Slave = Asc2Bin(argv[1]);

					MonIndex = Asc2Bin(argv[2]);
					MonWdata = Asc2Bin(argv[4]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonIndex = Asc2Bin(argv[3]);
					MonWdata = Asc2Bin(argv[5]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, MonIndex);

					Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);

	}
	*/

	//---------------- Change I2C -----------------------
	/*
	else if( !stricmp( (BYTE *)argv[0],(BYTE *) "C" ) ) {
		//Monitor_I2C = Asc2Bin( argv[1] );
		SetNowMode();
	}
	*/
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"PON"))
	{
		// Monitor_I2C = Asc2Bin( argv[1] );
		SetNowMode();
		LCDPowerON(1);
	}

	//---------------- Help -----------------------------
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"H") || !stricmp((BYTE *)argv[0], (BYTE *)"HELP") || !stricmp((BYTE *)argv[0], (BYTE *)"?"))
	{
		MonHelp();
	}
	else if (!stricmp(argv[0], (BYTE *)"PWM"))
	{

		user_pwm_setvalue(Asc2Bin(argv[1]) * 15);

		Printf("\r\nPWM=%d", (WORD)(Asc2Bin(argv[1]) * 15));
	}
	else if (!stricmp(argv[0], (BYTE *)"ALCR"))
	{

		Printf("\r\nALCR=%04x", (WORD)GET_ALC122(0x02));
	}
	else if (!stricmp(argv[0], (BYTE *)"ALCW"))
	{
		WORD val = 0;
		val = Asc2Bin(argv[1]);
		val <<= 8;
		val |= Asc2Bin(argv[1]);

		SET_ALC122(0x02, val);

		Printf("\r\nALCR=%04x", (WORD)GET_ALC122(0x02));
	}
	else if (!stricmp(argv[0], (BYTE *)"tri"))
	{

		TriggerFlag = Asc2Bin(argv[1]);

		Printf("\r\nTriggerFlag=%02x", (WORD)TriggerFlag);
	}
	else if (!stricmp(argv[0], (BYTE *)"triset"))
	{

		if (Asc2Bin(argv[1]) == 1)
			bytTriggerTest = 1;
		else
			bytTriggerTest = 0;

		Printf("\r\nbytTriggerTest=%02x", (WORD)bytTriggerTest);
	}
	else if (!stricmp(argv[0], (BYTE *)"sw"))
	{
		BYTE ch, win;
		ch = Asc2Bin(argv[1]);
		win = Asc2Bin((BYTE *)argv[2]);
		VS4210_SwitchChannel(ch, win);

		Printf("\r\n(VS4210_SwitchChannel ch=%02x win=%02x)", (WORD)ch, (WORD)win);
	}
	else if (!stricmp(argv[0], (BYTE *)"sel"))
	{
		BYTE ch;
		ch = Asc2Bin(argv[1]);

		if (ch == 3)
		{
			CAM_R_SW(ON);
			CAM_D_SW(OFF);
			Printf("\r\n(Select CAM R)");
		}
		else if (ch == 4)
		{
			CAM_R_SW(OFF);
			CAM_D_SW(ON);
			Printf("\r\n(Select CAM D)");
		}
		else
		{
			CAM_R_SW(ON);
			CAM_D_SW(OFF);
			Printf("\r\n(CAM D and CAM R for OFF)");
		}
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"dpreg"))
	{
		BYTE i;
		for (i = 0; i < 4; i++)
		{ /*
			 count[i] = 0;
			 state[i] = VIDEO_UNPLUG;
			 std[i] = TVI;
			 mode[i] = NoDet;
			 egain[4][4];
			 */
			Printf("\r\n(ch=%02x count=%02x state=%02x std=%02x mode=%02x)", (WORD)i, (WORD)count[i], (WORD)state[i], (WORD)std[i], (WORD)mode[i]);
			Printf("(egain[%02x]  [0]=%02x [1]=%02x [2]=%02x [3]=%02x)", (WORD)i, (WORD)egain[i][0], (WORD)egain[i][1], (WORD)egain[i][2], (WORD)egain[i][3]);
		}
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"ACCESS"))
	{
		//	if( argc==2 ) {
		AccessEnable = Asc2Bin(argv[1]);
		//	}
		Printf("\r\n2824 AUTO ACCESS = %d", (WORD)AccessEnable);
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"ptz"))
	{

		PtzTxChNum = Asc2Bin(argv[1]);
		Printf("\r\n(bytVideoInput=%d)(PtzTxChNum=%d)", (WORD)bytVideoInput, (WORD)PtzTxChNum);
	}
	else if (!stricmp((BYTE *)argv[0], (BYTE *)"getvideo"))
	{

		Printf("\r\n(bytVideoInput=%d)(PtzTxChNum=%d)", (WORD)bytVideoInput, (WORD)PtzTxChNum);
	}
#endif
	/*
//---------------------------------------------------
else if( !stricmp( argv[0], "*" ) ) {

			if( argc==1 ) {
				Printf("\r\n  * 0 : Program default Loader");
				Printf("\r\n  * 1 : Program external Loader");
				Printf("\r\n  * 2 : Execute Loader");
			}
			else {
				BYTE mode;
				mode = Asc2Bin(argv[1]);
				//Loader(mode);
			}
}

//---------------------------------------------------
#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
else if( !stricmp( argv[0], "PCLK" ) ) {
	PclkAccess();
}
#endif

//---------------------------------------------------
else if( !stricmp( argv[0], "POWER" ) ) {
	//LCDPower();
}

//---------------------------------------------------
#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
else if( !stricmp( argv[0], "M" ) ) {
	if( argc==2 ) {
		if( !stricmp( argv[1], "?" ) ) {
			BYTE i;
			Printf("\r\nCurrent Input:%d \r\n", (WORD)InputSelection);
			for(i=1; struct_InputSelection[i].Id!=0 ; i++)
					Printf("%s:%d  ",struct_InputSelection[i].Name,(WORD)struct_InputSelection[i].Id );
			Printf("\r\n");
		}
		else
			ChangeInput(Asc2Bin( argv[1] ));
	}
	else
		ChangeInput(GetNextInputSelection());
}
//---------------------------------------------------
else if( !stricmp( argv[0], "ACCESS" ) ) {
	if( argc==2 ) {
		AccessEnable = Asc2Bin(argv[1]);
	}
	Printf("\r\nTW88xx AUTO ACCESS = %d", (WORD)AccessEnable);
}
#if 0
//---------------------------------------------------
else if( !stricmp( argv[0], "AUTODETECT" ) ) {
	if( argc==2 ) {
		AutoDetect = Asc2Bin(argv[1]);
	}
	Printf("\r\nPC Auto Detect = %d", (WORD)AutoDetect);
}
#endif
//---------------------------------------------------
else if( !stricmp( argv[0], "PCAUTO" ) ) {
	Puts("\r\nPC Auto Measurement");
	AutoAdjust();
}
//---------------------------------------------------
else if( !stricmp( argv[0], "PCCOLOR" ) ) {
	Puts("\r\nPC Color Auto Measurement");
	AutoColorAdjust();
}
//---------------------------------------------------
else if( !stricmp( argv[0], "7" ) ) {
	Puts("\r\nGet HPN / VPN");
	GetHpnVpn(DebugLevel);
}
#endif
//---------------- Debug Level ---------------------
else if ( !stricmp( argv[0], "DEBUG" ) ) {
	if( argc==2 ) {
		DebugLevel = Asc2Bin(argv[1]);
		SaveDebugLevelEE(DebugLevel);
	}
	Printf("\r\nDebug Level = %2x", (WORD)DebugLevel);
}

//---------------- Display on/off -------------------
else if ( !stricmp( argv[0], "echo" ) ) {
	if( !stricmp( argv[1], "off" ) ) {
		echo = 0;
		Printf("\r\necho off");
	}
	else {
		echo = 1;
		Printf("\r\necho on");
	}
}
//---------------- RESET/ TEST / POWER_DOWN -------------------
else if ( !stricmp( argv[0], "reset" ) ) {
	if( !stricmp( argv[1], "0" ) ) {
//			P3_4 = 0;
		Printf("\r\nTW_reset_pin => 0");
	}
	else if( !stricmp( argv[1], "1" ) ) {
//		P3_4 = 1;
		Printf("\r\nTW_reset_pin => 1");
	}
	else {
		//Printf("\r\nTW_reset_pin = %d", (WORD)P3_4);
	}
}
else if ( !stricmp( argv[0], "test" ) ) {
	if( !stricmp( argv[1], "0" ) ) {
		//P0_7 = 0;
		Printf("\r\nTW_test_pin => 0");
	}
	else if( !stricmp( argv[1], "1" ) ) {
		//P0_7 = 1;
		Printf("\r\nTW_test_pin => 1");
	}
	else {
	//	Printf("\r\nTW_test_pin = %d", (WORD)P0_7);
	}
}
else if ( !stricmp( argv[0], "pdn" ) ) {
	if( !stricmp( argv[1], "0" ) ) {
	//	P3_7 = 0;
		Printf("\r\nTW_PowerDown_pin => 0");
	}
	else if( !stricmp( argv[1], "1" ) ) {
		//P3_7 = 1;
		Printf("\r\nTW_PowerDown_pin => 1");
	}
	else {
		//Printf("\r\nTW_PowerDown_pin = %d", (WORD)P3_7);
	}
}
//---------------- OSD test for parallel -----------------------------------
else if( !stricmp( argv[0], "OSD" ) ) {
	if( !stricmp( argv[1], "logo" ) ) {
//			DisplayLogo();
	}
	else if( !stricmp( argv[1], "end" ) ) {
		Puts("\r\nIf you'd like to exit, press any Key....");
		while ( !RS_ready() ){
			P2 = ReadTW88( 0x95 );
		}
	}
	else if( !stricmp( argv[1], "input" ) ) {
//			DisplayInput();
	}
	else if( !stricmp( argv[1], "off" ) ) {
////			ShowOSDWindowAll(OFF);
	}
	//else if( !stricmp( argv[1], "grid" ) ) {
	//	DisplayGrid();
	//}
#if 0
#ifndef BANKING
	else if( !stricmp( argv[1], "cone" ) ) {
		DisplayCone();
		DisplayJapanese();
	}
#endif
#endif
	#ifdef BANKING
	else if( !stricmp( argv[1], "agrid" ) ) {
		BYTE i, j, k;
		Puts("\r\nIf you'd like to exit, press any Key....");
		j = 0;
		k = 0;
		DisplayMessage();
		if ( argc == 3 )
			i = Asc2Bin(argv[2]);
		else i = 0;
		while ( !RS_ready() ){
			if ( j < 9 )
				DisplayGridBank0(j);
			else
				DisplayGridBank1(j);
			if ( i )
				delay(10*i);
			if (k==0) {
				if (j==16) {
					k = 1;
					j--;
				}
				else j++;
			}
			else {
				if ( j==0 ) {
					k = 0;
					j++;
				}
				else j--;
			}
		}
	}
	else if( !stricmp( argv[1], "cgrid" ) ) {
		BYTE i, j, k;
		Puts("\r\nIf you'd like to exit, press any Key....");
		j = 0;
		k = 0;
		DisplayChinese();
		if ( argc == 3 )
			i = Asc2Bin(argv[2]);
		else i = 0;
		while ( !RS_ready() ){
			if ( j < 9 )
				DisplayGridBank0(j);
			else
				DisplayGridBank1(j);
			if ( i )
				delay(10*i);
			if (k==0) {
				if (j==16) {
					k = 1;
					j--;
				}
				else j++;
			}
			else {
				if ( j==0 ) {
					k = 0;
					j++;
				}
				else j--;
			}
		}
	}
	else if( !stricmp( argv[1], "jgrid" ) ) {
		BYTE i, j, k;
		Puts("\r\nIf you'd like to exit, press any Key....");
		j = 0;
		k = 0;
		DisplayJapanese();
		if ( argc == 3 )
			i = Asc2Bin(argv[2]);
		else i = 0;
		while ( !RS_ready() ){
			if ( j < 9 )
				DisplayGridBank0(j);
			else
				DisplayGridBank1(j);
			if ( i )
				delay(10*i);
			if (k==0) {
				if (j==16) {
					k = 1;
					j--;
				}
				else j++;
			}
			else {
				if ( j==0 ) {
					k = 0;
					j++;
				}
				else j--;
			}
		}
	}
	else if( !stricmp( argv[1], "kgrid" ) ) {
		BYTE i, j, k;
		Puts("\r\nIf you'd like to exit, press any Key....");
		j = 0;
		k = 0;
		DisplayKorean();
		if ( argc == 3 )
			i = Asc2Bin(argv[2]);
		else i = 0;
		while ( !RS_ready() ){
			if ( j < 9 )
				DisplayGridBank0(j);
			else
				DisplayGridBank1(j);
			if ( i )
				delay(10*i);
			if (k==0) {
				if (j==16) {
					k = 1;
					j--;
				}
				else j++;
			}
			else {
				if ( j==0 ) {
					k = 0;
					j++;
				}
				else j--;
			}
		}
	}
	#endif
}
//---------------- TELI ML070I Test function -----------------------------------
else if( !stricmp( argv[0], "PWM0" ) ) {
	SET_PWM(_BL_PWM, Asc2Bin(argv[1]));
	Puts("\r\nSET PWMP1.4");
}
else if( !stricmp( argv[0], "PWM1" ) ) {
	SET_PWM(_CHG_CURR, Asc2Bin(argv[1]));
	Puts("\r\nSET PWMP1.5");
}
else if( !stricmp( argv[0], "PWM" ) ) {
	if(Asc2Bin(argv[1]))
		{
		SET_PWM(_RUN_PWM, _RUN_PWM);
		Puts("\r\nPOEN PWM");
		}
	else
		{
		SET_PWM(_STOP_PWM, _STOP_PWM);
		Puts("\r\nSTOP PWM");
		}
}
//---------------- serial tx test with some number -----------------------------------
else if( !stricmp( argv[0], "TX" ) ) {
	MonTxDump();
}
//---------------- OSD FONTtest for parallel -----------------------------------
else if( !stricmp( argv[0], "FONT" ) ) {
	if ( !stricmp( argv[1], "RAM" ) ) {
//			DisplayRAMFont( Asc2Bin(argv[2]) );
	}
	else if ( !stricmp( argv[1], "ROM" ) ) {
	//	DisplayROMFont(Asc2Bin(argv[2]));
	}
	else {
		Puts("\r\n Font ROM # or Font RAM # for testing");
	}
}
*/
//---------------- Delta RGB Panel Test -------------------------
#if 0 // def SUPPORT_DELTA_RGB

	else if( !stricmp( argv[0], "delta" ) ) {
		if( argc==1 )	DeltaRGBPanelInit();
		else {
			
			if( !stricmp( argv[1], "w" ) ) {

				WORD val;
				val= Asc2Bin(argv[3])<<8 | Asc2Bin(argv[4]);
				Printf("\r\n SPI Write: Addr:%2x  Data:%4x", (WORD)Asc2Bin(argv[2]), val);
				WriteSPI(Asc2Bin(argv[2]), val );
			}
			else if( !stricmp( argv[1], "r" ) ) {
				WORD val;
				val = ReadSPI(Asc2Bin(argv[2]));
				Printf("\r\n SPI Read: Addr:%2x  Data:%4x", (WORD)Asc2Bin(argv[2]), val);
			}
		}
	}
#endif
	//----------------------------------------------------
	else
	{
		Printf("\r\nInvalid command...");
	}

#if (HS_DEBUG == ON)
	Prompt();
#endif
}
//#endif

//#endif

//-------------------------------------------------------------------------

//#endif
