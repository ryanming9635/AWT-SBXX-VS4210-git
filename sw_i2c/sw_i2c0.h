#ifndef DEF_SW_I2C0_H_
#define DEF_SW_I2C0_H_
#include "VXIS_Extern_Define.h"

extern void SW_I2C0_Init(void);
extern void I2C0_Write(uint8_t chipID, uint8_t reg, uint8_t val);
extern void I2C0_WriteNByte(uint8_t chipID, uint8_t reg, const uint8_t *ptr, uint8_t NByte);
extern uint8_t I2C0_Read(uint8_t chipID, uint8_t reg);

// extern void I2C0_TC9548A_CtrWrite(uint8_t channel) ;
extern tByte HDMIRX_ReadI2C_Byte(tByte RegAddr);
extern tByte HDMIRX_WriteI2C_Byte(tByte RegAddr, tByte val);
extern tByte HDMIRX_WriteI2C_ByteN(tByte RegAddr, tcByte *pData, int N);

// TX
// HDMI_TX
// extern tByte HDMITX_ReadI2C_Byte(tByte RegAddr) ;
// extern int HDMITX_WriteI2C_Byte(tByte RegAddr,tByte d) ;
// extern int HDMITX_ReadI2C_ByteN(tByte RegAddr,tByte *pData,int N) ;
// extern int HDMITX_SetI2C_Byte(tByte Reg,tByte Mask,tByte Value);

#endif
