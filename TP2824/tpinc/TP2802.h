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

enum
{
	CH1 = 0,
	CH2,
	CH3,
	CH4,
	AutoSel
};

enum
{
	FCH1 = 0,
	FCH2 = 2,
	FCH3 = 3,
	FCH4 = 1,
};

enum
{
	VIN1_Y = 0,
	VIN2_Y,
	VIN3_Y,
	VIN4_Y,
	VIN1_C,
	VIN2_C,
	VIN3_C,
	VIN4_C
};

enum
{
	R720P60 = 0x00,
	R720P50 = 0x01,
	R1080P30 = 0x02,
	R1080P25 = 0x03,
	R720P30 = 0x04,
	R720P25 = 0x05,
	NTPAL = 0x06,
	NoDet = 0x07,
	R720P30V2 = 0x0C,
	R720P25V2 = 0x0D,
	NTSC = 0x08,
	PAL = 0x09,
	R3M18 = 0x20,
	R5M12 = 0x21,
	R4M15 = 0x22,
	R3M20 = 0x23,
	R4M12 = 0x24,
	R6M10 = 0x25,
	R4M30 = 0x26
};
enum
{
	RES1280X720 = 0x00,
	RES1920X1080 = 0x01,
	RES1920X480 = 0x02,
	RES1920X576 = 0x03,
	RES2048X1536 = 0x04,
	RES2056X1944 = 0x05
};
enum
{
	TVI,
	HDA,
	HDC
};
enum
{
	VIDEO_UNPLUG,
	VIDEO_IN,
	VIDEO_LOCKED,
	VIDEO_UNLOCK
};

#define FLAG_LOSS 0x80
#define TVI_FLAG_LOCKED 0x60
#define CVBS_FLAG_LOCKED 0x60
#define FLAG_HDC_MODE 0x80
#define FLAG_HALF_MODE 0x40
#define FLAG_MEGA_MODE 0x20
#define FLAG_HDA_MODE 0x10

#define Bits16out 0
#define Bits8out 1

#define OUTPUT Bits8out
//#define OUTPUT Bits16out
#define EQ_COUNT 20
#define EQ_DIFF 2

#define NoVid 0
#define IsVid 1

#define Tp2802_Boot_Set_std 0
#define Tp2802_Boot_Set_output 1
#define Tp2802_Boot_Set_gain 2
#define Tp2802_Boot_Set_COC 3

void Init_TP280x_RegSet(void);
unsigned char Set_ResWithAutoDet(unsigned char ch);
void Sel_VD_Out(unsigned char ch);
void Init_TP280x_CHx_RegSet(BYTE Action, BYTE index);
void Boot_Trigger_Function(BYTE ch);
void SetTP2824CHVloss(BYTE index);

// void Menu(void);
// void Left(void);
// void Right(void);
// void Up(void);
// void Down(void);

void EQ_Detect(void);
// void MonTX(BYTE,BYTE,BYTE,BYTE,BYTE);
// void BurstTX(BYTE *buf);
void SetCOCCom(BYTE action, BYTE param);
void SETChannelReset(BYTE index);

extern void Printf(const char /*CODE_P*/ *fmt, ...);
void Get_2824cIrq(void); // cocrx
void TVI_BOX_Set(void);
extern BYTE GetDisplayedOSD(void);
extern void BackLightON(void);
extern void ShowJUMPOSD(void);
extern void user_pwm_setvalue(uint16_t value);
extern int GetBackLightLevel(void);

#endif
