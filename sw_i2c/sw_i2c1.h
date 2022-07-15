#ifndef DEF_SW_I2C1_H_
#define DEF_SW_I2C1_H_
#include "VXIS_Extern_Define.h"

extern void SW_I2C1_Init(void);

extern void SW_I2C1Write(uint8_t chipID, uint8_t reg, uint8_t val);
extern void SW_I2C1WriteNByte(uint8_t chipID, uint8_t reg, const uint8_t *ptr, uint8_t NByte);

extern uint8_t SW_I2C1Read(uint8_t chipID, uint8_t reg);
extern void SW_I2C1nRead(uint8_t chipID, uint8_t reg, uint8_t *rBuf, uint8_t n);
// extern void I2C1_TC9548A_CtrWrite(uint8_t channel) ;
extern void msleep(unsigned int i);

void EEPWrite(WORD index, BYTE dat);
BYTE EEPRead(WORD index);

#endif
