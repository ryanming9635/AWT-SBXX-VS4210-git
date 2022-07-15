/*
//#include <stdio.h>
//#include "82Gxx.h"
#include "Config.h"
#include "reg.h"
#include "typedefs.h"
#include "main.h"
//#include "menu.h"
#include "tw88.h"    
#include "i2c.h"
#include "adc.h"
#include "osdbasic.h"
#include "dispinfo.h"
#include "debug.h"
#include "etc_eep.h"
#include "measure.h"
#include "Printf.h"
#include "audio.h"
#include "osdmenu.h"
#include "pc_eep.h"
#include "KeyRemo.h"
#include "Monitor.h"
#include "CPU.h"
#include "HS_keyremo.h"
#include "HS_DisplayOSD.h"
#include "rgbmix.h"			
#include "panel.h"
#include "HS_IOdef.h"
#include "HS_DVRProtocol.h"
#include "TD5300.h"
#include "panel.h"
*/
//#define BYTE unsigned char
//#include "main.h"
//#define BYTE char
//#define WORD unsigned int

#include "VXIS_Extern_Define.h"
#include "TD5300.h"
#include "main.h"
#include "VX_OSD_Simple.h"
#include "VX_swi2c.h"

#include "VS4210_param_Mode88.h"
#include "VS4210_param_Modex26.h"

#include "keyremo.h"
#include "tpinc/TP2802.h"
#include "Printf.h"
#include <stdlib.h>

//////////////////////////////////

#if (TPMSOSD==ON)
BYTE TP_HIGH_PSI[6]={0};
BYTE TP_LOW_PSI[6]={0};
BYTE TP_HIGH_TEMP[6]={0};
#endif

#if 1
extern CODE BYTE Hex[16];
extern DATA BYTE Key;
extern bit KeyReady,RepeatKey;
extern bit RS2_Xbusy;

CODE BYTE Ascii[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
CODE BYTE Device_Mask[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};


BYTE  cursor_p=0,change_keyon=0;
#if 0
BYTE *empty=" ";
BYTE *TPMS="TPMS information";
BYTE *Hex_p="112233445566";
 #endif
//BYTE SR_Temp[3]={'1','2','3'};
//BYTE SR_Press[3]={'4','5','6'};
BYTE sb_data[3]={0,0,0};
BYTE sb_tp_data[12]={60,30,60,30,60,30,60,30,60,30,60,30};
BYTE sb_p_data_max[6]={45,45,45,45,45,45};

BYTE buf_tpms[99];

BYTE TPMS_P=0;
BYTE  TPMS_Ready=0;
BYTE  TPMS_Start=0;
BYTE  TPMS_Count=0;
BYTE  TPMS_val=0;
BYTE  TPMS_Relay_Battery=0;

    
BYTE TPMS_Flag=0;
BYTE TPMS_Pop_Flag=0;
//BYTE TPMS_Mune_Pop_Flag=0;

BYTE TPMS_Cursor=0;
BYTE TPMS_Setting_Mode=0;
   
BYTE loop_flag=0;
BYTE TPMS_OSD=0;
BYTE SB_Temper_Error_Flag=0x00;
BYTE SB_Press_Error_Flag=0x00;
BYTE SB_Work_Error_Flag=0x00;
BYTE SB_Work_Flag=0x00;
BYTE SB_Work_Flag_F=0x00;
BYTE TPMS_LED=0;
BYTE TPMS_Reset_Flag=0;

extern BYTE PowerFlag;

#endif
///////////////////////////////// add by ryan 20140812.
#define FORNT_LEFT_TEMPTURE 72
#define FORNT_LEFT_PRESSURE 82
#define REAR_LEFT_TEMPTURE 112
#define REAR_LEFT_PRESSURE 122
#define REAR2_LEFT_TEMPTURE 117
#define REAR2_LEFT_PRESSURE 127
#define FORNT_RIGHT_TEMPTURE 177
#define FORNT_RIGHT_PRESSURE 187
#define REAR_RIGHT_TEMPTURE 212
#define REAR_RIGHT_PRESSURE 222
#define REAR2_RIGHT_TEMPTURE 217
#define REAR2_RIGHT_PRESSURE 227

#define FORNT_LEFT_TIRE 22
#define FORNT_RIGHT_TIRE 27
#define REAR_LEFT_TIRE 40
#define REAR_RIGHT_TIRE 45
#define REAR_LEFT2_TIRE 41
#define REAR_RIGHT2_TIRE 44

/////////////////////////////////
/*
BYTE Sensor0[15];
BYTE Sensor1[15];
BYTE Sensor2[15];
BYTE Sensor3[15];
BYTE Sensor4[15];
BYTE Sensor5[15];

BYTE Sensor_state[15];
*/
#if 0
        ////////////////////////add by ryan 20140805_start.
        //////////////////////////////////////----- Title Window
CODE BYTE Init_Osd[] = {
	17,
	OSD_Win_Num			( BACK_COLOR|OSDMENU_TOPWIN ),	// 2 bytes
	OSD_Win_Attr			( WIN_BG_COLOR_EX0 | BACK_COLOR | WIN_3D_TYPE0 | WIN_3D_EN0 | WIN_EN0 ),
	OSD_Win_Start			( (OSDHSTART_MAX/2-85-65+80), ((OSDVSTART_MAX/2)-84) ), // 3 bytes
	OSD_Win_Width			(16 ),
	OSD_Win_Height			( 1),
	OSD_Win_Border			( WIN_BD_EN | OSDMENU_OUTLINE_COLOR | WIN_BD_WIDTH(0) ),
	OSD_Win_HBorder			( WIN_BD_COLOR_EX | WIN_H_BD_W(0x00) ),
	OSD_Win_VBorder			( WIN_V_BD_W(0x00) ),
	OSD_Win_CH_Space		( WIN_CH_HSPACE(0) | WIN_CH_VSPACE(0x00) ),
	OSD_Win_ZM_SAddr		( WIN_VZOOM(WINZOOMx3), WIN_HZOOM(WINZOOMx3), WIN_SADDR(OSDMENU_MAINADDR) ),
      OSD_Win_Shadow			( WIN_SHADOW_EN | BG_COLOR_BLACK | WIN_SHADOW_W(0) ),
	OSD_Win_Blending		( 0x0f ),
//	OSD_Win_Blending		( 0x0f ),
	OSD_Win_CH_BorderShadow	( WIN_SHADOW_COLOR_EX0 | WIN_MULTI_EN | WIN_V_EXT_EN0 
	                        | WIN_CH_BDSH_SEL(SEL_BORDER) 
	                        | WIN_CH_BDSH_COLOR_EX0 | CH_COLOR_BLACK ),


//#if HS_NEWOSDMENU_2
//#ifdef HS_NEWOSDMENU
	/////////////////////////////////////----- Car Window
	17,                           
	//OSD_Win_Num					( BACK_COLOR|OSDMENU_TITLEWIN ),	// 2 bytes
	OSD_Win_Num					( BACK_COLOR|OSDCURSORWINDOW ),	// 2 bytes

//#if 1	
	OSD_Win_Attr				( WIN_BG_COLOR_EX0 | TITLE_BACK_COLOR | WIN_3D_TYPE0 | WIN_3D_EN0 | WIN_EN0 ),
//#else	
//	OSD_Win_Attr				( WIN_BG_COLOR_EX0 | DEFAULT_COLOR | WIN_3D_TYPE0 | WIN_3D_EN0 | WIN_EN0 ),
//#endif	
	 OSD_Win_Start			( ((OSDHSTART_MAX/2-85-65+3+120+120+15+10)), (OSDVSTART_MAX/2-20-60+60+30) ), // 3 bytes       
	//OSD_Win_Start				( ((OSDHSTART_MAX/2)+66+14+2), ((OSDVSTART_MAX/2)-60) ), // 3 bytes
	OSD_Win_Width				( 6 ), 
	OSD_Win_Height				( 6 ), 
	OSD_Win_Border				( WIN_BD_EN0 | OSDMENU_OUTLINE_COLOR | WIN_BD_WIDTH(0) ),
	OSD_Win_HBorder				( WIN_BD_COLOR_EX | WIN_H_BD_W(0) ),
	OSD_Win_VBorder				( WIN_V_BD_W(0) ),
	OSD_Win_CH_Space			( WIN_CH_HSPACE(0) | WIN_CH_VSPACE(0) ),
	
//#ifdef TW8816
	//OSD_Win_ZM_SAddr			( WIN_VZOOM(WINZOOMx1), WIN_HZOOM(WINZOOMx1), WIN_SADDR(OSDMENU_SUBADDR) ),	//Weylis
	OSD_Win_ZM_SAddr			( WIN_VZOOM(WINZOOMx3), WIN_HZOOM(WINZOOMx3), WIN_SADDR(OSDMENU_SUBADDR+2) ),	//Weylis
//#else
	//OSD_Win_ZM_SAddr			( WIN_VZOOM(WINZOOMx1), WIN_HZOOM(WINZOOMx2), WIN_SADDR(OSDMENU_SUBADDR) ),	//25=MENU_WIDTH
//    OSD_Win_ZM_SAddr			( WIN_VZOOM(WINZOOMx3), WIN_HZOOM(WINZOOMx3), WIN_SADDR(OSDMENU_SUBADDR+3) ),	//25=MENU_WIDTH

//#endif

	OSD_Win_Shadow				( WIN_SHADOW_EN0 | BG_COLOR_BLACK | WIN_SHADOW_W(0) ),
	OSD_Win_Blending				( 0x0f ),
	//OSD_Win_Blending			( 0x05 ),
	OSD_Win_CH_BorderShadow		( WIN_SHADOW_COLOR_EX0 | WIN_MULTI_EN | WIN_V_EXT_EN0 | WIN_CH_BDSH_SEL(SEL_BORDER) | 
									WIN_CH_BDSH_COLOR_EX0 | CH_COLOR_BLACK ),

	////////////////////////////////////----- Left_Numbers Window 
	17,
	OSD_Win_Num				( BACK_COLOR|OSDMENUWINDOW ),	// 2 bytes
	OSD_Win_Attr			( WIN_BG_COLOR_EX0 | BACK_COLOR | WIN_3D_TYPE0 | WIN_3D_EN0 | WIN_EN0 ),
//#ifdef E_Wintek
//	OSD_Win_Start			( ((OSDHSTART_MAX/2-145)), (OSDVSTART_MAX/2-60) ), // 3 bytes
//#else
	OSD_Win_Start			( ((OSDHSTART_MAX/2-85-65+3+10)), (OSDVSTART_MAX/2-20+7) ), // 3 bytes
	//OSD_Win_Start			( ((OSDHSTART_MAX/2-85)), (OSDVSTART_MAX/2-10) ), // 3 bytes
	//OSD_Win_Start			( ((OSDHSTART_MAX/2)+66+14+2), (OSDVSTART_MAX/2+28) ), // 3 bytes
//#endif
	//OSD_Win_Width			( MENU_WIDTH+8 ), //MENU_WIDTH
	//OSD_Win_Height			( MENU_HEIGHT-1),	// MENU_HEIGHT
	OSD_Win_Width			(10 ), //MENU_WIDTH
	OSD_Win_Height			( 10),	// MENU_HEIGHT


     // OSD_Win_Border			( WIN_BD_EN | OSDMENU_OUTLINE_COLOR | WIN_BD_WIDTH(1) ),
    //OSD_Win_HBorder			( WIN_BD_COLOR_EX | WIN_H_BD_W(0x0c+8) ),
    //OSD_Win_VBorder			( WIN_V_BD_W(0x16+1) ),
      OSD_Win_Border			( WIN_BD_EN | OSDMENU_OUTLINE_COLOR | WIN_BD_WIDTH(0) ),
	OSD_Win_HBorder			( WIN_BD_COLOR_EX | WIN_H_BD_W(0) ),
	OSD_Win_VBorder			( WIN_V_BD_W(0) ),


OSD_Win_CH_Space		( WIN_CH_HSPACE(0) | WIN_CH_VSPACE(0x00) ),

//#ifdef E_Wintek	
//	OSD_Win_ZM_SAddr		( WIN_VZOOM(WINZOOMx3), WIN_HZOOM(WINZOOMx2), WIN_SADDR(OSDMENU_SUBADDR+MENU_WIDTH) ),	//25=MENU_WIDTH// 2 bytes
//#else
	//OSD_Win_ZM_SAddr		( WIN_VZOOM(WINZOOMx2), WIN_HZOOM(WINZOOMx2), WIN_SADDR(OSDMENU_SUBADDR+MENU_WIDTH) ),	//25=MENU_WIDTH// 2 bytes
	OSD_Win_ZM_SAddr		( WIN_VZOOM(WINZOOMx2), WIN_HZOOM(WINZOOMx2), WIN_SADDR(52) ),	//25=MENU_WIDTH// 2 bytes
//#endif
	//OSD_Win_Shadow			( WIN_SHADOW_EN0 | BG_COLOR_BLACK | WIN_SHADOW_W(8) ),
	OSD_Win_Shadow			( WIN_SHADOW_EN0 | BG_COLOR_BLACK | WIN_SHADOW_W(0) ),

	OSD_Win_Blending	( 0x0f ),
	//	OSD_Win_Blending	( 0x0f ),

	OSD_Win_CH_BorderShadow	( WIN_SHADOW_COLOR_EX0 | WIN_MULTI_EN | WIN_V_EXT_EN0 
	                        | WIN_CH_BDSH_SEL(SEL_BORDER) 
	                        | WIN_CH_BDSH_COLOR_EX0 | CH_COLOR_BLACK ),



	/////////////////////////////////////----- Right_Numbers Window.
	17,
	OSD_Win_Num				( BACK_COLOR|OSDMENU_TITLEWIN ),	// 2 bytes
	OSD_Win_Attr			( WIN_BG_COLOR_EX | BACK_COLOR | WIN_3D_TYPE0 | WIN_3D_EN | WIN_EN0 ),
//#ifdef E_Wintek
//	OSD_Win_Start			( ((OSDHSTART_MAX/2)-145+8), ((OSDVSTART_MAX/2)-80)+10 ), // 3 bytes
//#else
//#ifdef HS_NEWOSDMENU
        OSD_Win_Start			( ((OSDHSTART_MAX/2-85-65+3+485+10)), (OSDVSTART_MAX/2-20+7) ), // 3 bytes
	//OSD_Win_Start			( (OSDHSTART_MAX/2-85), ((OSDVSTART_MAX/2)-84) ), // 3 bytes
//#else	
//	OSD_Win_Start			( ((OSDHSTART_MAX/2)+7+14+1), ((OSDVSTART_MAX/2)+4+28+2) ), // 3 bytes
//#endif	
//#endif
	OSD_Win_Width			( 10 ),
	OSD_Win_Height			( 10),
	OSD_Win_Border			( WIN_BD_EN | BG_COLOR_WHITE | WIN_BD_WIDTH(0) ),
	OSD_Win_HBorder			( WIN_BD_COLOR_EX0 | WIN_H_BD_W(0) ),
	OSD_Win_VBorder			( WIN_V_BD_W(0) ),
	OSD_Win_CH_Space		( WIN_CH_HSPACE(0) | WIN_CH_VSPACE(0) ),
//#ifdef E_Wintek
//	OSD_Win_ZM_SAddr		( WIN_VZOOM(WINZOOMx4), WIN_HZOOM(WINZOOMx4), WIN_SADDR(OSDMENU_MAINADDR) ),	// 2 bytes
//#else
	OSD_Win_ZM_SAddr		( WIN_VZOOM(WINZOOMx2), WIN_HZOOM(WINZOOMx2), WIN_SADDR(152) ),	// 2 bytes
//#endif
	OSD_Win_Shadow			( WIN_SHADOW_EN0 | BG_COLOR_BLACK | WIN_SHADOW_W(0) ),
	//OSD_Win_Blending		( 3 ),
		OSD_Win_Blending		(0x0f ),
	OSD_Win_CH_BorderShadow	( WIN_SHADOW_COLOR_EX | WIN_MULTI_EN | WIN_V_EXT_EN0 
	                        | WIN_CH_BDSH_SEL(SEL_SHADOW) 
	                        | WIN_CH_BDSH_COLOR_EX0 | CH_COLOR_BLACK ),

	0 // End of Data
	
///////////////////////////add by ryan 20140805_End.
 
};
#endif


void TPMS_redraw(BYTE tp)
{
#if 0
BYTE tire_p[13]={22,22,27,27,40,40,41,41,44,44,45,45,252};
BYTE cursor[13]={72,82,177,187,112,122,117,127,212,222,217,227,250};

BYTE *OK="OK";
//BYTE *set="Setting mode:   ";
BYTE *t_set="Temperature Set:";
BYTE *p_set="Pressure Set:   ";
BYTE x=0;

ClearDataCnt(52, 247);
DrawAttrCnt(52,(CH_COLOR_WHITE|BG_COLOR_CYAN), 247);
   
if(tp==0)
{
            for(x=0 ;x<=5;x++)
            {
             WriteStringToAddr(cursor[x+(x*1)], DataToAscii(sb_tp_data[x+(x*1)]), 3);  //temp
              WriteStringToAddr(cursor[x+(x*1)]+3,Temp,2);   //unit   
           
              
            }
             WriteStringToAddr(0, t_set,16); //title string..

             //WriteStringToAddr(52,L_arrow,2);  
}
else
{
       for(x=0 ;x<=5;x++)
            {
             WriteStringToAddr(cursor[x+(x*1)], DataToAscii(sb_p_data_max[x]), 3);  //temp
             WriteStringToAddr(cursor[x+(x*1)]+3,PSI,4);   //unit        
           
             WriteStringToAddr(cursor[(x+1)+(x*1)], DataToAscii(sb_tp_data[(x+1)+(x*1)]),3);  //temp
             WriteStringToAddr(cursor[(x+1)+(x*1)]+3,PSI,4);   //unit        
            }
       
             WriteStringToAddr(0, p_set,16); //title string..

             //WriteStringToAddr(161,R_arrow,2);  
}
       
  WriteStringToAddr(cursor[12], OK, 2);   //press  
   //WriteStringToAddr(0, set,16); //title string..
                    
WriteStringToAddr(22,&Hex[1],1);
WriteStringToAddr(27,&Hex[2],1);
WriteStringToAddr(40,&Hex[3],1);
WriteStringToAddr(41,&Hex[4],1);
WriteStringToAddr(44,&Hex[5],1);
WriteStringToAddr(45,&Hex[6],1);   

 #endif

}




BYTE TPMS_Keyin_Mode(BYTE val)
{
#if 0
BYTE tire_p[13]={22,22,27,27,40,40,41,41,44,44,45,45,252};
BYTE cursor[13]={72,82,177,187,112,122,117,127,212,222,217,227,250};

//BYTE tire_p[13]={ 22,22,27,27,40,40,41,41,44,44,45,45,252};
//BYTE cursor[13]={72,82,177,187,112,122,117,127,212,222,217,227,249};
//BYTE  cursor_p=0;
//BYTE Action=1,x,y,z,ikey;
//BYTE *empty=" ";
//BYTE *TPMS="TPMS information";
//BYTE *Hex_p="112233445566";
//BYTE *pairing="$C010#";
//BYTE *end="$C02#";

BYTE x;


if(TPMS_Setting_Mode==0)
{

TPMS_redraw(0); //temperature page.
DrawAttrCnt(cursor[0],(CH_COLOR_WHITE|BG_COLOR_MAGENTA),5);
DrawAttrCnt(tire_p[0],(CH_COLOR_WHITE|BG_COLOR_MAGENTA),1);
cursor_p=0;
TPMS_Setting_Mode=1;
}
//else if(change_keyon==1)
//{
 // TPMS_Change_Mode(val);
 // return 0;
//}
//WriteStringToAddr(tire_p[0],Tire_Yellow,2);
 //while(Action==1)
 //{   		
  // switch (GetKey(1)) 
    DrawAttrCnt(cursor[cursor_p],(CH_COLOR_WHITE|BG_COLOR_CYAN), 5);
    DrawAttrCnt(tire_p[cursor_p],(CH_COLOR_WHITE|BG_COLOR_CYAN), 1);

                     switch (val) 
                    {
          
			//case MENUKEY: 
                   case 2:
                                        //Printf("\r\nMENU key pressed");                                      
                                                                            
						if(cursor_p==12) 
						    {
                                          DrawAttrCnt(cursor[cursor_p],(CH_COLOR_WHITE|BG_COLOR_CYAN), 3);
						    for(x=0;x<=199;x++)  WriteStringToAddr(52+x, empty, 1);   //press  
                                            //Action=0;
                                            TPMS_Setting_Mode=0;
                                            WriteStringToAddr(22,Tire_None,2);
                                            WriteStringToAddr(27,Tire_None,2);
                                            WriteStringToAddr(40,Tire_None,2);
                                            WriteStringToAddr(41,Tire_None,2);
                                            WriteStringToAddr(44,Tire_None,2);
                                            WriteStringToAddr(45,Tire_None,2);
                       
                                            WriteStringToAddr(0, TPMS,16); //title string..
                                           // TPMS_OSD_OFF();
                                           TPMS_Write_eeprom();
						    }
                                         
                        
                                         break;
			//case UPKEY:	  
			case 1:	  
                                           WriteStringToAddr(tire_p[cursor_p],(Hex_p+cursor_p),1);
                                           DrawAttrCnt(cursor[cursor_p],(CH_COLOR_WHITE|BG_COLOR_CYAN), 5);
                                           DrawAttrCnt(tire_p[cursor_p],(CH_COLOR_WHITE|BG_COLOR_CYAN), 1);
                                         if(cursor_p==12)
                                            cursor_p=0;
                                         else if(TPMS_Setting_Mode==1)
                                            cursor_p+=2;
                                         else                                            
                                            cursor_p++;    
                                         DrawAttrCnt(cursor[cursor_p],(CH_COLOR_WHITE|BG_COLOR_MAGENTA), 5);
                                          DrawAttrCnt(tire_p[cursor_p],(CH_COLOR_WHITE|BG_COLOR_MAGENTA), 1);
                                        //WriteStringToAddr(tire_p[cursor_p],Tire_Yellow,2);
                                        //Printf("\r\nUP key pressed");  
                                          break;
			//case DOWNKEY:
			case 0:
                                       WriteStringToAddr(tire_p[cursor_p],(Hex_p+cursor_p),1);
                                       DrawAttrCnt(cursor[cursor_p],(CH_COLOR_WHITE|BG_COLOR_CYAN), 5);
                                       DrawAttrCnt(tire_p[cursor_p],(CH_COLOR_WHITE|BG_COLOR_CYAN), 1);
                                         if(cursor_p==0)
                                          cursor_p=12;
                                         else if(TPMS_Setting_Mode==1)                                           
                                           cursor_p-=2;
                                         else
                                           cursor_p--;                                                                                        
                                        DrawAttrCnt(cursor[cursor_p],(CH_COLOR_WHITE|BG_COLOR_MAGENTA), 5);
                                       DrawAttrCnt(tire_p[cursor_p],(CH_COLOR_WHITE|BG_COLOR_MAGENTA), 1);
                                         //WriteStringToAddr(tire_p[cursor_p],Tire_Yellow,2);
                                         //Printf("\r\nUP key pressed"); 
                                         break;
                    //Pressure page.
                     case 3:
                                        TPMS_redraw(1); 
                                        DrawAttrCnt(cursor[0],(CH_COLOR_WHITE|BG_COLOR_MAGENTA),5);
                                        DrawAttrCnt(tire_p[0],(CH_COLOR_WHITE|BG_COLOR_MAGENTA),1);
                                        cursor_p=0;
                                       TPMS_Setting_Mode=2;                                       
                                        break;
                     //Temperature page.
                     case 4:
                                        TPMS_redraw(0); 
                                        DrawAttrCnt(cursor[0],(CH_COLOR_WHITE|BG_COLOR_MAGENTA),5);
                                        DrawAttrCnt(tire_p[0],(CH_COLOR_WHITE|BG_COLOR_MAGENTA),1);
                                        cursor_p=0;
                                       TPMS_Setting_Mode=1;                                       
                                        break;


		    }   
#endif
  //  }
  
/*
WriteStringToAddr(22,Tire_None,2);
WriteStringToAddr(27,Tire_None,2);
WriteStringToAddr(40,Tire_None,2);
WriteStringToAddr(41,Tire_None,2);
WriteStringToAddr(44,Tire_None,2);
WriteStringToAddr(45,Tire_None,2);
TPMS_Setting_Mode=0;
WriteStringToAddr(0, TPMS,16); //title string..
*/
		return 0;   
}

void TPMS_Change_Mode(BYTE val)
{
#if 0
BYTE tire_p[13]={22,22,27,27,40,40,41,41,44,44,45,45,252};
BYTE cursor[13]={72,82,177,187,112,122,117,127,212,222,217,227,250};
BYTE val_temp=0;
//BYTE  cursor[13]={72,82,177,187,112,122,117,127,212,222,217,227,248};
//BYTE tire_p[13]={ 22,22,27,27,40,40,41,41,44,44,45,45,252};
if(change_keyon==0)
 {
  change_keyon=1;
  DrawAttrCnt(cursor[cursor_p],(CH_COLOR_GREEN|BG_COLOR_CYAN), 5);

if((TPMS_Setting_Mode==2)&&(cursor_p==0||cursor_p==2||cursor_p==4||cursor_p==6||cursor_p==8||cursor_p==10))
{
        TPMS_val=sb_p_data_max[(cursor_p/2)];
        WriteStringToAddr(cursor[cursor_p], DataToAscii(TPMS_val), 3);  
}
else
  {
        TPMS_val=sb_tp_data[cursor_p];
     WriteStringToAddr(cursor[cursor_p], DataToAscii(TPMS_val), 3); 
    }
 }

       //while(change_keyon)
        //{
         
          //switch (GetKey(1)) 
         switch (val) 
                 {
                  // case UPKEY:   
                      case 1:        

                                        if(TPMS_val==180)
                                           TPMS_val=1;  
                                        else
                                          TPMS_val++;
                                       
                            WriteStringToAddr(cursor[cursor_p], DataToAscii(TPMS_val), 3); 
                                        break;
                   //case DOWNKEY:                                         
                   case 0:                                         
                                        if(TPMS_val==0)
                                           TPMS_val=180;             
                                         else
                                            TPMS_val--;
                                         
                            WriteStringToAddr(cursor[cursor_p], DataToAscii(TPMS_val), 3); 
    
                                        break;
                    
			case 2:                                    
                          DrawAttrCnt(cursor[cursor_p],(CH_COLOR_WHITE|BG_COLOR_MAGENTA), 5);
                        change_keyon=0;
                               break;
		        }
    //}
if((TPMS_Setting_Mode==2)&&(cursor_p==0||cursor_p==2||cursor_p==4||cursor_p==6||cursor_p==8||cursor_p==10))
        sb_p_data_max[(cursor_p/2)]=TPMS_val;
else
        sb_tp_data[cursor_p]=TPMS_val;
           
    #endif   
}    





void TPMS_Read_eeprom(void)
{

   WORD x=0;
    for(x=0;x<=11;x++)    sb_tp_data[x]=ReadEEP(EEP_SB_data_offset+x);

}

void TPMS_Write_eeprom(void)
{

    WORD x=0;
    for(x=0;x<=11;x++)    WriteEEP(EEP_SB_data_offset+x,sb_tp_data[x]);
  for(x=0;x<=5;x++)    WriteEEP(EEP_SB_data_offset+x+12,sb_p_data_max[x]);

}

void TPMS_Default_Setting(void)
{

   WORD x=0;
    for(x=0;x<=11;x+=2)
        {
            sb_tp_data[x]=60;
            sb_tp_data[x+1]=30;
            //sb_p_data_max[x]=45;
        }

  for(x=0;x<=5;x++)    sb_p_data_max[x]=45;

  for(x=0;x<=11;x++)    WriteEEP(EEP_SB_data_offset+x,sb_tp_data[x]);
  for(x=0;x<=5;x++)    WriteEEP(EEP_SB_data_offset+x+12,sb_p_data_max[x]);

}
    
void Init_TPMS_OSD(void)          //add by ryan chen 20140728 
{
#if 0
  BYTE i=0;
  
if(TPMS_OSD==1) return;

  ShowTPMSOSD();

//  InitOSDWindow(Init_Osd);  // Define Window Top(4), Sub(3), Bar(2), Cursor(1) 
//  ClearDataCnt(OSDMENU_MAINADDR, 255);	// Clear the counter of OSD RAM Data from addr with space(' ').	
//  DrawAttrCnt(0,(CH_COLOR_WHITE|BG_COLOR_CYAN), 252);   
//  Draw_CAR(); 
  
   TPMS_OSD=1;
 #endif
}



void Display_TPMS_info(BYTE Vchar)
{
#if 0
BYTE x,y,z;
BYTE *module="TPMS was failure";
//Init_TPMS_OSD();
//ShowOSDWindowAll(1);
//SetDisplayedOSD( MENU);
//Check_tire_sensors();

/*
if((Get_Relay_Data('6',1))==0&&(TPMS_Ready==0))   //check relay board...
{
DrawAttrCnt(0,(CH_COLOR_RED|BG_COLOR_CYAN), 16); 
WriteStringToAddr(0, module, 16);
    
ShowOSDWindowAll(1);
WaitEndofDisplayArea();
ShowOSDWindow_TW88(3,TRUE);  
//TPMS_OSD=1;
TPMS_Reset_Flag=1;
}
else
{
*/

//GetTireDatawhole(5,Vchar);

//ShowOSDWindowAll(Vchar);
//WaitEndofDisplayArea();
//ShowOSDWindow_TW88(1,Vchar);                                        
//ShowOSDWindow_TW88(2,Vchar);                                        
//ShowOSDWindow_TW88(3,Vchar);                                        
//ShowOSDWindow_TW88(4,Vchar); 
//}
//   WriteDecoder(MISC3, ReadDecoder(MISC3)&0xfd);   //black screen..  //add by ryan
//else
//{
//DrawAttrCnt(0,(CH_COLOR_RED|BG_COLOR_CYAN), 16); 
//WriteStringToAddr(0, module, 16);
//ShowOSDWindowAll(1);
//WaitEndofDisplayArea();
//ShowOSDWindow_TW88(3,TRUE);  
//TPMS_OSD=0;
//}

/*
            if((TPMS_Ready==1)&&Vchar)
            {
                  Get_Tire_Data(5,0,FORNT_LEFT_TEMPTURE,FORNT_LEFT_PRESSURE,Vchar);
                  
                  Get_Tire_Data(5,1,FORNT_RIGHT_TEMPTURE,FORNT_RIGHT_PRESSURE,Vchar);

                  Get_Tire_Data(5,2,REAR_LEFT_TEMPTURE,REAR_LEFT_PRESSURE,Vchar);

                  Get_Tire_Data(5,3,REAR2_RIGHT_TEMPTURE,REAR2_RIGHT_PRESSURE,Vchar);
                  
                  TPMS_Ready=0;
            }
 */   
//if(Vchar)
//{
//}  
//TPMS_OSD=1;
#endif
}

void Draw_CAR(void)
{
#if 0
    BYTE i=0,T[]={"TPMS information"},addr=17;
    
//PCT test

DrawAttrCnt(0,(CH_COLOR_WHITE|BG_COLOR_CYAN), 20);
WriteStringToAddr(0,&T[0],16);


WriteStringToAddr(16,Cube0,2);
WriteStringToAddr(21,Cube2,2);

WriteStringToAddr(17,Cube8,2);
WriteStringToAddr(18,Cube1,2);
WriteStringToAddr(19,Cube1,2);
WriteStringToAddr(20,Cube8,2);


WriteStringToAddr(28,Cube6,2);
WriteStringToAddr(34,Cube6,2);
//WriteStringToAddr(23,Cube6,2);
//WriteStringToAddr(41,Cube6,2);


WriteStringToAddr(33,Cube7,2);
WriteStringToAddr(39,Cube7,2);
//WriteStringToAddr(26,Cube7,2);
//WriteStringToAddr(44,Cube7,2);


WriteStringToAddr(46,Cube5,2);
WriteStringToAddr(51,Cube3,2);


WriteStringToAddr(47,Cube4,2);
WriteStringToAddr(48,Cube4,2);
WriteStringToAddr(49,Cube4,2);
WriteStringToAddr(50,Cube4,2);


WriteStringToAddr(22,Tire_None,2);
WriteStringToAddr(27,Tire_None,2);
WriteStringToAddr(40,Tire_None,2);
WriteStringToAddr(41,Tire_None,2);
WriteStringToAddr(44,Tire_None,2);
WriteStringToAddr(45,Tire_None,2);
#endif

/*
WriteStringToAddr(22,&Hex[1],1);
WriteStringToAddr(27,&Hex[2],1);
WriteStringToAddr(40,&Hex[3],1);
WriteStringToAddr(41,&Hex[4],1);
WriteStringToAddr(44,&Hex[5],1);
WriteStringToAddr(45,&Hex[6],1);
*/

                                             
//WriteStringToAddr(41,Tire_None,2);
//WriteStringToAddr(44,Tire_None,2);

}
          

BYTE Read_RX(void)
{

	BYTE x,y,z;

			//CREN=1;
	x=1;
   
	while(x==1)
	{		
			y=Input(2);
		
			if(y=='#')
			{
					//relay0		
					for(z=0;z<=14;z++)
					{	
						y=Input(2);
						buf_tpms[z]=y;
						if(buf_tpms[z]=='#') z=14;
					}
			//CREN=0;
			//RCIF=0;
			x=0;
			}
	}

		//CREN=0;
		//RCIF=0;
		
	return 1;
}

void putch(BYTE num) 
{
//TXEN=1;	
/* output one byte */
//	while(!TXIF)	/* set when register is empty */
//		continue;
//	TXREG = (byte);
}

BYTE getch(void)
{
		/* retrieve one byte */
	//	while(!RCIF)	/* set when register is not empty */
	//		continue;
      //			RCIF=0;
     //			return RCREG;
return 1;	
}

BYTE getche(void)
{
					
				//BYTE c;
				//putch(c = getch());
				//return c;
		return 1;			
}


BYTE GetData(BYTE *addr)
{
//   		BYTE x;

//	for(x=0;x<=14;x++)
  //    {
	//*addr=buf[x];
	//if(buf[x]=='#') x=14;
	//addr++;
      //}
 
return 1;
}

BYTE AsciiToData(BYTE num)
{

 BYTE y;

	
	for(y=0;y<=15;y++)
	{
				if(num==Hex[y]) 
				{
					//num=y;
					return y;
				}
	}	

return num;
}

BYTE* DataToAscii(BYTE val)
{
#if 0
 BYTE x,num[3];
 
                                       if(val>=200)
                                            {       val-=200;
                                                    num[0]='2';   
                                                     num[1]=Ascii[ (val/10)];   
                                                     num[2]=Ascii[(val%10)];
                                            }
                                            else if((val>=100)) 
                                                {   
                                                    val-=100;
                                                    num[0]='1';
                                                     num[1]=Ascii[ (val/10)];   
                                                     num[2]=Ascii[(val%10)];

                                                    
                                                 }
                                                   else 
                                                    {
                                                        num[0]=' ';

                                                                  if(val<=9)
                                                                  num[1]=' '; 
                                                                  else
                                                                  num[1]=Ascii[ (val/10)];    
                                                                  num[2]=Ascii[(val%10)];
                                                    }
                                       
                                         
						     
                                            
                                             
						   
return num;
#endif
return 0;
}

BYTE Check_Tire_Data(BYTE *addr ,BYTE SB_num)
{
#if 0
//SB_Work_Error_Flag=0x00;
SB_Temper_Error_Flag=0x00;
SB_Press_Error_Flag=0x00;
if((SB_Work_Flag&SB_num)==0) return 0;
	
if((*addr>SB_Temperature_High)) SB_Temper_Error_Flag|=SB_num;
addr++;
//if(((buf[2])>SB_Pressure_High)||((buf[2])<SB_Pressure_Low)) SB_Work_Error_Flag|=SB_num;
	if(((*addr)>SB_Pressure_High)||((*addr)<SB_Pressure_Low)) SB_Work_Error_Flag|=SB_num;

return (SB_Work_Error_Flag);
#endif

return 0;
}

void Check_Data(BYTE num,BYTE *addr, BYTE VChar)
{

#if 0
BYTE tire_p[6]={ 22,27,40,41,44,45};
BYTE *Empty="  ";
BYTE  loop_error=0,tp,pp_max,pp_min,pp,ppp_max,tp_limit,pp_limit,ppp_max_limit;
//check sensor info.
tp=(sb_tp_data[num+(num*1)]);    //degree C..
pp=(sb_tp_data[(num+(num*1)+1)]/10);

//pp_max=sb_tp_data[(num+(num*1)+1)]+(pp*2);
//pp_min=sb_tp_data[(num+(num*1)+1)]-(pp*2);
//ppp_max=sb_p_data_max[num];

pp_max=sb_tp_data[(num+(num*1)+1)];
pp_min=sb_tp_data[(num+(num*1)+1)];
ppp_max=sb_p_data_max[num];


tp_limit=(sb_tp_data[num+(num*1)])-((sb_tp_data[num+(num*1)])/10);  //10% alarm
pp_limit=sb_tp_data[(num+(num*1)+1)]-(pp);                                           //10% alarm
ppp_max_limit=sb_p_data_max[num]-(sb_p_data_max[num]/10);               //10% alarm



if(((*addr<tp)&&(*(addr+1)>pp_min))&&(*(addr+1))<ppp_max)
		{
              if(SB_Work_Error_Flag&Device_Mask[num]) SB_Work_Error_Flag&=(~Device_Mask[num]);                                           
              SB_Work_Flag|=Device_Mask[num];                     
              addr++;
              addr++;
              
             if(VChar)
                {
                    if((*addr)&0x02)
                    ;//WriteStringToAddr(tire_p[num],Low_Battery,2);                                                
                    else
                     ;//WriteStringToAddr(tire_p[num],Tire_Green,2);                                                
			
                }
             
              if(SB_Work_Error_Flag==0&&PowerFlag)  ;//PowerLED(ON);

               } 
   else if((SB_Work_Flag&Device_Mask[num])==(Device_Mask[num]))
             {
                           
              SB_Work_Error_Flag|=Device_Mask[num];
        
            
              
              if(VChar){
                         
              //              WriteStringToAddr(tire_p[num],Tire_Yellow,2);  
                            }

             /*
                if(PowerFlag)
                {
           
                    PowerLED(OFF);
                      if(VChar) WriteStringToAddr(tire_p[num],Empty,1);
                    Wait_ms(100);
                     P4=P4|0x01;		//GREEN_LED = 1;		// ON GREEN
		        P4=P4|0x02;		//RED_LED = 1;		// OFF RED	  
		       if(VChar)   WriteStringToAddr(tire_p[num],Tire_Red,2);

                   Wait_ms(100);
                 }
                */
             }

            #endif

}
BYTE GetTireDatawhole(BYTE ins,BYTE VChar)
{
#if 0
BYTE tire_p[6]={ 22,27,40,41,44,45};
BYTE tire_tp[6]={72,177,112,117,212,217};
BYTE tire_pp[6]={82,187,122,127,222,227};
BYTE x,y;
BYTE *Empty="  ";
BYTE Error,Work;
BYTE *Relay="Relay battery L ";
    
//Get_Tire_Data(5,0,FORNT_LEFT_TEMPTURE,FORNT_LEFT_PRESSURE,VChar);

//if(TPMS_Ready==1)
//{

    for( x=0; x<=(SB_MAX-1);x++)   Get_Tire_Data(ins,x,tire_tp[x],tire_pp[x],VChar);  
         for(x=0;x<=98;x++) buf_tpms[x]=0x00;  //clear buffer data..
             TPMS_Ready=0;


        if(TPMS_Relay_Battery)
        {
  //      DrawAttrCnt(0,(CH_COLOR_WHITE|BG_COLOR_RED), 16);
     //   WriteStringToAddr(0,Relay,16);       
        }
                    

                
                if(SB_Work_Error_Flag>=1)
                {
                    Error=SB_Work_Error_Flag;
                    Work=SB_Work_Flag;
                    
                     for( x=0; x<=(SB_MAX-1);x++)  
                        {
                             
                //            if(VChar&&(Error&Work&1)) WriteStringToAddr(tire_p[x],Empty,1);
                            Error>>=1;
  	  			Work>>=1;
                        }

                        
               //     PowerLED(OFF);                                
                    Wait_ms(100);
                    Error=SB_Work_Error_Flag;
                   Work=SB_Work_Flag;

                     for( x=0; x<=(SB_MAX-1);x++)  
                        {
                             
               //             if(VChar&&(Error&Work&1)) WriteStringToAddr(tire_p[x],Tire_Yellow,2);
                            Error>>=1;
                            Work>>=1;
                        }

                    
      //               P4=P4|0x01;		//GREEN_LED = 1;		// ON GREEN
//		        P4=P4|0x02;		//RED_LED = 1;		// OFF RED	  
		       //if(VChar)   WriteStringToAddr(tire_p[num],Tire_Red,2);

                   Wait_ms(100);
                
                }

    
  return 1;
//}
#endif
return 0 ;
}


BYTE Get_Tire_Data(BYTE  ins,BYTE num,BYTE  Temp_p,BYTE Press ,BYTE VChar)
{
#if 0
BYTE TPMS_end=1,x,y,z,TPMS_offset=0,tp,pp_min,pp_max,val[3]={0,0,0},pp,ppp_max;
BYTE tp_limit,pp_limit,ppp_max_limit;

tp=(sb_tp_data[num+(num*1)]); 
pp=(sb_tp_data[(num+(num*1)+1)]/10);
    
//pp_max=sb_tp_data[(num+(num*1)+1)]+(pp*2);
//pp_min=sb_tp_data[(num+(num*1)+1)]-(pp*2);  //20% alarm

pp_max=sb_tp_data[(num+(num*1)+1)];
pp_min=sb_tp_data[(num+(num*1)+1)];  //20% alarm

ppp_max=sb_p_data_max[num];

tp_limit=(sb_tp_data[num+(num*1)])-((sb_tp_data[num+(num*1)])/10);  //10% alarm
pp_limit=sb_tp_data[(num+(num*1)+1)]+(pp);                                           //10% alarm
ppp_max_limit=sb_p_data_max[num]-(sb_p_data_max[num]/10);               //10% alarm

            while(TPMS_offset<=98)
            {

                  if(buf_tpms[TPMS_offset]=='$')
                    {
                                if((buf_tpms[TPMS_offset+3]==Hex[ins])&&(buf_tpms[TPMS_offset+5]==Hex[num]))   
                                {
                                                                                                            
                                       //check sum.
                                        z=0;
                                         for(x=0;x<=10;x++)
                                         {
                                          //y=(buf_tpms[TPMS_offset+1+x]);
                                          z+=(buf_tpms[TPMS_offset+1+x]);
                                         }
                                        y=AsciiToData(buf_tpms[TPMS_offset+12])<<4;
                                        y|=AsciiToData(buf_tpms[TPMS_offset+13]);
                                        if(z!=y) return 0;                                     
                                        
                                
                                       //bettery status.
                                        sb_data[2]=(AsciiToData(buf_tpms[TPMS_offset+6]))<<4;
                                        sb_data[2]|=(AsciiToData(buf_tpms[TPMS_offset+7]));

                                        //sb_data[2]=0x02;//for test...    
                                           
                                       //temperature.
                                       val[2]=(AsciiToData(buf_tpms[TPMS_offset+8]))<<4;
                                       val[2]|=AsciiToData(buf_tpms[TPMS_offset+9]);                                              

                                       val[2]-=40;
                                                
                         if((val[2]>=tp)&&(SB_Work_Flag&Device_Mask[num])&&VChar) 
                     ;//        DrawAttrCnt(Temp_p,(CH_COLOR_WHITE|BG_COLOR_RED), 4);
                                else if((val[2]>=tp_limit)&&VChar)
                                 ;//   DrawAttrCnt(Temp_p,(CH_COLOR_YELLOW|BG_COLOR_CYAN), 4);
                                 else if(VChar)
                                  ;//   DrawAttrCnt(Temp_p,(CH_COLOR_GREEN|BG_COLOR_CYAN), 4);

                                          
                                            sb_data[0]=val[2];
                                                  
                                           if(val[2]>=200)
                                            {       val[2]-=200;
                                                    val[0]='2';   
                                            }
                                            else if((val[2]>=100)) 
                                                {   
                                                    val[2]-=100;
                                                    val[0]='1';
                                                 }
                                                     else  val[0]=' ';

                                             if((val[0]=' ')&&(val[2]==0))
                                           val[1]=' '; 
                                            else
                                             val[1]=Ascii[ (val[2]/10)];                                                                                            
                                               val[2]=Ascii[(val[2]%10)];
                                   



      if((SB_Work_Flag&Device_Mask[num])&&VChar) 
        {
      ;//  WriteStringToAddr(Temp_p,val,3);         
      ;//  WriteStringToAddr(Temp_p+3,Temp,2);     
        }                           
      
                                         //Pressure
                                        val[2]=(AsciiToData(buf_tpms[TPMS_offset+10]))<<4;
                                        //val[2]<<=4;
                                        val[2]|=AsciiToData(buf_tpms[TPMS_offset+11]);

                                        if(val[2] >=5)   //check sensor board   
                                        SB_Work_Flag|=Device_Mask[num];
   
 //if(((val[2]>=pp_max)||(val[2]<=pp_min))&&(SB_Work_Flag&Device_Mask[num])&&VChar) 
if((val[2]<=pp_min||(val[2]>=ppp_max))&&(SB_Work_Flag&Device_Mask[num])&&VChar) 
;//  DrawAttrCnt(Press,(CH_COLOR_WHITE|BG_COLOR_RED), 5);
else if((val[2]>=ppp_max_limit)||(val[2]<=pp_limit)&&VChar)
{
 ;//  DrawAttrCnt(Press,(CH_COLOR_YELLOW|BG_COLOR_CYAN), 5);
}
                                          else if(VChar)
                                          ;//   DrawAttrCnt(Press,(CH_COLOR_GREEN|BG_COLOR_CYAN), 5);

                                         sb_data[1]=val[2];

                                           if(val[2]>=200)
                                            {       val[2]-=200;
                                                    val[0]='2';   
                                            }
                                            else if((val[2]>=100)) 
                                                {   
                                                    val[2]-=100;
                                                    val[0]='1';
                                                 }
                                                     else  val[0]=' ';
                                                     
                                         if((val[0]=' ')&&(val[2]==0))
                                           val[1]=' '; 
                                            else
                                             val[1]=Ascii[ (val[2]/10)];                                                                                            
                                               val[2]=Ascii[(val[2]%10)];
     
                                                               
     if(SB_Work_Flag&Device_Mask[num]&&VChar)  
        {
   ;//     WriteStringToAddr(Press,val,3);  
  ;//         WriteStringToAddr(Press+3,PSI,4);
        }

                                        Check_Data(num,sb_data, VChar);  //check sensor of data.
                                                
                                       TPMS_end=0;      
                                      
                                }      

                    }
                    TPMS_offset++;                     
            }
#endif
return 0;

}

BYTE Get_Relay_Data(BYTE num,BYTE  relay_mask)
{
#if 1
    BYTE TPMS_offset=0,x;
    while(TPMS_offset<=98)
    {

                  if(buf_tpms[TPMS_offset]=='$')



                             if(buf_tpms[TPMS_offset+3]==num)
                                {
                                x=AsciiToData(buf_tpms[TPMS_offset+5]);                    

                                 TPMS_Relay_Battery=(x>>3)&1;
                                 
                                   return (x&relay_mask);                 
                                }

                TPMS_offset++;
  
    }
#endif    
    return 0;
}

#if 0
void Output(BYTE Select,BYTE d)
{
    BYTE T2;

    T2 = S2CON;
			
	  //while(TI == 0);   //Write transmitting has been finished
        //TI = 0;
    if(Select == 1)
    {
				/*
        if(d>9 & d<16)
            SBUF = d + 0x37;
        else if(d<10)
            SBUF = d + 0x30;
        else if(d==0xaa)    //NewLine
            SBUF = 0x0a;
        else if(d==0xdd)    //Return
            SBUF = 0x0d;
        else
					*/
            SBUF = d;
        while(TI == 0);   //Write transmitting has been finished
        TI = 0;
    }
    else//2
    {
        S2BUF = d;
        while(S2CON == T2);
        S2CON = T2;
    }
}

#endif


BYTE Input(BYTE Select)
{	
/*
	BYTE temp;
	S2CON|=0x10;
	S2CON&=0xfe;
	temp=S2CON;
	
	 if(Select==1)
    {
				RI=0;
				while(RI==0);
				return SBUF;
		}
		else
		{
				while(temp==S2CON);
				S2CON&=0xef;
				return S2BUF;
		}
*/
return 0;	
}

#if 0

void Output2(BYTE num,BYTE* addr)
{
BYTE temp;
	temp=*addr;
	
	while(temp!='\0')
	{			
			
	Output(num,*addr);
						if(temp=='#')	return;
				addr++;
					temp=*addr;
	}
}

#endif


