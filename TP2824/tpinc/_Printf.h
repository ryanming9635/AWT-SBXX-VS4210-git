// *****************************************************
// Company : Techpoint Inc
// $Id: Printf.h 16054 2013-10-17 21:05:52Z SJ $
// $Author: SJ $
// $Date: 2013-10-17 14:05:52 -0700 (Thu, 17 Oct 2013) $
// $Revision: 00 $
// $Log:  $
// ******************************************************

#ifndef __PRINTF__
#define __PRINTF__

#include "Config.h"
//#include "typedefs.h"
#include "reg.h"

//#define _outchar(c) while(1) { if( !RS_Xbusy ) { SBUF = c; RS_Xbusy=1; break; } }
//#define _outchar(c)  RS_tx(c);

#define Putch(c) _outchar(c)
#ifndef NOSERIAL

 void Printf ( const char  *fmt, ... );
 void Puts (  char *ptr );
 
#else

 #undef  DEBUG
 #define Printf	    ;//
 #define Puts   	;//

#endif


#ifndef DEBUG

 #define dPuts(a)	;//
 #define wPuts(a)	;//
 #define ePuts(a)	;//

#else

 void dPrintf( const char CODE_P *fmt, ... );
 void wPrintf( const char CODE_P *fmt, ... );
 void ePrintf( const char CODE_P *fmt, ... );

 void dPuts( CODE_P char *ptr );
 void wPuts( CODE_P char *ptr );
 void ePuts( CODE_P char *ptr );

#endif	// DEBUG

BYTE Getch(void);
BYTE Getche(void);


#endif	//__PRINTF__
