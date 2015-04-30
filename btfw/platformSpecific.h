/*****************************************************************************/
/************************Flaicomm CopyRight 2010.*****************************/ 
/*****************************************************************************/


#ifndef _PLATFORM_SPECIFIC_H
#define _PLATFORM_SPECIFIC_H

#include "ubcspcfg.h"

#define DEBUG_PRINTF
#ifdef DEBUG_PRINTF
#define DEBUG printf
#else
#define DEBUG(x)
#endif

/*send a byte to uart */
void put_uart (uint8 ch);
/*get a byte from uart*/
uint8 get_uart (uint8 *ch);
/*initialise the uart*/
void InitialiseUart(void);
/*delay 1ms*/
void Delay1ms(uint32 x);
/*open a dfu file*/
uint32 PrepareFile();
/*close a dfu file*/
void UnPrePareFile();
/*read a packet from the dfu file*/
uint16 ReadDfuPacket( uint8* address, uint16 readSize,uint16 block );

//void DEBUG("\n");
#endif
