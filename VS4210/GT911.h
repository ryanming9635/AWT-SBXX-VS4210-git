
#ifndef __GT911__
#define __GT911__

//I2C write read comm.
#define GT_CMD_WR 		0X28     		//Write command
#define GT_CMD_RD 		0X29			//Read command
  
//GT911 part of the register definition 
#define GT_CTRL_REG 		0X8040   		//GT9147control register
#define GT_CFGS_REG 		0X8047   		//GT9147configuration start address register
#define GT_CHECK_REG 	0X80FF   		//GT9147checksum register
#define GT_PID_REG 		0X8140   		//GT9147product ID register
 
#define GT_GSTID_REG 	0X814E   		//GT9147currently detected touch
#define GT_TP1_REG 		0X8150  		//Data address of the first touch point
#define GT_TP2_REG 		0X8158		//Data address of the second touch point
#define GT_TP3_REG 		0X8160		//Data address of the third touch poin
#define GT_TP4_REG 		0X8168		//The fourth touch point data address
#define GT_TP5_REG 		0X8170		//Data address of the fifth touch point 

#define  GT9147_WR_Reg(x,y,z)	SET_nGT911(x,y,z)
#define  GT9147_RD_Reg(x,y,z) 	GET_nGT911(x,y,z)
#define  delay_ms(x)  			msleep(x) 

#define Panel_Width	800
#define Panel_Height	480
#define SourceSelcet_X_Axis_Size	4////2
#define SourceSelcet_Y_Axis_Size	4////2
#define SourceSelcet_X_Grid		(Panel_Width/SourceSelcet_X_Axis_Size)
#define SourceSelcet_Y_Grid		(Panel_Height/SourceSelcet_Y_Axis_Size)

//OSD font 16x20(x1)  , 32x40 (x2)
#define OSD_FONT_X_Axis_Size		25
#define OSD_FONT_Y_Axis_Size		12
#define OSD_FONT_X_Grid			(Panel_Width/OSD_FONT_X_Axis_Size)
#define OSD_FONT_Y_Grid			(Panel_Height/OSD_FONT_Y_Axis_Size)

#define SLIDER_DISTANCE   10

  struct TouchInfo
{
   BYTE	sta;
   BYTE	 touchtype;
   BYTE	 touchcount;
   BYTE 	 touchOFF;
   WORD  startX;
   WORD  startY;
   WORD  endX;
   WORD  endY;   
   WORD	x[5];
   WORD	y[5];
};

struct PanelInfo
{
   WORD    id;
   WORD	width;
   WORD	height;
};

typedef enum
{
    _TOUCH_COUNT_0=0,
    _TOUCH_COUNT_1,
    _TOUCH_COUNT_2,
    _TOUCH_COUNT_3,
    _TOUCH_COUNT_4,
    _TOUCH_COUNT_5
} EnumTouchPanelConutID;

BYTE GT9147_Send_Cfg(BYTE mode); 
//BYTE GT9147_WR_Reg(u16 reg,u8 *buf,u8 len); 
//void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len);  
BYTE GT9147_Init(void); 
BYTE GT9147_Scan(BYTE mode);  
BYTE TOUCH_ADDR_OFFSET(WORD icnt,WORD offset);

void Touch_Screen_Handler(BYTE num);

extern void msleep(unsigned int i );
extern void TouchSetSelectMode(BYTE index);
extern void DisplayVol(void);
extern BYTE CheckTouchScreen(BYTE Action, WORD index);
extern void SetCOCCom(BYTE action,BYTE param);
extern void TouchSliderSetSelectMode(BYTE Action);
extern void BOX_SELECT_CHANNEL(BYTE index,BYTE action);
extern void VS4210_ResetCamMirror(tByte ch );

#endif	// __GT911__

