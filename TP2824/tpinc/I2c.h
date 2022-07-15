// *****************************************************
// Company : Techpoint Inc
// $Id: I2c.h 16792 2013-10-17 02:08:40Z SJ $
// $Author: SJ $
// $Date: 2013-10-17 18:08:40 -0800 (Thu, 17 Oct 2013) $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef	__I2C_H__
#define	__I2C_H__


void I2CReadByte3(BYTE *);
void I2CRead8Byte(BYTE *);
void I2CReadByteMSP(BYTE addr, BYTE index, BYTE *pd, BYTE cnt);
BYTE I2CWriteByte2(BYTE);
BYTE I2CWriteByte3(BYTE*);
BYTE I2CWrite8Byte(BYTE *);

void I2CStart(void);
void I2CReStart(void);

void I2CWriteData(BYTE value);
BYTE I2CReadData(void);
BYTE I2CReadDataWithACK(void);
BYTE I2CReadDataWithNAK(void);
void I2CStop(void);
void I2Cdelay(void);

BYTE tp28xx_byte_read		(BYTE addr, BYTE index);
WORD ReadI2C16		(BYTE addr, BYTE indexH, BYTE indexL);
void tp28xx_byte_write		(BYTE addr, BYTE index, BYTE val);
void WriteI2C16		(BYTE addr, BYTE indexH, BYTE indexL, BYTE valH, BYTE valL);
//BYTE WriteI2Cn		(BYTE addr, BYTE index, BYTE *val, BYTE cnt);
BYTE WriteI2Cn_(BYTE addr, BYTE *val, BYTE cnt);

//BYTE ReadI2CStatus( BYTE addr );

//void ReadI2Cmn(BYTE addr, BYTE wcnt, BYTE rcnt, BYTE *rwdata);


//void MDC_delay(void);
//void BitWriteMDIO(BYTE wbit);
//int ReadMDIO(BYTE chipno, BYTE regno);
//void WriteMDIO(BYTE chipno, BYTE regno, int regvalue);

#endif	/* __I2C_H__ */
