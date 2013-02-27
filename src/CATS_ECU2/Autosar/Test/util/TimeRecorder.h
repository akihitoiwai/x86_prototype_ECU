/* TimeRecorder.h


*/


#ifndef TIMERECORDER_H
#define TIMERECORDER_H

#include "Std_Types.h"
#include "Platform_Types.h"


#define TR_ID_MAX 1024
#define TR_PERSEC(x) ((x) / 1000000.0)
#ifdef __cplusplus
extern "C" {
#endif

uint64 getNow();

typedef struct {
  uint64 start;
  uint64 past;
  uint32 count;
} TimeRecorderData;

void TimeRecorderInit();
void TimeRecorderStart(int id);
void TimeRecorderStop(int id);
TimeRecorderData* TimeRecorderGetData(int id);

/* for debug */
void printTime(const char* msg, uint32 id, uint64 total);


#ifdef __cplusplus
} /* extern "C" { */
#endif


#define TR_CAN_SJA10000_CAN_SEND_DEVICE 101
#define TR_CAN_SJA10000_CAN_RECV_DEVICE 102
#define TR_CANIF_RXINDICATION 201
#define TR_CANIF_TXCONFIRMATION 202
#define TR_CANIF_MAINROUTINE 203

#define TR_FRIF_TRANSMIT 301
#define TR_FR_FCBTRANSMIT 302

#define TR_FR_FCBDEVICETHREAD 310
#define TR_FR_FCBRECEIVE 311    /* FcbDeviceThread‚æ‚èŒÄ‚Î‚ê‚é */
#define TR_FR_RXINDICATION 312  /* FcbDeviceThread‚æ‚èŒÄ‚Î‚ê‚é */
#define TR_FR_TXINDICATION 313  /* FcbDeviceThread‚æ‚èŒÄ‚Î‚ê‚é */

#define TR_FR_CCTIMERTHREAD 320


#endif

