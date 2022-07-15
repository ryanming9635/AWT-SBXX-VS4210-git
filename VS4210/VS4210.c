/** @file VS4210.c
 * @brief VS4210setup program
 *
 * Processing YUV422 BT656 In , output rgb24bits to HDMI.
 *
 * @author Cunu-Yu
 * @date 2019/06/08
 */

#include "VXIS_Extern_Define.h"
#include "VS4210.h"
#include "VX_OSD_Simple.h"
#include "VX_swi2c.h"

#include "VS4210_param_Mode88.h"
#include "VS4210_param_Modex26.h"

#include "keyremo.h"
#include "tpinc/TP2802.h"
#include "Printf.h"

tByte gabVXIS_Input_mirrorH[4] = {0, 0, 0, 0};
static tByte u8PreDrawOSDFlg = 0x04; // Title+Bar
tByte u8gDrawOSDFlg = 0x03;          // Title+Bar
tByte u8DrawBoxFlg = 1;
tByte u8DrawBoxColor = 0x00;
static tByte u8PreDrawBoxColor = 0xFF;
tByte gu8AudioMode = 0;
static tByte gu8PreAudioMode = 6;

tByte gbVXIS_OuputSize;
static tByte gaOpenWindowflg[4] = {1, 1, 1, 1};
// static
tByte bSignal[4];

tByte bytVideoInput = 0;
tByte g4210VinIndex;
tByte gbVXIS_OuputModeflg = 0x50; // 0x10 ;
tByte gbVXIS_InputMode[4];
tByte gaHDMI_MapWindowIndex[4];

static tByte PreaMapWindowIndex[4] = {5, 5, 5, 5};
static tByte aOpenWindowflgChange[4] = {1, 1, 1, 1};
static tByte PreOuputModeflg = VS4210_1080P_MODE_NUMBER;
static tByte PreVXIS_OuputSize = _OUTPUT_NUMBER;

tByte gWindowNum;
static tByte PreVXIS_FULLOuputModeflg = 0;
static tByte gvideofftmp = 128;

BYTE ResetCamMirror = 0;

extern BYTE NowModeState;
extern BYTE CAMA_M;
extern BYTE CAMB_M;
extern BYTE CAMC_M;
extern BYTE CAMR_M;
extern BYTE CAMD_M;
extern BYTE bytSelectCAMRD;
extern BYTE OSD_MIRROR[4];
extern BYTE TriggerFlag;
extern BYTE TriggerStatus;
extern BYTE CURRENT_CH;
extern BYTE SelectModeType;
extern BYTE VideoChangeFlag;
extern BYTE BOOT_ISSUE;
extern BYTE bytResetOSDValue;

void VS4210_displayon(void);
void VS4210_displayoff(void);

void VS4210_ch0_NoSignal_Rst(tByte ch); ////20210820@start

///////////////////////////////////////////////////////////////////////////////////
// static void VS4210_FULL88init( tcByte *Addrn, tcByte *Valn )
static void VS4210_FULL88init(tcByte *Valn)
{
    tByte i;
    tByte addr;
    tByte i2cVal;
    tcByte *Addrn;
    Addrn = &VS4210_Addr_88param[0];

    for (i = 0; i < VS4210_INIT_NUMBER; i++)
    {
        addr = Addrn[i];
        i2cVal = Valn[i];
        VXISI2CWrite(addr, i2cVal);
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_init_Input_MODE88(tByte out_Mode)
{
    tByte tmpFULLOuputModeflg;
    tmpFULLOuputModeflg = out_Mode & 0xF0;

    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        if (PreVXIS_FULLOuputModeflg != tmpFULLOuputModeflg)
        {
            if ((PreVXIS_FULLOuputModeflg == 0) || (tmpFULLOuputModeflg == 0))
            {
                PreVXIS_FULLOuputModeflg = tmpFULLOuputModeflg;
                VS4210_FULL88init(VS4210_bt656_720P_720P_mode0_w0_88param);
            }
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        if (PreVXIS_FULLOuputModeflg != tmpFULLOuputModeflg)
        {
            if ((PreVXIS_FULLOuputModeflg == 0) || (tmpFULLOuputModeflg == 0))
            {
                PreVXIS_FULLOuputModeflg = tmpFULLOuputModeflg;
                VS4210_FULL88init(VS4210_bt656_1080P_1080P_mode0_w0_88param);
            }
        }
    }
    else
    {
        if (PreVXIS_FULLOuputModeflg != tmpFULLOuputModeflg)
        {
            if ((PreVXIS_FULLOuputModeflg == 0) || (tmpFULLOuputModeflg == 0))
            {
                PreVXIS_FULLOuputModeflg = tmpFULLOuputModeflg;
#if (TH0701024600NYR50S1 == OFF)
                VS4210_FULL88init(VS4210_bt656_720P_800x480_mode0_w0_88param);
#else
                VS4210_FULL88init(VS4210_bt656_720P_1024x600_mode0_w0_88param);
#endif
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_Line_Clear(void)
{
    VXISI2CWrite(0xB8, 0x80);
    VXISI2CWrite(0xB9, 0x00);
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_Line_Draw(tByte mode)
{
    tByte i;
    tByte addr;
    tByte i2cVal;
    if (u8DrawBoxFlg == 1)
    {
        if (mode == VS4210_1080P_MODE1)
        {
            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode1Lin1[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode1Lin1[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode1Lin1[i];
#else
                    i2cVal = aLineMode1024x600Mode1Lin1[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }

            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode1Lin2[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode1Lin2[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode1Lin2[i];
#else
                    i2cVal = aLineMode1024x600Mode1Lin2[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                    msleep(3 + 3);
                }
                VXISI2CWrite(addr, i2cVal);
            }
        }
#if 1
        if (mode == VS4210_1080P_MODE10)
        {
            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode8Lin1[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode8Lin1[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode10Lin1[i];
#else
                    i2cVal = aLineMode1024x600Mode10Lin1[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }

            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode8Lin2[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode8Lin2[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode10Lin2[i];
#else
                    i2cVal = aLineMode1024x600Mode10Lin2[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                    msleep(3 + 3);
                }
                VXISI2CWrite(addr, i2cVal);
            }
        }

#else
        if (mode == VS4210_1080P_MODE2)
        {
            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode2Lin1[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode2Lin1[i];
                }
                else
                {
                    i2cVal = aLineMode800x480Mode2Lin1[i];
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }

            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode2Lin2[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode2Lin2[i];
                }
                else
                {
                    i2cVal = aLineMode800x480Mode2Lin2[i];
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }

            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode2Lin3[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode2Lin3[i];
                }
                else
                {
                    i2cVal = aLineMode800x480Mode2Lin3[i];
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }
        }
#endif
        if (mode == VS4210_1080P_MODE8)
        {
            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode8Lin1[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode8Lin1[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode8Lin1[i];
#else
                    i2cVal = aLineMode1024x600Mode8Lin1[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }

            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode8Lin2[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode8Lin2[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode8Lin2[i];
#else
                    i2cVal = aLineMode1024x600Mode8Lin2[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                    msleep(3 + 3);
                }
                VXISI2CWrite(addr, i2cVal);
            }
        }
        if (mode == VS4210_1080P_MODE9)
        {
            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode9Lin1[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode9Lin1[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode9Lin1[i];
#else
                    i2cVal = aLineMode1024x600Mode9Lin1[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }

            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode9Lin2[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode9Lin2[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode9Lin2[i];
#else
                    i2cVal = aLineMode1024x600Mode9Lin2[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }
        }
        if (mode == VS4210_1080P_MODE12)
        {
            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode12Lin1[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode12Lin1[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode12Lin1[i];
#else
                    i2cVal = aLineMode1024x600Mode12Lin1[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }
        }
        if (mode == VS4210_1080P_MODE14)
        {
            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode14Lin1[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode14Lin1[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode14Lin1[i];
#else
                    i2cVal = aLineMode1024x600Mode14Lin1[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }

            for (i = 0; i < 7; i++)
            {
                addr = aLineModeReg[i];
                if (gbVXIS_OuputSize == _OUTPUT_1080P60)
                {
                    i2cVal = aLineMode1080PMode14Lin2[i];
                }
                else if (gbVXIS_OuputSize == _OUTPUT_720P60)
                {
                    i2cVal = aLineMode720PMode14Lin2[i];
                }
                else
                {
#if (TH0701024600NYR50S1 == OFF)
                    i2cVal = aLineMode800x480Mode14Lin2[i];
#else
                    i2cVal = aLineMode1024x600Mode14Lin2[i];
#endif
                }

                if (i == 6)
                {
                    i2cVal |= u8DrawBoxColor;
                }
                VXISI2CWrite(addr, i2cVal);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_SwitchChannel(tByte ch, tByte win)
{
    tByte i2cVal;

    i2cVal = VXISI2CRead(0x02);
    i2cVal = (i2cVal & 0xFC) | win;
    VXISI2CWrite(0x02, i2cVal);

    i2cVal = VXISI2CRead(0x10);
    i2cVal = (i2cVal & 0xFC) | ch;

    VXISI2CWrite(0x10, i2cVal);

    // Printf("\r\n**%s,%d,ch=%d,win=%d", __func__,__LINE__ ,ch , win) ;
}

void VS4210_SetCamMirror(tByte ch, tByte win)
{
    tByte i2cVal;

    i2cVal = VXISI2CRead(0x02);
    i2cVal = (i2cVal & 0xFC) | win;
    VXISI2CWrite(0x02, i2cVal);

    i2cVal = VXISI2CRead(0x10);
    i2cVal = (i2cVal & 0xFC) | ch;
    i2cVal |= 0x04; // mirror
    VXISI2CWrite(0x10, i2cVal);
}

void VS4210_CLRCamMirror(tByte ch, tByte win)
{
    tByte i2cVal;

    i2cVal = VXISI2CRead(0x02);
    i2cVal = (i2cVal & 0xFC) | win;
    VXISI2CWrite(0x02, i2cVal);

    i2cVal = VXISI2CRead(0x10);
    i2cVal = (i2cVal & 0xFC) | ch;
    i2cVal &= 0xFB; // clr mirror
    VXISI2CWrite(0x10, i2cVal);
}

void VS4210_ResetCamMirror(tByte ch)
{

    //  	#if(	_DEBUG_MESSAGE_Source_Change==ON)
    //					GraphicsPrint(RED,"(1ResetCamMirror)");
    //					#endif
    switch (CURRENT_CH)
    {
    case VS4210_LOOP_MODE0_w0:
        if (ch == CH1)
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE0_w1:
        if (ch == CH2)
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE0_w2:
        if (ch == CH3)
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE0_w3:
    case VS4210_LOOP_MODE0_w4:
        if (ch == CH4)
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE12_12:
    case VS4210_LOOP_MODE12_21:
        if ((ch == CH1) || (ch == CH2))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE12_14:
        if ((ch == CH1) || (ch == CH4))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE10_123:
    case VS4210_LOOP_MODE8:
    case VS4210_LOOP_MODE8_123:
    case VS4210_LOOP_MODE8_213:
    case VS4210_LOOP_MODE8_312:
    case VS4210_LOOP_MODE9:
        if ((ch == CH1) || (ch == CH2) || (ch == CH3))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;

    case VS4210_LOOP_MODE12_23:
    case VS4210_LOOP_MODE12_32:
        if ((ch == CH2) || (ch == CH3))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE12_34:
    case VS4210_LOOP_MODE12_43:
        if ((ch == CH4) || (ch == CH3))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE12_31:
    case VS4210_LOOP_MODE12_13:
        if ((ch == CH1) || (ch == CH3))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE1:
    case VS4210_LOOP_MODE2:
        PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE12_42:
    case VS4210_LOOP_MODE12_24:
        if ((ch == CH4) || (ch == CH2))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE8_412:
    case VS4210_LOOP_MODE8_124:
    case VS4210_LOOP_MODE8_214:
    case VS4210_LOOP_MODE10_412:
    case VS4210_LOOP_MODE9_412:
        if ((ch == CH4) || (ch == CH1) || (ch == CH2))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE8_134:
    case VS4210_LOOP_MODE8_314:
    case VS4210_LOOP_MODE10_413:
    case VS4210_LOOP_MODE8_413:
        if ((ch == CH1) || (ch == CH3) || (ch == CH4))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    case VS4210_LOOP_MODE8_234:
    case VS4210_LOOP_MODE8_324:
    case VS4210_LOOP_MODE10_423:
    case VS4210_LOOP_MODE8_423:
        if ((ch == CH2) || (ch == CH3) || (ch == CH4))
            PreVXIS_OuputSize = _OUTPUT_NUMBER;
        break;
    default:
        break;
    }

    if (PreVXIS_OuputSize == _OUTPUT_NUMBER)
        ResetCamMirror = TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_OpenWindowsX(tByte n)
{
    tByte i2cVal;

    if (gbVXIS_OuputModeflg < VS4210_1080P_MODE1)
    {
        i2cVal = VXISI2CRead(0x02);
        i2cVal = (i2cVal & 0xFC); // 0
    }
    else
    {
        i2cVal = VXISI2CRead(0x02);
        i2cVal = (i2cVal & 0xFC) | n;
    }

    VXISI2CWrite(0x02, i2cVal);
    i2cVal = VXISI2CRead(0x10);
    i2cVal = i2cVal | 0x80;

    VXISI2CWrite(0x10, i2cVal);

    // 20210820@start
    if (bSignal[gaHDMI_MapWindowIndex[0]] == 0)
    {
        VS4210_ch0_NoSignal_Rst(0);
        i2cVal = VXISI2CRead(0x02);
        i2cVal = (i2cVal & 0xFC) | n;
        VXISI2CWrite(0x02, i2cVal);
    }
    else
    {
        msleep(50);
    }
    //    msleep(150) ;
    // 20210820@end

    VXISI2CWrite(0xE9, 0);
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_CloseWindowsX(tByte n)
{
    tByte i2cVal;
    i2cVal = VXISI2CRead(0x9C);
#if (TH0701024600NYR50S1 == ON)
    VXISI2CWrite(0x9C, i2cVal | (0x0A)); /// add no signal blue screen
#else
    VXISI2CWrite(0x9C, i2cVal | (No_Sgnal_Blue_Screen)); /// add no signal blue screen
#endif
    //    VXISI2CWrite( 0x9C, i2cVal | 0x08 ); ///add no signal blue screen
    i2cVal = VXISI2CRead(0x02);
    i2cVal = (i2cVal & 0xFC) | n;

    VXISI2CWrite(0x02, i2cVal);
    i2cVal = VXISI2CRead(0x10);
    i2cVal = i2cVal & 0x7F;
    VXISI2CWrite(0x10, i2cVal);
    VXISI2CWrite(0xE9, 0x50);
    // Printf("(Close ch=%02x)",(WORD)n);
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_CloseAllWindows(void)
{
    tByte i2cVal;
    tByte i;
    tByte tmp;

    i2cVal = VXISI2CRead(0x9C);
    VXISI2CWrite(0x9C, i2cVal | No_Sgnal_Blue_Screen); /// add no signal blue screen
#if (TH0701024600NYR50S1 == ON)
    VXISI2CWrite(0x9C, 0x0A);
#endif
    //   VXISI2CWrite( 0x9C, i2cVal | 0x08 );///add no signal blue screen

    tmp = VXISI2CRead(0x02);

    for (i = 0; i < HDMI_IN_NUMBER; i++)
    {
        VXISI2CWrite(0x02, i);
        i2cVal = VXISI2CRead(0x10);
        i2cVal = i2cVal & 0x7F;
        VXISI2CWrite(0x10, i2cVal);
    }
    VXISI2CWrite(0x02, tmp);
    // Printf("(2Close ALL)");
}

void VS4210_ch0_NoSignal_Rst(tByte ch)
{
    tByte i2cVal;
    tByte tmp;
    tByte tmp2;

    i2cVal = VXISI2CRead(0x02);
    i2cVal = ((i2cVal & 0xFC) | ch);

    VXISI2CWrite(0x02, i2cVal);
    tmp = VXISI2CRead(0x11);
    tmp2 = tmp;
    i2cVal = tmp & 0x80;

    if (i2cVal == 0x80)
    {
        tmp = tmp & 0x7F;
    }
    else
    {
        tmp = tmp | 0x80;
    }
    VXISI2CWrite(0x11, tmp);
    msleep(100);
    VXISI2CWrite(0x11, tmp2);
    msleep(100);

    VXISI2CWrite(0x11, tmp);
    msleep(100);
    VXISI2CWrite(0x11, tmp2);
}

///////////////////////////////////////////////////////////////////////////////////
/*
static void VS4210_init( tcByte *Addrn, tcByte *Valn )
{
    tByte i ;
    tByte addr ;
    tByte i2cVal ;

    i2cVal =  VXISI2CRead(0x9C ) ;
    VXISI2CWrite( 0x9C, i2cVal | 0x0A ); ///add no signal blue screen

    for (i=0 ; i< VS4210_INIT_MIN_NUM ; i++)
    {
        addr = Addrn[i] ;
        i2cVal  = Valn[i];
        VXISI2CWrite( addr,i2cVal);
    }
}
*/
static void VS4210_init(tcByte *Valn)
{
    tByte i;
    tByte addr;
    tByte i2cVal;
    tcByte *Addrn;
    Addrn = &VS4210_Addr_param[0];

#if (TH0701024600NYR50S1 == OFF)
    i2cVal = VXISI2CRead(0x9C);
    VXISI2CWrite(0x9C, i2cVal | (No_Sgnal_Blue_Screen)); /// add4no signal blue screen
#endif
    //    VXISI2CWrite( 0x9C, i2cVal | (0x08) );///add4no signal blue screen

    for (i = 0; i < VS4210_INIT_MIN_NUM; i++)
    {
        addr = Addrn[i];
        i2cVal = Valn[i];

        // 20210820@start
        if (addr == 0x10)
        {
            i2cVal = i2cVal & 0x7F;
        }

        if (addr == 0xE9)
        {
            i2cVal = 0x50;
        }
        // 20210820@end

        VXISI2CWrite(addr, i2cVal);
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_clearAllsignal(void)
{
    tByte i;
    tByte i2cVal;

    i2cVal = VXISI2CRead(0x9C);
#if (TH0701024600NYR50S1 == ON)
    if ((gbVXIS_OuputModeflg <= VS4210_1080P_MODE0_w4) /*&& BOOT_ISSUE*/)
        VXISI2CWrite(0x9C, 0x0A); /// add no signal blue screen
    else
#endif
        VXISI2CWrite(0x9C, i2cVal | (No_Sgnal_Blue_Screen)); /// add no signal blue screen
    //    VXISI2CWrite( 0x9C, i2cVal | 0x08 );///add no signal blue screen

    for (i = 0; i < HDMI_IN_NUMBER; i++)
    {
        i2cVal = VXISI2CRead(0x02);
        i2cVal = (i2cVal & 0xFC) | i;
        VXISI2CWrite(0x02, i2cVal);

        i2cVal = VXISI2CRead(0x10);
        i2cVal = i2cVal & 0x7F;
        VXISI2CWrite(0x10, i2cVal);
    }
    // Printf("(3Close ALL)");
}
///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE0_w0(tByte in_Mode)
{
    tByte tmpFULLOuputModeflg;
    tmpFULLOuputModeflg = gbVXIS_OuputModeflg & 0xF0;
    PreVXIS_FULLOuputModeflg = tmpFULLOuputModeflg;
    VS4210_displayoff();

    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_FULL88init(VS4210_bt656_NTSC_720P_mode0_w0_88param);
            break;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_bt656_PAL_720P_mode0_w0_88param);
            break;
        case _OSD_1280X720P:
            VS4210_FULL88init(VS4210_bt656_720P_720P_mode0_w0_88param);
            break;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_bt656_1080P_720P_mode0_w0_88param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_FULL88init(VS4210_bt656_NTSC_1080P_mode0_w0_88param);
            break;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_bt656_PAL_1080P_mode0_w0_88param);
            break;
        case _OSD_1280X720P:
            VS4210_FULL88init(VS4210_bt656_720P_1080P_mode0_w0_88param);
            break;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_bt656_1080P_1080P_mode0_w0_88param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_FULL88init(VS4210_bt656_NTSC_800x480_mode0_w0_88param);
            break;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_bt656_PAL_800x480_mode0_w0_88param);
            break;
        case _OSD_1280X720P:
            VS4210_FULL88init(VS4210_bt656_720P_800x480_mode0_w0_88param);
            break;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_bt656_1080P_800x480_mode0_w0_88param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_FULL88init(VS4210_bt656_NTSC_1024x600_mode0_w0_88param);
            break;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_bt656_PAL_1024x600_mode0_w0_88param);
            break;
        case _OSD_1280X720P:
            VS4210_FULL88init(VS4210_bt656_720P_1024x600_mode0_w0_88param);
            break;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_bt656_1080P_1024x600_mode0_w0_88param);
            break;
        }

#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w0(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode1_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode1_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode1_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode1_w0_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode1_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode1_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode1_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode1_w0_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode1_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode1_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode1_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode1_w0_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode1_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode1_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode1_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode1_w0_param);
            break;
        }

#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w1(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode1_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode1_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode1_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode1_w1_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode1_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode1_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode1_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode1_w1_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode1_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode1_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode1_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode1_w1_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode1_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode1_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode1_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode1_w1_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w2(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode1_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode1_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode1_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode1_w2_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode1_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode1_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode1_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode1_w2_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode1_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode1_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode1_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode1_w2_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode1_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode1_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode1_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode1_w2_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w3(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode1_w3_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode1_w3_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode1_w3_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode1_w3_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode1_w3_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode1_w3_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode1_w3_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode1_w3_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode1_w3_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode1_w3_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode1_w3_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode1_w3_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode1_w3_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode1_w3_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode1_w3_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode1_w3_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE2_w0(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode2_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode2_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode2_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode2_w0_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode2_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode2_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode2_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode2_w0_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode2_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode2_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode2_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode2_w0_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode2_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode2_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode2_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode2_w0_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE2_w1(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode2_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode2_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode2_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode2_w1_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode2_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode2_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode2_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode2_w1_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode2_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode2_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode2_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode2_w1_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode2_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode2_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode2_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode2_w1_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE2_w2(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode2_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode2_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode2_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode2_w2_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode2_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode2_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode2_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode2_w2_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode2_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode2_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode2_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode2_w2_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode2_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode2_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode2_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode2_w2_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE2_w3(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode2_w3_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode2_w3_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode2_w3_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode2_w3_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode2_w3_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode2_w3_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode2_w3_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode2_w3_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode2_w3_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode2_w3_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode2_w3_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode2_w3_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode2_w3_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode2_w3_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode2_w3_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode2_w3_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE8_w0(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode8_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode8_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode8_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode8_w0_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode8_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode8_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode8_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode8_w0_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode8_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode8_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode8_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode8_w0_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode8_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode8_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode8_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode8_w0_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE8_w1(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode8_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode8_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode8_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode8_w1_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode8_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode8_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode8_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode8_w1_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode8_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode8_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode8_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode8_w1_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode8_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode8_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode8_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode8_w1_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE8_w2(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode8_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode8_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode8_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode8_w2_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode8_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode8_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode8_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode8_w2_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode8_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode8_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode8_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode8_w2_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode8_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode8_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode8_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode8_w2_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE9_w0(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode9_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode9_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode9_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode9_w0_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode9_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode9_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode9_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode9_w0_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode9_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode9_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode9_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode9_w0_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode9_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode9_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode9_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode9_w0_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE9_w1(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode9_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode9_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode9_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode9_w1_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode9_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode9_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode9_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode9_w1_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode9_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode9_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode9_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode9_w1_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode9_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode9_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode9_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode9_w1_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE9_w2(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode9_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode9_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode9_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode9_w2_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode9_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode9_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode9_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode9_w2_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode9_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode9_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode9_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode9_w2_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode9_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode9_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode9_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode9_w2_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE10_w0(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode10_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode10_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode10_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode10_w0_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode10_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode10_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode10_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode10_w0_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode10_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode10_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode10_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode10_w0_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode10_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode10_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode10_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode10_w0_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE10_w1(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode10_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode10_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode10_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode10_w1_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode10_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode10_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode10_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode10_w1_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode10_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode10_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode10_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode10_w1_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode10_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode10_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode10_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode10_w1_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE10_w2(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode10_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode10_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode10_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode10_w2_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode10_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode10_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode10_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode10_w2_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode10_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode10_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode10_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode10_w2_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode10_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode10_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode10_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode10_w2_param);
            break;
        }
#endif
    }
}
///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE12_w0(tByte in_Mode)
{

    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode12_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode12_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode12_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode12_w0_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode12_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode12_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode12_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode12_w0_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode12_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode12_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode12_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode12_w0_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode12_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode12_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode12_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode12_w0_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE12_w1(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode12_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode12_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode12_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode12_w1_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode12_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode12_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode12_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode12_w1_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode12_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode12_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode12_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode12_w1_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode12_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode12_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode12_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode12_w1_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE14_w0(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode14_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode14_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode14_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode14_w0_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode14_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode14_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode14_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode14_w0_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode14_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode14_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode14_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode14_w0_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode14_w0_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode14_w0_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode14_w0_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode14_w0_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE14_w1(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode14_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode14_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode14_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode14_w1_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode14_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode14_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode14_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode14_w1_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode14_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode14_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode14_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode14_w1_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode14_w1_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode14_w1_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode14_w1_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode14_w1_param);
            break;
        }
#endif
    }
}

static void VS4210_init_Input_MODE14_w2(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode14_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode14_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode14_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode14_w2_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode14_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode14_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode14_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode14_w2_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode14_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode14_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode14_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode14_w2_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode14_w2_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode14_w2_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode14_w2_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode14_w2_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////

static void VS4210_init_Input_MODE0_w0_mirrorH(tByte in_Mode)
{
    tByte tmpFULLOuputModeflg;
    tmpFULLOuputModeflg = gbVXIS_OuputModeflg & 0xF0;
    PreVXIS_FULLOuputModeflg = tmpFULLOuputModeflg;
    VS4210_displayoff();

    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_FULL88init(VS4210_bt656_NTSC_720P_mode0_w0_mirrorH_88param);
            break;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_bt656_PAL_720P_mode0_w0_mirrorH_88param);
            break;
        case _OSD_1280X720P:
            VS4210_FULL88init(VS4210_bt656_720P_720P_mode0_w0_mirrorH_88param);
            break;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_bt656_1080P_720P_mode0_w0_mirrorH_88param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_FULL88init(VS4210_bt656_NTSC_1080P_mode0_w0_mirrorH_88param);
            break;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_bt656_PAL_1080P_mode0_w0_mirrorH_88param);
            break;
        case _OSD_1280X720P:
            VS4210_FULL88init(VS4210_bt656_720P_1080P_mode0_w0_mirrorH_88param);
            break;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_bt656_1080P_1080P_mode0_w0_mirrorH_88param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_FULL88init(VS4210_bt656_NTSC_800x480_mode0_w0_mirrorH_88param);
            break;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_bt656_PAL_800x480_mode0_w0_mirrorH_88param);
            break;
        case _OSD_1280X720P:
            VS4210_FULL88init(VS4210_bt656_720P_800x480_mode0_w0_mirrorH_88param);
            break;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_bt656_1080P_800x480_mode0_w0_mirrorH_88param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_FULL88init(VS4210_bt656_NTSC_1024x600_mode0_w0_mirrorH_88param);
            break;
        case _OSD_720X576I50:
            VS4210_FULL88init(VS4210_bt656_PAL_1024x600_mode0_w0_mirrorH_88param);
            break;
        case _OSD_1280X720P:
            VS4210_FULL88init(VS4210_bt656_720P_1024x600_mode0_w0_mirrorH_88param);
            break;
        case _OSD_1920X1080P:
            VS4210_FULL88init(VS4210_bt656_1080P_1024x600_mode0_w0_mirrorH_88param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w0_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode1_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode1_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode1_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode1_w0_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode1_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode1_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode1_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode1_w0_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode1_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode1_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode1_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode1_w0_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode1_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode1_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode1_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode1_w0_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w1_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode1_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode1_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode1_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode1_w1_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode1_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode1_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode1_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode1_w1_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode1_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode1_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode1_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode1_w1_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode1_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode1_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode1_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode1_w1_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w2_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode1_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode1_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode1_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode1_w2_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode1_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode1_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode1_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode1_w2_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode1_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode1_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode1_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode1_w2_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode1_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode1_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode1_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode1_w2_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE1_w3_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode1_w3_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode1_w3_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode1_w3_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode1_w3_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode1_w3_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode1_w3_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode1_w3_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode1_w3_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode1_w3_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode1_w3_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode1_w3_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode1_w3_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode1_w3_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode1_w3_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode1_w3_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode1_w3_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE2_w0_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode2_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode2_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode2_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode2_w0_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode2_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode2_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode2_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode2_w0_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode2_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode2_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode2_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode2_w0_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode2_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode2_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode2_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode2_w0_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE2_w1_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode2_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode2_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode2_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode2_w1_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode2_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode2_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode2_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode2_w1_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode2_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode2_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode2_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode2_w1_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode2_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode2_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode2_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode2_w1_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE2_w2_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode2_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode2_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode2_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode2_w2_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode2_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode2_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode2_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode2_w2_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode2_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode2_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode2_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode2_w2_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode2_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode2_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode2_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode2_w2_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE2_w3_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode2_w3_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode2_w3_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode2_w3_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode2_w3_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode2_w3_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode2_w3_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode2_w3_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode2_w3_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode2_w3_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode2_w3_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode2_w3_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode2_w3_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode2_w3_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode2_w3_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode2_w3_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode2_w3_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE8_w0_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode8_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode8_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode8_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode8_w0_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode8_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode8_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode8_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode8_w0_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode8_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode8_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode8_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode8_w0_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode8_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode8_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode8_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode8_w0_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE8_w1_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode8_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode8_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode8_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode8_w1_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode8_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode8_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode8_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode8_w1_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode8_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode8_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode8_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode8_w1_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode8_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode8_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode8_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode8_w1_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE8_w2_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode8_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode8_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode8_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode8_w2_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode8_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode8_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode8_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode8_w2_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode8_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode8_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode8_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode8_w2_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode8_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode8_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode8_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode8_w2_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE9_w0_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode9_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode9_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode9_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode9_w0_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode9_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode9_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode9_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode9_w0_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode9_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode9_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode9_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode9_w0_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode9_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode9_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode9_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode9_w0_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE9_w1_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode9_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode9_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode9_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode9_w1_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode9_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode9_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode9_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode9_w1_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode9_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode9_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode9_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode9_w1_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode9_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode9_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode9_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode9_w1_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE9_w2_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode9_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode9_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode9_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode9_w2_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode9_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode9_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode9_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode9_w2_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode9_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode9_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode9_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode9_w2_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode9_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode9_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode9_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode9_w2_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE10_w0_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode10_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode10_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode10_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode10_w0_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode10_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode10_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode10_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode10_w0_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode10_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode10_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode10_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode10_w0_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode10_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode10_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode10_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode10_w0_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE10_w1_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode10_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode10_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode10_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode10_w1_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode10_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode10_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode10_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode10_w1_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode10_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode10_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode10_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode10_w1_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode10_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode10_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode10_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode10_w1_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE10_w2_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode10_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode10_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode10_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode10_w2_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode10_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode10_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode10_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode10_w2_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode10_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode10_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode10_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode10_w2_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode10_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode10_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode10_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode10_w2_mirrorH_param);
            break;
        }
#endif
    }
}
///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE12_w0_mirrorH(tByte in_Mode)
{

    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode12_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode12_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode12_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode12_w0_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode12_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode12_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode12_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode12_w0_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode12_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode12_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode12_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode12_w0_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode12_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode12_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode12_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode12_w0_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE12_w1_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode12_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode12_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode12_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode12_w1_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode12_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode12_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode12_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode12_w1_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode12_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode12_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode12_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode12_w1_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode12_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode12_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode12_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode12_w1_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE14_w0_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode14_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode14_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode14_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode14_w0_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode14_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode14_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode14_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode14_w0_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode14_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode14_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode14_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode14_w0_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode14_w0_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode14_w0_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode14_w0_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode14_w0_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
static void VS4210_init_Input_MODE14_w1_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode14_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode14_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode14_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode14_w1_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode14_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode14_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode14_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode14_w1_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode14_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode14_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode14_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode14_w1_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode14_w1_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode14_w1_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode14_w1_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode14_w1_mirrorH_param);
            break;
        }
#endif
    }
}

static void VS4210_init_Input_MODE14_w2_mirrorH(tByte in_Mode)
{
    if (gbVXIS_OuputSize == _OUTPUT_720P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_720P_mode14_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_720P_mode14_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_720P_mode14_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_720P_mode14_w2_mirrorH_param);
            break;
        }
    }
    else if (gbVXIS_OuputSize == _OUTPUT_1080P60)
    {
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1080P_mode14_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1080P_mode14_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1080P_mode14_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1080P_mode14_w2_mirrorH_param);
            break;
        }
    }
    else
    {
#if (TH0701024600NYR50S1 == OFF)
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_800x480_mode14_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_800x480_mode14_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_800x480_mode14_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_800x480_mode14_w2_mirrorH_param);
            break;
        }
#else
        switch (in_Mode)
        {
        case _OSD_720X480I60:
            VS4210_init(VS4210_bt656_NTSC_1024x600_mode14_w2_mirrorH_param);
            break;
        case _OSD_720X576I50:
            VS4210_init(VS4210_bt656_PAL_1024x600_mode14_w2_mirrorH_param);
            break;
        case _OSD_1280X720P:
            VS4210_init(VS4210_bt656_720P_1024x600_mode14_w2_mirrorH_param);
            break;
        case _OSD_1920X1080P:
            VS4210_init(VS4210_bt656_1080P_1024x600_mode14_w2_mirrorH_param);
            break;
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_init_mode(tByte ch, tByte in_mode, tByte out_mode)
{
    switch (out_mode)
    {
    case VS4210_1080P_MODE0_w0:
    {
        if (gabVXIS_Input_mirrorH[0] == 0)
            VS4210_init_Input_MODE0_w0(in_mode);
        else
            VS4210_init_Input_MODE0_w0_mirrorH(in_mode);

        VS4210_SwitchChannel(0, 0);

        if (bSignal[0] == 0)
        {
            VS4210_clearAllsignal();
            VS4210_displayon();
        }
        else
        {
            VS4210_displayon();
            VS4210_OpenWindowsX(0);
            if (OSD_MIRROR[0])
                VS4210_SetCamMirror(0, 0);
        }
        break;
    }
    case VS4210_1080P_MODE0_w1:
    {
        if (gabVXIS_Input_mirrorH[3] == 0)
            VS4210_init_Input_MODE0_w0(in_mode);
        else
            VS4210_init_Input_MODE0_w0_mirrorH(in_mode);

        VS4210_SwitchChannel(1, 0);

        if (bSignal[1] == 0)
        {

            VS4210_clearAllsignal();
            VS4210_displayon();
        }
        else
        {
            VS4210_displayon();
            VS4210_OpenWindowsX(0);
            if (OSD_MIRROR[0])
                VS4210_SetCamMirror(1, 0);
        }

        break;
    }
    case VS4210_1080P_MODE0_w2:
    {
        if (gabVXIS_Input_mirrorH[1] == 0)
            VS4210_init_Input_MODE0_w0(in_mode);
        else
            VS4210_init_Input_MODE0_w0_mirrorH(in_mode);

        VS4210_SwitchChannel(2, 0);

        if (bSignal[2] == 0)
        {
            VS4210_clearAllsignal();
            VS4210_displayon();
        }
        else
        {
            VS4210_displayon();
            VS4210_OpenWindowsX(0);
            if (OSD_MIRROR[0])
                VS4210_SetCamMirror(2, 0);
        }

        break;
    }
    case VS4210_1080P_MODE0_w3:
    case VS4210_1080P_MODE0_w4:
    {
#if 1
        if (bytSelectCAMRD == SelectCameraD)
        {
            if (CAMD_M == 0)
                VS4210_init_Input_MODE0_w0(in_mode);
            else
                VS4210_init_Input_MODE0_w0_mirrorH(in_mode);
        }
        else /// CAMERA C
        {
            if (CAMC_M == 0)
                VS4210_init_Input_MODE0_w0(in_mode);
            else
                VS4210_init_Input_MODE0_w0_mirrorH(in_mode);
        }

#else
        if (gabVXIS_Input_mirrorH[2] == 0)
        {
            VS4210_init_Input_MODE0_w0(in_mode);
        }
        else
        {
            VS4210_init_Input_MODE0_w0_mirrorH(in_mode);
        }
#endif
        VS4210_SwitchChannel(3, 0);

        if (bSignal[3] == 0)
        {
            VS4210_clearAllsignal();
            VS4210_displayon();
        }
        else
        {
            VS4210_displayon();
            VS4210_OpenWindowsX(0);

#if 1
            if (bytSelectCAMRD == SelectCameraD)
            {
                if (CAMD_M)
                {
                    VS4210_SetCamMirror(3, 0);
                    // Printf("(*CAMD_M)");
                }
            }
            else /// CAMERA C
            {
                if (CAMC_M)
                {
                    VS4210_SetCamMirror(3, 0);
                    // Printf("(*CAMC_M)");
                }
            }

#else
            if (OSD_MIRROR[0])
                VS4210_SetCamMirror(3, 0);
#endif
        }

        break;
    }
    case VS4210_1080P_MODE1:
    {
        if (ch == 0)
        {
            if (gabVXIS_Input_mirrorH[0] == 0)
                VS4210_init_Input_MODE1_w0(in_mode);
            else
                VS4210_init_Input_MODE1_w0_mirrorH(in_mode);
        }
        else if (ch == 1)
        {
            if (gabVXIS_Input_mirrorH[1] == 0)
                VS4210_init_Input_MODE1_w1(in_mode);
            else
                VS4210_init_Input_MODE1_w1_mirrorH(in_mode);
        }
        else if (ch == 2)
        {
            // VS4210_init_Input_MODE1_w2(in_mode ) ;
            if (gabVXIS_Input_mirrorH[2] == 0)
                VS4210_init_Input_MODE1_w2(in_mode);
            else
                VS4210_init_Input_MODE1_w2_mirrorH(in_mode);
        }
        else
        {
            // VS4210_init_Input_MODE1_w3(in_mode ) ;
            if (gabVXIS_Input_mirrorH[3] == 0)
                VS4210_init_Input_MODE1_w3(in_mode);
            else
                VS4210_init_Input_MODE1_w3_mirrorH(in_mode);
        }

        //	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[ch],ch ) ;
        if (OSD_MIRROR[ch])
            VS4210_SetCamMirror(ch, ch);
        break;
    }

    case VS4210_1080P_MODE2:
    {
        if (ch == 0)
        {
            if (gabVXIS_Input_mirrorH[0] == 0)
                VS4210_init_Input_MODE2_w0(in_mode);
            else
                VS4210_init_Input_MODE2_w0_mirrorH(in_mode);
        }
        else if (ch == 1)
        {
            // VS4210_init_Input_MODE2_w1(in_mode ) ;
            if (gabVXIS_Input_mirrorH[1] == 0)
                VS4210_init_Input_MODE2_w1(in_mode);
            else
                VS4210_init_Input_MODE2_w1_mirrorH(in_mode);
        }
        else if (ch == 2)
        {
            /// VS4210_init_Input_MODE2_w2(in_mode ) ;
            if (gabVXIS_Input_mirrorH[2] == 0)
                VS4210_init_Input_MODE2_w2(in_mode);
            else
                VS4210_init_Input_MODE2_w2_mirrorH(in_mode);
        }
        else
        {
            // VS4210_init_Input_MODE2_w3(in_mode ) ;
            if (gabVXIS_Input_mirrorH[3] == 0)
                VS4210_init_Input_MODE2_w3(in_mode);
            else
                VS4210_init_Input_MODE2_w3_mirrorH(in_mode);
        }
        // VS4210_SwitchChannel(gaHDMI_MapWindowIndex[ch],ch ) ;
        break;
    }

    case VS4210_1080P_MODE8:
    {
        if (ch == 0)
        {
            if (gabVXIS_Input_mirrorH[0] == 0)
                VS4210_init_Input_MODE8_w0(in_mode);
            else
                VS4210_init_Input_MODE8_w0_mirrorH(in_mode);
        }
        else if (ch == 1)
        {
            // VS4210_init_Input_MODE8_w1(in_mode ) ;
            if (gabVXIS_Input_mirrorH[1] == 0)
                VS4210_init_Input_MODE8_w1(in_mode);
            else
                VS4210_init_Input_MODE8_w1_mirrorH(in_mode);
        }
        else
        {
            // VS4210_init_Input_MODE8_w2(in_mode ) ;
            if (gabVXIS_Input_mirrorH[2] == 0)
                VS4210_init_Input_MODE8_w2(in_mode);
            else
                VS4210_init_Input_MODE8_w2_mirrorH(in_mode);
        }
#if (TH0701024600NYR50S1 == OFF)
        VXISI2CWrite(REG_SC1_A1, 0x26);
#endif
        // VS4210_SwitchChannel(gaHDMI_MapWindowIndex[ch],ch ) ;
        if (OSD_MIRROR[ch])
            VS4210_SetCamMirror(ch, ch);
        break;
    }

    case VS4210_1080P_MODE10:
    {
        if (ch == 0)
        {
            if (gabVXIS_Input_mirrorH[0] == 0)
                VS4210_init_Input_MODE10_w0(in_mode);
            else
                VS4210_init_Input_MODE10_w0_mirrorH(in_mode);
            //#if (TH0701024600NYR50S1==OFF)
            // VXISI2CWrite( REG_PRES3_2D,0x33);
            //#endif
        }
        else if (ch == 1)
        {
            // VS4210_init_Input_MODE10_w1(in_mode ) ;
            if (gabVXIS_Input_mirrorH[1] == 0)
                VS4210_init_Input_MODE10_w1(in_mode);
            else
                VS4210_init_Input_MODE10_w1_mirrorH(in_mode);
        }
        else
        {
            // VS4210_init_Input_MODE10_w2(in_mode ) ;
            if (gabVXIS_Input_mirrorH[2] == 0)
                VS4210_init_Input_MODE10_w2(in_mode);
            else
                VS4210_init_Input_MODE10_w2_mirrorH(in_mode);
        }

        if (OSD_MIRROR[ch])
            VS4210_SetCamMirror(ch, ch);

#if (TH0701024600NYR50S1 == OFF)
        VXISI2CWrite(REG_SC1_A1, 0x24);
#endif
        //	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[ch],ch ) ;
        break;
    }
    case VS4210_1080P_MODE9:
    {
        if (ch == 0)
        {
            if (gabVXIS_Input_mirrorH[0] == 0)
                VS4210_init_Input_MODE9_w0(in_mode);
            else
                VS4210_init_Input_MODE9_w0_mirrorH(in_mode);
        }
        else if (ch == 1)
        {
            // VS4210_init_Input_MODE9_w1(in_mode ) ;
            if (gabVXIS_Input_mirrorH[1] == 0)
                VS4210_init_Input_MODE9_w1(in_mode);
            else
                VS4210_init_Input_MODE9_w1_mirrorH(in_mode);
        }
        else
        {
            // VS4210_init_Input_MODE9_w2(in_mode ) ;
            if (gabVXIS_Input_mirrorH[2] == 0)
                VS4210_init_Input_MODE9_w2(in_mode);
            else
                VS4210_init_Input_MODE9_w2_mirrorH(in_mode);
        }

        //	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[ch],ch ) ;
        if (OSD_MIRROR[ch])
            VS4210_SetCamMirror(ch, ch);
        break;
    }
    case VS4210_1080P_MODE12:
    {
        if (ch == 0)
        {
            if (gabVXIS_Input_mirrorH[0] == 0)
                VS4210_init_Input_MODE12_w0(in_mode);
            else
                VS4210_init_Input_MODE12_w0_mirrorH(in_mode);
        }
        else if (ch == 1)
        {
            // VS4210_init_Input_MODE12_w1(in_mode ) ;
            if (gabVXIS_Input_mirrorH[1] == 0)
                VS4210_init_Input_MODE12_w1(in_mode);
            else
                VS4210_init_Input_MODE12_w1_mirrorH(in_mode);
        }

        //	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[ch],ch ) ;
        if (OSD_MIRROR[ch])
            VS4210_SetCamMirror(ch, ch);
        break;
    }
    case VS4210_1080P_MODE14:
    {
        if (ch == 0)
        {
            if (gabVXIS_Input_mirrorH[0] == 0)
                VS4210_init_Input_MODE14_w0(in_mode);
            else
                VS4210_init_Input_MODE14_w0_mirrorH(in_mode);
        }
        else if (ch == 1)
        {
            // VS4210_init_Input_MODE14_w1(in_mode ) ;
            if (gabVXIS_Input_mirrorH[1] == 0)
                VS4210_init_Input_MODE14_w1(in_mode);
            else
                VS4210_init_Input_MODE14_w1_mirrorH(in_mode);
        }
        else if (ch == 2)
        {
            // VS4210_init_Input_MODE14_w2(in_mode ) ;
            if (gabVXIS_Input_mirrorH[2] == 0)
                VS4210_init_Input_MODE14_w2(in_mode);
            else
                VS4210_init_Input_MODE14_w2_mirrorH(in_mode);
        }

        //	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[ch],ch ) ;
        if (OSD_MIRROR[ch])
            VS4210_SetCamMirror(ch, ch);
        break;
    }
    }
}

///////////////////////////////////////////////////////////////////////////////////
tByte VS4210_SetInModeOutMode(tByte ch, tByte inmode, tByte outmode)
{
    tByte osdflash = 0;
    tByte i;

    gbVXIS_OuputModeflg = outmode;
    g4210VinIndex = ch;
    gbVXIS_InputMode[g4210VinIndex] = inmode;

#if (_DEBUG_MESSAGE_BOOT_NOISE == ON)
    GraphicsPrint(CYAN, "(**VS4210_SetInModeOutMode)\r\n");
#endif

    if (inmode != _OSD_No_Signal)
    {
        bSignal[ch] = 1;

        bytVideoInput |= (1 << ch);
    }
    else
    {
        bSignal[ch] = 0;
        bytVideoInput &= (1 << ch);
    }

    if (gbVXIS_OuputModeflg == VS4210_1080P_MODE0_w0)
    {
        if (g4210VinIndex == 0)
        {
            VS4210_init_mode(g4210VinIndex, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg);
            osdflash = 1;
        }
        else
        {
            osdflash = 0;
        }
    }
    else if (gbVXIS_OuputModeflg == VS4210_1080P_MODE0_w1)
    {
        if (g4210VinIndex == 1)
        {
            VS4210_init_mode(g4210VinIndex, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg);
            osdflash = 1;
        }
        else
        {
            osdflash = 0;
        }
    }
    else if (gbVXIS_OuputModeflg == VS4210_1080P_MODE0_w2)
    {
        if (g4210VinIndex == 2)
        {
            VS4210_init_mode(g4210VinIndex, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg);
            osdflash = 1;
        }
        else
        {
            osdflash = 0;
        }
    }
    else if ((gbVXIS_OuputModeflg == VS4210_1080P_MODE0_w3) || (gbVXIS_OuputModeflg == VS4210_1080P_MODE0_w4))
    {
        if (g4210VinIndex == 3)
        {
            VS4210_init_mode(g4210VinIndex, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg);
            osdflash = 1;

#if (_DEBUG_MESSAGE_Source_Change == ON)
            GraphicsPrint(CYAN, "(**gbVXIS_OuputModeflg=%d)\r\n", (WORD)g4210VinIndex);
#endif
        }
        else
        {
            osdflash = 0;
        }
    }
    else // if (gbVXIS_OuputModeflg == VS4210_1080P_MODE1 )
    {

        for (i = 0; i < gWindowNum; i++)
        {

            if ((gaHDMI_MapWindowIndex[i] == g4210VinIndex))
            {
                // VXISI2CWrite( 2,i);
                // VXISI2CWrite( 0xE9, 50 );

                VS4210_init_mode(i, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg);

                if ((gaOpenWindowflg[i] == 1))
                {

#if 1 /// ryan@20210802  1//ryan@20020616 wax�ץ�channel�|���ð��D�C
                    if (i != g4210VinIndex)
                    {
                        VS4210_SwitchChannel(g4210VinIndex, i); // ch ->win
                    }
#endif

                    VS4210_OpenWindowsX(i);

                    VXIS_fShowInOutMode(i, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg);
                }
                osdflash = 1;
            }

            //	VS4210_SwitchChannel(gaHDMI_MapWindowIndex[i],i);//ryan@20200613
        }

        //  printf("\r\n @@%s,%d,i=%d,mapi=%d", __func__,__LINE__ ,g4210VinIndex ,gaHDMI_MapWindowIndex[g4210VinIndex] ) ;
        // 20210820@start
        // 	   if ( bSignal[gaHDMI_MapWindowIndex[0] ] == 0  )
        // 	   {
        // 		   VS4210_ch0_NoSignal_Rst(0 ) ;
        // 	   }
        // 20210820@end
    }

    if ((osdflash == 1) && (gbVXIS_OuputModeflg <= VS4210_1080P_MODE0_w4))
    {
        VXIS_fShowInOutMode0OSD(gbVXIS_OuputModeflg, gbVXIS_InputMode[gbVXIS_OuputModeflg], gbVXIS_OuputModeflg);
    }

#if 0
	for ( i = 0 ; i < gWindowNum ; i++)
	{
		if ((gaHDMI_MapWindowIndex[i] == g4210VinIndex) )
		{
			  
			if ((gaOpenWindowflg[i] == 1))
			{
			  VXIS_fShowInOutMode( i, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg ) ;

			//msleep(250);
			
			 VXISI2CWrite( 2, i );
  			VXISI2CWrite( 0xE9, 0 );	

			VideoChangeFlag=0;
			}
		}
	}



if((osdflash ==1 )&&(gbVXIS_OuputModeflg <= VS4210_1080P_MODE0_w4 ))
{
	VXIS_fShowInOutMode0OSD( gbVXIS_OuputModeflg,  gbVXIS_InputMode[gbVXIS_OuputModeflg], gbVXIS_OuputModeflg )  ;
}
#endif

    MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_BOOT_SETUP);
    MCUTimerActiveTimerEvent(1, _SYSTEM_TIMER_EVENT_BOOT_SETUP);

    return osdflash;
}

///////////////////////////////////////////////////////////////////////////////////
static tByte Get_WindowNum(tByte OuputMode)
{
    tByte Num;
    if (OuputMode < VS4210_1080P_MODE1)
    {
        Num = 1;
    }
    else if (OuputMode == VS4210_1080P_MODE12)
    {
        Num = 2;
    }
    else if ((OuputMode == VS4210_1080P_MODE8) || (OuputMode == VS4210_1080P_MODE9) || (OuputMode == VS4210_1080P_MODE14) || (OuputMode == VS4210_1080P_MODE10))
    {
        Num = 3;
    }
    else
    {
        Num = 4;
    }
    return Num;
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_StartInit(void)
{
    tByte i;
    tByte tmpval;

    gWindowNum = Get_WindowNum(gbVXIS_OuputModeflg);

    VS4210_displayoff();
    for (i = 0; i < HDMI_IN_NUMBER; i++)
    {
        VS4210_CloseWindowsX(i);
    }

    VS4210_init_Input_MODE88(gbVXIS_OuputModeflg);

    for (i = 0; i < HDMI_IN_NUMBER; i++)
    {
        VS4210_init_mode(i, gbVXIS_InputMode[i], gbVXIS_OuputModeflg);
    }

    for (i = 0; i < HDMI_IN_NUMBER; i++)
    {
        if (bSignal[i] == 0)
        {
            VS4210_CloseWindowsX(gaHDMI_MapWindowIndex[i]);
        }
        else
        {
            VS4210_OpenWindowsX(gaHDMI_MapWindowIndex[i]);
        }

        if (bSignal[i] == 0)
        {
            gbVXIS_InputMode[i] = _OSD_No_Signal;
        }
    }
    VS4210_ch0_NoSignal_Rst(0);

    gbVXIS_InputMode[0] = _OSD_No_Signal;
    gbVXIS_InputMode[1] = _OSD_No_Signal;
    gbVXIS_InputMode[2] = _OSD_No_Signal;
    gbVXIS_InputMode[3] = _OSD_No_Signal;

    tmpval = VXISI2CRead(0x81);
    VXISI2CWrite(0x81, tmpval & 0xBF);
    PreVXIS_OuputSize = gbVXIS_OuputSize;

    gbVXIS_InputMode[0] = 100;
    gbVXIS_InputMode[1] = 100;
    gbVXIS_InputMode[2] = 100;
    gbVXIS_InputMode[3] = 100;
    OSD_Clear_All();
    VS4210_Line_Clear();
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_No_Signal_Init(void)
{
    gbVXIS_InputMode[0] = _OSD_1920X1080P;
    gbVXIS_InputMode[1] = _OSD_1920X1080P;
    gbVXIS_InputMode[2] = _OSD_1920X1080P;
    gbVXIS_InputMode[3] = _OSD_1920X1080P;
    VS4210_StartInit();
    gbVXIS_InputMode[0] = _OSD_No_Signal;
    gbVXIS_InputMode[1] = _OSD_No_Signal;
    gbVXIS_InputMode[2] = _OSD_No_Signal;
    gbVXIS_InputMode[3] = _OSD_No_Signal;
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_displayoff(void)
{
    tByte tmpval;
    tmpval = VXISI2CRead(0x32);
    if (tmpval != 0)
    {
        gvideofftmp = tmpval;
    }
    VXISI2CWrite(0x32, 0);
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_displayon(void)
{
    msleep(80);
    VXISI2CWrite(0x32, gvideofftmp);
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_Rx_NoSignal(tByte ch)
{
    tByte j;
    g4210VinIndex = ch;
    bSignal[ch] = 0;

    if (gbVXIS_InputMode[g4210VinIndex] != _OSD_No_Signal)
    {
        // gbPreVXIS_InputMode[g4210VinIndex] = 251 ;
        gbVXIS_InputMode[g4210VinIndex] = _OSD_No_Signal;

        if ((gbVXIS_OuputModeflg >= VS4210_1080P_MODE1) && (gbVXIS_OuputModeflg < VS4210_1080P_MODE_NUMBER))
        {
            for (j = 0; j < gWindowNum; j++)
            {
                if (gaHDMI_MapWindowIndex[j] == g4210VinIndex)
                {
                    if (gaOpenWindowflg[j] == 1)
                    {
                        VXIS_fShowInOutMode(j, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg);
                        VS4210_CloseWindowsX(j); // chunyu
                    }
                    else
                    {
                        OSD_Clear(j);
                    }
                }
            }
        }
        else if (gbVXIS_OuputModeflg <= VS4210_1080P_MODE0_w4)
        {
            if (g4210VinIndex == gbVXIS_OuputModeflg)
            {
                VS4210_CloseAllWindows();
                VXIS_fShowInOutMode0OSD(g4210VinIndex, gbVXIS_InputMode[g4210VinIndex], gbVXIS_OuputModeflg);
            }
        }

        if (bSignal[gaHDMI_MapWindowIndex[0]] == 0)
        {
            VS4210_ch0_NoSignal_Rst(0);
        }
    }

    // Printf("(VS4210_Rx_NoSignal ch=%02x)",(WORD)ch);
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_PowerDown(void)
{
    VXISI2CWrite(0x03, 0x74);
}

///////////////////////////////////////////////////////////////////////////////////
void VS4210_PowerResume(void)
{
    VXISI2CWrite(0x03, 0x75);
}

///////////////////////////////////////////////////////////////////////////////////
bit VS4210_SetMutiWindows(T_VS4210_MutiWindows *pVS4210_JointKind1)
{
    tByte i;
    tByte j;
    tByte tmpindex;
    bit re = 1;
    tByte OutSizeChange;
    tByte OutModeChange;
    tByte WMap0;
    tByte WMap1;
    tByte WMap2;
    tByte WMap3;

#if (_DEBUG_MESSAGE_Source_Change == ON)
    GraphicsPrint(YELLOW, "(VS4210_SetMutiWindows)\r\n");
#endif

    u8gDrawOSDFlg = pVS4210_JointKind1->DrawOSDFlg;
    u8DrawBoxFlg = pVS4210_JointKind1->DrawBoxFlg;
    u8DrawBoxColor = pVS4210_JointKind1->DrawBoxColor;
    gu8AudioMode = pVS4210_JointKind1->AudioChannel;

    WMap0 = pVS4210_JointKind1->WindowMap.WMap0;
    WMap1 = pVS4210_JointKind1->WindowMap.WMap1;
    WMap2 = pVS4210_JointKind1->WindowMap.WMap2;
    WMap3 = pVS4210_JointKind1->WindowMap.WMap3;

    if (pVS4210_JointKind1->OutputMode == VS4210_1080P_MODE_SystemPowerDown)
    {
        VS4210_PowerDown();
        re = 0;
    }
    else if (pVS4210_JointKind1->OutputMode == VS4210_1080P_MODE_SystemPowerResume)
    {
        VS4210_PowerResume();
        re = 0;
    }

    if (re == 1)
    {
        OutSizeChange = 0;
        OutModeChange = 0;
        aOpenWindowflgChange[0] = 0;
        aOpenWindowflgChange[1] = 0;
        aOpenWindowflgChange[2] = 0;
        aOpenWindowflgChange[3] = 0;

        if (pVS4210_JointKind1->OutputMode <= VS4210_1080P_MODE0_w4)
        {
            if (WMap0 > 0)
            {
                gbVXIS_OuputModeflg = WMap0 - 1;
            }
            else
            {
                gbVXIS_OuputModeflg = 0;
            }
        }
        else
        {
            if (pVS4210_JointKind1->OutputMode < VS4210_1080P_MODE_SystemPowerDown)
            {
                gbVXIS_OuputModeflg = pVS4210_JointKind1->OutputMode;
            }
        }

        if (WMap0 == 0)
        {
            gaHDMI_MapWindowIndex[0] = 0;
            gaOpenWindowflg[0] = 0;
            PreaMapWindowIndex[0] = 6;
        }
        else
        {
            gaHDMI_MapWindowIndex[0] = WMap0 - 1;
            gaOpenWindowflg[0] = 1;
        }

        if (WMap1 == 0)
        {
            gaHDMI_MapWindowIndex[1] = 1;
            gaOpenWindowflg[1] = 0;
            PreaMapWindowIndex[1] = 6;
        }
        else
        {
            gaHDMI_MapWindowIndex[1] = WMap1 - 1;
            gaOpenWindowflg[1] = 1;
        }

        if (WMap2 == 0)
        {
            gaHDMI_MapWindowIndex[2] = 2;
            gaOpenWindowflg[2] = 0;
            PreaMapWindowIndex[2] = 6;
        }
        else
        {
            gaHDMI_MapWindowIndex[2] = WMap2 - 1;
            gaOpenWindowflg[2] = 1;
        }

        if (WMap3 == 0)
        {
            gaHDMI_MapWindowIndex[3] = 3;
            gaOpenWindowflg[3] = 0;
            PreaMapWindowIndex[3] = 6;
        }
        else
        {
            gaHDMI_MapWindowIndex[3] = WMap3 - 1;
            gaOpenWindowflg[3] = 1;
        }

        for (i = 0; i < 4; i++)
        {
            if (PreaMapWindowIndex[i] != gaHDMI_MapWindowIndex[i])
            {
                aOpenWindowflgChange[i] = 1;
            }
        }

        gbVXIS_OuputSize = pVS4210_JointKind1->OutputSize;
        if (PreOuputModeflg != gbVXIS_OuputModeflg)
        {
            gWindowNum = Get_WindowNum(gbVXIS_OuputModeflg);
        }

        tmpindex = g4210VinIndex;
        g4210VinIndex = 0;

        if (PreVXIS_OuputSize != gbVXIS_OuputSize)
        {
            OutSizeChange = 1;
            VS4210_displayoff();
            // PreVXIS_FULLOuputModeflg = VS4210_1080P_MODE_NUMBER   ;
            PreVXIS_FULLOuputModeflg = 0;

            if ((bSignal[0] == 0) && (bSignal[1] == 0) && (bSignal[2] == 0) && (bSignal[3] == 0))
            {
                if (ResetCamMirror == TRUE)
                {
                    for (i = 0; i < HDMI_IN_NUMBER; i++)
                        VXIS_fShowInOutMode(i, gbVXIS_InputMode[gaHDMI_MapWindowIndex[i]], gbVXIS_OuputModeflg);

                    ResetCamMirror = FALSE;
                }
                else
                {
                    VS4210_No_Signal_Init();
                }

                OutSizeChange = 0;
                VS4210_displayon();
            }
            else
            {
                VS4210_init_Input_MODE88(gbVXIS_OuputModeflg);
            }
            PreOuputModeflg = gbVXIS_OuputModeflg;
        }

        if ((PreOuputModeflg != gbVXIS_OuputModeflg) /*||(gbVXIS_OuputModeflg==(SV3-1))*/)
        {
            if ((PreOuputModeflg <= VS4210_1080P_MODE0_w4) && (gbVXIS_OuputModeflg > VS4210_1080P_MODE0_w4))
            {
                VS4210_init_Input_MODE88(gbVXIS_OuputModeflg);
            }
            OutModeChange = 1;
        }

        if ((OutModeChange == 1) || (OutSizeChange == 1))
        {
            VS4210_Line_Clear();
        }

        if (gbVXIS_OuputModeflg <= VS4210_1080P_MODE0_w4)
        {
            if (OutModeChange == 1)
            {
                VS4210_CloseWindowsX(1);
                VS4210_CloseWindowsX(2);
                VS4210_CloseWindowsX(3);
                OSD_Clear_234();
            }

            if (gaOpenWindowflg[0] == 1)
            {
                if ((aOpenWindowflgChange[0] == 1) || (OutModeChange == 1) || (OutSizeChange == 1))
                {
                    VS4210_init_mode(gbVXIS_OuputModeflg, gbVXIS_InputMode[gbVXIS_OuputModeflg], gbVXIS_OuputModeflg);
                    VXIS_fShowInOutMode0OSD(gbVXIS_OuputModeflg, gbVXIS_InputMode[gbVXIS_OuputModeflg], gbVXIS_OuputModeflg);
                    msleep(100);
#if (_DEBUG_MESSAGE_Source_Change == ON)
                    GraphicsPrint(RED, "(#1)\r\n");
#endif
                }
                else if ((SelectModeType == VS4210_1080P_MODE0_w3) || (SelectModeType == VS4210_1080P_MODE0_w4))
                {
#if (_DEBUG_MESSAGE_Source_Change == ON)
                    GraphicsPrint(RED, "(#2)\r\n");
#endif
                    VXIS_fShowInOutMode0OSD(gbVXIS_OuputModeflg, gbVXIS_InputMode[gbVXIS_OuputModeflg], gbVXIS_OuputModeflg);
                }
            }
            else
            {
                VS4210_CloseWindowsX(0);
                OSD_Clear(0);
            }
        }
        else
        {
            if ((OutModeChange == 1) || (OutSizeChange == 1))
            {
                VS4210_displayoff();
            }

            for (j = 0; j < 4; j++)
            {
                if ((OutSizeChange == 1) || (OutModeChange == 1))
                {
                    VS4210_CloseWindowsX(j);
                }
                else
                {
                    if (PreaMapWindowIndex[j] != gaHDMI_MapWindowIndex[j])
                    {
                        VS4210_CloseWindowsX(j);
                    }
                    else
                    {
                        if (gaOpenWindowflg[j] == 0)
                        {
                            VS4210_CloseWindowsX(j);
                        }
                    }
                }
            }

            for (i = 0; i < HDMI_IN_NUMBER; i++)
            {
                g4210VinIndex = i;
                for (j = 0; j < gWindowNum; j++)
                {
                    if (gaHDMI_MapWindowIndex[j] == i)
                    {
                        if ((OutSizeChange == 1) || (OutModeChange == 1))
                        {
                            VS4210_init_mode(j, gbVXIS_InputMode[i], gbVXIS_OuputModeflg);
                            if (j != i)
                            {
                                VS4210_SwitchChannel(i, j);
                            }
                        }

                        else
                        {
                            if (PreaMapWindowIndex[j] != gaHDMI_MapWindowIndex[j])
                            {

                                VS4210_init_mode(j, gbVXIS_InputMode[i], gbVXIS_OuputModeflg);
                                // Printf("\r\ngbVXIS_InputMode[%02x]=%02x, gbVXIS_OuputModeflg=%02x ",(WORD)i,(WORD) gbVXIS_InputMode[i],(WORD) gbVXIS_OuputModeflg);
                                VXIS_fShowInOutMode(j, gbVXIS_InputMode[i], gbVXIS_OuputModeflg);
                                if (j != i)
                                {
                                    VS4210_SwitchChannel(i, j);
                                    // Printf("\r\n*VS4210_SwitchChannel ch=%02x,win=%02x ",(WORD)i,(WORD)j);
                                }
                            }
                        }
                    }
                }
            }

            if ((OutModeChange == 1) || (OutSizeChange == 1))
            {
                if (gWindowNum == 2)
                {
                    OSD_Clear_34();
                }
                else if (gWindowNum == 3)
                {
                    OSD_Clear_4();
                }
                else if (gWindowNum == 1)
                {
                    OSD_Clear_234();
                }
            }

            for (i = 0; i < HDMI_IN_NUMBER; i++)
            {
                g4210VinIndex = i;
                for (j = 0; j < gWindowNum; j++)
                {
                    if (gaHDMI_MapWindowIndex[j] == i)
                    {
                        if ((OutSizeChange == 1) || (OutModeChange == 1))
                        {
                            VXIS_fShowInOutMode(j, gbVXIS_InputMode[i], gbVXIS_OuputModeflg);
                            gu8PreAudioMode = gu8AudioMode;
                            u8PreDrawOSDFlg = u8gDrawOSDFlg;
                        }
                        else
                        {
                            if (PreaMapWindowIndex[j] != gaHDMI_MapWindowIndex[j])
                            {
                                if (gbVXIS_InputMode[i] != _OSD_No_Signal)
                                {
                                    VXIS_fShowInOutMode(j, gbVXIS_InputMode[i], gbVXIS_OuputModeflg);
                                }
                                gu8PreAudioMode = gu8AudioMode;
                            }
                        }
                    }

                    // VXISI2CWrite( 2, j );
                    // VXISI2CWrite( 0xE9, 0 );
                }
            }

            for (j = 0; j < gWindowNum; j++)
            {
                if (gaOpenWindowflg[j] == 0)
                {
                    OSD_Clear(j);
                }
            }
            if (pVS4210_JointKind1->OutputMode > VS4210_1080P_MODE0_w4)
            {
                for (i = 0; i < HDMI_IN_NUMBER; i++)
                {
                    for (j = 0; j < gWindowNum; j++)
                    {
                        if (gaHDMI_MapWindowIndex[j] == i)
                        {
                            if (gaOpenWindowflg[j] == 1)
                            {
                                if (bSignal[i] == 1)
                                {
                                    VS4210_OpenWindowsX(j);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (gaOpenWindowflg[0] == 1)
                {
                    VS4210_OpenWindowsX(0);
                }
            }

            if (bSignal[gaHDMI_MapWindowIndex[0]] == 0)
            {
                VS4210_ch0_NoSignal_Rst(0);
            }

            if (gaOpenWindowflg[0] == 0)
            {
                VS4210_ch0_NoSignal_Rst(0);
            }

            if ((OutModeChange == 1) || (OutSizeChange == 1))
            {
                VS4210_displayon();
            }
        }

        g4210VinIndex = tmpindex;
        PreVXIS_OuputSize = gbVXIS_OuputSize;
        PreOuputModeflg = gbVXIS_OuputModeflg;

        if (u8DrawBoxFlg == 1)
        {
            if ((OutModeChange == 1) || (OutSizeChange == 1))
            {
                u8PreDrawBoxColor = u8DrawBoxColor;
                VS4210_Line_Draw(gbVXIS_OuputModeflg);
            }
            else
            {
                if (u8DrawBoxColor != u8PreDrawBoxColor)
                {
                    u8PreDrawBoxColor = u8DrawBoxColor;
                    VS4210_Line_Draw(gbVXIS_OuputModeflg);
                }
            }
        }
        else
        {
            u8PreDrawBoxColor = 0xFF;
            VS4210_Line_Clear();
        }

        if (gbVXIS_OuputModeflg <= VS4210_1080P_MODE0_w4)
        {
            if ((gu8PreAudioMode != gu8AudioMode) || (u8PreDrawOSDFlg != u8gDrawOSDFlg))
            {
                gu8PreAudioMode = gu8AudioMode;
                u8PreDrawOSDFlg = u8gDrawOSDFlg;
                VXIS_fShowInOutMode0OSD(gbVXIS_OuputModeflg, gbVXIS_InputMode[gbVXIS_OuputModeflg], gbVXIS_OuputModeflg);
            }
        }
        else
        {
            if ((gu8PreAudioMode != gu8AudioMode) || (u8PreDrawOSDFlg != u8gDrawOSDFlg))
            {
                gu8PreAudioMode = gu8AudioMode;
                u8PreDrawOSDFlg = u8gDrawOSDFlg;

                for (i = 0; i < HDMI_IN_NUMBER; i++)
                {
                    // mask by ryan@20210805       VXIS_fShowInOutMode(i,gbVXIS_InputMode[gaHDMI_MapWindowIndex[i]], gbVXIS_OuputModeflg ) ;
                }
            }
        }

        for (j = 0; j < 4; j++)
        {
            if (gaOpenWindowflg[j] != 0)
            {
                PreaMapWindowIndex[j] = gaHDMI_MapWindowIndex[j];
            }
            else
            {
                PreaMapWindowIndex[j] = 5;
            }
        }
    }

    if ((NowModeState == JumpMode) && (TriggerFlag == 0))
        ShowJUMPOSD();

    return re;
}
