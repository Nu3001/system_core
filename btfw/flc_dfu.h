/*****************************************************************************/
/************************Flaicomm CopyRight 2010.*****************************/ 
/*****************************************************************************/
/************************Flaicomm CopyRight 2010.*****************************/ 
/*****************************************************************************/

#ifndef _FCL_DFU_H_
#define _FCL_DFU_H_
#include "platformSpecific.h"


#define  DFU_DETACH       0x00
#define  DFU_DNLOAD       0x01
#define  DFU_UPLOAD       0x02
#define  DFU_GETSTATUS    0x03
#define  DFU_CLRSTATUS    0x04
#define  DFU_GETSTATE     0x05
#define  DFU_ABORT        0x06
#define  DFU_RESET        0x07
#define  DFU_GETDEVICE    0x08
#define  DFU_GETFUNCT     0x09



enum DFUStage 
{
    STAGE_IDLE = 0,
    STAGE1_GOTDEV, 
    STAGE1_GOTFUN,
    STAGE1_DETACHED,
    STAGE1_RESETED,
    STAGE2_GOTSTATUS,
    STAGE2_GETDEV,
    STAGE2_GOTFUN,
    STAGE2_DOWNLOADING,
    STAGE2_DOWNLOADED,
    STAGE2_GOT_WAITINGTIME,
    STAGE2_DNLD_COMPLETE,
    STAGE2_RESETED,
    STAGE3_GOTDEV,
    STAGE_CLRSTATUS//Eshin add 
    
    //STAGE3_GOTDEV
};



struct dfuReplyStatus
{
    uint16_t status;
    uint16_t lenth;
};

struct replyForGetDevice
{
    struct dfuReplyStatus replyStatus;
    uint16_t bDeviceClass;
    uint16_t bDeviceSubClass;
    uint16_t bDeviceProtocol;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
};


struct replyforGetFunction
{
    struct dfuReplyStatus replyStatus;
    uint16_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
};


struct replyforGetStatus
{
    struct dfuReplyStatus replyStatus;
    uint32_t bStatus : 8;
    uint32_t bwPollTimeout : 24;
    uint8_t bState;
    uint8_t iString;
};

uint8 startBcsp(void);
//int32 Win32Getime(void);
void PrintData(unsigned char *data, int length);

#endif
