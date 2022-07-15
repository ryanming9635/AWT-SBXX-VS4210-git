// *****************************************************
// Company : Techpoint Inc
// $Date: 2014-03-10
// $Log:  $
// ******************************************************


#include "VXIS_Extern_Define.h"
//#include "inc\Config.h"

//#include "inc\typedefs.h"
//#include "inc\I2C.h"
//#include "inc\main.h"
//#include "inc\printf.h"

#include "inc\Device_Rx.h"
#include "inc\TP2802.h"
#include "stdlib.h"
#include "sw_i2c0.h"

extern tByte ManVidRes;
extern tByte ManSelCh;

//TP2802D EQ for short cable option
#define TP2802D_EQ_SHORT 0x0d
#define TP2802D_CGAIN_SHORT 0x74

//#define TP2802D_EQ_SHORT 0x01
//#define TP2802D_CGAIN_SHORT 0x70
#define CVI_TEST 0
#define AHD_TEST 1

#define CH_PER_CHIP 0x04
#define MAX_COUNT 	0xff

//extern void CreateMDIN340VideoInstance(void);
extern bit DetAutoRes;
extern void delay(tByte);

tByte	state[CH_PER_CHIP];
tByte	count[CH_PER_CHIP];
tByte	mode[CH_PER_CHIP];
tByte	std[CH_PER_CHIP];
tByte	egain[CH_PER_CHIP][4];
tByte	revision;
tByte	chip;
tByte	flag_LOCKED;
tByte TVII2CAddress=0x88;  //def
tByte	PTZ_buf[12];
tDWord PTZ_pelco;
tByte CurRes;
bit sywd;
bit flag_CVBS;
bit flag_AUDIO;

tcByte SYS_MODE[]= {0x01,0x02,0x04,0x08,0x0f};
tcByte SYS_AND[] = {0xfe,0xfd,0xfb,0xf7,0xf0};

enum
{
    TP2802C=0x02,
    TP2804 =0x04,
    TP2802D=0x05,
    TP2822 =0x22,
    TP2823 =0x23,
    TP2824 =0x24,
    TP2833 =0x25,
    TP2853C=0x26
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

    0xff, 0xff
};
tcByte TP2853C_MODE_DataSet[5][17]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0x18,0x2c,0x21,0x25,0xff}, //ADDR
    {0x03,0x50,0x30,0x05,0x60,0x30,0x70,0x48,0xbb,0x2e,0x90,0x0c,0x12,0x0a,0x84,0xf0,0xff}, //V1
    {0x13,0x50,0x30,0x05,0x60,0x30,0x70,0x48,0xbb,0x2e,0x90,0x08,0x12,0x0a,0x84,0xf0,0xff},	//V2
    {0x13,0x50,0x40,0x05,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x04,0x12,0x2a,0x84,0xf0,0xff}, //NTSC
    {0x13,0x51,0x48,0x05,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x04,0x17,0x2a,0x84,0xf0,0xff}  //PAL
};
tcByte TP2853C_HDA_DataSet[5][10]=
{
    {0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x21,0x25,0xff}, //ADDR
    {0x70,0x48,0x5e,0x27,0x72,0x80,0x77,0x44,0xf0,0xff}, //720p30
    {0x70,0x48,0x5e,0x27,0x88,0x04,0x23,0x44,0xf0,0xff}, //720p25
    {0x58,0x45,0x50,0x29,0x65,0x78,0x16,0x44,0xf0,0xff}, //1080p30
    {0x58,0x45,0x40,0x29,0x61,0x78,0x16,0x44,0xf0,0xff}, //1080p25
};
tcByte TP2853C_HDC_DataSet[5][14]=
{
    {0x13,0x15,0x16,0x20,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x21,0x25,0xff}, //ADDR
    {0x40,0x13,0x40,0x50,0x60,0x42,0x40,0x48,0x67,0x2e,0x90,0x84,0xf0,0xff}, //720p30
    {0x40,0x13,0x40,0x50,0x60,0x42,0x40,0x48,0x67,0x2e,0x90,0x84,0xf0,0xff}, //720p25
    {0x40,0x13,0xd3,0x48,0x60,0x45,0x40,0x41,0x83,0x2e,0x90,0x84,0xf0,0xff}, //1080p30
    {0x40,0x13,0xd3,0x48,0x60,0x45,0x40,0x41,0x83,0x2e,0x90,0x84,0xf0,0xff}, //1080p25
};

tcByte TP2834_Audio_DataSet[]=
{
//audio
    0x40, 0x40, //audio page
    0x00, 0x01, //A1
    0x01, 0x02, //A2
    0x02, 0x03, //A3
    0x03, 0x04, //A4
    0x04, 0x11, //AUX
    0x17, 0x00,
    0x18, 0x10, //5ch mode
    0x19, 0x0f,
    0x1A, 0x15, //PB to DAC
    0x1b, 0x01, //PB master
    0x37, 0x20,

    0x1d, 0x08,
    0x1d, 0x00,

    0x3c, 0x3f,
    0x3c, 0x00,

    0x3d, 0x01, //ADC reset
    0xff, 0xff
};

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
    0xff, 0xff
};
tcByte TP2833_MODE_DataSet[5][14]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0x18,0xff},
    {0x03,0x50,0x30,0x05,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x0c,0x12,0xff},
    {0x13,0x50,0x30,0x05,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x08,0x12,0xff},
    {0x13,0x50,0x40,0x05,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x04,0x11,0xff},
    {0x13,0x51,0x48,0x05,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x04,0x11,0xff}
};
tcByte TP2833_HDA_DataSet[5][8]=
{
    {0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0xff}, //ADDR
    {0x70,0x48,0x5e,0x27,0x72,0x80,0x77,0xff}, //720p30
    {0x70,0x48,0x5e,0x27,0x88,0x04,0x23,0xff}, //720p25
    {0x58,0x45,0x50,0x29,0x65,0x78,0x16,0xff}, //1080p30
    {0x58,0x45,0x40,0x29,0x61,0x78,0x16,0xff}, //1080p25
};

tcByte TP2824_DataSet[] =
{

    //video
    0x40, 0x04,
    0x07, 0xc0,
    0x0b, 0xc0,
    0x3a, 0x70,
    0x26, 0x01,

    0x4d, 0x0f,
    0x4e, 0x0f,

    0xff, 0xff
};
tcByte TP2824_MODE_DataSet[5][13]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0xff},
    {0x03,0x10,0x60,0x02,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x8c,0xff},
    {0x03,0x10,0x60,0x02,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x88,0xff},
    {0x43,0x10,0xa0,0x12,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x84,0xff},
    {0x43,0x11,0xb0,0x02,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x84,0xff}
};

//////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////
tcByte TP2823_DataSet[] =
{

    //video
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

    0xff, 0xff
};
tcByte TP2823_MODE_DataSet[5][13]=
{
    {0x0c,0x0d,0x20,0x26,0x2b,0x2d,0x2e,0x30,0x31,0x32,0x33,0x39,0xff},
    {0x43,0x10,0x60,0x02,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x30,0xff},
    {0x53,0x10,0x60,0x02,0x58,0x30,0x70,0x48,0xbb,0x2e,0x90,0x20,0xff},
    {0x53,0x10,0xa0,0x12,0x70,0x68,0x5e,0x62,0xbb,0x96,0xc0,0x10,0xff},
    {0x53,0x11,0xb0,0x02,0x70,0x60,0x5e,0x7a,0x4a,0x4d,0xf0,0x10,0xff}
};
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

    0xff, 0xff
};
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

    //now TP2801A just support 2 lines, to disable line3&4, else IRQ is in trouble.
    0x40, 0x01,
    0x50, 0x00,
    0x51, 0x00,
    0x52, 0x00,
    0x7F, 0x00,
    0x80, 0x00,
    0x81, 0x00,

    //0x50~0xb2 need bank switch
    0x40, 0x00,
    //TX total 34bit, valid load 32bit
    0x50, 0x00,
    0x51, 0x0b,
    0x52, 0x0c,
    0x53, 0x19,
    0x54, 0x78,
    0x55, 0x21,
    0x7e, 0x0f,   //

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
    0x8A, 0x27,   //
    0xB9, 0x0F,   //RX channel enable
    0xff, 0xff
};
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
void I2CDeviceSet(tByte addr, tcByte *RegSet)
{
	int	cnt=0;
	tByte index, val;

	// Output discription

	while ( *RegSet != 0xFF ) 
	{
		index = *RegSet;
		val = *(RegSet+1);
		tp28xx_byte_write(addr, index, val);
		RegSet+=2;

		cnt--;
	}

	delay(50);
}


void I2CDeviceSet2(tByte addr, tcByte *RegSet, tcByte *DatSet)
{
	int	cnt=0;
	tByte index, val;

	while ( *RegSet != 0xFF ) 
	{
		index = *RegSet;
		val = *DatSet;
		tp28xx_byte_write(addr, index, val);
		RegSet++;
		DatSet++;
		cnt--;
	}
	delay(50);
}


void SetRegPage(tByte channel)
{

    tp28xx_byte_write(TVII2CAddress,0x40,channel);
    //printf("\r\nChannel-%02x ",(tWord)channel);
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
    {0x02,0x15,0x16,0x17,0x18,0x19,0x1a,0x1c,0x1d,0x35,0x36,0xff},
    {0xc8,0x03,0xd3,0x80,0x29,0x38,0x47,0x08,0x98,0x05,0xdc,0xff}, //1080p30
    {0xc8,0x03,0xd3,0x80,0x29,0x38,0x47,0x0a,0x50,0x05,0xdc,0xff}, //1080p25
    {0xca,0x13,0x16,0x00,0x19,0xd0,0x25,0x06,0x72,0x05,0xdc,0xff}, //720p60 720p30V2
    {0xca,0x13,0x16,0x00,0x19,0xd0,0x25,0x07,0xbc,0x05,0xdc,0xff}, //720p50 720p25V2
    {0xca,0x13,0x16,0x00,0x19,0xd0,0x25,0x0c,0xe4,0x05,0xdc,0xff}, //720p30
    {0xca,0x13,0x16,0x00,0x19,0xd0,0x25,0x0f,0x78,0x05,0xdc,0xff}, //720p25
    {0xcf,0x13,0x4e,0xbc,0x15,0xf0,0x07,0x09,0x38,0x25,0xdc,0xff}, //NTSC
    {0xcf,0x13,0x5f,0xbc,0x17,0x20,0x17,0x09,0x48,0x25,0xdc,0xff}, //PAL
    {0xc8,0x13,0x6c,0x00,0x2c,0x00,0x68,0x09,0xc4,0x16,0x30,0xff}, //3M
    {0xc8,0x13,0x1f,0x20,0x34,0x98,0x7a,0x0b,0x9a,0x17,0xd0,0xff}, //5M
    {0xca,0x13,0x16,0x00,0x19,0xd0,0x25,0x06,0x72,0x25,0xdc,0xff}, //720p30V2
    {0xca,0x13,0x16,0x00,0x19,0xd0,0x25,0x07,0xbc,0x25,0xdc,0xff}  //720p25V2
};
void Set_VidRes(tByte Res)
{

    //tByte *ptr_TP2802_Res;
    tByte index, sys;
	index =1 ; 
	sys = 1 ; 
	
    switch(Res)
    {

    case R1080P30:
        //ptr_TP2802_Res = TP2802_1080P30_DataSet;
        index=1;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("1080p30");
        break;

    case R1080P25:
        //ptr_TP2802_Res = TP2802_1080P25_DataSet;
        index=2;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("1080p25");
        break;

    case R720P60:
        //ptr_TP2802_Res = TP2802_720P60_DataSet;
        index=3;
        sys=1;
        ManVidRes=RES1280X720;
        printf("720p60");
        break;
    case R720P30V2:
        //ptr_TP2802_Res = TP2802_720P60_DataSet;
        index=11;
        sys=2;
        ManVidRes=RES1280X720;
        printf("720p30V2");
        break;

    case R720P50:
        //ptr_TP2802_Res = TP2802_720P50_DataSet;
        index=4;
        sys=1;
        ManVidRes=RES1280X720;
        printf("720p50");
        break;
    case R720P25V2:
        //ptr_TP2802_Res = TP2802_720P50_DataSet;
        index=12;
        sys=2;
        ManVidRes=RES1280X720;
        printf("720p25V2");
        break;

    case R720P30:
        //ptr_TP2802_Res = TP2802_720P30_DataSet;
        index=5;
        sys=1;
        ManVidRes=RES1280X720;
        printf("720p30");
        break;

    case R720P25:
        //ptr_TP2802_Res = TP2802_720P25_DataSet;
        index=6;
        sys=1;
        ManVidRes=RES1280X720;
        printf("720p25");
        break;

    case PAL:
        //ptr_TP2802_Res = TP2802_PAL_DataSet;
        index=8;
        sys=4;
        ManVidRes=RES1920X576;
        printf("PAL");
        break;

    case NTSC:
        //ptr_TP2802_Res = TP2802_NTSC_DataSet;
        index=7;
        sys=3;
        ManVidRes=RES1920X480;
        printf("NTSC");
        break;
    case R3M:
        //ptr_TP2802_Res = TP2802_3M_DataSet;
        index=9;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("3M");
        break;
    case R5M:
        //ptr_TP2802_Res = TP2802_5M_DataSet;
        index=10;
        sys=1;
        ManVidRes=RES1920X1080;
        printf("5M");
        break;
    default:
        index=1;
        break;
    }
    printf("...\r\n");

    //I2CDeviceSet(TVII2CAddress, ptr_TP2802_Res );
    I2CDeviceSet2(TVII2CAddress, TP2802_FORMAT_DataSet[0],TP2802_FORMAT_DataSet[index] );
    if(TP2853C == chip)
    {
        I2CDeviceSet2(TVII2CAddress, TP2853C_MODE_DataSet[0],TP2853C_MODE_DataSet[sys] );
    }
    else if(TP2833 == chip)
    {
        I2CDeviceSet2(TVII2CAddress, TP2833_MODE_DataSet[0],TP2833_MODE_DataSet[sys] );
    }
    else if(TP2824 == chip)
    {
        I2CDeviceSet2(TVII2CAddress, TP2824_MODE_DataSet[0],TP2824_MODE_DataSet[sys] );
    }
    else if(TP2823 == chip)
    {
        I2CDeviceSet2(TVII2CAddress, TP2823_MODE_DataSet[0],TP2823_MODE_DataSet[sys] );
    }
}
tByte Set_HDA(tByte Res)
{

    tByte index,tmp;
    index=0;
    switch(Res)
    {

    case R1080P30:
        index=3;
        break;

    case R1080P25:
        index=4;
        break;

    case R720P30V2:
        index=1;
        break;

    case R720P25V2:
        index=2;
        break;
    }
    tmp=tp28xx_byte_read(TVII2CAddress, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(TVII2CAddress, 0x14, tmp);

    if(TP2853C == chip)
    {
        I2CDeviceSet2(TVII2CAddress, TP2853C_HDA_DataSet[0],TP2853C_HDA_DataSet[index] );
    }
    else if(TP2833 == chip || TP2824 == chip )
    {
        I2CDeviceSet2(TVII2CAddress, TP2833_HDA_DataSet[0],TP2833_HDA_DataSet[index] );
    }
    return index;
}

tByte Set_HDC(tByte Res)
{

    tByte index,tmp;
    index=0;
    switch(Res)
    {

    case R1080P30:
        index=3;
        break;

    case R1080P25:
        index=4;
        break;

    case R720P30V2:
        index=1;
        break;

    case R720P25V2:
        index=2;
        break;
    }
    tmp=tp28xx_byte_read(TVII2CAddress, 0x14);
    tmp |= 0x40;
    tp28xx_byte_write(TVII2CAddress, 0x14, tmp);

    if(TP2853C == chip)
    {
        I2CDeviceSet2(TVII2CAddress, TP2853C_HDC_DataSet[0],TP2853C_HDC_DataSet[index] );
    }
    else if(TP2833 == chip || TP2824 == chip )
    {
        I2CDeviceSet2(TVII2CAddress, TP2853C_HDC_DataSet[0],TP2853C_HDC_DataSet[index] );
    }
    return index;
}

tByte Det_VidRes()
{
    tByte CVSTD;
    tByte RES;

    CVSTD = tp28xx_byte_read(TVII2CAddress,0x03) & 0x07;

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

    tp28xx_byte_write(TVII2CAddress,0x40,ch&0x03);
    VDLOSS = tp28xx_byte_read(TVII2CAddress,0x01);
//	printf("video status 0x%02x on ch%01x\r\n",(tWord)VDLOSS,(tWord)ch);

    if(VDLOSS)	printf("No ");

    printf("video detected");


    VDLOSS = VDLOSS>>7;
    return VDLOSS;
}

static void tp28xx_manual_agc(unsigned char addr, unsigned char ch)
{
    unsigned int agc;
    unsigned char tmp;

    tp28xx_byte_write(addr, 0x2F, 0x02);
    agc = tp28xx_byte_read(addr, 0x04);
    //printf("AGC=0x%04x ch%02x\r\n", (tWord)agc, (tWord)ch);
    agc += tp28xx_byte_read(addr, 0x04);
    agc += tp28xx_byte_read(addr, 0x04);
    agc += tp28xx_byte_read(addr, 0x04);
    agc &= 0x3f0;
    agc >>=1;
    if(agc > 0x1ff) agc = 0x1ff;

    printf("result AGC=0x%04x ch%02x\r\n", (tWord)agc, (tWord)ch);
    tp28xx_byte_write(addr, 0x08, agc&0xff);
    tmp = tp28xx_byte_read(addr, 0x06);
    tmp &=0xf9;
    tmp |=(agc>>7)&0x02;
    tmp |=0x04;
    tp28xx_byte_write(addr, 0x06,tmp);
}

static void tp28xx_SYSCLK_V2(unsigned char addr, unsigned char ch)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(addr, 0xf5);
    tmp |= SYS_MODE[ch];
    tp28xx_byte_write(addr, 0xf5, tmp);

}
static void tp28xx_SYSCLK_V1(unsigned char addr, unsigned char ch)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(addr, 0xf5);
    tmp &= SYS_AND[ch];
    tp28xx_byte_write(addr, 0xf5, tmp);
}

static void tp2802D_reset_default(unsigned char addr, unsigned char ch)
{
    unsigned char tmp;
    tp28xx_byte_write(addr, 0x3A, 0x01);
    tp28xx_byte_write(addr, 0x0B, 0xC0);
    tp28xx_byte_write(addr, 0x07, 0xC0);
    tp28xx_byte_write(addr, 0x2e, 0x70);
    tp28xx_byte_write(addr, 0x39, 0x42);
    tp28xx_byte_write(addr, 0x09, 0x24);
    tmp = tp28xx_byte_read(addr, 0x06);   // soft reset and auto agc when cable is unplug
    tmp &= 0x7b;
    tp28xx_byte_write(addr, 0x06, tmp);

    tmp = tp28xx_byte_read(addr, 0xf5);
    if( tmp & SYS_MODE[ch])
    {
        tmp &= SYS_AND[ch];
        tp28xx_byte_write(addr, 0xf5, tmp);
    }
}

static void tp282x_reset_AGC_CLK(unsigned char addr, unsigned char ch)
{
    unsigned char tmp;

    tmp = tp28xx_byte_read(addr, 0x06);
    tmp &= 0xfb;
    tp28xx_byte_write(addr, 0x06, tmp);

    tmp = tp28xx_byte_read(addr, 0xf5);
    tmp &= SYS_AND[ch];
    tp28xx_byte_write(addr, 0xf5, tmp);

    tp28xx_byte_write(addr, 0x35, 0x05);
    tp28xx_byte_write(addr, 0x26, 0x01);
}

static void tp2824_reset_default(unsigned char addr)
{
    //unsigned char tmp;
    //tp28xx_byte_write(addr, 0x07, 0x40);
    //tp28xx_byte_write(addr, 0x0b, 0x40);
    //tp28xx_byte_write(addr, 0x3A, 0x88);
    tp28xx_byte_write(addr, 0x26, 0x01);
}
static void tp2853C_reset_default(unsigned char addr)
{
    unsigned char tmp;
    tp28xx_byte_write(addr, 0x07, 0xC0);
    tmp = tp28xx_byte_read(addr, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(addr, 0x14, tmp);
    tp28xx_byte_write(addr, 0x13, 0x00);
    tp28xx_byte_write(addr, 0x26, 0x04);
    tp28xx_byte_write(addr, 0x06, 0xb2);
}
static void TP28xx_reset_default(int chip, unsigned char addr, unsigned char ch)
{
    if(TP2823 == chip )
    {
        tp282x_reset_AGC_CLK(addr, ch);
    }
    else if(TP2822 == chip )
    {
        tp282x_reset_AGC_CLK(addr, ch);
        tp28xx_byte_write(addr, 0x07, 0x40);
    }
    else if(TP2802D == chip )
    {
        tp2802D_reset_default(addr, ch);
    }
    else if(TP2824 == chip )
    {
        tp2824_reset_default(addr);
    }
    else if(TP2833 == chip )
    {
        tp2853C_reset_default(addr);
    }
    else if(TP2853C == chip )
    {
        tp2853C_reset_default(addr);
    }
}

void EQ_Detect()
{
    tByte cvstd,status,i;
    tByte gain, tmp;
    //tWord agc;
    //tcByte UV_offset[16]={0x00,0x00,0x0b,0x0b,0x16,0x16,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21};
    const unsigned char TP2802D_CGAIN[16]   = {0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x60,0x50,0x40,0x38,0x30};
    const unsigned char TP2802D_CGAINMAX[16]= {0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x44,0x44,0x44,0x44,0x44};
    const unsigned char TP2823_CGAINMAX[16] = {0x4a,0x47,0x46,0x45,0x44,0x44,0x43,0x43,0x42,0x42,0x42,0x42,0x40,0x40,0x40,0x40};


    for(i = 0; i < CH_PER_CHIP; i++)//scan four inputs:
    {
        SetRegPage(i);
        status = tp28xx_byte_read(TVII2CAddress, 0x01);

        //state machine for video checking
        if(status & FLAG_LOSS)
        {

            if(VIDEO_UNPLUG == state[i])
            {
                if(count[i] < MAX_COUNT) count[i]++;
                continue;
            }
            else
            {
                state[i] = VIDEO_UNPLUG;
                count[i] = 0;
                //std[i] = NoDet;
                mode[i] = NoDet;
                TP28xx_reset_default(chip, TVII2CAddress, i);
                //I2CDeviceSet(TVII2CAddress, TP2802_720P50_DataSet);
                printf("video loss ch%02x \r\n", (tWord)i);
            }

        }
        else
        {
            if(PAL == mode[i] || NTSC == mode[i])
                flag_LOCKED = CVBS_FLAG_LOCKED;
            else
                flag_LOCKED = TVI_FLAG_LOCKED;
            if( flag_LOCKED == (status & flag_LOCKED) ) //video locked
            {
                if(VIDEO_LOCKED == state[i])
                {
                    if(count[i] < MAX_COUNT) count[i]++;
                }
                else if(VIDEO_UNPLUG == state[i])
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
                }
            }
            else  //video in but unlocked
            {
                if(VIDEO_UNPLUG == state[i])
                {
                    state[i] = VIDEO_IN;
                    count[i] = 0;
                    printf("video in ch%02x \r\n", (tWord)i);
                }
                else if(VIDEO_LOCKED == state[i])
                {
                    state[i] = VIDEO_UNLOCK;
                    count[i] = 0;
                    printf("video unstable ch%02x \r\n", (tWord)i);
                }
                else
                {
                    if(count[i] < MAX_COUNT) count[i]++;
                    if(VIDEO_UNLOCK == state[i] && count[i] > 2)
                    {
                        state[i] = VIDEO_IN;
                        count[i] = 0;
                        TP28xx_reset_default(chip, TVII2CAddress, i);
                        printf("video unlocked ch%02x \r\n", (tWord)i);
                    }
                }
            }

        }

        if( VIDEO_IN == state[i])
        {
            tp28xx_byte_write(TVII2CAddress, 0x35, 0x05);
            tp28xx_byte_write(TVII2CAddress, 0x36, 0xdc);
            cvstd = tp28xx_byte_read(TVII2CAddress, 0x03);
            printf("video format %2x detected ch%02x \r\n", (tWord)(cvstd), (tWord)i );
            sywd = cvstd & 0x08;
            cvstd &= 0x07;
            if(NTPAL == cvstd && flag_CVBS)
            {

                if(count[i] & 0x01)
                {
                    mode[i] = PAL;
                    Set_VidRes(mode[i]);

                    if(TP2823 == chip)
                    {
                        if(0x02 == i) tp28xx_byte_write(TVII2CAddress, 0x0d, 0x01);
                    }

                }
                else
                {
                    mode[i] = NTSC;
                    Set_VidRes(mode[i]);
                    if(TP2823 == chip)
                    {
                        if(0x02 == i) tp28xx_byte_write(TVII2CAddress, 0x0d, 0x00);
                    }

                }
                tp28xx_SYSCLK_V2(TVII2CAddress, i);
            }
            else if(cvstd == 4 || cvstd == 5 )
            {
                mode[i] = cvstd|0x08;
                Set_VidRes(mode[i]);
                tp28xx_SYSCLK_V2(TVII2CAddress, i);

            }
            else if(cvstd < 4 )
            {
                mode[i] = cvstd;
                Set_VidRes(mode[i]);
                tp28xx_SYSCLK_V1(TVII2CAddress, i);

            }
            else
            {
                if( TP2853C == chip || TP2833 == chip )//TP2853C 3M auto detect test
                {
                    tp28xx_byte_write(TVII2CAddress, 0x2f, 0x09);
                    tmp = tp28xx_byte_read(TVII2CAddress, 0x04);
                    if((tmp > 0x48) && (tmp < 0x55))
                    {
                        mode[i] = R3M;
                        Set_VidRes(mode[i]);
                    }
                    else if((tmp > 0x55) && (tmp < 0x62))
                    {
                        mode[i] = R5M;
                        Set_VidRes(mode[i]);
                    }
                }
            }

        }
#define EQ_COUNT 10
        if( VIDEO_LOCKED == state[i]) //check signal lock
        {
            if(0 == count[i])
            {
                std[i] = TVI;
                if( (R720P30V2 == mode[i]) || (R720P25V2 == mode[i]) )
                {
                    if( 0x00 == (0x08 & tp28xx_byte_read(TVII2CAddress, 0x03)))
                    {

                        mode[i] &= 0xf7;
                        Set_VidRes(mode[i]);
                        tp28xx_SYSCLK_V1(TVII2CAddress, i);
                        //if(TP2802D == chip) tp28xx_byte_write(TVII2CAddress, 0x39, 0x41);
                        //else tp28xx_byte_write(TVII2CAddress, 0x35, 0x25);
                        tp28xx_byte_write(TVII2CAddress, 0x35, 0x05);

                    }

                }
                tmp=tp28xx_byte_read(TVII2CAddress, 0x14);
                tmp &= 0xbf;
                tp28xx_byte_write(TVII2CAddress, 0x14, tmp);

                {
                    tcByte CH[4]= {0x00,0x11,0x22,0x33};

                    tp28xx_byte_write(TVII2CAddress, 0xf6, CH[i]);
                    //printf("set output ch%02x \r\n", (tWord)CH[i]);
                }
                tmp=tp28xx_byte_read(TVII2CAddress, 0xf5);
                if( tmp &= SYS_MODE[i])
                {
                    tp28xx_byte_write(TVII2CAddress, 0xfa, 0xaa);
                }
                else
                {
                    extern bit half_scaler;
                    if(half_scaler) //down scaler output
                    {
                        tp28xx_byte_write(TVII2CAddress, 0x35, 0x45);
                        tp28xx_byte_write(TVII2CAddress, 0xfa, 0xaa);
                    }
                    else
                    {
                        tp28xx_byte_write(TVII2CAddress, 0xfa, 0x88);
                    }
                }
                if( CurRes != ManVidRes)
                {
                    CurRes = ManVidRes;
                    //CreateMDIN340VideoInstance();
                }
            }
            else if(1 == count[i])
            {
                if( chip > TP2823 )
                {
                    tmp = tp28xx_byte_read(TVII2CAddress, 0x01);
                    printf("Reg01 %02x\r\n", (tWord)tmp);
                    if( 0x60 == (0x64 & tmp))
                    {
                        //if(Set_HDA(mode[i])) printf("turn to A ch%02x\r\n", (tWord)i);
                        if(Set_HDC(mode[i]))
                        {
                            std[i] = HDC;
                            printf("turn to C ch%02x\r\n", (tWord)i);
                        }
                    }

                }

            }
            else if(2 == count[i])
            {
                if( chip > TP2823 )
                {
                    tmp = tp28xx_byte_read(TVII2CAddress, 0x01);
                    printf("Reg01 %02x\r\n", (tWord)tmp);
                    if( 0x10 != (0x11 & tmp) && HDC == std[i])
                    {
                        if(Set_HDA(mode[i]))
                        {
                            std[i] = HDA;
                            printf("turn to A ch%02x\r\n", (tWord)i);
                        }

                    }

                }

            }
            else if(3 == count[i])
            {
                if( chip > TP2823 )
                {
                    tmp = tp28xx_byte_read(TVII2CAddress, 0x01);
                    printf("Reg01 %02x\r\n", (tWord)tmp);
                    if( 0x00 != (0x01 & tmp))
                    {
                        //Set_VidRes(mode[i]);	printf("back to T ch%02x\r\n", (tWord)i);
                    }

                }

            }
            else if( count[i] < EQ_COUNT-3)
            {
                //wdi->count[i]++;
                egain[i][3] = egain[i][2];
                egain[i][2] = egain[i][1];
                egain[i][1] = egain[i][0];
                gain = tp28xx_byte_read(TVII2CAddress, 0x03);
                //egain[i][EQ_COUNT - count[i]] = gain&0xf0;
                egain[i][0] = gain&0xf0;
                printf("Egain=0x%02x ch%02x\r\n", (tWord)gain, (tWord)i);

            }
            else if( EQ_COUNT == count[i])
            {
                count[i]--;
                egain[i][3] = egain[i][2];
                egain[i][2] = egain[i][1];
                egain[i][1] = egain[i][0];

                gain = tp28xx_byte_read(TVII2CAddress, 0x03);
                egain[i][0] = gain&0xf0;
                if(abs(egain[i][3] - egain[i][0])< 0x20 && abs(egain[i][2] - egain[i][0])< 0x20 && abs(egain[i][1] - egain[i][0])< 0x20 )
                {

                    count[i]++;

                    tmp = tp28xx_byte_read(TVII2CAddress, 0x03);
                    printf("result Egain=0x%02x ch%02x\r\n", (tWord)tmp, (tWord)i);

                    if(TP2822 == chip)
                    {
                        //tmp = tp28xx_byte_read(TVII2CAddress, 0x03);
                        //printf("result Egain=0x%02x ch%02x\r\n", (tWord)tmp, (tWord)i);
                        tmp &=0xf0;
                        tp28xx_byte_write(TVII2CAddress, 0x07, tmp>>2);  // manual mode
                        tmp >>=4;
                        tp28xx_byte_write(TVII2CAddress, 0x2b, TP2823_CGAINMAX[tmp]);
                    }
                    else if(TP2802D == chip )
                    {

                        //tmp = tp28xx_byte_read(TVII2CAddress, 0x03);
                        //printf("result Egain=0x%02x ch%02x\r\n", (tWord)tmp, (tWord)i);

                        tmp &=0xf0;
                        tmp >>=4;
                        tp28xx_byte_write(TVII2CAddress, 0x2e, TP2802D_CGAIN[tmp]);
                        tp28xx_byte_write(TVII2CAddress, 0x2b, TP2802D_CGAINMAX[tmp]);
                        if(tmp < 0x02)
                        {
                            tp28xx_byte_write(TVII2CAddress, 0x3a, TP2802D_EQ_SHORT);
                            tp28xx_byte_write(TVII2CAddress, 0x2e, TP2802D_CGAIN_SHORT);
                        }
                        if(tmp > 0x03 && ((R720P30V2 == mode[i]) || (R720P25V2 == mode[i])) )
                        {
                            tp28xx_byte_write(TVII2CAddress, 0x3A, 0x09);  // long cable
                            tp28xx_byte_write(TVII2CAddress, 0x07, 0x40);  // for long cable
                            tp28xx_byte_write(TVII2CAddress, 0x09, 0x20);  // for long cable
                            tmp = tp28xx_byte_read(TVII2CAddress, 0x06);
                            tmp |=0x80;
                            tp28xx_byte_write(TVII2CAddress, 0x06,tmp);
                        }
                    }
                    else if(TP2823 == chip)
                    {
                        //tmp = tp28xx_byte_read(TVII2CAddress, 0x03);
                        //printf("result Egain=0x%02x ch%02x\r\n", (tWord)tmp, (tWord)i);
                        tmp >>=4;
                        tp28xx_byte_write(TVII2CAddress, 0x2b, TP2823_CGAINMAX[tmp]);
                    }
                    else if(TP2824 == chip)
                    {
                        /*
                                                                                                //gpio_i2c_write(wdi->addr, 0x06, 0x33); //
                                                                                                tp28xx_byte_write(TVII2CAddress, 0x2F, 0x00);
                                                                                                //msleep(5);
                                                                                                tmp = tp28xx_byte_read(TVII2CAddress, 0x03);
                                                                                                printf("EQ1=0x%02x ch%02x\r\n", (tWord)tmp, (tWord)i);
                                                                                                tmp &= 0xf0;
                                                                                                if(tmp > 0x40)
                                                                                                {
                                                                                                    gain = tp28xx_byte_read(TVII2CAddress, 0x04);
                                                                                                    printf("EQ2=0x%02x ch%02x\r\n", (tWord)gain, (tWord)i);
                                                                                                    //tmp &= 0xf0;
                                                                                                    tmp >>= 2;
                                                                                                    tmp += (tmp>>2);
                                                                                                    tmp |= 0x80;
                                                                                                    //printk("EQ1=0x%02x ch%02x\r\n", tmp, i);
                                                                                                    tp28xx_byte_write(TVII2CAddress, 0x07, tmp);
                                                                                                    gain >>= 3;
                                                                                                    gain += (gain>>1);
                                                                                                    gain |= 0x80;
                                                                                                    //printk("EQ2=0x%02x ch%02x\r\n", gain, i);
                                                                                                    tp28xx_byte_write(TVII2CAddress, 0x0B, gain);
                                                                                                    tp28xx_byte_write(TVII2CAddress, 0x2B, 0x44);
                                                                                                }
                                                                                                else{
                                                                                                    tp28xx_byte_write(TVII2CAddress, 0x07, 0xC0);
                                                                                                    tp28xx_byte_write(TVII2CAddress, 0x0B, 0xC0);
                                                                                                }
                                                                                                tp28xx_byte_write(TVII2CAddress, 0x3A, 0x00);
                                                                                                tp28xx_byte_write(TVII2CAddress, 0x06, 0x33);

                         */
                    }

                }
            }
            else if(EQ_COUNT+1 == count[i])
            {
                if(TP2822 == chip || TP2823 == chip)
                {
                    tp28xx_manual_agc(TVII2CAddress, i);

                }
                else if( TP2802D == chip)
                {
                    if( (R720P30V2 != mode[i]) && (R720P25V2 != mode[i]) )
                    {
                        tp28xx_manual_agc(TVII2CAddress, i);
                    }
                }

            }
            else if( EQ_COUNT+5 == count[i])
            {
                if( TP2802D == chip )
                {
                    if( (R720P30V2== mode[i]) || (R720P25V2 == mode[i]) )
                    {
                        tp28xx_manual_agc(TVII2CAddress, i);
                    }
                }

            }



        }

    }

}



tByte Det_HLock()
{
    tByte Reg01;
    tByte HLOCK;

    Reg01 = tp28xx_byte_read(TVII2CAddress,0x01);
    HLOCK = (Reg01>>5)&0x01;

    if(HLOCK==1)
        return TRUE;
    else
        return FALSE;
}

/*
tByte Set_ResWithAutoDet(tByte ch){
	tByte VidRes;
	tByte cnt=5;
	SetRegPage(ch);
//	printf("\r\nReg40=%02x, Reg01=%02x",(tWord)tp28xx_byte_read(TVII2CAddress,0x40),(tWord)tp28xx_byte_read(TVII2CAddress,0x01));
	if(!Det_VideoLoss()){
		if(Det_HLock()== FALSE){
			Change_720P1080P_Det_mode();
			while(Det_HLock()== FALSE){
				printf(", H/V is not locked");
				if((cnt--)==0)
					return FALSE;
			}
		}
		VidRes = Det_VidRes();
		Set_VidRes(VidRes);

		return TRUE;

	}
	return FALSE;
}
*/


tByte Set_ResWithAutoDet(tByte ch)
{
    tByte VidRes;
    tByte cnt=5;
	bit bi = 0 ; 
    SetRegPage(ch);
    //	printf("\r\nReg40=%02x, Reg01=%02x",(tWord)tp28xx_byte_read(TVII2CAddress,0x40),(tWord)tp28xx_byte_read(TVII2CAddress,0x01));
    while(cnt--)
    {
        if(!Det_VideoLoss(ch))
        {
            for(VidRes=R720P60; VidRes<=R720P25; VidRes++)
            {
                Set_VidRes(VidRes);
                //printf("\r\nVidRes=%02x, Reg01=%02x, Reg03=%02x",(tWord)VidRes,(tWord)tp28xx_byte_read(TVII2CAddress,0x01),(tWord)tp28xx_byte_read(TVII2CAddress,0x03));
                if((Det_HLock()== TRUE)&&(Det_VidRes()==VidRes))
				{
					printf("locked VidRes=%d\r\n",VidRes);
					bi =1 ; 
                    //return TRUE;
				}
            }
        }
    }
	if (bi == 0)
    printf("H/V is not locked\r\n");

    return bi ;
}

/*
tByte Scan_Channels(){
	tByte ch;
	tByte Det_ch=0;
	tByte Sel_ch=0xff;

	for(ch = CH1;ch<=CH4;ch++){
		if(Set_ResWithAutoDet(ch)&&!Det_ch){
			//Sel_VD_Out(ch);
			Sel_ch = ch;
			Det_ch = 1;
		}
	}

	return Sel_ch;
}
*/

void Set_PLL_StartUp()
{
    tByte PLL_con;
    PLL_con = tp28xx_byte_read(TVII2CAddress,0x44);
    tp28xx_byte_write(TVII2CAddress,0x44,(PLL_con|0x40));
    tp28xx_byte_write(TVII2CAddress,0x44,(PLL_con&0xbf));

}

void Init_TP280x_RegSet()
{

    tcByte *ptr_TP2802;
    tByte i;

    TVII2CAddress = 0x88;

    for(i=0; i<4; i++)
    {
        if(0x28 == tp28xx_byte_read(TVII2CAddress,0xfe)) 
			break;
        TVII2CAddress++;
        TVII2CAddress++;
    }

    chip = tp28xx_byte_read(TVII2CAddress,0xff);
    revision = tp28xx_byte_read(TVII2CAddress,0xfd);
    revision &= 0x07;

    printf("\r\nchip %02x revision %02x\r\n", (int)chip,(int)revision);
    flag_CVBS = 0;
    flag_AUDIO = 0;
	
	ptr_TP2802 = TP2824_DataSet;
	
    switch(chip)
    {
    case TP2802C:
    case TP2804:
        if(0x01 == revision)
        {
            chip=TP2802D;
            ptr_TP2802 = TP2802D_DataSet;
        }
        break;

    case TP2822:
        ptr_TP2802 = TP2822_DataSet;
        break;

    case TP2823:
        flag_CVBS = 1;
        flag_AUDIO = 1;
        if(0x01 == revision)
        {
            chip=TP2833;
            ptr_TP2802 = TP2833_DataSet;
        }
        else
        {
            ptr_TP2802 = TP2823_DataSet;
        }
        break;

    case TP2824:
        flag_CVBS = 1;
        flag_AUDIO = 1;
        ptr_TP2802 = TP2824_DataSet;
        break;

    case TP2853C:
        flag_CVBS = 1;
        flag_AUDIO = 1;
        ptr_TP2802 = TP2853C_DataSet;
        break;

    default:
        ptr_TP2802 = TP2853C_DataSet;; //default
        printf("unknown ID\r\n");
        break;
    }

    Set_PLL_StartUp();

    I2CDeviceSet(TVII2CAddress, ptr_TP2802 );


//	if(chip > TP2802D)	I2CDeviceSet(TVII2CAddress, TP2822_PTZ_DataSet );
    if(flag_AUDIO)	I2CDeviceSet(TVII2CAddress, TP2834_Audio_DataSet );
    if(flag_CVBS)
    {
        tp28xx_byte_write(TVII2CAddress, 0x7E, 0x0F);   //TX channel enable
        tp28xx_byte_write(TVII2CAddress, 0xB9, 0x0F);   //RX channel enable
    }
    /*
    	if(ManVidRes==NoDet){
    		printf("Auto resolution detection...\r\n");
    		Sel_ch=Scan_Channels();
    	}
    	else {
    		printf("\r\nSet resolution for all channel\r\n");
    		Set_VidRes(ManVidRes);
    	}
    */
    Set_VidRes(R1080P25);
    CurRes=RES1920X1080;
//	Write_AllRegPage(0x06,7,1);		//soft reset for all channel


    for( i = 0; i < CH_PER_CHIP; i++)
    {
        //loss[i] = 1;
        //locked[i] = 0;
        count[i] = 0;
        state[i] = VIDEO_UNPLUG;
        std[i] = TVI;
        mode[i] = NoDet;
    }

}
void ConvertData(unsigned char dat)
{
    unsigned char i;

    PTZ_pelco=0;

    for(i = 0; i < 8; i++)
    {
        PTZ_pelco <<= 3;

        if(0x80 & dat) PTZ_pelco |= 0x06;
        else PTZ_pelco |= 0x04;

        dat <<= 1;
    }
}
//
//ch 0~3 for channel select
//bank 0x00-> 1&2 lines 0x10-> 3&4 lines
void PTZ_SetData(tByte ch,tByte bank)
{

    tp28xx_byte_write(TVII2CAddress, 0x40, bank); //data buffer bank1 switch for 282x

    //line1
    tp28xx_byte_write(TVII2CAddress, 0x7f + ch*2 , PTZ_buf[0]);
    tp28xx_byte_write(TVII2CAddress, 0x56 + ch*10 , PTZ_buf[1]);
    tp28xx_byte_write(TVII2CAddress, 0x57 + ch*10 , PTZ_buf[2]);
    tp28xx_byte_write(TVII2CAddress, 0x58 + ch*10 , PTZ_buf[3]);
    tp28xx_byte_write(TVII2CAddress, 0x59 + ch*10 , PTZ_buf[4]);
    tp28xx_byte_write(TVII2CAddress, 0x5A + ch*10 , PTZ_buf[5]);

    //line2
    tp28xx_byte_write(TVII2CAddress, 0x80 + ch*2 , PTZ_buf[6]);
    tp28xx_byte_write(TVII2CAddress, 0x5B + ch*10 , PTZ_buf[7]);
    tp28xx_byte_write(TVII2CAddress, 0x5C + ch*10 , PTZ_buf[8]);
    tp28xx_byte_write(TVII2CAddress, 0x5D + ch*10 , PTZ_buf[9]);
    tp28xx_byte_write(TVII2CAddress, 0x5E + ch*10 , PTZ_buf[10]);
    tp28xx_byte_write(TVII2CAddress, 0x5F + ch*10 , PTZ_buf[11]);
}
//////////////////////////////////////////////////////////////////////////////
//PTZ TX
//////////////////////////////////////////////////////////////////////////////
void MonTX(tByte cmd, tByte data1, tByte data2, tByte data3, tByte data4, tByte ch)
{
    unsigned char sum,tmp,index;
    unsigned char i;

    tcByte PTZ_bank[4]= {0x00,0x00,0x10,0x10};
    tcByte PTZ_reg[4][7]=
    {
        {0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8},
        {0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0},
        {0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8},
        {0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0}
    };
    tcByte PTZ_dat[6][7]=
    {
        {0x0b,0x0c,0x00,0x00,0x19,0x78,0x21}, //TVI1.0
        {0x0b,0x0c,0x00,0x00,0x33,0xf0,0x21}, //TVI2.0
        {0x0e,0x0f,0x10,0x11,0x64,0xf0,0x17}, //A2.0 for 2833B
        {0x0e,0x0f,0x10,0x11,0x24,0xf0,0x57}, //A2.0 for 2833C
        {0x0e,0x0f,0x00,0x00,0x24,0xe0,0xef}, //A1.0 for 2833C
        {0x0f,0x10,0x00,0x00,0x48,0xf0,0x6f}  //960H for 2833C
    };

    for(i = 0; i < 12; i++)
    {
        PTZ_buf[i]=0x00; //clear buf
    }

    if(0xff == cmd) //HDA 1080p
    {
        if(TP2853C == chip)
            index=3;
        else
            index=2;

        ConvertData(data1);
        PTZ_buf[3]= (PTZ_pelco>>16)&0xff;
        PTZ_buf[4]= (PTZ_pelco>>8)&0xff;
        PTZ_buf[5]= (PTZ_pelco)&0xff;
        ConvertData(data2);
        PTZ_buf[9]= (PTZ_pelco>>16)&0xff;
        PTZ_buf[10]= (PTZ_pelco>>8)&0xff;
        PTZ_buf[11]= (PTZ_pelco)&0xff;

    }
    else if(0xfe == cmd ||0xfd == cmd) //HDA 720p/960H
    {
        if(0xfe == cmd ) index=4;
        if(0xfd == cmd ) index=5;
        ConvertData(data1);
        PTZ_buf[0]= (PTZ_pelco>>16)&0xff;
        PTZ_buf[1]= (PTZ_pelco>>8)&0xff;
        PTZ_buf[2]= (PTZ_pelco)&0xff;
        ConvertData(data2);
        PTZ_buf[3]= (PTZ_pelco>>16)&0xff;
        PTZ_buf[4]= (PTZ_pelco>>8)&0xff;
        PTZ_buf[5]= (PTZ_pelco)&0xff;
        ConvertData(data3);
        PTZ_buf[6]= (PTZ_pelco>>16)&0xff;
        PTZ_buf[7]= (PTZ_pelco>>8)&0xff;
        PTZ_buf[8]= (PTZ_pelco)&0xff;
        ConvertData(data4);
        PTZ_buf[9]= (PTZ_pelco>>16)&0xff;
        PTZ_buf[10]= (PTZ_pelco>>8)&0xff;
        PTZ_buf[11]= (PTZ_pelco)&0xff;
    }
    else
    {

        tmp = tp28xx_byte_read(TVII2CAddress, 0xf5); //check TVI 1 or 2
        if( (tmp >>ch) & 0x01)
        {
            index=1;
        }
        else
        {
            index=0;
        }
        sum = 0xb5;
        sum = sum + cmd + data1 + data2 + data3 + data4;
        PTZ_buf[1]=0x02;
        PTZ_buf[2]=0xB5;
        PTZ_buf[4]=cmd;
        PTZ_buf[5]=data1;

        PTZ_buf[7]=0x02;
        PTZ_buf[8]=data2;
        PTZ_buf[9]=data3;
        PTZ_buf[10]=data4;
        PTZ_buf[11]=sum;
    }

    tmp = tp28xx_byte_read(TVII2CAddress, 0x40);
    tmp &= 0xaf;
    tmp |=PTZ_bank[ch];
    tp28xx_byte_write(TVII2CAddress, 0x40, tmp); //PTZ bank switch for 282x

    for(i = 0; i < 7; i++)
    {
        tp28xx_byte_write(TVII2CAddress, PTZ_reg[ch][i], PTZ_dat[index][i]);
    }

    // TX disable
    tp28xx_byte_write(TVII2CAddress, 0xBB, tp28xx_byte_read(TVII2CAddress,0xBB) & ~(0x01<<ch));

    PTZ_SetData(ch,0x00); //line1&2

    if(0xff==cmd)
    {
        ConvertData(data3);
        PTZ_buf[3]= (PTZ_pelco>>16)&0xff;
        PTZ_buf[4]= (PTZ_pelco>>8)&0xff;
        PTZ_buf[5]= (PTZ_pelco)&0xff;
        ConvertData(data4);
        PTZ_buf[9]= (PTZ_pelco>>16)&0xff;
        PTZ_buf[10]= (PTZ_pelco>>8)&0xff;
        PTZ_buf[11]= (PTZ_pelco)&0xff;

        PTZ_SetData(ch, 0x10); //3&4 lines
    }
    // TX enable
    tp28xx_byte_write(TVII2CAddress, 0xBB, (0x01<<ch));

}
