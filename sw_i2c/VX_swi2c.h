#ifndef VX_SWI2C_H_
#define VX_SWI2C_H_
#include "VXIS_Extern_Define.h"

#define PCA9554PW_addr 0x40
#define GT911_addr 0x28
#define LTC2451_addr 0x28
//#define GT911_addr  		0xBA

#define ALC122_addr 0x4A

#define PCA9554PW_reg0 0
#define PCA9554PW_reg1 1
#define PCA9554PW_reg2 2
#define PCA9554PW_reg3 3

#define _COL_PW_5V 0x10
#define _PCON5V_AUDIO 0x20
#define _CCD12_SW 0x40
#define _Shutter_SW 0x80

extern tByte gskipsleepflg;
extern volatile tByte VS4210_ADDR;
extern volatile tByte VS8812_ADDR;

extern void delay1ms(unsigned int i);
extern void msleep(unsigned int i);
extern void delay(unsigned int i);
// extern uint8_t VXISI2CInit(uint8_t sclpin , uint8_t sdapin) ;
// extern uint8_t VXISI2CRead( uint8_t reg) ;
uint8_t VXISI2CInit(uint8_t sclpin, uint8_t sdapin);
uint8_t VXISI2CRead(uint8_t reg);

extern void VXISI2CWrite(uint8_t reg, uint8_t val);
extern void I2CMultiWrite(unsigned char maddr, unsigned char saddr, unsigned char number, const unsigned char *value);
extern void I2CMultiAttribute(unsigned char maddr, unsigned char saddr, unsigned char number, unsigned char *value);
extern void osd_cont_write(unsigned char cw_dest, unsigned char init_addr, unsigned short num_of_data, const unsigned char *buf_data); // 20200110-01
extern BYTE ReadI2C(BYTE addr, BYTE index);
extern void WriteI2C(BYTE addr, BYTE index, BYTE val);
extern WORD ReadI2C4(BYTE addr, BYTE index);
extern void I2C4_nRead(uint8_t chipID, WORD reg, uint8_t *rBuf, uint8_t n);
extern void I2C3_WriteNByte(uint8_t chipID, WORD reg, uint8_t *ptr, uint8_t NByte);

unsigned char ReadEEP(unsigned int reg);
void WriteEEP(unsigned int reg, unsigned char val);
WORD GET_ALC122(BYTE reg);
void SET_ALC122(BYTE addr, WORD val);
void GET_nGT911(WORD Reg, BYTE *pData, BYTE N);
void SET_nGT911(WORD Reg, BYTE *pData, BYTE N);
WORD GET_LTC2451(BYTE reg);
void SET_LTC2451(BYTE addr, BYTE val);

uint8_t VS8812Read(uint8_t reg);
void VS8812Write(uint8_t reg, uint8_t val);
// extern void EEPWrite(WORD index, BYTE dat);
// extern BYTE EEPRead(WORD index);

// RX
extern void delay(unsigned int i); // 5ms
extern tByte tp28xx_byte_read(tByte index);
extern void tp28xx_byte_write(tByte index, tByte val);

// TX
extern tByte HDMITX_ReadI2C_Byte(tByte RegAddr);
extern int HDMITX_WriteI2C_Byte(tByte RegAddr, tByte d);
// HDCP
//#ifdef SUPPORT_HDCP
extern int HDMITX_ReadI2C_ByteN(tByte RegAddr, tByte *pData, int N);
//#endif
extern int HDMITX_SetI2C_Byte(tByte Reg, tByte Mask, tByte Value);

#endif
