/*****************************************************************************/
/************************Flaicomm CopyRight 2010.*****************************/ 
/*****************************************************************************/

#include <stdio.h>
#include "ubcsp.h"
#include "flc_dfu.h"
#include <stdio.h>
#include <string.h>
#include "platformSpecific.h"
#include "BTUpdateClientSocket.h"

void InitialiseWindows(char *);
void ubcsp_end_of_functions (void);
uint8 MenuSelection(void);
void DecodeSentState(struct ubcsp_packet *send_packet);
void DecodeReceiveState(struct ubcsp_packet *rx_packet, unsigned char *rx_buffer);
void PrintData(unsigned char *data, int length);
enum channelreliability {UNRELIABLE, RELIABLE};

enum ubcspstates
{
    POLL_STATE,
    SENT_STATE,
    RECEIVED_STATE,
    PEER_RESET_STATE,
    SLEEP_STATE
};

enum BCSPreliablechannels 
{
	ACKKNOWLEDGE, 
	UNUSED_RELIABLE_CHANNEL_1, 
	BCCMD, 
	HQ,
	DEVICE_MGT,
	HCI_COMMAND,
	HCI_ACL_RELIABLE,
	HCI_SCO_RELIABLE,
	L2CAP,
	RFCOMM,
	SDD,
	RESERVED_RELIABLE_CHANNEL_11, 
	DFU,
	VM,
	UNUSED_RELIABLE_CHANNEL_14, 
	RESERVED_RELIABLE_CHANNEL_15 
};

enum BCSPunreliablechannels 
{
	LINK_ESTABLISHMENT = 1, 
	HCI_SCO_UNRELIABLE
};


#if 0
static const uint8 HCICommand_Read_BD_Addr[] =
{
	0x09, 0x10, 0x00
};

static const uint8 HCICommand_Read_Local_Name[] =
{
	0x14, 0x0C, 0x00
};


static const uint8 HCICommand_Change_Local_Name[] =
{
	0x13, 0x0C
};

static const uint8 HCICommand_Write_Scan_Enable[] =
{
	0x1A, 0x0C, 0x01, 0x03
};

static const uint8 HCICommand_Inquiry[] =
{
  0x01, 0x04, 0x05, 0x33, 0x8b, 0x9e, 0x0a, 0x0a
};

static const uint8 HCICommand_Create_Connection[] =
{
  0x05, 0x04,                           /* HCI Command                    */
  0x0d,                                 /* No of Bytes                    */
  0xe8, 0x0e, 0xff, 0x5b, 0x02, 0x00,   /* BD Addr                        */
  0x18, 0xcc,                           /* DM1, DH1, DM3, DH3, DM5, DH5 Data Type */
  0x01,                                 /* Page Scan Repetition Mode = R1 */
  0x00,                                 /* Page Scan Mode = Mandatory     */
  0x8f, 0x50,                           /* Clock Offset                   */
  0x00                                  /* Master Slave Switch = Master   */
};

static const uint8 HCICommand_Add_SCO_Connection[] =
{
  0x07, 0x04, /* HCI Command */
  0x20, 0x00  /* Packet Type = HV1 */
};
#endif
static const uint8 DFUCommand_Get_Dev[] =
{
    0xc1, 0x08, 0x00, 0x00, 0x00, 0x00, 0x0c,0x00
};
static const uint8 DFUCommand_Get_Fun[] = 
{
    0xc1, 0x09, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00
};

static const uint8 DFUCommand_Detach[] = 
{
    0x21, 0x00, 0x88, 0x13, 0x00, 0x00, 0x00, 0x00
};

static const uint8 DFUCommand_Reset[] = 
{
    0x41, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8 DFUCommand_GetStatus[] = 
{
    0xa1, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00
};

static const uint8 DFUCommand_CLRStatus[] = //Eshin add 0826
{
    0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//static uint8 ConnectionHandle[2];

// HCI Command Queues
struct DfuQueue
{
    uint8 NoOfOpcodes;
    uint8 commandnumber;
    uint8 BmRequestType;
    uint8 BRequest;
    uint8 NoOfRetries;
};

static struct DfuQueue Dfu_TXQueue;
static struct DfuQueue Dfu_RXQueue;
static uint8 SCO_Flag, FirstInquiry;
static enum DFUStage currentDfuStage,BackupDfuStage;
static uint16 dfuPacketSize;
static uint16 dfuCurrentPacketNum;
static uint8  fileTransfering;
static uint32 waitingTimeAfterDownloadFinsh;
static uint8  dfu_completed = 0;

//LARGE_INTEGER li;
//int64 start, end, freq;


void paramReset()
{
    currentDfuStage = STAGE_IDLE;
    BackupDfuStage = STAGE_IDLE;
    dfuPacketSize = 0;
    dfuCurrentPacketNum = 0;
    fileTransfering = 0;
    waitingTimeAfterDownloadFinsh = 0;
	dfu_completed = 0;

}
/*****************************************************************************/


uint8 startBcsp(void)
{

    char
        activity;

    int
        lastsentpacket,
        delay;

    struct ubcsp_packet
        send_packet,
        receive_packet;

    unsigned char
        send_buffer[512],
        receive_buffer[512];

	memset(send_buffer,0,sizeof(send_buffer));
	memset(receive_buffer,0,sizeof(receive_buffer));

   /* initializing windows COM port */
    DEBUG("\n");


    InitialiseUart();

    /* initialize the ubcsp stack */
    DEBUG("Initialising uBCSP - Please Wait\n");
    ubcsp_initialize ();

    /* setup the send packets payload */

    send_packet.payload = send_buffer;

    /* setup the receive packet payload and length */

    receive_packet.payload = receive_buffer;
    receive_packet.length = 512;
    activity = 0;
    /* ask ubcsp to receive a packet */

    ubcsp_receive_packet (&receive_packet);


    Dfu_TXQueue.commandnumber = RECEIVED_STATE;
    Dfu_TXQueue.NoOfOpcodes = 0;
    Dfu_TXQueue.NoOfRetries = 0;
    lastsentpacket = 0;
    FirstInquiry = 1;

//fplog

    paramReset();
#if 1
    while(dfu_completed != 1)

    {
     //   DEBUG("\commandnumber activity %d  %x\n",Dfu_TXQueue.commandnumber,activity );

        switch(Dfu_TXQueue.commandnumber)
        {
        case POLL_STATE:
            {
                /* poll the ubcsp stack */
                delay = ubcsp_poll ((uint8*)&activity);
                Dfu_TXQueue.commandnumber = RECEIVED_STATE;
		  // Delay1ms(100);
		//printf("POLL_STATE----POLL_STATE end\n");
            }
            break;

        case RECEIVED_STATE:
            {
                Dfu_TXQueue.commandnumber = PEER_RESET_STATE;
                if(activity & UBCSP_PACKET_RECEIVED)
                {
                    activity = activity&(~UBCSP_PACKET_RECEIVED);
                    DecodeReceiveState(&receive_packet, receive_buffer);
                }

                if(activity & UBCSP_PACKET_SENT)
                {
                    //DEBUG("\nLast Packet sent and acknowledged by uBCSP\n");
                    /* Register the fact we have sent packet and it has been acknowledged*/
                    if(lastsentpacket == 0)
                    {
                        /* The link is now established */
                        DEBUG("\nLink is now Established\n");

                        lastsentpacket = 1;
                        Dfu_TXQueue.commandnumber = SENT_STATE;
                    }
                    else
                    {
                        lastsentpacket = 1;
                    }
                }
            }
            break;

        case SENT_STATE:
            {
                /* Returns the appropriate next state depending on Valid Menu selection */
                DecodeSentState(&send_packet);
            }
            break;

        case PEER_RESET_STATE:
            {
                if(activity & UBCSP_PEER_RESET)
                {
                    /* We have detected a peer reset */
                    activity = 0;
                    /* ask ubcsp to receive a packet */
                    Dfu_TXQueue.commandnumber = RECEIVED_STATE;
                    Dfu_TXQueue.NoOfOpcodes = 0;
                    Dfu_TXQueue.NoOfRetries = 0;
                    lastsentpacket = 0;
                    FirstInquiry = 1;
                    DEBUG("\nAPPLICATION PEER RESET - Please Wait\n\n");

                    /* Normally, we would restart our stack at this point */
                }
                Dfu_TXQueue.commandnumber = SLEEP_STATE;

            }
            break;

        case SLEEP_STATE:
            {
                if(delay)
                {
                    /* If we needed to delay, then Sleep for a bit */
                    //Sleep(1);
                    Delay1ms(1);
                }
                Dfu_TXQueue.commandnumber = POLL_STATE;
            }
            break;
        }
    }
#endif
		return 0;

}


void SendDfuCmd(struct ubcsp_packet *sd_packet, const uint8* cmd, uint16 size_t)
{
	//static uint8 dfuCmd[100];
//	uint16 i = 0;
 //   DEBUG("\n");
    //DEBUG("Dfun cmd: %d\n",cmd[1] );
	
//	for( i = 0 ; i < size_t; i++)
//		dfuCmd[i] = cmd[i];
	
    sd_packet->payload = (uint8*)cmd;
    sd_packet->channel = DFU;
    sd_packet->length = size_t;
    sd_packet->reliable = RELIABLE;
    sd_packet->use_crc = 1;

    Dfu_TXQueue.NoOfRetries = 0;
    Dfu_TXQueue.BmRequestType = cmd[0];
    Dfu_TXQueue.BRequest = cmd[1];
    Dfu_TXQueue.commandnumber = RECEIVED_STATE;
    Dfu_TXQueue.NoOfOpcodes = 1;

}

void SendDfuPacket(struct ubcsp_packet *sd_packet, uint8 isEmptyPacket)
{

    uint8 dfuDownLoadPacket[2048];
    uint16 dataReaded;
    //dfuPacketSize = 256;
    dfuPacketSize = 280;

    /*command code*/
    dfuDownLoadPacket[0] = 0x21;
    dfuDownLoadPacket[1] = 0x01;

    /*packet Num*/
    dfuDownLoadPacket[2] = (uint8)(dfuCurrentPacketNum & 0x00ff);
    dfuDownLoadPacket[3] = (uint8)(dfuCurrentPacketNum>>8 & 0x00ff);   
    
    /*keep empty*/
    dfuDownLoadPacket[4] = 0x00;
    dfuDownLoadPacket[5] = 0x00;

    if(!isEmptyPacket)
    {
        if(dfuCurrentPacketNum == 0)
        {
            packagetotal=PrepareFile()/dfuPacketSize+1;
            fileTransfering = 1; 
        }
        dataReaded = ReadDfuPacket( &dfuDownLoadPacket[8], dfuPacketSize,dfuCurrentPacketNum );
        if(dataReaded < dfuPacketSize)
        {
            UnPrePareFile();
            fileTransfering = 0; 
            dataReaded -= 16;
        }
        dfuCurrentPacketNum ++;

    }
    else
    {
        dataReaded = 0x0000;
    }
    dfuDownLoadPacket[6] = (uint8)(dataReaded & 0x00ff);
    dfuDownLoadPacket[7] = (uint8)(dataReaded>>8 & 0x00ff);

    packageid=dfuCurrentPacketNum;
 //   printf("\n");
    printf("Last Dfu downloading packet. %d  T%d downloaded\n",dataReaded,dfuCurrentPacketNum );
//    printf("Time:%d\n",Win32Getime());
    sd_packet->payload = dfuDownLoadPacket;
    sd_packet->channel = DFU;
    sd_packet->length = dataReaded+8;  /*payload + head*/
    sd_packet->reliable = RELIABLE;
    sd_packet->use_crc = 1;
    Dfu_TXQueue.NoOfRetries = 0;
    Dfu_TXQueue.BmRequestType = dfuDownLoadPacket[0];
    Dfu_TXQueue.BRequest = dfuDownLoadPacket[1];
    Dfu_TXQueue.commandnumber = RECEIVED_STATE;
    Dfu_TXQueue.NoOfOpcodes = 1;

}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

void DecodeSentState(struct ubcsp_packet *sd_packet)
{
    static uint16 i;
    //DEBUG ("\nDecodeSentState:%d",currentDfuStage);
    switch(currentDfuStage)
    {
        case STAGE_IDLE:
		//Delay1ms(2);
            SendDfuCmd(sd_packet, DFUCommand_Get_Dev, sizeof(DFUCommand_Get_Dev));
            break;
        case STAGE1_GOTDEV:
            SendDfuCmd(sd_packet, DFUCommand_Get_Fun, sizeof(DFUCommand_Get_Fun));
            break;
        case STAGE1_GOTFUN:
            SendDfuCmd(sd_packet, DFUCommand_Detach, sizeof(DFUCommand_Detach));
           break;
        case STAGE1_DETACHED:
            SendDfuCmd(sd_packet, DFUCommand_Reset, sizeof(DFUCommand_Reset));
            break;
        case STAGE1_RESETED:
	    //Delay1ms(100);
            SendDfuCmd(sd_packet, DFUCommand_GetStatus, sizeof(DFUCommand_GetStatus));
           break;
        case STAGE2_GOTSTATUS:
	    //Delay1ms(100);
            SendDfuCmd(sd_packet, DFUCommand_Get_Fun, sizeof(DFUCommand_Get_Fun));
            break;
        case STAGE2_GOTFUN:
            Delay1ms(10);
            SendDfuCmd(sd_packet, DFUCommand_GetStatus, sizeof(DFUCommand_GetStatus));            
            break;
			
        case STAGE_CLRSTATUS://Eshin add 0826
            //Delay1ms(10);//just test
            SendDfuCmd(sd_packet, DFUCommand_CLRStatus, sizeof(DFUCommand_CLRStatus));            
            break;

			
        case STAGE2_DOWNLOADING:
	    Delay1ms(1);
            SendDfuPacket( sd_packet, 0 );
            break;
        case STAGE2_DOWNLOADED:
            Delay1ms(20);
            SendDfuCmd(sd_packet, DFUCommand_GetStatus, sizeof(DFUCommand_GetStatus));
            break;
        case STAGE2_GOT_WAITINGTIME:
            SendDfuPacket(sd_packet, 1);
            break;
        case STAGE2_DNLD_COMPLETE:
		Delay1ms(50);
            Delay1ms(waitingTimeAfterDownloadFinsh);
            //SendDfuCmd(sd_packet, DFUCommand_GetStatus, sizeof(DFUCommand_GetStatus));
            SendDfuCmd(sd_packet, DFUCommand_Reset, sizeof(DFUCommand_Reset));
            break;
        case STAGE2_RESETED:
		Delay1ms(100);
            SendDfuCmd(sd_packet, DFUCommand_Get_Dev, sizeof(DFUCommand_Get_Dev));
            break;
        case STAGE3_GOTDEV:
            SendDfuCmd(sd_packet, DFUCommand_Reset, sizeof(DFUCommand_Reset));
        default:
            break;
    }

  if(Dfu_TXQueue.commandnumber == RECEIVED_STATE)
    {
        /* Send the Transmit Packet */
  //      DEBUG("\nHCI COMMAND SENT = \n"); 
        ubcsp_send_packet(sd_packet);
        // Some Debug Information
        #if 0
        //if( sd_packet->length < 100)
        {
            for (i = 0; i < sd_packet->length; i++)
            {
                DEBUG ("%x ", sd_packet->payload[i]);
				if((i+1)%32==0)
            		DEBUG ("\n");
            }
          //  DEBUG ("\n");
        }
	#endif
    }
    else
    {
        /* Else Menu selection invalid there load Menu selection again */
        Dfu_TXQueue.commandnumber = SENT_STATE;
    }

}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

void DecodeReceiveState(struct ubcsp_packet *rx_packet, unsigned char *rx_buffer)
{
    static int i;

    /* We've received a packet */
   // DEBUG ("\nRECEIVED PACKET: ");
//	printf ("\nDecodeReceiveState startime: %d \n",Win32Getime());		
    // Only print out HCI information
    if(rx_packet->channel == HCI_COMMAND)
    {
        DEBUG("\nHCI COMMAND RECEIVED: ");
        switch((rx_buffer[0] & 0xFF))
        {

        case 0x05:
            {
                DEBUG("DISCONNECTION COMPLETE: ");
                //PrintData(rx_buffer, rx_packet->length);
            }
            break;
        case 0x0f:
            {
                DEBUG("COMMAND STATUS: ");
                //PrintData(rx_buffer, rx_packet->length);
    		//DEBUG ("\n");
				
            }
            break;

        case 0x10:
            {
                DEBUG("HARDWARE ERROR: ");
                //PrintData(rx_buffer, rx_packet->length);
            }
            break;

        case 0x1b:
            {
                DEBUG("MAX SLOT CHANGE: ");
                //PrintData(rx_buffer, rx_packet->length);
            }
            break;

        default:
            {
                DEBUG("UNKNOWN COMMAND: ");
                //PrintData(rx_buffer, rx_packet->length);
            }
        }
    }
    else 	if (rx_packet->channel == DFU)
    {
        Dfu_TXQueue.commandnumber = SENT_STATE;
        Dfu_TXQueue.NoOfOpcodes = 0;
        //DEBUG("\nPacket Received, Channel = DFU ", rx_packet->channel);
        //DEBUG("\nString = ");
        //PrintData(rx_buffer, rx_packet->length);
        //DEBUG("\nOldDfuStage: %d ", currentDfuStage);
        switch( Dfu_TXQueue.BRequest )
        {
        case DFU_DETACH:
			{   struct dfuReplyStatus reply;
               // DEBUG("\n DFU_DETACH");
                (void)memcpy((void *)(&reply),rx_buffer,rx_packet->length);
                if( 0x00 == reply.status && STAGE1_GOTFUN == currentDfuStage)
                    currentDfuStage = STAGE1_DETACHED;
                break;
            }  
        case DFU_GETSTATUS: 
            {
                struct replyforGetStatus reply;
               // DEBUG("\n DFU_GETSTATUS");
                memcpy((void *)(&reply),rx_buffer,rx_packet->length);
                if( reply.replyStatus.status != 0x00 /*|| reply.replyStatus.lenth < sizeof(reply) - sizeof(reply.replyStatus)*/)
                {
                    DEBUG("\nget status reply return error or param lenth not right");
                }
                else
                {
                    if( STAGE1_RESETED == currentDfuStage ){
                        currentDfuStage = STAGE2_GOTSTATUS;
			   //DEBUG ("\ncurrentDfuStage = STAGE2_GOTSTATUS%d ");
                    	}
                    else if( STAGE2_GOTFUN == currentDfuStage)
                    {
                        if(reply.bState == 0x02 || reply.bState == 0x05 )
                        {
                            currentDfuStage = STAGE2_DOWNLOADING;
			   //DEBUG ("\ncurrentDfuStage = STAGE2_DOWNLOADING ");
					
                        } 
			   else if(reply.bState == 0x0a)//Eshin add 0826
			   {
			     	currentDfuStage = BackupDfuStage; 
                            currentDfuStage = STAGE_CLRSTATUS;
							
			   DEBUG ("\ncurrentDfuStage = STAGE_CLRSTATUS ");

			   }
                        else 
                        {
                            currentDfuStage = STAGE2_GOTFUN;
			   DEBUG ("\ncurrentDfuStage = STAGE2_GOTFUN  ");
							
                        }
                    } 
                    else if(STAGE2_DOWNLOADED ==  currentDfuStage)
                    {
                        //if(reply.bState == 0x02 )
                        {
                            waitingTimeAfterDownloadFinsh = reply.bwPollTimeout;
			    //printf("waitingTimeAfterDownloadFinsh=[%d]\n",waitingTimeAfterDownloadFinsh);
                            currentDfuStage = STAGE2_GOT_WAITINGTIME;
			   DEBUG ("\ncurrentDfuStage = STAGE2_GOT_WAITINGTIME ");
							
                        }
                        //else
                        {

                        }
                    }
                    else if( STAGE2_DNLD_COMPLETE == currentDfuStage)
                    {
                        currentDfuStage = STAGE2_RESETED;
			   DEBUG ("\ncurrentDfuStage = STAGE2_RESETED ");
						
                    }
                }
            //currentDfuStage = STAGE1_DETACHED:
            break;
            }

        case DFU_RESET :
            {
                struct dfuReplyStatus reply;
                //DEBUG("\n DFU_RESET");
                memcpy((void *)(&reply),rx_buffer,rx_packet->length);
                if( 0x00 == reply.status )
                {
                    if(STAGE1_DETACHED == currentDfuStage)
                    	{
                        currentDfuStage = STAGE1_RESETED;
			   DEBUG ("\ncurrentDfuStage = STAGE1_RESETED ");
						
                    	}		
                    else if(STAGE2_DNLD_COMPLETE == currentDfuStage)
                    	{
                    	DEBUG ("\ncurrentDfuStage = STAGE2_RESETED ");
                        currentDfuStage = STAGE2_RESETED;
                    	}
					else if (STAGE3_GOTDEV == currentDfuStage)
						{
			   DEBUG ("\ndfu_completed ");
						dfu_completed = 1;
						}
                }
                break;
            }
            break;
        case DFU_GETDEVICE:  
            {
                struct replyForGetDevice reply;
                //memset((void *)reply, 0, sizeof(reply));
                //DEBUG("\n DFU_GETDEVICE");
                memcpy((void *)(&reply),rx_buffer,rx_packet->length);
                if( reply.replyStatus.status != 0x00 || reply.replyStatus.lenth < sizeof(reply) - sizeof(reply.replyStatus))
                {
			
                    DEBUG("\nget device reply return error or param lenth not right");
					
                }
                else if(STAGE_IDLE == currentDfuStage)
                {
                    if(reply.bDeviceClass == 0x00fe)
                    	{
                    	 DEBUG ("\ncurrentDfuStage = STAGE1_RESETED ");

                         currentDfuStage = STAGE1_RESETED;
                    	}
                    else if(reply.bDeviceClass == 0x00e0)
                    	{
                    	DEBUG ("\ncurrentDfuStage = STAGE1_GOTDEV ");
                        currentDfuStage =  STAGE1_GOTDEV;
                    	}
                    else
                        DEBUG("\nget unkoown device class ");
                }
                else if( STAGE2_RESETED == currentDfuStage )
                {
                    	 DEBUG ("\ncurrentDfuStage = STAGE3_GOTDEV ");
                     currentDfuStage =  STAGE3_GOTDEV;
                }

                break;
            }
        case DFU_GETFUNCT:
            {
                struct replyforGetFunction reply;
                //memset((void *)reply, 0, sizeof(reply));
                //DEBUG("\n DFU_GETFUNCT");
                memcpy((void *)(&reply),rx_buffer,rx_packet->length);
                if( reply.replyStatus.status != 0x00 || reply.replyStatus.lenth < sizeof(reply) - sizeof(reply.replyStatus))
                {
                    DEBUG("\nget device reply return error or param lenth not right  ");
                }
                else
                {
                    if( STAGE1_GOTDEV == currentDfuStage )
                    	{
                        currentDfuStage = STAGE1_GOTFUN;
                    	 DEBUG ("\ncurrentDfuStage = STAGE1_GOTFUN  ");
                    	}
                    else if(STAGE2_GOTSTATUS == currentDfuStage )
                    {
                    	   //DEBUG ("\ncurrentDfuStage = STAGE2_GOTFUN ");
                        currentDfuStage = STAGE2_GOTFUN;
                        dfuPacketSize = reply.wTransferSize;
			   printf("dfuPacketSize=[%d]>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n",dfuPacketSize);
                    }
                    else
                    {
                        DEBUG("\nunkown status,should not arrive here ");
                    }
                }

                break;
            }
        case DFU_DNLOAD:
            {
                struct dfuReplyStatus reply;
                //DEBUG("\nDFU_DNLOAD\n");
                memcpy((void *)(&reply),rx_buffer,rx_packet->length);
                if( 0x00 == reply.status)
                {
                    if( STAGE2_DOWNLOADING == currentDfuStage)
                    {
                        if( !fileTransfering)
                        	{
                    	 DEBUG ("currentDfuStage = STAGE2_DOWNLOADED\n ");
                            currentDfuStage = STAGE2_DOWNLOADED;
                        	}
                        else
                        	{
                    	 //DEBUG ("currentDfuStage = STAGE2_GOTFUN\n ");
                            currentDfuStage = STAGE2_GOTFUN;
                        	}
                    }
                    else if( STAGE2_GOT_WAITINGTIME == currentDfuStage)
                    {
                    	 DEBUG ("currentDfuStage = STAGE2_DNLD_COMPLETE\n ");
                        currentDfuStage = STAGE2_DNLD_COMPLETE;
                    }   
               }
                break;
            }  

        case DFU_CLRSTATUS:  
		{//add by Eshin 0826 need check
                //DEBUG("\n DFU_CLRSTATUS");
             		currentDfuStage = BackupDfuStage; 
			break;
        	}
        case DFU_GETSTATE:
                //DEBUG("\n DFU_GETSTATE");
        case DFU_UPLOAD:   
                //DEBUG("\n DFU_UPLOAD");
        case DFU_ABORT:     
                //DEBUG("\n DFU_ABORT");
        default:
            break;

        }
    }
    else
    {
        //DEBUG("\nPacket Received, Channel = %02x ", rx_packet->channel);
        DEBUG("\n NOT DFU data: ");
        PrintData(rx_buffer, rx_packet->length);
        DEBUG("\n");
    }

    /* Allow another packet to be received up to 512 bytes long */
    rx_packet->length = 512;

    /* Setup the receive again */
    ubcsp_receive_packet(rx_packet);
}
#if 1
void PrintData(unsigned char *data, int length)
{
    static int i;
  //  DEBUG("\nData = "); 
    for (i = 0; i < length; i++)
    {
     //   DEBUG("%02x ", data[i] & 0xFF);
    }
//if(length==0)
//    DEBUG("Null");
}
#endif
