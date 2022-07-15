// *****************************************************
// Company : Techpoint Inc
// $Id: I2c.h 16792 2013-10-17 02:08:40Z SJ $
// $Author: SJ $
// $Date: 2013-10-17 18:08:40 -0800 (Thu, 17 Oct 2013) $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef __Coaxitron_H__
#define __Coaxitron_H__

typedef struct _PelcoD_format 
{
	BYTE sync;
	BYTE addr;
	BYTE cmd1;
	BYTE cmd2;
	BYTE data1;
	BYTE data2;
	BYTE chksum;
}PelcoD_format;

typedef struct _PelcoD_cmd
{
	BYTE cmd1;
	BYTE cmd2;
	BYTE data1;
	BYTE data2;
	
}PelcoD_cmd;

enum
{
	no_cmd = 0,
	CamOn,
	CamOff,
	AutoScan,
	ManScan,
	IRS_close,
	IRS_open,
	Focus_near,
	Focus_far,
	Zoom_wide,
	Zoom_tele,
	PTZ_down,
	PTZ_up,
	PTZ_left,
	PTZ_right,
	SP_max
};

void Tx_RS485_Byte(BYTE dat);

void Set_CAM_addr(BYTE addr);

void Send_PelcoD_data_stream(BYTE commd);
void Send_PelcoD_commend(PelcoD_cmd *PD_cmd);



PelcoD_cmd *PTZ_command(BYTE cmd);








#endif

