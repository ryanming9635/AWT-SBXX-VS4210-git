#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* memset */

#include <sw_i2c0.h>
#include <sw_i2c1.h>
#include <sw_i2c2.h>
#include "VX_swi2c.h"

extern tByte TVII2CAddress;
tByte gskipsleepflg;
extern WORD BAT_VOLTAGE, BAT_ADC;

#ifndef ITE66121_ADDR
#define ITE66121_ADDR 0x98
#endif

// Sleep
void msleep(unsigned int i)
{
    if (gskipsleepflg == 0)
    {
        HAL_Delay(i);
        // printf("VS4210_ADDR=0x%02x, msleep=%d\r\n",(int)VS4210_ADDR , i) ;
        // usleep(i*1000);
    }
}

void delayTX1ms(unsigned int i)
{
    HAL_Delay(i);
}

uint8_t VXISI2CRead(uint8_t reg)
{
    uint8_t val = 0;
    val = SW_I2C1Read(VS4210_ADDR, reg);
    //    printf("R 0x%02x,0x%02x,0x%02x\r\n",(int)VS4210_ADDR, (int)reg,(int)val) ;
    return val;
}

void VXISI2CWrite(uint8_t reg, uint8_t val)
{
    SW_I2C1Write(VS4210_ADDR, reg, val);
}

uint8_t VS8812Read(uint8_t reg)
{
    uint8_t val = 0;
    val = SW_I2C1Read(VS8812_ADDR, reg);
    //    printf("R 0x%02x,0x%02x,0x%02x\r\n",(int)VS4210_ADDR, (int)reg,(int)val) ;
    return val;
}

void VS8812Write(uint8_t reg, uint8_t val)
{
    SW_I2C1Write(VS8812_ADDR, reg, val);
}

unsigned char ReadEEP(unsigned int reg)
{
    unsigned char val = 0;
    val = EEPRead(reg);
    //    printf("R 0x%02x,0x%02x,0x%02x\r\n",(int)VS4210_ADDR, (int)reg,(int)val) ;
    return val;
}

void WriteEEP(unsigned int reg, unsigned char val)
{
    EEPWrite(reg, val);
}

void I2CMultiWrite(unsigned char maddr, unsigned char saddr, unsigned char number, const unsigned char *value)
{
    SW_I2C1WriteNByte(maddr, saddr, value, number);
}

void I2CMultiAttribute(unsigned char maddr, unsigned char saddr, unsigned char number, unsigned char *value)
{
    uint8_t aAttribute[32];
    memset(aAttribute, value[0], number);
    SW_I2C1WriteNByte(maddr, saddr, &aAttribute[0], number);
}

void osd_cont_write(unsigned char cw_dest, unsigned char init_addr, unsigned short num_of_data, const unsigned char *buf_data) // 20200110-01
{                                                                                                                              ////Extern OSD
    unsigned char wbuf[255];
    wbuf[0] = cw_dest;
    wbuf[1] = init_addr;
    memcpy(&wbuf[2], buf_data, num_of_data);
    SW_I2C1WriteNByte(VS4210_ADDR, 0xFD, &wbuf[0], num_of_data + 2);
}

// RX
void delay(unsigned int i)
{ // 5ms
    tWord j;
    for (j = 0; j < i; j++)
    {
        HAL_Delay(5);
    }
}

tByte tp28xx_byte_read(tByte index)
{
    tByte val;
    val = I2C0_Read(TVII2CAddress, index);

    return val;
}

void tp28xx_byte_write(tByte index, tByte val)
{
    I2C0_Write(TVII2CAddress, index, val);
}

// HDMI_TX

void delay1ms(unsigned int i)
{
    HAL_Delay(i);
}

tByte HDMITX_ReadI2C_Byte(tByte RegAddr)
{
    tByte val;
    val = SW_I2C2Read(ITE66121_ADDR, RegAddr);

    return val;
}

int HDMITX_WriteI2C_Byte(tByte RegAddr, tByte d)
{
    // tByte val ;
    SW_I2C2Write(ITE66121_ADDR, RegAddr, d);

    //    HAL_Delay(5 );
    //    val = SW_I2C2Read(ITE66121_ADDR, RegAddr)  ;
    //    printf("R 0x%02x,0x%02x,W=0x%02x,R=0x%02x\r\n",(int)ITE66121_ADDR, (int)RegAddr,(int)d, (int)val) ;

    return 0;
}

// HDCP

//#ifdef SUPPORT_HDCP
int HDMITX_ReadI2C_ByteN(tByte RegAddr, tByte *pData, int N)
{
    SW_I2C2nRead(ITE66121_ADDR, RegAddr, pData, N);
    return 0;
}
//#endif

int HDMITX_SetI2C_Byte(tByte Reg, tByte Mask, tByte Value)
{
    tByte Temp;
    if (Mask != 0xFF)
    {
        Temp = HDMITX_ReadI2C_Byte(Reg);
        Temp &= (~Mask);
        Temp |= Value & Mask;
    }
    else
    {
        Temp = Value;
    }
    HDMITX_WriteI2C_Byte(Reg, Temp);
    return 0;
}

BYTE GET_PCA9554PW(BYTE reg)
{
    BYTE ret = 0;

    ret = ReadI2C(PCA9554PW_addr, reg); // Modified. David. 20180130

    return ret;
}

void SET_PCA9554PW(BYTE addr, BYTE val)
{
    BYTE ret = 0;
    ret = ReadI2C(PCA9554PW_addr, PCA9554PW_reg1);

    switch (addr)
    {
    case _COL_PW_5V:
        ret &= ~(_COL_PW_5V);
        WriteI2C(PCA9554PW_addr, PCA9554PW_reg1, (ret | ((val ^ 1) << 4)));
        break;
    case _PCON5V_AUDIO:
        ret &= ~(_PCON5V_AUDIO);
        WriteI2C(PCA9554PW_addr, PCA9554PW_reg1, (ret | (val << 5)));
        break;
    case _CCD12_SW:
        ret &= ~(_CCD12_SW);
        WriteI2C(PCA9554PW_addr, PCA9554PW_reg1, (ret | (val << 6)));
        break;
    case _Shutter_SW:
        ret &= ~(_Shutter_SW);
        WriteI2C(PCA9554PW_addr, PCA9554PW_reg1, (ret | (val << 7)));
        break;
    }
}

WORD GET_LTC2451(BYTE reg)
{
    WORD ret = 0;
    WORD val1, val2;

    ret = ReadI2C4(LTC2451_addr, reg); // Modified. David. 20180130

    BAT_ADC = ret;
    val1 = ((BAT_ADC / 10) * 5) / 10;
    val2 = ((BAT_ADC % 10) * 5) / 100;
    val1 = ((val1 + val2) * (75)) / 100;
    BAT_VOLTAGE = val1;

    ret = BAT_VOLTAGE;

    return ret;
}

void SET_LTC2451(BYTE addr, BYTE val)
{
    //	BYTE ret=0;
    //	ret=ReadI2C3(PCA9554PW_addr, PCA9554PW_reg1);

    /*
        switch(addr)
            {
             case _COL_PW_5V:   	ret&=~(_COL_PW_5V); 		WriteI2C( PCA9554PW_addr, PCA9554PW_reg1, (ret|((val^1)<<4)));  break;
             case _PCON5V_AUDIO:	ret&=~(_PCON5V_AUDIO); 	WriteI2C( PCA9554PW_addr, PCA9554PW_reg1, (ret|(val<<5)));  break;
             case _CCD12_SW:		ret&=~(_CCD12_SW); 		WriteI2C( PCA9554PW_addr, PCA9554PW_reg1, (ret|(val<<6)));  break;
             case _Shutter_SW:		ret&=~(_Shutter_SW); 		WriteI2C( PCA9554PW_addr, PCA9554PW_reg1, (ret|(val<<7)));  break;
            }
        */
}

WORD GET_ALC122(BYTE reg)
{
    WORD ret = 0;

    ret = ReadI2C3(ALC122_addr, reg);

    return ret;
}

void SET_ALC122(BYTE addr, WORD val)
{
    WriteI2C3(ALC122_addr, addr, val);
}

void GET_nGT911(WORD Reg, BYTE *pData, BYTE N)
{
    // WORD ret=0;

    //	ret=I2C3_nRead(GT911_addr, Reg, pData, N);
    // ret=
    I2C4_nRead(GT911_addr, Reg, pData, N);

    // return ret;
}

void SET_nGT911(WORD Reg, BYTE *pData, BYTE N)
{
    I2C3_WriteNByte(GT911_addr, Reg, pData, N);
}

//
//
//
