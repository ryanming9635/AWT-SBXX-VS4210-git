#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#include "VXIS_Extern_Define.h"
#include "VX_swi2c.h"

#define PIN_SCL1 GPIO_PIN_9
#define PIN_SDA1 GPIO_PIN_8
#define dd() I2Cdelay2()

// extern tByte g998xIndex  ;
extern void delay1ms(unsigned int i);

static __inline void TWI_SCL_0(void)
{
    GPIOB->BRR = PIN_SCL1;
}
static __inline void TWI_SCL_1(void)
{
    GPIOB->BSRR = PIN_SCL1;
}
static __inline void TWI_SDA_0(void)
{
    GPIOB->BRR = PIN_SDA1;
}
static __inline void TWI_SDA_1(void)
{
    GPIOB->BSRR = PIN_SDA1;
}
static __inline uint8_t TWI_SDA_STATE(void)
{
    return (GPIOB->IDR & PIN_SDA1) != 0;
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
    // for (j = 0; j < 1 ; j++)
    for (j = 0; j < 2; j++) // ryan@20210623 for GD32F103RET
    {
    }
}

static void TWI_Initialize(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pin = PIN_SDA1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; // GPIO_Mode_Out_PP;
    GPIO_InitStructure.Pin = PIN_SCL1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

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

void SW_I2C1_Init(void)
{
    TWI_Initialize();
}

void SW_I2C1Write(uint8_t chipID, uint8_t reg, uint8_t val)
{
    TWI_START();
    TWI_SendByte(chipID);
    TWI_SendByte(reg);
    TWI_SendByte(val);
    TWI_STOP();
}

void SW_I2C1WriteNByte(uint8_t chipID, uint8_t reg, const uint8_t *ptr, uint8_t NByte)
{
    uint8_t i;
    uint8_t val;
    TWI_START();
    TWI_SendByte(chipID);
    TWI_SendByte(reg);
    for (i = 0; i < NByte; i++)
    {
        val = ptr[i];
        TWI_SendByte(val);
    }

    TWI_STOP();
}

uint8_t SW_I2C1Read(uint8_t chipID, uint8_t reg)
{
    uint8_t val;
    TWI_START();
    TWI_SendByte(chipID);
    TWI_SendByte(reg);
    TWI_STOP(); // VXIS

    TWI_NOP();
    TWI_NOP();

    TWI_START();
    TWI_SendByte(chipID | 0x01);
    val = TWI_ReceiveByte();

    TWI_SendNACK();

    TWI_STOP();
    return val;
}

void SW_I2C1nRead(uint8_t chipID, uint8_t reg, uint8_t *rBuf, uint8_t n)
{
    uint8_t i;
    TWI_START();
    TWI_SendByte(chipID);
    TWI_SendByte(reg);
    // TWI_STOP();  //VXIS
    TWI_NOP();
    // TWI_NOP();
    TWI_START();
    TWI_SendByte(chipID | 0x01);

    for (i = 1; i < n; i++)
    {
        rBuf[i - 1] = TWI_ReceiveByte();
        TWI_SendACK();
    }
    rBuf[n - 1] = TWI_ReceiveByte();
    TWI_SendNACK();

    TWI_STOP();
}

//==========================================================================
//==========================================================================
//==========================================================================

void I2Cdelay2(void)
{
    int i;
    for (i = 0; i < 10; i++)
    {
    };
}

void I2CStart2(void)
{
    TWI_SCL_1();
    I2Cdelay2();
    TWI_SDA_1();
    I2Cdelay2();
    TWI_SDA_0();
    I2Cdelay2();
}

void I2CReStart2(void)
{
    TWI_SDA_1();
    I2Cdelay2();
    TWI_SCL_1();
    I2Cdelay2();
    TWI_SDA_0();
    I2Cdelay2();
}

void I2CStop2(void)
{
    TWI_SDA_0();
    I2Cdelay2();
    TWI_SCL_0();
    I2Cdelay2();
    TWI_SCL_1();
    I2Cdelay2();
    TWI_SDA_1();
    I2Cdelay2();
    //	I2C_SCL = 1;	I2Cdelay();
    //	I2C_SDA = 1;
}

void I2CWriteData2(BYTE value)
{
    /*
        int i;

        for(i=0; i<8; i++, value<<=1) {
            I2C_SCL = 0;
            I2Cdelay();	// HHY 2.04 in special case, it was unstable
            if(value & 0x80) I2C_SDA = 1;
            else 			 I2C_SDA = 0;
            I2Cdelay();
            I2C_SCL = 1;
        }
    */

#if 1

    int i;

    for (i = 0; i < 8; i++)
    {
        TWI_SCL_0();
        I2Cdelay2();
        if ((value & 0x80) != 0)
            TWI_SDA_1();
        else
            TWI_SDA_0();
        I2Cdelay2();
        TWI_SCL_1();
        I2Cdelay2(); // vhuang
        TWI_SCL_0();
        I2Cdelay2();
        value <<= 1;
    }
    TWI_SCL_0();
    I2Cdelay2();
    TWI_SCL_1();
    I2Cdelay2();
    TWI_SCL_0();
    I2Cdelay2();
    //}

#else
    TWI_SCL_0();
    TWI_SDA_STATE() = (value & 0x80) ? 1 : 0;
    TWI_SCL_1();
    dd();
    TWI_SCL_0();
    TWI_SDA_STATE() = (value & 0x40) ? 1 : 0;
    TWI_SCL_1();
    dd();
    TWI_SCL_0();
    TWI_SDA_STATE() = (value & 0x20) ? 1 : 0;
    TWI_SCL_1();
    dd();
    TWI_SCL_0();
    TWI_SDA_STATE() = (value & 0x10) ? 1 : 0;
    TWI_SCL_1();
    dd();

    TWI_SCL_0();
    TWI_SDA_STATE() = (value & 0x08) ? 1 : 0;
    TWI_SCL_1();
    dd();
    TWI_SCL_0();
    TWI_SDA_STATE() = (value & 0x04) ? 1 : 0;
    TWI_SCL_1();
    dd();
    TWI_SCL_0();
    TWI_SDA_STATE() = (value & 0x02) ? 1 : 0;
    TWI_SCL_1();
    dd();
    TWI_SCL_0();
    TWI_SDA_STATE() = (value & 0x01) ? 1 : 0;
    TWI_SCL_1();
    dd();

    TWI_SCL_0();
    TWI_SCL_0();
    TWI_SCL_0();
    TWI_SDA_1();

    TWI_SCL_1();
    TWI_SCL_1();
    TWI_SCL_1();
    TWI_SCL_0();
#endif
}

BYTE I2CReadData2()
{
    int i;
    BYTE value = 0;

    TWI_SDA_1();
    dd();
    TWI_SCL_0();

    for (i = 0; i < 8; i++)
    {
        TWI_SCL_1();
        value <<= 1;
        if (TWI_SDA_STATE())
            value |= 1;
        TWI_SCL_0();
    }
    dd();

    TWI_SCL_1();
    TWI_SCL_1();
    TWI_SCL_1();
    TWI_SCL_0();

    return value;
}

BYTE ReadI2C2(BYTE addr, BYTE index)
{
    BYTE val;

    I2CStart2();
    I2CWriteData2(addr);
    I2CWriteData2(index);
    I2CStart2();
    I2CWriteData2(addr | 0x01);
    val = I2CReadData2();
    I2CStop2();

    return val;
}

void EEPWrite(WORD index, BYTE dat)
{
    BYTE addr;

    addr = (0xA0 | (index >> 7)) & 0xfe;

    I2CStart2();
    I2CWriteData2(addr);
    I2CWriteData2((BYTE)index);
    I2CWriteData2(dat);
    I2CStop2();

#ifdef DEBUG_EEP
    dPrintf("Write %02xh to [Adrs(%02xh)Index(%02xh)\r\n", (WORD)dat, (WORD)addr, (WORD)index);
#endif

    // delay(2);
    msleep(20 - 10);
}

BYTE EEPRead(WORD index)
{
    BYTE val;
    BYTE addr;

    addr = (0xA0 | (index >> 7)) & 0xfe;

    val = ReadI2C2(addr, (BYTE)index);

#ifdef DEBUG_OSD
    dPrintf("Read [Adrs:%02xh,Index:%02xh] %02xh\r\n", (WORD)addr, (WORD)index, (WORD)val);
#endif
    //	delay(2);
    msleep(1 /*20-10*/);
    return val;
}

/// VS9989 I2C Switch
// void I2C1_TC9548A_CtrWrite(uint8_t channel)
//{
//     //dbg_printf("channel=%d\r\n",(int) channel ) ;
//     uint8_t ch ;
//	g998xIndex = channel ;
//     if (channel < 8)
//     {
//     ch = (uint8_t)(1 << channel ) ;
//     TWI_START();
//     TWI_SendByte(0xE0 ) ;
//     TWI_SendByte(ch) ;
//     TWI_STOP();
//     delay1ms(5) ;
//     }
// }
