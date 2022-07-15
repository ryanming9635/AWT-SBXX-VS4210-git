// *****************************************************
// Company : Techpoint Inc
// $Id: I2c.h 16792 2013-10-17 02:08:40Z SJ $
// $Author: SJ $
// $Date: 2013-10-17 18:08:40 -0800 (Thu, 17 Oct 2013) $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef __Device_Rx_H__
#define __Device_Rx_H__


void Set_THS8200_720P();
void Set_THS8200_1080i();
void Set_THS8200_1080P30F();
void Set_SII9022();


void SetDevicesFor1080P30();
void SetDvicesFor720P60();
void SetDvicesFor720P50();

void Init_RegSet_For_Devices();


#endif


