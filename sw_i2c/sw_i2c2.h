#ifndef DEF_SW_I2C2_H_
#define DEF_SW_I2C2_H_
#include "VXIS_Extern_Define.h"

extern void SW_I2C2_Init(void);
extern void SW_I2C2Write(uint8_t chipID, uint8_t reg, uint8_t val);
extern void SW_I2C2WriteNByte(uint8_t chipID, uint8_t reg, const uint8_t *ptr, uint8_t NByte);

extern uint8_t SW_I2C2Read(uint8_t chipID, uint8_t reg);
extern void SW_I2C2nRead(uint8_t chipID, uint8_t reg, uint8_t *rBuf, uint8_t n);

// TX
// HDMI_TX
extern tByte HDMITX_ReadI2C_Byte(tByte RegAddr);
extern int HDMITX_WriteI2C_Byte(tByte RegAddr, tByte d);
extern int HDMITX_ReadI2C_ByteN(tByte RegAddr, tByte *pData, int N);
extern int HDMITX_SetI2C_Byte(tByte Reg, tByte Mask, tByte Value);

extern BYTE GET_PCA9554PW(BYTE reg);
extern void SET_PCA9554PW(BYTE addr, BYTE val);
BYTE ReadI2C(BYTE addr, BYTE index);
void WriteI2C(BYTE addr, BYTE index, BYTE val);
WORD ReadI2C3(BYTE addr, BYTE index);
void WriteI2C3(BYTE addr, BYTE index, WORD val);

#endif
