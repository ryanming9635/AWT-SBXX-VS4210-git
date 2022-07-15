// *****************************************************
// Company : Techpoint Inc
// $Id: TP2802.h 16792 2013-10-17 02:08:40Z SJ $
// $Author: SJ $
// $Date: 2014-03-10 $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef __TP2802_H__
#define __TP2802_H__

enum {
	CH1 = 0,
	CH2,
	CH3,
	CH4,
	AutoSel
};

enum{
	VIN1_Y = 0,
	VIN2_Y,
	VIN3_Y,
	VIN4_Y,
	VIN1_C,
	VIN2_C,
	VIN3_C,
	VIN4_C
};

enum{
	R720P60=0x00,
	R720P50=0x01,
	R1080P30=0x02,
	R1080P25=0x03,
	R720P30=0x04,
	R720P25=0x05,
	NTPAL=0x06,
	NoDet=0x07,
	R720P30V2=0x0C,
	R720P25V2=0x0D,
	NTSC=0x08,
	PAL=0x09,
	R3M=0x20,
	R5M=0x21	
};
enum{
	RES1280X720=0x00,
	RES1920X1080=0x01,
	RES1920X480=0x02,
	RES1920X576=0x03,
	RES2048X1536=0x04,
	RES2056X1944=0x05
};
enum{
	TVI,
	HDA,
	HDC
};
enum{
    VIDEO_UNPLUG,
    VIDEO_IN,
    VIDEO_LOCKED,
    VIDEO_UNLOCK
};

#define FLAG_LOSS       0x80
#define TVI_FLAG_LOCKED     0x20
#define CVBS_FLAG_LOCKED	0x60
#define Bits16out	0
#define Bits8out	1

#define NoVid	0
#define IsVid	1

void Init_TP280x_RegSet(void);
unsigned char Set_ResWithAutoDet(unsigned char ch);
void Sel_VD_Out(unsigned char ch);
void EQ_Detect() ; 


#endif

