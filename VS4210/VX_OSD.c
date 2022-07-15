/** @file VX_OSD.c
 * @brief OSD font utility
 *
 * Input source or resolution displayed on the screen.
 *
 * @author Cunu-Yu
 * @date 2018/06/19
 */

#include "VXIS_Extern_Define.h"
#include "VX_OSD.h"
#include "VX_swi2c.h"

tByte DisplayedOSD = 0;

void ClearDisplayedOSD(BYTE newd)
{
    DisplayedOSD &= (~(newd));
}

void SetDisplayedOSD(BYTE newd)
{
    DisplayedOSD |= newd;
}

BYTE GetDisplayedOSD(void)
{
    return DisplayedOSD;
}

void SetOsdStartAddress(tByte block, tByte addr)
{
    switch (block)
    {
    case (_Title0):
    case (_Title1):
    case (_Title2):
    case (_Title3):
        VXISI2CWrite(0x52, addr);
        break;

    case (_Content):
        VXISI2CWrite(0x57, addr);
        break;

    case (_Bar0):
    case (_Bar1):
    case (_Bar2):
    case (_Bar3):
        VXISI2CWrite(0x5D, addr);
        break;

    default:
        break;
    }
}

//---------------------------------------------------------------------------
void SetOsdSize(tByte block, tByte sizeX, tByte sizeY)
{
    tByte temp;
    tByte tempBar;
    tByte sizeX3f;

    temp = VXISI2CRead(0x53) & 0x3f;
    tempBar = VXISI2CRead(0x5E) & 0x3f;
    sizeX3f = sizeX & 0x3f;

    switch (block)
    {
    case _Title0:
        // VXISI2CWrite(0x53,temp&0x3f);
        // VXISI2CWrite(0x53,sizeX&0x3f);
        VXISI2CWrite(0x53, temp);
        VXISI2CWrite(0x53, sizeX3f);
        break;
    case _Title1:
        VXISI2CWrite(0x53, temp | 0x40);
        VXISI2CWrite(0x53, sizeX3f | 0x40);
        break;
    case _Title2:
        VXISI2CWrite(0x53, temp | 0x80);
        VXISI2CWrite(0x53, sizeX3f | 0x80);
        break;
    case _Title3:
        VXISI2CWrite(0x53, temp | 0xC0);
        VXISI2CWrite(0x53, sizeX3f | 0xC0);
        break;

    case _Bar0:
        VXISI2CWrite(0x5E, tempBar);
        VXISI2CWrite(0x5E, sizeX3f);
        break;
    case _Bar1:
        VXISI2CWrite(0x5E, tempBar | 0x40);
        VXISI2CWrite(0x5E, sizeX3f | 0x40);
        break;
    case _Bar2:
        VXISI2CWrite(0x5E, tempBar | 0x80);
        VXISI2CWrite(0x5E, sizeX3f | 0x80);
        break;
    case _Bar3:
        VXISI2CWrite(0x5E, tempBar | 0xC0);
        VXISI2CWrite(0x5E, sizeX3f | 0xC0);
        break;

    case _Content:
        VXISI2CWrite(0x58, sizeX3f);
        VXISI2CWrite(0x59, sizeY & 0x1f);
        break;
    }
}

//---------------------------------------------------------------------------
void SetOsdPosition(tByte block, tWord X_Pos, tWord Y_Pos)
{
    tByte XHighByte;
    tByte XLowByte;
    tByte YHighByte;
    tByte YLowByte;

    XLowByte = (tByte)(X_Pos & 0x00FF);
    XHighByte = ((tByte)((X_Pos >> 8) & 0x07)) << 4;

    YLowByte = (tByte)(Y_Pos & 0x00FF);
    YHighByte = (tByte)((Y_Pos >> 8) & 0x07);

    switch (block)
    {
    case (_Title0):
    case (_Title1):
    case (_Title2):
    case (_Title3):
        VXISI2CWrite(0x54, XHighByte | YHighByte);
        VXISI2CWrite(0x55, XLowByte);
        VXISI2CWrite(0x56, YLowByte);
        break;
    case (_Content):
        VXISI2CWrite(0x5A, XHighByte | YHighByte);
        VXISI2CWrite(0x5B, XLowByte);
        VXISI2CWrite(0x5C, YLowByte);
        break;
    case (_Bar0):
    case (_Bar1):
    case (_Bar2):
    case (_Bar3):
        VXISI2CWrite(0x5F, XHighByte | YHighByte);
        VXISI2CWrite(0x60, XLowByte);
        VXISI2CWrite(0x61, YLowByte);
        break;
    }
}

//---------------------------------------------------------------------------
void OsdDumpString(tByte OSDaddr, tcByte /* tByte*/ *Strdata)
{
    tByte Lan;
    Lan = Strdata[0];
    VXISI2CWrite(0xfd, 0x10);
    VXISI2CWrite(0xfe, OSDaddr);
    Strdata++;
    I2CMultiWrite(VS4210_ADDR, 0xff, Lan, Strdata);
}

//---------------------------------------------------------------------------
void OsdDumpAttribute(tByte OSDaddr, tcByte /*tByte*/ *Strdata, tByte index)
{
    tByte Lan;
    Lan = Strdata[0];
    VXISI2CWrite(0xfd, 0x70);
    VXISI2CWrite(0xfe, OSDaddr);
    I2CMultiAttribute(VS4210_ADDR, 0xff, Lan, &index);
}

void OsdDumpAttributeHS(tByte OSDaddr, tcByte *Strdata, tByte index, tByte Len)
{
    tByte Lan;
    Lan = Len;
    VXISI2CWrite(0xfd, 0x70);
    VXISI2CWrite(0xfe, OSDaddr);
    I2CMultiAttribute(VS4210_ADDR, 0xff, Lan, &index);
}

//---------------------------------------------------------------------------
void VXIS_fSetOsdOnOff(tByte ch, tByte block)
{
    tByte temp;
    temp = VXISI2CRead(0x53);
    temp = (temp & 0x3F) | (ch << 6);
    VXISI2CWrite(0x53, temp);

    temp = VXISI2CRead(0x5E);
    temp = (temp & 0x3F) | (ch << 6);
    VXISI2CWrite(0x5E, temp);

    temp = VXISI2CRead(0x51);
    if ((block & WINZOOMx2) == 0)
        temp &= 0xf7;
    // temp=(temp&0xf8)|(block&0x07);//mask by ryan@20200523

    temp = (temp & 0xf8) | (block & 0x0F);
    VXISI2CWrite(0x51, temp);
}

void VXIS_fSetContentOsdOnOff(tByte block)
{
    tByte temp;
    temp = VXISI2CRead(0x51);
    temp = temp & 0xfD;
    if (block > 0)
    {
        temp = temp | _ContentOn;
    }
    VXISI2CWrite(0x51, temp | WINZOOMx2);
}

//---------------------------------------------------------------------------
/*
void VXIS_fSetOsdx2(bit OnOff)
{
    tByte temp=0;
    temp=VXISI2CRead(0x51);
    if (OnOff)
        temp=(temp&0xf7)|(0x08);
    else
        temp=temp&0xf7;
    VXISI2CWrite(0x51,temp);
}
*/
//---------------------------------------------------------------------------
void setTransParentIndex(tByte index)
{
    tByte temp;
    temp = VXISI2CRead(0x69);
    temp = (temp & 0x0f) | (index << 4);
    VXISI2CWrite(0x69, temp);
    VXISI2CWrite(0x58, 0x0); // VS4210 //black content ryan@20200523
}
