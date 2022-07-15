#ifndef _VX_OSD_H
#define _VX_OSD_H

#include "VXIS_Extern_Define.h"

#define _Title0 0x00
#define _Title1 0x01
#define _Title2 0x02
#define _Title3 0x03

#define _Bar0 0x04
#define _Bar1 0x05
#define _Bar2 0x06
#define _Bar3 0x07

#define _Content 0x08

#define WINZOOMx1 0x00
#define WINZOOMx2 0x08

///////   Color Define  ///////////////////////////////////////////////////////
#define _Black 0
#define _Blue 1
#define _Green 2
#define _Aqua 3
#define _Red 4
#define _Fuchsia 5
#define _Yellow 6
#define _White 7
#define _Transparent 8
#define _RoyalBlue 9
#define _MediumAqua 10
#define _LightGreen 11
#define _Orange 12
#define _HotPink 13
#define _Silver 14
#define _Gray 15

///////   OSD Font Define  ///////////////////////////////////////////////////////
#define _0 0x62
#define _1 0x63
#define _2 0x64
#define _3 0x65
#define _4 0x66
#define _5 0x67
#define _6 0x68
#define _7 0x69
#define _8 0x6a
#define _9 0x6b
#define _A 0x00
#define _B 0x01
#define _C 0x02
#define _D 0x03
#define _E 0x04
#define _F 0x05
#define _G 0x06
#define _H 0x07
#define _I 0x08
#define _J 0x09
#define _K 0x0a
#define _L 0x0b
#define _M 0x0c
#define _N 0x0d
#define _O 0x0e
#define _P 0x0f
#define _Q 0x10
#define _R 0x11
#define _S 0x12
#define _T 0x13
#define _U 0x14
#define _V 0x15
#define _W 0x16
#define _X 0x17
#define _Y 0x18
#define _Z 0x19
#define _a 0x1a
#define _b 0x1b
#define _c 0x1c
#define _d 0x1d
#define _e 0x1e
#define _f 0x1f
#define _g 0x20
#define _h 0x21
#define _i 0x22
#define _j 0x23
#define _k 0x24
#define _l 0x25
#define _m 0x26
#define _n 0x27
#define _o 0x28
#define _p 0x29
#define _q 0x2a
#define _r 0x2b
#define _s 0x2c
#define _t 0x2d
#define _u 0x2e
#define _v 0x2f
#define _w 0x30
#define _x 0x31
#define _y 0x32
#define _z 0x33
#define _spc 0x6d
#define _action 0x58
#define _and 0x4C
#define _plus 0x53
#define _slash 0x4E
#define _bar0 0x7D
#define _bar1 0x70
#define _bar2 0x71
#define _bar3 0x72
#define _bar4 0x73
#define _bar5 0x74
#define _bar6 0x75
#define _bar7 0x76
#define _bar8 0x77
#define _bar9 0x78
#define _bar10 0x7E

#define _arrow_left 0x56  //<
#define _arrow_right 0x58 //>
#define _arrow_up 0x79	  //^
#define _arrow_down 0x7A  // V
#define _OK 0x57
#define _DIA_F 0x6f
#define _DIA_E 0x7f
#define _DOT 0x45

#define _g01 0x84
#define _g02 0x85
#define _g03 0x86
#define _g04 0x87
#define _g05 0x88
#define _g06 0x89
#define _g07 0x8A
#define _g08 0x8B
#define _g09 0x8C
#define _g10 0x8D
#define _g11 0x8E
#define _g12 0x8F
#define _g13 0x90
#define _g14 0x91
#define _g15 0x92
#define _g16 0x93
#define _g17 0x94
#define _g18 0x95
#define _g19 0x96
#define _g20 0x97
#define _g21 0x98
#define _g22 0x99
#define _g23 0x9B

#define _AL0 0xA0
#define _AR0 0xA4
#define _AU0 0xA8
#define _AD0 0x9C
#define _E0 0xAC

#define _M1G 0x9C
#define _M4G 0x9D
#define _M9G 0x9E
#define _M11G 0x9F
#define _M13G 0xA0
#define _M19G 0xA1
#define _M3G 0xA4
#define _M14G 0xA3

#define _CLG 0xA2

// OSD Function
void VXIS_fSetOsdOnOff(tByte ch, tByte block);
void SetOsdStartAddress(tByte block, tByte addr);
void OsdDumpString(tByte OSDaddr, tcByte /*tByte*/ *Strdata);
void SetOsdPosition(tByte block, tWord X_Pos, tWord Y_Pos);
void SetOsdSize(tByte block, tByte sizeX, tByte sizeY);
void VXIS_fSetOsdx2(bit OnOff);
void setTransParentIndex(tByte index);
void OsdDumpAttribute(tByte OSDaddr, tcByte /*tByte*/ *Strdata, tByte index);
void SetDisplayedOSD(BYTE newd);
BYTE GetDisplayedOSD(void);
void OsdDumpAttributeHS(tByte OSDaddr, tcByte *Strdata, tByte index, tByte Len);

#define NIL 0xff
// for previous, next, parent or child
#define YES 0xfe
#define NO 0
#define DEFAULT_COLOR CH_COLOR_GREEN
#define TITLE_COLOR CH_COLOR_GREEN
#define CURSOR_COLOR CH_COLOR_GREEN | BG_COLOR_Orange
#define TOUCH_CURSOR_COLOR CH_COLOR_GREEN | BG_COLOR_Hot_Pink
#define TOUCH_ADJUST_COLOR CH_COLOR_RED | BG_COLOR_Hot_Pink
#define OSDLOCK_COLOR CH_COLOR_RED
#define TPMS_NUMBER_COLOR CH_COLOR_WHITE
#define OSDCOC_COLOR CH_COLOR_RED

#define RAMFONT 0xFE
#define ROMFONT 0xFF
#define COLORFONT 0x100
//#define CODE  code
#define ICON_X 0
#define ICON_COLOR DEFAULT_COLOR
#define TOPICON_COLOR DEFAULT_COLOR
#define NO_ICON_COLOR NIL

#define DESC_X 2
#define DESC_X_NO_ICON 0
#define DESC_COLOR DEFAULT_COLOR

//#define CURSOR_START			0

#define ITEMSTARTX 0
#define TITLESTARTX 0

#define BAR_NUM_RADIX 10
#define NUM_RADIX 10
#define VIDEOPICTURE 0x9f
#define MENU_WIDTH 20
#define GAUGE_WIDTH 18
#define GRAABER_WIDTH 20

#define MENU_START 64
#define CURSOR_WIDTH (16 - 1)
#define JUMP_WIDTH 2
#define COC_WIDTH 33
#define GrabberMessageWidth 22

//#define MENU_OSD_X_ADDR		48
//#define MENU_OSD_Y_ADDR		64
#if (TH0701024600NYR50S1 == OFF)
#define MENU_OSD_X_ADDR 48 + 50 + 20
#else
#define MENU_OSD_X_ADDR 48 + 50 + 20 + 224
#endif

#if (TH0701024600NYR50S1 == OFF)
#define MENU_OSD_Y_ADDR 64 + 50
#else
#define MENU_OSD_Y_ADDR 64 + 50 + 80
#endif

#define TOUCH_OSD_DEC_ADDR MENU_START + (7 * MENU_WIDTH) + 4
#define TOUCH_OSD_INC_ADDR MENU_START + (7 * MENU_WIDTH) + 14

#define MENU_HEIGHT 8
#define JUMP_HEIGHT 1
#define COC_HEIGHT 2

#define MIRROR_OSD_START 1
#define SPEAKER_OSD_START 5 - 1
#define TRIGGER_OSD_START 3

#define COCOSD_WIDTH 2
#define COCOSD_START 64
#define COCOSD_OSD_X_ADDR 48 + 50 + 20
#define COCOSD_OSD_Y_ADDR 64 + 50
#define COCOSD_HEIGHT 20

#define Title_Main_OSD 0x01
#define VIDEO_MODE 0x90
#define VIDEO_MODE1 0x91
#define VIDEO_MODE2 0x92
#define VIDEO_MODE3 0x93
#define VIDEO_USER 0x94
#define VIDEO_CONTRAST 0x95
#define VIDEO_BRIGHTNESS 0x96
#define VIDEO_SATURATION 0x97
#define VIDEO_HUE 0x99
#define VIDEO_SHARPNESS 0x9a
#define VIDEO_BACKLIGHT 0x9c // add new value
#define RESET_VIDEOVALUE 0x9b
#define RESET_DVRVALUE 0x9e
#define RESET_MonitorVALUE 0x70

#define TRIGGER_CamADelay 0xb2
#define TRIGGER_CamBDelay 0xb3
#define TRIGGER_CamCDelay 0xb4
#define TRIGGER_CamRDelay 0xb5

#define DayChoice 0xb8
#define CAMOSDONOFF 0xb9
#define GAUGEChoice 0xba
#define OsdLockChoice 0xbb
#define PRIORITYChoice 0xcA
#define DVRDisplay 0xcc

#define AutoSleeping 0xbc
#define TripleScreenType 0xbd
#define DistanceGaugePosition 0xbe
#define DistanceGaugeWidth 0xbf

#define CAMAONOFF 0xc1
#define CAMBONOFF 0xc2
#define CAMCONOFF 0xc3
#define CAMRONOFF 0xc4

#define CAMDONOFF 0x54

#define IMAGEChoice 0xc5
#define REARChoice 0xc6
#define JUMPChoice 0xc7
#define CAMOUTChoice 0xc8
#define STIME_STEP 0xc9
#define TriggerVGA 0xcb
#define IMAGE_A_Choice 0xcd
#define IMAGE_B_Choice 0xce
#define IMAGE_C_Choice 0xcf

#define DELAYCAMA 0xd0
#define DELAYCAMB 0xd1
#define DELAYCAMC 0xd2
#define DELAYCAMR 0xd3
#define DELAYCAMD 0xd4

#define VIDEO_R_B_GAIN 0x3B
#define VIDEO_G_B_GAIN 0x3C
#define VIDEO_B_B_GAIN 0x3D
#define VIDEO_R_C_GAIN 0x3E
#define VIDEO_G_C_GAIN 0x3F
#define VIDEO_B_C_GAIN 0x40

#define MenutouchChoice 0x41

#define TIRE1_High_PSI 0x42
#define TIRE1_Low_PSI 0x43
#define TIRE1_High_Temp 0x44

#define TIRE2_High_PSI 0x45
#define TIRE2_Low_PSI 0x46
#define TIRE2_High_Temp 0x47

#define TIRE3_High_PSI 0x48
#define TIRE3_Low_PSI 0x49
#define TIRE3_High_Temp 0x4A

#define TIRE4_High_PSI 0x4B
#define TIRE4_Low_PSI 0x4C
#define TIRE4_High_Temp 0x4D

#define TIRE5_High_PSI 0x4E
#define TIRE5_Low_PSI 0x4F
#define TIRE5_High_Temp 0x50

#define TIRE6_High_PSI 0x51
#define TIRE6_Low_PSI 0x52
#define TIRE6_High_Temp 0x53

#define Grabber_Liner 0x60
#define Grabber_Liner_Width 0x61
#define Grabber_Liner_Position 0x62

#define COC_Action 0x63

//-------------OSDMENU_TITLEWIN------------------------------------------------------
// Window Start address
#define OSDMENU_MAINADDR 64
//#define OSDMENU_SUBADDR     OSDMENU_MAINADDR+15    //25*9:196
//#define OSDMENU_SUBADDR    	OSDMENU_MAINADDR+HS_WIDTH //0+40   //25*9:196
//#define OSDMENU_SUBADDR    	OSDMENU_MAINADDR+10 //0+40   //25*9:196
#define OSDMENU_SUBADDR 64 //	OSDMENU_MAINADDR + 14 //0+40   //25*9:196  // Weylis 121008 �M�� Icon �᭱

#define OSDMENU_BARADDR 200 // OSDMENU_SUBADDR+225    //256   //25x2:50
#define OSDMENU_CURSORADDR 64 + (16)

//#ifdef WQVGA
//#define INPUTINFO1_ADDR		INPUTINFO_ADDR+17	//17x3:51
//#else
//#define INPUTINFO1_ADDR		INPUTINFO_ADDR+34	//17x2:51
//#endif

//#define MUTEINFO_ADDR 		INPUTINFO1_ADDR+51	// 4x1:4
//#define TVCHINFO_ADDR  		MUTEINFO_ADDR//324	// 3X1:3
#define LOGO_ADDR 0
#define RATINGINFO_ADDR 0				   // addr:210(8x1:8)
#define PCMEASUREINFO_ADDR OSDMENU_BARADDR // 10x1

/*
struct DefineMenu {
		BYTE						MenuLevel;
//  		struct DefineMenu	CODE	*Child;
		BYTE						Type;				// MENU, CHOICEMENU, BAR, NUMBER, TIME, ACTION, ENUM_STRING
		BYTE						ActiveWithCursor;
		BYTE						Id;
		//BYTE				CODE	*TypeData;	//MenuType, BarType, NumType,,,

} ;
*/
// CODE struct DefineMenu DMPicture[]={0};
/*
CODE BYTE PictureMenu[]=
			{//3,
			MENU_WIDTH,		0,
			TITLE_COLOR,
			CURSOR_COLOR,
};
*/
// DefineMenu DMMainMenu[]={    {Title_Main_OSD,  /*  DMPicture,*/     OSDMENU,     NO,  VIDEOPICTURE/* PictureMenu */}};
// };
#if 0
CODE BYTE TV_ICON[]={		RAMFONT, COLORFONT+0x0c,	COLORFONT+0x0f, 0};
CODE BYTE INPUT_ICON[]={	RAMFONT, COLORFONT+0x12,	COLORFONT+0x15, 0};
CODE BYTE VIDEO_ICON[]={	RAMFONT, COLORFONT+0x00,	COLORFONT+0x03, 0};
CODE BYTE UTIL_ICON[]={	RAMFONT, COLORFONT+0x18,	COLORFONT+0x1b, 0};





struct DefineMenu {
		//WORD						Icon[ICON_BUF_SZ];	
		BYTE				CODE	*Icon;
		BYTE						**Desc; 			
		BYTE						PreFnId, 
									PostFnId;
	//	void		(*CalltoFunction)(void);					// Un/Dn/Sel Function
		BYTE						IconColor;			//NIL: not to display icon
		BYTE						DescColor;			//NIL: not to display desc. (IconColor==NIL && DescColor==NIL)->Last Item
		struct DefineMenu	CODE	*Child;
		BYTE						Type;				// MENU, CHOICEMENU, BAR, NUMBER, TIME, ACTION, ENUM_STRING
		BYTE						ActiveWithCursor;
		BYTE						Id;
		BYTE				CODE	*TypeData;	//MenuType, BarType, NumType,,,

} ;

//Common
//CODE BYTE NumberOnly[]=	{0};
//CODE BYTE DefaultBar[]=	{0};
#define PICTURE_H 4
CODE BYTE PictureMenu[]=
			{//3,
			MENU_WIDTH,		PICTURE_H,
			TITLE_COLOR,
			CURSOR_COLOR,
};

CODE BYTE InputSourceMenu[]=
			{	//3,
				MENU_WIDTH,		PICTURE_H,
				TITLE_COLOR,
				CURSOR_COLOR,
};

CODE struct DefineMenu DMPicture[1][]={
#if 0
		{	{0},												Str_Picture,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			0,	
			ENUM_STRING,			NO,
			VIDEO_MODE,
			ModeChoice,
		},
#endif
/*		
		{	{0},								Str_Brightness,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			0,
			BAR,////NUMBER,	
			NO,
			VIDEO_BRIGHTNESS,
			DefaultBar
		},

		{	{0},								Str_Contrast,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			0,
			BAR,	
			NO,
			VIDEO_CONTRAST,
			DefaultBar
		},

		{	{0},									Str_Saturation,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			0,	
			BAR,	
			NO,
			VIDEO_SATURATION,
			DefaultBar
		},
		{	{0},										Str_Hue,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			0,	
			BAR,	
			NO,
			VIDEO_HUE,
			DefaultBar
		},
		{	{0},									Str_Sharpness,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			0,	
			BAR,	
			NO,
			VIDEO_SHARPNESS,
			DefaultBar
		},
        {   
            {0},                     //ICON��m                     
            Str_RETURN,              //���"RETRUN"�r��             
            NIL,                     //�W��ID                       
            NIL,                     //�U��ID                       
            NO_ICON_COLOR,           //ICON���Flag                 
            DESC_COLOR,              //�r�����Flag                 
            0,                       //�ثeOSD�����e(�ĤT�hOSD)     
            ACTION,                  //�ثeOSD���A(�]�w�ʧ@)     
            NO,                                                     
            TOHIGHMENU               //�ʧ@(�^��W�@��)             
        },                           
		
*/

#if 0
		{	{0},									Str_Backlight,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			0,	
			BAR,	NO,
			VIDEO_BACKLIGHT,
			DefaultBar
		},
#endif
		{	{0},												0,//Terminator
			0,				0,		
			NIL,
			NIL,
			0,
			0,				0,
		},
};
CODE struct DefineMenu DMINPUTSOURCE[1][]={
#if 0
        /////////////>>>>>>>>>>>>>>>>>>>>>>>andy AWT8.4 980811
#ifdef QUAD
	    {   {0},                     //ICON��m                                          
            Str_Auto_Day_Night,      //���"Auto Day & Night"�r�� 
            NIL,                     //�W��ID                     
            NIL,                     //�U��ID                                                     
            NO_ICON_COLOR,           //ICON���Flag                                                       
            DESC_COLOR,              //�r�����Flag                                                       
            0,                       //�ثeOSD�����e(�ĤT�hOSD)                                           
            ENUM_STRING,             //�ثeOSD���A(�r��)
            NO, 
            DayChoice,               //�ʧ@(�^��W�@��)
#ifdef SUPPORT_ML072Q_AUTO_DAY_NIGHT // Pinchi 20150825 add  //AWT_ML072Q  //Pinchi 20140903 add for AWT ML072Q
			ONOFFChoiceString,       //
#else
            DayChoiceString,       //
#endif
        },
#endif
                                                          
        {   {0},                     //ICON��m                       
            Str_DISPLAY,             //���"Display"�r��               
            NIL,                     //�W��ID                         
            NIL,                     //�U��ID                         
            NO_ICON_COLOR,           //ICON���Flag                   
            DESC_COLOR,              //�r�����Flag                   
            0,                       //�ثeOSD�����e(�ĤT�hOSD)
            ENUM_STRING,             //�ثeOSD���A(�r��)
            NO,                                                 
            CAMOSDONOFF,             //�ʧ@(�^��W�@��)         
            ONOFFChoiceString,       //                         
        },

#ifdef SUPPORT_SDCARD	
	{   {0},                     //ICON��m                       
            Str_DVR_DISPLAY,             //���"Display"�r��               
            NIL,                     //�W��ID                         
            NIL,                     //�U��ID                         
            NO_ICON_COLOR,           //ICON���Flag                   
            DESC_COLOR,              //�r�����Flag                   
            0,                       //�ثeOSD�����e(�ĤT�hOSD)
            ENUM_STRING,             //�ثeOSD���A(�r��)
            NO,                                                 
            DVRDisplay,             //�ʧ@(�^��W�@��)         
            ONOFFChoiceString,       //                         
        },
#endif

        {   {0},                     //ICON��m                       
            Str_DISTANCE,            //���"DISTANCE GAUGE"�r��               
            NIL,                     //�W��ID                         
            NIL,                     //�U��ID                         
            NO_ICON_COLOR,           //ICON���Flag                   
            DESC_COLOR,              //�r�����Flag                   
            0,                       //�ثeOSD�����e(�ĤT�hOSD)
            ENUM_STRING,             //�ثeOSD���A(�r��) 
            NO,                                                 
            GAUGEChoice,             //�ʧ@(�^��W�@��)         
            ONOFFChoiceString,       //                         
        },

#ifdef AWT_ML072S
	    {   {0},                     //ICON��m                                          
            Str_Auto_Day_Night,      //���"Auto Day & Night"�r�� 
            NIL,                     //�W��ID                     
            NIL,                     //�U��ID                                                     
            NO_ICON_COLOR,           //ICON���Flag                                                       
            DESC_COLOR,              //�r�����Flag                                                       
            0,                       //�ثeOSD�����e(�ĤT�hOSD)                                           
            ENUM_STRING,             //�ثeOSD���A(�r��)
            NO, 
            DayChoice,               //�ʧ@(�^��W�@��)                         
            DayChoiceString,       //   
        },
#endif

#ifdef HTRV
	////_Anderson_20140310_add_xx_B
		{   {0},                     //ICON��m                       
            Str_GAUGE_STEP,            //���"DISTANCE GAUGE"�r��               
            NIL,                     //�W��ID                         
            NIL,                     //�U��ID                         
            NO_ICON_COLOR,           //ICON���Flag                   
            DESC_COLOR,              //�r�����Flag                   
            0,                       //�ثeOSD�����e(�ĤT�hOSD)
            GRAPH,////ACTION,             //�ثeOSD���A(�r��) 
            NO,                                                 
            Gauge_VStart,             //�ʧ@(�^��W�@��)         
            DefaultBar ,       //                         
        },
	////_Anderson_20140310_Disable_xx_E
#endif

        {   {0},                     //ICON��m                       
            Str_OSD_LOCK,            //���"OSD LOCK"�r��               
            NIL,                     //�W��ID                         
            NIL,                     //�U��ID                         
            NO_ICON_COLOR,           //ICON���Flag                   
            DESC_COLOR,              //�r�����Flag                   
            0,                       //�ثeOSD�����e(�ĤT�hOSD)
            ENUM_STRING,             //�ثeOSD���A(�r��) 
            NO,                                                 
            OsdLockChoice,             //�ʧ@(�^��W�@��)         
            ONOFFChoiceString,       //                         
        },

#ifndef HTRV
#ifdef AWT_ML072Q
        {	 {0},					  //ICON��m					   
			 Str_PRIORITY,			  //���"PRIORITY"�r��				 
			 NIL,					  //�W��ID						   
			 NIL,					  //�U��ID						   
			 NO_ICON_COLOR, 		  //ICON���Flag				   
			 DESC_COLOR,			  //�r�����Flag				   
			 0, 					  //�ثeOSD�����e(�ĤT�hOSD)
			 ENUM_STRING,			  //�ثeOSD���A(�r��) 
			 NO,												 
			 PRIORITYChoice,			  //�ʧ@(�^��W�@��)		 
			 ONOFFChoiceString, 	  //						 
	    },
#endif

	    //#ifdef Func_ReversMode
		{  {0},					   //ICON��m						
		   Str_ReversMode,			 //���"OSD LOCK"�r��				
		   NIL,					   //�W��ID 						
		   NIL,					   //�U��ID 						
		   NO_ICON_COLOR,		   //ICON���Flag					
		   DESC_COLOR,			   //�r�����Flag					
		   0,					   //�ثeOSD�����e(�ĤT�hOSD)
		   ENUM_STRING,			   //�ثeOSD���A(�r��) 
		   NO,												  
		   ReverseMode,			   //�ʧ@(�^��W�@��)		  
		   ReversModeString, 	  //						 
		 },
		 //#endif
#endif

#ifdef SUPPORT_RS485
		{	{0},					 //ICON��m 					   
		    Str_RS485,			 //���"RETRUN"�r�� 			   
   		    NIL,					  //�W��ID				 
   		    NIL,					  //�U��ID				 
   		    NO_ICON_COLOR,		  //ICON���Flag		 
   		    DESC_COLOR,			  //�r�����Flag		 
   		    RS485SELECT,		   //�ثeOSD�����e(�ĤT�hOSD)	  
   		    OSDMENU,//CHOICEMENU, 			//�ثeOSD���A				   
   		    NO,											 
   		    0,//INPUTSELECTION,			   //<william-961004-ver1.41>				   
   		   RS485SelectMenu 	   //�ʧ@Flag				   
		},
#endif
			
		{	{0},                     //ICON��m                     
            Str_RETURN,              //���"RETRUN"�r��             
            NIL,                     //�W��ID                       
            NIL,                     //�U��ID                       
            NO_ICON_COLOR,           //ICON���Flag                 
            DESC_COLOR,              //�r�����Flag                 
            0,                       //�ثeOSD�����e(�ĤT�hOSD)     
            ACTION,                  //�ثeOSD���A(�]�w�ʧ@)     
            NO,                                                     
            TOHIGHMENU               //�ʧ@(�^��W�@��)             
        },             

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<andy AWT8.4 980811

#if 0 
		{	{0},											Str_Input,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			DMINPUTSELECT,
			CHOICEMENU,			NO,
			INPUTSELECTION,
			InputSelectMenu
		},

		{	{0},										Str_Information,
			NIL,	NIL,	
			NO_ICON_COLOR,
			DESC_COLOR,
			0,
			ACTION,		NO,
			DISP_INFO ,
		},
#endif
#endif
		{	{0},												0,//Terminator
			0,				0,		
			NIL,
			NIL,
			0,
			0,				0,
		},
};

CODE struct DefineMenu DMMainMenu[3][]={

	    {
			TV_ICON,                //Display��ICON
            0,//Str_Picture,           //��ܦr��
            NIL,                    //�W��ID
            NIL,                    //�U��ID
            TOPICON_COLOR,          //ICON���Flag(ON)
            NIL,                    //�r�����Flag(OFF)
            DMPicture,              //�ثeOSD�����e(�ĤG�hOSD)
            OSDMENU,                //�ثeOSD���A
            NO,
            VIDEOPICTURE,
            PictureMenu
        },

		{	INPUT_ICON,
		    0,
			NIL,
			NIL,
			TOPICON_COLOR,
			NIL,
			DMINPUTSOURCE,
			OSDMENU,
			NO,
			0,
			InputSourceMenu
		},
/*
		{	VIDEO_ICON,
		    0,
			NIL,
			NIL,
			TOPICON_COLOR,
			NIL,
			DMCAMERA,
			OSDMENU,
			NO,
			VIDEOPICTURE,
			CAMERAMenu
		},


		{	UTIL_ICON,
		    0,
			NIL,
			NIL,
			TOPICON_COLOR,
			NIL,
			DMUtility,
			OSDMENU,
			NO,
			0,
			UtilityMenu
		},

*/
		{	0,
			0,				//Terminator
			0,
			0,
			NIL,
			NIL,
			0,
			0,
			0,
		},
};

#endif
#endif
