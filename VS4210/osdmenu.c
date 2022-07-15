// osdmenu.c
#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"

#define DEBUG_OSD 1

/*
#include "osdmenu.h"

#include "Config.h"
#include "typedefs.h"
#include "Reg.h"
#include "main.h"
#include "i2c.h"
#include "Tw88.h"
#include "measure.h"
#include "dispinfo.h"
#include "osdbasic.h"
#include "osdmenu.h"
#include "adc.h"
#include "debug.h"
#include "etc_eep.h"
#include "pc_eep.h"
#include "audio.h"
#include "printf.h"
#include "udfont.h"
#include "panel.h"
#include "rgbmix.h"		// for TW8801
#include "cpu.h"

#include "HS_keyremo.h"
#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
#include "HS_DVRProtocol.h"
#endif
#include "HS_DisplayOSD.h"
#include "HS_IOdef.h"
*/

#ifdef HTRV  //Pinchi 20150825 add model #define
extern BYTE GaugeVStart;
extern BYTE GaugeSetup;
extern BYTE GaugeInfoOSD;
#endif

static  WORD	OSDItemValue;
//static  BYTE	OSDDuration;

static  BYTE	title_item, cursor_item, /*cursor_y,*/  cur_osdwin, /*cur_osdaddr,*/ val_osdwin=0xff, 
						val_index=0;

static	BYTE	OSDMenuX, OSDMenuY;/*ljy100303..., OSDZoom*/
static  BYTE	OSDLang=0;

BYTE    OSDMenuLevel;
	    bit     OnChangingValue = 0;
	   	bit   	displayOnValueWindow = 0;
//static  BYTE    OSDNumberString[5];

//extern  IDATA struct BarInfo BarInfo;
extern BYTE	year1,year2,month,day,hour,minute,second;

BYTE	PW_FirstNumber=0			;
BYTE	PW_SecondNumber=0			;
BYTE	PW_ThridNumber=0			;
BYTE	PW_FourthNumber=0			;
BYTE	DVR_DataChange=0;
#ifdef E_Wintek
bit	FactoryMenu;
#endif

extern BYTE  ReverseModeFlag;//william-v1.43-970402
#ifdef AWT_ML072S //william @20130902 v1.0.1.2
extern bit 	JUMPAVFlag;
#endif

extern BYTE  CAMNumber;
extern BYTE  PelcoChoiceFLAG;
extern BYTE  Baud_rate;
extern BYTE	DVR_daylightsaving;
extern BYTE	DVR_timebeginesmonth;
extern BYTE	DVR_timebeginesweek;
extern BYTE	DVR_timebeginesTime;
extern BYTE	DVR_sandardtimebeginesMonth;
extern BYTE	DVR_standardtimebeginesWeek;
extern BYTE	DVR_standardtimebeginesTime;
extern BYTE	DVR_Resolution			;
extern BYTE	DVR_FPS					;
extern BYTE	DVR_Quality				;
extern BYTE	DVR_RecordMode			;
extern BYTE	DVR_RecordLength		;
extern BYTE	DVR_PreAlarmLength		;
extern BYTE	DVR_PostAlarmLength		;
extern BYTE	FormatStatus			;
extern bit		FormatStart				;
extern BYTE	FormatWait				;
extern BYTE	DVR_TimeZone			;
extern BYTE	DVR_AudioRecord			;
extern BYTE	DVR_OverWrite;
extern bit	 	SpeedDomeMode;
extern BYTE	DVR_SDCardAvailable		;
extern BYTE	UpdateOK;
extern BYTE	DVR_RecordStatus		;
extern DATA BYTE	RS2_in;	
extern  CODE  struct struct_IdName struct_VInputStd[];
extern  CODE  struct struct_IdName struct_InputSelection[];
//extern  IDATA BYTE InputSelectionInx;
//extern	bit	  PowerMode ;
extern	bit   I2CAutoIncFlagOn ;
extern	BYTE  VInputStdDetectMode;
extern CODE BYTE Init_Osd_MainMenu[];
extern BYTE Information[14];
extern BYTE DayNightLevel;
#ifdef ADD_ANALOGPANEL
extern CODE BYTE Init_Osd_MainMenu_A[];
#endif
extern CODE unsigned char ParkTable[];
//extern  bit		DisplayInputHold;		//william-981029

#ifdef AWT_NO_SIGNAL_MSG  //Pinchi 20150320 add
extern bit IsDrawNoVideoSignal;
#endif
extern BYTE displayhold_flag;  //Pinchi 20150901 add

extern BYTE OSD_LOCKF;
extern BYTE Disp_Input;


struct MenuType   	CODE *MenuFormat;
struct RegisterInfo CODE *BarRange;
struct DefineMenu 	CODE *MenuTitle;
struct DefineMenu 	CODE *MenuChild;
struct DefineMenu	CODE *MenuStack[4];
static IDATA  BYTE MenuSP=0, MenuIndexStack[4];


#include "OSDString.h"
#include "menu.h"
#include "OSDOper.c"

#ifdef SUPPORT_LINE_OSDDEMO
 #include "OsdInitTable_Line.c"
#endif
//---------------------------------------------------------------------
// Push MenuTile, title_item in stack 
//      MenuTile[title_item][]...  
//
BYTE pushMenu(void)
{
	#ifdef DEBUG_OSD
	dPrintf("\r\n(pushMenu)-MenuSP:%d", (WORD)MenuSP);
	#endif
	if( MenuSP < sizeof( MenuIndexStack ) ) {
		#ifdef DEBUG_OSD
		{
			BYTE  **Desc;
			BYTE i;

			Desc = MenuTitle[title_item].Desc;
			for(i=0; i<OSDLang; i++)
				Desc += ( strlen(*Desc) + 1 );

			dPrintf("...MenuStack[%d]:Desc(%s)",(WORD)MenuSP, Desc);
		}
		#endif
		MenuStack[MenuSP] = MenuTitle;
		MenuIndexStack[MenuSP++] = title_item;
		return 1;
	}
	return 0;
}

BYTE popMenu(void)
{
	#ifdef DEBUG_OSD
	dPrintf("\r\n(popMenu)-MenuSP:%d", (WORD)MenuSP);
	#endif
	if( MenuSP ) {
		MenuTitle = MenuStack[--MenuSP];
		title_item = MenuIndexStack[MenuSP];
		#ifdef DEBUG_OSD
		{
			BYTE  **Desc;
			BYTE i;

			Desc = MenuTitle[title_item].Desc;
			for(i=0; i<OSDLang; i++)
				Desc += ( strlen(*Desc) + 1 );


			dPrintf("...MenuStack[%d]:Desc(%s)",(WORD)MenuSP, Desc);
		}
		#endif
		return 1;
	}
	return 0;
}		

BYTE GetLowerMenuItem( CODE_P struct DefineMenu *DMp, BYTE itemno )
{
	if( itemno==NIL )
		return NIL;
	itemno++;
	if( (DMp[itemno].IconColor==NIL) && (DMp[itemno].DescColor==NIL) )		// end
		return NIL;
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

BYTE GetTargetChild( BYTE tid)
{
	BYTE i;


	if( MenuChild )		i = 0;
	else				i = NIL;

	
	#ifdef DEBUG_OSD
	dPrintf("\r\nTarget id:0x%x", (WORD)tid);
	#endif

	while( i != NIL ) {
		
		#ifdef DEBUG_OSD
		dPrintf("__ForActionId[%d]:0x%x", (WORD)i, (WORD)MenuChild[i].Id);
		#endif
		
		if ( ( MenuChild[i].Id & 0x0f ) == tid )
			return i;//break;
		i = GetLowerMenuItem(MenuChild, i );
	}

	return i;
}
/*
BYTE GetTargetChild1( BYTE tid)
{
	BYTE i;


	if( MenuChild )		i = 0;
	else				i = NIL;

	
	#ifdef DEBUG_OSD
	dPrintf("\r\nTarget id:0x%x", (WORD)tid);
	#endif

	while( i != NIL ) {
		
		#ifdef DEBUG_OSD
		dPrintf("__ForActionId[%d]:0x%x", (WORD)i, (WORD)MenuChild[i]. PreFnId);
		#endif
		
		if ( ( MenuChild[i]. PreFnId & 0x0f ) == tid )
			return i;//break;
		i = GetLowerMenuItem( MenuChild, i );
	}
	return i;
}
*/
//===============================================================================
//
//  Display Information of the menu
//
//===============================================================================
CODE BYTE OSDInfoStr[][26]={
	{ "   Input  :              " },
	{ "   System :              " },
	{ "   H Freq :      kHz     " },	
	{ "   V Freq :       Hz     " },
	{ "   F/W Rev:              " }, 	
	{ "" }	
};
#define OSD_INFO_SUB_X	12//9

// Display the information on the screen.
// Input, System and F/W version
//----------------------------------------------------------------
void DisplayInfo_OSD(void)
{
	BYTE i, title=1, len, inputs;
	BYTE mstr[5];

	pushMenu();   //Jason Chen, the important subroutine.
	MenuTitle = MenuChild;
	title_item = cursor_item;

	OSDMenuLevel++; 
	
	MenuChild = 0;
	cursor_item = NIL;

	DrawMenulist(1); // Display Title

	DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH, OSD_INFO_COLOR, 5*MENU_WIDTH );	 // Line 1 ~ Line 5
	for(i=0; OSDInfoStr[i][0]; i++) 
		WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(i+title), OSDInfoStr[i], MENU_WIDTH ); 

	// Display Input
	inputs = GetInputSelection();
	for (i=1; ;i++)
		if( struct_InputSelection[i].Id==inputs )  break;
	len=strlen(struct_InputSelection[i].Name);
	WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(0+title)+OSD_INFO_SUB_X, struct_InputSelection[i].Name, len ); 

	// Display ColorSystem
	#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
	i = GetInputSelection();
	if( i==PC || i==DTV ){
		len=strlen(GetPCInputSourceName());
		WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(1+title)+OSD_INFO_SUB_X, GetPCInputSourceName(), len ); 
	}
	else
	#endif
	{
		len=strlen(struct_VInputStd[GetVInputStdInx()].Name);
		WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(1+title)+OSD_INFO_SUB_X, struct_VInputStd[GetVInputStdInx()].Name, len ); 
	}
#ifdef REFERENCE
	//IHF
	MeasureAndWait(3);

	ltoa_K(GetIHF(), mstr, 1);
	len=strlen(mstr);
	WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(2+title)+OSD_INFO_SUB_X, mstr, len ); 

	utoa(GetIVF(),mstr,10);
	len=strlen(mstr);
	WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(3+title)+OSD_INFO_SUB_X, mstr, len ); 
#endif
	//FWRev
	i = ReadEEP(EEP_FWREV_MAJOR);
	mstr[0] = i + '0';
	mstr[1] = '.';
	i = ReadEEP(EEP_FWREV_MINOR);
	mstr[2] = (i/0x10) + '0';
	mstr[3] = (i%0x10) + '0';
	mstr[4] = '\0';
#ifdef REFERENCE	//Weylis -0@121008
	WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(4+title)+OSD_INFO_SUB_X, mstr, 4 ); 
#else
	#ifdef HS_NEWOSDMENU_2
	DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH*(0+title)+OSD_INFO_SUB_X+HS_CHSPACE, DEFAULT_COLOR, 4 ); 
	WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(0+title)+OSD_INFO_SUB_X+HS_CHSPACE, mstr, 4 ); 
	#else
	WriteStringToAddr(OSDMENU_SUBADDR+MENU_WIDTH*(2+title)+OSD_INFO_SUB_X, mstr, 4 ); 
	#endif
#endif
}

//********************************************************************
//
//	Functions for BAR.
//
//********************************************************************
//#define VOLBAR_LENGTH 21+3
#define VOLBAR_LENGTH 21//+3//ryan@21070919

void DisplayVolumebar(BYTE val)
{
#if 1//ryan@20170919

	CODE BYTE BAR_ROM_TABLE[] = {
		0x01, // ...
		0x02,0x02, // |..
		0x05,0x05,0x04, // ||.
		0x03, // |
		0x04, // |||
	};
	
	BYTE i;
	BYTE Bar_str[VOLBAR_LENGTH]={   0x04, 0x04,
							   0x04, 0x04, 0x04, 0x04, 0x04,
							   0x04, 0x04, 0x04, 0x04, 0x04,
							   0x04, 0x04, 0x04, 0x04, 0x04, 0x03,' ',' ',' ' };

	WORD addr;
	
	if(val <100)
	{
		Bar_str[val/6] = BAR_ROM_TABLE[val%6];
		for(i = val/6+1; i < 17; i++)
			Bar_str[i] = BAR_ROM_TABLE[0];
	}

	if( val==99 )
		Bar_str[16] = 0x05; // ||.


	if( Bar_str[0] == 0x01 )
		Bar_str[0] = 0x02;	// |..

	// Number
    Bar_str[20] = val % 10 + '0';    val /= 10;
    Bar_str[19] =  val % 10 + '0';    val /= 10;
    Bar_str[18] = val + '0';

	if( Bar_str[18] == '0' )
		Bar_str[18] = ' ';

	addr = OSDMENU_BARADDR  + VOLBAR_LENGTH;

	WriteStringToAddr(addr, Bar_str, VOLBAR_LENGTH );
#else
	CODE BYTE BAR_ROM_TABLE[] = {
		0x01, // ...
		0x02,0x02, // |..
		0x05,0x05, // ||.
		0x03, // |
		0x04, // |||
	};

	BYTE i;
	BYTE Bar_str[VOLBAR_LENGTH]={ 0x04, 0x04, 0x04, 0x04, 0x04,
							   0x04, 0x04, 0x04, 0x04, 0x04,
							   0x04, 0x04, 0x04, 0x04, 0x04,
							   0x04, 0x04, 0x04, 0x04, 0x04, 0x03,' ',' ',' ' };

	WORD addr;
	
	if(val < 100)
	{
		Bar_str[val/5] = BAR_ROM_TABLE[val%5];
		for(i = val/5 + 1; i < 20; i++)
			Bar_str[i] = BAR_ROM_TABLE[0];
	}

	if( Bar_str[0] == 0x01 )
		Bar_str[0] = 0x02;	// |..

	// Number
    Bar_str[23] = val % 10 + '0';    val /= 10;
    Bar_str[22] =  val % 10 + '0';    val /= 10;
    Bar_str[21] = val + '0';

	if( Bar_str[21] == '0' )
		Bar_str[21] = ' ';

	addr = OSDMENU_BARADDR + 1 + VOLBAR_LENGTH;

	WriteStringToAddr(addr, Bar_str, VOLBAR_LENGTH );
	#endif
}

BYTE Get_cursor_item(BYTE id)
{
	BYTE i=NIL, tid;

	#ifdef DEBUG_OSD
	dPrintf("\r\n++(Get_cursor_item) id:%d", (WORD)id);
	#endif

	
	switch ( id ) {
	case INPUTSELECTION:	tid = GetInputSelection(); 
							break;

	default:				return 0;// The first one
	}

	i = GetTargetChild( tid );

	if( i==NIL ) {	//couldn't find
		dPuts("___Couldn't find cursor!!");
		i=0;
	}

	#ifdef DEBUG_OSD
	dPrintf("___Cursor:%d\r\n__(GetDefaultForChoiceMenu)", (WORD)i);
	#endif

	return i;
}

//==================================================================================================
// Func: DisplayOSDMenuOneItem
//
//
void DisplayOSDMenuOneItem( winno, CODE_P struct DefineMenu *DMp, BYTE x, BYTE y, BYTE NOTViewflag )
{
	#ifdef HS_NEWOSDMENU
	static BYTE xx;
	#endif	
	BYTE i,j,k,IconColor;
	WORD addr;
	#ifdef HS_NEWOSDMENU	
	WORD addr1;
	#endif	
	CODE_P BYTE *Str;
	//CODE BYTE RightSign_str[]=	{ 0x1c,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',0 };// RIGHT mark //Weylis
	CODE BYTE RightSign_str[]=	{ 0x1c,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',0 };// RIGHT mark
	CODE BYTE SpaceStr[]=  { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',0 }; // V1.1 02/28/07 simonsung
	CODE BYTE Dot[]=  { '.',0 };
	CODE BYTE Colon[]=  { ':',0 };
		
	#ifdef DEBUG_OSD
	dPrintf("\r\n++(DisplayOSDMenuOneItem) ");
	#endif

	if( cur_osdwin == OSDMENU_TOPWIN) addr = OSDMENU_MAINADDR;
	else addr = OSDMENU_SUBADDR;

	#ifdef HS_NEWOSDMENU
	if (y == 0)
		xx = x;
	else
		xx += 2;
		
	addr1 = (addr) + (xx) + (y*1) ;
	#endif	
	addr = addr + x + y*MenuFormat->width;

	#ifdef DEBUG_OSD
	dPrintf("\r\nStartaddr:%d", addr1);
	//dPrintf("\r\nStartaddr:%d", addr);
	#endif

	if( DMp->IconColor !=NIL ) {		// Draw Icon
		//if(OnChangingValue) IconColor = BG_COLOR_BLACK|CH_COLOR_WHITE|EXTENDED_COLOR; 
		//else
			IconColor =  DMp->IconColor;

		Str = DMp->Icon;
		j=strlen(Str);
	#ifdef HS_NEWOSDMENU
	WriteStringToAddr(addr1, Str, j);  //Weylis		
	#else
	WriteStringToAddr(addr, Str, j);
	#endif		

		x += ICON_CH_SZ;
	}
//Weylis -@121008 
	#ifdef HS_NEWOSDMENU_2	
 	DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH+HS_CHSPACE, DEFAULT_COLOR, (1*MENU_WIDTH) );	 // Line 1 ~ Line 5
	addr+=HS_CHSPACE;
 	#else
 //	DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH, DEFAULT_COLOR, (6*MENU_WIDTH) );	 // Line 1 ~ Line 5
 	DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH, DEFAULT_COLOR, (MENU_HEIGHT*MENU_WIDTH) );//ryan@20170825	 // Line 1 ~ Line 5
 	#endif
//

	if( DMp->DescColor !=NIL ) {		// Draw Desc.

		Str = DMp->Desc[OSDLang];
		j=strlen(Str);

		for( i=0,k=0; i<j; i++ )
			if( *(Str+i) == ROMFONT || *(Str+i) == RAMFONT ) k++;

		#ifdef DEBUG_OSD
		dPrintf("\r\naddr:%d x:%d y:%d MenuFormat->width:%d", addr, (WORD)x, (WORD)y, (WORD)MenuFormat->width );
		#endif

		WriteStringToAddr(addr, Str, j);

		if( (j-k ) < MAX_DESC ) 
			ClearDataCnt(addr+j-k, MAX_DESC-j+k);
	
		addr+=MAX_DESC;

	}

	if( OSDMenuLevel < 3) // Main Menu Item.
	 if ( NOTViewflag==1 )	return;	// In case of not need diplay contents. ex, BAR windows, top menu display.
	
	// Menu contents view
	/*if( x < (MenuFormat->width) )
		if( x <  (MAX_DESC + ICON_CH_SZ) ){
		//x+=2;
		ClearDataCnt(addr, 2);
		addr+=2;
	}*/

	switch (DMp->Type ) {
	case BAR:	// Draw Bar and Number value
	   		  #ifdef AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150209 add
	           if (((DMp->Id) == TRIGGER_CamADelay) || ((DMp->Id) == TRIGGER_CamBDelay)
      		     	  || ((DMp->Id) == TRIGGER_CamCDelay) || ((DMp->Id) == TRIGGER_CamRDelay))
            		   AWT_DisplayViewBar(addr , GetItemValue(DMp->Id));
           		else
       		#endif
	   		DisplayViewBar( addr, GetItemValue(DMp->Id));
		break;

	case NUMBER: // Draw Number value
		{
			CODE_P BYTE *ptr;
			ptr = DMp->TypeData;
			switch ( DMp->Id ) {
			#if defined( PAL_TV )
			case MANUAL_TUNE_CHANNEL:
				WriteStringToAddr(addr, RightSign_str, BARNUM_LENGTH); 
				break;
			#endif
			default:
				DrawNum( addr, (CODE_P struct NumType *)ptr, GetItemValue(DMp->Id) );
				break;
			}
		}
		break;
		
	case INFORMATION: // Draw Number value
		{
			CODE_P BYTE *ptr;
			ptr = DMp->TypeData;
			switch ( DMp->Id ) 
				{
			case WIFIIPNumber:							
				DrawNum( addr-15, (CODE_P struct NumType *)ptr, Information[0]);
				WriteStringToAddr(addr-12, Dot, 1); 
				DrawNum( addr-11, (CODE_P struct NumType *)ptr, Information[1] );
				WriteStringToAddr(addr-8, Dot, 1); 
				DrawNum( addr-7, (CODE_P struct NumType *)ptr, Information[2] );
				WriteStringToAddr(addr-4, Dot, 1); 
				DrawNum( addr-3, (CODE_P struct NumType *)ptr, Information[3]);
				break;

			case WIFIMACNumber:						
				DrawNum( addr-15, (CODE_P struct NumType *)ptr, Information[4]);
				WriteStringToAddr(addr-12, Colon, 1); 
				DrawNum( addr-11, (CODE_P struct NumType *)ptr, Information[5] );
				WriteStringToAddr(addr-8, Colon, 1); 
				DrawNum( addr-7, (CODE_P struct NumType *)ptr, Information[6] );
				WriteStringToAddr(addr-4, Colon, 1); 
				DrawNum( addr-3, (CODE_P struct NumType *)ptr, Information[7]);
				WriteStringToAddr(addr, Colon, 1); 
				DrawNum( addr+1, (CODE_P struct NumType *)ptr, Information[8]);
				WriteStringToAddr(addr+4, Colon, 1); 
				DrawNum( addr+5, (CODE_P struct NumType *)ptr, Information[9]);
				break;

			case GETWAYNumber:						
				DrawNum( addr-15, (CODE_P struct NumType *)ptr, Information[10]);
				WriteStringToAddr(addr-12, Dot, 1); 
				DrawNum( addr-11, (CODE_P struct NumType *)ptr, Information[11] );
				WriteStringToAddr(addr-8, Dot, 1); 
				DrawNum( addr-7, (CODE_P struct NumType *)ptr, Information[12] );
				WriteStringToAddr(addr-4, Dot, 1); 
				DrawNum( addr-3, (CODE_P struct NumType *)ptr, Information[13]);
				break;	
				}
		}
		break;
		
	case ENUM_STRING:		
		{
			CODE_P BYTE *ptr;

			ptr = DMp->TypeData;
			DrawStringByLang( addr, (CODE_P struct EnumType *)ptr, GetItemValue(DMp->Id) );

		}
		break;

	case NUMBER_STRING:
	case CHOICEMENU:
	case OSDMENU:
	case ACTION:
		 WriteStringToAddr(addr, RightSign_str, BARNUM_LENGTH); 
		 break;

#ifdef HTRV
	case GRAPH:				//william-v1.43-970321
         WriteStringToAddr(addr, RightSign_str, BARNUM_LENGTH); 
         break;
#endif
			
	case TEXT_STR:  // V1.1 02/28/07 simonsung
	     WriteStringToAddr(addr, SpaceStr, BARNUM_LENGTH); 
		 break;
	}
}

void ClearCursor(void)
{
	//BYTE cursor_x;
	BYTE cursor_y;
	WORD addr;

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
}

void DisplayCursor(void)
{
	BYTE cursor_y=0,i ;
	WORD addr;
#ifdef HS_NEWOSDMENU  //weylis -@121008
	CODE_P BYTE 	*Str;
#endif
	if( cursor_item==NIL )
		return;

//    for(i=0;i<8;i++)  //william-20091026
    for(i=0;i<(MENU_HEIGHT+1);i++)  //ryan@20170825
    {
      cursor_y = i ;
      addr = OSDMENU_SUBADDR;  
      addr += cursor_y*MenuFormat->width; 
      DrawAttrCnt(addr, DEFAULT_COLOR, MenuFormat->width );  // 清除殘留小方格 
    }

	cursor_y = cursor_item + ( MenuFormat->TitleColor ==NIL ? 0 : 1 );
#ifdef HS_NEWOSDMENU_2
	if( OSDMenuLevel == 1 ) addr = OSDMENU_MAINADDR+HS_CHSPACE;
	//if( OSDMenuLevel == 1 ) addr = OSDMENU_MAINADDR;
	else addr = OSDMENU_SUBADDR+HS_CHSPACE;
#else
	if( OSDMenuLevel == 1 ) addr = OSDMENU_MAINADDR;
	else addr = OSDMENU_SUBADDR;
#endif
	addr += cursor_y*MenuFormat->width;

	if( OSDMenuLevel == 1)
		DrawAttrCnt(addr, MenuFormat->CursorColor, MenuFormat->width );	
	else
	{
		if(OnChangingValue)
		{
			GetItemValue(GetItemValue(MenuChild[cursor_item].Id));
			DrawAttrCnt(addr, MenuFormat->CursorColor, MenuFormat->width );	
		}
		else
		{
			DrawAttrCnt(addr, MenuFormat->CursorColor, MenuFormat->width-BARNUM_LENGTH );	
			DrawAttrCnt(addr+MAX_DESC, DEFAULT_COLOR, BARNUM_LENGTH );	
		}
	}

	//Weylis -@121008
#ifdef HS_NEWOSDMENU_2  //Pinchi modify condition for compiler warning //Weylis081202 mark not shoe comment  Show Comment String
	#if 0
			//DrawAttrCnt((OSDMENU_SUBADDR+TITLE_MAX_DESC*MENU_HEIGHT)+18, COMMENT1_COLOR, TITLE_MAX_DESC );	// All Line Attr clear by default color.
			DrawAttrCnt((OSDMENU_SUBADDR+TITLE_MAX_DESC*MENU_HEIGHT)+HS_CHSPACE, COMMENT1_COLOR, TITLE_MAX_DESC );	// All Line Attr clear by default color.
	#endif
	/*		
			if(ChkOSDMENU_TOPWIN())
				Str = Str_Comments[OSDLang];
			else
				if(ChkOSDMENU_WINDOW())
					Str = Str_Comments1[OSDLang];
				else
					if (MenuTitle[cursor_item].Id == DISP_INFO)
						Str = Str_Comments[OSDLang];
				else
					
					Str = Str_Comments[OSDLang];
			i = strlen(Str);
			WriteStringToAddr((OSDMENU_SUBADDR+TITLE_MAX_DESC*MENU_HEIGHT)+HS_CHSPACE, Str, i);
	*/
#endif
}

//===============================================================================
//  OSD Menu Operation funtion moved to "OSDoper.c" by RYU.
// 
//===============================================================================
WORD GetItemValue( BYTE id)
{
	WORD val=0;

	#ifdef DEBUG_OSD
	dPrintf("\r\n++(GetItemValue) id:0x%x __", (WORD)id);
	#endif

	switch( id ) {
	//case SYSTEM_CLOCK:	val = GetSystemClock_HM();			break;
	//case WAKEUP_TIME:		val = OSDItemValue = GetWakeupTime();	break;
	//case OFF_TIME:		val = OSDItemValue = GetOffTime();		break;
	//case SLEEP_TIMER:		val = OSDItemValue = GetSleepTimer();	break;


#ifdef HTRV  //Pinchi 20140902 add for AWT ML072Q
//WILLIAM-V1.43-970318
    case Gauge_VStart: 
           val = ReadEEP(EEP_Gauge_VStart);
           GaugeVStart=val;    
           break;
//WILLIAM-V1.43-970318
#endif

#ifdef SUPPORT_OSDPOSITIONMOVE
	case OSDPOSITIONMODE:   val = GetOSDPositionMode();				break;
	//case OSD_VPOS:			val = GetOSDMenuYValue();				break;
	//case OSD_HPOS:			val = GetOSDMenuXValue();				break;
#endif
//	case OSD_DURATION:		val = GetOSDDurationValue();			break;
	case OSDLANG:			val = OSDLang & 0x0f;					break;

	#ifdef SUPPORT_PC
	case PANEL_CONTRAST:	val = OSDItemValue = GetPanelContrastEE();				break;
	case PANEL_BRIGHTNESS:	val = OSDItemValue = GetPanelBrightnessEE();			break;

	case PC_VPOS:			val = GetVPosition();					break;
	case PC_HPOS:			val = GetHPosition();					break;
	case PC_CLOCK:			val = GetCoarseValue();					break;
	case PC_PHASE:			val = GetPhaseCurrent();				break;

	case PC_COLOR_TEMP:		val = GetPCColorTempModeEE();			break;
	case PC_COLOR_R:		val = GetPCColorTemp(RED);				break;	
	case PC_COLOR_G:		val = GetPCColorTemp(GREEN);			break;	
	case PC_COLOR_B:		val = GetPCColorTemp(BLUE);				break;	
	#endif

	case VIDEO_MODE:		val = GetVideoModeEE();					break;

	case VIDEO_CONTRAST:	
	case VIDEO_BRIGHTNESS:	
	case VIDEO_SATURATION:	
	case VIDEO_HUE:			
	case VIDEO_SHARPNESS:	
	case VIDEO_BACKLIGHT:	
							val = GetVideoDatawithOffset(id-VIDEO_CONTRAST);	break;
	case AUDIO_VOLUME:		
//if((NowModeState==SelectMode&&SelectModeType==VGAMode)||(NowModeState==JumpMode&&((ReadEEP(EEP_JUMPMode)==14)||SelectModeType==VGAMode)))		//william-v1.1-20110114
		if(VGAAudio)
			val=PCAudioVol;
		else
			val = AudioVol;
		break;

	#if 0
	case AUDIO_BALANCE:		val = GetAudioBalance();				break;
	case AUDIO_BASS:		val = GetAudioBass();					break;
	case AUDIO_TREBLE:		val = GetAudioTreble();					break;
    #endif
	//case AUDIO_EFFECT:		val = GetAudioEffect();					break;
	//case AUDIO_MUTE:		val = IsAudioMuteOn();					break;

	case INPUTSTD:			val = OSDItemValue = VInputStdDetectMode;			
							break;

	#ifdef WIDE_SCREEN		//.................................................
	case WIDESCREEN:
							val = WideScreenMode;					break;
	#endif					//.................................................
    case DayChoice:           
         val =ReadEEP(EEP_AutoMode);
         //val=DayNightLevel;
	     break;
	case CAMOSDONOFF:         
         val = ReadEEP(EEP_COSDMode);
	     break;
	case DVRDisplay:         
         val = ReadEEP(EEP_DVRDisplay);
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
	case ReverseMode:
         val = ReadEEP(EEP_ReverseMode);
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
	case IMAGEChoice:
        val = ReadEEP(EEP_IMAGEMode);
	    break;
	case IMAGE_A_Choice: 				   //左右轉顯示畫面
	 val = ReadEEP(EEP_IMAGE_A_Mode);  //andy A1.4 20100113
		   break;
   	case IMAGE_B_Choice: 				   //左右轉顯示畫面
	 val = ReadEEP(EEP_IMAGE_B_Mode);  //andy A1.4 20100113
		   break;
   	case IMAGE_C_Choice: 				   //左右轉顯示畫面
	 val = ReadEEP(EEP_IMAGE_C_Mode);  //andy A1.4 20100113
		   break;		
	case REARChoice:
        val = ReadEEP(EEP_RearMode);
	    break;
	case TriggerVGA:
        val = ReadEEP(EEP_TriggerVGA);
	    break;	

//#ifndef	AWT_ML072Q  //Pinchi 20140918 add for AWT ML072Q
#ifdef  SUPPORT_ML84TQ_TRIGGER_DELAY  //Pinchi 20150825 add for function define
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
  #ifdef USE_CAMD		
	case DELAYCAMD:
        val = ReadEEP(EEP_DELAYCAMD);	
	    break;	
  #endif
#endif

	case JUMPChoice:
        val = ReadEEP(EEP_JUMPMode);
	    break;
	case CAMOUTChoice:
        val = ReadEEP(EEP_CAMOutMode);
	    break;
	case STIME_STEP:
        val = ReadEEP(EEP_TimeStep);
		//TimeStep=val; 
	    break;

#ifdef AWT_ML072S	//william @20130902	v1.1.1.2
	case JUMP_AV:
           val =ReadEEP(EEP_JUMPAV_TIMESET);
           JUMPAVFlag=val;
		   break;
#endif

#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
	case SDCardDetect:
	        val = ReadEEP(EEP_SDCardDetect);
			//TimeStep=val; 
		    break;
		
//#ifdef SUPPORT_SDCARD		
	case Resolution:
		val = DVR_Resolution;
		break;
	case FPS:
		val = DVR_FPS;
		break;
	case Quality:
		val = DVR_Quality;
		break;
	case RecordMode:
		val = DVR_RecordMode;
		break;
	case RecordLength:
		val = DVR_RecordLength;
		break;
	case PreAlarmLength:
		val = DVR_PreAlarmLength;
		break;
	case PostAlarmLength:
		val = DVR_PostAlarmLength;
		break;

	case AudioRecord:
		val = DVR_AudioRecord;
		break;
		
	case Year:
		val = year2;
		break;
	case Month:
		val = month;
		break;
	case Day:
		val = day;
		break;	
	case Hour:
		val = hour;
		break;
	case Minute:
		val = minute;
		break;

	case DaylightSaving:
		val = DVR_daylightsaving;
		break;
	case TimeBeginesMonth:
		val = DVR_timebeginesmonth;
		break;
	case TimeBeginesWeek:
		val = DVR_timebeginesweek;
		break;
	case TimeBeginesTime:
		val = DVR_timebeginesTime;
		break;
	case StandardTimeBeginesMonth:
		val = DVR_sandardtimebeginesMonth;
		break;			
	case StandardTimeBeginesWeek:
		val = DVR_standardtimebeginesWeek;
		break;
	case StandardTimeBeginesTime:
		val = DVR_standardtimebeginesTime;
		break;

	case TimeZone:
		val = DVR_TimeZone;
		break;

	case OverWrite:
		val = DVR_OverWrite;
		break;

	case FirstNumber:
		val = PW_FirstNumber;
		break;

	case SecondNumber:
		val = PW_SecondNumber;
		break;

	case ThridNumber:
		val = PW_ThridNumber;
		break;
	
	case FourthNumber:
		val = PW_FourthNumber;
		break;

#endif	//#ifdef SUPPORT_SDCARD	

       case CAMNumberChoice:      		//TM0350-980227 
               val = ReadEEP(EEP_CAMNUMBER);
               break;
       case PelcoChoice:        		//TM0350-980227
               val = ReadEEP(EEP_PELCO);
               break;
    
       case Baud_rateChoice:        		//TM0350-980227
               val = ReadEEP(EEP_Baud_rate);
			   break;
	#ifdef AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150209 add
    case TRIGGER_CamADelay :
           val = ReadEEP(EEP_CamADelay);
           break;

    case TRIGGER_CamBDelay :
           val = ReadEEP(EEP_CamBDelay);
           break;

    case TRIGGER_CamCDelay :
           val = ReadEEP(EEP_CamCDelay);
           break;

    case TRIGGER_CamRDelay :
           val = ReadEEP(EEP_CamRDelay);
           break;
#endif		   
	}

	return val;
}

#define MAX_VIDEOMODE	4
#define MAXVCHIPMOVIE	7 

WORD SetItemValue( BYTE id, BYTE flag )
{
	WORD val=0;
//	BYTE	i;

	#ifdef Hs_debug
	Printf("\r\nSET Item Value: id-%X", (WORD)id);
	#endif
	
	switch( id ) {

	//WILLIAM-970318
	#ifdef HTRV
    case Gauge_VStart:    
         val=ReadEEP(EEP_Gauge_VStart);
         if(flag==UP)
         {
         #if 1  //Pinchi 20150901 add to expand the adjust range by follow HTRV ML072Q of TW8806
		     if(val < 200) val += 2;
         #else    
             if(val<100) val+=2;//william-v1.43-970321
         #endif
             // else val=2;  2007.4.19  Disable Justin
         }           
         else 
         {
             if(val>0/*2*/) val-=2;//william-v1.43-970321
             // else val=100;   2007.4.19  Disable Justin
         }      
              
         WriteEEP( EEP_Gauge_VStart,val);  
         GaugeVStart=val;   
         break;
	#endif
	// WILLIAM-970318
		
	//case SYSTEM_CLOCK_H:	
	//case WAKEUP_TIME_H:		
	//case OFF_TIME_H:
	//						val = SetClock_hour(flag);				break;

	//case SYSTEM_CLOCK:
	//case WAKEUP_TIME:
	//case OFF_TIME:
	//						val = SetClock_min(flag);				break;
	//case SLEEP_TIMER:		val = ChangeSleepTimer( flag );			break;

#ifdef SUPPORT_OSDPOSITIONMOVE
	case OSDPOSITIONMODE:    val = SetOSDPositionMode( flag );		break;
	//case OSD_VPOS:			val = SetOSDMenuYValue(flag);			break;
	//case OSD_HPOS:			val = SetOSDMenuXValue(flag);			break;
#endif

//	case OSD_DURATION:		val = SetOSDDurationValue(flag);		break;
	case OSDLANG:			val = SetLanguage (flag);				break;

	#if defined(SUPPORT_PC) || defined(SUPPORT_DTV)
	case PANEL_CONTRAST:	val = SetPanelContrast(flag);			break;
	case PANEL_BRIGHTNESS:	val = SetPanelBrightness(flag);			break;
	#endif

	#ifdef SUPPORT_PC
	case PC_VPOS:			val = SetVPosition(flag);				break;
	case PC_HPOS:			val = SetHPosition(flag);				break;
	case PC_CLOCK:			val = SetPCCoarseValue(flag);			break;
	case PC_PHASE:			val = SetPhaseValue(flag);				break;
	#endif

	case PC_COLOR_TEMP:		val = SetPCColorTemp(flag);			break;

	case PC_COLOR_R:		val = SetPCColor(RED, flag);			break;	
	case PC_COLOR_G:		val = SetPCColor(GREEN, flag);			break;	
	case PC_COLOR_B:		val = SetPCColor(BLUE, flag);			break;	

	case VIDEO_MODE:		val = GetVideoModeEE();
							if(flag==UP) val ++;
							else val = val + MAX_VIDEOMODE -1;
							val %= MAX_VIDEOMODE ; 
							SetVideoMode(val);		
							
							if( GetVideoModeEE() != 3 )  // if not user mode, mask the control.
							#ifdef HS_NEWOSDMENU_2 
							 	DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH*2+HS_CHSPACE, BACK_COLOR, (MENU_HEIGHT-1-1)*MENU_WIDTH);	//Weylis 081226
							#else
								DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH*2, BACK_COLOR, (MENU_HEIGHT-1)*MENU_WIDTH );	
							#endif	
							else
							#ifdef HS_NEWOSDMENU_2
								DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH*2+HS_CHSPACE, DEFAULT_COLOR, (MENU_HEIGHT-1-1)*MENU_WIDTH);	//Weylis 081226
							#else
								DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH*2, DEFAULT_COLOR, (MENU_HEIGHT-1)*MENU_WIDTH );	
							#endif	
																	break;

	case VIDEO_CONTRAST:	
	case VIDEO_BRIGHTNESS:	
	case VIDEO_HUE:			val= ChangeVideoData(id-VIDEO_CONTRAST,flag);
																	break;
	case VIDEO_SATURATION:	val = ChangeVideoData(id-VIDEO_CONTRAST,flag);	
							ChangeVideoData(id+1-VIDEO_CONTRAST,flag);
																	break;
							//val = SetVideoSaturation(V_SAT, flag);	break;
	case VIDEO_SHARPNESS:	val = SetVideoSharpness(flag);			break;
	case VIDEO_BACKLIGHT:	val = ChangeVideoData(id-VIDEO_CONTRAST,flag);
																	break;
	case AUDIO_VOLUME:		val = ChangeVol( (flag==UP ? 1 : -1) ); break;

    #if 0
	case AUDIO_BALANCE:		val = ChangeBalance( (flag==UP ? 1 : -1) );	break;
	case AUDIO_BASS:		val = ChangeBass( (flag==UP ? 1 : -1) );	break;
	case AUDIO_TREBLE:		val = ChangeTreble( (flag==UP ? 1 : -1) );	break;
    #endif

	#ifdef WIDE_SCREEN
	case WIDESCREEN:			val = WideScreenMode;
								if(flag==UP) val++;
								else val = ( val+MAXWIDESCREEN -1 );	
								val %= MAXWIDESCREEN;
								Set4WideScreen( val );
								break;
	#endif	//WIDE_SCREEN

    case DayChoice:           
         val = ReadEEP(EEP_AutoMode);
     //    DayNightLevel=val;
		 
		#ifdef SUPPORT_ML072Q_AUTO_DAY_NIGHT  //Pinchi 20150825 add  //AWT_ML072Q  //Pinchi 20140902 add for AWT ML072Q
		  #if 1
		  if (flag == UP)
			  val = (!val ? 1 : 0);
		  else 
			  val = (val ? 0 : 1);
		  #else
          if (flag == UP)
          {
           	if (DayNightLevel < 1) 
		   		DayNightLevel += 1;
            else 
				DayNightLevel=0;
          }            
          else 
          {
             if (DayNightLevel > 0) 
			 	DayNightLevel -= 1;
             else
			 	DayNightLevel=1;	
          }
		  #endif
		#else
         if(flag==UP)
          {
           if(DayNightLevel<2) DayNightLevel+=1;
             else DayNightLevel=0;
          }            
          else 
          {
             if(DayNightLevel>0) DayNightLevel-=1;
             else DayNightLevel=2;	
          }
		#endif
		
	//	val=DayNightLevel;
		DayNightLevel = val;
		WriteEEP( EEP_AutoMode,val);
	//	 AutoDayNight=val;
	//	 if(val==1)
	//	 	AdjustAutoDayNight(0xff);
         //DaySelect( val );              //設定Auto Day & Night動作(No=0,OFF=1)
	     break;

	case CAMOSDONOFF:         
         val = ReadEEP(EEP_COSDMode);                                                     
         if(flag==UP)                                                        
              val = (!val ? 1 : 0);                                             
         else                                              
              val = (val ? 0 : 1);                                              
         WriteEEP(EEP_COSDMode,val);   

		Disp_Input=val;
	if(val==0) SetNowMode();//ryan@20170922
		 
		
	     break;

	case DVRDisplay:        
         val = ReadEEP(EEP_DVRDisplay);                                                    
         if(flag==UP)                                  
           val = (!val ? 1 : 0);                                             
         else                                                                
           val = (val ? 0 : 1);                                              
         WriteEEP(EEP_DVRDisplay,val);  
	     break; 
		 
	case GAUGEChoice:        
         val = ReadEEP(EEP_GAUGEMode);                                                    
         if(flag==UP)                                  
           val = (!val ? 1 : 0);                                             
         else                                                                
           val = (val ? 0 : 1);                                              
         WriteEEP(EEP_GAUGEMode,val);  
	     break;

	case OsdLockChoice:        
         val = ReadEEP(EEP_ONOFFChoice);                                                    
         if(flag==UP)                                  
            val = (!val ? 1 : 0);                                             
         else                                                                
            val = (val ? 0 : 1);                                              
         WriteEEP(EEP_ONOFFChoice,val);   
		 OSD_LOCKF=val;
	     break;

	case PRIORITYChoice:
		val = ReadEEP(EEP_PRIORITY);												   
		if(flag==UP)								  
			val = (!val ? 1 : 0); 											
		else																
			val = (val ? 0 : 1);	
		
			Priority= val;
		if(Priority==0)  ///1==off, 0==on
		{		
			 WriteEEP( EEP_JUMPMode,0);
			 WriteEEP(EEP_ONOFFChoice,0x01);     // OSD  lock....... off    	
			 OSD_LOCKF=0x01;
		}
		else
		{	
			WriteEEP(EEP_JUMPMode,13); 
			WriteEEP(EEP_ONOFFChoice,0x00);     // OSD  lock....... on   
			 OSD_LOCKF=0x00;
			if(SelectModeType>CAM_R)
			{
				SelectModeType=CAM_A;
				SetNowMode();
				WriteEEP(EEP_SelectMode,SelectModeType);
			}
		}

		WriteEEP(EEP_PRIORITY,val);																					  
		break; 

	case ReverseMode: 										   
		val = ReadEEP(EEP_ReverseMode);													   
		if(flag==UP)														  
			val = (!val ? 1 : 0);											  
		else											 
			val = (val ? 0 : 1);						 
		WriteEEP(EEP_ReverseMode,val);	
		ReverseModeFlag=val;
		break;
										 //CAMA Mirror開關設定										
	case CAMAONOFF: 										   
		  val = ReadEEP(EEP_CAMAMode);													   
		  if(flag==UP)														  
			val = (!val ? 1 : 0);											  
		  else											 
			val = (val ? 0 : 1);						 
		  WriteEEP(EEP_CAMAMode,val);	
	
		  if(val==0)					 //ON			   
			Attr_A|=0x01;				 
		  else							 //OFF			  
			Attr_A&=0x0E;			
	#ifdef QUAD
		if(GetInputSelection()==CH_TW2835)
		  SetNowMode();
		else
		  TW2835Command(CAM_QUAD,Attr_A,Attr_B,Attr_C,Attr_R);
	#endif
		  break;										 
														 
	case CAMBONOFF: 					 //CAMB Mirror開關設定				   
		  val = ReadEEP(EEP_CAMBMode);								  
		  if(flag==UP)									 
		   val = (!val ? 1 : 0);						 
		  else											 
		   val = (val ? 0 : 1); 						 
		  WriteEEP(EEP_CAMBMode,val);	
	
		  if(val==0)					//ON			  
			 Attr_B|=0x01;				 
		  else							//OFF							 
			 Attr_B&=0x0E;	
	#ifdef QUAD
		if(GetInputSelection()==CH_TW2835)
		  SetNowMode();
		else
		  TW2835Command(CAM_QUAD,Attr_A,Attr_B,Attr_C,Attr_R);
	#endif		
		  break;										 
			 
	case CAMCONOFF: 					 //CAMC Mirror開關設定		 
		   val = ReadEEP(EEP_CAMCMode); 										   
		   if(flag==UP) 									  
			 val = (!val ? 1 : 0);							  
		   else 								
			 val = (val ? 0 : 1);							  
		   WriteEEP(EEP_CAMCMode,val);	 
		   
		   if(val==0)					 //ON						   
			 Attr_C|=0x01;						  
		   else 						 //OFF							 
			 Attr_C&=0x0E;	
	#ifdef QUAD		   
		if(GetInputSelection()==CH_TW2835)
		  SetNowMode();
		else
		  TW2835Command(CAM_QUAD,Attr_A,Attr_B,Attr_C,Attr_R);
	#endif		
		   break;									  
										  
	case CAMRONOFF: 					 //CAMR Mirror開關設定					   
		   val = ReadEEP(EEP_CAMRMode); 									 
		   if(flag==UP) 									  
			 val = (!val ? 1 : 0);			
		   else 
			 val = (val ? 0 : 1);		 
		   WriteEEP( EEP_CAMRMode,val);   
		 
		   if(val==0)					 //CAM OSD ON							
			 Attr_R|=0x01;			  
		   else 						 //CAM OSD OFF							
			 Attr_R&=0x0E;
	#ifdef QUAD		   
		if(GetInputSelection()==CH_TW2835)
		  SetNowMode();
		else
		  TW2835Command(CAM_QUAD,Attr_A,Attr_B,Attr_C,Attr_R);
	#endif		
		   break;

#ifdef HTRV  //Pinchi 20150825 add
	case IMAGEChoice:
        val = ReadEEP(EEP_IMAGEMode);
        if(flag==UP)
        {
           if(val<2) val+=1;
           else      val=0;
        }            
        else 
        {
           if(val>0) val-=1;
           else      val=2;
        }                                                     
        WriteEEP(EEP_IMAGEMode,val);   
		break;
#else
	case IMAGE_A_Choice: 				   //左右轉顯示畫面設定(0=SINGLE , 1=DUAL , 2=TRIPLE)
			val = ReadEEP(EEP_IMAGE_A_Mode);  //andy A1.4 20100113
			if(flag==UP)
			{
			   if(val<6) val+=1;
			   else val=0;
			}			 
			else 
			{
			   if(val>0) val-=1;
			   else val=6;
			}
			WriteEEP(EEP_IMAGE_A_Mode,val);   
			break;
       
    case IMAGE_B_Choice:                    //左右轉顯示畫面設定(0=SINGLE , 1=DUAL , 2=TRIPLE)
           val = ReadEEP(EEP_IMAGE_B_Mode);//andy A1.4 20100113
           if(flag==UP)
           {
              if(val<6) val+=1;
              else val=0;
           }            
           else 
           {
              if(val>0) val-=1;
              else val=6;
           }
           WriteEEP(EEP_IMAGE_B_Mode,val);   
           break;
			
	case IMAGE_C_Choice:					 //左右轉顯示畫面設定(0=SINGLE , 1=DUAL , 2=TRIPLE)
		  val = ReadEEP(EEP_IMAGE_C_Mode);//andy A1.4 20100113
		  if(flag==UP)
		  {
			 if(val<6) val+=1;
			 else val=0;
		  } 		   
		  else 
		  {
			 if(val>0) val-=1;
			 else val=6;
		  }
		  WriteEEP(EEP_IMAGE_C_Mode,val);	 
		  break;
#endif

	case REARChoice:
        val = ReadEEP(EEP_RearMode);
        if(flag==UP)
        {
        #ifdef HTRV  //Pinchi 20150825 add
            if(val < 5)
		#else
            if(val<9)
		#endif
				val+=1;
            else
				val=0;
        }            
        else 
        {
            if(val>0)
				val-=1;
            else
            {
            #ifdef HTRV  //Pinchi 20150825 add
				val = 5;
			#else
				val=9;
			#endif
            }
         }
	    WriteEEP(EEP_RearMode,val);  
	    break;

	case TriggerVGA: 					 //CAMR Mirror開關設定					   
		   val = ReadEEP(EEP_TriggerVGA); 									 
		   if(flag==UP) 									  
			 val = (!val ? 1 : 0);			
		   else 
			 val = (val ? 0 : 1);		 
		   WriteEEP(EEP_TriggerVGA,val);   
		 			 
		   break;

//#ifndef  AWT_ML072Q  //Pinchi 20140918 add for AWT ML072Q
#ifdef  SUPPORT_ML84TQ_TRIGGER_DELAY  //Pinchi 20150825 add for function define
  	case DELAYCAMA:
	val = ReadEEP(EEP_DELAYCAMA); 	
	  if(flag==UP)
		{
		 if(val<3) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=3;
		}	
	WriteEEP(EEP_DELAYCAMA,val);  ;
	  break;

	 case DELAYCAMB:
	val = ReadEEP(EEP_DELAYCAMB); 	
	  if(flag==UP)
		{
		 if(val<3) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=3;
		}	
	WriteEEP(EEP_DELAYCAMB,val);  ;
	  break; 

	 case DELAYCAMC:
	val = ReadEEP(EEP_DELAYCAMC); 	
	  if(flag==UP)
		{
		 if(val<3) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=3;
		}	
	WriteEEP(EEP_DELAYCAMC,val);  ;
	  break; 	
	  
	  case DELAYCAMR:
	val = ReadEEP(EEP_DELAYCAMR); 	
	  if(flag==UP)
		{
		 if(val<3) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=3;
		}	
	WriteEEP(EEP_DELAYCAMR,val);  ;
	  break;

  #ifdef USE_CAMD
	  case DELAYCAMD:
	val = ReadEEP(EEP_DELAYCAMD); 	
	  if(flag==UP)
		{
		 if(val<3) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=3;
		}	
	WriteEEP(EEP_DELAYCAMD,val);  ;
	  break; 
  #endif
#endif

#ifdef QUAD
	case JUMPChoice:
		val=ReadEEP(EEP_JUMPMode);
		if(flag==UP)
		{
		#ifdef HTRV  //Pinchi 20150825 add  //#ifdef USE_CAMD
		   if(val<14) val+=1;
		#else   
		   if(val<13) val+=1;
		#endif
		   else val=0;
		}			
		else 
		{
		   if(val>0) val-=1;
		#ifdef HTRV  //Pinchi 20150825 add  //#ifdef USE_CAMD   
		   else val=14;  
		#else  
		   else val=13;  
		#endif	
		}	
		
		WriteEEP( EEP_JUMPMode,val);  
		//if(val==SEQ && NowModeState==JumpMode)
		if(val==0x01 && NowModeState==JumpMode)//if JUMP mode=SEQ
		{
			SEQIndex=0;
			TW2835Cmd("\n\rmode 2\n\r");//set 
			SEQMode();
			SEQFlag=1;
			SEQTime=0;
		}
		else if(NowModeState==JumpMode)///ryan@20170921
			{
			SetJumpMode();
			SEQFlag=0;//clear SEQ mode
			}	
		break;

	case CAMOUTChoice:  //Pinchi 20150825 move to QUAD #define
        val = ReadEEP(EEP_CAMOutMode);  
		CAMOutDSel( val );             //設定CAMOut動作  	    //mask by ryan@20170922
        if(flag==UP)
        {
           if(val<4) val+=1;
             else val=0;
        }           
        else 
        {
           if(val>0) val-=1;
             else val=4;
        }                                                    
        WriteEEP(EEP_CAMOutMode,val);      
        CAMOutDSel( val );             //設定CAMOut動作   
	    break;
#else
	case JUMPChoice:
		val=ReadEEP(EEP_JUMPMode);
		if(flag==UP)
		{
		   if(val<3) val+=1;
		   else val=0;
		}			
		else 
		{
		   if(val>0) val-=1;
		   else val=3;  
		}	
		
		WriteEEP( EEP_JUMPMode,val);  
		#if 0
		if(val==SEQ && NowModeState==JumpMode)
		{
			SEQIndex=0;
			SEQMode();
			SEQFlag=1;
			SEQTime=0;
		}
		#endif

	  
		break;
#endif

	case STIME_STEP:
        val=ReadEEP(EEP_TimeStep);
        if(flag==UP)
          {
                   if(val<100) val+=1;
				#ifdef TW8816
				  else val=2;
				  #endif   
          }           
          else 
          {
                 if(val>2) val-=1;
				 #ifdef TW8816
				  else val=100;
				  #endif
          }     
		  
          WriteEEP(EEP_TimeStep,val); 
		  TimeStep=val;  
	    break;

#ifdef AWT_ML072S	//william @20130902	v1.1.1.2

	   case JUMP_AV://2009.4.15 Andy
              val =ReadEEP(EEP_JUMPAV_TIMESET);

			  if(flag==UP)                                       
                val = (!val ? 1 : 0);                            
              else                                 
                val = (val ? 0 : 1);   
			 

		   WriteEEP(EEP_JUMPAV_TIMESET,val);
		   JUMPAVFlag=val;
	       break;
		   
#endif

#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
	case SDCardDetect:
	   val = ReadEEP(EEP_SDCardDetect); 									 
	   if(flag==UP) 									  
		 val = (!val ? 1 : 0);			
	   else 
		 val = (val ? 0 : 1);		 

	   SDCardCoverDetect=val;
	   WriteEEP(EEP_SDCardDetect,val);   	 
	   if(SDCardCoverDetect==1)	//Door det OFF
	   	Hs_DvrTxCmdPaser(DoorClose);

	    break;

	case Resolution:
        val=DVR_Resolution;
        if(flag==UP)									  
		  {
		   if(val<2) val+=1;
			 else val=1;
		  } 		  
		  else 
		  {
		   if(val>1) val-=1;
			 else val=2;
		  }  	 
		 DVR_Resolution=val; 
		 DVR_DataChange=1;
	    break;
		
	case FPS:
		val = DVR_FPS;			
		if(flag==UP)
		  {
		   if(val<6) val+=1;
			 else val=1;
		  } 		  
		  else 
		  {
		   if(val>1) val-=1;
			 else val=6;
		  } 												   
		  DVR_FPS=val; 	  
		  DVR_DataChange=1;
	  break;
	  
   case Quality:
 	  	val = DVR_Quality;			
 		if(flag==UP)
 		  {
 		   if(val<5) val+=1;
 			 else val=1;
 		  } 		  
 		  else 
 		  {
 		   if(val>1) val-=1;
 			 else val=5;
 		  } 												   
 		  DVR_Quality=val;
		  DVR_DataChange=1;
 	  break;
	  
  case RecordMode:
	  val =DVR_RecordMode;		  
	  if(flag==UP)
		{
		 if(val<3) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=3;
		}													 
		DVR_RecordMode=val;
		DVR_DataChange=1;
	  break;
	  
  case RecordLength:
	  val = DVR_RecordLength;		  
	  if(flag==UP)
		{
		 if(val<6) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=6;
		}													 
		DVR_RecordLength=val;
		DVR_DataChange=1;
	  break;
	  
  case PreAlarmLength:
	  val = DVR_PreAlarmLength;		  
	  if(flag==UP)
		{
		 if(val<5) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=5;
		}													 
		DVR_PreAlarmLength=val;
		DVR_DataChange=1;
	  break;
	  
  case PostAlarmLength:
	  val = DVR_PostAlarmLength;		  
	  if(flag==UP)
		{
		 if(val<3) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=3;
		}													 
		DVR_PostAlarmLength=val;
		DVR_DataChange=1;
	  break;

  case AudioRecord:
	  val = DVR_AudioRecord;		  
	  if(flag==UP)
		{
		 if(val<3) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=3;
		}	
		
	DVR_AudioRecord=val;
	DVRAodioSel(DVR_AudioRecord);
	WriteEEP(EEP_AudioRecord,val); 	
	
	  break;
	  
  case Year:
	  val =year2;		  
	  if(flag==UP)
		{
		 if(val<99) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=99;
		}	
		year2=val;
		DVR_DataChange=1;
	  break;
	  
  case Month:
	  val =month;		  
	  if(flag==UP)
		{
		 if(val<12) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=12;
		}	
		month=val;
		DVR_DataChange=1;
	  break;
	  
  case Day:
	  val =day;		  
	  if(flag==UP)
		{
		 if(val<31) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=31;
		}	
		day=val;
		DVR_DataChange=1;
	  break; 
	  
  case Hour:
	  val =hour;		  
	  if(flag==UP)
		{
		 if(val<23) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=23;
		}	
		hour=val;
		DVR_DataChange=1;
	  break;
	  
  case Minute:
	  val = minute;
	  if(flag==UP)
		{
		 if(val<59) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=59;
		}	
		minute=val;
		DVR_DataChange=1;
	  break;
	  
  case TimeZone:
	  val = DVR_TimeZone;
	  if(flag==UP)
		{
		 if(val<25) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=25;
		}	
		DVR_TimeZone=val;
		DVR_DataChange=1;
	  break;
  
  case DaylightSaving:
	  val = DVR_daylightsaving; 									  
        if(flag==UP)									  
		  {
		   if(val<2) val+=1;
			 else val=1;
		  } 		  
		  else 
		  {
		   if(val>1) val-=1;
			 else val=2;
		  }  
	 DVR_daylightsaving = val; 
	 DVR_DataChange=1;
	  break;
	  
  case TimeBeginesMonth:
	  val = DVR_timebeginesmonth;
	  if(flag==UP)
		{
		 if(val<12) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=12;
		}
	  DVR_timebeginesmonth=val;
	  DVR_DataChange=1;
	  break;
	  
  case TimeBeginesWeek:
	  val = DVR_timebeginesweek;
	  if(flag==UP)
		{
		 if(val<4) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=4;
		}
	  DVR_timebeginesweek = val;
	  DVR_DataChange=1;
	  break;
  case TimeBeginesTime:
	  val = DVR_timebeginesTime;
	  if(flag==UP)
		{
		 if(val<23) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=23;
		}
	  DVR_timebeginesTime = val;
	  DVR_DataChange=1;
	  break;
  case StandardTimeBeginesMonth:
	  val = DVR_sandardtimebeginesMonth;
	  if(flag==UP)
		{
		 if(val<12) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=12;
		}
	  DVR_sandardtimebeginesMonth=val;	
	  DVR_DataChange=1;
	  break;		  
  case StandardTimeBeginesWeek:
	  val = DVR_standardtimebeginesWeek;
	  if(flag==UP)
		{
		 if(val<4) val+=1;
		   else val=1;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=4;
		}
	  DVR_standardtimebeginesWeek = val;
	  DVR_DataChange=1;
	  break;
  case StandardTimeBeginesTime:
	  val = DVR_standardtimebeginesTime;
	  if(flag==UP)
		{
		 if(val<23) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>1) val-=1;
		   else val=23;
		}
	  DVR_standardtimebeginesTime = val;
	  DVR_DataChange=1;
	  break;

	  case FirstNumber:
		  val = PW_FirstNumber;
		  if(flag==UP)
			{
			 if(val<9) val+=1;
			   else val=0;
			}			
			else 
			{
			 if(val>0) val-=1;
			   else val=9;
			}
		  PW_FirstNumber = val;
		  break;

	case SecondNumber:
		val = PW_SecondNumber;
		if(flag==UP)
		  {
		   if(val<9) val+=1;
			 else val=0;
		  } 		  
		  else 
		  {
		   if(val>0) val-=1;
			 else val=9;
		  }
		PW_SecondNumber = val;
		break;

	case ThridNumber:
	  val = PW_ThridNumber;
	  if(flag==UP)
		{
		 if(val<9) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=9;
		}
	  PW_ThridNumber = val;
	  break;
	  
  case FourthNumber:
	  val = PW_FourthNumber;
	  if(flag==UP)
		{
		 if(val<9) val+=1;
		   else val=0;
		}			
		else 
		{
		 if(val>0) val-=1;
		   else val=9;
		}
	  PW_FourthNumber = val;
	  break;
#endif

    case CAMNumberChoice:                    //TM0350-980227          
              val=ReadEEP(EEP_CAMNUMBER);
              if(flag==UP)
              {
                 if(val<255) val+=1;  
                 else val=0;
              }           
              else 
              {
                 if(val>0) val-=1;
                 else val=255;
              }    
			  CAMNumber=val;
              WriteEEP(EEP_CAMNUMBER,val);      
              break;  
	
       case PelcoChoice:                   		//TM0350-980227              
             val = ReadEEP(EEP_PELCO);                                                     
             if(flag==UP)                                                        
               val = (!val ? 1 : 0);                                             
             else                                              
               val = (val ? 0 : 1);
			 PelcoChoiceFLAG=val;
             WriteEEP(EEP_PELCO,val);  		 
             break;  	

       case Baud_rateChoice:                    //TM0350-980227          
              val=ReadEEP(EEP_Baud_rate);
              if(flag==UP)
              {
                 if(val<2) val+=1;  
                 else val=0;
              }           
              else 
              {
                 if(val>0) val-=1;
                 else val=2;
              }    
			  Baud_rate=val;
              WriteEEP(EEP_Baud_rate,val);      
              break; 

  case OverWrite:
	  val = DVR_OverWrite; 									  
        if(flag==UP)									  
		  {
		   if(val<2) val+=1;
			 else val=1;
		  } 		  
		  else 
		  {
		   if(val>1) val-=1;
			 else val=2;
		  }  
	 DVR_OverWrite = val; 
	 DVR_DataChange=1;
	  break;	
    #ifdef AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150209 add
       case TRIGGER_CamADelay :
           val = ReadEEP(EEP_CamADelay);  //0~3
           if (flag == UP)
           {
               if (val < 3)
                   val += 1;
               else
                   val = 3;
           }
           else 
           {
               if (val > 0)
                   val -= 1;
               else
                   val = 0;
           }
           WriteEEP(EEP_CamADelay , val);
           break;

       case TRIGGER_CamBDelay :
           val = ReadEEP(EEP_CamBDelay);  //0~3
           if (flag == UP)
           {
               if (val < 3)
                   val += 1;
               else
                   val = 3;
           }
           else 
           {
               if (val > 0)
                   val -= 1;
               else
                   val = 0;
           }
           WriteEEP(EEP_CamBDelay , val);
           break;

       case TRIGGER_CamCDelay :
           val = ReadEEP(EEP_CamCDelay);  //0~3
           if (flag == UP)
           {
               if (val < 3)
                   val += 1;
               else
                   val = 3;
           }
           else 
           {
               if (val > 0)
                   val -= 1;
               else
                   val = 0;
           }
           WriteEEP(EEP_CamCDelay , val);
           break;

       case TRIGGER_CamRDelay :
           val = ReadEEP(EEP_CamRDelay);  //0~3
           if (flag == UP)
           {
               if (val < 3)
                   val += 1;
               else
                   val = 3;
           }
           else 
           {
               if (val > 0)
                   val -= 1;
               else
                   val = 0;
           }
           WriteEEP(EEP_CamRDelay , val);
           break;
    #endif
		
	}

	return val;
}


void DisplayViewBar(WORD addr, BYTE val)
{
#if 1//ryan@20170919

CODE BYTE BAR_ROM_TABLE[] = {
		0x01,0x01,0x01,0x01,0x01,0x01,0x01, // ...
		0x02,0x02,0x02,0x02,0x02,0x02,0x02, // |..
		0x05,0x05,0x05,0x05,0x05,0x05,// ||.
		///0x03, // |
		0x04, // |||
	};
	
#else
	CODE BYTE BAR_ROM_TABLE[] = {
		0x01,0x01,0x01,0x01, // ...
		0x02,0x02,0x02,0x02,0x02, // |..
		0x05,0x05,0x05,0x05,0x05,// ||.
		0x05, // ||.
		///0x03, // |
		0x04, // |||
	};
#endif
	BYTE i=0;
	//BYTE Bar_str[]={ 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, ' ', ' ', ' ',0 };
	BYTE Bar_str[]={  0x04, 0x04, 0x04, 0x04, 0x04, 0x03, ' ', ' ', ' ',0 };

#if 1//ryan@20170821
 // Bar
 	#if 1//ryan@20170919
	
    if(val < 100)
    {
        Bar_str[val/20] = BAR_ROM_TABLE[val%20];

		for(i = (val/20)+1 ; i < 5; i++)
             Bar_str[i] = BAR_ROM_TABLE[0];
		
    }


    if( Bar_str[0] == 0x01 ) 
      Bar_str[0] = 0x02; // |..

    // Number
    Bar_str[8] = val % 10 + '0';    val /= 10;  //個位數
    Bar_str[7] =  val % 10 + '0';    val /= 10;  //十位數
    Bar_str[6] = val + '0';                      //百位數
    if( Bar_str[6] == '0' ) Bar_str[6] = ' ';

/*
    if(val < 96)
    {
        Bar_str[val/16] = BAR_ROM_TABLE[val%16];

		for(i = (val/16) + 1; i < 6; i++)
             Bar_str[i] = BAR_ROM_TABLE[0];
		
    }
	
    if( Bar_str[0] == 0x01 ) 
      Bar_str[0] = 0x02; // |..

    // Number
    Bar_str[9] = val % 10 + '0';    val /= 10;  //個位數
    Bar_str[8] =  val % 10 + '0';    val /= 10;  //十位數
    Bar_str[7] = val + '0';                      //百位數
    if( Bar_str[7] == '0' ) Bar_str[7] = ' ';

*/
	#else
    if(val < 98)
    {
        Bar_str[val/14] = BAR_ROM_TABLE[val%14];
        for(i = val/14 + 1; i < 7; i++)
            Bar_str[i] = BAR_ROM_TABLE[0];
    }
	
    if( Bar_str[0] == 0x01 ) 
      Bar_str[0] = 0x02; // |..

    // Number
    Bar_str[10] = val % 10 + '0';    val /= 10;  //個位數
    Bar_str[9] =  val % 10 + '0';    val /= 10;  //十位數
    Bar_str[8] = val + '0';                      //百位數
    if( Bar_str[8] == '0' ) Bar_str[8] = ' ';
	#endif

#else
	// Bar
 	if( BAR_LENGTH != 0 ) {
		if(val < 98)
		{
			Bar_str[val/14] = BAR_ROM_TABLE[val%14];
			for(i = val/14 + 1; i < 7; i++)
				Bar_str[i] = BAR_ROM_TABLE[0];
		}

		if( Bar_str[0] == 0x01 ) Bar_str[0] = 0x02;	// |..
	}

 	// Number
    Bar_str[i+2] = val % 10 + '0';    val /= 10;
    Bar_str[i+1] =  val % 10 + '0';    val /= 10;
    Bar_str[i] = val + '0';
	if( Bar_str[i] == '0' ) Bar_str[i] = ' ';

	for(;i<7;i++)
		Bar_str[i+3] = ' ';
#endif
	WriteStringToAddr(addr, Bar_str, BARNUM_LENGTH); 

}

#ifdef AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150209 add
/*-------------------------------------*/
/*-------------------------------------*/ 
void AWT_DisplayViewBar(WORD addr , BYTE val)
{

#ifdef TW8816
   BYTE i;         //||||||||百十個 
   BYTE Bar_str[]={ 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, ' ', ' ', ' ',0 }; 
    // Bar
    if (val < 3)
    {
        switch(val)
        {
            case 0 :
                Bar_str[0] = 0x02;  // |..
                i = 1;
                break;

            case 1 :
                Bar_str[1] = 0x04;  // |||
                i = 2;
                break;

            case 2 :
                Bar_str[3] = 0x05 ;//||.
                i = 4;
                break;

             default :
                break;
        }

        for ( ; i <5 ; i++)
            Bar_str[i] = 0x01;  // ...
    }

    // Number
    Bar_str[8] = val + '0';   //個位數
    Bar_str[7] = '0';   //十位數

#else
    BYTE i;         //||||||||百十個 
    BYTE Bar_str[]={ 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, ' ', ' ', ' ',0 }; 
    // Bar
    if (val < 3)
    {
        switch(val)
        {
            case 0 :
                Bar_str[0] = 0x02;  // |..
                i = 1;
                break;

            case 1 :
                Bar_str[2] = 0x05;  // ||.
                i = 3;
                break;

            case 2 :
                Bar_str[4] = 0x05;
                i = 5;
                break;

             default :
                break;
        }

        for ( ; i < 7 ; i++)
            Bar_str[i] = 0x01;  // ...
    }

    // Number
    Bar_str[10] = val + '0';   //個位數
    Bar_str[9] = '0';   //十位數

    #endif

    WriteStringToAddr(addr , Bar_str , BARNUM_LENGTH); 
}
#endif
void DrawNum(WORD addr,CODE_P struct NumType *nptr, WORD val)
{
	BYTE x,i, j,k, len, mstr[5];//,color;
	BYTE Num_str[]={ ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',0 };
	CODE_P BYTE *str;
	
	x=  MAX_DESC;//+ICON_CH_SZ;
	i =  nptr->PrePostCharLen >> 4 ;
	j =  nptr->PrePostCharLen & 0x0f ;
	str = nptr->PrePostChar;

	// Draw char on front of number. 
	if( i ) 
		for(k=0; k<i; k++)
			Num_str[k] = *str++;
	// Draw number.
	utoa(val,mstr,BAR_NUM_RADIX);
	len = strlen(mstr);
	str=mstr;
	for(k=0; k<len; k++)
		Num_str[k+i] = *str++;

	// Draw char on back of number
	if( j ) 
		for(k=0; k<j; k++)
			Num_str[k+i+len] = *str++;
	WriteStringToAddr(addr, Num_str, BARNUM_LENGTH); 
	
}

void DrawStringByLang( WORD addr, CODE_P struct EnumType *yptr, WORD val )
{

	BYTE j,l,Size_val;
	BYTE Buf[]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',0 };
	CODE_P BYTE *Str;

	Size_val = yptr->Sz;
	Str = yptr->Str;

	if ( yptr->ByLang)
	{
		for(j=0; j<OSDLang*Size_val; j++)			
		Str += ( strlen(Str) + 1 );
	}

	if( val ){ 
		for(j=0; j<val; j++)			
		Str += ( strlen(Str) + 1 );	
	}

	WriteStringToAddr(addr, Buf, BARNUM_LENGTH); 

	l = CStrlen(Str);

	WriteStringToAddr(addr, Str, l); 

}
/*
#ifdef PAL_TV
void DrawFraction( CODE_P struct NumType *nptr, DWORD val, BYTE fp )
{
	BYTE mstr[6], len;

	ltoa_K( val, mstr, fp );
	len = strlen(mstr);
	CopyStrToOSDWindow(cur_osdwin, 0, 0, len, nptr->NumColor, mstr);
	ClearOSDLine(cur_osdwin, len, 0, nptr->width-len, nptr->NumColor);
}
#endif

BYTE OnChangingValue(void)
{
	return ( (val_osdwin==0xff  ) ? 0 : 1 );	//OSDItemValue==0xffff: password error
}
// Not used yet *******************************************
BYTE OnChangingTime(void)
{
	if ( val_osdwin==0xff )						return 0 ;
	if ( MenuChild[cursor_item].Type==TIME )	return 1;
	else										return 0;

}
*/

void DrawMenulist(BYTE NOTViewflag)
{
	BYTE  i,j,k, x=0, y=0;
	CODE_P struct DefineMenu *DMp;
	CODE_P BYTE *Desc;
	BYTE  Max_y;
	WORD addr;
#ifdef HS_NEWOSDMENU_2  //Weylis -@121008
	CODE_P BYTE *Str;
#endif

	// Draw Title line
	if( MenuFormat->TitleColor!=NIL ) {

		#ifdef DEBUG_OSD
		dPrintf("\r\nTitle color:0x%x", (WORD)MenuFormat->TitleColor);
		#endif
 
		DMp = &MenuTitle[title_item];
		Desc = DMp->Desc[OSDLang];
		j=strlen(Desc);

		for( i=0,k=0; i<j; i++ )
			if( *(Desc+i) == ROMFONT || *(Desc+i) == RAMFONT ) k++;

		//ClearDataCnt(OSDMENU_SUBADDR, MenuFormat->width);	// Clear the counter of OSD RAM Data from addr with space(' ').
		#ifdef HS_NEWOSDMENU_2 
		WriteStringToAddr((OSDMENU_SUBADDR)+1, Desc, j);   //Weylis show setup Page(Desc *)
		#else
		DrawAttrCnt(OSDMENU_SUBADDR, MenuFormat->TitleColor, MenuFormat->width );	 
		WriteStringToAddr(OSDMENU_SUBADDR, Desc, j);
		#endif
		
	   	#ifdef HS_NEWOSDMENU_2
		if( (j-k ) < TITLE_MAX_DESC )
	   		ClearDataCnt((OSDMENU_SUBADDR+j-k)+1, (TITLE_MAX_DESC-j+k)-1);
	   	#else
			ClearDataCnt(OSDMENU_SUBADDR+j-k, TITLE_MAX_DESC-j+k);
	   	#endif

		y++;
	}
	
	if( MenuChild )	i = 0;
	else			i = NIL;

	// Draw
	#ifdef HS_NEWOSDMENU
     Max_y = (cur_osdwin == OSDMENU_TOPWIN ? TOTAL_TOP_HEIGHT: MENU_HEIGHT+1);  //Weylis -@121008
	#else 
	 Max_y = (cur_osdwin == OSDMENU_TOPWIN ? TOTAL_TOP_HEIGHT: MENU_HEIGHT+1);
	#endif
	 addr = (cur_osdwin == OSDMENU_TOPWIN ? OSDMENU_MAINADDR: OSDMENU_SUBADDR);
	 addr += y*MenuFormat->width;

	 if( NOTViewflag==0 ) {
		 I2CAutoIncFlagOn = 1;	// speedy submenu drawing 
		 WriteDecoder(TW88_ENABLERAM, 0x04); // OSD RAM Auto Access Enable
	 }

	 for(  ; y < Max_y; y++)	{

		if(i != NIL) {
			DisplayOSDMenuOneItem( cur_osdwin, &MenuChild[i], x, y, NOTViewflag);
			i = GetLowerMenuItem( MenuChild, i );
		}
		else {
			#ifdef HS_NEWOSDMENU
				ClearDataCnt(addr+HS_CHSPACE, MenuFormat->width);
			#else
				ClearDataCnt(addr, MenuFormat->width);	// Clear the counter of OSD RAM Data from addr with space(' ').
			#endif
		}
		addr += MenuFormat->width;
	 }

 	 if( NOTViewflag==0 ) {
		 I2CAutoIncFlagOn = 0;	// Recover  
		 WriteDecoder(TW88_ENABLERAM, 0x00); // OSD RAM Auto Access Enable
	 }

#ifdef HS_NEWOSDMENU_2	//Weylis -@121008
	//DrawAttrCnt(OSDMENU_SUBADDR+TITLE_MAX_DESC*8, COMMENT1_COLOR, TITLE_MAX_DESC );  // All Line Attr clear by default color.
	DrawAttrCnt(OSDMENU_SUBADDR+TITLE_MAX_DESC*(MENU_HEIGHT - 1), COMMENT1_COLOR, TITLE_MAX_DESC );	// All Line Attr clear by default color.
	Str = Str_Comments[OSDLang];
	//  i=strlen(Str);
	WriteStringToAddr(OSDMENU_SUBADDR+TITLE_MAX_DESC*(MENU_HEIGHT - 1), Str, strlen(Str));
#endif
}

//
// Here, Main Menu will display Only Icon at leftside.
//
#define ALLDATA  175 //25*7  MENU_WIDTH*MENU_HEIGT
void DisplayOSDMenu(void)
{
#ifndef HS_NEWOSDMENU_2
  #if 0  //Pinchi disable for compiler warning
	CODE_P BYTE *Str;
	BYTE i;
  #endif
#endif	
	#ifdef DEBUG_OSD
	dPrintf("\r\n++(DisplayOSDMenu)");
	#endif

	// Clear all MENU Window Ram data
#ifdef HS_NEWOSDMENU
	ClearDataCnt(OSDMENU_MAINADDR+(HS_WIDTH * 2), MENU_WIDTH*(MENU_HEIGHT-1));	// Clear the counter of OSD RAM Data from addr with space(' ').
	DrawAttrCnt(OSDMENU_MAINADDR, DEFAULT_COLOR, 13 );	 // All Line Attr clear by default color. //Weylis 081111 for icon
	DrawAttrCnt(OSDMENU_SUBADDR, TITLE_COLOR, TITLE_MAX_DESC );	 // All Line Attr clear by default color.
#else
DrawAttrCnt(OSDMENU_MAINADDR, DEFAULT_COLOR, 5*2 );	 // All Line Attr clear by default color.
//ryan@20170907		DrawAttrCnt(OSDMENU_SUBADDR, TITLE_COLOR, TITLE_MAX_DESC ); // All Line Attr clear by default color.

#endif
	if( MenuChild[ cursor_item ].Id == TVINSTALL && GetInputSelection()!= TV )
		DrawAttrCnt(OSDMENU_SUBADDR+TITLE_MAX_DESC, CH_COLOR_MAGENTA|BACK_COLOR, MENU_HEIGHT*MENU_WIDTH );	
	else
	{
		if( (MenuChild[ cursor_item ].Id == PC_COLOR_MODE && GetPCColorTempModeEE() != 3 )  // User
		||( MenuChild[ cursor_item ].Id == VIDEOPICTURE && GetVideoModeEE() != 3 ) ) // User
		{
			DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH, DEFAULT_COLOR, MENU_WIDTH );
			DrawAttrCnt(OSDMENU_SUBADDR+MENU_WIDTH*2, BACK_COLOR, (MENU_HEIGHT-1)*MENU_WIDTH );	
		}
		else
			DrawAttrCnt(OSDMENU_SUBADDR+TITLE_MAX_DESC, DEFAULT_COLOR, MENU_HEIGHT*MENU_WIDTH );
	}

#ifdef TW8816
#else
DrawAttrCnt(OSDMENU_SUBADDR+TITLE_MAX_DESC*8, COMMENT_COLOR, TITLE_MAX_DESC );	 // All Line Attr clear by default color.
#endif

	DrawMenulist(1);

	// Draw Child
	if( OSDMenuLevel == 1 ) // 
	{
		pushMenu();	// Store ManuTile, tile_item.
		MenuTitle = MenuChild;
		title_item = cursor_item;
		
		MenuFormat = ( CODE_P struct MenuType *) MenuTitle[title_item].TypeData;
		MenuChild = MenuTitle[title_item].Child;
		cur_osdwin--; 

		DrawMenulist(0);	// draw total Submemu.
		
		popMenu(); // Restore ManuTile, tile_item.
		MenuFormat = ( CODE_P struct MenuType *) MenuTitle[title_item].TypeData;
		MenuChild = MenuTitle[title_item].Child;
		cur_osdwin++;
	}
#if 0			//william-AWT-ML082Q-v1.2-20100315
	Str = Str_Comments[OSDLang];
	i=strlen(Str);
	WriteStringToAddr(OSDMENU_SUBADDR+TITLE_MAX_DESC*8, Str, i);
#endif
	DisplayCursor();
}

void OSDHighMenu_sub(void)
{
	BYTE temp_cursor;

	//#ifdef DEBUG_OSD
	dPuts("\r\n(OSDHighMenu_sub)");
	//#endif
	temp_cursor = cursor_item ;
	MenuChild = MenuTitle;
	cursor_item = title_item;

	if( !popMenu() ) {
		CloseOSDMenu();

		MenuTitle = MenuChild;	//  Restore the Menu status.
		title_item = cursor_item;
		MenuChild = MenuTitle[title_item].Child;
		cursor_item = temp_cursor;
	
		//DoAction(EXITMENU);
		#if 0		//william-981029
		if( DisplayInputHold ) DisplayInput();
		else if( IsAudioMuteOn() ) DisplayMuteInfo();
		return;
		#endif
	}

	//#ifdef DEBUG_OSD
	Printf("\r\n++(OSDHighMenu_sub) OSDMenuLevel:%d", (WORD)OSDMenuLevel );
	//#endif
	
	MenuFormat = ( CODE_P struct MenuType *) MenuTitle[title_item].TypeData;

	OSDMenuLevel--; // Up Menu tree

	if(OSDMenuLevel == 1){	// Top Menu Level
		cur_osdwin = OSDMENU_TOPWIN;//Window #4
		DisplayOSDMenu();
	}
	else
	{
		DrawMenulist(0); // Redraw List and contents.
		DisplayCursor();
	}
	
	// 3D Effect Cursor Top
	if(OSDMenuLevel == 1) {
		WriteDecoder(TW88_WINNUM, OSDCURSORWINDOW-1);
		WriteDecoder(TW88_WINATTR, OSD_Window_Attr(WINDOW_3D_TOP,BG_COLOR_BLACK|EXTENDED_COLOR,0,WINDOW_3D_ENABLE,WINDOW_ENABLE));	
	}

	return;
}


BYTE DoAction(BYTE id)
{
	BYTE ret = 1;

	#ifdef DEBUG_OSD
	dPrintf("\r\n++(DoAction) id:0x%x", (WORD)id );
	#endif
	switch( id ) {

	case CHANGE_TO_COMPOSITE:
	case CHANGE_TO_SVIDEO:
	case CHANGE_TO_COMPONENT:

	#ifdef SUPPORT_PC
	case CHANGE_TO_PC:
	case CHANGE_TO_DIGITALVGA:
	#endif

	#ifdef SUPPORT_DTV
	case CHANGE_TO_DTV:
	#endif

								ChangeInput( id & 0x0f );			
								break;
							
	#ifdef SUPPORT_PC
	case AUTOADJUST:			
								AutoAdjust();
								/*ClearOSDInfo();
								DisplayAutoAdjust();
								ret = DetectAndSetForVGAInput(0);	
								ClearAutoAdjust();
								LCDPowerON(0);	//HHY 1.63*/
								break;
	/*case AUTOADJUST2:			ret = DetectAndSetForVGAInput(2);
								LCDPowerON(0);	//HHY 1.63
								SetOSDLastKeyInTime();
								ClearCursor();
								cursor_item = 0;
								DisplayCursor();		
								break;*/
	case RESET_PCVALUE:			ClearOSDInfo();
								ResetPanelAttributeValue();
								//ResetPCValue();		william-v1.1-20100204
								ResetOSDValue();
								//ResetAudioValue();
								SetNowMode();
							#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
								Hs_DvrTxCmdPaser(SetDVRDefault);
							#endif
								DisplayInput();

								#ifdef AWT_NO_SIGNAL_MSG  //Pinchi 20150320 add
								IsDrawNoVideoSignal = 0;
								#endif
								
								break;

	case PCCOLORADJUST:			
								AutoColorAdjust();
								break;
	#endif
	
	
	case RESET_VIDEOVALUE:		ClearOSDInfo();
							#ifdef SUPPORT_PC
								ResetPanelAttributeValue();
							#endif
								ResetVideoValue();
								ResetOSDValue();
								//ResetAudioValue();
								SetNowMode();
							#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
								Hs_DvrTxCmdPaser(SetDVRDefault);
							#endif
								DisplayInput();

								#ifdef AWT_NO_SIGNAL_MSG  //Pinchi 20150320 add
								IsDrawNoVideoSignal = 0;
								#endif

		#if 1//def TW8816//ryan@20170919                                           
             if(ReadEEP(EEP_COSDMode)==0)                  //CAM OSD ON //william-980428
             {
                Attr_A|=0x02;
                Attr_B|=0x02;
                Attr_C|=0x02;
                Attr_R|=0x02;
				//displayhold_flag=0;
             }			 
             else                           //CAM OSD OFF 
             {
                Attr_A&=0x0d;
                Attr_B&=0x0d;
                Attr_C&=0x0d;
                Attr_R&=0x0d;
             }  

		SetNowMode();
	#endif
								
								break;

#ifdef HTRV
	 case RESET_GAUGESETUP:				//WILLIAM-V1.43-970325
	 	ClearOSDInfo();                //清除目前左上角Mode的OSD視窗 ...Abel  960122
	 	WriteEEP(EEP_Gauge_VStart,0x0C); 	//WILLIAM-V1.43-970325
	 	GaugeVStart= ReadEEP(EEP_Gauge_VStart);                 //william-v1.43-970331
	      break;
#endif

	case RESET_MonitorVALUE:
								ClearOSDInfo();
								//ResetPanelAttributeValue();
								ResetVideoValue();
								ResetOSDValue();
								//ResetAudioValue();
								SetNowMode();
								//Hs_DvrTxCmdPaser(SetDVRDefault);
								DisplayInput();
								Printf("\r\nRESET_MonitorVALUE",0);
								break;

#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
	case RESET_DVRVALUE:		ClearOSDInfo();
								//ResetPanelAttributeValue();
								//ResetVideoValue();
								//ResetOSDValue();
								//ResetAudioValue();
								SetNowMode();
								Hs_DvrTxCmdPaser(SetDVRDefault);
								DisplayInput();
								break;
#endif

#ifdef E_Wintek
	case RESET_USERVALUE:		ClearOSDInfo();
								ResetPanelAttributeValue();
								ResetVideoValue();
								ResetUESROSDValue();
								SetNowMode();
								DisplayInput();
								break;
#endif
//----
	case TOHIGHMENU:			OSDHighMenu_sub();						
								break;

#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
	case DVRParamApply:			if(DVR_DataChange==1)
								{
									Hs_DvrTxCmdPaser(SetDVRParam);
									DVR_DataChange=0;
								}	
								OSDHighMenu_sub();	
								break;							

	case DATETIMEApply:			if(DVR_DataChange==1)
								{
									Hs_DvrTxCmdPaser(SetDATETIME);
									DVR_DataChange=0;
								}
								OSDHighMenu_sub();								
								break;
#endif

	case DISP_INFO:
								DisplayInfo_OSD();	
								displayOnValueWindow = 1;
								break;

	case EXITMENU:				CloseOSDMenu();
							#if 1  //Pinchi 20150901 add to setup 2s for Display OFF
								if (ReadEEP(EEP_COSDMode) == 1)
									displayhold_flag = 3;
							#endif
								DisplayInput();
							#ifdef AWT_NO_SIGNAL_MSG  //Pinchi 20150320 add
								IsDrawNoVideoSignal = 0;
								#endif
					
								PW_FirstNumber=0;
								PW_SecondNumber=0;
								PW_ThridNumber=0;
								PW_FourthNumber=0;
								#if 0	//william-981029
								if( DisplayInputHold ) DisplayInput();
								else if( IsAudioMuteOn() ) DisplayMuteInfo();
								#endif
								break;
    //==========================>>>>HS andy AWT 9801001

 //#ifdef SUPPORT_SDCARD
 #if 0 //william-awt ml073q-20120313
	case ManualRecordStart:
							if(DVR_Ready==0)
								{
								//DisplayDVRnotReady();
								DisplayInformation(InfoSDCARDNotReady);
								Wait_ms(3000);
				  				ClearGAUGEINFO();
								DisplayInput();
								}
							else
								{
								//Hs_DvrTxCmdPaser(GetDATETIME);
								Hs_DvrTxCmdPaser(MANUALRECSTART);
								CloseOSDMenu();	
								DisplayInput();
								}
								break;
	case ManualRecordStop:
							if(DVR_Ready==0)
								{
								//DisplayDVRnotReady();
								DisplayInformation(InfoSDCARDNotReady);
								Wait_ms(3000);
				  				ClearGAUGEINFO();
								DisplayInput();
								}
							else
								{
								Hs_DvrTxCmdPaser(MANUALRECSTOP);
								CloseOSDMenu();	
								DisplayInput();
								}
								break;
	case Snapshot:
							if(DVR_Ready==0)
								{
								DisplayDVRnotReady();
								DisplayInformation(InfoSDCARDNotReady);
								Wait_ms(3000);
				  				ClearGAUGEINFO();
								DisplayInput();
								}
							else
								{
								Hs_DvrTxCmdPaser(SNAPSHOT);
								CloseOSDMenu();	
								DisplayInput();
								}
								break;
#endif

#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
   	case FormatSD:				
						if(DVR_SDCardAvailable!=0xff)
							{
						if((ReadEEP(EEP_FirstNumber)==PW_FirstNumber
							&&ReadEEP(EEP_SecondNumber)==PW_SecondNumber
							&&ReadEEP(EEP_ThridNumber)==PW_ThridNumber
							&&ReadEEP(EEP_FourthNumber)==PW_FourthNumber)
							||(PW_FirstNumber==0&&PW_SecondNumber==6&&PW_ThridNumber==3&&PW_FourthNumber==0))
								{
								Printf("\r\n PASSWORD OK",0);	
								Hs_DvrTxCmdPaser(SetFormatSD);
								FormatStart=1;
								CloseOSDMenu();		
								DisplayInformation(InfoFORMATSDCARD);
								//DisplayFormatOSD();

								Printf("\r\n FormatStart=%02x",(WORD)FormatStart);
								Printf("\r\n FormatWait=%02x",(WORD)FormatWait);
								Printf("\r\n FormatStatus=%02x",(WORD)FormatStatus);
								Printf("\r\n UpdateOK=%02x",(WORD)UpdateOK);
								
								while(FormatWait<=150)
									{
									if(RS2_in!=0)
										DvrReceivePaser();	
									//RS2_in=0;
									
									if(FormatStatus==1)	
										{
										if(UpdateOK==1)
											{
											DisplayInformation(InfoFORMATSUCCESS);
											//DisplayFormatOkOSD();
											//Wait_ms(3000);
				  							//ClearGAUGEINFO();
				  							PW_FirstNumber=0;
											PW_SecondNumber=0;
											PW_ThridNumber=0;
											PW_FourthNumber=0;
											}
										break;
										}			
									}
								
								if(UpdateOK==0)
									{
									DisplayInformation(InfoFORMATFAIL);
									//DisplayFormatFailOSD();
									//Wait_ms(3000);
									//ClearGAUGEINFO();
									}
								Printf("\r\n Clear FormatStatus",0);
								FormatStart=0;
								FormatWait=0;
								FormatStatus=0;
								UpdateOK=0;
								PW_FirstNumber=0;
								PW_SecondNumber=0;
								PW_ThridNumber=0;
								PW_FourthNumber=0;
								//DisplayInput();
						}
						else
							{
							DisplayInformation(InfoPASSWORDERROR);
							PW_FirstNumber=0;
							PW_SecondNumber=0;
							PW_ThridNumber=0;
							PW_FourthNumber=0;
							//DisplayFormatFailOSD();
							//Wait_ms(3000);
							//DisplayInput();

							}
			
							}	
								break;	

	case FirmwareUpdate:		
						if(DVR_SDCardAvailable!=0xff)
							{
								Hs_DvrTxCmdPaser(FW_Update_Start);
								FormatStart=1;
								CloseOSDMenu(); 
								
								DisplayInformation(InfoFWUpdate);
								//DisplayFWUpDate();
								
								while(FormatWait<=250)
									{
									if(RS2_in!=0)
										DvrReceivePaser();	
									//RS2_in=0;
									
									if(FormatStatus==1) 
										{
										if(UpdateOK==1)
											{
											//DisplayFWUpDateOK();
											DisplayInformation(InfoFWUpdateSUCCESS);
											//Wait_ms(3000);
											//ClearGAUGEINFO();
											}
										
										break;
										}			
									}
								
								if(UpdateOK==0)
									{
									//DisplayFWUpDateFail();
									DisplayInformation(InfoFWUpdateFAIL);
									//Wait_ms(3000);
									//ClearGAUGEINFO();
									}
								Printf("\r\n Clear FormatStatus",0);	
								FormatStart=0;
								FormatWait=0;
								FormatStatus=0;
								UpdateOK=0;
								//DisplayInput();
							}	
								break;	

	case UpdateConfig:			
						//if(DVR_SDCardAvailable!=0xff)
							{
								Hs_DvrTxCmdPaser(Update_CONFIG);
								FormatStart=1;
								CloseOSDMenu(); 
								
	
								DisplayInformation(InfoUpdateConfigStart);
								
								while(FormatWait<=30)
									{
									if(RS2_in!=0)
										DvrReceivePaser();	
									//RS2_in=0;
									
									if(FormatStatus==1) 
										{
										if(UpdateOK==1)
											{
											DisplayInformation(InfoUpdateConfigSuccess);
											//Wait_ms(3000);
											//ClearGAUGEINFO();			
											}
											break;
										}			
									}
								
								if(UpdateOK==0)
									{
									DisplayInformation(InfoUpdateConfigFail);
									//Wait_ms(3000);
									//ClearGAUGEINFO();
									}
								Printf("\r\n Clear FormatStatus",0);
								FormatStart=0;
								FormatWait=0;
								FormatStatus=0;
								UpdateOK=0;
								//DisplayInput();
							}	
								break;

	case ChangePassword:		
								#if 0
								Printf("\r\nPW_FirstNumber=%02x",(WORD)PW_FirstNumber);
								Printf("\r\nPW_SecondNumber=%02x",(WORD)PW_SecondNumber);
								Printf("\r\nPW_ThridNumber=%02x",(WORD)PW_ThridNumber);
								Printf("\r\nPW_FourthNumber=%02x",(WORD)PW_FourthNumber);
								Printf("\r\nReadEEP(EEP_FirstNumber)=%02x",(WORD)ReadEEP(EEP_FirstNumber));
								Printf("\r\nReadEEP(EEP_FirstNumber)=%02x",(WORD)ReadEEP(EEP_SecondNumber));
								Printf("\r\nReadEEP(EEP_ThridNumber)=%02x",(WORD)ReadEEP(EEP_ThridNumber));
								Printf("\r\nReadEEP(EEP_FourthNumber)=%02x",(WORD)ReadEEP(EEP_FourthNumber));

								#endif

								WriteEEP(EEP_FirstNumber,PW_FirstNumber);
								WriteEEP(EEP_SecondNumber,PW_SecondNumber);
								WriteEEP(EEP_ThridNumber,PW_ThridNumber);
								WriteEEP(EEP_FourthNumber,PW_FourthNumber);
								PW_FirstNumber=0;
								PW_SecondNumber=0;
								PW_ThridNumber=0;
								PW_FourthNumber=0;
								
								OSDHighMenu_sub();	

								break;

	case CameraOSD:
								CloseOSDMenu();
								SpeedDomeMode=1;
								//RS232Select=1;
								Wait_ms(200);
								CAM_TxCommand_toRS485(CAM_MENU_ENTER);
								DisplayInput();

								break;
#endif		//#ifdef SUPPORT_SDCARD								
	}
	return ret;
}


BYTE GetNextCursorMenuItem( CODE_P struct DefineMenu *DMp, BYTE itemno, BYTE flag)
{
	BYTE new_cursor;

	switch( flag ) {
	case UP:	new_cursor = GetUpperMenuItem( /*DMp,*/ itemno );		break;
	case DN:	new_cursor = GetLowerMenuItem( DMp, itemno );		break;
	}
	return new_cursor;
}

//============== Public functions =================================================
//CODE BYTE TryAgainMsg[]={ "Try again."};

BYTE SaveValue1(void)
{
//	BYTE          /*ret=0,*/ addroff;

	switch( MenuChild[cursor_item].Type )	{
	
	case NUMBER:
	       	switch ( MenuChild[cursor_item].Id ) {

			//case SLEEP_TIMER:		SetSleepTimer( OSDItemValue );	
			//						return 1;	//break;
			}
			//ret = 1;
			break;
	
	default:	break;
	}

	return 0;	
}

void ClearValueWin(void)
{
	SaveValue1();
	//ShowOSDWindow(OSDBARWINDOW, FALSE);
	val_osdwin = 0xff;
	OnChangingValue = 0;
}

void OSDCursorMove(BYTE flag )
{
	BYTE new_cursor, old_cursor;
	WORD dat;
#ifdef HS_NEWOSDMENU_2
		CODE_P BYTE *Str;
		BYTE i;
#endif
		bit	OSDLANG_Changing = 0;		// Weylis for OSD changing redrawing issue

	#ifdef DEBUG_OSD
	dPrintf("\r\n++(OSDCursorMove)cursor_item:%d __", (WORD)cursor_item);
	#endif

		if(displayOnValueWindow == 1) 
			return; 
		if( MenuChild[ cursor_item ].Id == PC_COLOR_TEMP && 
			GetPCColorTempModeEE() != 3 ) 
			return; // User
		if( MenuChild[ cursor_item ].Id == VIDEO_MODE && GetVideoModeEE() != 3 ) return; // User

		new_cursor = GetNextCursorMenuItem( MenuChild, cursor_item, flag );	// Get item to move
		
		if( new_cursor ==NIL ) {		
			BYTE i;
			if( flag == UP )
				for(i=0; i<8; ){
					new_cursor=i;
					if( (i=GetLowerMenuItem(MenuChild, i) ) == NIL) break;
					
				}
				//new_cursor = MenuFormat->height - 1;
			else
				new_cursor = 0;
		}

		if( OnChangingValue )	{
			ClearValueWin();
		}
		if( OSDMenuLevel != 1) 
			ClearCursor();

	#ifdef SUPPORT_SDCARD	
	#if 0
		Printf("\r\ntitle_item=%02x",(WORD)title_item);
		Printf("\r\ncur_osdwin=%02x",(WORD)cur_osdwin);
		Printf("\r\nOSDMenuLevel=%02x",(WORD)OSDMenuLevel);
		Printf("\r\ncursor_item=%02x",(WORD)cursor_item);
		Printf("\r\nnew_cursor=%02x",(WORD)new_cursor);
	#endif
		if((ReadEEP(EEP_FirstNumber)==PW_FirstNumber
		&&ReadEEP(EEP_SecondNumber)==PW_SecondNumber
		&&ReadEEP(EEP_ThridNumber)==PW_ThridNumber
		&&ReadEEP(EEP_FourthNumber)==PW_FourthNumber)
		||(PW_FirstNumber==0&&PW_SecondNumber==6&&PW_ThridNumber==3&&PW_FourthNumber==0))
		{
			Printf("\r\n PASSWORD OK",0);
		}
		else
		{
			if( title_item==3 &&cur_osdwin==2 && OSDMenuLevel==3 
			  && flag==UP && /*cursor_item==6 && */new_cursor==5)	 
			{
			   //cursor_item=6;
			   new_cursor=3;			  					
			}
			
			if( title_item==3 && cur_osdwin==2 &&OSDMenuLevel==3 
			  && flag==DN&& /*cursor_item==3 && */new_cursor==4 ) 
			{
			   //cursor_item=3;
			   new_cursor=6;
			}
		}

		//awt ml073q william-v0.2-20120308
		// link GPS datatime lock

		if((DVR_RecordStatus&GPS)==GPS)		
		{
			if( title_item==1 &&cur_osdwin==2 && OSDMenuLevel==3 
			&& flag==UP && new_cursor==5)	 
			{
				new_cursor=0;			  					
			}
				
			if( title_item==1 && cur_osdwin==2 &&OSDMenuLevel==3 
			&& flag==DN&& new_cursor==1 ) 
			{
				new_cursor=6;
			}
		}

		//DVR NOT READY		//WILLIAM-v1.1-990921		
			if(DVR_Ready==0)
			{
				if(/* title_item==4 && */OSDMenuLevel==1 
				  && flag==UP && cursor_item==4 )	 
				   {
				   cursor_item=4;
				   new_cursor=2;			  					//william-v1.43-970331
					}
				
				if( /*title_item==2 && */OSDMenuLevel==1 
				  && flag==DN&& cursor_item==2  ) //william-v1.43-970331
				{
				   cursor_item=2;
				   new_cursor=4;
				}
			}
	#endif	//	#ifdef SUPPORT_SDCARD		

#ifdef	AWT_ML072Q  //Pinchi 20140905 add for AWT ML072Q
  if ( title_item == 3 && cur_osdwin == 2 && OSDMenuLevel == 2 
   	&& flag == UP && cursor_item == 0 && new_cursor == 6 )   // 當RECALL選項 且欲往上移 時,
     {
	 	new_cursor = 1;
     }
  
   if ( title_item == 3 && cur_osdwin == 2 && OSDMenuLevel == 2 
   	&& flag == DN && cursor_item == 1 && new_cursor == 2 ) //  當exit 選項 且 欲 往下移時
     {
	 	new_cursor = 0;
     }
#else
//	william @20130813 >>>>>>>>>>>>>>>>>>>>>>>>>>
  if( title_item==3 && cur_osdwin==2 && OSDMenuLevel==2 
   	&& flag==UP && cursor_item==0 && new_cursor==6 )   // 當RECALL選項 且欲往上移 時,
     {
   	 cursor_item=0;
	 new_cursor=2;				//william-v1.43-970331
      }
   if( title_item==3 && cur_osdwin==2 && OSDMenuLevel==2 
   	&& flag==DN&& cursor_item==2 && new_cursor==3 ) //  當exit 選項 且 欲 往下移時
     {
   	 cursor_item=1;
	 new_cursor=0;
      }
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<	
		old_cursor = cursor_item;
		cursor_item = new_cursor;
		if(OSDMenuLevel == 1)
		{ 		
		WaitEndofDisplayArea();	

		WriteDecoder(TW88_WINNUM, 0x01);		//ryan@20170828 
		WriteDecoder(TW88_WINCHEFFECT, 0x20);	//ryan@20170828 修正ICON 有雜點	
		ShowOSDWindow_TW88(OSDCURSORWINDOW, OFF);//ryan@20170808

	

		#ifdef HS_NEWOSDMENU	//Weylis -@121008
		 	dat = GetOSDWindowX(OSDCURSORWINDOW);
			dat += ((cursor_item-old_cursor)*(18));
			SetOSDWindowX_TW88(OSDCURSORWINDOW,dat);		
		#else
			dat = GetOSDWindowY(OSDCURSORWINDOW);
			#ifdef TW8816
			dat += (cursor_item-old_cursor)*(27+2);  //ryan@20170901	
			
			#else
			dat += (cursor_item-old_cursor)*(ICON_HIGH*2);			//william @20130808
			#endif
			SetOSDWindowY_TW88(OSDCURSORWINDOW,dat);	
		#endif
		
			WriteDecoder(TW88_WINNUM, OSDCURSORWINDOW-1);
		#ifdef HS_NEWOSDMENU
			WriteDecoder(TW88_WINSADDRLO, ((cursor_item*3)));
		#else
			WriteDecoder(TW88_WINSADDRLO, cursor_item*2);	
		#endif
			DisplayOSDMenu();
			ShowOSDWindow_TW88(OSDCURSORWINDOW, ON);//ryan@20170808
		
			WriteDecoder(TW88_WINNUM, 0x01);		//ryan@20170904
			WriteDecoder(TW88_WINCHEFFECT, 0x00);	//ryan@20170904  解除修正ICON 有雜點
		
		
		}
		else 	
		{
			if( MenuChild[ old_cursor ].Id == OSDLANG)
				DrawMenulist(0); //		Recover Attr. and menu Data list
			DisplayCursor();
		}

#ifdef HS_NEWOSDMENU_2  //Weylis080929
	DrawAttrCnt(OSDMENU_SUBADDR+TITLE_MAX_DESC*8, COMMENT1_COLOR, TITLE_MAX_DESC );  // All Line Attr clear by default color.
	Str = Str_Comments[OSDLang];
	i=strlen(Str);
	WriteStringToAddr(OSDMENU_SUBADDR+TITLE_MAX_DESC*8, Str, i);
#endif		
}

void OSDValueUpDn(BYTE flag)
{
	CODE_P BYTE *ptr;
	WORD val;
	WORD addr;
//	BYTE len1;

	addr = OSDMENU_SUBADDR;
	addr = addr + MAX_DESC + (cursor_item+1)*MenuFormat->width;
	
	switch ( MenuChild[cursor_item].Type ) {
	
	case BAR:	
				val = SetItemValue(MenuChild[cursor_item].Id, flag );

 #ifdef AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150209 add
             if (((MenuChild[cursor_item].Id) == TRIGGER_CamADelay) || ((MenuChild[cursor_item].Id) == TRIGGER_CamBDelay)
           	     || ((MenuChild[cursor_item].Id) == TRIGGER_CamCDelay) || ((MenuChild[cursor_item].Id) == TRIGGER_CamRDelay))
                 AWT_DisplayViewBar(addr , val);
             else
         #endif			
				DisplayViewBar( addr, val);
				break;

#ifdef HTRV
	case GRAPH:		//william-v1.43-970319
                val = SetItemValue(MenuChild[cursor_item].Id, flag );//依UP/DN設定每一個功能選項的值,存入eeporm,並傳回設定值
                DisplayGaugeOSD();
     ////       DisplayViewBar( addr, val);   //   顯示bar 與數值
                break;		 
#endif
	
	case NUMBER:
				ptr = MenuChild[cursor_item].TypeData;
				switch ( MenuChild[cursor_item].Id ) {

				#ifdef PAL_TV	//-----------------------------------------------
				case MANUAL_TUNE_CHANNEL:
						ManualTune(flag);
						break;
				#endif					//-----------------------------------------------
				default:
						val = SetItemValue(MenuChild[cursor_item].Id, flag);
						DrawNum( addr, (CODE_P struct NumType *)ptr, val );
						break;
				}
				
				break;
	/*case TIME:
				//tptr = (CODE_P struct TimeType *)MenuChild[cursor_item].TypeData;
				//val = SetItemValue( (val_index==HOUR ? MenuChild[cursor_item].Id + 1 : MenuChild[cursor_item].Id ) , flag);
				//if( val_index == HOUR )
				//	DrawTime( val, tptr->CursorColor, tptr->TimeColor);
				//else
				//	DrawTime( val, tptr->TimeColor,   tptr->CursorColor);

				
				ptr = MenuChild[cursor_item].TypeData;
				val = SetItemValue( (val_index==HOUR ? MenuChild[cursor_item].Id + 1 : MenuChild[cursor_item].Id ) , flag);
				if( val_index == HOUR )
					DrawTime( val, 
					((CODE_P struct TimeType *)ptr)->CursorColor, 
					((CODE_P struct TimeType *)ptr)->TimeColor);
				else
					DrawTime( val, 
					((CODE_P struct TimeType *)ptr)->TimeColor,   
					((CODE_P struct TimeType *)ptr)->CursorColor);
				break;
	*/
	case ENUM_STRING:
				ptr = MenuChild[cursor_item].TypeData;
				val = SetItemValue( MenuChild[cursor_item].Id , flag);
				DrawStringByLang( addr, (CODE_P struct EnumType *)ptr, val );
				break;
	
	default:	break;
	}
}

BYTE CloseOSDMenu(void)
{
	BYTE i;
	extern BYTE PcMode;

	ShowOSDWindowAll(0);  // TW8806B

	//ClearOSD();
	WaitEndofDisplayArea();	
	for (i=OSDCURSORWINDOW; i<=OSDMENU_TITLEWIN ;i++ )
	{
		SetOSDWindowNum(i);
		WriteDecoder(TW88_WINATTR, 0x00);
	}

	ClearDisplayedOSD (MENU);
	#ifdef E_Wintek
	FactoryMenu=0;
	#endif

	#ifndef QUAD
	CheckMirror();
	#endif

//mask by ryan@20170922	SetNowMode();		//william-@20130813 

/*
#ifdef SUPPORT_PC
	// If PC mode is unknown, display outof range	// HHY 7/25/05
	if( GetInputSelection()==PC || GetInputSelection()==DTV ) {
		if( PcMode==EE_PC_UNKNOWN ) {
			//DisplayPCInfo(OutOfRangeStr[GetOSDLang()]);
			ShowOSDWindow(PCINFO_OSDWIN,TRUE);
			ShowOSDWindowAll(1);
			SetDisplayedOSD(PCINFO);
		}
	}
#endif //
*/
	return 1;
}
#ifdef SUPPORT_OSDPOSITIONMOVE
void InitOSDMenuXY(void)
{
	int val, addx, addy;
	BYTE winno;

	addx = OSDMenuX - OSDMenuXRange.Default;
	addy = OSDMenuY - OSDMenuYRange.Default;

	#ifdef DEBUG_OSD
	dPrintf("\r\nOSDMenuX:%d, OSDMenuY:%d", (WORD)OSDMenuX, (WORD)OSDMenuY);
	dPrintf("\r\nOSDMenuYRange.Min=%d", (WORD)OSDMenuXRange.Min);
	dPrintf("\r\nOSDMenuYRange.Max=%d", (WORD)OSDMenuXRange.Max);
	dPrintf("\r\nOSDMenuYRange.Default=%d", (WORD)OSDMenuXRange.Default);
	dPrintf("\r\nOSDMenuX - OSDMenuXRange.Default=%d", (WORD)addx);
	#endif

	for( winno=OSDCURSORWINDOW; winno<=OSDMENU_TITLEWIN; winno++) {

		val = GetOSDWindowX( winno );
		SetOSDWindowX(winno,val+addx);

		val = GetOSDWindowY( winno );
		SetOSDWindowY(winno,val+addy);
	}
}
#endif

#ifdef HS_NEWOSDMENU
void OpenOSDMenu(BYTE OSD_ID)
#else
void OpenOSDMenu(void)
#endif
{
	BYTE i;
#ifdef HS_NEWOSDMENU_2	
	WORD dat;
#endif
	ClearOSDInfo();
	{ // TW8806 
	#ifdef HS_NEWOSDMENU
		InitOSDMenu(OSD_ID);		// reset OSD window and prepare menu
	#else
		InitOSDMenu();		// reset OSD window and prepare menu
	#endif	
	}
#ifdef REFERENCE
	Change_OSDColorLookup();
#endif

#ifdef HS_NEWOSDMENU
	for(i=OSDCURSORWINDOW; i<=OSDMENU_TITLEWIN; i++)
#else
	for(i=OSDCURSORWINDOW; i<=OSDMENU_TOPWIN; i++)
#endif
	{
		ShowOSDWindow_TW88(i,TRUE);
	}

	ShowOSDWindow_TW88(DVRStatus_OSDWIN,FALSE);	
	ShowOSDWindowAll(1);
	SetDisplayedOSD( MENU );

#ifdef HS_NEWOSDMENU_2 // Weylis -@120322
	cursor_item = 3;	//set "DMUtility" Icon Active
	WaitEndofDisplayArea();
	dat = GetOSDWindowY(OSDCURSORWINDOW);
	dat += (cursor_item) * ICON_HIGH;
	SetOSDWindowY_TW88(OSDCURSORWINDOW, dat);
	WriteDecoder(TW88_WINNUM, OSDCURSORWINDOW - 1);
	WriteDecoder(TW88_WINSADDRLO, cursor_item * 2);
	DisplayOSDMenu();

	OSDSelect();		//Enter "DMUtility" item
	cursor_item = 3;  	//select  "Format SD Card" 
	OSDSelect();		//Enter  "Format SD Card"  Item
#endif

}

#ifdef Format_SDCard
#if 0
void OpenFORMATMenu(void)
{
	BYTE i;
	WORD dat;

	ClearOSDInfo();
	{ // TW8806 
		InitOSDMenu();		// reset OSD window and prepare menu
#ifdef REFERENCE
		Change_OSDColorLookup();
#endif	
	}
	for(i=OSDCURSORWINDOW; i<=OSDMENU_TOPWIN; i++) {
		ShowOSDWindow_TW88(i,TRUE);
	}

	ShowOSDWindow_TW88(DVRStatus_OSDWIN,FALSE);	
	SetDisplayedOSD( MENU );
// Weylis -@120322
#if 1
	cursor_item = 3;	//set "DMUtility" Icon Active
	WaitEndofDisplayArea();
	dat = GetOSDWindowY(OSDCURSORWINDOW);
	dat += (cursor_item) * ICON_HIGH;
	SetOSDWindowY_TW88(OSDCURSORWINDOW, dat);
	WriteDecoder(TW88_WINNUM, OSDCURSORWINDOW - 1);
	WriteDecoder(TW88_WINSADDRLO, cursor_item * 2);
	DisplayOSDMenu();

	OSDSelect();		//Enter "DMUtility" item
	cursor_item = 3;  	//select  "Format SD Card" 
	OSDSelect();		//Enter  "Format SD Card"  Item
	ShowOSDWindowAll(1);
#endif

}
#endif
#endif

#ifdef HS_NEWOSDMENU
void InitOSDMenu(BYTE OSD_ID)
#else
void InitOSDMenu()
#endif
{
	extern CODE BYTE OSDPOSITIONMODE_TABLE[][2] ;
	BYTE i=0;

	//DownloadUDFont();			//WILLIAM-981029
	OSDLang = GetOSDLangEE();

	switch( GetInputSelection() ) {

	#ifdef SUPPORT_PC
	case PC:
	  #ifdef E_Wintek
		if(FactoryMenu==0)
			MenuTitle = ChinesePCMenu;
		else
	  #endif
			MenuTitle = PCMenu;
		break;
	#endif

	//#ifdef SUPPORT_DTV
	//case DTV:	MenuTitle = DTVMenu;		break;
	//#endif 

	default:
	#ifdef HS_NEWOSDMENU
		if(OSD_ID == 0)
				;//MenuTitle = OSDSetupMenu;	
		else
		{
		#ifdef E_Wintek
			if(FactoryMenu==0)
				MenuTitle = ChineseMenu;
				//MenuTitle = OSDSetupMenu;	//Weylis
			else
		#endif		
				MenuTitle = DefaultMenu;	
		}
	#else
		#ifdef E_Wintek
			if(FactoryMenu==0)
				MenuTitle = ChineseMenu;
			else
		#endif
				MenuTitle = DefaultMenu;
	#endif
		break;
	}

	MenuSP = 0;								// Clear stack of menu.
	title_item = 0;
	MenuFormat = ( CODE_P struct MenuType *) MenuTitle[title_item].TypeData;

	OSDMenuLevel = 1; // OSD menu tree level
	cur_osdwin = OSDMENU_TOPWIN;

	MenuChild = MenuTitle[title_item].Child;
	cursor_item = 0;
	
	val_osdwin = 0xff;
	val_index = 0;
	OnChangingValue = 0;

	//--- OSD Window Initialization
#ifdef ADD_ANALOGPANEL
	if(IsAnalogOn())
		InitOSDWindow(Init_Osd_MainMenu_A);
	else
#endif
		InitOSDWindow(Init_Osd_MainMenu);  // Define Window Top(4), Sub(3), Bar(2), Cursor(1) 
	ClearDataCnt(OSDMENU_MAINADDR, 255);	// Clear the counter of OSD RAM Data from addr with space(' ').	

	//--- Positioning 
#ifdef SUPPORT_OSDPOSITIONMOVE
	i = GetOSDPositionModeEE();

	OSDMenuX = OSDPOSITIONMODE_TABLE[i][0];
	OSDMenuY = OSDPOSITIONMODE_TABLE[i][1];

	InitOSDMenuXY();	 // Default + Saved OSD Position
#endif

	//--- Display OSD Menu
	DisplayOSDMenu(); 
}

void OSDHighMenu(void)
{
#ifdef HS_NEWOSDMENU_2  //Weylis -@121008
	CODE_P BYTE *Str;
	BYTE i;
#endif
	if( OnChangingValue )
	{
		//OnChangingValue = 0;
		ClearValueWin();
//		DisplayCursor();
		DrawMenulist(0); //		Recover Attr. and menu Data list
		DisplayCursor();

		return;
	}

	displayOnValueWindow = 0;
	DoAction(MenuChild[cursor_item].PostFnId); // When exiting from the previous menu, Use to need some of action.

	if( OSDMenuLevel != 1) ClearCursor();
		OSDHighMenu_sub();

#ifdef HS_NEWOSDMENU_2 //Weylis -@121008
	DrawAttrCnt(OSDMENU_SUBADDR+TITLE_MAX_DESC*8, COMMENT1_COLOR, TITLE_MAX_DESC );  // All Line Attr clear by default color.
	Str = Str_Comments[OSDLang];
	i=strlen(Str);
	//WriteStringToAddr(OSDMENU_SUBADDR+TITLE_MAX_DESC*8, Str, i);
	WriteStringToAddr(OSDMENU_SUBADDR+TITLE_MAX_DESC*8, Str, i);
#endif
}

void OSDSelect(void)
{
	BYTE ret;
	WORD addr;

	if( cursor_item==NIL )			return;
	if( MenuChild[ cursor_item ].Id == TVINSTALL && GetInputSelection()!= TV )return;

	if( MenuChild[ cursor_item ].Id == VIDEO_HUE && ReadVInputSTD()!= NTSC && ReadVInputSTD()!= NTSC4 )  		return;
	//if( MenuChild[ cursor_item ].Id == VIDEO_SHARPNESS && GetInputSelection()== SCART )  		return;

	//do  {
		ret = DoAction(MenuChild[cursor_item].PreFnId);
		if(!ret) return;

		switch ( MenuChild[ cursor_item ].Type ) {
		case ACTION:
						DoAction(MenuChild[cursor_item].Id); // Action.
						return;

	    case TEXT_STR_exit:  // Ryan
		     ClearCursor();
	         CloseOSDMenu();    
			 break;
		 
		case ENUM_STRING:		
		case BAR:
						OnChangingValue = 1; // Value changable label on 				
						DisplayCursor();	//Display Setect Item cursor
						GetItemValue(MenuChild[cursor_item].Id);
						return;

	#ifdef HTRV
        case GRAPH:		//william-v1.43-970319
            OnChangingValue = 1;                     // Value changable label on      
            GaugeSetup=1; //WILLIAM-V1.43-970319
            GaugeInfoOSD =1;
            //DisplayCursor();                         //Display Setect Item cursor//在設定icon視窗與文字視窗的    背景色,前景文字色
            //william-v1.430-970402>>>>>>>>>>>>>>>>>>>>>>>         
            if(GetInputSelection()==COMPOSITE3)              
                ChangeInput(COMPOSITE1);

	        //william-v1.430-970402>>>>>>>>>>>>>>>>>>>>>>>	 
            GetItemValue(MenuChild[cursor_item].Id);	
         // TW2835Command(0x03,0x02|(Attr_A&0x01),0x02|(Attr_B&0x01),0x02|(Attr_C&0x01),0x0E|(Attr_R&0x01));  //Pinchi 20151209 disable  //WILLIM-V1.43-970324

            SelectModeType = CAM_R;////_20140313_xx_B

			// RearMode=0; //WILLIM-V1.43-970324
        #ifdef HTRV_SUPPORT_4_3_SCREEN  //Pinchi 20151209 add
          #if 1  //Pinchi 20150826 change the setting for 4:3
            WriteDecoder(0x0a,0x13); 
            WriteDecoder(0x61,0xa6);
            WriteDecoder(0x66,0x60);//<William-960905> 
          #else
		   	WriteDecoder(0x0a,0x21); 
			WriteDecoder(0x60,0xdf);
			WriteDecoder(0x66,0x60);
          #endif
        #endif
            DisplayGaugeOSD();				
		
	////	 SetDisplayedOSD( MENU );	////_20140312_xx_B
	
            Printf("\r\nOnChangingValue=0x%bx", OnChangingValue);		
            return;
	#endif
						
		case NUMBER:	
						OnChangingValue = 1;

						switch ( MenuChild[cursor_item].Id ) {
						default:
								DisplayCursor();

						//		DrawNum( (CODE_P struct NumType *)ptr, GetItemValue(MenuChild[cursor_item].Id) );
								break;
						}
						return;

		case OSDMENU:
		case CHOICEMENU:
						if(OSDMenuLevel == 1) {
							addr = OSDMENU_MAINADDR + cursor_item*MenuFormat->width + 0;
							//DrawAttrCnt(addr, CURSOR_COLOR, 2);	// TOP Menu Icon Selected by color
							// 3D Effect Cursor Bottom
							WriteDecoder(TW88_WINNUM, OSDCURSORWINDOW-1);
							WriteDecoder(TW88_WINATTR, OSD_Window_Attr(WINDOW_3D_BOTTOM,BG_COLOR_BLACK|EXTENDED_COLOR,0,WINDOW_3D_ENABLE,WINDOW_ENABLE));	
						}

						OSDMenuLevel++;	// Down Menu tree.

						pushMenu();	// Push MenuTitle, title_item in stack.
						MenuTitle = MenuChild;
						title_item = cursor_item;

						MenuFormat = ( CODE_P struct MenuType *) MenuTitle[title_item].TypeData;				
						MenuChild = MenuTitle[title_item].Child;
						cursor_item = Get_cursor_item( MenuTitle[title_item].Id );

						if( cur_osdwin == OSDMENU_TOPWIN ) // Window #4
							cur_osdwin = OSDMENUWINDOW; // Window #3
						else
							DisplayOSDMenu();

						DisplayCursor();
						break;
		}
	//} while( cursor_item!=NIL && MenuChild[cursor_item].ActiveWithCursor==YES );
}

#if 0
//-------------------------------------------
//         設定Auto Day & Night動作
//-------------------------------------------
//0->ON ; 1->0FF
void DaySelect(BYTE val)
{     
   if(val)      //Auto OFF
     CDS_CTRL=0;
   else         //Auto ON
     CDS_CTRL=1;
}
#endif

//-------------------------------------------
//         設定CAM Out動作
//-------------------------------------------
//0->QUAL ; 1->CAMA ;2->CAMB ; 3->CAMC
void CAMOutDSel(BYTE  val)
{
    if(val==0)
      TW2835Cmd("\n\ryvmix 4\n\r"); 
    else if(val==1)
      TW2835Cmd("\n\ryvmix 0\n\r");
    else if(val==2)
      TW2835Cmd("\n\ryvmix 1\n\r");
    else if(val==3)
      TW2835Cmd("\n\ryvmix 2\n\r");
    else if(val==4)
      TW2835Cmd("\n\ryvmix 3\n\r");
}

//-------------------------------------------
//         設定DVR AUDIO RECORD CH動作
//-------------------------------------------
//0->A ; 2->B ;1->C ; 3->R
#ifdef SUPPORT_SDCARD  //Pinchi 20140903 add for AWT ML072Q
void DVRAodioSel(BYTE  val)
{
#if 1
    if(val==0)
      TW2835Cmd("\n\raudio 0\n\r"); 
    else if(val==1)
      TW2835Cmd("\n\raudio 2\n\r");
    else if(val==2)
      TW2835Cmd("\n\raudio 1\n\r");
    else if(val==3)
      TW2835Cmd("\n\raudio 3\n\r");
#endif
}
#endif

#ifdef 	 HS_NEWOSDMENU
BYTE ChkOSDMENU_TOPWIN(void)
{
	if( cur_osdwin == OSDMENU_TOPWIN ) // Window #4
		{
			return 1;
		}
	return 0;
}
BYTE ChkOSDMENU_WINDOW(void)
{
	if( cur_osdwin == OSDMENUWINDOW ) // Window #4
			return 1;
	return 0;
}

BYTE ChkOSDMENU(void)
{
	if (GetDisplayedOSD() & MENU) 
		return 1;
	else
		return 0;
}
#if 0
BYTE ChkOSDTYPE(void)
{
	//MenuTitle = OSDSetupMenu;
//Printf("KKK = %02x",MenuTitle[title_item].Type);
	if ( MenuTitle[title_item].Type & OSDMENU) 
		return 1;
	else
		return 0;
}
#endif
#endif




