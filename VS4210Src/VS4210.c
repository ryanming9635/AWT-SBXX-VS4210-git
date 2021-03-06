/** @file VS4210.c
  * @brief VS4210setup program
  *
  * Processing YUV422 BT1120 In , output rgb24bits to HDMI.
  *
  * @author Cunu-Yu
  * @date 2017/06/08
  */

#include "VXIS_Extern_Define.h"
#include "VS4210.h"
#include "VX_OSD_Simple.h"
#include "VX_swi2c.h"

#include "VS4210_param_Mode88.h"
#include "VS4210_param_ModeMuti.h"

//#define _RX_EX_SYNC_

static bit isVS4210ParamInit = 1 ;
volatile tByte VS4210ChipIndex = 0 ;
volatile tByte gVS4210I2CBus = 0 ;
volatile tByte VS4210_ADDR = 0x40 ;
volatile tByte VS4210_2_ADDR = 0x40 ;

static tByte isfirstinit[VS4210_MUTICHIP_NUMBER] ; //= {1, 1, 1, 1}  ;
static tByte u8PreDrawOSDFlg[VS4210_MUTICHIP_NUMBER] ; // = {0x04,0x04,0x04,0x04} ;
tByte u8gDrawOSDFlg[VS4210_MUTICHIP_NUMBER] ;
tByte u8DrawBoxFlg[VS4210_MUTICHIP_NUMBER] ;
static tByte u8PreDrawBoxFlg[VS4210_MUTICHIP_NUMBER] ;

tByte u8DrawBoxColor[2] ={0x00, 0x00};
static tByte u8PreDrawBoxColor[2] = {0xFF, 0xFF} ;
tByte gu8AudioMode[VS4210_MUTICHIP_NUMBER] ;
static tByte gu8PreAudioMode[VS4210_MUTICHIP_NUMBER] ;
tByte gbVXIS_OuputSize[VS4210_MUTICHIP_NUMBER] ;
static tByte gaOpenWindowflg[4][VS4210_MUTICHIP_NUMBER] ; //= {{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}} ;
tByte bSignal[4][VS4210_MUTICHIP_NUMBER] ;
//tByte xchMapl[4] ;
tByte gHDMI_Index  ;
tByte gbVXIS_OuputModeflg[VS4210_MUTICHIP_NUMBER] ; //= {0x10,0x10,0x10,0x10} ;
tByte gbVXIS_InputMode[4][VS4210_MUTICHIP_NUMBER]  ;
tByte gaHDMI_MapWindowIndex[4][VS4210_MUTICHIP_NUMBER] ;

unsigned short gHActive[4][VS4210_MUTICHIP_NUMBER]; //={{1920,1920,1920,1920},{1920,1920,1920,1920},{1920,1920,1920,1920},{1920,1920,1920,1920}} ;
unsigned short gVActive[4][VS4210_MUTICHIP_NUMBER]; //={{1080,1080,1080,1080},{1080,1080,1080,1080},{1080,1080,1080,1080},{1080,1080,1080,1080}} ;
unsigned short gHTotal[4][VS4210_MUTICHIP_NUMBER]; //={{2640,2640,2640,2640},{2640,2640,2640,2640},{2640,2640,2640,2640},{2640,2640,2640,2640}} ;
unsigned short gVTotal[4][VS4210_MUTICHIP_NUMBER]; //={{1250,1250,1250,1250},{1250,1250,1250,1250},{1250,1250,1250,1250},{1250,1250,1250,1250}} ;

static tByte gbPreVXIS_InputMode[4][VS4210_MUTICHIP_NUMBER]  ;
static tByte PreaMapWindowIndex[4][VS4210_MUTICHIP_NUMBER] ; //={{5,5,5,5},{5,5,5,5},{5,5,5,5},{5,5,5,5}};
static tByte aOpenWindowflgChange[4][VS4210_MUTICHIP_NUMBER] ; //={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};

static tByte PreOuputModeflg[VS4210_MUTICHIP_NUMBER] ;//= {VS4210_1080P_MODE_NUMBER,VS4210_1080P_MODE_NUMBER,VS4210_1080P_MODE_NUMBER,VS4210_1080P_MODE_NUMBER} ;
static tByte PreVXIS_OuputSize[VS4210_MUTICHIP_NUMBER] ;// = {_OUTPUT_NUMBER,_OUTPUT_NUMBER,_OUTPUT_NUMBER,_OUTPUT_NUMBER} ;
static tByte PreMode0inputmode[VS4210_MUTICHIP_NUMBER] ;// ={255,255,255,255}  ;
static tByte onMode0[VS4210_MUTICHIP_NUMBER] ; //= {0,0,0,0} ;

static tByte onModd0Win0Flg[VS4210_MUTICHIP_NUMBER] ;// = {0,0,0,0}  ;
static tByte gWindowNum ;
static tByte PreVXIS_FULLOuputModeflg[VS4210_MUTICHIP_NUMBER];// = {250,250,250,250} ;
static tByte gvideofftmp = 128  ;

void VS4210_displayon(void) ;
void VS4210_displayoff(void) ;
static tByte Get_WindowNum(tByte OuputMode) ;

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_ParamInit( void )
{
    tByte i , j  ;

    for (i = 0 ; i < VS4210_MUTICHIP_NUMBER ; i++ )
    {
        isfirstinit[i] = 1 ;
        u8PreDrawBoxFlg[i] =0 ;
        u8PreDrawOSDFlg[i] = 4 ;        
        gu8PreAudioMode[i] = 6;
        gu8AudioMode[i] = 5;
        gbVXIS_OuputModeflg[i] = 0x10 ;

        PreOuputModeflg[i] = VS4210_1080P_MODE_NUMBER ;
        PreVXIS_OuputSize[i] = _OUTPUT_NUMBER ;
        PreMode0inputmode[i] = 255 ;
        onMode0[i] =0;
        onModd0Win0Flg[i] =0 ;
        PreVXIS_FULLOuputModeflg[i] = 250;

        for(j = 0 ; j < 4 ; j++)
        {
            gaOpenWindowflg[j][i] = 1 ;
            bSignal[j][i] = 0 ;

            gHActive[j][i] = 1920 ;
            gVActive[j][i] = 1080 ;
            gHTotal[j][i] = 2200 ;
            gVTotal[j][i] = 1250 ;

            PreaMapWindowIndex[j][i] = 5 ;            
            aOpenWindowflgChange[j][i] = 1 ;
            gbVXIS_InputMode[j][i] = _OSD_No_Signal ;
            gbPreVXIS_InputMode[j][i]= 251 ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_FULL88init( tcByte *Addrn, tcByte *Valn )
{
    tByte i ;
    tByte addr ;
    tByte i2cVal ;
//    printf("VS4210_FULL88init,VS4210ChipIndex=%d \r\n", VS4210ChipIndex ) ;

    for (i=0 ; i< VS4210_INIT_NUMBER ; i++)
    {
        addr   = Addrn[i] ;
        i2cVal = Valn[i];

        if (gbVXIS_OuputModeflg[VS4210ChipIndex] >= VS4210_1080P_MODE1)
        {
            if ( addr == 0x10)
            {
                i2cVal =  (i2cVal & 0x7C ) | gHDMI_Index  ;
                VXISI2CWrite( 0x10  ,i2cVal  );
            }
            else if ( addr == 0x9C)
            {
                i2cVal =  VXISI2CRead(0x9C ) ;
                VXISI2CWrite( 0x9C , i2cVal | 0x08 );
                //VXISI2CWrite( 0x9C  , i2cVal | 0x08 );
            }
            else
            {
                VXISI2CWrite( addr  ,i2cVal);
            }
        }
        else
        {
            if ( addr != 0xE9)
            {
                VXISI2CWrite( addr  ,i2cVal);
            }
        }
    }
/*
#ifdef DEF_OUT_YUV16HV_
//		VXISI2CWrite( 0x82 , 0xC0); //OFMT + OUV_INV
		if ((VS4210ChipIndex +1 ) == VS4210_MUTICHIP_NUMBER )
		{
            //VXISI2CWrite( 0x82 , 0xC0); //OFMT + OUV_INV

            //RGB24
            //if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60)
            //{
            //}

            VXISI2CWrite( 0x81 , 0x14);

		}
		else 
		{
            VXISI2CWrite( 0x7F , 0x00) ; //RGB MUX
            //VXISI2CWrite( 0x82 , 0x40);

            VXISI2CWrite( 0x74 , 0x02);
            VXISI2CWrite( 0x81 , 0x14);
            VXISI2CWrite( 0x82 , 0xC0);

            if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60)
            {
                VXISI2CWrite( 0x84 , 0x24);
                VXISI2CWrite( 0x85 , 0xE8);
            }
            else
            {

                VXISI2CWrite( 0x84 , 0x6D);
                VXISI2CWrite( 0x85 , 0xDA);
            }
                VXISI2CWrite( 0x8E , 0x6C);
		}		        
#endif
*/
	VXISI2CWrite( 0x01 , 0xA5); //clear
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_init_Input_MODE88( tByte out_Mode  )
{
    tByte tmpFULLOuputModeflg;
    tmpFULLOuputModeflg = out_Mode & 0xF0   ;

    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        if (PreVXIS_FULLOuputModeflg[VS4210ChipIndex] != tmpFULLOuputModeflg )
        {
            PreVXIS_FULLOuputModeflg[VS4210ChipIndex] = tmpFULLOuputModeflg ;
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_1080P_720P_mode0_w0_88param);
        }
    }
    else
    {
        if (PreVXIS_FULLOuputModeflg[VS4210ChipIndex] != tmpFULLOuputModeflg )
        {
            PreVXIS_FULLOuputModeflg[VS4210ChipIndex] = tmpFULLOuputModeflg ;
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_1080P_1080P_mode0_w0_88param);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_Line_Clear(void)
{    
    //printf("VS4210_Line_Clear,VS4210ChipIndex=%d\r\n",VS4210ChipIndex ) ;
    VXISI2CWrite( 0xB8 , 0x80 );
    VXISI2CWrite( 0xB9 , 0x00 );
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_Line_Draw(tByte mode )
{
    tByte i ;
    tByte addr ;
    tByte i2cVal ;
    //printf("VS4210_Line_Draw,VS4210ChipIndex=%d\r\n",VS4210ChipIndex ) ;

    if (u8DrawBoxFlg[VS4210ChipIndex] ==1)
    {
        if (mode == VS4210_1080P_MODE1 )
        {
            for (i = 0 ; i < 7 ; i++ )
            {
                addr = aLineModeReg[i] ;
                if(gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode1Lin1[i]  ;
                }
                else
                {
                    i2cVal = aLineMode720PMode1Lin1[i]   ;
                }
                if (i == 6 )
                {
                    i2cVal |= u8DrawBoxColor[gVS4210I2CBus] ;
                }
                VXISI2CWrite( addr , i2cVal ) ;
            }

            for (i = 0 ; i < 7 ; i++ )
            {
                addr = aLineModeReg[i] ;
                if(gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode1Lin2[i]   ;
                }
                else
                {
                    i2cVal = aLineMode720PMode1Lin2[i] ;
                }

                if (i == 6 )
                {
                    i2cVal |= u8DrawBoxColor[gVS4210I2CBus]   ;
                }
                VXISI2CWrite( addr , i2cVal ) ;
            }
        }
        else if (mode == VS4210_1080P_MODE12 )
        {
            for (i = 0 ; i < 7 ; i++ )
            {
                addr = aLineModeReg[i] ;
                if(gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode12Lin1[i]   ;
                }
                else
                {
                    i2cVal = aLineMode720PMode12Lin1[i] ;
                }

                if (i == 6 )
                {
                    i2cVal |= u8DrawBoxColor[gVS4210I2CBus]   ;
                }
                VXISI2CWrite( addr  ,i2cVal ) ;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_SwitchChannel(tByte ch , tByte win )
{
    tByte i2cVal ;
    i2cVal = VXISI2CRead(0x02 ) ;
    i2cVal = (i2cVal & 0xFC) | win  ;
    VXISI2CWrite( 0x02 , i2cVal ) ;

    i2cVal = VXISI2CRead(0x10 ) ;
    i2cVal = (i2cVal & 0xFC) | ch  ;
    VXISI2CWrite( 0x10 , i2cVal );
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_OpenWindowsX(tByte n)
{
    tByte i2cVal ;

    if ( gbVXIS_OuputModeflg[VS4210ChipIndex] < VS4210_1080P_MODE1)
    {
        i2cVal = VXISI2CRead(0x02) ;
        i2cVal = (i2cVal & 0xFC)  ;  //0
    }
    else
    {
        i2cVal =  VXISI2CRead(0x02) ;
        i2cVal =  (i2cVal & 0xFC) | n  ;
    }

    VXISI2CWrite( 0x02 , i2cVal ) ;
    i2cVal =  VXISI2CRead(0x10 ) ;
    i2cVal =  i2cVal | 0x80  ;
    VXISI2CWrite( 0x10 , i2cVal );

    msleep(150) ;
    VXISI2CWrite( 0xE9 , 0 );
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_CloseWindowsX(tByte n)
{
    tByte i2cVal ;
    i2cVal =  VXISI2CRead(0x9C ) ;
    VXISI2CWrite( 0x9C , i2cVal | 0x08 );
    i2cVal = VXISI2CRead(0x02 ) ;
    i2cVal = (i2cVal & 0xFC) | n  ;

    VXISI2CWrite( 0x02 , i2cVal );
    i2cVal =  VXISI2CRead(0x10 ) ;
    i2cVal =  i2cVal & 0x7F ;
    VXISI2CWrite( 0x10 ,i2cVal  );
    VXISI2CWrite( 0xE9 , 0x50 );
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_CloseAllWindows(void)
{
    tByte i2cVal ;
    tByte i ;
    tByte tmp ;
	
    i2cVal =  VXISI2CRead(0x9C ) ;
    VXISI2CWrite( 0x9C , i2cVal | 0x08 );

    tmp = VXISI2CRead(0x02 ) ;

    for (i = 0 ; i < VIDEO_IN_NUMBER ; i ++ )
    {
        VXISI2CWrite(0x02, i );
        i2cVal =  VXISI2CRead(0x10 ) ;
        i2cVal =  i2cVal & 0x7F ;
        VXISI2CWrite( 0x10 , i2cVal  );
    }
    VXISI2CWrite( 0x02 , tmp);
}


void VS4210_channel0clear(tByte ch )
{
    tByte i2cVal ;
    tByte tmp ;
    tByte tmp2 ;

    tmp = gaHDMI_MapWindowIndex[0][VS4210ChipIndex] ;
//    printf("channel0clear,VS4210ChipIndex=%d , ch =%d\r\n" , (int)VS4210ChipIndex , (int )tmp ) ;

    i2cVal = VXISI2CRead(0x02 ) ;
    i2cVal = ((i2cVal & 0xFC) | ch ) ;

    VXISI2CWrite( 0x02  , i2cVal );
    tmp  =  VXISI2CRead(0x11 ) ;
    tmp2 =  tmp  ;
    i2cVal =  tmp & 0x80 ;

    if (i2cVal == 0x80)
    {
        tmp =  tmp & 0x7F ;
    }
    else
    {
        tmp =  tmp | 0x80 ;
    }

    VXISI2CWrite( 0x11  , tmp );
    delay1ms(100);
    VXISI2CWrite( 0x11  , tmp2 );
    delay1ms(100);
    VXISI2CWrite( 0x11  , tmp );
    delay1ms(100);
    VXISI2CWrite( 0x11  , tmp2 );
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init( tcByte *Addrn, tcByte *Valn )
{
    tByte i ;
    tByte addr ;
    tByte i2cVal ;
    
//#ifndef __GNUC__
//    EA = 0 ;
//#endif

    //VXISI2CWrite( 0x9C , 0x08 );
    i2cVal =  VXISI2CRead(0x9C ) ;
    VXISI2CWrite( 0x9C , i2cVal | 0x08 );
    
    for (i=0 ; i< VS4210_INIT_MIN_NUM ; i++)
    {
        addr = Addrn[i] ;
        i2cVal  = Valn[i];

        if (gbVXIS_OuputModeflg[VS4210ChipIndex] >= VS4210_1080P_MODE1)
        {
            if ( addr == 0x10)
            {
                i2cVal =  ( i2cVal & 0x7C) | gHDMI_Index ;
                VXISI2CWrite( 0x10  ,i2cVal  );
            }
/*            
            else if ( addr == 0x9C)
            {
                VXISI2CWrite( 0x9C  , i2cVal | 0x08 );
            }
*/
            else if ( addr == 0xE9 )
            {
                VXISI2CWrite( 0xE9, 0x50 );
            }
            else
            {
                VXISI2CWrite( addr  ,i2cVal);
            }
        }
        else
        {
            VXISI2CWrite( addr  ,i2cVal);
        }
    }
//#ifndef __GNUC__
//    EA = 1 ;
//#endif
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_clearAllsignal(void)
{
    tByte i ;

    tByte i2cVal;

    i2cVal =  VXISI2CRead(0x9C ) ;
    VXISI2CWrite( 0x9C  , i2cVal | 0x08 );

    for (i = 0 ; i < VIDEO_IN_NUMBER ; i++ )
    {
        i2cVal = VXISI2CRead(0x02 ) ;
        i2cVal = (i2cVal & 0xFC) | i  ;
        VXISI2CWrite( 0x02 , i2cVal );

        i2cVal =  VXISI2CRead(0x10 ) ;
        i2cVal =  i2cVal & 0x7F ;
        VXISI2CWrite( 0x10 ,i2cVal  );
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_SetHVLenght(tByte ch )
{
    tWord tmp ;
    tByte Hi,Lo , Hi2 ,Lo2 ;

    tmp = gHActive[ch][VS4210ChipIndex] & 0x0FFF ;
    Hi = (tmp >> 4 ) & 0x00F0 ;
    Lo = gHActive[ch][VS4210ChipIndex] & 0x00FF ;

    tmp =  gVActive[ch][VS4210ChipIndex] & 0x0FFF ;
    Hi2 = tmp >> 8 ;
    Lo2 =  gVActive[ch][VS4210ChipIndex] & 0x00FF ;

    Hi = Hi | Hi2 ;
    VXISI2CWrite( 0x12, Hi  );
    VXISI2CWrite( 0x13, Lo  );
    VXISI2CWrite( 0x14, Lo2  );

    tmp = gHTotal[ch][VS4210ChipIndex] & 0x0FFF ;
    Hi = (tmp >> 4 ) & 0x00F0 ;
    Lo = gHTotal[ch][VS4210ChipIndex] & 0x00FF ;

    tmp =  gVTotal[ch][VS4210ChipIndex] & 0x0FFF ;
    Hi2 = tmp >> 8 ;
    Lo2 =  gVTotal[ch][VS4210ChipIndex] & 0x00FF ;

    Hi = Hi | Hi2 ;
    VXISI2CWrite( 0x1F, Hi  );
    VXISI2CWrite( 0x20, Lo  );
    VXISI2CWrite( 0x21, Lo2 );
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE0_w0( tByte in_Mode  )
{
    tByte tmpFULLOuputModeflg;
    tmpFULLOuputModeflg = gbVXIS_OuputModeflg[VS4210ChipIndex] & 0xF0   ;
    PreVXIS_FULLOuputModeflg[VS4210ChipIndex] = tmpFULLOuputModeflg ;
    VS4210_displayoff() ;

    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_NTSC_720P_mode0_w0_88param);
            break ;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_PAL_720P_mode0_w0_88param);
            break ;
        case _OSD_1280X720P :
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_720P_720P_mode0_w0_88param);
            break ;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_1080P_720P_mode0_w0_88param);
            break ;
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_NTSC_1080P_mode0_w0_88param);
            break ;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_PAL_1080P_mode0_w0_88param);
            break ;
        case _OSD_1280X720P :
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_720P_1080P_mode0_w0_88param);
            break ;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_Addr_88param, VS4210_bt656_1080P_1080P_mode0_w0_88param);
            break ;

        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w0( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480i_720P_mode1_w0_param);
            break ;
        case _OSD_720X480P60:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480P_720P_mode1_w0_param);
            break ;
        case _OSD_720X576I50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576i_720P_mode1_w0_param);
            break ;
        case _OSD_720X576P50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576P_720P_mode1_w0_param);
            break ;
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode1_w0_param);
            break ;
        case _OSD_1920X1080I:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080i_720P_mode1_w0_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode1_w0_param);
            break ;

//DVI 720P out
        case _OSD_640X480P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi640x480_720P_mode1_w0_param);
            break ;
        case _OSD_800X600P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi800x600_720P_mode1_w0_param);
            break ;
        case _OSD_1024X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1024x768_720P_mode1_w0_param);
            break ;
        case _OSD_1280X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x768_720P_mode1_w0_param);
            break ;
        case _OSD_1360X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1360x768_720P_mode1_w0_param);
            break ;
        case _OSD_1366X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1366x768_720P_mode1_w0_param);
            break ;
        case _OSD_1280X800P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x800_720P_mode1_w0_param);
            break ;
        case _OSD_1440X900P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1440x900_720P_mode1_w0_param);
            break ;
        case _OSD_1280X1024P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x1024_720P_mode1_w0_param);
            break ;
        case _OSD_1400X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1400x1050_720P_mode1_w0_param);
            break ;
        case _OSD_1680X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_720P_mode1_w0_param);
            break ;
        case _OSD_1600X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1600x1200_720P_mode1_w0_param);
            break ;
        case _OSD_1920X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_720P_mode1_w0_param);
            break ;
#ifdef _RX_EX_SYNC_
        case _OSD_1680X1050PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_720PRB_mode1_w0_param);
            break ;
        case _OSD_1920X1200PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_720PRB_mode1_w0_param);
            break ;
#endif
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480i_1080P_mode1_w0_param);
            break ;
        case _OSD_720X480P60:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480P_1080P_mode1_w0_param);
            break ;
        case _OSD_720X576I50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576i_1080P_mode1_w0_param);
            break ;
        case _OSD_720X576P50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576P_1080P_mode1_w0_param);
            break ;
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode1_w0_param);
            break ;
        case _OSD_1920X1080I:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080i_1080P_mode1_w0_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode1_w0_param);
            break ;

//DVI 1080P out
        case _OSD_640X480P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi640x480_1080P_mode1_w0_param);
            break ;
        case _OSD_800X600P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi800x600_1080P_mode1_w0_param);
            break ;
        case _OSD_1024X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1024x768_1080P_mode1_w0_param);
            break ;
        case _OSD_1280X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x768_1080P_mode1_w0_param);
            break ;
        case _OSD_1360X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1360x768_1080P_mode1_w0_param);
            break ;
        case _OSD_1366X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1366x768_1080P_mode1_w0_param);
            break ;
        case _OSD_1280X800P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x800_1080P_mode1_w0_param);
            break ;
        case _OSD_1440X900P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1440x900_1080P_mode1_w0_param);
            break ;
        case _OSD_1280X1024P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x1024_1080P_mode1_w0_param);
            break ;
        case _OSD_1400X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1400x1050_1080P_mode1_w0_param);
            break ;
        case _OSD_1680X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_1080P_mode1_w0_param);
            break ;
        case _OSD_1600X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1600x1200_1080P_mode1_w0_param);
            break ;
        case _OSD_1920X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_1080P_mode1_w0_param);
            break ;
#ifdef _RX_EX_SYNC_
        case _OSD_1680X1050PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_1080PRB_mode1_w0_param);
            break ;
        case _OSD_1920X1200PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_1080PRB_mode1_w0_param);
            break ;        
#endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w1( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480i_720P_mode1_w1_param);
            break ;
        case _OSD_720X480P60:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480P_720P_mode1_w1_param);
            break ;
        case _OSD_720X576I50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576i_720P_mode1_w1_param);
            break ;
        case _OSD_720X576P50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576P_720P_mode1_w1_param);
            break ;
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode1_w1_param);
            break ;
        case _OSD_1920X1080I:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080i_720P_mode1_w1_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode1_w1_param);
            break ;

//DVI 720P out
        case _OSD_640X480P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi640x480_720P_mode1_w1_param);
            break ;
        case _OSD_800X600P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi800x600_720P_mode1_w1_param);
            break ;
        case _OSD_1024X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1024x768_720P_mode1_w1_param);
            break ;
        case _OSD_1280X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x768_720P_mode1_w1_param);
            break ;
        case _OSD_1360X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1360x768_720P_mode1_w1_param);
            break ;
        case _OSD_1366X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1366x768_720P_mode1_w1_param);
            break ;
        case _OSD_1280X800P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x800_720P_mode1_w1_param);
            break ;
        case _OSD_1440X900P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1440x900_720P_mode1_w1_param);
            break ;
        case _OSD_1280X1024P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x1024_720P_mode1_w1_param);
            break ;
        case _OSD_1400X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1400x1050_720P_mode1_w1_param);
            break ;
        case _OSD_1680X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_720P_mode1_w1_param);
            break ;
        case _OSD_1600X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1600x1200_720P_mode1_w1_param);
            break ;
        case _OSD_1920X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_720P_mode1_w1_param);
            break ;
#ifdef _RX_EX_SYNC_
        case _OSD_1680X1050PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_720PRB_mode1_w1_param);
            break ;
        case _OSD_1920X1200PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_720PRB_mode1_w1_param);
            break ;
#endif
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480i_1080P_mode1_w1_param);
            break ;
        case _OSD_720X480P60:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480P_1080P_mode1_w1_param);
            break ;
        case _OSD_720X576I50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576i_1080P_mode1_w1_param);
            break ;
        case _OSD_720X576P50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576P_1080P_mode1_w1_param);
            break ;
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode1_w1_param);
            break ;
        case _OSD_1920X1080I:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080i_1080P_mode1_w1_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode1_w1_param);
            break ;

//DVI 1080P out
        case _OSD_640X480P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi640x480_1080P_mode1_w1_param);
            break ;
        case _OSD_800X600P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi800x600_1080P_mode1_w1_param);
            break ;
        case _OSD_1024X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1024x768_1080P_mode1_w1_param);
            break ;
        case _OSD_1280X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x768_1080P_mode1_w1_param);
            break ;
        case _OSD_1360X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1360x768_1080P_mode1_w1_param);
            break ;
        case _OSD_1366X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1366x768_1080P_mode1_w1_param);
            break ;
        case _OSD_1280X800P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x800_1080P_mode1_w1_param);
            break ;
        case _OSD_1440X900P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1440x900_1080P_mode1_w1_param);
            break ;
        case _OSD_1280X1024P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x1024_1080P_mode1_w1_param);
            break ;
        case _OSD_1400X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1400x1050_1080P_mode1_w1_param);
            break ;
        case _OSD_1680X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_1080P_mode1_w1_param);
            break ;
        case _OSD_1600X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1600x1200_1080P_mode1_w1_param);
            break ;
        case _OSD_1920X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_1080P_mode1_w1_param);
            break ;
#ifdef _RX_EX_SYNC_
        case _OSD_1680X1050PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_1080PRB_mode1_w1_param);
            break ;
        case _OSD_1920X1200PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_1080PRB_mode1_w1_param);
            break ;
#endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w2( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480i_720P_mode1_w2_param);
            break ;
        case _OSD_720X480P60:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480P_720P_mode1_w2_param);
            break ;
        case _OSD_720X576I50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576i_720P_mode1_w2_param);
            break ;
        case _OSD_720X576P50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576P_720P_mode1_w2_param);
            break ;
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode1_w2_param);
            break ;
        case _OSD_1920X1080I:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080i_720P_mode1_w2_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode1_w2_param);
            break ;

//DVI 720P out
        case _OSD_640X480P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi640x480_720P_mode1_w2_param);
            break ;
        case _OSD_800X600P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi800x600_720P_mode1_w2_param);
            break ;
        case _OSD_1024X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1024x768_720P_mode1_w2_param);
            break ;
        case _OSD_1280X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x768_720P_mode1_w2_param);
            break ;
        case _OSD_1360X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1360x768_720P_mode1_w2_param);
            break ;
        case _OSD_1366X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1366x768_720P_mode1_w2_param);
            break ;
        case _OSD_1280X800P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x800_720P_mode1_w2_param);
            break ;
        case _OSD_1440X900P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1440x900_720P_mode1_w2_param);
            break ;
        case _OSD_1280X1024P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x1024_720P_mode1_w2_param);
            break ;
        case _OSD_1400X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1400x1050_720P_mode1_w2_param);
            break ;
        case _OSD_1680X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_720P_mode1_w2_param);
            break ;
        case _OSD_1600X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1600x1200_720P_mode1_w2_param);
            break ;
        case _OSD_1920X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_720P_mode1_w2_param);
            break ;
#ifdef _RX_EX_SYNC_
        case _OSD_1680X1050PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_720PRB_mode1_w2_param);
            break ;
        case _OSD_1920X1200PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_720PRB_mode1_w2_param);
            break ;
#endif
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480i_1080P_mode1_w2_param);
            break ;
        case _OSD_720X480P60:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480P_1080P_mode1_w2_param);
            break ;
        case _OSD_720X576I50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576i_1080P_mode1_w2_param);
            break ;
        case _OSD_720X576P50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576P_1080P_mode1_w2_param);
            break ;
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode1_w2_param);
            break ;
        case _OSD_1920X1080I:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080i_1080P_mode1_w2_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode1_w2_param);
            break ;

//DVI 1080P out
        case _OSD_640X480P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi640x480_1080P_mode1_w2_param);
            break ;
        case _OSD_800X600P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi800x600_1080P_mode1_w2_param);
            break ;
        case _OSD_1024X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1024x768_1080P_mode1_w2_param);
            break ;
        case _OSD_1280X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x768_1080P_mode1_w2_param);
            break ;
        case _OSD_1360X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1360x768_1080P_mode1_w2_param);
            break ;
        case _OSD_1366X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1366x768_1080P_mode1_w2_param);
            break ;
        case _OSD_1280X800P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x800_1080P_mode1_w2_param);
            break ;
        case _OSD_1440X900P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1440x900_1080P_mode1_w2_param);
            break ;
        case _OSD_1280X1024P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x1024_1080P_mode1_w2_param);
            break ;
        case _OSD_1400X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1400x1050_1080P_mode1_w2_param);
            break ;
        case _OSD_1680X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_1080P_mode1_w2_param);
            break ;
        case _OSD_1600X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1600x1200_1080P_mode1_w2_param);
            break ;
        case _OSD_1920X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_1080P_mode1_w2_param);
            break ;
#ifdef _RX_EX_SYNC_
        case _OSD_1680X1050PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_1080PRB_mode1_w2_param);
            break ;
        case _OSD_1920X1200PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_1080PRB_mode1_w2_param);
            break ;
#endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w3( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480i_720P_mode1_w3_param);
            break ;
        case _OSD_720X480P60:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480P_720P_mode1_w3_param);
            break ;
        case _OSD_720X576I50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576i_720P_mode1_w3_param);
            break ;
        case _OSD_720X576P50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576P_720P_mode1_w3_param);
            break ;
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode1_w3_param);
            break ;
        case _OSD_1920X1080I:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080i_720P_mode1_w3_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode1_w3_param);
            break ;

//DVI 720P out
        case _OSD_640X480P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi640x480_720P_mode1_w3_param);
            break ;
        case _OSD_800X600P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi800x600_720P_mode1_w3_param);
            break ;
        case _OSD_1024X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1024x768_720P_mode1_w3_param);
            break ;
        case _OSD_1280X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x768_720P_mode1_w3_param);
            break ;
        case _OSD_1360X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1360x768_720P_mode1_w3_param);
            break ;
        case _OSD_1366X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1366x768_720P_mode1_w3_param);
            break ;
        case _OSD_1280X800P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x800_720P_mode1_w3_param);
            break ;
        case _OSD_1440X900P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1440x900_720P_mode1_w3_param);
            break ;
        case _OSD_1280X1024P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x1024_720P_mode1_w3_param);
            break ;
        case _OSD_1400X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1400x1050_720P_mode1_w3_param);
            break ;
        case _OSD_1680X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_720P_mode1_w3_param);
            break ;
        case _OSD_1600X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1600x1200_720P_mode1_w3_param);
            break ;
        case _OSD_1920X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_720P_mode1_w3_param);
            break ;
#ifdef _RX_EX_SYNC_
        case _OSD_1680X1050PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_720PRB_mode1_w3_param);
            break ;
        case _OSD_1920X1200PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_720PRB_mode1_w3_param);
            break ;
#endif
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_720X480I60 :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480i_1080P_mode1_w3_param);
            break ;
        case _OSD_720X480P60:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi480P_1080P_mode1_w3_param);
            break ;
        case _OSD_720X576I50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576i_1080P_mode1_w3_param);
            break ;
        case _OSD_720X576P50:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi576P_1080P_mode1_w3_param);
            break ;
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode1_w3_param);
            break ;
        case _OSD_1920X1080I:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080i_1080P_mode1_w3_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode1_w3_param);
            break ;

//DVI 1080P out
        case _OSD_640X480P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi640x480_1080P_mode1_w3_param);
            break ;
        case _OSD_800X600P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi800x600_1080P_mode1_w3_param);
            break ;
        case _OSD_1024X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1024x768_1080P_mode1_w3_param);
            break ;
        case _OSD_1280X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x768_1080P_mode1_w3_param);
            break ;
        case _OSD_1360X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1360x768_1080P_mode1_w3_param);
            break ;
        case _OSD_1366X768P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1366x768_1080P_mode1_w3_param);
            break ;
        case _OSD_1280X800P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x800_1080P_mode1_w3_param);
            break ;
        case _OSD_1440X900P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1440x900_1080P_mode1_w3_param);
            break ;
        case _OSD_1280X1024P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1280x1024_1080P_mode1_w3_param);
            break ;
        case _OSD_1400X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1400x1050_1080P_mode1_w3_param);
            break ;
        case _OSD_1680X1050P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_1080P_mode1_w3_param);
            break ;
        case _OSD_1600X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1600x1200_1080P_mode1_w3_param);
            break ;
        case _OSD_1920X1200P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_1080P_mode1_w3_param);
            break ;
#ifdef _RX_EX_SYNC_
        case _OSD_1680X1050PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1680x1050_1080PRB_mode1_w3_param);
            break ;
        case _OSD_1920X1200PRB:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1920x1200_1080PRB_mode1_w3_param);
            break ;
#endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE12_w0( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode12_w0_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode12_w0_param);
            break ;
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode12_w0_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode12_w0_param);
            break ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE12_w1( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode12_w1_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode12_w1_param);
            break ;
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode12_w1_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode12_w1_param);
            break ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE_N12_w0( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode_n12_w0_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode_n12_w0_param);
            break ;
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode_n12_w0_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode_n12_w0_param);
            break ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE_N12_w1( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode_n12_w1_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode_n12_w1_param);
            break ;
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode_n12_w1_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode_n12_w1_param);
            break ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE_N13_w0( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode_n13_w0_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode_n13_w0_param);
            break ;
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode_n13_w0_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode_n13_w0_param);
            break ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE_N13_w1( tByte in_Mode  )
{
    if (gbVXIS_OuputSize[VS4210ChipIndex] == _OUTPUT_720P60 )
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_720P_mode_n13_w1_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_720P_mode_n13_w1_param);
            break ;
        }
    }
    else
    {
        switch(in_Mode)
        {
        case _OSD_1280X720P :
            VS4210_init(VS4210_Addr_param, VS4210_hdmi720P_1080P_mode_n13_w1_param);
            break ;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_Addr_param, VS4210_hdmi1080P_1080P_mode_n13_w1_param);
            break ;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
static void VS4210_Mode0FastChange(tByte ch  )
{
    tByte i2cVal  ;
    tByte i2cVal2  ;    

    i2cVal = VXISI2CRead(0x02 ) ;
    i2cVal = (i2cVal & 0xFC) ;
    VXISI2CWrite( 0x02 , i2cVal );

    i2cVal = VXISI2CRead(0x10 ) ;
    i2cVal = i2cVal | 0x10 ;
    VXISI2CWrite( 0x10 , i2cVal );

    VS4210_SetHVLenght(ch);
    msleep (20)  ;

    i2cVal = VXISI2CRead( 0x1E ) ;
    
#ifdef _RX_EX_SYNC_
    i2cVal = (i2cVal | 0xA0); //EP extern
#else
    i2cVal = (i2cVal | 0x80); //ITE
    i2cVal = i2cVal & 0xDF ;  //ITE
#endif
    
    VXISI2CWrite( 0x1E  , i2cVal );

    i2cVal = VXISI2CRead(0x10 ) ;

    i2cVal = (i2cVal & 0xFC ) ;
    i2cVal2 =  (0x80 | ch ) ;

    i2cVal =  i2cVal | i2cVal2 ;
    VXISI2CWrite( 0x10  ,i2cVal  );

    msleep(60) ;

    i2cVal = VXISI2CRead(0x1E ) ;
    i2cVal = (i2cVal & 0x7F )   ;
    VXISI2CWrite( 0x1E  , i2cVal );

    msleep(70) ;

    i2cVal = VXISI2CRead(0x10 ) ;
    i2cVal = i2cVal & 0xEF   ;
    VXISI2CWrite( 0x10  , i2cVal );
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_init_mode(tByte ch , tByte in_mode , tByte out_mode )
{
    bit fastmodeflg = 1 ;    
    switch(out_mode)
    {
    case VS4210_1080P_MODE0_w0:
    {
        if (onMode0[VS4210ChipIndex] == 0 )
        {
            onMode0[VS4210ChipIndex] =1;
            VS4210_init_Input_MODE0_w0(in_mode);
            VS4210_SwitchChannel(0  , 0 ) ;
            PreMode0inputmode [VS4210ChipIndex] = in_mode ;
            fastmodeflg = 0 ;
        }
        else
        {
            if (PreMode0inputmode [VS4210ChipIndex] != in_mode)
            {
                PreMode0inputmode [VS4210ChipIndex] = in_mode ;
                VS4210_init_Input_MODE0_w0(in_mode);
                VS4210_SwitchChannel(0  , 0 ) ;
                fastmodeflg = 0 ;
            }
            else
            {
                fastmodeflg = 1 ;
            }
        }

        if (in_mode !=_OSD_No_Signal )
        {
            if (fastmodeflg == 0 )
            {
                VS4210_displayoff();
            }
            else
            {
                VS4210_Mode0FastChange(0);
            }
        }

        if (bSignal[0][VS4210ChipIndex] == 0)
        {
            VS4210_clearAllsignal();
        }
        else
        {
            VS4210_displayon();
            if (fastmodeflg != 1)
            {
                VS4210_OpenWindowsX(0) ;
            }
        }
        break ;
    }
    case VS4210_1080P_MODE0_w1:
    {
        if (onMode0[VS4210ChipIndex] == 0 )
        {
            onMode0[VS4210ChipIndex] =1;
            VS4210_init_Input_MODE0_w0(in_mode);
            VS4210_SwitchChannel(1  , 0 ) ;
            PreMode0inputmode [VS4210ChipIndex] = in_mode ;
            fastmodeflg = 0 ;
        }
        else
        {
            if (PreMode0inputmode [VS4210ChipIndex] != in_mode)
            {
                PreMode0inputmode [VS4210ChipIndex] = in_mode ;
                VS4210_init_Input_MODE0_w0(in_mode);
                VS4210_SwitchChannel(1  , 0 ) ;
                fastmodeflg = 0 ;
            }
            else
            {
                fastmodeflg = 1 ;
            }
        }

        if (in_mode !=_OSD_No_Signal   )
        {
            if (fastmodeflg == 0 )
            {
                VS4210_displayoff();
            }
            else
            {
                VS4210_Mode0FastChange(1);
            }
        }

        if (bSignal[1][VS4210ChipIndex] == 0)
        {
            VS4210_clearAllsignal();
        }
        else
        {
            VS4210_displayon();
            if (fastmodeflg != 1)
            {
                VS4210_OpenWindowsX(0) ;
            }
        }
        break ;
    }
    case VS4210_1080P_MODE0_w2:
    {
        if (onMode0[VS4210ChipIndex] == 0 )
        {
            onMode0[VS4210ChipIndex] =1;
            VS4210_init_Input_MODE0_w0(in_mode);
            VS4210_SwitchChannel(2  , 0 ) ;
            PreMode0inputmode [VS4210ChipIndex] = in_mode ;
            fastmodeflg = 0 ;
        }
        else
        {
            if (PreMode0inputmode [VS4210ChipIndex] != in_mode)
            {
                PreMode0inputmode [VS4210ChipIndex] = in_mode ;
                VS4210_init_Input_MODE0_w0(in_mode);
                VS4210_SwitchChannel(2  , 0 ) ;
                fastmodeflg = 0 ;
            }
            else
            {
                fastmodeflg = 1 ;
            }

        }

        if (in_mode !=_OSD_No_Signal   )
        {
            if (fastmodeflg == 0 )
            {
                VS4210_displayoff();
            }
            else
            {
                VS4210_Mode0FastChange(2);
            }
        }

        if (bSignal[2][VS4210ChipIndex] == 0)
        {
            VS4210_clearAllsignal();
        }
        else
        {
            VS4210_displayon();
            if (fastmodeflg != 1)
            {
                VS4210_OpenWindowsX(0) ;
            }

        }
        break ;
    }
    case VS4210_1080P_MODE0_w3:
    {
        if (onMode0[VS4210ChipIndex] == 0 )
        {
            onMode0[VS4210ChipIndex] =1;
            VS4210_init_Input_MODE0_w0(in_mode);
            VS4210_SwitchChannel(3  , 0 ) ;
            PreMode0inputmode [VS4210ChipIndex] = in_mode ;
            fastmodeflg = 0 ;
        }
        else
        {
            if (PreMode0inputmode [VS4210ChipIndex] != in_mode)
            {
                PreMode0inputmode [VS4210ChipIndex] = in_mode ;
                VS4210_init_Input_MODE0_w0(in_mode);
                VS4210_SwitchChannel(3  , 0 ) ;
                fastmodeflg = 0 ;
            }
            else
            {
                fastmodeflg = 1 ;
            }
        }

        if (in_mode !=_OSD_No_Signal   )
        {
            if (fastmodeflg == 0 )
            {
                VS4210_displayoff();
            }
            else
            {
                VS4210_Mode0FastChange(3);
            }
        }

        if (bSignal[3][VS4210ChipIndex] == 0)
        {
            VS4210_clearAllsignal();
        }
        else
        {
            VS4210_displayon();
            if (fastmodeflg != 1)
            {
                VS4210_OpenWindowsX(0) ;
            }
        }
        break ;
    }
    case VS4210_1080P_MODE1:
    {
        onMode0[VS4210ChipIndex] = 0;
        if (ch == 0 )
        {
            VS4210_init_Input_MODE1_w0(in_mode ) ;
        }
        else if (ch == 1 )
        {
            VS4210_init_Input_MODE1_w1(in_mode ) ;
        }
        else if (ch == 2 )
        {
            VS4210_init_Input_MODE1_w2(in_mode ) ;
        }
        else
        {
            VS4210_init_Input_MODE1_w3(in_mode ) ;
        }
        break ;
    }
    case VS4210_1080P_MODE12:
    {
        onMode0[VS4210ChipIndex] = 0;
        if (ch == 0 )
        {
            VS4210_init_Input_MODE12_w0(in_mode ) ;
        }
        else if (ch == 1 )
        {
            VS4210_init_Input_MODE12_w1(in_mode ) ;
        }
        break ;
    }
    case VS4210_1080P_MODE_N12:
    {
        onMode0[VS4210ChipIndex] = 0;
        if (ch == 0 )
        {
            VS4210_init_Input_MODE_N12_w0(in_mode ) ;
        }
        else if (ch == 1 )
        {
            VS4210_init_Input_MODE_N12_w1(in_mode ) ;
        }
        break ;
    }
    
    
    case VS4210_1080P_MODE_N13:
    {
        onMode0[VS4210ChipIndex] = 0;
        if (ch == 0 )
        {
            VS4210_init_Input_MODE_N13_w0(in_mode ) ;
        }
        else if (ch == 1 )
        {
            VS4210_init_Input_MODE_N13_w1(in_mode ) ;
        }
        break ;
    }
    
    }
}

///////////////////////////////////////////////////////////////////////////////////
tByte  VS4210_SetInModeOutMode(tByte ch ,tByte inmode ,tByte outmode )
{
    tByte osdflash = 0 ;
    tByte i ;

    //printf("SetInModeOutMode,4210=%d,ch=%d,inmode=%d,outmode=0x%02x\r\n",(int)VS4210ChipIndex,(int)ch,(int)inmode,(int)outmode) ;
    gWindowNum = Get_WindowNum(gbVXIS_OuputModeflg[VS4210ChipIndex])  ;
    gHDMI_Index = ch ;
    gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] = inmode ;
    if (inmode != _OSD_No_Signal )
    {
        bSignal[ch][VS4210ChipIndex] = 1 ;
    }
    else
    {
        bSignal[ch][VS4210ChipIndex] = 0 ;
    }

    if (outmode <=  VS4210_1080P_MODE0_w3 )
    {
        if(gbPreVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] != gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] )
        {
            gbPreVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] = gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] ;
            onMode0[VS4210ChipIndex] = 0 ;
        }
        //if (WMap0 > 0 )
        //gbVXIS_OuputModeflg[VS4210ChipIndex] = WMap0 -1 ;
        gbVXIS_OuputModeflg[VS4210ChipIndex] = gaHDMI_MapWindowIndex[0][VS4210ChipIndex] ;
    }
    else
    {
        gbVXIS_OuputModeflg[VS4210ChipIndex] = outmode ;
    }

    if (gbVXIS_OuputModeflg[VS4210ChipIndex] ==  VS4210_1080P_MODE0_w0 )
    {
        if (gHDMI_Index == 0 )
        {
            VS4210_init_mode(gHDMI_Index  , gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex]  , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
            osdflash =1 ;
        }
        else
        {
            osdflash =0 ;
        }
    }
    else if (gbVXIS_OuputModeflg[VS4210ChipIndex] ==  VS4210_1080P_MODE0_w1 )
    {
        if (gHDMI_Index == 1 )
        {
            VS4210_init_mode(gHDMI_Index, gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex]  , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
            osdflash =1 ;
        }
        else
        {
            osdflash =0 ;
        }

    }
    else if (gbVXIS_OuputModeflg[VS4210ChipIndex] ==  VS4210_1080P_MODE0_w2 )
    {
        if (gHDMI_Index == 2 )
        {
            VS4210_init_mode(gHDMI_Index  , gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex]  , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
            osdflash =1 ;
        }
        else
        {
            osdflash =0 ;
        }
    }
    else if (gbVXIS_OuputModeflg[VS4210ChipIndex] ==  VS4210_1080P_MODE0_w3 )
    {
        if (gHDMI_Index == 3 )
        {
            VS4210_init_mode(gHDMI_Index  , gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex]  , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
            osdflash =1 ;
        }
        else
        {
            osdflash =0 ;
        }
    }
    else //if (gbVXIS_OuputModeflg[VS4210ChipIndex] == VS4210_1080P_MODE1 )
    {
        for ( i = 0 ; i < gWindowNum ; i++)
        {
            if (gaHDMI_MapWindowIndex[i][VS4210ChipIndex] == gHDMI_Index )
            {
                VS4210_init_mode(i  , gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex]  , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
                if (gaOpenWindowflg[i][VS4210ChipIndex] == 1)
                {
                    VS4210_OpenWindowsX(i);
                    VXIS_fShowInOutMode( i , gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] ) ;
                }
                osdflash =1 ;
            }
        }
        //printf("@@%s,%d,i=%d,mapi=%d \n", __func__,__LINE__ ,gHDMI_Index ,gaHDMI_MapWindowIndex[gHDMI_Index] ) ;
        if ( bSignal[gaHDMI_MapWindowIndex[0][VS4210ChipIndex]][VS4210ChipIndex] == 0  )
        {
            VS4210_channel0clear(0 ) ;
        }
    }

    if((osdflash ==1 )&&(gbVXIS_OuputModeflg[VS4210ChipIndex] <= VS4210_1080P_MODE0_w3 ))
    {
        VXIS_fShowInOutMode0Fast( gbVXIS_OuputModeflg[VS4210ChipIndex], gbVXIS_InputMode[gbVXIS_OuputModeflg[VS4210ChipIndex]][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] )  ;
    }
    return osdflash  ;
}

///////////////////////////////////////////////////////////////////////////////////
static tByte Get_WindowNum(tByte OuputMode)
{
    tByte Num ;
    if (OuputMode < VS4210_1080P_MODE1 )
    {
        Num = 1 ;
    }
    else if (OuputMode == VS4210_FULL_MODE12 )
    {
        Num = 2 ;
    }
    else if (OuputMode == VS4210_FULL_MODE_N12 )
    {
        Num = 2 ;
    }
    else if (OuputMode == VS4210_FULL_MODE_N13 )
    {
        Num = 2 ;
    }
    else
    {
        Num = 4 ;
    }
    return Num ;
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_StartInit(void)
{
    tByte i ;
    tByte tmpval ;
    //printf("VS4210_StartInit\r\n ") ;

    gWindowNum = Get_WindowNum(gbVXIS_OuputModeflg[VS4210ChipIndex])  ;

    VS4210_displayoff() ;
    for (i = 0 ; i < VIDEO_IN_NUMBER ; i++ )
    {
        VS4210_CloseWindowsX(i);
    }

    VS4210_init_Input_MODE88( gbVXIS_OuputModeflg[VS4210ChipIndex] )  ;

    for (i = 0 ; i < VIDEO_IN_NUMBER ; i++ )
    {
        VS4210_init_mode(i  , gbVXIS_InputMode[i][VS4210ChipIndex]  , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
    }

    for (i = 0 ; i < VIDEO_IN_NUMBER ; i++ )
    {
        if (bSignal[i][VS4210ChipIndex] ==0 )
        {
            VS4210_CloseWindowsX(gaHDMI_MapWindowIndex[i][VS4210ChipIndex]);
        }
        else
        {
            VS4210_OpenWindowsX(gaHDMI_MapWindowIndex[i][VS4210ChipIndex]) ;
        }

        if (bSignal[i][VS4210ChipIndex] ==0 )
        {
            gbVXIS_InputMode[i][VS4210ChipIndex] = _OSD_No_Signal ;
        }
    }
    if (isfirstinit[VS4210ChipIndex] ==0 )
    {
        VS4210_channel0clear( 0 )  ;
    }

    gbVXIS_InputMode[0][VS4210ChipIndex] = _OSD_No_Signal ;
    gbVXIS_InputMode[1][VS4210ChipIndex] = _OSD_No_Signal ;
    gbVXIS_InputMode[2][VS4210ChipIndex] = _OSD_No_Signal ;
    gbVXIS_InputMode[3][VS4210ChipIndex] = _OSD_No_Signal ;

    tmpval = VXISI2CRead(0x81);
    VXISI2CWrite(0x81, tmpval & 0xBF);
    PreVXIS_OuputSize[VS4210ChipIndex] = gbVXIS_OuputSize[VS4210ChipIndex] ;

    gbVXIS_InputMode[0][VS4210ChipIndex] = 100 ;
    gbVXIS_InputMode[1][VS4210ChipIndex] = 100 ;
    gbVXIS_InputMode[2][VS4210ChipIndex] = 100 ;
    gbVXIS_InputMode[3][VS4210ChipIndex] = 100 ;

//    OSD_Clear_All();
//    VS4210_Line_Clear() ;

}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_No_Signal_Init(void)
{
    gbVXIS_InputMode[0][VS4210ChipIndex] = _OSD_1920X1080P ;
    gbVXIS_InputMode[1][VS4210ChipIndex] = _OSD_1920X1080P ;
    gbVXIS_InputMode[2][VS4210ChipIndex] = _OSD_1920X1080P ;
    gbVXIS_InputMode[3][VS4210ChipIndex] = _OSD_1920X1080P ;
    VS4210_StartInit() ;
//    gbVXIS_InputMode[0][VS4210ChipIndex] = _OSD_No_Signal ;
//    gbVXIS_InputMode[1][VS4210ChipIndex] = _OSD_No_Signal ;
//    gbVXIS_InputMode[2][VS4210ChipIndex] = _OSD_No_Signal ;
//    gbVXIS_InputMode[3][VS4210ChipIndex] = _OSD_No_Signal ;
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_displayoff(void)
{
    tByte  tmpval ;
    tmpval = VXISI2CRead(0x32);
    if (tmpval != 0 )
    {
        gvideofftmp = tmpval  ;
    }
    VXISI2CWrite(0x32, 0 );
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_displayon(void)
{
    msleep(80);
    VXISI2CWrite(0x32,gvideofftmp  );
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_Rx_NoSignal(tByte ch )
{
    tByte j ;
    gHDMI_Index = ch ;
    bSignal[ch][VS4210ChipIndex] = 0 ;

    if (gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] !=  _OSD_No_Signal  )
    {
        gbPreVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] = 251 ;
        gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] =_OSD_No_Signal  ;

        if( (gbVXIS_OuputModeflg[VS4210ChipIndex] >= VS4210_1080P_MODE1 ) &&  (gbVXIS_OuputModeflg[VS4210ChipIndex] < VS4210_1080P_MODE_NUMBER ))
        {
            for (j = 0 ; j < gWindowNum ; j++)
            {
                if (gaHDMI_MapWindowIndex[j][VS4210ChipIndex] == gHDMI_Index )
                {
                    if (gaOpenWindowflg[j][VS4210ChipIndex] == 1 )
                    {
                        VXIS_fShowInOutMode(j ,  gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] )  ;
                        VS4210_CloseWindowsX(j);  //chunyu
                    }
                    else
                    {
                        OSD_Clear(j);
                    }
                }
            }
        }
        else if (gbVXIS_OuputModeflg[VS4210ChipIndex] <=  VS4210_1080P_MODE0_w3 )
        {
            if (gHDMI_Index == gbVXIS_OuputModeflg[VS4210ChipIndex] )
            {
                VS4210_CloseAllWindows();
                VXIS_fShowInOutMode0Fast(gHDMI_Index ,  gbVXIS_InputMode[gHDMI_Index][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] ) ;
            }
        }

        if ( bSignal[gaHDMI_MapWindowIndex[0][VS4210ChipIndex]][VS4210ChipIndex] == 0  )
        {
            VS4210_channel0clear(0 ) ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_PowerDown(void )
{
    VXISI2CWrite(0x03, 0x74 );
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_PowerResume(void )
{
    VXISI2CWrite(0x03, 0x75 );
}

///////////////////////////////////////////////////////////////////////////////////
bit VS4210_SetMutiWindows(T_VS4210_MutiWindows *pVS4210_JointKind1)
{
    tByte i ;
    tByte j ;
    tByte  tmpindex ;
    bit re = 1; 
    tByte WMap0 ;
    tByte WMap1 ;
    tByte WMap2 ;
    tByte WMap3 ;
    
    tByte  OutSizeChange ;
    tByte  OutModeChange ;

    if (isVS4210ParamInit == 1)
    {        
        VS4210_ParamInit() ;        
        isVS4210ParamInit = 0 ;
    }

    u8gDrawOSDFlg[VS4210ChipIndex] = pVS4210_JointKind1->DrawOSDFlg ;

    u8DrawBoxFlg[VS4210ChipIndex]= pVS4210_JointKind1->DrawBoxFlg ;
    u8DrawBoxColor[gVS4210I2CBus] = pVS4210_JointKind1->DrawBoxColor ;
    gu8AudioMode[VS4210ChipIndex] = pVS4210_JointKind1->AudioChannel ;

    WMap0 = pVS4210_JointKind1->WindowMap.WMap0 ;
    WMap1 = pVS4210_JointKind1->WindowMap.WMap1 ;
    WMap2 = pVS4210_JointKind1->WindowMap.WMap2 ;
    WMap3 = pVS4210_JointKind1->WindowMap.WMap3 ;

    if (pVS4210_JointKind1->OutputMode == VS4210_1080P_MODE_SystemPowerDown)
    {
        VS4210_PowerDown() ;
        re = 0 ;
    }
    else if (pVS4210_JointKind1->OutputMode == VS4210_1080P_MODE_SystemPowerResume)
    {
        VS4210_PowerResume();
        re = 0 ;
    }

    if (re == 1)
    {
        OutSizeChange = 0 ;
        OutModeChange = 0 ;
        aOpenWindowflgChange[0][VS4210ChipIndex] = 0 ;
        aOpenWindowflgChange[1][VS4210ChipIndex] = 0 ;
        aOpenWindowflgChange[2][VS4210ChipIndex] = 0 ;
        aOpenWindowflgChange[3][VS4210ChipIndex] = 0 ;

        if (pVS4210_JointKind1->OutputMode <= VS4210_1080P_MODE0_w3  )
        {
            if (WMap0 > 0 )
            {
                gbVXIS_OuputModeflg[VS4210ChipIndex] = WMap0 - 1 ;
            }
            else
            {
                gbVXIS_OuputModeflg[VS4210ChipIndex] = 0 ;
            }
        }
        else
        {
            if (pVS4210_JointKind1->OutputMode < VS4210_1080P_MODE_SystemPowerDown)
            {
                gbVXIS_OuputModeflg[VS4210ChipIndex] = pVS4210_JointKind1->OutputMode ;
            }
        }

        if (WMap0 == 0)
        {
            gaHDMI_MapWindowIndex[0][VS4210ChipIndex] = 0 ;
            gaOpenWindowflg[0][VS4210ChipIndex] = 0 ;
            PreaMapWindowIndex[0][VS4210ChipIndex]=6 ;
        }
        else
        {
            gaHDMI_MapWindowIndex[0][VS4210ChipIndex] = WMap0 - 1 ;
            gaOpenWindowflg[0][VS4210ChipIndex] = 1 ;
        }

        if (WMap1 == 0)
        {
            gaHDMI_MapWindowIndex[1][VS4210ChipIndex] = 1 ;
            gaOpenWindowflg[1][VS4210ChipIndex] = 0 ;
            PreaMapWindowIndex[1][VS4210ChipIndex]=6 ;
        }
        else
        {
            gaHDMI_MapWindowIndex[1][VS4210ChipIndex] = WMap1 - 1 ;
            gaOpenWindowflg[1][VS4210ChipIndex] = 1 ;
        }

        if (WMap2 == 0)
        {
            gaHDMI_MapWindowIndex[2][VS4210ChipIndex] = 2 ;
            gaOpenWindowflg[2][VS4210ChipIndex] = 0 ;
            PreaMapWindowIndex[2][VS4210ChipIndex]=6 ;
        }
        else
        {
            gaHDMI_MapWindowIndex[2][VS4210ChipIndex] = WMap2 - 1 ;
            gaOpenWindowflg[2][VS4210ChipIndex] = 1 ;
        }

        if (WMap3 == 0)
        {
            gaHDMI_MapWindowIndex[3][VS4210ChipIndex] = 3 ;
            gaOpenWindowflg[3][VS4210ChipIndex] = 0 ;
            PreaMapWindowIndex[3][VS4210ChipIndex]=6 ;
        }
        else
        {
            gaHDMI_MapWindowIndex[3][VS4210ChipIndex] = WMap3 - 1;
            gaOpenWindowflg[3][VS4210ChipIndex] = 1 ;
        }

        for (i = 0 ; i< 4 ; i++ )
        {
            if (PreaMapWindowIndex[i][VS4210ChipIndex] != gaHDMI_MapWindowIndex[i][VS4210ChipIndex] )
            {
                aOpenWindowflgChange[i][VS4210ChipIndex] = 1 ;
            }
        }

        gbVXIS_OuputSize[VS4210ChipIndex] = pVS4210_JointKind1->OutputSize ;
        //if (PreOuputModeflg[VS4210ChipIndex] != gbVXIS_OuputModeflg[VS4210ChipIndex] )
        //{
        gWindowNum = Get_WindowNum(gbVXIS_OuputModeflg[VS4210ChipIndex])  ;
        //}

        tmpindex = gHDMI_Index ;
        gHDMI_Index = 0 ;

        if (PreVXIS_OuputSize[VS4210ChipIndex] != gbVXIS_OuputSize[VS4210ChipIndex] )
        {
            OutSizeChange = 1 ;
            VS4210_displayoff() ;
            PreMode0inputmode [VS4210ChipIndex] = 255 ;
            PreVXIS_FULLOuputModeflg[VS4210ChipIndex] = VS4210_1080P_MODE_NUMBER   ;

            if (gbVXIS_OuputModeflg[VS4210ChipIndex] >= 0x10 )
            {
                if ( (bSignal[0][VS4210ChipIndex] == 0 ) && (bSignal[1][VS4210ChipIndex] == 0 ) && (bSignal[2][VS4210ChipIndex] == 0 ) && (bSignal[3][VS4210ChipIndex] == 0 ) )
                {
                    VS4210_No_Signal_Init();
                    OutSizeChange = 0 ;
                    VXISI2CWrite(0x32,gvideofftmp  );
                    if (u8DrawBoxFlg[VS4210ChipIndex] ==1)
                    {
                        VS4210_Line_Draw(gbVXIS_OuputModeflg[VS4210ChipIndex]);
                    }
                }
                else
                {
                    VS4210_init_Input_MODE88( gbVXIS_OuputModeflg[VS4210ChipIndex] )  ;
                }
            }
            else
            {
                VS4210_init_Input_MODE88( gbVXIS_OuputModeflg[VS4210ChipIndex] )  ;
            }
            PreOuputModeflg[VS4210ChipIndex] = gbVXIS_OuputModeflg[VS4210ChipIndex] ;
        }

        if (PreOuputModeflg[VS4210ChipIndex] != gbVXIS_OuputModeflg[VS4210ChipIndex])
        {
            if ((PreOuputModeflg[VS4210ChipIndex] <= VS4210_1080P_MODE0_w3 )&&(gbVXIS_OuputModeflg[VS4210ChipIndex] > VS4210_1080P_MODE0_w3 ))
            {
                VS4210_init_Input_MODE88( gbVXIS_OuputModeflg[VS4210ChipIndex] )  ;
            }
            OutModeChange = 1 ;
        }

        if ((OutModeChange == 1 ) || (OutSizeChange ==1 ))
        {
            VS4210_Line_Clear() ;
        }

        if (gbVXIS_OuputModeflg[VS4210ChipIndex] <= VS4210_1080P_MODE0_w3 )
        {
            if ((OutModeChange == 1) || (OutSizeChange ==1))
            {
                VS4210_CloseWindowsX(1);
                VS4210_CloseWindowsX(2);
                VS4210_CloseWindowsX(3);
                OSD_Clear_234();
            }

            if (gaOpenWindowflg[0][VS4210ChipIndex] == 1  )
            {
                if (onModd0Win0Flg[VS4210ChipIndex] == 0 )
                {
                    onMode0[VS4210ChipIndex] = 0 ;
                    onModd0Win0Flg[VS4210ChipIndex] = 1 ;
                }

                if ((aOpenWindowflgChange[0][VS4210ChipIndex] == 1 )||(OutModeChange ==1) ||(OutSizeChange ==1 ))
                {
                    VS4210_init_mode(gbVXIS_OuputModeflg[VS4210ChipIndex] , gbVXIS_InputMode[gbVXIS_OuputModeflg[VS4210ChipIndex]][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
                    VXIS_fShowInOutMode0Fast( gbVXIS_OuputModeflg[VS4210ChipIndex],  gbVXIS_InputMode[gbVXIS_OuputModeflg[VS4210ChipIndex]][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] )  ;
                }
            }
            else
            {
                VS4210_CloseWindowsX(0);
                OSD_Clear(0);
                onModd0Win0Flg[VS4210ChipIndex] = 0 ;
            }

        }
        else
        {
            if ((OutModeChange == 1 ) || (OutSizeChange == 1 ))
            {
                VS4210_displayoff() ;
            }

            for (j = 0 ; j < 4 ; j++ )
            {
                if ((OutSizeChange == 1 ) || (OutModeChange == 1 ))
                {
                    VS4210_CloseWindowsX(j);
                }
                else
                {
                    if (PreaMapWindowIndex[j][VS4210ChipIndex] != gaHDMI_MapWindowIndex[j][VS4210ChipIndex] )
                    {
                        VS4210_CloseWindowsX(j);
                    }
                    else
                    {
                        if (gaOpenWindowflg[j][VS4210ChipIndex] == 0  )
                        {
                            VS4210_CloseWindowsX(j);
                        }
                    }
                }
            }
            for (i = 0 ; i < VIDEO_IN_NUMBER ; i++ )
            {
                gHDMI_Index = i ;
                for (j = 0 ; j < gWindowNum ; j++)
                {
                    if (gaHDMI_MapWindowIndex[j][VS4210ChipIndex] == i )
                    {
                        if ((OutSizeChange == 1 ) || (OutModeChange == 1 ))
                        {
                            VS4210_init_mode( j , gbVXIS_InputMode[i][VS4210ChipIndex]  , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
                        }
                        else
                        {
                            if (PreaMapWindowIndex[j][VS4210ChipIndex] != gaHDMI_MapWindowIndex[j][VS4210ChipIndex] )
                            {
                                VS4210_init_mode( j , gbVXIS_InputMode[i][VS4210ChipIndex]  , gbVXIS_OuputModeflg[VS4210ChipIndex]) ;
                                VXIS_fShowInOutMode(j, gbVXIS_InputMode[i][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] ) ;
                            }
                        }
                    }
                }
            }

            if ((OutModeChange == 1 ) || (OutSizeChange == 1 ))
            {
                if (gWindowNum == 2 )
                {
                    OSD_Clear_34();
                }
                else if (gWindowNum == 3 )
                {
                    OSD_Clear_4();
                }
                else if (gWindowNum == 1 )
                {
                    OSD_Clear_234();
                }
            }

            for (i = 0 ; i < VIDEO_IN_NUMBER ; i++ )
            {
                gHDMI_Index = i ;
                for (j = 0 ; j < gWindowNum ; j++)
                {
                    if (gaHDMI_MapWindowIndex[j][VS4210ChipIndex] == i )
                    {
                        if ((OutSizeChange == 1 ) || (OutModeChange == 1 ))
                        {
                            VXIS_fShowInOutMode(j, gbVXIS_InputMode[i][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] ) ;
                            gu8PreAudioMode[VS4210ChipIndex] = gu8AudioMode[VS4210ChipIndex] ;
                            u8PreDrawOSDFlg[VS4210ChipIndex] = u8gDrawOSDFlg[VS4210ChipIndex] ;
                        }
                        else
                        {
                            if (PreaMapWindowIndex[j][VS4210ChipIndex] != gaHDMI_MapWindowIndex[j][VS4210ChipIndex] )
                            {
                                if (gbVXIS_InputMode[i][VS4210ChipIndex] != _OSD_No_Signal  )
                                {
                                    VXIS_fShowInOutMode(j, gbVXIS_InputMode[i][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] ) ;
                                }
                                gu8PreAudioMode[VS4210ChipIndex] = gu8AudioMode[VS4210ChipIndex] ;
                            }
                        }
                    }
                }
            }

            for (j = 0 ; j < gWindowNum ; j++)
            {
                if (gaOpenWindowflg[j][VS4210ChipIndex] == 0 )
                {
                    OSD_Clear(j);
                }
            }
            if (pVS4210_JointKind1->OutputMode > VS4210_1080P_MODE0_w3  )
            {
                for (i = 0 ; i < VIDEO_IN_NUMBER ; i++ )
                {
                    for ( j = 0 ; j < gWindowNum ; j++)
                    {
                        if (gaHDMI_MapWindowIndex[j][VS4210ChipIndex] == i )
                        {
                            if (gaOpenWindowflg[j][VS4210ChipIndex] == 1 )
                            {
                                if (bSignal[i][VS4210ChipIndex] == 1 )
                                {
                                    VS4210_OpenWindowsX(j);
                                }
/*
                                if ((OutSizeChange == 1 ) || (OutModeChange == 1 ))
                                {
                                    if (bSignal[i][VS4210ChipIndex] == 1 )
                                    {
                                        VS4210_OpenWindowsX(j);
                                    }
                                }
                                else
                                {
                                    if (bSignal[i][VS4210ChipIndex] == 1 )
                                    {
                                        VS4210_OpenWindowsX(j);
                                    }
                                }
*/
                            }
                        }
                    }
                }
            }
            else
            {
                if (gaOpenWindowflg[0][VS4210ChipIndex] == 1 )
                {
                    VS4210_OpenWindowsX(0);
                }
            }
            
            if ( bSignal[gaHDMI_MapWindowIndex[0][VS4210ChipIndex]][VS4210ChipIndex] == 0  )
            {
                //if (isfirstinit[VS4210ChipIndex] == 0 )
                //{
                    VS4210_channel0clear(0 ) ;
                //}
            }

            if ((OutModeChange == 1 ) || (OutSizeChange == 1 ))
            {
                VS4210_displayon() ;
            }
        }

        gHDMI_Index = tmpindex ;
        PreVXIS_OuputSize[VS4210ChipIndex] = gbVXIS_OuputSize[VS4210ChipIndex] ;
        PreOuputModeflg[VS4210ChipIndex] = gbVXIS_OuputModeflg[VS4210ChipIndex] ;

        if (u8DrawBoxFlg[VS4210ChipIndex] ==1 )
        {
            if ((OutModeChange == 1 ) || (OutSizeChange == 1 ))
            {
                u8PreDrawBoxFlg[VS4210ChipIndex] = u8DrawBoxFlg[VS4210ChipIndex] ;
                u8PreDrawBoxColor[gVS4210I2CBus] = u8DrawBoxColor[gVS4210I2CBus] ;
                VS4210_Line_Draw(gbVXIS_OuputModeflg[VS4210ChipIndex]);
            }
            else
            {
                if (u8DrawBoxColor[gVS4210I2CBus] != u8PreDrawBoxColor[gVS4210I2CBus] )
                {
                    u8PreDrawBoxColor[gVS4210I2CBus] = u8DrawBoxColor[gVS4210I2CBus] ;
                }

                if (u8PreDrawBoxFlg[VS4210ChipIndex] != u8DrawBoxFlg[VS4210ChipIndex] )
                {
                    u8PreDrawBoxFlg[VS4210ChipIndex]  = u8DrawBoxFlg[VS4210ChipIndex] ;
                    VS4210_Line_Draw(gbVXIS_OuputModeflg[VS4210ChipIndex]);
                }
            }
        }
        else
        {
            u8PreDrawBoxColor[gVS4210I2CBus] = 0xFF ;
            VS4210_Line_Clear() ;
            u8PreDrawBoxFlg[VS4210ChipIndex] = 0 ;
        }

        if (gbVXIS_OuputModeflg[VS4210ChipIndex] <=  VS4210_1080P_MODE0_w3 )
        {
            if ((gu8PreAudioMode[VS4210ChipIndex] != gu8AudioMode[VS4210ChipIndex]) || (u8PreDrawOSDFlg != u8gDrawOSDFlg))
            {
                gu8PreAudioMode[VS4210ChipIndex] = gu8AudioMode[VS4210ChipIndex] ;
                u8PreDrawOSDFlg[VS4210ChipIndex] = u8gDrawOSDFlg[VS4210ChipIndex] ;
                VXIS_fShowInOutMode0Fast(gbVXIS_OuputModeflg[VS4210ChipIndex] ,  gbVXIS_InputMode[gbVXIS_OuputModeflg[VS4210ChipIndex]][VS4210ChipIndex] , gbVXIS_OuputModeflg[VS4210ChipIndex] ) ;
            }
        }
        else
        {
            if ((gu8PreAudioMode[VS4210ChipIndex] != gu8AudioMode[VS4210ChipIndex]) || (u8PreDrawOSDFlg != u8gDrawOSDFlg))
            {
                gu8PreAudioMode[VS4210ChipIndex] = gu8AudioMode[VS4210ChipIndex] ;
                u8PreDrawOSDFlg[VS4210ChipIndex] = u8gDrawOSDFlg[VS4210ChipIndex] ;

                for (i = 0 ; i < VIDEO_IN_NUMBER ; i++ )
                {
                    VXIS_fShowInOutMode(i,gbVXIS_InputMode[gaHDMI_MapWindowIndex[i][VS4210ChipIndex]][VS4210ChipIndex], gbVXIS_OuputModeflg[VS4210ChipIndex] ) ;
                }
            }
        }

        for(j = 0 ; j < 4 ; j++)
        {
            if (gaOpenWindowflg[j][VS4210ChipIndex] != 0)
            {
                PreaMapWindowIndex[j][VS4210ChipIndex] = gaHDMI_MapWindowIndex[j][VS4210ChipIndex] ;
            }
            else
            {
                PreaMapWindowIndex[j][VS4210ChipIndex] = 5 ;
            }
        }
    }

    isfirstinit[VS4210ChipIndex] = 0 ;
    return re ;
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_SetInputSizeHV(tByte ch ,tWord HActive , tWord VActive , tWord HTotal, tWord VTotal  )
{
    gHActive[ch][VS4210ChipIndex] = HActive ;
    gVActive[ch][VS4210ChipIndex]= VActive  ;
    gHTotal[ch][VS4210ChipIndex] = HTotal ;
    gVTotal[ch][VS4210ChipIndex] = VTotal ;
}

void VS4210_SwitchChip(tByte i2cChipAddr, tByte chipIndex )
{
    VS4210_ADDR = i2cChipAddr ;
    VS4210ChipIndex = chipIndex ;
    if (chipIndex < 4 )
    {        
        gVS4210I2CBus = 0 ;
    }
    else
    {
        gVS4210I2CBus = 1 ;
    }    
}

void VS4210_FastClearLineAndOSD(void )
{

//Clear Line
    VXISI2CWrite( 0xB8 , 0x80 );
    VXISI2CWrite( 0xB9 , 0x00 );

//Clear OSD
    OSD_Clear_All( );

//Reset State
    u8PreDrawOSDFlg[VS4210ChipIndex] = 0x00 ;
    u8PreDrawBoxFlg[VS4210ChipIndex] = 0;

}

