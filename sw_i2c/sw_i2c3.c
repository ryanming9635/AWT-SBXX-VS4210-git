#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#include "VXIS_Extern_Define.h"

#define PIN_SCL3 GPIO_PIN_5
#define PIN_SDA3 GPIO_PIN_4

extern tByte gHDMI_Index;
extern void delay1ms(unsigned int i);

#define GT_CMD_WR 0X28
#define GT_CMD_RD 0X29

static __inline void TWI_SCL_0(void)
{
    GPIOC->BRR = PIN_SCL3;
}
static __inline void TWI_SCL_1(void)
{
    GPIOC->BSRR = PIN_SCL3;
}
static __inline void TWI_SDA_0(void)
{
    GPIOC->BRR = PIN_SDA3;
}
static __inline void TWI_SDA_1(void)
{
    GPIOC->BSRR = PIN_SDA3;
}
static __inline uint8_t TWI_SDA_STATE(void)
{
    return (GPIOC->IDR & PIN_SDA3) != 0;
}

static const uint8_t TWI_ACK = 0;
static const uint8_t TWI_READY = 0;
static const uint8_t TWI_NACK = 1;
static const uint8_t TWI_BUS_BUSY = 2;
static const uint8_t TWI_BUS_ERROR = 3;
// static const uint8_t  TWI_RETRY_COUNT = 3;
// extern void Delay_mS(uint32_t n);

static void TWI_NOP(void)
{
    uint8_t j;
    // for (j = 0; j < 12 ; j++)
    for (j = 0; j < 16; j++) // ryan@20210623 for GD32F103RET
    {
    }
}

static void TWI_Initialize(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pin = PIN_SDA3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; // GPIO_Mode_Out_PP;
    GPIO_InitStructure.Pin = PIN_SCL3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    TWI_SDA_1();
    TWI_SCL_0();
}

static uint8_t TWI_START(void)
{
    TWI_SDA_1();
    TWI_NOP();

    TWI_SCL_1();
    TWI_NOP();

    if (!TWI_SDA_STATE())
    {
        return TWI_BUS_BUSY;
    }
    TWI_SDA_0();
    TWI_NOP();

    TWI_SCL_0();
    TWI_NOP();

    if (TWI_SDA_STATE())
    {
        return TWI_BUS_ERROR;
    }

    return TWI_READY;
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_STOP
 */
/* --------------------------------------------------------------------------*/
static void TWI_STOP(void)
{
    TWI_SDA_0();
    TWI_NOP();

    TWI_SCL_1();
    TWI_NOP();

    TWI_SDA_1();
    TWI_NOP();
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_SendACK
 */
/* --------------------------------------------------------------------------*/

static void TWI_SendACK(void)
{
    TWI_SDA_0();
    TWI_NOP();
    TWI_SCL_1();
    TWI_NOP();
    TWI_SCL_0();
    TWI_NOP();
    TWI_SDA_1();
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_SendNACK
 */
/* --------------------------------------------------------------------------*/

static void TWI_SendNACK(void)
{
    TWI_SDA_1();
    TWI_NOP();
    TWI_SCL_1();
    TWI_NOP();
    TWI_SCL_0();
    TWI_NOP();
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_SendByte
 *
 * @Param: Data
 *
 * @Returns:
 */
/* --------------------------------------------------------------------------*/
static uint8_t TWI_SendByte(uint8_t Data)
{
    uint8_t i;
    TWI_SCL_0();
    for (i = 0; i < 8; i++)
    {
        if (Data & 0x80)
        {
            TWI_SDA_1();
        }
        else
        {
            TWI_SDA_0();
        }
        TWI_NOP();
        TWI_SCL_1();
        TWI_NOP();
        TWI_SCL_0();
        Data <<= 1;
        // TWI_NOP();
        //---------------------------
    }
    TWI_SDA_1();
    TWI_NOP();
    TWI_SCL_1();
    TWI_NOP();
    if (TWI_SDA_STATE())
    {
        TWI_SCL_0();
        TWI_SDA_1();

        return TWI_NACK;
    }
    else
    {
        TWI_SCL_0();
        TWI_SDA_1();

        return TWI_ACK;
    }
}

/* --------------------------------------------------------------------------*/
/**
 * @Brief:  TWI_ReceiveByte
 *
 * @Returns:
 */
/* --------------------------------------------------------------------------*/
static uint8_t TWI_ReceiveByte(void)
{
    uint8_t i, Dat;
    TWI_SDA_1();
    TWI_SCL_0();
    Dat = 0;
    for (i = 0; i < 8; i++)
    {
        TWI_SCL_1();
        TWI_NOP();
        Dat <<= 1;
        if (TWI_SDA_STATE())
        {
            Dat |= 0x01;
        }
        TWI_SCL_0();
        TWI_NOP();
    }
    return Dat;
}

void SW_I2C3_Init(void)
{
    TWI_Initialize();
}

void I2C3_Write(uint8_t chipID, uint8_t reg, uint8_t val)
{
    TWI_START();
    TWI_SendByte(chipID);
    TWI_SendByte(reg);
    TWI_SendByte(val);
    TWI_STOP();
    if (chipID == 0xA0)
    {
        delay1ms(10);
    }
}

// void I2C3_WriteNByte(uint8_t chipID ,uint8_t reg, const uint8_t *ptr ,uint8_t NByte )
void I2C3_WriteNByte(uint8_t chipID, WORD reg, uint8_t *ptr, uint8_t NByte)
{
#if 1
    uint8_t i;
    uint8_t val;
    I2CStart3();
    I2CWriteData5(chipID);
    // WI_SendACK();
    I2CWriteData5(reg >> 8);
    // I_SendACK();
    I2CWriteData5(reg & 0xFF);
    // TWI_SendACK();

    for (i = 0; i < NByte; i++)
    {
        val = ptr[i];
        I2CWriteData5(val);
        // I2CACK3;/// TWI_SendACK();
    }
    I2CStop3();
#else
    uint8_t i;
    uint8_t val;
    TWI_START();
    TWI_SendByte(chipID);
    TWI_SendACK();
    TWI_SendByte(reg >> 8);
    TWI_SendACK();
    TWI_SendByte(reg & 0xFF);
    TWI_SendACK();

    for (i = 0; i < NByte; i++)
    {
        val = ptr[i];
        TWI_SendByte(val);
        TWI_SendACK();
    }
    TWI_STOP();
#endif
}

/// HDMI_ RX TX
// void I2C0_TC9548A_CtrWrite(uint8_t channel)
//{
//     uint8_t ch ;
//     ch = (uint8_t)(1 << channel ) ;
//     TWI_START();
//     TWI_SendByte(0xE0 ) ;
//     TWI_SendByte(ch) ;
//     TWI_STOP();
//     delay1ms(5) ;
// }

uint8_t I2C3_Read(uint8_t chipID, uint8_t reg)
{
    uint8_t val;
    TWI_START();
    TWI_SendByte(chipID);
    TWI_SendByte(reg);
    // TWI_STOP();  //VXIS
    TWI_NOP();
    // TWI_NOP();
    TWI_START();
    TWI_SendByte(chipID | 0x01);
    val = TWI_ReceiveByte();

    TWI_SendNACK();

    TWI_STOP();

    return val;
}

// void I2C3_nRead(uint8_t chipID ,uint8_t reg ,uint8_t *rBuf , uint8_t n )
void I2C3_nRead(uint8_t chipID, WORD reg, uint8_t *rBuf, uint8_t n)
{
    // uint8_t val ;
    uint8_t i;
    TWI_START();
    TWI_SendByte(chipID);
    TWI_SendACK();
    TWI_SendByte(reg >> 8);
    TWI_SendACK();
    TWI_SendByte(reg & 0xFF);
    TWI_SendACK();
    //   TWI_STOP();

    TWI_START();
    TWI_SendByte(chipID | 0x01);
    TWI_SendACK();

    for (i = 1; i < n; i++)
    {
        rBuf[i - 1] = TWI_ReceiveByte();
        TWI_SendACK();
    }
    rBuf[n - 1] = TWI_ReceiveByte();
    TWI_SendNACK();

    TWI_STOP();
    // return val ;
}

//==========================================================
//==========================================================
//==========================================================

void I2Cdelay3(void)
{
    int i;
    for (i = 0; i < 10; i++)
    {
    };
}

void I2CStart3(void)
{
    TWI_SCL_1();
    I2Cdelay3();
    TWI_SDA_1();
    I2Cdelay3();
    TWI_SDA_0();
    I2Cdelay3();
}

void I2CReStart3(void)
{
    TWI_SDA_1();
    I2Cdelay3();
    TWI_SCL_1();
    I2Cdelay3();
    TWI_SDA_0();
    I2Cdelay3();
}

void I2CStop3(void)
{
    TWI_SDA_0();
    I2Cdelay3();
    TWI_SCL_0();
    I2Cdelay3();
    TWI_SCL_1();
    I2Cdelay3();
    TWI_SDA_1();
    I2Cdelay3();
    //	I2C_SCL = 1;	I2Cdelay();
    //	I2C_SDA = 1;
}

/*
void I2CWriteData(BYTE value)
{
    int i;

    for(i=0; i<8; i++){
        TWI_SCL_0();
        I2Cdelay3();
        if((value & 0x80) != 0)
            TWI_SDA_1();
        else
            TWI_SDA_0();
        I2Cdelay3();
        TWI_SCL_1();
        I2Cdelay3();//vhuang
        TWI_SCL_0();
        I2Cdelay3();
        value<<=1;
    }
    TWI_SCL_0();	I2Cdelay3();
    TWI_SCL_1();	I2Cdelay3();
    TWI_SCL_0();	I2Cdelay3();
}

BYTE I2CReadData(void)
{
    int i;
    BYTE value=0;


    for(i=0; i<8; i++) {
        TWI_SCL_0();
        I2Cdelay3();
        TWI_SCL_1();
        value <<= 1;
        if(TWI_SDA_STATE()) value |= 1;
        I2Cdelay3();
        TWI_SCL_0();
        I2Cdelay3();
    }

    TWI_SCL_0();	I2Cdelay3();
    TWI_SCL_1();	I2Cdelay3();
    TWI_SCL_0();	I2Cdelay3();

    return value;

}
*/
void I2CWriteData5(BYTE value)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        TWI_SCL_0();
        I2Cdelay3();
        if ((value & 0x80) != 0)
            TWI_SDA_1();
        else
            TWI_SDA_0();
        I2Cdelay3();
        TWI_SCL_1();
        I2Cdelay3(); // vhuang
        TWI_SCL_0();
        I2Cdelay3();
        value <<= 1;
    }
    TWI_SCL_0();
    I2Cdelay3();
    TWI_SCL_1();
    I2Cdelay3();
    TWI_SCL_0();
    I2Cdelay3();
}

void I2CWriteData6(BYTE value)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        TWI_SCL_0();
        I2Cdelay3();
        if ((value & 0x80) != 0)
            TWI_SDA_1();
        else
            TWI_SDA_0();
        I2Cdelay3();
        TWI_SCL_1();
        I2Cdelay3(); // vhuang
        TWI_SCL_0();
        I2Cdelay3();
        value <<= 1;
    }
    // TWI_SCL_0();	I2Cdelay();
    // TWI_SCL_1();	I2Cdelay();
    // TWI_SCL_0();	I2Cdelay();
}

BYTE I2CReadData5(void)
{
    int i;
    BYTE value = 0;

    for (i = 0; i < 8; i++)
    {
        TWI_SCL_0();
        I2Cdelay3();
        TWI_SCL_1();
        value <<= 1;
        if (TWI_SDA_STATE())
            value |= 1;
        I2Cdelay3();
        TWI_SCL_0();
        I2Cdelay3();
    }

    //	TWI_SCL_0();	I2Cdelay();
    //	TWI_SCL_1();	I2Cdelay();
    //	TWI_SCL_0();	I2Cdelay();

    return value;
}
/*
BYTE ReadI2C(BYTE addr, BYTE index)
{
    BYTE val;

    I2CStart3();
    I2CWriteData5(addr);
    I2CWriteData5(index);
    I2CReStart3();
    I2CWriteData5(addr | 0x01);
    val = I2CReadData5();
    I2CStop3();

    return val;
}

void WriteI2C(BYTE addr, BYTE index, BYTE val)
{
    I2CStart3();
    I2CWriteData5(addr);
    I2CWriteData5(index);
    I2CWriteData5(val);
    I2CStop3();
}
*/

static void I2CACK3(void)
{
    TWI_SDA_0();
    I2Cdelay3(); /// TWI_NOP();
    TWI_SCL_1();
    I2Cdelay3(); /// TWI_NOP();
    TWI_SCL_0();
    I2Cdelay3(); /// TWI_NOP();
    TWI_SDA_1();
    I2Cdelay3(); /// TWI_NOP();
}

static void I2CNACK3(void)
{
    TWI_SDA_1();
    TWI_NOP();
    TWI_SCL_1();
    TWI_NOP();
    TWI_SCL_0();
    TWI_NOP();
    TWI_SDA_1();
}
/*
WORD ReadI2C3(BYTE addr, BYTE index)
{
    WORD val;

    I2CStart3();
    I2CWriteData5(addr);
//	I2CACK();
    I2CWriteData5(index);
//	I2CACK();
    I2CReStart3();
    I2CWriteData5(addr | 0x01);
//	I2CACK();
    val = I2CReadData5();
    I2CACK3();
    val<<=8;
    val |= I2CReadData5();
    I2CNACK3();
    I2CStop3();

    return val;
}

void WriteI2C3(BYTE addr, BYTE index, WORD val)
{

    I2CStart3();
    I2CWriteData5(addr);
    //I2CACK();
    I2CWriteData5(index);
    //I2CACK();
    I2CWriteData5((val>>8));
//	I2CACK();
    I2CWriteData5((val));
    //I2CACK();
    I2CStop3();
}
*/
void I2C4_nRead(uint8_t chipID, WORD reg, uint8_t *rBuf, uint8_t n)
{
    // uint8_t val ;
    uint8_t i;
    I2CStart3();
    I2CWriteData5(chipID);
    //  I2CACK3();
    I2CWriteData5(reg >> 8);
    //  I2CACK3();
    I2CWriteData5(reg & 0xFF);
    // I2CACK3();

    // I2CStop3();
    I2CStart3();
    I2CWriteData5(chipID | 0x01);
    //  I2CACK3();

    for (i = 1; i < n; i++)
    {
        rBuf[i - 1] = I2CReadData5();
        I2CACK3();
    }
    rBuf[n - 1] = I2CReadData5();
    I2CNACK3();

    I2CStop3();
    // return val ;
}

/*
tByte HDMIRX_ReadI2C_Byte(tByte RegAddr)
{
    tByte uc ;
    if (gHDMI_Index % 2 == 0  )
    {
        uc = I2C0_Read(HDMIRXADR,RegAddr) ;
    }
    else
    {
        uc = I2C0_Read(HDMIRXADR_B,RegAddr) ;
    }

    return uc ;
}
tByte HDMIRX_WriteI2C_Byte(tByte RegAddr,tByte val)
{
    if (gHDMI_Index % 2 == 0 )
    {
        I2C0_Write(HDMIRXADR,RegAddr,val) ;
    }
    else
    {
        I2C0_Write(HDMIRXADR_B,RegAddr,val) ;
    }

    return 0 ;

}

tByte HDMIRX_WriteI2C_ByteN(tByte RegAddr,tcByte *pData,int N)
{
    if (gHDMI_Index % 2 == 0 )
    {
        //void I2C0_WriteNByte(uint8_t chipID ,uint8_t reg, const uint8_t *ptr ,uint8_t NByte )
        I2C0_WriteNByte(HDMIRXADR,RegAddr,pData,(tByte) N) ;
    }
    else
    {
        I2C0_WriteNByte(HDMIRXADR_B,RegAddr, pData ,(tByte) N ) ;
    }
    return 0  ;
}
*/

// HDMI_TX
// tByte HDMITX_ReadI2C_Byte(tByte RegAddr)
//{
//	tByte val ;
//
//     if (gHDMI_Index % 2 == 0 )
//     {
//		val = I2C0_Read(ITE66121_ADDR, RegAddr)  ;
//	}
//	else
//	{
//		val = I2C0_Read(ITE66121_BADDR, RegAddr)  ;
//	}
//	return val ;
// }
//
// int HDMITX_WriteI2C_Byte(tByte RegAddr,tByte d)
//{
//     if (gHDMI_Index % 2 == 0 )
//     {
//		I2C0_Write(ITE66121_ADDR, RegAddr, d )  ;
//
//	}
//	else
//	{
//		I2C0_Write(ITE66121_BADDR, RegAddr, d)  ;
//	}
//     return 0 ;
// }
//
////HDCP
//
////#ifdef SUPPORT_HDCP
// int HDMITX_ReadI2C_ByteN(tByte RegAddr,tByte *pData,int N)
//{
//     //int i ;
//	tByte chipid ;
//     if (gHDMI_Index % 2 == 0 )
//     {
//		chipid = ITE66121_ADDR ;
//	}
//	else
//	{
//		chipid = ITE66121_BADDR ;
//	}
//     I2C0_nRead(chipid, RegAddr, pData , N )  ;
//
////    for (i = 0 ; i< N ; i++)
////    {
////		pData[i] = I2C0_Read(chipid, RegAddr) ;
////    }
//
//	return 0 ;
//}
////#endif
//
//
// int HDMITX_SetI2C_Byte(tByte Reg,tByte Mask,tByte Value)
//{
//    tByte Temp;
//    if( Mask != 0xFF )
//    {
//        Temp=HDMITX_ReadI2C_Byte(Reg);
//        Temp&=(~Mask);
//        Temp|=Value&Mask;
//    }
//    else
//    {
//        Temp=Value;
//    }
//    //return HDMITX_WriteI2C_Byte(Reg,Temp);
//    HDMITX_WriteI2C_Byte(Reg,Temp);
//    return 0 ;
//}
