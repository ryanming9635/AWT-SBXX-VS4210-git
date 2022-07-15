// *****************************************************
// Company : Techpoint Inc
// $Date: 2014-03-10
// $Log:  $
// ******************************************************
#include "VXIS_Extern_Define.h"
#include "stdlib.h"
//#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"

#include "VX_swi2c.h"
#include "VS4210.h"
#include "etc_eep.h"
#include "keyremo.h"
#include "Printf.h"

// extern tByte ManVidResRx;

tByte ManVidResRx;
extern tByte ManSelCh;
extern tByte gRxLoopFlg;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// TP2802D EQ for short cable option
#define TP2802D_EQ_SHORT 0x0d
#define TP2802D_CGAIN_SHORT 0x74
//#define TP2802D_EQ_SHORT 0x01
//#define TP2802D_CGAIN_SHORT 0x70

//#define CVI_TEST 0
//#define AHD_TEST 1

#define CH_PER_CHIP 0x04
#define MAX_COUNT 0xff

// extern void CreateMDIN340VideoInstance(void);

tByte newInRes[4] = {7, 7, 7, 7};

extern bit DetAutoRes;
tByte state[CH_PER_CHIP];
tByte count[CH_PER_CHIP];
tByte mode[CH_PER_CHIP];
tByte std[CH_PER_CHIP];
tByte egain[CH_PER_CHIP][4];
tByte VideoSave[CH_PER_CHIP];
tByte ch_boot_reset[CH_PER_CHIP] = {0, 0, 0, 0};
tByte revision;
tByte chip;
tByte flag_LOCKED;
tByte TVII2CAddress;
tByte PTZ_buf[16];
tWord PTZ_pelco;
tByte CurRes;
tByte CurCH;
tByte crc;
bit sywd;
bit flag_CVBS;
bit flag_AUDIO;
bit flag_MEGA;

tcByte SYS_MODE[] = {0x01, 0x02, 0x04, 0x08, 0x0f};
tcByte SYS_AND[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xf0};

tcByte REG_F6[4] = {0x00, 0x11, 0x22, 0x33};
tcByte REG_F7[4] = {0x44, 0x55, 0x66, 0x77};
tcByte REG_FA[4] = {0xBB, 0xCC, 0xDD, 0xEE};

// cocrx
tByte tpRxD[26];
BYTE COCRX1[12];
BYTE COCRX2[12];
BYTE COCRX3[12];
BYTE COCRX4[12];
/*
extern BYTE AHDcmd1,AHDcmd2,AHDcmd3,AHDcmd4;
extern BYTE TVI_Command;
extern BYTE presetNum;
extern BYTE TVI_Chksum;
extern BYTE TVI_speed;
extern bit ptztxflag;
*/
BYTE AHDcmd1, AHDcmd2, AHDcmd3, AHDcmd4;
BYTE TVI_Command;
BYTE presetNum;
BYTE TVI_Chksum;
BYTE TVI_speed;
bit ptztxflag;
BYTE Flisheye_CAM[4] = {0};
BYTE Flisheye_COC = 0;

WORD Flisheye_VLOSS_Count[4] = {0};

// BYTE Flisheye_CAM[4]={1,1,1,1};
BYTE COCSelectModeType[4] = {5, 5, 5, 5};
BYTE PreCOCSelectModeType[4] = {5, 5, 5, 5};

// extern BYTE PtzTxChNum;//0:ch1 / 1:ch2 / 2:ch3 / 3:ch4

BYTE PtzTxChNum; // 0:ch1 / 1:ch2 / 2:ch3 / 3:ch4
// extern BYTE QuadBoxId;//1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
BYTE QuadBoxId = 0; // 1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
BYTE QuadBoxMode = 0;
BYTE VideoChangeFlag = 0;

extern BYTE SelectModeType;
extern volatile BYTE MainSetSelectMode[13];
extern BYTE BOOT_ISSUE;
extern BYTE CURRENT_CH;
extern BYTE BOX_ISSUE;
extern BYTE TriggerStatus;
extern BYTE TriggerFlag;
extern BYTE TriggerMode;
extern BYTE PowerFlag;
extern BYTE NowModeState;
extern BYTE bytSelectCAMRD;
extern BYTE bytGarbberLinerMode;
extern BYTE bytResetOSDValue;
extern tByte bSignal[4];

enum
{
	TP2802C = 0x02,
	TP2804 = 0x04,
	TP2802D = 0x05,
	TP2822 = 0x22,
	TP2823 = 0x23,
	TP2834 = 0x24,
	TP2833 = 0x25,
	TP2853C = 0x26,
	TP2824C = 0x27,
	TP2826 = 0x2f
};

tcByte TP2824C_DataSet[] =
	{
		0x40, 0x04,
		0x07, 0xC0,
		0x0B, 0xC0,
		0x21, 0x84,

		0x38, (0x00 | 0x4E), // 0x4E ryan@20220525 Regulus suggusted the value of register for noise issue.(Pseudo differential mode)
		0x3D, 0x40,
		0x41, 0x06,

		0x3A, 0x22,
		0x3B, 0x05,
		0x26, 0x04,
		// 0x4d, 0x07,
		// 0x4e, 0x05,
		0x4d, 0x0F,
		0x4e, 0x0F, // chunyu
		0x6f, 0x60, /// default cvbs coc  ryan@20210416
		0xf1, 0x00, // different with TP2826
		/*cocrx
		//PTZ RX
		//	0xc6, 0x1F,//0x19,	//TXBITCNNUM
		//	0xc7, 0x78,		//TXHST
			0xcb, 0x07,		//RXLINE1
			0xcc, 0x08,		//RXLINE2
			0xcd, 0x00,//0x09,		//RXLINE3
			0xce, 0x00,//0x0a,		//RXLINE4
			0xcf, 0x04,		//RXFREQ
			0xd0, 0x00,		//RXFREQ
			0xd1, 0x00,		//RXFREQ
			0xd2, 0x60,		//RXTHLEVEL
			0xd3, 0x10,		//RXHINUM
			0xd4, 0x06,		//RXHST / RXHEND
			0xd5, 0xbe,		//RXHST
			0xd6, 0x39,		//RXHEND
			0xd7, 0x27,		//RXBITNUM
		*/
		0xff, 0xff};

tcByte TP2824C_DataSetRD[] =
	{
		//    0x40, 0x04,
		0x07, 0xC0,
		0x0B, 0xC0,
		0x21, 0x84,
		0x38, 0x00,
		0x3A, 0x22,
		0x3B, 0x05,
		0x26, 0x04,
		// 0x4d, 0x07,
		// 0x4e, 0x05,
		0x4d, 0x0F,
		0x4e, 0x0F, // chunyu
		0x6f, 0x60, /// default cvbs coc  ryan@20210416
		0xf1, 0x00, // different with TP2826
		/*cocrx
		//PTZ RX
		//	0xc6, 0x1F,//0x19,	//TXBITCNNUM
		//	0xc7, 0x78,		//TXHST
			0xcb, 0x07,		//RXLINE1
			0xcc, 0x08,		//RXLINE2
			0xcd, 0x00,//0x09,		//RXLINE3
			0xce, 0x00,//0x0a,		//RXLINE4
			0xcf, 0x04,		//RXFREQ
			0xd0, 0x00,		//RXFREQ
			0xd1, 0x00,		//RXFREQ
			0xd2, 0x60,		//RXTHLEVEL
			0xd3, 0x10,		//RXHINUM
			0xd4, 0x06,		//RXHST / RXHEND
			0xd5, 0xbe,		//RXHST
			0xd6, 0x39,		//RXHEND
			0xd7, 0x27,		//RXBITNUM
		*/
		0xff, 0xff};

tcByte TP2826_DataSet[] =
	{
		0x40, 0x04,
		0x07, 0xC0,
		0x0B, 0xC0,
		0x21, 0x84,
		0x38, 0x00,
		0x3A, 0x22,
		0x3B, 0x05,
		0x26, 0x04,
		0x4d, 0x07,
		0x4e, 0x05,
		0xf1, 0x07,
		0xff, 0xff};

tcByte TP2826_MODE_DataSet[5][20] =
	{
		{0x0c, 0x0d, 0x20, 0x26, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x39, 0x2c, 0x21, 0x22, 0x23, 0x25, 0x27, 0x28, 0xff}, // ADDR
		{0x03, 0x50, 0x30, 0x05, 0x60, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x1c, 0x0a, 0x84, 0x36, 0x3c, 0xff, 0x2d, 0x00, 0xff}, // V1
		{0x13, 0x50, 0x30, 0x05, 0x60, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x18, 0x0a, 0x84, 0x36, 0x3c, 0xff, 0x2d, 0x00, 0xff}, // V2
		{0x13, 0x50, 0x40, 0x05, 0x70, 0x68, 0x5e, 0x62, 0xbb, 0x96, 0xc0, 0x04, 0x2a, 0x84, 0x36, 0x3c, 0xff, 0x2d, 0x00, 0xff}, // NTSC
		{0x13, 0x51, 0x48, 0x05, 0x70, 0x60, 0x5e, 0x7a, 0x4a, 0x4d, 0xf0, 0x04, 0x2a, 0x84, 0x37, 0x3f, 0xff, 0x2d, 0x00, 0xff}  // PAL
};
tcByte TP2824C_MODE_DataSet[5][20] =
	{
		{0x0c, 0x0d, 0x20, 0x26, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x39, 0x2c, 0x21, 0x22, 0x23, 0x25, 0x27, 0x28, 0xff}, // ADDR
		{0x03, 0x50, 0x30, 0x05, 0x60, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x1c, 0x0a, 0x84, 0x36, 0x3c, 0xff, 0x2d, 0x00, 0xff}, // V1
		{0x13, 0x50, 0x30, 0x05, 0x60, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x18, 0x0a, 0x84, 0x36, 0x3c, 0xff, 0x2d, 0x00, 0xff}, // V2
		{0x13, 0x50, 0x40, 0x05, 0x70, 0x68, 0x5e, 0x62, 0xbb, 0x96, 0xc0, 0x04, 0x2a, 0x84, 0x36, 0x3c, 0xff, 0x2d, 0x00, 0xff}, // NTSC
		{0x13, 0x51, 0x48, 0x05, 0x70, 0x60, 0x5e, 0x7a, 0x4a, 0x4d, 0xf0, 0x04, 0x2a, 0x84, 0x37, 0x3f, 0xff, 0x2d, 0x00, 0xff}  // PAL
};

tcByte TP2826_HDA_DataSet[5][12] =
	{
		{0x0c, 0x20, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x21, 0x25, 0xff}, // ADDR
		{0x03, 0x40, 0x60, 0x48, 0x40, 0x27, 0x72, 0x80, 0x77, 0x46, 0xfe, 0xff}, // 720p30
		{0x03, 0x40, 0x60, 0x48, 0x40, 0x27, 0x88, 0x04, 0x23, 0x46, 0xfe, 0xff}, // 720p25
		{0x03, 0x38, 0x60, 0x44, 0x40, 0x29, 0x65, 0x78, 0x16, 0x46, 0xfe, 0xff}, // 1080p30
		{0x03, 0x3c, 0x60, 0x44, 0x40, 0x29, 0x61, 0x78, 0x16, 0x46, 0xfe, 0xff}, // 1080p25
};
tcByte TP2824C_HDA_DataSet[5 + 1][12 + 1] =
	{
		{0x0c, 0x16, 0x20, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x21, 0x25, 0xff}, // ADDR
		{0x03, 0x16, 0x40, 0x60, 0x48, 0x40, 0x27, 0x72, 0x80, 0x77, 0x46, 0xfe, 0xff}, // 720p30
		{0x03, 0x16, 0x40, 0x60, 0x48, 0x40, 0x27, 0x88, 0x04, 0x23, 0x46, 0xfe, 0xff}, // 720p25
		{0x03, 0xD3, 0x38, 0x60, 0x44, 0x40, 0x29, 0x65, 0x78, 0x16, 0x46, 0xfe, 0xff}, // 1080p30
		{0x03, 0xD3, 0x3c, 0x60, 0x44, 0x40, 0x29, 0x61, 0x78, 0x16, 0x46, 0xfe, 0xff}, // 1080p25
		{0x03, 0x00, 0x3c, 0x60, 0x44, 0x40, 0x13, 0xbe, 0x37, 0x16, 0x46, 0xfe, 0xff}, // 720p60  //ryan@20211207
};
tcByte TP2826_HDC_DataSet[5][20] =
	{
		{0x0c, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1c, 0x1d, 0x20, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x27, 0x28, 0xff}, // ADDR
		{0x03, 0x13, 0x08, 0x00, 0x19, 0xd0, 0x25, 0x06, 0x72, 0x30, 0x60, 0x37, 0x40, 0x48, 0x67, 0x6f, 0x30, 0x5a, 0x04, 0xff}, // 720p30
		{0x03, 0x13, 0x0a, 0x00, 0x19, 0xd0, 0x25, 0x06, 0x7a, 0x3a, 0x60, 0x42, 0x40, 0x48, 0x67, 0x6f, 0x33, 0x5a, 0x04, 0xff}, // 720p25
		{0x03, 0x13, 0x60, 0x80, 0x29, 0x38, 0x47, 0x09, 0x60, 0x3c, 0x60, 0x47, 0x40, 0x41, 0x82, 0x27, 0xa4, 0x5a, 0x04, 0xff}, // 1080p30
		{0x03, 0x13, 0x60, 0x80, 0x29, 0x38, 0x47, 0x09, 0x60, 0x50, 0x60, 0x54, 0x40, 0x41, 0x82, 0x27, 0xa2, 0x5a, 0x04, 0xff}, // 1080p25
};
tcByte TP2824C_HDC_DataSet[5][20] =
	{
		{0x0c, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1c, 0x1d, 0x20, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x27, 0x28, 0xff}, // ADDR
		{0x03, 0x13, 0x08, 0x00, 0x19, 0xd0, 0x25, 0x06, 0x72, 0x30, 0x60, 0x37, 0x40, 0x48, 0x67, 0x6f, 0x30, 0x5a, 0x04, 0xff}, // 720p30
		{0x03, 0x13, 0x0a, 0x00, 0x19, 0xd0, 0x25, 0x06, 0x7a, 0x3a, 0x60, 0x42, 0x40, 0x48, 0x67, 0x6f, 0x33, 0x5a, 0x04, 0xff}, // 720p25
		{0x03, 0x13, 0x60, 0x80, 0x29, 0x38, 0x47, 0x09, 0x60, 0x3c, 0x60, 0x47, 0x40, 0x41, 0x82, 0x27, 0xa4, 0x5a, 0x04, 0xff}, // 1080p30
		{0x03, 0x13, 0x60, 0x80, 0x29, 0x38, 0x47, 0x09, 0x60, 0x50, 0x60, 0x54, 0x40, 0x41, 0x82, 0x27, 0xa2, 0x5a, 0x04, 0xff}, // 1080p25
};
tcByte TP2853C_DataSet[] =
	{
		0x40, 0x04,
		0x07, 0xC0,
		0x0B, 0xC0,
		0x21, 0x84,
		0x38, 0x00,
		0x3A, 0x32,
		0x3B, 0x05,
		0x26, 0x04,
		0x4d, 0x03,
		0x4e, 0x03,
		0x4f, 0x01,
		0xf5, 0xf0,
		0xf1, 0x07, // tp2826c

		0xff, 0xff};
tcByte TP2853C_MODE_DataSet[5][18] =
	{
		{0x0c, 0x0d, 0x20, 0x26, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x39, 0x2c, 0x21, 0x25, 0x27, 0x28, 0xff}, // ADDR
		{0x03, 0x50, 0x30, 0x05, 0x60, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x0c, 0x0a, 0x84, 0xf0, 0x2d, 0x00, 0xff}, // V1
		{0x13, 0x50, 0x30, 0x05, 0x60, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x08, 0x0a, 0x84, 0xf0, 0x2d, 0x00, 0xff}, // V2
		{0x13, 0x50, 0x40, 0x05, 0x70, 0x68, 0x5e, 0x62, 0xbb, 0x96, 0xc0, 0x04, 0x2a, 0x84, 0xf0, 0x2d, 0x00, 0xff}, // NTSC
		{0x13, 0x51, 0x48, 0x05, 0x70, 0x60, 0x5e, 0x7a, 0x4a, 0x4d, 0xf0, 0x04, 0x2a, 0x84, 0xf0, 0x2d, 0x00, 0xff}  // PAL
};
tcByte TP2853C_HDA_DataSet[5][10] =
	{
		{0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x21, 0x25, 0xff}, // ADDR
		{0x70, 0x48, 0x5e, 0x27, 0x72, 0x80, 0x77, 0x44, 0xf0, 0xff}, // 720p30
		{0x70, 0x48, 0x5e, 0x27, 0x88, 0x04, 0x23, 0x44, 0xf0, 0xff}, // 720p25
		{0x58, 0x45, 0x50, 0x29, 0x65, 0x78, 0x16, 0x44, 0xf0, 0xff}, // 1080p30
		{0x58, 0x45, 0x40, 0x29, 0x61, 0x78, 0x16, 0x44, 0xf0, 0xff}, // 1080p25
};
tcByte TP2853C_HDC_DataSet[5][13] =
	{
		{0x15, 0x16, 0x20, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x27, 0x28, 0xff}, // ADDR
		{0x13, 0x40, 0x50, 0x60, 0x42, 0x40, 0x48, 0x67, 0x2e, 0x90, 0x5a, 0x04, 0xff}, // 720p30
		{0x13, 0x40, 0x50, 0x60, 0x42, 0x40, 0x48, 0x67, 0x2e, 0x90, 0x5a, 0x04, 0xff}, // 720p25
		{0x13, 0x20, 0x48, 0x60, 0x45, 0x40, 0x41, 0x83, 0x2e, 0x90, 0x5a, 0x04, 0xff}, // 1080p30
		{0x13, 0x84, 0x48, 0x60, 0x45, 0x40, 0x41, 0x83, 0x2e, 0x90, 0x5a, 0x04, 0xff}, // 1080p25
};

tcByte TP2834_Audio_DataSet[] =
	{
		// audio
		0x40, 0x40, // audio page
		0x00, 0x01, // A1
		0x01, 0x02, // A2
		0x02, 0x03, // A3
		0x03, 0x04, // A4
		0x04, 0x11, // AUX
		0x17, 0x00,
		0x18, 0x10, // 5ch mode
		0x19, 0x0f,
		0x1A, 0x15, // PB to DAC
		0x1b, 0x01, // PB master
		0x37, 0x20,

		0x1d, 0x08,
		0x1d, 0x00,

		0x3c, 0x3f,
		0x3c, 0x00,

		0x3d, 0x01, // ADC reset
		0xff, 0xff};

tcByte TP2834_Audio_DataSetRD[] =
	{
		// audio
		//    0x40, 0x40, //audio page
		0x00, 0x01, // A1
		0x01, 0x02, // A2
		0x02, 0x03, // A3
		0x03, 0x04, // A4
		0x04, 0x11, // AUX
		0x17, 0x00,
		0x18, 0x10, // 5ch mode
		0x19, 0x0f,
		0x1A, 0x15, // PB to DAC
		0x1b, 0x01, // PB master
		0x37, 0x20,

		0x1d, 0x08,
		0x1d, 0x00,

		0x3c, 0x3f,
		0x3c, 0x00,

		0x3d, 0x01, // ADC reset
		0xff, 0xff};

tcByte TP2833_DataSet[] =
	{

		0x40, 0x04,
		0x07, 0xC0,
		0x0B, 0xC0,
		0x22, 0x36,
		0x38, 0x40,
		0x26, 0x04,
		0x4d, 0x03,
		0x4e, 0x03,
		0x4f, 0x01,
		0xff, 0xff};
tcByte TP2833_MODE_DataSet[5][14] =
	{
		{0x0c, 0x0d, 0x20, 0x26, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x39, 0x18, 0xff},
		{0x03, 0x50, 0x30, 0x05, 0x58, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x0c, 0x12, 0xff},
		{0x13, 0x50, 0x30, 0x05, 0x58, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x08, 0x12, 0xff},
		{0x13, 0x50, 0x40, 0x05, 0x70, 0x68, 0x5e, 0x62, 0xbb, 0x96, 0xc0, 0x04, 0x11, 0xff},
		{0x13, 0x51, 0x48, 0x05, 0x70, 0x60, 0x5e, 0x7a, 0x4a, 0x4d, 0xf0, 0x04, 0x11, 0xff}};
tcByte TP2833_HDA_DataSet[5][8] =
	{
		{0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0xff}, // ADDR
		{0x70, 0x48, 0x5e, 0x27, 0x72, 0x80, 0x77, 0xff}, // 720p30
		{0x70, 0x48, 0x5e, 0x27, 0x88, 0x04, 0x23, 0xff}, // 720p25
		{0x58, 0x45, 0x50, 0x29, 0x65, 0x78, 0x16, 0xff}, // 1080p30
		{0x58, 0x45, 0x40, 0x29, 0x61, 0x78, 0x16, 0xff}, // 1080p25
};

tcByte TP2834_DataSet[] =
	{

		// video
		0x40, 0x04,
		0x07, 0xc0,
		0x0b, 0xc0,
		0x3a, 0x70,
		0x26, 0x01,

		0x4d, 0x0f,
		0x4e, 0x0f,

		0xff, 0xff};
tcByte TP2834_MODE_DataSet[5][13] =
	{
		{0x0c, 0x0d, 0x20, 0x26, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x39, 0xff},
		{0x03, 0x10, 0x60, 0x02, 0x58, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x8c, 0xff},
		{0x03, 0x10, 0x60, 0x02, 0x58, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x88, 0xff},
		{0x43, 0x10, 0xa0, 0x12, 0x70, 0x68, 0x5e, 0x62, 0xbb, 0x96, 0xc0, 0x84, 0xff},
		{0x43, 0x11, 0xb0, 0x02, 0x70, 0x60, 0x5e, 0x7a, 0x4a, 0x4d, 0xf0, 0x84, 0xff}};

//////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////

tcByte TP2823_DataSet[] =
	{

		// video
		0x40, 0x04,
		0x0b, 0x60,
		0x22, 0x34,
		0x23, 0x44,
		0x26, 0x01,
		0x38, 0x40,
		0x3f, 0x08,

		0x4d, 0x03,
		0x4e, 0x33,
		0xfa, 0x00,

		0xff, 0xff};
tcByte TP2823_MODE_DataSet[5][13] =
	{
		{0x0c, 0x0d, 0x20, 0x26, 0x2b, 0x2d, 0x2e, 0x30, 0x31, 0x32, 0x33, 0x39, 0xff},
		{0x43, 0x10, 0x60, 0x02, 0x58, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x30, 0xff},
		{0x53, 0x10, 0x60, 0x02, 0x58, 0x30, 0x70, 0x48, 0xbb, 0x2e, 0x90, 0x20, 0xff},
		{0x53, 0x10, 0xa0, 0x12, 0x70, 0x68, 0x5e, 0x62, 0xbb, 0x96, 0xc0, 0x10, 0xff},
		{0x53, 0x11, 0xb0, 0x02, 0x70, 0x60, 0x5e, 0x7a, 0x4a, 0x4d, 0xf0, 0x10, 0xff}};
tcByte TP2822_DataSet[] =
	{

		0x40, 0x04,
		0x39, 0x00,
		0x3d, 0x08,
		0x3a, 0x01,

		0x30, 0x48,
		0x31, 0xbb,
		0x32, 0x2e,
		0x33, 0x90,

		0x4d, 0x03,
		0x4e, 0x33,
		0x3d, 0x00,

		0xff, 0xff};

tcByte TP2802D_DataSet[] =
	{
		0x40, 0x04,
		//		0x07, 0xc0,
		//		0x0b, 0xc0,
		0x2b, 0x44,
		0x23, 0x50,
		0x39, 0x42,
		0x3a, 0x01,

		0x30, 0x48,
		0x31, 0xbb,
		0x32, 0x2e,
		0x33, 0x90,

		0x4d, 0x0f,
		0x4e, 0xff,

		// now TP2801A just support 2 lines, to disable line3&4, else IRQ is in trouble.
		0x40, 0x01,
		0x50, 0x00,
		0x51, 0x00,
		0x52, 0x00,
		0x7F, 0x00,
		0x80, 0x00,
		0x81, 0x00,

		// 0x50~0xb2 need bank switch
		0x40, 0x00,
		// TX total 34bit, valid load 32bit
		0x50, 0x00,
		0x51, 0x0b,
		0x52, 0x0c,
		0x53, 0x19,
		0x54, 0x78,
		0x55, 0x21,
		0x7e, 0x0f, //

		// RX total 40bit, valid load 39bit
		0x7F, 0x00,
		0x80, 0x07,
		0x81, 0x08,
		0x82, 0x04,
		0x83, 0x00,
		0x84, 0x00,
		0x85, 0x60,
		0x86, 0x10,
		0x87, 0x06,
		0x88, 0xBE,
		0x89, 0x39,
		0x8A, 0x27, //
		0xB9, 0x0F, // RX channel enable
		0xff, 0xff};

/*
//total 34bit, valid load 32bit
tcByte TP2822_PTZ_DataSet[] = {

		//PTZ
		0x40, 0x10,
		0xc8, 0x21,
		0xe0, 0x21,
		0x40, 0x00,
		0xc8, 0x21,
		0xe0, 0x21,
		0x7e, 0x0f,
		0xb9, 0x0f,

	0xff, 0xff
};
*/

#if 1
tcByte TP2824C_TVI_FHD_PTZ_DataSet[] = { // 1080P30/25 TVI

	// PTZ COC
	// TX
	0xc0, 0x00,
	0xc2, 0x0B,
	0xc3, 0x0C,
	0xc1, 0x00,
	0xc4, 0x00,
	0xc5, 0x00,
	0xc6, 0x33,
	0xc7, 0x78,
	0xc8, 0x21,
	0x6f, 0x40,
	0x70, 0x00,

	0x55, 0x00,
	0x56, 0x02,
	0x57, 0xB5,
	0x58, 0x01,
	0x59, 0x17,
	0x5a, 0x01,
	0x5b, 0x00,
	0x5c, 0x02,
	0x5d, 0x00,
	0x5e, 0x00,
	0x5f, 0x00,
	0x60, 0xCE,

	// RX
	0xcb, 0x07,
	0xcc, 0x08,
	0xcd, 0x00,
	0xce, 0x00,
	0xcf, 0x04,
	0xd0, 0x00,
	0xd1, 0x00,
	0xd2, 0x60,
	0xd3, 0x10,
	0xd4, 0x06,
	0xd5, 0xbe,
	0xd6, 0x39,
	0xd7, 0x21,
	0xa7, 0x00,
	0xff, 0xff};

tcByte TP2824C_AHD_FHD_PTZ_DataSet[] = { // 1080P30/25 AHD

	// PTZ COC
	// TX
	0xc0, 0x00,
	0xc2, 0x0E,
	0xc3, 0x0F,
	0xc1, 0x00,
	0xc4, 0x10,
	0xc5, 0x11,
	0xc6, 0x24,
	0xc7, 0x15,
	0xc8, 0x97,
	0x6f, 0x44,
	0x70, 0x00,

	0x55, 0x00,
	0x56, 0x00,
	0x57, 0x00,
	0x58, 0x00,
	0x59, 0x00,
	0x5a, 0x00,
	0x5b, 0x00,
	0x5c, 0x00,
	0x5d, 0x00,
	0x5e, 0x00,
	0x5f, 0x00,
	0x60, 0x00,
	0x62, 0x00,
	0x63, 0x00,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	0x68, 0x00,
	0x69, 0x00,
	0x6a, 0x00,
	0x6b, 0x00,
	0x6c, 0x00,

	// RX
	0xcb, 0x07,
	0xcc, 0x08,
	0xcd, 0x00,
	0xce, 0x00,
	0xcf, 0x04,
	0xd0, 0x00,
	0xd1, 0x00,
	0xd2, 0x60,
	0xd3, 0x10,
	0xd4, 0x06,
	0xd5, 0xbe,
	0xd6, 0x39,
	0xd7, 0x27,

	0xa7, 0x24,

	0xff, 0xff};

tcByte TP2824C_CVBS_PTZ_DataSet[] = { // CVBS NTSC/PAL

	// PTZ COC
	// TX
	0xc0, 0x00,
	0xc2, 0x0F,
	0xc3, 0x10,
	0xc1, 0x00,
	0xc4, 0x00,
	0xc5, 0x00,
	0xc6, 0x49,
	0xc7, 0x14,
	0xc8, 0xAF,
	0x6f, 0x60,
	0x70, 0x00,

	0x55, 0x00,
	0x56, 0x00,
	0x57, 0x00,
	0x58, 0x00,
	0x59, 0x00,
	0x5a, 0x00,
	0x5b, 0x00,
	0x5c, 0x00,
	0x5d, 0x00,
	0x5e, 0x00,
	0x5f, 0x00,
	0x60, 0x00,
	0x62, 0x00,
	0x63, 0x00,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	0x68, 0x00,
	0x69, 0x00,
	0x6a, 0x00,
	0x6b, 0x00,
	0x6c, 0x00,

	0xff, 0xff};

tcByte TP2824C_CVIHD_PTZ_DataSet[] = { // CVI CHD

	// PTZ COC
	// TX
	0x70, 0x1D,
	0xc0, 0x00,
	0xc2, 0x10,
	0xc3, 0x00,
	0xc1, 0x00,
	0xc4, 0x00,
	0xc5, 0x00,
	0xc6, 0x15,
	0xc7, 0x5B,
	0xc8, 0xa4,
	0x6f, 0x48,

	0x55, 0x00,
	0x56, 0x00,
	0x57, 0x00,
	0x58, 0x00,
	0x59, 0x00,
	0x5a, 0x00,
	0x5b, 0x00,
	0x5c, 0x00,
	0x5d, 0x00,
	0x5e, 0x00,
	0x5f, 0x00,
	0x60, 0x00,
	0x62, 0x00,
	0x63, 0x00,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	0x68, 0x00,
	0x69, 0x00,
	0x6a, 0x00,
	0x6b, 0x00,
	0x6c, 0x00,

	0xff, 0xff};

#else
tcByte TP2824C_TVI_FHD_PTZ_DataSet[] = { // 1080P30/25 TVI

	// PTZ COC
	// TX
	0xc0, 0x00, // TXLINE2  / TXLINE1
	0xc2, 0x0B, // TXLINE1
	0xc3, 0x0C, // TXLINE2
	0xc1, 0x00, // TXLINE4  / TXLINE3
	0xc4, 0x00, // TXLINE3
	0xc5, 0x00, // TXLINE4
	0xc6, 0x33, // TXBITCKNUM  0x19  0x1F  //0x19 for 1080P  ;  0x33 for 720P   // TVI 720P ptz 20200512
	0xc7, 0x78, // TXHST
	0xc8, 0x21, // TXBITNUM  //0x21  0x27
	0x6f, 0x40, // TXn pin enable
	0x70, 0x00, // TXLMD[3]=0 / TXLNUM[2:0]=0

	// PRESET 1
	// TXLINE1DATA
	0x55, 0x00,
	0x56, 0x02,
	0x57, 0xB5,
	0x58, 0x01,
	0x59, 0x17,
	0x5a, 0x01,
	// TXLINE2DATA
	0x5b, 0x00,
	0x5c, 0x02,
	0x5d, 0x00,
	0x5e, 0x00,
	0x5f, 0x00,
	0x60, 0xCE,

	// RX
	0xcb, 0x07, // RXLINE1
	0xcc, 0x08, // RXLINE2
	0xcd, 0x00, // 0x09,		//RXLINE3
	0xce, 0x00, // 0x0a,		//RXLINE4
	0xcf, 0x04, // RXFREQ
	0xd0, 0x00, // RXFREQ
	0xd1, 0x00, // RXFREQ
	0xd2, 0x60, // RXTHLEVEL
	0xd3, 0x10, // RXHINUM
	0xd4, 0x06, // RXHST / RXHEND
	0xd5, 0xbe, // RXHST
	0xd6, 0x39, // RXHEND
	0xd7, 0x21, // 0x27,		//RXBITNUM
	0xa7, 0x00, //
	0xff, 0xff};

tcByte TP2824C_AHD_FHD_PTZ_DataSet[] = { // 1080P30/25 AHD

	// PTZ COC
	// TX
	0xc0, 0x00, // TXLINE2  / TXLINE1
	0xc2, 0x0E, // TXLINE1
	0xc3, 0x0F, // TXLINE2
	0xc1, 0x00, // TXLINE4  / TXLINE3
	0xc4, 0x10, // TXLINE3
	0xc5, 0x11, // TXLINE4
	0xc6, 0x24, // TXBITCKNUM
	0xc7, 0x15, // TXHST[7:0]		//TXHST=0x215
	0xc8, 0x97, // TXHST[9:8] / TXBITNUM:0x17
	0x6f, 0x44, // TXn pin enable / TXACP
	0x70, 0x00, // TXLMD[3]=0 / TXLNUM[2:0]=0

	// TXLINE1DATA
	0x55, 0x00,
	0x56, 0x00,
	0x57, 0x00,
	0x58, 0x00,
	0x59, 0x00,
	0x5a, 0x00,
	// TXLINE2DATA
	0x5b, 0x00,
	0x5c, 0x00,
	0x5d, 0x00,
	0x5e, 0x00,
	0x5f, 0x00,
	0x60, 0x00,
	// TXLINE3DATA
	0x62, 0x00,
	0x63, 0x00,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	// TXLINE4DATA
	0x68, 0x00,
	0x69, 0x00,
	0x6a, 0x00,
	0x6b, 0x00,
	0x6c, 0x00,

	// RX
	0xcb, 0x07, // RXLINE1
	0xcc, 0x08, // RXLINE2
	0xcd, 0x00, // 0x09,		//RXLINE3
	0xce, 0x00, // 0x0a,		//RXLINE4
	0xcf, 0x04, // RXFREQ
	0xd0, 0x00, // RXFREQ
	0xd1, 0x00, // RXFREQ
	0xd2, 0x60, // RXTHLEVEL
	0xd3, 0x10, // RXHINUM
	0xd4, 0x06, // RXHST / RXHEND
	0xd5, 0xbe, // RXHST
	0xd6, 0x39, // RXHEND
	0xd7, 0x27, // RXBITNUM

	0xa7, 0x24, // RXPWM / RXACP

	0xff, 0xff};

tcByte TP2824C_CVBS_PTZ_DataSet[] = { // CVBS NTSC/PAL

	// PTZ COC
	// TX
	0xc0, 0x00, // TXLINE2  / TXLINE1
	0xc2, 0x0F, // TXLINE1		//line18
	0xc3, 0x10, // TXLINE2		//line19
	0xc1, 0x00, // TXLINE4  / TXLINE3
	0xc4, 0x00, // TXLINE3
	0xc5, 0x00, // TXLINE4
	0xc6, 0x49, // TXBITCKNUM
	0xc7, 0x14, // TXHST[7:0]		//TXHST=0x214
	0xc8, 0xAF, // TXHST[9:8] / TXBITNUM:0x2F
	0x6f, 0x60, // TXn pin enable / TXPELCO2
	0x70, 0x00, // TXLMD[3]=0 / TXLNUM[2:0]=0

	// TXLINE1DATA
	0x55, 0x00,
	0x56, 0x00,
	0x57, 0x00,
	0x58, 0x00,
	0x59, 0x00, // cmd2 [LSB first]
	0x5a, 0x00, // cmd1 [LSB first]
	// TXLINE2DATA
	0x5b, 0x00,
	0x5c, 0x00,
	0x5d, 0x00,
	0x5e, 0x00,
	0x5f, 0x00, // data2 [LSB first]
	0x60, 0x00, // data1 [LSB first]
	// TXLINE3DATA
	0x62, 0x00,
	0x63, 0x00,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	// TXLINE4DATA
	0x68, 0x00,
	0x69, 0x00,
	0x6a, 0x00,
	0x6b, 0x00,
	0x6c, 0x00,
	/*
	 //RX
		0xcb, 0x07,		//RXLINE1
		0xcc, 0x08,		//RXLINE2
		0xcd, 0x00,//0x09,		//RXLINE3
		0xce, 0x00,//0x0a,		//RXLINE4
		0xcf, 0x04,		//RXFREQ
		0xd0, 0x00,		//RXFREQ
		0xd1, 0x00,		//RXFREQ
		0xd2, 0x60,		//RXTHLEVEL
		0xd3, 0x10,		//RXHINUM
		0xd4, 0x06,		//RXHST / RXHEND
		0xd5, 0xbe,		//RXHST
		0xd6, 0x39,		//RXHEND
		0xd7, 0x27,		//RXBITNUM

		0xa7, 0x24,//RXPWM / RXACP
	*/
	0xff, 0xff};

tcByte TP2824C_CVIHD_PTZ_DataSet[] = { // CVI CHD

	// PTZ COC
	// TX
	0x70, 0x1D, // TCMFR[4]=1 / TXLMD[3]=1 / TXLNUM[2:0]=5
	0xc0, 0x00, // TXLINE2  / TXLINE1
	0xc2, 0x10, // TXLINE1		//line18
	0xc3, 0x00, // TXLINE2		//line19
	0xc1, 0x00, // TXLINE4  / TXLINE3
	0xc4, 0x00, // TXLINE3
	0xc5, 0x00, // TXLINE4
	0xc6, 0x15, // TXBITCKNUM
	0xc7, 0x5B, // TXHST[7:0]		//TXHST=0x25B
	0xc8, 0xa4, // 0x97,	//TXHST[9:8]0b10xx xxxx / TXBITNUM:0x17=>0x24
	0x6f, 0x48, // TXn pin enable / TXDAH

	// TXLINE1DATA
	0x55, 0x00,
	0x56, 0x00,
	0x57, 0x00,
	0x58, 0x00,
	0x59, 0x00, // cmd2 [LSB first]
	0x5a, 0x00, // cmd1 [LSB first]
	// TXLINE2DATA
	0x5b, 0x00,
	0x5c, 0x00,
	0x5d, 0x00,
	0x5e, 0x00,
	0x5f, 0x00, // data2 [LSB first]
	0x60, 0x00, // data1 [LSB first]
	// TXLINE3DATA
	0x62, 0x00,
	0x63, 0x00,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	// TXLINE4DATA
	0x68, 0x00,
	0x69, 0x00,
	0x6a, 0x00,
	0x6b, 0x00,
	0x6c, 0x00,
	/*
	 //RX
		0xcb, 0x07,		//RXLINE1
		0xcc, 0x08,		//RXLINE2
		0xcd, 0x00,//0x09,		//RXLINE3
		0xce, 0x00,//0x0a,		//RXLINE4
		0xcf, 0x04,		//RXFREQ
		0xd0, 0x00,		//RXFREQ
		0xd1, 0x00,		//RXFREQ
		0xd2, 0x60,		//RXTHLEVEL
		0xd3, 0x10,		//RXHINUM
		0xd4, 0x06,		//RXHST / RXHEND
		0xd5, 0xbe,		//RXHST
		0xd6, 0x39,		//RXHEND
		0xd7, 0x27,		//RXBITNUM

		0xa7, 0x24,//RXPWM / RXACP
	*/
	0xff, 0xff};
#endif

// chunyu
void I2CDeviceSet1(tcByte *RegSet)
{
	int cnt = 0;
	tByte index, val;

// Output discription
#ifdef TASKING
	while (*RegSet)
		RS_tx(*RegSet++);
	RegSet++;
#endif

	while (*RegSet != 0xFF)
	{

		index = *RegSet;
		val = *(RegSet + 1);
		//		MonWriteI2C(addr, index, val);
		tp28xx_byte_write(index, val);

		//		Puts("\r\n");

		RegSet += 2;

		cnt--;
	}
	delay(50); // 50*5 =250ms
}

void I2CDeviceSet2(tcByte *RegSet, tcByte *DatSet)
{
	int cnt = 0;
	tByte index, val;

	while (*RegSet != 0xFF)
	{

		index = *RegSet;
		val = *DatSet;
		//		MonWriteI2C(addr, index, val);
		tp28xx_byte_write(index, val);

		//		Puts("\r\n");

		RegSet++;
		DatSet++;

		cnt--;
	}
	delay(50); // 50*5 =250ms
}

void SetRegPage(tByte channel)
{
	// tByte tmp;
	// tmp = tp28xx_byte_read(tByte index);
	tp28xx_byte_write(0x40, channel);
	// tp28xx_byte_write(0x40,0x04); //chunyu
	// printf("\r\nChannel-%02x ",(tWord)channel);
}

/*
void Write_AllRegPage(tByte addr, tByte bitpos, tByte bitval){
	tByte ch;
	tByte val;
	tByte mask=0x00;
	tByte maskb;
	tByte Reg40;

	Reg40 = tp28xx_byte_read(TVII2CAddress,0x40);

	mask = 0x01 << bitpos;
	maskb = ~mask;

	for(ch=CH1;ch<=CH4;ch++){
		tp28xx_byte_write(TVII2CAddress,0x40,ch);
		val = tp28xx_byte_read(TVII2CAddress,addr);
		if(bitval)
			tp28xx_byte_write(TVII2CAddress,addr,(val|mask));
		else
			tp28xx_byte_write(TVII2CAddress,addr,(val&maskb));
	}

	tp28xx_byte_write(TVII2CAddress,0x40,Reg40);
}
*/

/*
void Set_16Bits_VDCLK_Out(){

	Write_AllRegPage(0x02,7,0);
	tp28xx_byte_write(TVII2CAddress,0x4b,0x40);
}

void Set_16Bits_VD_Out(tByte channel){
	tByte Reg02,Reg4b;
	tByte VD2SEL;
	tByte VD1SEL;
	tByte dat;

	Reg02 = tp28xx_byte_read(TVII2CAddress,0x02);
	Reg4b = tp28xx_byte_read(TVII2CAddress,0x4b);


	tp28xx_byte_write(TVII2CAddress,0x02,(Reg02&0x7f));

	switch (channel){
		case CH1:
			VD2SEL = VIN1_C;
			VD1SEL = VIN1_Y;
			break;
		case CH2:
			VD2SEL = VIN2_C;
			VD1SEL = VIN2_Y;
			break;
		case CH3:
			VD2SEL = VIN3_C;
			VD1SEL = VIN3_Y;
			break;
		case CH4:
			VD2SEL = VIN4_C;
			VD1SEL = VIN4_Y;
			break;
		}
	dat = (VD2SEL<<4)|VD1SEL;
	tp28xx_byte_write(TVII2CAddress,0x4b,dat);

}
*/

tcByte TP2802_FORMAT_DataSet[][12] =
	{
		{0x02, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1c, 0x1d, 0x35, 0x36, 0xff},			   // reg
		{0x48, 0x03, 0xd3, 0x80, 0x29, 0x38, 0x47, 0x08, 0x98, 0x05, 0xdc, 0xff},			   // 1080p30
		{0x48, 0x03, 0xd3, 0x80, 0x29, 0x38, 0x47, 0x0a, 0x50, 0x05, 0xdc, 0xff},			   // 1080p25
		{0x4a, 0x13, 0x16, 0x00, 0x19, 0xd0, 0x25, 0x06, 0x72, 0x05, 0xdc, 0xff},			   // 720p60 720p30V2
		{0x4a, 0x13, 0x16, 0x00, 0x19, 0xd0, 0x25, 0x07, 0xbc, 0x05, 0xdc, 0xff},			   // 720p50 720p25V2
		{0x4a, 0x13, 0x16, 0x00, 0x19, 0xd0, 0x25, 0x0c, 0xe4, 0x05, 0xdc, 0xff},			   // 720p30    //5
		{0x4a, 0x13, 0x16, 0x00, 0x19, 0xd0, 0x25, 0x0f, 0x78, 0x05, 0xdc, 0xff},			   // 720p25
		{0x4f, 0x13, 0x63 /*0x4e*/, 0xbc, 0x15, 0xf0, 0x07, 0x09, 0x38, 0x25, 0xdc, 0xff},	   // NTSC
		{0x4f, 0x13, 0x73 + 5 /*0x5f*/, 0xbc, 0x17, 0x20, 0x17, 0x09, 0x48, 0x25, 0xdc, 0xff}, // PAL
		{0x48, 0x13, 0x6c, 0x00, 0x2c, 0x00, 0x68, 0x09, 0xc4, 0x16, 0x30, 0xff},			   // 3M18
		{0x48, 0x13, 0x1f, 0x20, 0x34, 0x98, 0x7a, 0x0b, 0x9a, 0x17, 0xd0, 0xff},			   // 5M12      //10
		{0x4a, 0x13, 0x16, 0x00, 0x19, 0xd0, 0x25, 0x06, 0x72, 0x25, 0xdc, 0xff},			   // 720p30V2
		{0x4a, 0x13, 0x16, 0x00, 0x19, 0xd0, 0x25, 0x07, 0xbc, 0x25, 0xdc, 0xff},			   // 720p25V2
		{0x48, 0x13, 0x1f, 0x80, 0x7d, 0xf0, 0x5a, 0x0b, 0xb8, 0x16, 0x72, 0xff},			   // 4M15
		{0x48, 0x13, 0x6c, 0x00, 0x2c, 0x00, 0x68, 0x09, 0xc4, 0x16, 0x72, 0xff},			   // 3M20
		{0x48, 0x23, 0x34, 0x78, 0x8c, 0xf0, 0x5a, 0x0c, 0xe4, 0x17, 0x08, 0xff},			   // 4M12
		{0x48, 0x13, 0xec, 0x80, 0xb0, 0x08, 0x7c, 0x0e, 0xa6, 0x17, 0xbc, 0xff},			   // 6M10
		{0x48, 0x13, 0x0f, 0x00, 0x32, 0xa0, 0x55, 0x06, 0x72, 0x15, 0xdc, 0xff},			   // 4M30
		{0x48, 0x13, 0x20, 0x00, 0x20, 0xa0, 0x55, 0x07, 0xbc, 0x15, 0xdc, 0xff}			   // 4M25

		//{0xc8,0x13,0x6c,0x20,0x2c,0xf0,0x5a,0x0b,0xb8,0x16,0x72,0xff}, //2592x1520p15
};

static void tp28xx_SYSCLK_V2(unsigned char ch)
{
	unsigned char tmp;
	tmp = tp28xx_byte_read(0xf5);
	tmp |= SYS_MODE[ch];
	tp28xx_byte_write(0xf5, tmp);
	tmp = tp28xx_byte_read(0x35);
	tmp |= 0x20;
	tp28xx_byte_write(0x35, tmp);
}
static void tp28xx_SYSCLK_V1(unsigned char ch)
{
	unsigned char tmp;
	tmp = tp28xx_byte_read(0xf5);
	tmp &= SYS_AND[ch];
	tp28xx_byte_write(0xf5, tmp);
	tmp = tp28xx_byte_read(0x35);
	tmp &= 0xdf;
	tp28xx_byte_write(0x35, tmp);
}

void Set_VidRes(tByte ch)
{
	tByte index, sys, tmp;
	sys = 1; // V1
	// printf("Set_VidRes");
	switch (mode[ch])
	{
	case R1080P30:
		index = 1; // 1080p30
		sys = 1;   // V1
		ManVidResRx = RES1920X1080;
		printf("1080p30");
		break;

	case R1080P25:
		index = 2; // 1080p25
		sys = 1;   // V1
		ManVidResRx = RES1920X1080;
		printf("1080p25");
		break;

	case R720P60:
		index = 3; // 720p60 720p30V2
		sys = 1;   // V1
		ManVidResRx = RES1280X720;
		printf("720p60");
		break;

	case R720P30V2:
		index = 11; // 720p30V2
		sys = 2;	// V2
		ManVidResRx = RES1280X720;
		printf("720p30V2");
		break;

	case R720P50:
		index = 4; // 720p50 720p25V2
		sys = 1;   // V1
		ManVidResRx = RES1280X720;
		printf("720p50");
		break;

	case R720P25V2:
		index = 12; // 720p25V2
		sys = 2;	// V2
		ManVidResRx = RES1280X720;
		printf("720p25V2");
		break;

	case R720P30:
		index = 5; // 720p30
		sys = 1;   // V1
		ManVidResRx = RES1280X720;
		printf("720p30");
		break;

	case R720P25:
		index = 6; // 720p25
		sys = 1;   // V1
		ManVidResRx = RES1280X720;
		printf("720p25");
		break;

	case PAL:
		index = 8;
		sys = 4;
		ManVidResRx = RES1920X576;
		printf("PAL");
		break;

	case NTSC:
		index = 7;
		sys = 3;
		ManVidResRx = RES1920X480;
		printf("NTSC");
		break;

	case R3M18:
		index = 9; // 3M18
		sys = 1;   // V1
		ManVidResRx = RES1920X1080;
		printf("3M18");
		break;

	case R5M12:
		index = 10; // 5M12
		sys = 1;	// V1
		ManVidResRx = RES1920X1080;
		printf("5M12");
		break;

	case R4M15:
		index = 13; // 4M15
		sys = 1;	// V1
		ManVidResRx = RES1920X1080;
		printf("4M15");
		break;

	case R3M20:
		index = 14; // 3M20
		sys = 1;	// V1
		ManVidResRx = RES1920X1080;
		printf("3M20");
		break;

	case R4M12:
		index = 15; // 4M12
		sys = 1;	// V1
		ManVidResRx = RES1920X1080;
		printf("4M12");
		break;

	case R6M10:
		index = 16; // 6M10
		sys = 1;	// V1
		ManVidResRx = RES1920X1080;
		printf("6M10");
		break;

	case R4M30:
		index = 17; // 4M30
		sys = 1;	// V1
		ManVidResRx = RES1280X720;
		printf("4M30");
		break;

	default:
		index = 1; // 1080p30
		break;
	}
	printf("...\r\n");

	// I2CDeviceSet(TVII2CAddress, ptr_TP2802_Res );
	I2CDeviceSet2(TP2802_FORMAT_DataSet[0], TP2802_FORMAT_DataSet[index]);
	//	printf("chip= 0x%4x\r\n",chip);
	if (TP2853C == chip)
	{
		I2CDeviceSet2(TP2853C_MODE_DataSet[0], TP2853C_MODE_DataSet[sys]);
	}
	else if (TP2833 == chip)
	{
		I2CDeviceSet2(TP2833_MODE_DataSet[0], TP2833_MODE_DataSet[sys]);
	}
	else if (TP2834 == chip)
	{
		I2CDeviceSet2(TP2834_MODE_DataSet[0], TP2834_MODE_DataSet[sys]);
	}
	if (TP2826 == chip)
	{
		printf("tp2826MODEDATASET\r\n");
		I2CDeviceSet2(TP2826_MODE_DataSet[0], TP2826_MODE_DataSet[sys]);
	}
	if (TP2824C == chip)
	{
		printf("tp2824C MODEDATASET\r\n");
		I2CDeviceSet2(TP2824C_MODE_DataSet[0], TP2824C_MODE_DataSet[sys]);
	}

	if (1 == sys)
		tp28xx_SYSCLK_V1(ch);
	else
		tp28xx_SYSCLK_V2(ch);

	tp28xx_byte_write(0x13, 0x00);
	tmp = tp28xx_byte_read(0x14);
	tmp &= 0x9f;
	tp28xx_byte_write(0x14, tmp);

#if (OUTPUT == Bits16out)
	tmp = tp28xx_byte_read(0x02);
	tmp &= 0x7f;
	tp28xx_byte_write(0x02, tmp);
#else
	tmp = tp28xx_byte_read(0x02);
	tmp |= 0x80;
	tp28xx_byte_write(0x02, tmp);
#endif
}

tByte Set_HDA(tByte ch)
{

	tByte index, tmp;
	index = 0;

	switch (mode[ch])
	{
	case R1080P30:
		index = 3;
		break;

	case R1080P25:
		index = 4;
		break;

	case R720P60: // 20200507
		index = 5;
		break;
	case R720P30V2:
		index = 1;
		break;

	case R720P50: // 20200507
	case R720P25V2:
		index = 2;
		break;
	}
	if (index)
	{
		Set_VidRes(ch);

		if (TP2853C == chip)
		{
			I2CDeviceSet2(TP2853C_HDA_DataSet[0], TP2853C_HDA_DataSet[index]);
		}
		else if (TP2833 == chip || TP2834 == chip)
		{
			I2CDeviceSet2(TP2833_HDA_DataSet[0], TP2833_HDA_DataSet[index]);
		}
		else if (TP2826 == chip)
		{
			I2CDeviceSet2(TP2826_HDA_DataSet[0], TP2826_HDA_DataSet[index]);
		}
		else if (TP2824C == chip)
		{
			I2CDeviceSet2(TP2824C_HDA_DataSet[0], TP2824C_HDA_DataSet[index]);
		}
		tmp = tp28xx_byte_read(0x14);
		tmp |= 0x40;
		tp28xx_byte_write(0x14, tmp);
	}
	return index;
}

tByte Set_HDC(tByte ch)
{

	tByte index;

	index = 0;

	switch (mode[ch])
	{

	case R1080P30:
		index = 3;
		break;

	case R1080P25:
		index = 4;
		break;

	case R720P60: // 20200507
	case R720P30V2:
		index = 1;
		break;

	case R720P50: // 20200507
	case R720P25V2:
		index = 2;
		break;
	}
	if (index)
	{
		Set_VidRes(ch);

		if (TP2853C == chip)
		{
			I2CDeviceSet2(TP2853C_HDC_DataSet[0], TP2853C_HDC_DataSet[index]);
		}
		else if (TP2833 == chip || TP2834 == chip)
		{
			I2CDeviceSet2(TP2853C_HDC_DataSet[0], TP2853C_HDC_DataSet[index]);
		}
		else if (TP2826 == chip)
		{
			I2CDeviceSet2(TP2826_HDC_DataSet[0], TP2826_HDC_DataSet[index]);
		}
		else if (TP2824C == chip)
		{
			I2CDeviceSet2(TP2824C_HDC_DataSet[0], TP2824C_HDC_DataSet[index]);
		}
		tp28xx_byte_write(0x13, 0x40);
	}
	return index;
}

tByte Det_VidRes()
{
	tByte CVSTD;
	tByte RES;

	CVSTD = tp28xx_byte_read(0x03) & 0x07;

	switch (CVSTD)
	{

	case 0x00:
		RES = R720P60;
		break;
	case 0x01:
		RES = R720P50;
		break;
	case 0x02:
		RES = R1080P30;
		break;
	case 0x03:
		RES = R1080P25;
		break;
	case 0x04:
		RES = R720P30;
		break;
	case 0x05:
		RES = R720P25;
		break;
	case 0x06:
		printf("\r\nN/P detected");
		RES = NTPAL;
		break;
	default:
		printf("\r\nNo res detect...");
		RES = NoDet;
		break;
	}

	return RES;
}
tByte Det_VideoLoss(tByte ch)
{

	tByte VDLOSS;

	tp28xx_byte_write(0x40, ch & 0x03);
	VDLOSS = tp28xx_byte_read(0x01);
	//	printf("video status 0x%02x on ch%01x\r\n",(tWord)VDLOSS,(tWord)ch);

	if (VDLOSS)
		printf("No ");

	printf("video detected");

	VDLOSS = VDLOSS >> 7;
	return VDLOSS;
}

void ManualAGC(unsigned char ch)
{
	unsigned int agc;
	unsigned char tmp;
	// unsigned int i;

	tp28xx_byte_write(0x2F, 0x02);
	agc = tp28xx_byte_read(0x04);
	agc += tp28xx_byte_read(0x04);
	agc += tp28xx_byte_read(0x04);
	agc += tp28xx_byte_read(0x04);
	agc &= 0x3f0;
	agc >>= 1;
	if (agc > 0x1ff)
		agc = 0x1ff;

	printf("result AGC=0X%04x ch%02X\r\n", (tWord)agc, (tWord)ch);
	tp28xx_byte_write(0x08, agc & 0xff);
	tmp = tp28xx_byte_read(0x06);
	tmp &= 0xf9;
	tmp |= (agc >> 7) & 0x02;
	tmp |= 0x04;
	tmp &= 0x32; /// ryan@20211028 AHD 720P �G�װ��D���ϥ�AGCgain
	tp28xx_byte_write(0x06, tmp);

	// test Dump Reg
	/*
		printf("DumpReg@strat\r\n") ;
		tp28xx_byte_write(0x40,01); //
		for (i = 2 ; i< 255 ;i++)
		{
			tmp = tp28xx_byte_read((tByte)i);
			printf("88,%02X,%02X\r\n", i , (int)tmp ) ;
		}
		printf("DumpReg@end\r\n") ;
	*/
}

/*
static void tp2802D_reset_default(unsigned char ch)
{
	unsigned char tmp;
	tp28xx_byte_write(0x3A, 0x01);
	tp28xx_byte_write(0x0B, 0xC0);
	tp28xx_byte_write(0x07, 0xC0);
	tp28xx_byte_write(0x2e, 0x70);
	tp28xx_byte_write(0x39, 0x42);
	tp28xx_byte_write(0x09, 0x24);
	tmp = tp28xx_byte_read(0x06);   // soft reset and auto agc when cable is unplug
	tmp &= 0x7b;
	tp28xx_byte_write(0x06, tmp);

	tmp = tp28xx_byte_read(0xf5);
	if( tmp & SYS_MODE[ch])
	{
		tmp &= SYS_AND[ch];
		tp28xx_byte_write(0xf5, tmp);
	}
}
*/
/*  //chunyu
static void tp282x_reset_AGC_CLK(unsigned char ch)
{
	unsigned char tmp;

	tmp = tp28xx_byte_read(0x06);
	tmp &= 0xfb;
	tp28xx_byte_write(0x06, tmp);

	tmp = tp28xx_byte_read(0xf5);
	tmp &= SYS_AND[ch];
	tp28xx_byte_write(0xf5, tmp);

	tp28xx_byte_write(0x35, 0x05);
	tp28xx_byte_write(0x26, 0x01);
}
*/
static void tp2824_reset_default(void)
{
	// unsigned char tmp;
	// tp28xx_byte_write(addr, 0x07, 0x40);
	// tp28xx_byte_write(addr, 0x0b, 0x40);
	// tp28xx_byte_write(addr, 0x3A, 0x88);
	tp28xx_byte_write(0x26, 0x01);
}
static void tp2853C_reset_default(void)
{
	unsigned char tmp;
	tp28xx_byte_write(0x07, 0xC0);
	tmp = tp28xx_byte_read(0x14);
	tmp &= 0x9f;
	tp28xx_byte_write(0x14, tmp);
	tp28xx_byte_write(0x13, 0x00);
	tp28xx_byte_write(0x26, 0x04);
	tp28xx_byte_write(0x06, 0xb2);
}
static void TP28xx_reset_default(int chip, unsigned char ch)
{
	/*    if(TP2823 == chip )
		{
			   tp282x_reset_AGC_CLK(ch);
		}
		else if(TP2822 == chip )
		{
			   tp282x_reset_AGC_CLK(ch);
			   tp28xx_byte_write(0x07, 0x40);
		}
		else if(TP2802D == chip )
		{
			   tp2802D_reset_default( ch);
		}
	*/
	if (TP2834 == chip)
	{
		tp2824_reset_default();
	}
	else if (TP2833 == chip)
	{
		tp2853C_reset_default();
	}
	else if (TP2853C == chip)
	{
		tp2853C_reset_default();
	}
	else // chunyu
	{
		tp2824_reset_default(); // chunyu
	}
}
////////////////////////////////////////
tByte ReadGain(void)
{
	tp28xx_byte_write(0x2f, 0x00);
	return tp28xx_byte_read(0x04);
}
////////////////////////////////////////
void SetCGain(unsigned char CGAIN_STD)
{
	unsigned char tmp, cgain;
	unsigned char retry = 30;

	tp28xx_byte_write(0x2f, 0x06);
	cgain = tp28xx_byte_read(0x04);

	if (cgain < CGAIN_STD)
	{
		while (retry)
		{
			retry--;

			tmp = tp28xx_byte_read(0x07);
			tmp &= 0x3f;
			while (abs(CGAIN_STD - cgain))
			{
				if (tmp)
					tmp--;
				else
					break;
				cgain++;
			}

			tp28xx_byte_write(0x07, 0x80 | tmp);
			if (0 == tmp)
				break;
			delay(8); // delay 40ms
			tp28xx_byte_write(0x2f, 0x06);
			cgain = tp28xx_byte_read(0x04);

			if (cgain > (CGAIN_STD + 1))
			{
				tmp = tp28xx_byte_read(0x07);
				tmp &= 0x3f;
				// tmp +=0x02;
				tmp++;
				tmp++;
				if (tmp > 0x3f)
					tmp = 0x3f;
				tp28xx_byte_write(0x07, 0x80 | tmp);
				if (0x3f == tmp)
					break;
				delay(8); // delay 40ms
				cgain = tp28xx_byte_read(0x04);
			}
			if (abs(cgain - CGAIN_STD) < 2)
				break;
		}

#if (_DEBUG_MESSAGE_TP2824 == ON)
		Printf("SetCGain");
#endif
	}
}
////////////////////////////////////////

BYTE INPUT_VIDEO_CHECK(BYTE ch)
{

	BYTE ret = 0;

	switch (CURRENT_CH)
	{
	case VS4210_LOOP_MODE0_w0:
		if ((ch == CH2) || (ch == CH3) || (ch == CH4))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE0_w1:
		if ((ch == CH1) || (ch == CH3) || (ch == CH4))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE0_w2:
		if ((ch == CH1) || (ch == CH2) || (ch == CH4))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE0_w3:
	case VS4210_LOOP_MODE0_w4:
		if ((ch == CH1) || (ch == CH2) || (ch == CH3))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE12_12:
	case VS4210_LOOP_MODE12_21:
		if ((ch == CH3) || (ch == CH4))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE12_14:
		if ((ch == CH2) || (ch == CH3))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE10_123:
	case VS4210_LOOP_MODE8:
	case VS4210_LOOP_MODE8_123:
	case VS4210_LOOP_MODE8_213:
	case VS4210_LOOP_MODE8_312:
	case VS4210_LOOP_MODE9:
		if (ch == CH4)
			ret = _TRUE;
		break;

	case VS4210_LOOP_MODE12_23:
	case VS4210_LOOP_MODE12_32:
		if ((ch == CH1) || (ch == CH4))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE12_34:
	case VS4210_LOOP_MODE12_43:
		if ((ch == CH1) || (ch == CH2))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE12_31:
	case VS4210_LOOP_MODE12_13:
		if ((ch == CH2) || (ch == CH4))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE1:
	case VS4210_LOOP_MODE2:
		ret = _FALSE;
		break;
	case VS4210_LOOP_MODE12_42:
	case VS4210_LOOP_MODE12_24:
		if ((ch == CH1) || (ch == CH3))
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE8_412:
	case VS4210_LOOP_MODE8_124:
	case VS4210_LOOP_MODE8_214:
	case VS4210_LOOP_MODE10_412:
	case VS4210_LOOP_MODE9_412:
		if (ch == CH3)
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE8_134:
	case VS4210_LOOP_MODE8_314:
	case VS4210_LOOP_MODE10_413:
	case VS4210_LOOP_MODE8_413:
		if (ch == CH2)
			ret = _TRUE;
		break;
	case VS4210_LOOP_MODE8_234:
	case VS4210_LOOP_MODE8_324:
	case VS4210_LOOP_MODE10_423:
	case VS4210_LOOP_MODE8_423:
		if (ch == CH1)
			ret = _TRUE;
		break;
	default:
		ret = 0;
		break;
	}

	return ret;
}
void SetTP2824CHVloss(BYTE index)
{
	state[index] = VIDEO_UNPLUG;
	count[index] = 0;
	TP28xx_reset_default(chip, index);
	Printf("\r\n(SetTP2824CHVloss ch%02x)", (tWord)index);
	VS4210_Rx_NoSignal(index); // chunyu
}
void SETChannelReset(BYTE index)
{
	Printf("\r\n(SETChannelReset=%02x)", (WORD)index);

	SetRegPage(index);
	tp2824_reset_default(); // chunyu
	Set_VidRes(index);
	// VS4210_Rx_NoSignal(index) ; //chunyu
	gbVXIS_InputMode[index] = _OSD_No_Signal;
	bSignal[index] = 0;
	mode[index] = 0x07;
	state[index] = 0;
	count[index] = 0;
	VideoSave[index] = 0;
}
void EQ_Detect()
{
	// tByte tmpval;
	tByte cvstd, status, i;
	tByte gain, tmp, fmt, tmpab;

	for (i = 0; i < CH_PER_CHIP; i++) // scan four inputs:
	{
		//	if((INPUT_VIDEO_CHECK(i))&&(BOOT_ISSUE==ON))
		//      continue;

		SetRegPage(i);
		status = tp28xx_byte_read(0x01);

		// state machine for video checking
		if (status & FLAG_LOSS)
		{

			if (VIDEO_UNPLUG == state[i])
			{
				if (count[i] < MAX_COUNT)
					count[i]++;
				continue;
			}
			else
			{

				state[i] = VIDEO_UNPLUG;
				count[i] = 0;

				VideoSave[i] = 0;

#if 0
	if(i==2)
		{
		if(bytSelectCAMRD==SelectCameraR)	
		WriteEEP(EEP_TP2824_STD3,0);
		else
		WriteEEP(EEP_TP2824_STD4,0);
		}
	else
		{
		WriteEEP(EEP_TP2824_STD3,0);
		}

#else
				if (i == 3)
				{
					if (bytSelectCAMRD == SelectCameraR)
					{
						WriteEEP(EEP_TP2824_STD0 + (i * 3) + 2, 0xFF);
						Printf("\r\n(#video loss ch%02x)", (WORD)i);
					}
					else
					{
						WriteEEP(EEP_TP2824_STD0 + ((i + 1) * 3) + 2, 0xFF);
						Printf("\r\n(#video loss ch%02x)", (WORD)i);
					}
				}
				else
				{
					WriteEEP(EEP_TP2824_STD0 + (i * 3) + 2, 0xFF);
					Printf("\r\n(#video loss ch%02x)", (WORD)i);
				}

#endif
				TP28xx_reset_default(chip, i);
				printf("video loss ch%02x\r\n", (tWord)i);

				mode[i] = NoDet;
				Set_VidRes(i);
				VS4210_Rx_NoSignal(i); // chunyu
				QuadBoxId &= ~(1 << i);

				Flisheye_CAM[i] = FALSE;
				Flisheye_VLOSS_Count[i] = 0;
				WriteEEP((EEP_FlishCAM1 + i), FALSE);

				if (GetDisplayedOSD() & (MENU | TVVOL | _COC_OSD | _VERSION_OSD))
					CloseOSDMenu();

				ShowJUMPOSD();
			}
		}
		else
		{
			if (PAL == mode[i] || NTSC == mode[i])
				flag_LOCKED = CVBS_FLAG_LOCKED;
			else
				flag_LOCKED = TVI_FLAG_LOCKED;

			if (flag_LOCKED == (status & flag_LOCKED)) // video locked
			{
				if (VIDEO_LOCKED == state[i])
				{
					if (count[i] < MAX_COUNT)
					{
						count[i]++;
						// if (i==0)
						// Printf("(count[%02x]=%02x)",(tWord)i, (tWord)count[i]);
					}
#if 0
				if(VideoSave[i]==_TRUE)
				{
			 		if((count[i]==1)&&((mode[i] == NTSC) ||( mode[i] == PAL)))
			 		count[i]=20;//ryan@20200903
			 		else if((count[i]==1)/*&&(ReadEEP(EEP_TP2824_STD0+(i*3))==HDA)*/)
			 			{
			 			std[i]=ReadEEP(EEP_TP2824_STD0+(i*3));
						
		 			 		if(std[i]==HDC)
							{
							Set_HDC(i);
							printf("(Set_HDC)");
							}
							 if(std[i]==HDA)
						 	{
						 	Set_HDA(i);
							printf("(Set_HDA)");
						 	}
							 
						count[i]=20;
			 			}
					
				}
#endif
				}
				else if (VIDEO_UNPLUG == state[i])
				{
					state[i] = VIDEO_IN;
					count[i] = 0;
					printf("1video in ch%02x \r\n", (tWord)i);
				}
				else
				{
					state[i] = VIDEO_LOCKED;
					count[i] = 0;
					printf("video locked ch%02x \r\n", (tWord)i);
					Flisheye_VLOSS_Count[i] = 0;
				}
			}
			else // video in but unlocked
			{

				if (VIDEO_UNPLUG == state[i])
				{
					state[i] = VIDEO_IN;
					count[i] = 0;
					printf("video in ch%02x \r\n", (tWord)i);
				}
				else if (VIDEO_LOCKED == state[i])
				{

					if ((Flisheye_CAM[i] == TRUE) && (Flisheye_VLOSS_Count[i] < 100))
					{
						Flisheye_VLOSS_Count[i]++;
					}
					else if ((PAL == mode[i]) && (Flisheye_VLOSS_Count[i] < 2)) ///�ץ�CAM R PAL�q���Ұʤ�í�w
					{
						Flisheye_VLOSS_Count[i]++;
					}
					else
					{
						VS4210_Rx_NoSignal(i); // chunyu
						state[i] = VIDEO_UNLOCK;
						count[i] = 0;
						Printf("video unstable ch%02x \r\n", (tWord)i);

						if (i == 3)
						{
							if (bytSelectCAMRD == SelectCameraR)
							{
								WriteEEP(EEP_TP2824_STD0 + (i * 3) + 2, 0xFF);
								WriteEEP(EEP_TP2824_SAVE3, 0xFF);
								VideoSave[i] = 0;
								Printf("\r\n(#video loss ch%02x)", (WORD)i);
							}
							else
							{
								WriteEEP(EEP_TP2824_STD0 + ((i + 1) * 3) + 2, 0xFF);
								WriteEEP(EEP_TP2824_SAVE4, 0xFF);
								VideoSave[i] = 0;
								Printf("\r\n(#video loss ch%02x)", (WORD)i);
							}
						}
						else
						{
							WriteEEP(EEP_TP2824_STD0 + (i * 3) + 2, 0xFF);
							VideoSave[i] = 0;
							Printf("\r\n(#video loss ch%02x)", (WORD)i);
						}
					}
				}
				else
				{
					if (count[i] < MAX_COUNT)
						count[i]++;
					if (VIDEO_UNLOCK == state[i] && count[i] > 2)
					{
						state[i] = VIDEO_IN;
						count[i] = 0;
						TP28xx_reset_default(chip, i);
						printf("video unlocked ch%02x \r\n", (tWord)i);
						VS4210_Rx_NoSignal(i); // chunyu
					}
				}
			}
		}

		if (VIDEO_IN == state[i])
		{
			cvstd = tp28xx_byte_read(0x03);
			sywd = cvstd & 0x08;
			cvstd &= 0x07;
			fmt = mode[i];
			if (NTPAL == cvstd && flag_CVBS)
			{
				// if(count[i] & 0x01)
				if (status & 0x04)
					fmt = PAL;
				else
					fmt = NTSC;
			}
			else if (cvstd == R720P30)
			{
				if (flag_MEGA && (count[i] & 0x02)) // 3M auto detect test
					fmt = R4M12;
				else
					fmt = R720P30V2;
			}
			else if (cvstd == R720P25)
			{
				fmt = R720P25V2;
			}
			else if (cvstd == R1080P30)
			{
				if (flag_MEGA && (count[i] & 0x02)) // TP2853C 3M auto detect test
					fmt = R3M20;
				else
					fmt = R1080P30;
			}
			else if (cvstd == R720P60)
			{
				if (flag_MEGA && (count[i] & 0x02)) // TP2853C 3M auto detect test
					fmt = R4M30;
				else
					fmt = R720P60;
			}
			else if (cvstd < 4)
			{
				fmt = cvstd;
			}
			else
			{
				if (flag_MEGA) // TP2853C 3M auto detect test
				{
					tp28xx_byte_write(0x2f, 0x09);
					tmp = tp28xx_byte_read(0x04);
					if ((tmp > 0x48) && (tmp < 0x55))
					{
						fmt = R3M18;
					}
					else if ((tmp > 0x55) && (tmp < 0x62))
					{
						fmt = R4M15;
					}
					else if ((tmp > 0x70) && (tmp < 0x80))
					{
						fmt = R6M10;
					}
				}
			}
			/*
					if(VideoSave[i]==_TRUE)
					{
					printf("(B)");
					Set_VidRes(i);
					}
					else
						*/
			{
				if (mode[i] != fmt)
				{
					mode[i] = fmt;
					printf("Set_VidResmode[%d] =%d\r\n", (int)i, (int)fmt);
					Set_VidRes(i);
				}
			}
		}

		if (VIDEO_LOCKED == state[i]) // check signal lock
		{
			if (0 == count[i])
			{
				std[i] = TVI;
				CurCH = i;
				if ((R720P30V2 == mode[i]) || (R720P25V2 == mode[i]))
				{
					if (0x00 == (0x08 & tp28xx_byte_read(0x03)))
					{
						mode[i] &= 0xf7;
						Set_VidRes(i);
						// printf("TVI_Set_VidRes\r\n");
					}
				}

				// tp28xx_byte_write(0xf6  , REG_F6[i]);
				tp28xx_byte_write(0xf6 + i, REG_F6[i]); // chunyu

#if (OUTPUT == Bits16out)
				if (TP2826 == chip)
				{
					tp28xx_byte_write(0xf8, REG_F7[i]);
					tp28xx_byte_write(0xfa, REG_FA[i]);
					tp28xx_byte_write(0xfb, REG_FA[i]);
				}
				else
				{
					tp28xx_byte_write(0xf7, REG_F7[i]);
					tp28xx_byte_write(0xfa, REG_FA[i]);
					// printf("set output ch%02x \r\n", (tWord)CH[i]);
				}
#else // BT656

				tmp = tp28xx_byte_read(0xf5);
				if (tmp &= SYS_MODE[i])
				{
					printf("//75.25M output\r\n");

					if (i == 0)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0xF0) | 0x0A;
						tp28xx_byte_write(0xfa, tmpab);
					}
					else if (i == 1)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0x0F) | 0xA0;
						tp28xx_byte_write(0xfa, tmpab);
					}

					if (i == 2)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0xF0) | 0x0A;
						tp28xx_byte_write(0xfb, tmpab);
					}
					else if (i == 3)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0x0F) | 0xA0;
						tp28xx_byte_write(0xfb, tmpab);
					}
				}
				else
				{
					extern tByte half_scaler;
					if (half_scaler) // down scaler output
					{
						tmp = tp28xx_byte_read(0x35);
						tmp |= 0x40;
						tp28xx_byte_write(0x35, tmp);

						if (i == 0)
						{
							tmpab = tp28xx_byte_read(0xfa);
							tmpab = (tmpab & 0xF0) | 0x0A;
							tp28xx_byte_write(0xfa, tmpab);
						}
						else if (i == 1)
						{
							tmpab = tp28xx_byte_read(0xfa);
							tmpab = (tmpab & 0x0F) | 0xA0;
							tp28xx_byte_write(0xfa, tmpab);
						}

						if (i == 2)
						{
							tmpab = tp28xx_byte_read(0xfb);
							tmpab = (tmpab & 0xF0) | 0x0A;
							tp28xx_byte_write(0xfb, tmpab);
						}
						else if (i == 3)
						{
							tmpab = tp28xx_byte_read(0xfb);
							tmpab = (tmpab & 0x0F) | 0xA0;
							tp28xx_byte_write(0xfb, tmpab);
						}
					}
					else
					{
						tmp = tp28xx_byte_read(0x35);
						tmp &= 0xbf;
						tp28xx_byte_write(0x35, tmp);

						if (i == 0)
						{
							tmpab = tp28xx_byte_read(0xfa);
							tmpab = (tmpab & 0xF0) | 0x08;
							tp28xx_byte_write(0xfa, tmpab);
						}
						else if (i == 1)
						{
							tmpab = tp28xx_byte_read(0xfa);
							tmpab = (tmpab & 0x0F) | 0x80;
							tp28xx_byte_write(0xfa, tmpab);
						}

						if (i == 2)
						{
							tmpab = tp28xx_byte_read(0xfb);
							tmpab = (tmpab & 0xF0) | 0x08;
							tp28xx_byte_write(0xfb, tmpab);
						}
						else if (i == 3)
						{
							tmpab = tp28xx_byte_read(0xfb);
							tmpab = (tmpab & 0x0F) | 0x80;
							tp28xx_byte_write(0xfb, tmpab);
						}
					}
				}
#endif
				if (CurRes != ManVidResRx)
				{
					CurRes = ManVidResRx;
					// CreateMDIN340VideoInstance();  //chunyu
					printf("ManVidResRx=0x%02x\r\n", ManVidResRx);
				}
			}
			else if (1 == count[i])
			{
				if (chip > TP2823)
				{
					tmp = tp28xx_byte_read(0x01);
					printf("Reg01 %02x\r\n", (tWord)tmp);
					if (mode[i] == NTSC || mode[i] == PAL)
						printf("CVBS  mode:%d\r\n", (tWord)mode[i]);
					//                 if( 0x60 == (0x64 & tmp))
					else if ((0x60 == (0x64 & tmp)) &&
							 (mode[i] != R720P30 && mode[i] != R720P25 && mode[i] != R720P30V2 && mode[i] != R720P25V2 && mode[i] != R720P60 && mode[i] != R720P50))
					{
						// if(Set_HDA(mode[i])) printf("turn to A ch%02x\r\n", (tWord)i);
						if (Set_HDC(i))
						{
							std[i] = HDC;
							printf("turn to C ch%02x\r\n", (tWord)i);

							Set_HDA(i);	  // ryan@20211125 disable CVI function
							std[i] = HDA; // ryan@20211125
							printf("(turn to AHD ch%02x)\r\n", (tWord)i);
						}
					}
				}
			}
			else if (2 == count[i])
			{
				if (chip > TP2823)
				{
					tmp = tp28xx_byte_read(0x01);
					printf("Reg01 %02x\r\n", (tWord)tmp);
					if (mode[i] == NTSC || mode[i] == PAL)
						printf("CVBS  mode:%d\r\n", (tWord)mode[i]);
					//                 if( 0x10 != (0x11 & tmp) && HDC == std[i])
					else if ((0x10 != (0x11 & tmp) && HDC == std[i]) ||
							 (0x00 == (0x04 & tmp) &&
							  (mode[i] == R720P30 || mode[i] == R720P25 || mode[i] == R720P30V2 || mode[i] == R720P25V2 || mode[i] == R720P60 || mode[i] == R720P50)))
					{
						if (Set_HDA(i))
						{
							std[i] = HDA;
							printf("turn to A ch%02x\r\n", (tWord)i);
						}
					}
				}
			}
			else if (3 == count[i])
			{
				if (chip > TP2823)
				{
					tmp = tp28xx_byte_read(0x01);
					printf("Reg01 %02x\r\n", (tWord)tmp);

					if ((mode[i] == R1080P30 || mode[i] == R1080P25) && 0x00 == (0x04 & tmp))
						printf("AHD 1080P30/P25  mode:%d\r\n", (tWord)mode[i]);
					else if ((mode[i] == R1080P30 || mode[i] == R1080P25) && 0x04 == (0x04 & tmp) && std[i] == TVI)
						printf("TVI 1080P30/P25   mode:%d\r\n", (tWord)mode[i]);
					else if ((mode[i] == R720P30 || mode[i] == R720P25 || mode[i] == R720P30V2 || mode[i] == R720P25V2 || mode[i] == R720P60 || mode[i] == R720P50) &&
							 0x00 == (0x04 & tmp) && std[i] == HDA)
						printf("AHD 720P30/P25/P60/P50   mode:%d\r\n", (tWord)mode[i]);
					else if (mode[i] == NTSC || mode[i] == PAL)
						printf("CVBS    mode:%d\r\n", (tWord)mode[i]);
					//		      if( 0x00 != (0x01 & tmp))
					else if (0x00 != (0x01 & tmp))
					{
						// Set_VidRes(mode[i]); //chunyu
						// Set_VidRes(i); //is it right? ryan@20200904
						printf("back to T ch%02x\r\n", (tWord)i); // chunyu
						state[i] = VIDEO_UNPLUG;
						count[i] = 0;
						VideoSave[i] = 0;
						mode[i] = NoDet;
						Set_VidRes(i);
						VS4210_Rx_NoSignal(i); // chunyu
					}
				}
			}
			else if (count[i] < 7) // skip first 3 second
			{
				egain[i][3] = egain[i][2];
				egain[i][2] = egain[i][1];
				egain[i][1] = egain[i][0];

				gain = ReadGain();
				egain[i][0] = gain;
			}
			else if (count[i] < EQ_COUNT)
			{
				egain[i][3] = egain[i][2];
				egain[i][2] = egain[i][1];
				egain[i][1] = egain[i][0];

				gain = ReadGain();
				egain[i][0] = gain;

				if (abs(egain[i][3] - egain[i][0]) < EQ_DIFF && abs(egain[i][2] - egain[i][0]) < EQ_DIFF && abs(egain[i][1] - egain[i][0]) < EQ_DIFF)
				{
					count[i] = EQ_COUNT - 1;
				}
			}
			else if (EQ_COUNT == count[i])
			{
				gain = ReadGain();
				printf("Egain=0x%02x ch%02x,std=%d,mode=0x%02X\r\n", (tWord)gain, (tWord)i, (tWord)std[i], (tWord)mode[i]); // chunyu

				if ((mode[i] == R1080P30) || (mode[i] == R1080P25))
				{
					newInRes[i] = _OSD_1920X1080P;
				}
				else if ((mode[i] == R720P50) || (mode[i] == R720P60))
				{
					newInRes[i] = _OSD_1280X720P;
				}
				else if ((mode[i] == R720P25) || (mode[i] == R720P30))
				{
					newInRes[i] = _OSD_1280X720P;
				}
				else if ((mode[i] == R720P30V2) || (mode[i] == R720P25V2))
				{
					newInRes[i] = _OSD_1280X720P;
				}
				else if (mode[i] == NTSC)
				{
					newInRes[i] = _OSD_720X480I60;
				}
				else if (mode[i] == PAL)
				{
					newInRes[i] = _OSD_720X576I50;
				}
				else
				{
					newInRes[i] = _OSD_No_Signal;
				}

				if (newInRes[i] != gbVXIS_InputMode[i])
				{

					if (bytResetOSDValue == 0)
						VideoChangeFlag = 1;

					VS4210_SetInModeOutMode(i, newInRes[i], gbVXIS_OuputModeflg);

// save tp2824 data
#if 0
		if((bytSelectCAMRD==SelectCameraD)&&(i==2))	
			{

			if( ReadEEP(EEP_TP2824_SAVE4)==0)
			{
			 WriteEEP(EEP_TP2824_STD4,std[i]);
			 WriteEEP(EEP_TP2824_MODE4,mode[i]); 	
			  WriteEEP(EEP_TP2824_SAVE4,1);
			  VideoSave[i]=_TRUE;

#if (_DEBUG_MESSAGE_TP2824 == ON)
			 Printf("\r\n(WriteModeDataToEEPROM ch=%02x)",(WORD)i+1) ;
#endif
			}

			}
		else
			{
				if( ReadEEP(EEP_TP2824_SAVE2)==0)
				{
				 WriteEEP(EEP_TP2824_STD2,std[i]);
				 WriteEEP(EEP_TP2824_MODE2,mode[i]);		
			    	  WriteEEP(EEP_TP2824_SAVE2,1);
				  VideoSave[i]=_TRUE;

#if (_DEBUG_MESSAGE_TP2824 == ON)
				 Printf("\r\n(WriteModeDataToEEPROM ch=%02x)",(WORD)i) ;
#endif
				}
			}

#else
					if ((bytSelectCAMRD == SelectCameraD) && (i == 3))
					{

						if (/*ReadEEP(EEP_TP2824_STD0+((i+1)*3)+2)*/ VideoSave[i] != _TRUE)
						{
							WriteEEP(EEP_TP2824_STD0 + ((i + 1) * 3), std[i]);
							WriteEEP(EEP_TP2824_STD0 + ((i + 1) * 3) + 1, mode[i]);
							WriteEEP(EEP_TP2824_STD0 + ((i + 1) * 3) + 2, 1);
							VideoSave[i] = _TRUE;

#if (_DEBUG_MESSAGE_TP2824 == ON)
							GraphicsPrint(RED, "(WriteModeDataToEEPROM ch=%02x std=%02x mode=%02x)\r\n", (WORD)i + 1, (WORD)std[i], (WORD)mode[i]);
#endif
						}
					}
					else
					{
						if (/*ReadEEP(EEP_TP2824_STD0+(i*3)+2)*/ VideoSave[i] != _TRUE)
						{
							WriteEEP(EEP_TP2824_STD0 + (i * 3), std[i]);
							WriteEEP(EEP_TP2824_STD0 + (i * 3) + 1, mode[i]);
							WriteEEP(EEP_TP2824_STD0 + (i * 3) + 2, 1);
							VideoSave[i] = _TRUE;

#if (_DEBUG_MESSAGE_TP2824 == ON)
							GraphicsPrint(RED, "(WriteModeDataToEEPROM ch=%02x std=%02x mode=%02x)\r\n", (WORD)i, (WORD)std[i], (WORD)mode[i]);
#endif
						}
					}

#endif
					//          printf("VS4210_SetInModeOutMode(),i=%d,newInRes=%d,OuputModeflg=%d\r\n",(int)i ,(int)newInRes[i],(int) gbVXIS_OuputModeflg ) ;

					//                    tp28xx_byte_write(0xF6,0x00);
					//
					//                    tmpval = tp28xx_byte_read(0x40);
					//                    printf("0x40=0x%02x\r\n",tmpval ) ;
					//                    tmpval = tp28xx_byte_read(0x4d);
					//                    printf("0x4D=0x%02x\r\n",tmpval ) ;
					//                    tmpval = tp28xx_byte_read(0x4E);
					//                    printf("0x4E=0x%02x\r\n",tmpval ) ;
					//
					//                    tmpval = tp28xx_byte_read(0x50);
					//                    printf("0x50=0x%02x\r\n",tmpval ) ;
					//                    tmpval = tp28xx_byte_read(0x51);
					//                    printf("0x51=0x%02x\r\n",tmpval ) ;
					//                    tmpval = tp28xx_byte_read(0x52);
					//                    printf("0x52=0x%02x\r\n",tmpval ) ;
					//                    tmpval = tp28xx_byte_read(0x53);
					//                    printf("0x53=0x%02x\r\n",tmpval ) ;
					//
					//
					//                    tmpval = tp28xx_byte_read(0xF6);
					//                    printf("0xF6=0x%02x\r\n",tmpval ) ;
					//                    tmpval = tp28xx_byte_read(0xF7);
					//                    printf("0xF7=0x%02x\r\n",tmpval ) ;
					//                    tmpval = tp28xx_byte_read(0xF8);
					//                    printf("0xF8=0x%02x\r\n",tmpval ) ;
					//                    tmpval = tp28xx_byte_read(0xF9);
					//                    printf("0xF9=0x%02x\r\n",tmpval ) ;
					//
					//                    tmpval = tp28xx_byte_read(0xFA);
					//                    printf("0xFA=0x%02x\r\n",tmpval ) ;
					//
					//                    tmpval = tp28xx_byte_read(0xFB);
					//                    printf("0xFB=0x%02x\r\n",tmpval ) ;
				}

				if (TVI != std[i])
				{
					// mask by ryan@20211213 �j��EQ�Ұ�  tp28xx_byte_write(0x07, 0x80|(gain>>2));  // manual mode
					if (std[i] == HDA)
					{
						printf("AHD Mode\r\n");
					}
					else
					{
						printf("CVI Mode\r\n");
					}
				}
				else // TVI
				{
					if (TP2833 == chip || TP2834 == chip)
					{
						if (mode[i] & FLAG_MEGA_MODE)
						{
							tp28xx_byte_write(0x07, 0x80 | (gain / 3)); // manual mode
						}
					}
					if (mode[i] == NTSC) // 20200408 cvbs night no burst
						printf("CVBS NTSC   mode\r\n");
					else if (mode[i] == PAL) // 20200408 cvbs night no burst
						printf("CVBS PAL   mode\r\n");
					else
						printf("LINE=%d,TVI Mode\r\n", __LINE__);
					// gRxLoopFlg = 0 ;
				}
			}
			else if (EQ_COUNT + 1 == count[i])
			{
				if (TVI != std[i])
				{
					ManualAGC(i);
					SetCGain(0x0c);
				}
				else // TVI
				{
					if (TP2833 == chip || TP2834 == chip)
					{
						if (mode[i] & FLAG_MEGA_MODE)
						{
							SetCGain(0x16);
						}
					}
				}
			}
			else if (EQ_COUNT + 2 == count[i]) // cocrx
			{
				if (state[i] == VIDEO_LOCKED)
				{
					tcByte *ptr_TP2824C;
					if (TVI == std[i])
					{
						if (mode[i] == NTSC || mode[i] == PAL)
						{
							ptr_TP2824C = TP2824C_CVBS_PTZ_DataSet;
#if (_DEBUG_MESSAGE_TP2824 == ON)
							Printf("(TP2824C_CVBS_PTZ_DataSet)");
#endif

							// SetGarbberLinerMode(bytGarbberLinerMode);
						}
						else
						{
							ptr_TP2824C = TP2824C_TVI_FHD_PTZ_DataSet;
#if (_DEBUG_MESSAGE_TP2824 == ON)
							Printf("(TP2824C_TVI_FHD_PTZ_DataSet)");
#endif
							// SetGarbberLinerMode(bytGarbberLinerMode);
						}

						I2CDeviceSet1(ptr_TP2824C);

						if (mode[i] == R1080P30 || mode[i] == R1080P25)
						{
							tp28xx_byte_write(0xc6, 0x19);
						}

						tp28xx_byte_write(0x40, i);

						tp28xx_byte_write(0xbe, 0x80);
						tp28xx_byte_read(0xB6);
						tp28xx_byte_read(0xB5);
						tp28xx_byte_read(0xB4);

						tp28xx_byte_write(0xa7, 0x00);
						delay(1); // 5ms
						tp28xx_byte_write(0xa7, 0x83);

						if (mode[i] == R1080P30 || mode[i] == R1080P25)
						{
							PtzTxChNum = i;
							printf("EQdet %d\r\n", PtzTxChNum);
						}
					}

					if (HDA == std[i])
					{
						ptr_TP2824C = TP2824C_AHD_FHD_PTZ_DataSet;
						I2CDeviceSet1(ptr_TP2824C);

						tp28xx_byte_write(0x40, i);
						tp28xx_byte_write(0xbe, 0x80);
						tp28xx_byte_read(0xB6);
						tp28xx_byte_read(0xB5);
						tp28xx_byte_read(0xB4);

						tp28xx_byte_write(0xa7, 0x24);
						delay(1); // 5ms
						tp28xx_byte_write(0xa7, 0xA7);

						// SetGarbberLinerMode(bytGarbberLinerMode);
					}

					if (HDC == std[i])
					{
						ptr_TP2824C = TP2824C_CVIHD_PTZ_DataSet;
						I2CDeviceSet1(ptr_TP2824C);

						tp28xx_byte_write(0x40, i);
						tp28xx_byte_write(0xbe, 0x80);
						tp28xx_byte_read(0xB6);
						tp28xx_byte_read(0xB5);
						tp28xx_byte_read(0xB4);

						tp28xx_byte_read(0xA4);
						tp28xx_byte_read(0xA2);
						tp28xx_byte_read(0xA3);

						tp28xx_byte_write(0xa7, 0x00);
						delay(1); // 5ms
						tp28xx_byte_write(0xa7, 0x83);

						// SetGarbberLinerMode(bytGarbberLinerMode);
					}

					if ((std[i] == TVI) && (mode[i] != NTSC) && (mode[i] != PAL) && (state[i] == VIDEO_LOCKED))
					{

						/*

						SetCOCCom(_COCOSD_BOX_Sel,i);
						SetCOCCom(_COCOSD_BOXONLINE,0);
						 Get_2824cIrq();

						 msleep(50);

						 SetCOCCom(_COCOSD_BOX_Channel_Sel,5);
						// COCSelectModeType=5;
						 Get_2824cIrq();
						msleep(50);
			*/

						BOX_ISSUE = 0;

						MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_TVI_BOX_Set);
						MCUTimerActiveTimerEvent(SEC(0.5), _SYSTEM_TIMER_EVENT_TVI_BOX_Set);
					}
				}
			}
		}
	}
}

void TVI_BOX_Set(void)
{
	BYTE i, ch[4] = {0, 3, 1, 2};

	// TriggerHandler(CheckTrigger());

#if 1
	for (i = 0; i < 4; i++)
	{

		if ((std[i] == TVI) && (mode[i] != NTSC) && (mode[i] != PAL) && (state[i] == VIDEO_LOCKED))
		{
#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
			Printf("(*TVI_BOX_Set=%d)", (WORD)i);
#endif
			SetCOCCom(_COCOSD_BOX_Sel, ch[i]);
			SetCOCCom(_COCOSD_BOXONLINE, ch[i]);
			Get_2824cIrq();

			SetCOCCom(_COCOSD_BOX_Channel_Sel, 5);
			COCSelectModeType[SelectModeType] = 5;
			Get_2824cIrq();
		}
	}

#else
	for (i = 0; i < 4; i++)
	{

		if ((std[i] == TVI) && (mode[i] != NTSC) && (mode[i] != PAL) && (state[i] == VIDEO_LOCKED))
		{

			SetCOCCom(_COCOSD_BOX_Sel, i);
			SetCOCCom(_COCOSD_BOXONLINE, 0);
			Get_2824cIrq();

			SetCOCCom(_COCOSD_BOX_Channel_Sel, 5);
			COCSelectModeType[SelectModeType] = 5;
			Get_2824cIrq();
		}
	}
#endif
}

tByte Det_HLock()
{
	tByte Reg01;
	tByte HLOCK;

	Reg01 = tp28xx_byte_read(0x01);
	HLOCK = (Reg01 >> 5) & 0x01;

	if (HLOCK == 1)
		return TRUE;
	else
		return FALSE;
}

tByte Set_ResWithAutoDet(tByte ch)
{
	tByte VidRes = R1080P30;
	tByte cnt = 5;
	SetRegPage(ch);
	//	printf("\r\nReg40=%02x, Reg01=%02x",(tWord)tp28xx_byte_read(TVII2CAddress,0x40),(tWord)tp28xx_byte_read(TVII2CAddress,0x01));
	while (cnt--)
	{
		if (!Det_VideoLoss(ch))
		{
			// for(VidRes=R720P60; VidRes<=R720P25; VidRes++)
			//{
			Set_VidRes(ch);
			// printf("\r\nVidRes=%02x, Reg01=%02x, Reg03=%02x",(tWord)VidRes,(tWord)tp28xx_byte_read(TVII2CAddress,0x01),(tWord)tp28xx_byte_read(TVII2CAddress,0x03));
			if ((Det_HLock() == TRUE) && (Det_VidRes() == VidRes))
				return TRUE;
			//}
		}
	}
	printf("H/V is not locked\r\n");
	return FALSE;
}

tByte Scan_Channels()
{
	tByte ch;
	tByte Det_ch = 0;
	tByte Sel_ch = 0xff;

	for (ch = CH1; ch <= CH4; ch++)
	{
		if (Set_ResWithAutoDet(ch) && !Det_ch)
		{
			// Sel_VD_Out(ch);
			Sel_ch = ch;
			Det_ch = 1;
		}
	}

	return Sel_ch;
}

void Set_PLL_StartUp()
{
	tByte PLL_con;
	PLL_con = tp28xx_byte_read(0x44);
	tp28xx_byte_write(0x44, (PLL_con | 0x40));
	tp28xx_byte_write(0x44, (PLL_con & 0xbf));
}
void Boot_Trigger_Function(BYTE ch)
{

	if (VideoSave[ch /*(TriggerStatus-1)*/] == _TRUE)
	{
		Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_std, ch);
		Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_output, ch);
		//	Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_COC,ch);

		// delay(100);//5ms

		//			count[ch]=23;

		//�ץ�����NTSC�v���ɷ|�����ܫG���D
		if (((mode[ch] == 8) || (mode[ch] == 9)) && (ch == 3) && (SelectModeType <= 4))
		{
			Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_COC, ch);
			msleep(800);
			count[ch] = 23;
		}
		else
		{
			count[ch] = 20;
		}
	}
	//	SetCOCCom(_COCOSD_BOX_Sel,ch);
	//	SetCOCCom(_COCOSD_BOXONLINE,0);
	//	 Get_2824cIrq();
}

void Init_TP280x_RegSet()
{

	tcByte *ptr_TP2802;
	tByte i, val;
	//    tByte ch[4]={0,2,3,1};
	//     tByte ch[4]={1,3,2,0};  ///this is ok
	//	tByte ch[4]={3,2,0,1};	//0=A,1=R,2=B,3=C
	tByte ch[4] = {0, 2, 3, 1}; // 0=A,1=R,2=B,3=C
	//	tByte ch[4]={2,3,1,0};	//0=A,1=R,2=B,3=C
	//	tByte ch[4]={1,3,2,0};	//0=A,1=R,2=B,3=C

	TVII2CAddress = 0x88;
	// TVII2CAddress = 0x8A;
	// TVII2CAddress = 0x8C;
	// TVII2CAddress = 0x8E;

	for (i = 0; i < 4; i++)
	{
		if (0x28 == tp28xx_byte_read(0xfe))
			break;
		TVII2CAddress++;
		TVII2CAddress++;
	}

	chip = tp28xx_byte_read(0xff);

	revision = tp28xx_byte_read(0xfd);
	revision &= 0x07;

	printf("\r\nchip %02x revision %02x\r\n", (tWord)chip, (tWord)revision);
	flag_CVBS = 0;
	flag_AUDIO = 0;
	flag_MEGA = 0;
	ptr_TP2802 = TP2824C_DataSet; // def  chunyu
	switch (chip)
	{

	case TP2802C:
	case TP2804:
	case TP2822:
		printf("EOL ID\r\n");
		break;

	case TP2823:
		flag_CVBS = 1;
		flag_AUDIO = 1;
		if (0x01 == revision)
		{
			chip = TP2833;
			ptr_TP2802 = TP2833_DataSet;
		}
		else
		{
			ptr_TP2802 = TP2823_DataSet;
		}
		break;

	case TP2834:
		flag_CVBS = 1;
		flag_AUDIO = 1;
		ptr_TP2802 = TP2834_DataSet;
		break;

	case TP2853C:
		flag_CVBS = 1;
		flag_AUDIO = 1;

		if (0x01 == revision)
		{
			chip = TP2826;
			// flag_MEGA = 1;
			flag_MEGA = 0; // chunyu
			ptr_TP2802 = TP2826_DataSet;
		}
		else if (0x00 == revision)
		{
			flag_MEGA = 1;
			ptr_TP2802 = TP2853C_DataSet;
		}
		else
		{
			ptr_TP2802 = TP2853C_DataSet;
		}
		break;

	case TP2824C:
		flag_CVBS = 1;
		flag_AUDIO = 1;

		if (0x04 == revision)
		{
			// flag_MEGA = 1;
			flag_MEGA = 0; // chunyu
			ptr_TP2802 = TP2824C_DataSet;
			printf("TP2824C test\r\n");
		}
		break;

	default:
		printf("unknown ID\r\n");
		break;
	}

	Set_PLL_StartUp();

	I2CDeviceSet1(ptr_TP2802);

	if (flag_AUDIO)
		I2CDeviceSet1(TP2834_Audio_DataSet);
		/*
			mode[CH1]= R1080P25;	//chunyu
			mode[CH2]= R1080P25;
			mode[CH3]= R1080P25;
			mode[CH4]= R1080P25;


			//mode[CH1]= R720P25;
			//mode[CH1]= R1080P25;
		   Set_VidRes(CH1);
		*/
		//    Set_VidRes(CH2);
		//    Set_VidRes(CH3);
		//    Set_VidRes(CH4);
		//	mode[CH3]= NoDet;	//chunyu
		//	Set_VidRes(CH3);
		// msleep(2000);//ryan@20200622

		// CurRes=RES1920X1080;
		// CurRes=RES1280X720;
		/*
		if((VideoSave[0] == _TRUE)&&(CURRENT_CH==VS4210_LOOP_MODE1))
		{
				ch[0]=0;
				ch[1]=2;
				ch[2]=3;
				ch[3]=1;
		}
		*/

#if 1

	CurRes = RES1920X1080;

	val = CheckTrigger();

	if (val)
	{
		TriggerHandler(CheckTrigger());

		// if((VideoSave[(TriggerStatus-1)] ==_TRUE))
		//	{
		Boot_Trigger_Function((TriggerStatus - 1));
		/*
		if(Flisheye_CAM[(TriggerStatus-1)]==TRUE)
		{
		TriggerMode=0;
		TriggerHandler(CheckTrigger());
		}
		*/
//}
// if(PowerFlag==ON)///power on
#if (StartPowerBlackScreen == OFF)
		BackLightON();
#endif
	}

#if 0 ///���y

  for( i = 0; i < CH_PER_CHIP; i++)
{

	  if(INPUT_VIDEO_CHECK(ch[i])||(TriggerStatus==(ch[i]+1)))
	  {
		  continue;
	  }

	
  if( VideoSave[ch[i]] ==_TRUE)
  {   
	Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_std,ch[i]);
//	Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_output,ch[i]);

   //  Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_COC,i);	   
	  //  count[i]=23;
	  }
  else
	  { 	  
	  count[ch[i]] = 0;
	  state[ch[i]] = VIDEO_UNPLUG;
	  std[ch[i]] = TVI;
	  mode[ch[i]] = NoDet;///R1080P30/*NoDet*/;
	  }
				  
}

  for( i = 0; i < CH_PER_CHIP; i++)
{

	  if(INPUT_VIDEO_CHECK(ch[i])||(TriggerStatus==(ch[i]+1)))
	  {
		  continue;
	  }

	
  if( VideoSave[ch[i]] ==_TRUE)
  {   
	//Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_std,ch[i]);
	Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_output,ch[i]);

   //  Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_COC,i);	   
	  //  count[i]=23;
	  }
  else
	  { 	  
	  count[ch[i]] = 0;
	  state[ch[i]] = VIDEO_UNPLUG;
	  std[ch[i]] = TVI;
	  mode[ch[i]] = NoDet;///R1080P30/*NoDet*/;
	  }
				  
}

#else
	for (i = 0; i < CH_PER_CHIP; i++)
	{

		if (INPUT_VIDEO_CHECK(ch[i]) || (TriggerStatus == (ch[i] + 1)))
		{
			continue;
		}

		if (VideoSave[ch[i]] == _TRUE)
		{
			Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_std, ch[i]);
			Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_output, ch[i]);

			//  Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_COC,i);
			//	count[i]=23;
		}
		else
		{
			count[ch[i]] = 0;
			state[ch[i]] = VIDEO_UNPLUG;
			std[ch[i]] = TVI;
			mode[ch[i]] = NoDet; /// R1080P30/*NoDet*/;
		}
	}
#endif

#if 0 ///���y
 for( i = 0; i <CH_PER_CHIP; i++)
  {

  		if((INPUT_VIDEO_CHECK(ch[i])==0))
		{
        	continue;
		}
  
	if( VideoSave[ch[i]] ==_TRUE)
	{
	  Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_std,ch[i]);
	// Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_output,ch[i]);
	 

  		}
	else
		{		
		count[ch[i]] = 0;
		state[ch[i]] = VIDEO_UNPLUG;
		std[ch[i]] = TVI;
		mode[ch[i]] = NoDet;///R1080P30/*NoDet*/;
		}
  }

for( i = 0; i <CH_PER_CHIP; i++)
 {

	   if((INPUT_VIDEO_CHECK(ch[i])==0))
	   {
		   continue;
	   }
 
   if( VideoSave[ch[i]] ==_TRUE)
   {
//	 Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_std,ch[i]);
	Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_output,ch[i]);
	

	   }
   else
	   {	   
	   count[ch[i]] = 0;
	   state[ch[i]] = VIDEO_UNPLUG;
	   std[ch[i]] = TVI;
	   mode[ch[i]] = NoDet;///R1080P30/*NoDet*/;
	   }
 }

#else
	for (i = 0; i < CH_PER_CHIP; i++)
	{

		if ((INPUT_VIDEO_CHECK(ch[i]) == 0))
		{
			continue;
		}

		if (VideoSave[ch[i]] == _TRUE)
		{
			/*
			  count[i] =0;
			  state[i] =VIDEO_IN;//VIDEO_IN;//VIDEO_IN;//VIDEO_LOCKED;
		//	  std[i] = 0x02;///ReadEEP(EEP_TP2824_STD0+(i*2));
		//	  mode[i] = NTSC;///ReadEEP(EEP_TP2824_STD0+(i*2)+1);
			  std[i] = ReadEEP(EEP_TP2824_STD0+(i*3));
			  mode[i] = ReadEEP(EEP_TP2824_STD0+(i*3)+1);
			  */
			Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_std, ch[i]);
			Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_output, ch[i]);

			// Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_COC,i);
			//  count[i]=23;
			/*
				if(std[i]==HDC)
					{
					Set_HDC(i);
					printf("(Set_HDC)");
					}
				 if(std[i]==HDA)
					{
					Set_HDA(i);
					printf("(Set_HDA)");
					}
			*/
			// SetRegPage(i);
			//  Set_VidRes(i);
		}
		else
		{
			count[ch[i]] = 0;
			state[ch[i]] = VIDEO_UNPLUG;
			std[ch[i]] = TVI;
			mode[ch[i]] = NoDet; /// R1080P30/*NoDet*/;
								 // Set_VidRes(i);
								 //  TP28xx_reset_default(chip, i);
		}
		// newInRes[i] = _OSD_720X480P60 ;
	}

#if (StartPowerBlackScreen == ON)
	if (val)
	{
		user_pwm_setvalue(GetBackLightLevel());
		BackLightON();
	}
#endif

#endif
	/*
	   for( i = 0; i <CH_PER_CHIP; i++)
	  {
		if( VideoSave[i] ==_TRUE)
		{
		Init_TP280x_CHx_RegSet(Tp2802_Boot_Set_output,i);
		}

	   }
	   */

#else

	mode[CH1] = R1080P25; // chunyu
	mode[CH2] = R1080P25;
	mode[CH3] = R1080P25;
	mode[CH4] = R1080P25;

	// mode[CH1]= R720P25;
	// mode[CH1]= R1080P25;
	Set_VidRes(CH1);

	//	  Set_VidRes(CH2);
	//	  Set_VidRes(CH3);
	//	  Set_VidRes(CH4);

	// msleep(2000);//ryan@20200622

	CurRes = RES1920X1080;
	// CurRes=RES1280X720;

	for (i = 0; i < CH_PER_CHIP; i++)
	{
		count[i] = 0;
		state[i] = VIDEO_UNPLUG;
		std[i] = TVI;
		mode[i] = NoDet;
	}

#endif
}

void Init_TP280x_CHx_RegSet(BYTE Action, BYTE index)
{

	tByte /*cvstd,status,*/ i = index, CAMD = 0;
	tByte gain, tmp, /*fmt,*/ tmpab;

	count[i] = 20;
	state[i] = VIDEO_LOCKED; // VIDEO_IN;//VIDEO_IN;//VIDEO_LOCKED;
	//	  std[i] = 0x02;///ReadEEP(EEP_TP2824_STD0+(i*2));
	//	  mode[i] = NTSC;///ReadEEP(EEP_TP2824_STD0+(i*2)+1);

	//#if(   _DEBUG_MESSAGE_Source_Change==ON)
	if (Action == Tp2802_Boot_Set_std)
		GraphicsPrint(RED, "(Tp2802_Boot_Set_std)\r\n");
	else if (Action == Tp2802_Boot_Set_output)
		GraphicsPrint(GREEN, "(Tp2802_Boot_Set_output)\r\n");
	else if (Action == Tp2802_Boot_Set_COC)
		GraphicsPrint(BLUE, "(Tp2802_Boot_Set_COC)\r\n");
		//	#endif

// if((SelectModeType==SelectCameraD)&&(NowModeState==SelectMode))
//	CAMD=1;
// bytSelectCAMRD
#if 0
if(i==2)
{
		//if(bytSelectCAMRD==SelectCameraD) 
		  std[i] = ReadEEP(EEP_TP2824_STD4);
		  mode[i] = ReadEEP(EEP_TP2824_MODE4);
		  
		  printf("Init_TP280x_CHx_RegSet in ch%02x (Action=%02x)\r\n", (tWord)(i),(tWord)Action);
}
else
{
	std[i] = ReadEEP(EEP_TP2824_STD2);
	mode[i] = ReadEEP(EEP_TP2824_MODE2);

}

#else
	if (i == 3)
	{
		if (bytSelectCAMRD == SelectCameraD)
			CAMD = 1;

		std[i] = ReadEEP(EEP_TP2824_STD0 + ((i + CAMD) * 3));
		mode[i] = ReadEEP(EEP_TP2824_STD0 + ((i + CAMD) * 3) + 1);

		printf("Init_TP280x_CHx_RegSet in ch%02x (Action=%02x)\r\n", (tWord)(i + CAMD), (tWord)Action);
	}
	else
	{
		std[i] = ReadEEP(EEP_TP2824_STD0 + ((i)*3));
		mode[i] = ReadEEP(EEP_TP2824_STD0 + ((i)*3) + 1);
	}
#endif

	SetRegPage(i);

	// Set_VidRes(i);

	// msleep(200) ;
	switch (Action)
	{
	case Tp2802_Boot_Set_std:
		if (std[i] == TVI)
		{
			Set_VidRes(i);
			// if((mode[i]==NTSC)||(mode[i]==PAL))
			// msleep(300) ;
			// std[i] = TVI;
			CurCH = i;
			if ((R720P30V2 == mode[i]) || (R720P25V2 == mode[i]))
			{
				if (0x00 == (0x08 & tp28xx_byte_read(0x03)))
				{
					mode[i] &= 0xf7;
					Set_VidRes(i);
					printf("(TVIA)");
					// printf("TVI_Set_VidRes\r\n");
				}
			}

			// tp28xx_byte_write(0xf6  , REG_F6[i]);
			tp28xx_byte_write(0xf6 + i, REG_F6[i]); // chunyu

#if (OUTPUT == Bits16out)
			if (TP2826 == chip)
			{
				tp28xx_byte_write(0xf8, REG_F7[i]);
				tp28xx_byte_write(0xfa, REG_FA[i]);
				tp28xx_byte_write(0xfb, REG_FA[i]);
			}
			else
			{
				tp28xx_byte_write(0xf7, REG_F7[i]);
				tp28xx_byte_write(0xfa, REG_FA[i]);
				// printf("set output ch%02x \r\n", (tWord)CH[i]);
			}
#else // BT656

			tmp = tp28xx_byte_read(0xf5);
			if (tmp &= SYS_MODE[i])
			{
				printf("//75.25M output\r\n");

				if (i == 0)
				{
					tmpab = tp28xx_byte_read(0xfa);
					tmpab = (tmpab & 0xF0) | 0x0A;
					tp28xx_byte_write(0xfa, tmpab);
				}
				else if (i == 1)
				{
					tmpab = tp28xx_byte_read(0xfa);
					tmpab = (tmpab & 0x0F) | 0xA0;
					tp28xx_byte_write(0xfa, tmpab);
				}

				if (i == 2)
				{
					tmpab = tp28xx_byte_read(0xfb);
					tmpab = (tmpab & 0xF0) | 0x0A;
					tp28xx_byte_write(0xfb, tmpab);
				}
				else if (i == 3)
				{
					tmpab = tp28xx_byte_read(0xfb);
					tmpab = (tmpab & 0x0F) | 0xA0;
					tp28xx_byte_write(0xfb, tmpab);
				}
			}
			else
			{
				extern tByte half_scaler;
				if (half_scaler) // down scaler output
				{
					tmp = tp28xx_byte_read(0x35);
					tmp |= 0x40;
					tp28xx_byte_write(0x35, tmp);

					if (i == 0)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0xF0) | 0x0A;
						tp28xx_byte_write(0xfa, tmpab);
					}
					else if (i == 1)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0x0F) | 0xA0;
						tp28xx_byte_write(0xfa, tmpab);
					}

					if (i == 2)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0xF0) | 0x0A;
						tp28xx_byte_write(0xfb, tmpab);
					}
					else if (i == 3)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0x0F) | 0xA0;
						tp28xx_byte_write(0xfb, tmpab);
					}
				}
				else
				{
					tmp = tp28xx_byte_read(0x35);
					tmp &= 0xbf;
					tp28xx_byte_write(0x35, tmp);

					if (i == 0)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0xF0) | 0x08;
						tp28xx_byte_write(0xfa, tmpab);
					}
					else if (i == 1)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0x0F) | 0x80;
						tp28xx_byte_write(0xfa, tmpab);
					}

					if (i == 2)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0xF0) | 0x08;
						tp28xx_byte_write(0xfb, tmpab);
					}
					else if (i == 3)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0x0F) | 0x80;
						tp28xx_byte_write(0xfb, tmpab);
					}
				}
			}
#endif

			if (CurRes != ManVidResRx)
			{
				CurRes = ManVidResRx;
				// CreateMDIN340VideoInstance();  //chunyu
				printf("ManVidResRx=0x%02x\r\n", ManVidResRx);
			}
		}
		else if (std[i] == HDC)
		{
			Set_HDC(i);
			printf("(Set_HDC)");
		}
		else if (std[i] == HDA)
		{
			Set_HDA(i);
			printf("(Set_HDA)");
/*
							CurCH = i;
							if( (R720P30V2 == mode[i]) || (R720P25V2 == mode[i]) )
							{
								if( 0x00 == (0x08 & tp28xx_byte_read(0x03)))
								{
									mode[i] &= 0xf7;
									Set_VidRes(i);
									printf("(TVIA)");
									//printf("TVI_Set_VidRes\r\n");
								}
							}

							//tp28xx_byte_write(0xf6  , REG_F6[i]);
							tp28xx_byte_write(0xf6 +i  , REG_F6[i]);  //chunyu
					*/
#if (OUTPUT == Bits16out)
			if (TP2826 == chip)
			{
				tp28xx_byte_write(0xf8, REG_F7[i]);
				tp28xx_byte_write(0xfa, REG_FA[i]);
				tp28xx_byte_write(0xfb, REG_FA[i]);
			}
			else
			{
				tp28xx_byte_write(0xf7, REG_F7[i]);
				tp28xx_byte_write(0xfa, REG_FA[i]);
				// printf("set output ch%02x \r\n", (tWord)CH[i]);
			}
#else // BT656

			tmp = tp28xx_byte_read(0xf5);
			if (tmp &= SYS_MODE[i])
			{
				printf("//75.25M output\r\n");

				if (i == 0)
				{
					tmpab = tp28xx_byte_read(0xfa);
					tmpab = (tmpab & 0xF0) | 0x0A;
					tp28xx_byte_write(0xfa, tmpab);
				}
				else if (i == 1)
				{
					tmpab = tp28xx_byte_read(0xfa);
					tmpab = (tmpab & 0x0F) | 0xA0;
					tp28xx_byte_write(0xfa, tmpab);
				}

				if (i == 2)
				{
					tmpab = tp28xx_byte_read(0xfb);
					tmpab = (tmpab & 0xF0) | 0x0A;
					tp28xx_byte_write(0xfb, tmpab);
				}
				else if (i == 3)
				{
					tmpab = tp28xx_byte_read(0xfb);
					tmpab = (tmpab & 0x0F) | 0xA0;
					tp28xx_byte_write(0xfb, tmpab);
				}
			}
			else
			{
				extern tByte half_scaler;
				if (half_scaler) // down scaler output
				{
					tmp = tp28xx_byte_read(0x35);
					tmp |= 0x40;
					tp28xx_byte_write(0x35, tmp);

					if (i == 0)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0xF0) | 0x0A;
						tp28xx_byte_write(0xfa, tmpab);
					}
					else if (i == 1)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0x0F) | 0xA0;
						tp28xx_byte_write(0xfa, tmpab);
					}

					if (i == 2)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0xF0) | 0x0A;
						tp28xx_byte_write(0xfb, tmpab);
					}
					else if (i == 3)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0x0F) | 0xA0;
						tp28xx_byte_write(0xfb, tmpab);
					}
				}
				else
				{
					tmp = tp28xx_byte_read(0x35);
					tmp &= 0xbf;
					tp28xx_byte_write(0x35, tmp);

					if (i == 0)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0xF0) | 0x08;
						tp28xx_byte_write(0xfa, tmpab);
					}
					else if (i == 1)
					{
						tmpab = tp28xx_byte_read(0xfa);
						tmpab = (tmpab & 0x0F) | 0x80;
						tp28xx_byte_write(0xfa, tmpab);
					}

					if (i == 2)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0xF0) | 0x08;
						tp28xx_byte_write(0xfb, tmpab);
					}
					else if (i == 3)
					{
						tmpab = tp28xx_byte_read(0xfb);
						tmpab = (tmpab & 0x0F) | 0x80;
						tp28xx_byte_write(0xfb, tmpab);
					}
				}
			}
#endif

			if (CurRes != ManVidResRx)
			{
				CurRes = ManVidResRx;
				// CreateMDIN340VideoInstance();  //chunyu
				printf("ManVidResRx=0x%02x\r\n", ManVidResRx);
			}
		}
		break;
	case Tp2802_Boot_Set_output:
		//{
		gain = ReadGain();
		printf("Egain=0x%02x ch%02x,std=%d,mode=0x%02X\r\n", (tWord)gain, (tWord)i, (tWord)std[i], (tWord)mode[i]); // chunyu

		if ((mode[i] == R1080P30) || (mode[i] == R1080P25))
		{
			newInRes[i] = _OSD_1920X1080P;
		}
		else if ((mode[i] == R720P50) || (mode[i] == R720P60))
		{
			newInRes[i] = _OSD_1280X720P;
		}
		else if ((mode[i] == R720P25) || (mode[i] == R720P30))
		{
			newInRes[i] = _OSD_1280X720P;
		}
		else if ((mode[i] == R720P30V2) || (mode[i] == R720P25V2))
		{
			newInRes[i] = _OSD_1280X720P;
		}
		else if (mode[i] == NTSC)
		{
			newInRes[i] = _OSD_720X480I60;
		}
		else if (mode[i] == PAL)
		{
			newInRes[i] = _OSD_720X576I50;
		}
		else
		{
			newInRes[i] = _OSD_No_Signal;
		}

		//  if (newInRes[i] != gbVXIS_InputMode[i] )
		{
			VS4210_SetInModeOutMode(i, newInRes[i], gbVXIS_OuputModeflg);

			// save tp2824 data
			/*
				if( ReadEEP(EEP_TP2824_STD0+(i*3)+2)==0)
				{
				 WriteEEP(EEP_TP2824_STD0+(i*3),std[i]);
				 WriteEEP(EEP_TP2824_STD0+(i*3)+1,mode[i]);
					  WriteEEP(EEP_TP2824_STD0+(i*3)+2,1);
				  VideoSave[i]=1;

				 printf("\r\n(WriteModeDataToEEPROM ch=%02x)",(WORD)i) ;
				}
				*/
		}

		if (TVI != std[i])
		{
			// mask by ryan@20211213 �j��EQ�Ұ�  tp28xx_byte_write(0x07, 0x80|(gain>>2));  // manual mode
			if (std[i] == HDA)
			{
				printf("AHD Mode\r\n");
			}
			else
			{
				printf("CVI Mode\r\n");
			}
		}
		else // TVI
		{
			if (TP2833 == chip || TP2834 == chip)
			{
				if (mode[i] & FLAG_MEGA_MODE)
				{
					tp28xx_byte_write(0x07, 0x80 | (gain / 3)); // manual mode
				}
			}
			if (mode[i] == NTSC) // 20200408 cvbs night no burst
				printf("CVBS NTSC   mode\r\n");
			else if (mode[i] == PAL) // 20200408 cvbs night no burst
				printf("CVBS PAL   mode\r\n");
			else
				printf("LINE=%d,TVI Mode\r\n", __LINE__);
			// gRxLoopFlg = 0 ;
		}
		// }
		break;
	case Tp2802_Boot_Set_COC:
		//	{
		if (TVI != std[i])
		{
			ManualAGC(i);
			SetCGain(0x0c);
		}
		else // TVI
		{
			if (TP2833 == chip || TP2834 == chip)
			{
				if (mode[i] & FLAG_MEGA_MODE)
				{
					SetCGain(0x16);
				}
			}
		}
		//}
		if (state[i] == VIDEO_LOCKED)
		{
			tcByte *ptr_TP2824C;
			if (TVI == std[i])
			{
				if (mode[i] == NTSC || mode[i] == PAL)
				{
					ptr_TP2824C = TP2824C_CVBS_PTZ_DataSet;
#if (_DEBUG_MESSAGE_TP2824 == ON)
					Printf("(2.TP2824C_CVBS_PTZ_DataSet)");
#endif
				}
				else
				{
					ptr_TP2824C = TP2824C_TVI_FHD_PTZ_DataSet;
#if (_DEBUG_MESSAGE_TP2824 == ON)
					Printf("(2.TP2824C_TVI_FHD_PTZ_DataSet)");
#endif
				}

				I2CDeviceSet1(ptr_TP2824C);

				if (mode[i] == R1080P30 || mode[i] == R1080P25)
				{
					tp28xx_byte_write(0xc6, 0x19);
				}

				tp28xx_byte_write(0x40, i);

				tp28xx_byte_write(0xbe, 0x80);
				tp28xx_byte_read(0xB6);
				tp28xx_byte_read(0xB5);
				tp28xx_byte_read(0xB4);

				tp28xx_byte_write(0xa7, 0x00);
				delay(1); // 5ms
				tp28xx_byte_write(0xa7, 0x83);

				if (mode[i] == R1080P30 || mode[i] == R1080P25)
				{
					PtzTxChNum = i;
					printf("EQdet %d\r\n", PtzTxChNum);
				}
			}

			if (HDA == std[i])
			{
				ptr_TP2824C = TP2824C_AHD_FHD_PTZ_DataSet;
				I2CDeviceSet1(ptr_TP2824C);

				tp28xx_byte_write(0x40, i);
				tp28xx_byte_write(0xbe, 0x80);
				tp28xx_byte_read(0xB6);
				tp28xx_byte_read(0xB5);
				tp28xx_byte_read(0xB4);

				tp28xx_byte_write(0xa7, 0x24);
				delay(1); // 5ms
				tp28xx_byte_write(0xa7, 0xA7);
			}

			if (HDC == std[i])
			{
				ptr_TP2824C = TP2824C_CVIHD_PTZ_DataSet;
				I2CDeviceSet1(ptr_TP2824C);

				tp28xx_byte_write(0x40, i);
				tp28xx_byte_write(0xbe, 0x80);
				tp28xx_byte_read(0xB6);
				tp28xx_byte_read(0xB5);
				tp28xx_byte_read(0xB4);

				tp28xx_byte_read(0xA4);
				tp28xx_byte_read(0xA2);
				tp28xx_byte_read(0xA3);

				tp28xx_byte_write(0xa7, 0x00);
				delay(1); // 5ms
				tp28xx_byte_write(0xa7, 0x83);
			}
		}
		break;
	}
}

void ConvertData(unsigned char dat, unsigned char buf_index)
{
	unsigned char i;

	PTZ_pelco = 0;

	for (i = 0; i < 8; i++)
	{
		PTZ_pelco <<= 3;

		if (0x80 & dat)
			PTZ_pelco |= 0x06;
		else
			PTZ_pelco |= 0x04;

		dat <<= 1;
	}
	PTZ_buf[buf_index] = (PTZ_pelco >> 16) & 0xff;
	PTZ_buf[buf_index + 1] = (PTZ_pelco >> 8) & 0xff;
	PTZ_buf[buf_index + 2] = (PTZ_pelco)&0xff;
}
unsigned char SwapBit(unsigned char dat, unsigned char buf_index)
{
	unsigned char i;
	unsigned char ret = 0;
	crc = 0;
	for (i = 0; i < 8; i++)
	{
		ret >>= 1;
		if (0x80 & dat)
		{
			ret |= 0x80;
			crc += 0x80;
		}
		dat <<= 1;
	}
	PTZ_buf[buf_index] = 0x02;
	PTZ_buf[buf_index + 1] = ret;
	PTZ_buf[buf_index + 2] = 0x7f | crc;
	PTZ_buf[buf_index + 3] = 0xff;
	PTZ_buf[buf_index + 4] = 0xfc;
	return ret;
}

BYTE AHDBitsReverse(BYTE val)
{
	BYTE i;
	BYTE cnt = 7;
	BYTE bitflag = 0x01;
	BYTE rval = 0x00;
	for (i = 0; i < 4; i++)
	{
		rval |= ((val >> cnt) & bitflag);
		bitflag <<= 1;
		cnt -= 2;
	}
	cnt = 7;
	bitflag = 0x80;
	for (i = 0; i < 4; i++)
	{
		rval |= ((val << cnt) & bitflag);
		bitflag >>= 1;
		cnt -= 2;
	}
	return rval;
}

void HDC_SetData(tByte reg, tByte dat)
{
	// line1
	// tp28xx_byte_write(reg + 0 , 0x00);
	tp28xx_byte_write(reg + 0, 0x02);
	tp28xx_byte_write(reg + 1, SwapBit(dat, 1));
	tp28xx_byte_write(reg + 2, 0x7f | crc);
	tp28xx_byte_write(reg + 3, 0xff);
	tp28xx_byte_write(reg + 4, 0xfc);
}

void HDA_SetData(tByte reg, tByte dat)
{
	ConvertData(dat, 0);
	tp28xx_byte_write(reg + 0, PTZ_buf[0]);
	tp28xx_byte_write(reg + 1, PTZ_buf[1]);
	tp28xx_byte_write(reg + 2, PTZ_buf[2]);
}

// 20200511 COC  >>
void TVICommandToAHData(void)
{
	AHDcmd3 = TVI_speed;
	AHDcmd4 = presetNum;

	if (TVI_Command == 0x17) // KEY_PRESET_CALL//"preset xx"
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x07;
		AHDcmd3 = 0;
	}
	else if (TVI_Command == 0x06) // KEY_TILT_UP
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x08;
	}
	else if (TVI_Command == 0x07) // KEY_TILT_DOWN
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x10;
	}
	else if (TVI_Command == 0x09) // KEY_PAN_LEFT
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x04;
	}
	else if (TVI_Command == 0x08) // KEY_PAN_RIGHT
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x02;
	}
	else if (TVI_Command == 0x0A) // KEY_TILT_UP + KEY_PAN_LEFT
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x0c;
	}
	else if (TVI_Command == 0x0C) // KEY_TILT_UP + KEY_PAN_RIGHT
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x0a;
	}
	else if (TVI_Command == 0x0B) // KEY_TILT_DOWN + KEY_PAN_LEFT
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x14;
	}
	else if (TVI_Command == 0x0D) // KEY_TILT_DOWN + KEY_PAN_RIGHT
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x12;
	}
	else if (TVI_Command == 0x0F) // KEY_IRIS_OPEN / OSD_MENU
	{
		AHDcmd1 = 0x02;
		AHDcmd2 = 0x00;
	}
	else if (TVI_Command == 0x0E) // KEY_IRIS_CLOSE
	{
		AHDcmd1 = 0x04;
		AHDcmd2 = 0x00;
	}
	else if (TVI_Command == 0x11) // KEY_FOCUS_FAR
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x20;
	}
	else if (TVI_Command == 0x10) // KEY_FOCUS_NEAR
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x40;
	}
	else if (TVI_Command == 0x13) // KEY_ZOOM_TELE
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x80;
	}
	else if (TVI_Command == 0x12) // KEY_ZOOM_WIDE
	{
		AHDcmd1 = 0x01;
		AHDcmd2 = 0x00;
	}
	else if (TVI_Command == 0x14) // KEY_MOTOR_STOP
	{
		AHDcmd1 = 0x00;
		AHDcmd2 = 0x00;
		AHDcmd3 = 0x00;
		AHDcmd4 = 0x00;
	}
}

void Get_2824cIrq(void) // cocrx
{
#if (COC_RX == ON)
	tByte addr, pcnt, RegB6;
#endif
	if ((count[0] > (EQ_COUNT + 2) && state[0] == VIDEO_LOCKED) ||
		(count[1] > (EQ_COUNT + 2) && state[1] == VIDEO_LOCKED) ||
		(count[2] > (EQ_COUNT + 2) && state[2] == VIDEO_LOCKED) ||
		(count[3] > (EQ_COUNT + 2) && state[3] == VIDEO_LOCKED))
	{

		SetRegPage(0x04);
		delay(1); // 5ms
#if (COC_RX == ON)
		RegB6 = tp28xx_byte_read(0xB6);
#endif
		tp28xx_byte_read(0xB5);
		tp28xx_byte_read(0xB4);

// PTZ RX	//TVI_1080P COC
#if (COC_RX == ON)
		if ((RegB6 & 0xF0) != 0) // VnRXIRQ
		{
			// V4RXIRQ---
			if ((RegB6 & 0x80) != 0 && count[3] > (EQ_COUNT + 2) /*&&(state[3] == VIDEO_LOCKED)&&std[3]==TVI && mode[3]!=NTSC && mode[3]!=PAL*/)
			{
				SetRegPage(0x03);
				// delay(1);//5ms
				pcnt = 0;
				for (addr = 0x8A; addr <= 0xA1; addr++) // Line1 & Line2
				{
					tpRxD[pcnt] = tp28xx_byte_read(addr);
					pcnt++;
				}
				tp28xx_byte_write(0xa7, 0x00);
				// delay(1);//5ms
				tp28xx_byte_write(0xa7, 0x83);

				for (pcnt = 0; pcnt < 5; pcnt++)
					COCRX4[pcnt] = tpRxD[pcnt + 1];
				for (pcnt = 5; pcnt < 10; pcnt++)
					COCRX4[pcnt] = tpRxD[pcnt + 2];
#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
				// GraphicsPrint(CYAN,"(TP_PRES_DOWN=%02x)",(WORD)TP_PRES_DOWN);
				printf("\r\nCOC4:");
				for (pcnt = 0; pcnt < 6; pcnt++)
					printf(" %02X", COCRX4[pcnt]);
				printf(" %02X", COCRX4[9]);
#endif

				// BOX on line  return 0x61, 0x01, 0x04, 0x9E,Lcd VINn=0x01 / Box CHn=0x04
				// COC2: 02 B5 01 61 01 02 9E
				if (COCRX4[3] == 0x61 && COCRX4[9] == 0x9E)
				{
					QuadBoxId |= 8; // 1:ch1 / 2:ch2 / 4:ch3 / 8:ch4

#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
					printf("QuadBoxId %02x\r\n", QuadBoxId);
#endif

					if (COCRX4[4] == 0x38 && COCRX4[5] == 0x02)
					{
						Flisheye_CAM[CH4] = TRUE;
						WriteEEP(EEP_FlishCAM4, TRUE);
						Flisheye_VLOSS_Count[FCH4] = 0;
						QuadBoxId &= ~(0x08);
					}
				}
			}

			// V3RXIRQ---
			if ((RegB6 & 0x40) != 0 && count[2] > (EQ_COUNT + 2) /*&&(state[2] == VIDEO_LOCKED)&&std[2]==TVI && mode[2]!=NTSC && mode[2]!=PAL*/)
			{
				SetRegPage(0x02);
				// delay(1);//5ms

				pcnt = 0;
				for (addr = 0x8A; addr <= 0xA1; addr++)
				{
					tpRxD[pcnt] = tp28xx_byte_read(addr);
					pcnt++;
				}

				tp28xx_byte_write(0xa7, 0x00);
				// delay(1);//5ms
				tp28xx_byte_write(0xa7, 0x83);

				for (pcnt = 0; pcnt < 5; pcnt++)
					COCRX3[pcnt] = tpRxD[pcnt + 1];
				for (pcnt = 5; pcnt < 10; pcnt++)
					COCRX3[pcnt] = tpRxD[pcnt + 2];

#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
				printf("\r\nCOC3:");
				for (pcnt = 0; pcnt < 6; pcnt++)
					printf(" %02X", COCRX3[pcnt]);
				printf(" %02X", COCRX3[9]);
#endif

				// BOX on line  return 0x61, 0x01, 0x04, 0x9E,Lcd VINn=0x01 / Box CHn=0x04
				// COC2: 02 B5 01 61 01 02 9E
				if (COCRX3[3] == 0x61 && COCRX3[9] == 0x9E)
				{
					QuadBoxId |= 4; // 1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
					printf("QuadBoxId %02x\r\n", QuadBoxId);
#endif

					if (COCRX3[4] == 0x38 && COCRX3[5] == 0x02)
					{
						Flisheye_CAM[CH3] = TRUE;
						WriteEEP(EEP_FlishCAM3, TRUE);
						Flisheye_VLOSS_Count[CH3] = 0;
						QuadBoxId &= ~(0x04);
					}
				}
			}

			// V2RXIRQ---
			if ((RegB6 & 0x20) != 0 && count[1] > (EQ_COUNT + 2) /*&&(state[1] == VIDEO_LOCKED) &&std[1]==TVI && mode[1]!=NTSC && mode[1]!=PAL*/)
			{
				SetRegPage(0x01);
				// delay(1);//5ms

				pcnt = 0;
				for (addr = 0x8A; addr <= 0xA1; addr++)
				{
					tpRxD[pcnt] = tp28xx_byte_read(addr);
					pcnt++;
				}

				tp28xx_byte_write(0xa7, 0x00);
				// delay(1);//5ms
				tp28xx_byte_write(0xa7, 0x83);

				for (pcnt = 0; pcnt < 5; pcnt++)
					COCRX2[pcnt] = tpRxD[pcnt + 1];
				for (pcnt = 5; pcnt < 10; pcnt++)
					COCRX2[pcnt] = tpRxD[pcnt + 2];

#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
				printf("\r\nCOC2:");
				for (pcnt = 0; pcnt < 6; pcnt++)
					printf(" %02X", COCRX2[pcnt]);
				printf(" %02X", COCRX2[9]);
#endif

				// BOX on line  return 0x61, 0x01, 0x04, 0x9E,Lcd VINn=0x01 / Box CHn=0x04
				// COC2: 02 B5 01 61 01 02 9E
				if (COCRX2[3] == 0x61 && COCRX2[9] == 0x9E)
				{
					QuadBoxId |= 2; // 1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
					printf("QuadBoxId %02x\r\n", QuadBoxId);
#endif

					if (COCRX2[4] == 0x38 && COCRX2[5] == 0x02)
					{
						Flisheye_CAM[CH2] = TRUE;
						WriteEEP(EEP_FlishCAM2, TRUE);
						Flisheye_VLOSS_Count[CH2] = 0;
						QuadBoxId &= ~(0x02);
					}
				}
			}

			// V1RXIRQ---
			if ((RegB6 & 0x10) != 0 && count[0] > (EQ_COUNT + 2) /*&&(state[0] == VIDEO_LOCKED) &&std[0]==TVI && mode[0]!=NTSC && mode[0]!=PAL*/)
			{
				SetRegPage(0x00);
				// delay(1);//5ms

				pcnt = 0;
				for (addr = 0x8A; addr <= 0xA1; addr++)
				{
					tpRxD[pcnt] = tp28xx_byte_read(addr);
					pcnt++;
				}

				tp28xx_byte_write(0xa7, 0x00);
				// delay(1);//5ms
				tp28xx_byte_write(0xa7, 0x83);

				for (pcnt = 0; pcnt < 5; pcnt++)
					COCRX1[pcnt] = tpRxD[pcnt + 1];
				for (pcnt = 5; pcnt < 10; pcnt++)
					COCRX1[pcnt] = tpRxD[pcnt + 2];

#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
				printf("\r\nCOC1:");
				for (pcnt = 0; pcnt < 6; pcnt++)
					printf(" %02X", COCRX1[pcnt]);
				printf(" %02X", COCRX1[9]);
#endif

				// BOX on line  return 0x61, 0x01, 0x04, 0x9E,Lcd VINn=0x01 / Box CHn=0x04
				// COC2: 02 B5 01 61 01 02 9E
				if (COCRX1[3] == 0x61 && COCRX1[9] == 0x9E)
				{
					QuadBoxId |= 1; // 1:ch1 / 2:ch2 / 4:ch3 / 8:ch4
#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
					printf("QuadBoxId %02x\r\n", QuadBoxId);
#endif

					if (COCRX1[4] == 0x38 && COCRX1[5] == 0x02)
					{
						Flisheye_CAM[CH1] = TRUE;
						WriteEEP(EEP_FlishCAM1, TRUE);
						Flisheye_VLOSS_Count[CH1] = 0;
						QuadBoxId &= ~(0x01);
					}
				}
			}
			// printf("\r\n");
		}
#endif
		// PTZ TX
		// else
		if (ptztxflag == 1) // no any RXIRQ
		{
			if (TVI == std[PtzTxChNum])
			{
				if (mode[PtzTxChNum] == NTSC || mode[PtzTxChNum] == PAL)
				{
					tByte cmd1 = 0, cmd2 = 0, dat1 = 0, dat2 = 0;

					dat1 = (tByte)TVI_speed;
					dat2 = (tByte)presetNum;

					if (TVI_Command == 0x0F) // SET//Iris +
						cmd1 = 0x02;
					else if (TVI_Command == 0x0E) // Iris -
						cmd1 = 0x04;
					else if (TVI_Command == 0x10) // Focus -
						cmd1 = 0x01;
					else if (TVI_Command == 0x11) // Focus +
						cmd2 = 0x80;
					else if (TVI_Command == 0x12) // Zoom -
						cmd2 = 0x40;
					else if (TVI_Command == 0x13) // Zoom +
						cmd2 = 0x20;
					else if (TVI_Command == 0x06) // up
						cmd2 = 0x08;
					else if (TVI_Command == 0x07) // down
						cmd2 = 0x10;
					else if (TVI_Command == 0x09) // left
						cmd2 = 0x04;
					else if (TVI_Command == 0x08) // right
						cmd2 = 0x02;
					else if (TVI_Command == 0x0A) // up + left
						cmd2 = 0x0c;
					else if (TVI_Command == 0x0C) // up + right
						cmd2 = 0x0a;
					else if (TVI_Command == 0x0B) // down + left
						cmd2 = 0x14;
					else if (TVI_Command == 0x0D) // down + right
						cmd2 = 0x12;
					else if (TVI_Command == 0x17) // PRESET x
					{
						cmd2 = 0x07;
						dat1 = 0;
						dat2 = (tByte)presetNum;
					}
					// set:0x0f / dn:0x06 / up:0x07 / L:0x13 / R:0x12
					cmd1 = AHDBitsReverse(cmd1); // 20200413
					cmd2 = AHDBitsReverse(cmd2); // 20200413
					dat1 = AHDBitsReverse(dat1); // 20200413
					dat2 = AHDBitsReverse(dat2); // 20200413

					SetRegPage(PtzTxChNum);
					tp28xx_byte_write(0x5a, cmd1); // command byte 1
					tp28xx_byte_write(0x59, cmd2); // command byte 2

					tp28xx_byte_write(0x60, dat1); // data byte 1
					tp28xx_byte_write(0x5f, dat2); // data byte 2

					tp28xx_byte_write(0x6f, 0x61);
					delay(10); // 50ms
					tp28xx_byte_write(0x6f, 0x60);
					printf("CVBS-Tx %x,%x,%x,%x\n", cmd1, cmd2, dat1, dat2);
				}
				else // 1080P
				{
					SetRegPage(PtzTxChNum);
					tp28xx_byte_write(0x59, (tByte)TVI_Command);
					tp28xx_byte_write(0x5a, (tByte)presetNum);
					tp28xx_byte_write(0x5d, (tByte)TVI_speed); // 0x5f  check this????
					tp28xx_byte_write(0x60, (tByte)TVI_Chksum);

					tp28xx_byte_write(0x6f, 0x41);
					delay(10); // 50ms
					tp28xx_byte_write(0x6f, 0x40);
					printf("TVI-Tx %x,%x,%x\n", TVI_Command, presetNum, TVI_speed);
				}

				ptztxflag = 0;
			}
			if (HDC == std[PtzTxChNum]) // 1080P / 720P
			{
				SetRegPage(PtzTxChNum);

				tp28xx_byte_write(0x6e, 0xA5);
				tp28xx_byte_write(0x6e, 0x02);
				switch (TVI_Command)
				{
				case 0x0F: // SET//Iris +
					tp28xx_byte_write(0x6e, 0x50);
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0x00);
					break;

				case 0x06: // up
					tp28xx_byte_write(0x6e, 0x08);
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0xFF);
					tp28xx_byte_write(0x6e, 0x00);
					break;

				case 0x07: // down
					tp28xx_byte_write(0x6e, 0x04);
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0xFF);
					tp28xx_byte_write(0x6e, 0x00);
					break;

				case 0x09: // left
					tp28xx_byte_write(0x6e, 0x02);
					tp28xx_byte_write(0x6e, 0xFF);
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0x00);
					break;

				case 0x08: // right
					tp28xx_byte_write(0x6e, 0x01);
					tp28xx_byte_write(0x6e, 0xFF);
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0x00);
					break;

				default:
				case 0x14: // STOP
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0x00);
					tp28xx_byte_write(0x6e, 0x00);
					break;
				}

				tp28xx_byte_write(0x6f, 0x49);
				delay(5); // 50ms
				tp28xx_byte_write(0x6f, 0x48);
				ptztxflag = 0;
				printf("CVI-Tx %x\n", TVI_Command);
			}
			if (HDA == std[PtzTxChNum]) // 1080P / 720P
			{
				TVICommandToAHData();

				SetRegPage(PtzTxChNum);

				// stop command
				tp28xx_byte_write(0x5a, 0x00);
				tp28xx_byte_write(0x60, 0x00);
				tp28xx_byte_write(0x66, 0x00);
				tp28xx_byte_write(0x6c, 0x00);

				tp28xx_byte_write(0x6f, 0x45);
				delay(5); // 50ms
				tp28xx_byte_write(0x6f, 0x44);

				// command data
				tp28xx_byte_write(0x5a, AHDBitsReverse(AHDcmd1));
				tp28xx_byte_write(0x60, AHDBitsReverse(AHDcmd2));
				tp28xx_byte_write(0x66, AHDBitsReverse(AHDcmd3));
				tp28xx_byte_write(0x6c, AHDBitsReverse(AHDcmd4));

				tp28xx_byte_write(0x6f, 0x45);
				delay(5); // 50ms
				tp28xx_byte_write(0x6f, 0x44);
				ptztxflag = 0;
				printf("AHD %x,%x,%x to ", TVI_Command, presetNum, TVI_speed);
				printf("Tx %x,%x,%x,%x\n", AHDcmd1, AHDcmd2, AHDcmd3, AHDcmd4);
			}
			ptztxflag = 0;
		}
	}
}

void SetCOCCom(BYTE action, BYTE param)
{
	BYTE SV[5] = {0, 2, 3, 1, 3};

#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
	Printf("\r\n(SetCOCCom a=%d p=%d)", (WORD)action, (WORD)param);
#endif

	switch (action)
	{
	case _COCOSD_UP:
		TVI_Command = 0x06;
		presetNum = 0x1F;
		TVI_speed = 0;
		TVI_Chksum = 0xDB;
		ptztxflag = 1;

		break;
	case _COCOSD_DOWN:
		TVI_Command = 0x07;
		presetNum = 0x1F;
		TVI_speed = 0;
		TVI_Chksum = 0xDC;
		ptztxflag = 1;

		break;
	case _COCOSD_LEFT:
		TVI_Command = 0x09;
		presetNum = 0;
		TVI_speed = 0x1F;
		TVI_Chksum = 0xDE;
		ptztxflag = 1;

		break;
	case _COCOSD_RIGHT:
		TVI_Command = 0x08;
		presetNum = 0;
		TVI_speed = 0x1F;
		TVI_Chksum = 0xDD;
		ptztxflag = 1;

		break;
	case _COCOSD_ENTER:
		TVI_Command = 0x0F;
		presetNum = 0;
		TVI_speed = 0;
		TVI_Chksum = 0xC5;
		ptztxflag = 1;
		break;

	case _COCOSD_BOXONLINE:
		TVI_Command = 0x60;
		presetNum = 1;
		TVI_speed = 0;
		TVI_Chksum = 0xFF - TVI_Command;
		ptztxflag = 1;

		break;
	case _COCOSD_STOP:
		TVI_Command = 0x14;
		presetNum = 0x00;
		TVI_speed = 0x00;
		TVI_Chksum = 0xCA;
		ptztxflag = 1;
		break;
	case _COCOSD_PRESET:
		TVI_Command = 0x17;
		presetNum = param;
		TVI_speed = 0;
		TVI_Chksum = 0xCD + presetNum;
		ptztxflag = 1;
		break;
	case _COCOSD_QUADMODE:
		TVI_Command = 0x62;
		presetNum = param;
		TVI_speed = 0;
		TVI_Chksum = 0xFF - TVI_Command;
		ptztxflag = 1;
		break;
	case _COCOSD_BOX_Sel:
		PtzTxChNum = SV[param]; // SelectModeType;
		// PtzTxChNum=param;
		break;
	case _COCOSD_BOX_Channel_Sel:
		TVI_Command = 0x62;
		presetNum = param;
		TVI_speed = 0;
		TVI_Chksum = 0xFF - TVI_Command;
		ptztxflag = 1;
		break;
	case _COC_FLISHEYE_MODE1:
		TVI_Command = 0x62;
		presetNum = 0x01; ///��@���e��
		TVI_speed = param;
		TVI_Chksum = 0xFF - TVI_Command;
		ptztxflag = 1;
		break;
	case _COC_FLISHEYE_MODE2:
		TVI_Command = 0x62;
		presetNum = 0x02; ///�����εe��
		TVI_speed = param;
		TVI_Chksum = 0xFF - TVI_Command;
		ptztxflag = 1;
		break;
	}

	// PtzTxChNum=SV[param];//SelectModeType;
#if (_DEBUG_MESSAGE_Get_2824cIrq == ON)
	Printf("(PtzTxChNum=%02x)", (WORD)PtzTxChNum);
#endif
}
