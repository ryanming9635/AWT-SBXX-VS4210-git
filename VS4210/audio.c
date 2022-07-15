//============ Audio ====================================================
#include "VXIS_Extern_Define.h"

#include "tpinc/Device_Rx.h"
#include "tpinc/TP2802.h"
#include "stdlib.h"
#include "VX_swi2c.h"
#include "VS4210.h"

//#include "Config.h"
//#include "typedefs.h"
#include "main.h"
//#include "i2c.h"
#include "printf.h"
#include "etc_eep.h"
#include "keyremo.h"
#include "audio.h"
#include "VX_OSD_Simple.h"

/*
#include "Config.h"

#include	"reg.h"
#include	"typedefs.h"
#include	"i2c.h"
#include	"main.h"
#include    "dispinfo.h"
#include	"osdmenu.h"
#include	"debug.h"
#include    "printf.h"
#include	"tw88.h"
#include	"etc_eep.h"
#include 	"audio.h"
#include 	"HS_IOdef.h"
#include "HS_keyremo.h"
*/
BYTE AudioVol = 50;
BYTE PCAudioVol = 50;
// extern CODE struct RegisterInfo UserRange;
CODE struct RegisterInfo UserRange = {0, 100, 50};
CODE struct RegisterInfo UserRangeBacklight = {0, 100, 100};

extern BYTE PowerFlag;
extern struct RegisterInfo AudioVolRange;
extern BYTE TriggerFlag;

#if 0
					AudioBalance = 50, 
					AudioBass = 50,
					AudioTreble = 46;
					//AudioEffect = 0;
		bit   TVTunerAudioActive=0;
static  bit   AudioMuteOn=0;

CODE struct RegisterInfo MSPVolRange=     {0x50, 0x7d,0x66};				
CODE struct RegisterInfo MSPBalanceRange= {-127,  127,   0};				
CODE struct RegisterInfo MSPBassRange=    { -12,   12,   0};				
CODE struct RegisterInfo MSPTrebleRange=  { -12,   15,   2};				

BYTE ResetMSP(void)
{
	BYTE mspdata[3];

#ifdef DEBUG_AUDIO
	dPrintf("\r\n++(ResetMSP)\r\nTVVol:%d", (WORD)AudioVol);
#endif

	AudioVol     = GetAudioVolEE();
	AudioBalance = GetAudioBalanceEE();
	AudioBass    = GetAudioBassEE();
	AudioTreble  = GetAudioTrebleEE();
	//AudioEffect  = GetAudioEffectEE();

	mspdata[0] = 0;
	mspdata[1] = 0x80;
	mspdata[2] = 0;
	WriteI2Cn_(MSPI2CAddress, mspdata, 3);

	mspdata[0] = 0;
	mspdata[1] = 0;
	mspdata[2] = 0;
	WriteI2Cn_(MSPI2CAddress, mspdata, 3);

#ifdef DEBUG_AUDIO
	dPuts("\r\n__(ResetMSP)");
#endif
	return 0;
}

void SetMSPVol(void)
{
	BYTE voldata[5];
	int vol;

	Mapping1( AudioVol, &UserRange, &vol, &MSPVolRange );

	voldata[0] = 0x12;
	voldata[1] = 0;
	voldata[2] = 0;
	voldata[3] = (BYTE)vol;
	voldata[4] = 0;
	WriteI2Cn_(MSPI2CAddress, voldata, 5);	//LJY010202

#ifdef DEBUG_AUDIO
	dPrintf("\r\n++(SetMSPVol)MSPVol:0x%x, AudioVol:%d__", (WORD)vol, (WORD)AudioVol);
#endif
}

void SetMSPBalance(void)
{
	BYTE valdata[5];
	int val;

	Mapping1( AudioBalance, &UserRange, &val, &MSPBalanceRange );
	
	valdata[0] = 0x12;
	valdata[1] = 0;		
	valdata[2] = 1;
	valdata[3] = (BYTE)val;
	valdata[4] = 0;
	WriteI2Cn_(MSPI2CAddress, valdata, 5);

#ifdef DEBUG_AUDIO
	dPrintf("\r\n++(SetMSPBalance)MSPBal:0x%x, AudioBalance:%d__", (WORD)val, (WORD)AudioBalance);
#endif
}

void SetMSPBass(void)
{
	BYTE valdata[5];
	int val;

	Mapping1( AudioBass, &UserRange, &val, &MSPBassRange );
	
	valdata[0] = 0x12;
	valdata[1] = 0;
	valdata[2] = 2;
	valdata[3] = (BYTE)val;
	valdata[4] = 0;
	WriteI2Cn_(MSPI2CAddress, valdata, 5);

#ifdef DEBUG_AUDIO
	dPrintf("\r\n++(SetMSPBass)MSPBass:0x%x, AudioBass:%d__", (WORD)val, (WORD)AudioBass);
#endif
}

void SetMSPTreble(void)
{
	BYTE valdata[5];
	int val;

	Mapping1( AudioTreble, &UserRange, &val, &MSPTrebleRange );
	
	valdata[0] = 0x12;
	valdata[1] = 0;
	valdata[2] = 3;
	valdata[3] = (BYTE)val;
	valdata[4] = 0;
	WriteI2Cn_(MSPI2CAddress, valdata, 5);

#ifdef DEBUG_AUDIO
	dPrintf("\r\n++(SetMSPTreble)MSPTreble:0x%x, AudioTreble:%d__", (WORD)val, (WORD)AudioTreble);
#endif
}

void AudioOff(void)
{
	BYTE mspdata[5];

#ifdef DEBUG_AUDIO
	dPuts("\r\n++(AudioOff)--");
#endif

	mspdata[0] = 0x12;
	mspdata[1] = 0x00;
	mspdata[2] = 0x00;
	mspdata[3] = 0xff;
	mspdata[4] = 0x00;
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);
}

#define END_OF_LOOP 0xff

BYTE AudioAutoStdDetection(void)
{
	BYTE mspdata[5], loop=0;
	WORD ret;

	mspdata[0] = 0x10;
	mspdata[1] = 0x00;
	mspdata[2] = 0x20;				// Standard selection
	mspdata[3] = 0x00;
	mspdata[4] = 0x01;				// Auto Std Detection
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);
	do {
		mspdata[0] = 0x00;
		mspdata[1] = 0x7E;
		I2CReadByteMSP(MSPI2CAddress, 0x11, mspdata, 2);
		if( loop==END_OF_LOOP )
			break;
		loop++;
		ret = mspdata[0];
		ret = ( ret << 8 ) | mspdata[1];
	} while( ret > 0x7ff );
#ifdef DEBUG_AUDIO
	dPrintf("\r\n(AudioAutoStdDetection)Auto Detection:0x%x", (WORD)ret );
#endif

	if( loop==END_OF_LOOP )		ret = 0;

	return ret;
}

void ExtAudio(void)
{
	BYTE mspdata[5], loop=0;
	WORD ret;

	AudioOff();

	mspdata[0] = 0x10;
	mspdata[1] = 0x00;
	mspdata[2] = 0x30;				// MODUS register: Automatic-on
	mspdata[3] = 0x20;
	mspdata[4] = 0x03;
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);

	mspdata[0] = 0x12;
	mspdata[1] = 0x00;
	mspdata[2] = 0x13;				// SCART switch
	mspdata[3] = 0x00;				
	mspdata[4] = 0x00;				
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);

	mspdata[0] = 0x12;
	mspdata[1] = 0x00;
	mspdata[2] = 0x08;				// source select
	mspdata[3] = 0x02;				// SCART input
	mspdata[4] = 0x20;				// stereo(transparent)
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);

	mspdata[0] = 0x12;
	mspdata[1] = 0x00;
	mspdata[2] = 0x0d;				// SCART input prescale
	mspdata[3] = 0x50;
	mspdata[4] = 0x00;
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);
	ret = AudioAutoStdDetection();

#ifdef DEBUG_AUDIO
	if( !ret ) {
		dPuts("\r\n--Fail to Audio AutoDetect");
	}
#endif
	AudioOn();			//SetMSPVol();
}

#ifdef SUPPORT_PC
/*
void PCAudio(void)
{
	BYTE mspdata[5], loop=0;
	WORD ret;

	AudioOff();

	mspdata[0] = 0x10;
	mspdata[1] = 0x00;
	mspdata[2] = 0x30;				// MODUS register: Automatic-on
	mspdata[3] = 0x20;
	mspdata[4] = 0x03;
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);

	mspdata[0] = 0x12;
	mspdata[1] = 0x00;
	mspdata[2] = 0x13;				// SCART switch
	mspdata[3] = 0x02;				// SCART2->DSP
	mspdata[4] = 0x00;				
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);

	mspdata[0] = 0x12;
	mspdata[1] = 0x00;
	mspdata[2] = 0x08;				// source select
	mspdata[3] = 0x02;				// SCART input
	mspdata[4] = 0x20;				// stereo(transparent)
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);

	mspdata[0] = 0x12;
	mspdata[1] = 0x00;
	mspdata[2] = 0x0d;				// SCART input prescale
	mspdata[3] = 0x50;
	mspdata[4] = 0x00;
	WriteI2Cn_(MSPI2CAddress, mspdata, 5);

	ret = AudioAutoStdDetection();

	#ifdef DEBUG_AUDIO
	if( !ret ) {
		Puts("\r\n--Fail to Audio AutoDetect");
	}
	#endif
	AudioOn();			//SetMSPVol();
}
*/
#endif

BYTE GetAudioVol(void)
{
	return AudioVol;
}
#endif
void AUDIO_SELECT(BYTE sel)
{
	AudioMute(ON);

	switch (sel)
	{
	case VS4210_LOOP_MODE0_w0: /// CAMA_AUD:
	case VS4210_LOOP_MODE10_123:
	case VS4210_LOOP_MODE8:
	case VS4210_LOOP_MODE12_12:
	case VS4210_LOOP_MODE12_13:
	case VS4210_LOOP_MODE8_123:
		AUD_SEL1(0);
		AUD_SEL2(0);
		break;

	case VS4210_LOOP_MODE0_w1: /// CAMB_AUD:
	case VS4210_LOOP_MODE12_21:
	case VS4210_LOOP_MODE12_23:
	case VS4210_LOOP_MODE8_213:
		AUD_SEL1(0);
		AUD_SEL2(1);
		break;

	case VS4210_LOOP_MODE0_w2: /// CAMC_AUD:
	case VS4210_LOOP_MODE0_w4: /// CAMD_AUD:

	case VS4210_LOOP_MODE12_31:
	case VS4210_LOOP_MODE12_32:
	case VS4210_LOOP_MODE8_312:
		AUD_SEL1(1);
		AUD_SEL2(1);
		break;

	case VS4210_LOOP_MODE0_w3: /// CAMR_AUD:
	case VS4210_LOOP_MODE1:
	case VS4210_LOOP_MODE2:
	case VS4210_LOOP_MODE9:
	case VS4210_LOOP_MODE12_34:
	case VS4210_LOOP_MODE12_14:
	case VS4210_LOOP_MODE12_42:
	case VS4210_LOOP_MODE12_43:
	case VS4210_LOOP_MODE12_24:
	case VS4210_LOOP_MODE8_412:
	case VS4210_LOOP_MODE8_124:
	case VS4210_LOOP_MODE8_134:
	case VS4210_LOOP_MODE8_214:
	case VS4210_LOOP_MODE8_234:
	case VS4210_LOOP_MODE8_314:
	case VS4210_LOOP_MODE8_324:
	case VS4210_LOOP_MODE10_412:
	case VS4210_LOOP_MODE10_413:
	case VS4210_LOOP_MODE10_423:
	case VS4210_LOOP_MODE8_413:
	case VS4210_LOOP_MODE8_423:
	case VS4210_LOOP_MODE9_412:
	case VS4210_LOOP_MODE12_41:
		AUD_SEL1(1);
		AUD_SEL2(0);
		break;

	default:
		AUD_SEL1(1); // CAMR_AUD:
		AUD_SEL2(1);
		break;
	}

	// AudioMute(OFF);
}

void AUDIO_SELECT_MUTE(BYTE sel)
{

#if (_DEBUG_MESSAGE_Audio == ON)
	Printf("(AUDIO_SELECT_MUTE=%02x)", (WORD)sel);
#endif

	AudioMute(ON);

	switch (sel)
	{
	case VS4210_LOOP_MODE0_w0: /// CAMA_AUD:
	case VS4210_LOOP_MODE10_123:
	case VS4210_LOOP_MODE8:
	case VS4210_LOOP_MODE12_12:
	case VS4210_LOOP_MODE12_13:
	case VS4210_LOOP_MODE8_123:
		AUD_SEL1(0);
		AUD_SEL2(1);
		break;

	case VS4210_LOOP_MODE0_w1: /// CAMB_AUD:
	case VS4210_LOOP_MODE12_21:
	case VS4210_LOOP_MODE12_23:
	case VS4210_LOOP_MODE8_213:
		AUD_SEL1(1);
		AUD_SEL2(0);
		break;

	case VS4210_LOOP_MODE0_w2: /// CAMC_AUD:
	case VS4210_LOOP_MODE0_w4: /// CAMD_AUD:

	case VS4210_LOOP_MODE12_31:
	case VS4210_LOOP_MODE12_32:
	case VS4210_LOOP_MODE8_312:
		AUD_SEL1(0);
		AUD_SEL2(0);
		break;

	case VS4210_LOOP_MODE0_w3: /// CAMR_AUD:
	case VS4210_LOOP_MODE1:
	case VS4210_LOOP_MODE2:
	case VS4210_LOOP_MODE9:
	case VS4210_LOOP_MODE12_34:
	case VS4210_LOOP_MODE12_14:
	case VS4210_LOOP_MODE12_41:
	case VS4210_LOOP_MODE12_42:
	case VS4210_LOOP_MODE12_43:
	case VS4210_LOOP_MODE12_24:
	case VS4210_LOOP_MODE8_412:
	case VS4210_LOOP_MODE8_124:
	case VS4210_LOOP_MODE8_134:
	case VS4210_LOOP_MODE8_214:
	case VS4210_LOOP_MODE8_234:
	case VS4210_LOOP_MODE8_314:
	case VS4210_LOOP_MODE8_324:
	case VS4210_LOOP_MODE10_412:
	case VS4210_LOOP_MODE10_413:
	case VS4210_LOOP_MODE10_423:
	case VS4210_LOOP_MODE8_413:
	case VS4210_LOOP_MODE8_423:
	case VS4210_LOOP_MODE9_412:
		AUD_SEL1(1);
		AUD_SEL2(1);
		break;

	default:
		AUD_SEL1(1); // CAMR_AUD:
		AUD_SEL2(1);
		break;
	}

	// AudioMute(OFF);
}

void AudioMute(bit mute)
{
	// WORD temp;
	// mute  0 Audio OFF		mute 1 Audio ON
	//	AUDIO_MUTE(mute);

	if (mute == ON)
	{
		//	SET_ALC122(0x02,(GET_ALC122(0x02)|0x8080));
		SET_ALC122(0x02, 0x8080);
	}
	else
	{
		// SET_ALC122(0x02,0x0F0F);
		ChangeVol(0);
	}

#if 0 /// def ALC106
	if(mute==0)
	{
		WriteALC106(0x02,0x9F9F);
		//WriteALC106(0x02,ReadALC106(0x02)||0x8080);
	}
	else
	{
		ChangeVol(0);
		//WriteALC106(0x02,ReadALC106(0x02)&&0x7F7F);
	}
#else // william-990729
/*
	if(mute==0)
		Audio_Mute=1;// 0;		//Audio ON
	else
		Audio_Mute=0;// 1;		//Audio OFF
		*/
#endif

#if 0
	if(mute==0)
		TW2835Cmd("\n\rmute 0\n\r");
	else
		TW2835Cmd("\n\rmute 1\n\r");
#endif
}
/*
BYTE GetAudioVolEE(void)
{
	return ReadEEP( EEP_AUDIOPVOL );
}

void SetAudioVolEE( BYTE vol )
{
	WriteEEP( EEP_AUDIOPVOL, vol );
}
*/

BYTE ChangeVol(char amount)
{

	WORD val = 0;
	int regv;
	BYTE AudioTemp; //,AudioTemp2;  //Pinchi disable for compiler warning
					//	WORD ALC106Volume;  //Pinchi disable for compiler warning

	// if(VGAAudio)
	//{
	// AudioTemp=PCAudioVol;
	// }
	// else
	//{
	AudioTemp = AudioVol;
	//}
	if (amount == UP)
	{
		if (AudioTemp < 100)
			AudioTemp += 1;
		else
			AudioTemp = 100;
	}
	else if (amount == DN)
	{
		if (AudioTemp > 1)
			AudioTemp -= 1;
		else
			AudioTemp = 0;
	}

	Mapping1(AudioTemp, &UserRange, &regv, &AudioVolRange);

	// Printf("\r\nAudio OSD val=%02x Regv=%02x",(WORD) AudioTemp,(WORD) regv);
	////�q�o�̼g�J��ƨ�Ȧs��
	val = regv;
	val <<= 8;
	val |= regv;
	//	val=((GET_ALC122(0x02))|val);
	SET_ALC122(0x02, val);

	//	WriteAD5110(AD5110_CMD_Write_Data_To_RDAC ,AD5110_setp[regv]);

	//		return regv;

	if ((AudioTemp == 0 || PowerFlag == 0) && (TriggerFlag == OFF))
		AudioMute(ON); // Audio_Mute = 0;		//�}���R��...... Abel 960122

		//	{
		//	AudioMute(1);	//Audio_Mute = 1;			//�����R��...... Abel 9601222
		//}

#if 0
	if(AudioTemp==0 || PowerFlag==0)
	 	AudioMute(0);	// Audio_Mute = 0;		//�}���R��...... Abel 960122
	else	
		{
	  	AudioMute(1);	//Audio_Mute = 1;			//�����R��...... Abel 9601222  	
		}
	if( amount>0 ) 
	{
		if( ((AudioTemp+amount) <= UserRange.Max) ) 
			chg=1;
	}
	else 
	{
		if( AudioTemp>UserRange.Min	) 
			chg=1;
	}

	if( chg ) 
	
	{
			AudioTemp += amount;
			
		//	if(AudioTemp==UserRange.Max) 
			  //VCOM=0xFD;			 
			//else
			 // VCOM=( (((WORD)AudioTemp*0xFD))/100);
	
			/*
			WriteI2C( TW88I2CAddress,0xff,0x01);
			
			WriteI2C( TW88I2CAddress,0xb4,VCOM);
		
			WriteI2C( TW88I2CAddress,0xff,0x00);   
		*/	
		   // if(AudioVol==0)
		//		Audio_Mute = 1; 				  //�}���R��...... Abel 960122
		//	  else		
		 // 	Audio_Mute = 0; 				  //�����R��...... Abel 9601222
	
			//#ifdef Hs_debug
			//Printf("\r\nVCOM=%x",(WORD)VCOM);  
			// #endif
		}
#endif
#if 0
		CCAP4H=120-AudioTemp;
#ifdef ALC106
		AudioTemp2=33-(AudioTemp/3);
		if(AudioTemp2>0x1f)
			AudioTemp2=0x1f;
		ALC106Volume=AudioTemp2;
		ALC106Volume=ALC106Volume<<8;
		ALC106Volume|=AudioTemp2;
		WriteALC106(0x02,ALC106Volume);
		Printf("\r\nALC106Volume=%x",(WORD)ALC106Volume);
#endif
#endif
	/*
		#ifdef Hs_debug
		Printf("\r\nAudioTemp=%x",(WORD)AudioTemp);
		#endif

		if(AudioTemp==0||PowerFlag==0)
			AudioMute(0);	// Audio_Mute = 0;		//�}���R��...... Abel 960122
		else
			AudioMute(1);
	*/
	//	SetAudioVolEE( AudioTemp );

	if (AudioVol != AudioTemp)
	{
		// Printf("\r\SetAudioVolEE=%x",(WORD)AudioTemp);
		AudioVol = AudioTemp;
		SetAudioVolEE(AudioTemp);
	}

	//	if(VGAAudio)
	//		return PCAudioVol;
	//	else
	return AudioVol;
}

#if 0
BYTE GetAudioBalance(void)
{
	return AudioBalance;
}

BYTE ChangeBalance(char amount)
{
	BYTE chg=0;

	if( amount>0 ) {
		if( ((AudioBalance+amount) <= UserRange.Max) ) {
			chg=1;
		}
	}
	else {
		if( AudioBalance>UserRange.Min  ) {
			chg=1;
		}
	}

	if( chg ) {
		AudioBalance += amount;
		SetAudioBalanceEE( AudioBalance );
		SetMSPBalance();
	}

	return AudioBalance;
}

BYTE GetAudioBass(void)
{
	return AudioBass;
}

BYTE ChangeBass(char amount)
{
	BYTE chg=0;

	if( amount>0 ) {
		if( ((AudioBass+amount) <= UserRange.Max) ) {
			chg = 1;
		}
	}
	else {
		if( AudioBass>UserRange.Min  ) {
			chg = 1;
		}
	}

	if( chg ) {
		AudioBass += amount;
		SetAudioBassEE( AudioBass );
		SetMSPBass();
	}

	return AudioBass;
}

BYTE GetAudioTreble(void)
{
	return AudioTreble;
}

BYTE ChangeTreble(char amount)
{
	BYTE chg=0;

	if( amount>0 ) {
		if( ((AudioTreble+amount) <= UserRange.Max) ) {
			chg = 1;
		}
	}
	else {
		if( AudioTreble>UserRange.Min  ) {
			chg = 1;
		}
	}

	if( chg ) {
		AudioTreble += amount;
		SetAudioTrebleEE( AudioTreble );
		SetMSPTreble();
	}

	return AudioTreble;
}

/*

BYTE GetAudioEffect(void)
{
	return AudioEffect;
}

BYTE ChangeAudioEffect(char amount)
{
	BYTE valdata[5];

	amount = 0;			//ignore
	AudioEffect = 0xff - AudioEffect;

	valdata[0] = 0x12;
	valdata[1] = 0;
	valdata[2] = 5;
	valdata[3] = AudioEffect;
	valdata[4] = 0;
	WriteI2Cn_(MSPI2CAddress, valdata, 5);

	SetAudioEffectEE( AudioEffect );
	return AudioEffect;
}
*/
void ToggleAudioMute(void)
{
	if( AudioMuteOn ) {
		AudioOn();			//SetMSPVol();
		AudioMuteOn = 0;
	}
	else {
		AudioOff();
		AudioMuteOn = 1;
	}
}

void CheckAndClearMuteOn(void)
{
	if( AudioMuteOn ) {
		AudioOn();			//SetMSPVol();
		AudioMuteOn = 0;
		ClearMuteInfo();
	}
}

/*void SetAudioMute(BYTE on )
{
		AudioMuteOn = on;
}
*/
BYTE IsAudioMuteOn(void)
{
	return (BYTE)AudioMuteOn;
}

void ResetAudioValue()
{
	SetAudioVolEE( 50 );
	SetAudioBalanceEE( 50 );
	SetAudioBassEE( 50 );
	SetAudioTrebleEE( 50 );
//	SetAudioEffectEE( 0 );

}
#endif
