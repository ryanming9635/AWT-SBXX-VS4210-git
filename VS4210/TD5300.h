//#define _XTAL_FREQ 16000000 
//extern BYTE buf[16];

//#define BYTE unsigned char
//#define WORD unsigned int
//#define CODE code
//#define IDATA idata

//#define	TRUE	1
//#define	FALSE	0

#define TruckHead 0
#define TruckTail1 1
#define TruckTail2 2
#define Bus 0
#define TruckWithoutTail 1
#define LEDG P4_0
#define LEDR P4_1
#define PHR_ 800
#define PVR_ 480//600
#define ON 1
#define OFF 0

//DTAT BYTE Dec[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//DATA BYTE buffer[16],DIN=0x00,DIN_Flag=0x00,DOUT=0x00;

//CODE BYTE Sensor0[15];
//CODE BYTE Sensor1[15];
//CODE BYTE Sensor2[15];
//CODE BYTE Sensor3[15];
//CODE BYTE Sensor4[15];
//CODE BYTE Sensor5[15];
//CODE BYTE Sensor_state[15];

//#define SB1_LED RB1
//#define SB2_LED RB2
//#define SB3_LED RB3
//#define SB4_LED RB4
#define SB_Temperature_High (26+40+13)  //79
#define SB_Pressure_Low  30
#define SB_Pressure_High 50

//BYTE x,y,z,c;

//bit DataArrive = 0;
//bit stop = 0;
//WORD Device_ID=0;
//ORD *Np;


#define TruckWithTail1 2
#define TruckWithTail2 4

#define TruckSegment10 0
#define TruckSegment11 1
#define TruckSegment12 2
#define TruckSegment13 3
#define TruckSegment14 4

#define TruckSegment20 0
#define TruckSegment21 1
#define TruckSegment22 2
#define TruckSegment23 3
#define TruckSegment24 4
#define TruckSegment25 7
#define TruckSegment26 8
#define TruckSegment27 9
#define TruckSegment28 10
#define TruckSegment29 11
#define TruckSegment2A 12
#define TruckSegment2B 13
#define TruckSegment2C 14
#define TruckSegment2D 15

#define TruckSegment30 0
#define TruckSegment31 1
#define TruckSegment32 2
#define TruckSegment33 3
#define TruckSegment34 4
#define TruckSegment35 7
#define TruckSegment36 8
#define TruckSegment37 9
#define TruckSegment38 10
#define TruckSegment39 11
#define TruckSegment3A 12
#define TruckSegment3B 13
#define TruckSegment3C 14
#define TruckSegment3D 15

#define AutoLinkFlag_NoAutoLink 0
#define AutoLinkFlag_AutoLink 1

#define TirePosition 0
#define TirePosition1 1
#define TirePosition2 2
#define TirePosition3 3
#define TirePosition4 4
#define TirePosition5 5
#define SB_MAX   6
#define EEP_SB_data_offset    0xDC//// 0xD0

//extern BYTE buf[6][15];

BYTE SetRelayLearning(BYTE);
BYTE ExitLearningMode(void);
BYTE SetTruckType(BYTE,BYTE,BYTE,BYTE,BYTE,BYTE);
BYTE SetSensorLearning(void);
BYTE GetTireData(BYTE);
BYTE GetTireDatawhole(BYTE,BYTE);
BYTE GetRelayStatus(void);
BYTE SetTireThreshold(BYTE,BYTE,BYTE,BYTE,BYTE);
BYTE GetMDID(void);
BYTE CalibratRF(void);
BYTE TestTXRFOn(void);
BYTE TestTXRFOff(void);
BYTE DisableRecon_RY(void);
BYTE SetAutoTX(BYTE);

void Write_TX(BYTE*);
BYTE Read_RX(void);
BYTE GetData(BYTE *);
BYTE Read_Relay_Data(void);
BYTE Check_TPMS(void);
BYTE Find_TPMS_DATA(BYTE*,BYTE,BYTE);
BYTE Order_Sensor_Data(void);
BYTE AsciiToData(BYTE);
BYTE* DataToAscii(BYTE );
    
BYTE Swap_data(BYTE*,BYTE*);
BYTE Check_tire_sensors(BYTE);
BYTE Get_Tire_Data(BYTE,BYTE,BYTE,BYTE,BYTE);
BYTE Get_Relay_Data(BYTE,BYTE);
BYTE Check_Tire_Data(BYTE*,BYTE);
BYTE CHARTOMUN(BYTE);

void Check_Data(BYTE,BYTE*,BYTE);
void Output(BYTE Select,BYTE d);
BYTE Input(BYTE Select);
void TPMS_redraw(BYTE);

void Display_TPMS_info(BYTE);
void Draw_CAR(void);
void Init_TPMS_OSD(void) ;
void Output2(BYTE num,BYTE* addr);
BYTE TPMS_Keyin_Mode(BYTE);
void TPMS_Change_Mode(BYTE );
void TPMS_Read_eeprom(void);
void TPMS_Write_eeprom(void);
void TPMS_Default_Setting(void);
//extern void RS2_tx(BYTE);

extern BYTE GetKey(BYTE repeat);
extern void ShowOSDWindow_TW88(BYTE winno, BYTE onoff);
extern void WaitEndofDisplayArea(void);
extern void SetDisplayedOSD( BYTE newd );
extern void InitOSDWindow(BYTE *ptr);
extern void ClearDataCnt(WORD addr, BYTE cnt);
extern void DrawAttrCnt(WORD addr, BYTE color, BYTE cnt);
extern void PowerLED(BYTE flag);
extern BYTE Asc1Bin(BYTE asc);
extern  void LoadEEPROM (void);

/*
extern CODE BYTE Temp[4];
extern CODE BYTE PSI[4];
extern CODE BYTE Tire_Green[4];
extern CODE BYTE Tire_Red[4];
extern CODE BYTE Tire_None[4];
extern CODE BYTE Cube0[4];
extern CODE BYTE Cube1[4];
extern CODE BYTE Cube2[4];
extern CODE BYTE Cube3[4];
extern CODE BYTE Cube4[4];
extern CODE BYTE Cube5[4];
extern CODE BYTE Cube6[4];
extern CODE BYTE Cube7[4];
extern CODE BYTE Cube8[4];
extern CODE BYTE Low_Battery[4];
extern CODE BYTE Empty[4];
extern CODE BYTE Tire_Yellow[4];
extern CODE BYTE R_arrow[4];
extern CODE BYTE L_arrow[4];
*/
//extern DATA BYTE DVR_buf[30];
