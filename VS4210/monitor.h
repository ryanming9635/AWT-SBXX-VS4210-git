/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Monitor.h		                                            										*/
/*                                                                           										*/
/*****************************************************************************/
#ifndef __MONITOR__H_
#define __MONITOR__H_

/* monitor.h */
#define DATA  //_data
#define PDATA //		_pdat
#define IDATA //	idat
#define CODE  //_rom
#define PDATA_P

#define TW88I2CAddress 1
#define ReadTW88(x)
//#define ReadI2C(x, y)
//#define WriteI2C(x, y, z);
#define WriteTW88(x, y) ;

#define BUF_MAX 30
#define DVR_BUF_MAX 30

void Mon_tx(tByte ch);

void MonHelp(void);
BYTE MonGetCommand(void);
void Monitor(void);
void Prompt(void);

BYTE GetMonAddress(void);
void SetMonAddress(tByte addr);
void MonReadI2C(void);
void MonWriteI2C(void);
BYTE Asc1Bin(BYTE asc);
BYTE Asc2Bin(BYTE *s);
int stricmp(BYTE *ptr1, BYTE *ptr2);

extern void SET_PWM(tByte index, tByte val);
extern void tp28xx_byte_write(tByte index, tByte val);
extern tByte tp28xx_byte_read(tByte index);
extern void MCUTimerReactiveTimerEvent(WORD usTime, BYTE ucEventID);
extern void SW_I2C1Write(uint8_t chipID, uint8_t reg, uint8_t val);
extern uint8_t SW_I2C1Read(uint8_t chipID, uint8_t reg);
extern void user_pwm_setvalue(uint16_t value);
extern WORD GET_ALC122(BYTE reg);
extern void SET_ALC122(BYTE addr, WORD val);
extern void GET_nGT911(WORD Reg, BYTE *pData, BYTE N);
extern void SET_nGT911(WORD Reg, BYTE *pData, BYTE N);
extern void VS4210_SwitchChannel(tByte ch, tByte win);
extern void SetNowMode(void);
extern void SetCOCCom(BYTE action, BYTE param);
extern void COMMAND_OSD(BYTE index);
#endif
