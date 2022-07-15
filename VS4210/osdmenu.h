//osdmenu.h
//LJY10292001
#ifndef __OSDMENU__
#define __OSDMENU__

//=======================================================================
// 
//BYTE GetOSDuration(void);

void ResetVideoValue(void);
//#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
//void ResetPCValue(void);
//#endif
BYTE GetOSDMenuLevel( void );

void DrawMenulist(BYTE);
void Display_EnterPin(void);
void OSDHighMenu_sub(void);
void ResetOSDValue(void);
#ifdef E_Wintek
void ResetUESROSDValue(void);
#endif
//BYTE OnChangingValue(void);
//Not used yet *******************BYTE OnChangingTime(void);

//WORD GetOSDclock(void);

#ifdef HS_NEWOSDMENU
void InitOSDMenu(BYTE OSD_ID);
void OpenOSDMenu(BYTE OSD_ID);
#else
void InitOSDMenu(void);
void OpenOSDMenu(void);
#endif
BYTE CloseOSDMenu(void);

void OpenFORMATMenu(void);
void OSDHighMenu(void);
void OSDSelect(void);
void OSDCursorMove(BYTE flag );
void OSDLeftRight(BYTE flag );
void OSDValueUpDn(BYTE flag);
WORD SetItemValue( BYTE id, BYTE flag );
BYTE DoAction(BYTE id);

BYTE SaveValue1(void);
void ClearValueWin(void);

void OSDNumber( BYTE num );

void Set4WideScreen(BYTE id);

void SetRGBContrast( BYTE id, BYTE val );
BYTE SetPCColorTemp(BYTE flag);
void ResetPanelAttributeValue(void);
WORD GetItemValue( BYTE);
void DisplayViewBar( WORD,BYTE);
void DrawNum(WORD, CODE_P struct NumType *, WORD);
void DrawStringByLang( WORD, CODE_P struct EnumType *, WORD);

void DisplayVolumebar(BYTE val);
BYTE GetOSDLang(void);
void InitOSDMenuXY(void);

void Display_TVGuide_Window(void);
void Display_TVGuide_Status(void);

void ChangeVchip_LevelCursor(BYTE);
void ChangeVchip_ContentCursor(BYTE);
void Select_VchipLevel(void);
void SetVideoMode(BYTE mode);
BYTE GetVideoDatawithOffset(BYTE offset);
void SetVideoContrast(BYTE newv);
void SetVideoBrightness(BYTE newv);
void SetVideoHue(BYTE newv) ;
void SetVideoSaturation(BYTE svtype, BYTE newv) ;
BYTE SetVideoSharpness(BYTE flag) ;
void SetVideoBacklight(BYTE newv) ;
//------------------------------------------------andy awt 981013
//void DaySelect(BYTE);
void CAMOutDSel(BYTE val);
void DVRAodioSel(BYTE  val);

//------------------------------------------------andy awt 981013

void DisplayLineOSD(void);	// OSD Line display Demo
#ifdef CENEC_OSDMENU_ACCM8
BYTE ChkOSDMENU_TOPWIN(void);  //Weylis 080930
BYTE ChkOSDMENU_WINDOW(void);
BYTE ChkOSDMENU(void);
#endif

#ifdef AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150209 add
void AWT_DisplayViewBar(WORD addr , BYTE val);
#endif
struct MenuType {
	BYTE	width,	height;
	BYTE	TitleColor;	
	BYTE	CursorColor;
};

struct NumType {
	BYTE	PrePostCharLen;		// high nibble(pre) / low nibble(post)
	BYTE	PrePostChar[4];
};

struct NumberStringType {
	BYTE	x, y;
	BYTE    width;
	BYTE	BackColor;
	BYTE	Radix;
	BYTE	NumColor;
	BYTE    MaxLen;				// maximum length of string
	BYTE	PrePostCharLen;		// high nibble(pre) / low nibble(post)
	BYTE	PrePostChar[3];
};

struct TimeType {
	BYTE	x, y;
	BYTE    width;
	BYTE	BackColor;
	BYTE	Format;				//_24H_TIME
	BYTE	TimeColor;
	BYTE    CursorColor;
};

struct EnumType {
	BYTE    Sz;//size
	BYTE    ByLang;
	BYTE	Str[15];			// doesn't matter the size !!!
};

#define ICON_CH_SZ		2
//#define BAR_NUM_SZ		4
#define ICON_BUF_SZ		2
struct DefineMenu {
	//WORD						Icon[ICON_BUF_SZ];	
	BYTE				CODE	*Icon;
	BYTE						**Desc;				
	BYTE						PreFnId, 
								PostFnId;
//	void		(*CalltoFunction)(void);				    // Un/Dn/Sel Function
	BYTE						IconColor;			//NIL: not to display icon
	BYTE						DescColor;			//NIL: not to display desc. (IconColor==NIL && DescColor==NIL)->Last Item
	struct DefineMenu	CODE	*Child;
	BYTE						Type;				// MENU, CHOICEMENU, BAR, NUMBER, TIME, ACTION, ENUM_STRING
	BYTE						ActiveWithCursor;
	BYTE						Id;
	BYTE				CODE	*TypeData;	//MenuType, BarType, NumType,,,
} ;

struct PictureInfo {

	BYTE Register_Addr;
	BYTE EEPROM_Addr;
	struct RegisterInfo 	CODE	*Control_Range;
};


//-------------------------------------------------------------------
// OSD Window Information
//-------------------------------------------------------------------
// Window Number
#define OSDCURSORWINDOW 		1
#define OSDMENUWINDOW   		OSDCURSORWINDOW+1
#define OSDMENU_TOPWIN  		OSDCURSORWINDOW+2
#define OSDMENU_TITLEWIN 		OSDCURSORWINDOW+3
#define OSDBARWINDOW 			OSDCURSORWINDOW

#define TECHWELLOGO_OSDWIN   	1	
#define TVINFO_OSDWIN 			2	
#define FONTTEST_OSDWIN 		2	
#define INPUTINFO_OSDWIN 		3  
#define RATINGINFO_OSDWIN 		1	
#define PCINFO_OSDWIN 			3
//#define PCINFO_OSDWIN		4

//-------------OSDMENU_TITLEWIN------------------------------------------------------
// Window Start address
#define OSDMENU_MAINADDR	0
//#define OSDMENU_SUBADDR     OSDMENU_MAINADDR+15    //25*9:196
//#define OSDMENU_SUBADDR    	OSDMENU_MAINADDR+HS_WIDTH //0+40   //25*9:196
//#define OSDMENU_SUBADDR    	OSDMENU_MAINADDR+10 //0+40   //25*9:196
#define OSDMENU_SUBADDR    	OSDMENU_MAINADDR + 14 //0+40   //25*9:196  // Weylis 121008 清除 Icon 後面

#define OSDMENU_BARADDR		200 //OSDMENU_SUBADDR+225    //256   //25x2:50

#ifdef WQVGA
#define INPUTINFO1_ADDR		INPUTINFO_ADDR+17	//17x3:51
#else
#define INPUTINFO1_ADDR		INPUTINFO_ADDR+34	//17x2:51
#endif

#define MUTEINFO_ADDR 		INPUTINFO1_ADDR+51	// 4x1:4
#define TVCHINFO_ADDR  		MUTEINFO_ADDR//324	// 3X1:3
#define LOGO_ADDR			0
#define RATINGINFO_ADDR		0	//addr:210(8x1:8)
#define PCMEASUREINFO_ADDR  OSDMENU_BARADDR //10x1
#ifdef QUAD
#define INPUTINFO_ADDR		160	// 9x1:9
#else
#define INPUTINFO_ADDR		20	// 9x1:9
#define INPUTINFO_MIRROR_ADDR	73//83	//william-v1.0-970804
#define INPUTINFO_WIDTH		40	// 9x1:9		//william-v1.0-971111
#endif
//--------------------------------------------------------------------
// OSD Menu Position Range
#if ((defined WQVGA) && (defined ANALOG_7INCH)) || ((defined QVGA) && (defined SHARP_LQ050Q5DR01))
	#define OSD_PHR_ PHR_/2
#else
	#define OSD_PHR_ PHR_
#endif

#define OSDHSTART_MAX (OSD_PHR_-(12+1)*25 - 56*2 -4)  //PHR - (pixelsize +Hspace)*MenuWidth - Hborder*2 - outlinewidth*2
#define OSDVSTART_MAX (PVR_-(18+11)*9 - 5*2 ) //  PVR - (Linesize+Vspace)*MENU_HEIGHT - Vborder*2  - outlinewidth*2

#define OSDHSTART_MAX_A (OSD_PHR_-(12+2)*25 - 56*2 -8) //OSDHSTART_MAX  //PHR - (pixelsize +Hspace)*MenuWidth - Hborder*2 - outlinewidth*2
#define OSDVSTART_MAX_A OSDVSTART_MAX  //  PVR - (Linesize+Vspace)*MENU_HEIGHT - Vborder*2	- outlinewidth*2

#define OSDMENUX_MIN 1//0
#define OSDMENUX_MAX (OSDHSTART_MAX>>3)
#define OSDMENUX_50  (OSDHSTART_MAX>>4)

#define OSDMENUY_MIN 2//0
#define OSDMENUY_MAX (OSDVSTART_MAX>>2)
#define OSDMENUY_50  (OSDVSTART_MAX>>3)

#ifdef HS_NEWOSDMENU	//Weylis -@121008
// HS OSD Style
#define HS_WIDTH			40
#define HS_CHSPACE			14
//#define HS_CHSPACE			18
#define HS_ICONSPACE		1
#define	HS_ICONSADDR		0
//#define HS_ICONSPACE		2
//#define HS_ICONSADDR		8
#endif

//--------------------------------------------------------------------
// OSD Menu Width, Height 
#ifdef E_Wintek
#define ICON_HIGH				0x14
#else
#define ICON_HIGH				0x0a
#endif

#define TOTAL_MAX_HIGHT 		9
#define TOTAL_TOP_HEIGHT		5	// Include blank line!
#define TOPMENU_WIDTH			2
#define TOPMENU_HEIGHT			4	

#if (defined QVGA) && (defined SHARP_LQ050Q5DR01)
#define MENU_WIDTH				17 //23//25 
#define TITLE_MAX_DESC			17 //25	//
#define BAR_LENGTH				0
#else
//#define MENU_WIDTH				23  //23//25 
//#define MENU_WIDTH				26  //ryan@20170821
#define MENU_WIDTH				24  //ryan@20170821

#define TITLE_MAX_DESC			23  //25	//
#define BAR_LENGTH				5  //8
#endif

#ifdef TW8816
#define MENU_HEIGHT 			8  //6 
#else
#define MENU_HEIGHT 			7  //6 
#endif
//#define BARNUM_LENGTH 			8  //11
#define BARNUM_LENGTH 			9//ryan@20170821

#ifdef REFERENCE
#define BACK_COLOR				BG_COLOR_CYAN
#define DEFAULT_COLOR			BG_COLOR_CYAN   | CH_COLOR_WHITE
//#define ACTIVE_COLOR			BG_COLOR_YELLOW | CH_COLOR_WHITE
#define TITLE_BACK_COLOR		BG_COLOR_MAGENTA
#define TITLE_COLOR				TITLE_BACK_COLOR | CH_COLOR_YELLOW
#define COMMENT_COLOR			TITLE_BACK_COLOR | CH_COLOR_WHITE
#define CURSOR_COLOR			BG_COLOR_YELLOW | CH_COLOR_BLUE
#define OSD_INFO_COLOR			DEFAULT_COLOR
#define BAR_COLOR				DEFAULT_COLOR	
#define BAR_NUM_COLOR			DEFAULT_COLOR
#define NUM_COLOR				DEFAULT_COLOR
#define OSDMENU_OUTLINE_COLOR	BG_COLOR_BLUE
#else
#define BACK_COLOR				BG_COLOR_CYAN
#ifdef AWT_ML072Q
//#define DEFAULT_COLOR			BG_COLOR_CYAN | CH_COLOR_GREEN//ryan@20170810
#define DEFAULT_COLOR			BG_COLOR_CYAN | CH_COLOR_GREEN//ryan@20170828

#else
#define DEFAULT_COLOR			BG_COLOR_CYAN | CH_COLOR_WHITE
#endif
//#define ACTIVE_COLOR			BG_COLOR_YELLOW | CH_COLOR_WHITE
#define TITLE_BACK_COLOR		BG_COLOR_BLUE
#define TITLE_COLOR				TITLE_BACK_COLOR | CH_COLOR_YELLOW
#define COMMENT_COLOR			TITLE_BACK_COLOR | CH_COLOR_WHITE
#define CURSOR_COLOR			BG_COLOR_YELLOW  | CH_COLOR_BLUE
#define OSD_INFO_COLOR			BG_COLOR_CYAN    | CH_COLOR_GREEN
#define BAR_COLOR				DEFAULT_COLOR
#define BAR_NUM_COLOR			DEFAULT_COLOR
#define NUM_COLOR				DEFAULT_COLOR
#define VER_COLOR 				BG_COLOR_CYAN   | CH_COLOR_YELLOW   /// V1.1 03/07/07 simonsung
#define OSDMENU_OUTLINE_COLOR		BG_COLOR_CYAN

#endif

#ifdef HS_NEWOSDMENU
#define COMMENT1_COLOR			BG_COLOR_CYAN | CH_COLOR_RED
#endif

#define Trigger_COLOR 			BG_COLOR_CYAN|CH_COLOR_RED			//william-v1.1-970827
#define Mirror_COLOR 			BG_COLOR_CYAN|CH_COLOR_YELLOW			//william-v1.1-970827
#define Jump_COLOR 				BG_COLOR_CYAN|CH_COLOR_BLUE			//william-v1.1-970827
#define Gauge_COLOR1 			BG_COLOR_BLACK|CH_COLOR_GREEN	
#define Gauge_COLOR2 			BG_COLOR_BLACK|CH_COLOR_YELLOW	
#define Gauge_COLOR3 			BG_COLOR_BLACK|CH_COLOR_RED

//DisplayedOSD
//		new	  |	MENU	TVINFO		CC		INPUTINFO		PCINFO		RATINGINFO
//	current   |
//============|=========================================================================
//	MENU	  |			TVINFO		--		INPUTINFO		PCINFO		MENU + RATINGINFO	
//			  |						 
//------------|-------------------------------------------------------------------------
//	TVINFO	  |	MENU	TVINFO		--		INPUTINFO		PCINFO		TVINFO+RATING
//------------|-------------------------------------------------------------------------
//	CC		  |	MENU->	TVINFO->			INPUTINFO->		PCINFO		RATINGINFO
//			  |	 CC		 CC					 CC				 		 
//------------|-------------------------------------------------------------------------
//	INPUTINFO |	MENU	TVINFO		--		INPUTINFO		PCINFO		--
//------------|-------------------------------------------------------------------------
//	PCINFO	  |	MENU	x			x		INPUTINFO		PCINFO		x
//------------|-------------------------------------------------------------------------
//	RATINGINFO|	MENU	TVINFO	   RATING   INPUTINFO		x			RATINGINFO
//									

//=======================================================================================
// OSD Menu Constant define.
//=======================================================================================
//Time type
#define _24H_TIME		1

//for previous, next, parent or child
#define YES				0xfe	
#define NO				0

//#define OVERWRITE		0
//#define POPUP			1

//CursorMove
#define VERT			0
#define HORI			1

//Type of MenuItem
#define OSDMENU			1
#define CHOICEMENU		2
#define ACTION			3
#define BAR				4
#define NUMBER			5
#define CHOICE			6
#define ENUM_STRING		7 //YESNO			7
#define CHOICE1			8
#define TIME			9
#define NUMBER_STRING	10
#define TEXT_STR              11   // V1.1 02/28/07 simonsung
#define INFORMATION              12
#define TEXT_STR_exit	13

#ifdef HTRV
#define GRAPH	16
#endif

//Bar type
#define LEFT_FILLED		1
#define RIGHT_FILLED	2
#define NO_FILLED		3

//OSDmenu up and down key
#define UP					1
#define DN					2
#define RIGHT				3
#define LEFT  				4

// Action Id
#define	DISP_INFO			0x16

#define	INPUTSELECTION  	0x2f

#define CHANGE_TO_COMPOSITE			(0x20|CH_TW2835)			// Low nibble need to start from 1 
#define CHANGE_TO_SVIDEO			(0x20|SVIDEO)				// so that it match with COMPOSITE...
#define CHANGE_TO_COMPONENT			(0x20|COMPONENT)		
#define CHANGE_TO_DTV				(0x20|DTV)		
#define CHANGE_TO_TV				(0x20|TV)					
#define CHANGE_TO_PC				(0x20|PC)					
#define CHANGE_TO_DIGITALVGA		(0x20|DIGITALVGA)			
#define CHANGE_TO_SCART				(0x20|SCART)				

#define OSDPOSITIONMODE     0x10
#define OSD_VPOS			0x11
#define OSD_HPOS			0x12
#define OSD_HALFTONE		0x13
#define OSD_DURATION		0x14
#define RESET_OSDVALUE		0x15
#define OSDLANG				0x16

#define PANEL_CONTRAST		0x58
#define PANEL_BRIGHTNESS	0x59
#define PANEL_HUE			0x5a
#define PANEL_SHARPNESS 	0x5b
#define PANEL_SATURATION	0x5c

#define PC_VPOS				0x43
#define PC_HPOS				0x44
#define PC_HSIZE 			0x4d

#define PC_CLOCK			0x45
#define PC_PHASE			0x46

#define AUTOADJUST			0x47
#define AUTOADJUST2 		0x48
//#define SAVE_AUTOADJUST		0x49
#define RESET_AUTOADJUST	0x4a

#define PCCOLORADJUST		0x4b
#define SAVE_COLORADJUST	0x4c
//#define RESET_COLORADJUST	0x4d

#define RESET_PCVALUE		0x4e
#define CameraOSD			0x4f

#define PC_COLOR_MODE		0x3f
#define PC_COLOR_TEMP		0x30
#define PC_COLOR_MODE1		0x31
#define PC_COLOR_MODE2		0x32
#define PC_COLOR_MODE3		0x33
#define PC_COLOR_USER		0x34
#define PC_COLOR_R			0x35
#define PC_COLOR_G			0x36
#define PC_COLOR_B			0x37

#define VIDEO_MODE			0x90
#define VIDEO_MODE1 		0x91
#define VIDEO_MODE2 		0x92
#define VIDEO_MODE3 		0x93
#define VIDEO_USER			0x94
#define VIDEO_CONTRAST		0x95
#define VIDEO_BRIGHTNESS	0x96
#define VIDEO_SATURATION	0x97
#define VIDEO_HUE			0x99
#define VIDEO_SHARPNESS		0x9a
#define VIDEO_BACKLIGHT		0x9c		// add new value 
#define RESET_VIDEOVALUE	0x9b
#define RESET_DVRVALUE		0x9e
#define RESET_MonitorVALUE		0x70


#ifdef AWT_ADD_TRIGGER_DELAY_FUN  //Pinchi 20150209 add
#define TRIGGER_CamADelay	0xb2
#define TRIGGER_CamBDelay	0xb3
#define TRIGGER_CamCDelay	0xb4
#define TRIGGER_CamRDelay	0xb5
#endif

#ifdef HTRV
#define RESET_GAUGESETUP	0x9d	 
#define Gauge_VStart			0x7a	 
#endif

#ifdef E_Wintek
#define RESET_USERVALUE	0x9d
#endif
#define VIDEOPICTURE		0x9f

#define INPUTSTD			0x6f
#define CHANGE_TO_AUTO  	0x60
#define CHANGE_TO_NTSC		(0x60 | NTSC)
#define CHANGE_TO_PAL		(0x60 | PAL)
#define CHANGE_TO_SECAM 	(0x60 | SECAM)
#define CHANGE_TO_NTSC4 	(0x60 | NTSC4)
#define CHANGE_TO_PALM		(0x60 | PALM)
#define CHANGE_TO_PALN		(0x60 | PALN)
#define CHANGE_TO_PAL60 	(0x60 | PAL60)

#if 0
#define CLOSED_CAPTION		0x7f
#define CLOSEDCAP_OFF		0x70
#define CLOSEDCAP_CC1		0x71
#define CLOSEDCAP_CC2		0x72
#define CLOSEDCAP_CC3		0x73
#define CLOSEDCAP_CC4		0x74
#define CLOSEDCAP_T1		0x75
#define CLOSEDCAP_T2		0x76
#define CLOSEDCAP_T3		0x77
#define CLOSEDCAP_T4		0x78

#define TV_GUIDE			0x8f
#define NO_VCHIP_LOCK		0
#define VCHIP_TV_Y			1
#define VCHIP_TV_Y7			2
#define VCHIP_TV_G			3
#define VCHIP_TV_PG			4
#define VCHIP_TV_14			5
#define VCHIP_TV_MA			6
#define PASSWORD			0x87
#define NEW_PASSWORD		0x88
#define CHK_PASSWORD		0x89
#define RET_CHK_PASSWORD	0x8a
#endif

#define MPAA_RATING			0x9f
#define NO_MPAA_RATING		0
#define MPAA_G				1
#define MPAA_PG				2
#define MPAA_PG_13			3
#define MPAA_R				4
#define MPAA_NC_17			5
#define MPAA_X				6

#define TVINSTALL			0xa0
#define SCAN_CHANNEL		0xa1
#define AIR_CABLE_TV		0xa3
#define STORE_CLEAR_TVCHN	0xa4
#define MTS					0xa5
#define EDIT_CHANNEL		0xa6
#define MTS_STEREO			0//0x10
#define MTS_MONO			1//0x11

#define AUDIO_VOLUME		0xb7

#if 0 //andy AWT8.4 20090811

#define AUDIO_BALANCE		0xb9
#define AUDIO_BASS			0xba
#define AUDIO_TREBLE		0xbb

#define AUDIO_LOUDNESS		0xbc
#define AUDIO_EFFECT		0xbd
#define AUDIO_MUTE			0xbe
#endif

#define DayChoice           0xb8
#define CAMOSDONOFF         0xb9
#define GAUGEChoice         0xba
#define OsdLockChoice       0xbb
#define PRIORITYChoice      0xcA
#define DVRDisplay		0xcc

#define CAMAONOFF		    0xc1
#define CAMBONOFF	        0xc2
#define CAMCONOFF		    0xc3
#define CAMRONOFF			0xc4

#define IMAGEChoice         0xc5 
#define REARChoice          0xc6
#define JUMPChoice          0xc7
#define CAMOUTChoice        0xc8 
#define STIME_STEP		    0xc9
#define TriggerVGA		    0xcb
#define IMAGE_A_Choice         0xcd
#define IMAGE_B_Choice         0xce
#define IMAGE_C_Choice         0xcf

#define DELAYCAMA		    0xd0
#define DELAYCAMB	           0xd1
#define DELAYCAMC		    0xd2
#define DELAYCAMR	   	    0xd3
#ifdef USE_CAMD
#define DELAYCAMD		    0xd4
#endif
#define ReverseMode		0xd6
#ifdef AWT_ML072S //william @20130902 v1.0.1.2
#define JUMP_AV			0xd7
#endif
#define MANUAL_TUNE_CHANNEL	0xab
#define FINE_TUNE_CHANNEL	0xac
#define STORE_PALCHANNEL	0xad

//#define CLEAR_TVCHN			0xb0
//#define STORE_TVCHN			0xb1

#define WIDESCREEN 			0xc0
#define WIDESCREEN_NORMAL	0 
#define WIDESCREEN_WIDE 	1 
#define WIDESCREEN_PANO 	2
#define WIDESCREEN_FULL 	3 
#define WIDESCREEN_ZOOM 	4 

//#define SYSTEM_CLOCK		0xd1
//#define SYSTEM_CLOCK_H		0xd2
//#define WAKEUP_TIME			0xd3
//#define WAKEUP_TIME_H		0xd4
//#define WAKEUP_PR			0xd5
//#define OFF_TIME			0xd6
//#define OFF_TIME_H			0xd7
//#define SLEEP_TIMER			0xd8

#define Resolution			0xe0
#define FPS					0xe1
#define Quality				0xe2
#define RecordMode			0xe3
#define RecordLength			0xe4
#define PreAlarmLength		0xe5
#define PostAlarmLength		0xe6
#define Apply				0xe7
#if 0	//william-awt ml073q-20120313
#define ManualRecordStart	0xe8
#define ManualRecordStop	0xe9
#define Snapshot				0xea
#endif
#define FormatSD			0xeb
#define AudioRecord			0xec
#define FirmwareUpdate		0xed
#define ChangePassword		0xee
#define OverWrite			0xef

#define UpdateConfig			0xd0
#define SDCardDetect			0xd5

#define Year				0x20
#define Month				0x21
#define Day					0x22
#define Hour				0x23
#define Minute				0x24

#define DaylightSaving				0x25
#define TimeBeginesMonth			0x26
#define TimeBeginesWeek				0x27
#define TimeBeginesTime				0x28
#define StandardTimeBeginesMonth	0x29
#define StandardTimeBeginesWeek		0x2a
#define StandardTimeBeginesTime		0x2b
#define TimeZone					0x2c
#define FirstNumber					0x2d
#define SecondNumber				0x2e
#define ThridNumber					0x2f
#define FourthNumber				0x38
#define WIFIIPNumber				0x39
#define WIFIMACNumber				0x3a
#define GETWAYNumber				0x3b

#define OSDLANGI			0xd9
#define OSDLANGII			OSDLANGI+1
#define OSDLANGIII			OSDLANGI+2
#define OSDLANGIV			OSDLANGI+3

#define TOHIGHMENU			0xf1
#define MOVECURSOR2FIRST	0xf2
#define RESET_ALL			0xf3
#define EXITMENU			0xf4
#define DVRParamApply		0xf5
#define DATETIMEApply		0xf6

#define CHOICE1_VAL			0xfc
#define CHOICE_VAL			0xfd
#define BAR_VAL				0xfe
#define NUM_VAL				0xff

#define CAMNumberChoice     0xa3 
#define PelcoChoice    	    0xa4
#define Baud_rateChoice		0xa5


#endif	// __OSDMENU__

