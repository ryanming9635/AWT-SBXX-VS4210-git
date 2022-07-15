#ifndef __REGMAP__
#define __REGMAP__

//-----------------------------------------------------------------------------
//				Individual Decoder Registers 
//-----------------------------------------------------------------------------

#define	MCU_STOP_MODE	0xc8	// bit 7 is mcu stop, bit5 is MCU debug

#define	LAD0			0xc9	// Low speed AD channel 0
#define LAD1			0xca	// Low speed AD channel 1

#define	REG_PAGE		0xff
#define STATUS0			0xd0
#define STATUS1			0xd1
#define IRQ_EN1			0xd2
#define IRQ_EN0			0xd3

//-----------------------------------------------------------------------------
//				Individual Decoder Registers 
//-----------------------------------------------------------------------------

#define DECODERADDRESS	TW88I2CAddress

#define CVFMT			0x1e	// Component Video Format
#define FBSHIFT			0x38	// RGB Anti-Alias Filter/FB Shift Level
#define SHUE			0x3e	// RGB Hue
#define SCONTRAST		0x3b	// RGB Contrast
#define SBRIGHT			0x3a	// RGB Brightness
#define SCbGAIN			0x3c	// RGB Cb Gain
#define SCrGAIN			0x3d	// RGB Cb Gain
#define RGBCONTROL		0x36	// RGB Powerdown..
#define RGBMISC			0x39
#define FBCONTRL		0x3F

//#define OSD_ADJ			0xce
#define PNLSHARPNESS	0x78	// Sharpness Control
#define OSDWINSTATUS	0x95
//-----------------------------------------------------------------------------
//				Common Registers
//-----------------------------------------------------------------------------
#define CSTATUS			0x01	// Chip Status
#define INFORM			0x02	// Input Format
#define ACNTL			0x06	// Analog Control
#define CROP_HI			0x07	// Cropping High
#define VDELAY_LO		0x08	// Vertical Delay Low
#define VACTIVE_LO		0x09	// Vertical Active Low
#define HDELAY_LO		0x0a	// Horizontal Delay Low
#define HACTIVE_LO		0x0b	// Horizontal Active Low
#define CNTRL1			0x0c	// Control 1

#define BRIGHT			0x10	// Brightness Control
#define CONTRAST		0x11	// Contrast Control
#define SHARPNESS		0x12	// Sharpness Control
#define SAT_U			0x13	// Chroma(U) Gain
#define SAT_V			0x14	// Chroma(V) Gain
#define HUE				0x15	// Hue Conrol
#define V_PEAKING		0x17	// Vertical Peaking

#define CC_STATUS		0x1a	// CC/EDS Status
#define CC_DATA			0x1b	// CC/EDS Data
#define SDT				0x1c	// Standard Selection
#define SDTR			0x1d	// Standard Recognition

#define V_CONTROL2		0x29	// Vertical Control II
#define HFILTER			0x2c
#define MISC1			0x2d	// Miscellaneous Control 1
#define MISC2			0x2e	// Miscellaneous Control 2
#define MISC3			0x2f	// Miscellaneous Control 2

#define VBI_CNTL2		0x34	// VBI control 2 - WSSEN
#define CC_ODDLINE		0x35	// CC Odd Line

#define LADC0			0xc9	//LADC Channel 0 Input Value 
#define LADC1			0xca	//LADC Channel 1 Input Value 

#define WriteDecoder(index, val) 		WriteTW88( index, val )
#define ReadDecoder(index)  			ReadTW88( index )

#endif // __REGMAP__

