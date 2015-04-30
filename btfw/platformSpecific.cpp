/*****************************************************************************/
/************************Flaicomm CopyRight 2010.*****************************/ 
/*****************************************************************************/


#include "platformSpecific.h"
#include "SerialPort.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>


FILE *fp;

SerialPort mSerialPort;

void put_uart (uint8 ch)
{
   mSerialPort.send((const char*)&ch,1);
}

void put_stream (uint8* stream, uint16 length)//eshin
{
    mSerialPort.send((const char*)stream,length);
}


uint8 get_uart (uint8 *ch)
{
    static uint8 fifo_buffer[256];

    static uint8
        read_index = 0,
        write_index = 0;

    unsigned long	read;

    // read the UART in large chunks
    if (read_index == write_index)
    {
        read_index = 0;
        write_index = 0;

        // read 254 octets
        read=	mSerialPort.recv((char*)fifo_buffer,254);
	
        if (read)
        {
            //printf("read=[%d]\n",read);
			
            write_index += (uint8) read;

            // read the first octet returned

            *ch = fifo_buffer[read_index ++];

            	//		printf (">%02X ", *ch);

            if (*ch == 0xc0)
            {
                //				printf ("\n");
            }

            return 1;
        }
    }
    else
    {
        // read the next octet in the fifo_buffer

        *ch = fifo_buffer[read_index ++];

        		//printf (">%02X ", *ch);

        if (*ch == 0xc0)
        {
            	//		printf ("\n");
        }

        return 1;
    }
    //printf("revc null\n");
    return 0;
}


static unsigned char recvBufHead[] = 
{
    0xc0,0x40,0x41,0x00,0x7e,0xda,0xdc,0xed,0xed,0xa9,0x7a,0xc0
};


bool IsRecvPacketHead(unsigned char* buf,int size)
{
	if(size<sizeof(recvBufHead))
	{
		return false;
	}

	int i=0;
	for(i=0;i<sizeof(recvBufHead);i++)
	{
		if(buf[i]!=recvBufHead[i])
		{
			return false;
		}
	}

	return true;
	
}

bool CheckRecvPacketHead(unsigned char* buf,int size)
{
	int i=0;
	for(i=0;i<size;i++)
	{
		if(buf[i]==recvBufHead[0])
		{
			if(IsRecvPacketHead(buf+i,size-i)==true)
			{
				return true;
			}
		}
	}

	return false;
}

void InitialiseUart(void)
{
		/*
	 	if(mSerialPort.open(1))
	 	{
	 		printf("SerialPort open  COM OK\n");
		//	mSerialPort.set_attribute(115200,CHECK_NONE_UPDATE);
		//	mSerialPort.set_block(false);
	 		mSerialPort.set_attribute(115200);
			
			printf(" Send mesg\n"); 		
	 		mSerialPort.send("AT#PK\r\n",7);
	 		sleep(1);
	 		char buf[64];
                        memset(buf,0,64);
                        mSerialPort.recv(buf,64);

                        printf("recv data:\n");
               		 for(int i=0;i<64;i++)
               		 {
                       	   printf("%02x",buf[i]);
               		 }
               		 printf("\n");
			
		}
		

		mSerialPort.close();
		sleep(1);

		mSerialPort.open(1);
		mSerialPort.set_attribute(115200,CHECK_NONE_UPDATE);
		*/

		/*
		mSerialPort.open(1);
		char buf[64];
	 	while(1)
	 	{
	 			memset(buf,0,64);
	 			mSerialPort.recv(buf,64);
	 			
	 			printf("recv data:\n");
    				for(int i=0;i<64;i++)
    				{
    					printf("%02x",buf[i]);
    				}
    				printf("\n");
	 			
	 			mSerialPort.send("AT#PK\r\n",7);
	 			Delay1ms(1);
	 	}*/


		/*
              mSerialPort.open(1);
                
                
		//mSerialPort.set_bonovo_bluetooth_power(0);

		//Delay1ms(1000);
		//mSerialPort.set_bonovo_bluetooth_power(1);
		
                char buf[64];
                int i=0;
                while(i<10)
                {
                                i++;
                                memset(buf,0,64);
                                mSerialPort.recv(buf,64);
                                
                                printf("recv data:\n");
                                for(int i=0;i<64;i++)
                                {
                                        printf("%02x",buf[i]);
                                }
                                printf("\n");
                                
                                mSerialPort.send("AT#PK\r\n",7);
                                Delay1ms(1);
                }

                mSerialPort.set_bonovo_update_fw();
		printf("send data 0xC0....................\n");
                Delay1ms(10000);
		unsigned char senddata[1];
		senddata[0]=0xC0;
		mSerialPort.send((const char*)senddata,1);
		
		printf("Delay1ms 10..........................\n");

		Delay1ms(10000);
		*/
		


		/*
		mSerialPort.open(1,false);
		
	
	       mSerialPort.set_bonovo_bluetooth_power(0);

              Delay1ms(5000);
			  
              mSerialPort.set_bonovo_bluetooth_power(1);
             

		  
		char buf[64];
		int i=0;
	 	while(i<10)
	 	{
	 			i++;
	 			memset(buf,0,64);
	 			mSerialPort.recv(buf,64);
	 			
	 			printf("recv data:\n");
    				for(int i=0;i<64;i++)
    				{
    					printf("%02x",buf[i]);
    				}
    				printf("\n");
	 			
	 			mSerialPort.send("AT#PK\r\n",7);
	 			Delay1ms(1);
	 	}

		mSerialPort.close();

		mSerialPort.open(1,true);

		
		Delay1ms(1000);

		mSerialPort.set_bonovo_update_fw();

		Delay1ms(3000);*/
		

		mSerialPort.open(1,false);
		
	
	       mSerialPort.set_bonovo_bluetooth_power(0);

              Delay1ms(5000);
			  
              mSerialPort.set_bonovo_bluetooth_power(1);

		#define RECVBUFLEN	512
		
		unsigned char buf[RECVBUFLEN];
		
		while(true)
	 	{
	 			memset(buf,0,RECVBUFLEN);
	 			mSerialPort.recv((char*)buf,RECVBUFLEN);

				
	 			printf("recv data:\n");
    				for(int i=0;i<18;i++)
    				{
    					printf("%02x",buf[i]);
    				}
    				printf("\n");

				if(CheckRecvPacketHead(buf,RECVBUFLEN)==true)
				{
					break;
				}
				
	 			
	 			mSerialPort.send("AT#PK\r\n",7);
	 			Delay1ms(1000);
				
				
	 	}

		mSerialPort.close();

		mSerialPort.open(1,true);

		
		Delay1ms(1000);

		mSerialPort.set_bonovo_update_fw();

		Delay1ms(3000);
	 	
}


void Delay1ms(uint32 x)
{
   // sleep(x);    
    usleep(1000 * x);

};



uint32  PrepareFile()
{
   // fp=fopen("D:\\1.dfu","rb");
    //fp=fopen("/home/gypeng/updateFile/BTHUpdate.dfu","rb");
    //fp=fopen("/mnt/sdcard/BTHUpdate.dfu","rb");
    fp=fopen("/mnt/external_sd/BTHUpdate.dfu","rb");
   
    if ( fp == NULL ){
        printf( "The file was not opened\n" );
	sleep(100);
	}
    else
        printf( "The file was opened\n" );

     fseek(fp,0,SEEK_END);
     uint32 length=ftell(fp);
     fseek(fp,0,SEEK_SET);

     return length;

}

void UnPrePareFile()
{
    fclose(fp);
}


uint16 ReadDfuPacket( uint8* address, uint16 readSize,uint16 block )
{
    uint16 actRead = 0;
    if( !feof( fp ))
        actRead = fread(address,1,readSize,fp);
    return actRead;
}

