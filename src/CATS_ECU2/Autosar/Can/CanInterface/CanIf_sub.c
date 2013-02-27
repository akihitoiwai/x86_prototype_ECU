#include <pthread.h>
#include "Platform_Types.h"
#include "Dem.h"
#include "CanIf.h"
#include "CanDriver/Can.h"
#include "CanIf_sub.h"



extern Can_FunctionTable* CanDriverList[NUMBER_OF_CANDRIVERS];

/* 受信バッファの実体 */
static CanIfReceiveBuffer ReceiveBuffer[NUMBER_OF_CANCONTROLLER];

/* PduID, CanPduId, controllerを対応付けするためのテーブル */
static CanPduIdTableType CanRxPduIdTable[NUMBER_OF_PDU_MAX];
static CanPduIdTableType CanTxPduIdTable[NUMBER_OF_PDU_MAX];
static uint32 CanRxPduIdTableSize = 0;
static uint32 CanTxPduIdTableSize = 0;



void CanIf_subInit() {
  int i;
  memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));
  memset(CanRxPduIdTable, 0, sizeof(CanRxPduIdTable));
  memset(CanTxPduIdTable, 0, sizeof(CanTxPduIdTable));
  CanRxPduIdTableSize = 0;
  CanTxPduIdTableSize = 0;
  for (i = 0; i < NUMBER_OF_CANCONTROLLER; ++i) {
    pthread_mutex_init(&ReceiveBuffer[i].critialSection, NULL);
  }
}



void AddRxPduTable(PduIdType pduId, uint8 controller, uint32 canId) {
  if (CanRxPduIdTableSize >= NUMBER_OF_PDU_MAX) {
    DebugOut("out of CanRxPduIdTableSize\n");
    return;
  }
  CanRxPduIdTable[CanRxPduIdTableSize].pduId = pduId;
  CanRxPduIdTable[CanRxPduIdTableSize].controller = controller;
  CanRxPduIdTable[CanRxPduIdTableSize].canPduId = canId;
  ++CanRxPduIdTableSize;
}

void AddTxPduTable(PduIdType pduId, uint8 controller, uint32 canId) {
  if (CanTxPduIdTableSize >= NUMBER_OF_PDU_MAX) {
    DebugOut("out of CanTxPduIdTableSize\n");
    return;
  }
  CanTxPduIdTable[CanTxPduIdTableSize].pduId = pduId;
  CanTxPduIdTable[CanTxPduIdTableSize].controller = controller;
  CanTxPduIdTable[CanTxPduIdTableSize].canPduId = canId;
  ++CanTxPduIdTableSize;
}

/* PduIdTypeからcontrollerを得る */
uint8 GetRxController(PduIdType id) {
  uint32 i;
  for (i = 0; i < CanRxPduIdTableSize; ++i) {
    if (CanRxPduIdTable[i].pduId == id) {
      return CanRxPduIdTable[i].controller;
    }
  }
  return CANIF_INVALID_CONTROLLER;
}
uint8 GetTxController(PduIdType id) {
  uint32 i;
  for (i = 0; i < CanTxPduIdTableSize; ++i) {
    if (CanTxPduIdTable[i].pduId == id) {
      return CanTxPduIdTable[i].controller;
    }
  }
  return CANIF_INVALID_CONTROLLER;
}
CanPduIdTableType* CanIf_FindTxPduIdTable(PduIdType pduId) {
  uint32 i;
  for (i = 0; i < CanTxPduIdTableSize; ++i) {
    if (CanTxPduIdTable[i].pduId == pduId) {
      return &CanTxPduIdTable[i];
    }
  }
  return NULL;
}
CanPduIdTableType* CanIf_FindCanIdToRxPduIdTable(uint8 controller, Can_IdType canId) {
  uint32 i;
  for (i = 0; i < CanRxPduIdTableSize; ++i) {
    if (CanRxPduIdTable[i].controller == controller && CanRxPduIdTable[i].canPduId == canId) {
      return &CanRxPduIdTable[i];
    }
  }
  return NULL;
}




/* CanDriverテーブル */
int CanIf_addCanDriver(Can_FunctionTable* candrv) {
  int i;
  for (i = 0; i < NUMBER_OF_CANDRIVERS; ++i) {
    if (CanDriverList[i] == candrv) return -1; /* すでに登録されている */
    if (CanDriverList[i] != 0) continue;
    CanDriverList[i] = candrv;
    return i; /* 登録 */
  }
  return -1; /* もう登録できない */
}



/* 受信データがあるか調べる */
uint32 CanIfReceiveBuffer_CheckReceivedData(CanIfReceiveBuffer* rb) {
  if (rb->wrotePosition == rb->readPosition) return 0;
  if (rb->wrotePosition > rb->readPosition)
    return rb->wrotePosition - rb->readPosition;
  else
    return CanIfRecvBufferSize + rb->wrotePosition - rb->readPosition;
}
/* mutex lock */
void CanIfReceiveBuffer_lock(CanIfReceiveBuffer* rb) {
  pthread_mutex_lock(&rb->critialSection);
}
/* mutex unlock */
void CanIfReceiveBuffer_unlock(CanIfReceiveBuffer* rb) {
  pthread_mutex_unlock(&rb->critialSection);
}
/* read 必ずlockしてから呼ぶこと */
const CanIfReceiveBufferData* CanIfReceiveBuffer_read(CanIfReceiveBuffer* rb) {
  if (rb->readPosition == rb->wrotePosition) return 0;
  rb->readPosition = (rb->readPosition + 1) % CanIfRecvBufferSize;
  return &rb->buffer[rb->readPosition];
}
/* write 必ずlockしてから呼ぶこと */
CanIfReceiveBufferData* CanIfReceiveBuffer_write(CanIfReceiveBuffer* rb) {
  uint32 newpos = (rb->wrotePosition + 1) % CanIfRecvBufferSize;
  if (newpos == rb->readPosition) {
    /* buffer overflow */
    DebugOut("CanIfReceiveBuffer_write: buffer over flow\n");
    return 0;
  }
  rb->wrotePosition = newpos;
  return &rb->buffer[rb->wrotePosition];
}

CanIfReceiveBuffer* CanIf_GetReceiveBuffer(uint8 controller) {
  return &ReceiveBuffer[controller];
}


/* ポーリング用スレッド制御 */

static pthread_mutex_t gCanIfMutex;
static pthread_cond_t gCanIfCond;
static volatile int gCanThreadExit;

/* CanDriverのポーリング用ルーチンを呼ぶ */
static void* CanIf_SubThread(void* ptr) {
  DebugOut("CanIf_SubThread start\n");
  while (gCanThreadExit == 0) {
    CanIf_MainRoutine();
    Dem_Sleep(100); // 0.1msecのsleep
  }
  pthread_exit(NULL);
  return 0;
}

Std_ReturnType CanIf_CreateThread() {
  static int threadStatus = 0;
  static pthread_t thread;
  if (threadStatus == 0) {

    pthread_mutex_init(&gCanIfMutex, NULL);
    pthread_cond_init(&gCanIfCond, NULL);
	gCanThreadExit = 0;

    if (pthread_create(&thread, NULL, CanIf_SubThread, NULL) != 0) {
      DebugOut("pthread_create failed\n");
      return E_NOT_OK;
    }
    pthread_detach(thread);

    threadStatus = 1; /* スレッド生成は一回のみ */
  }
  return E_OK;
}

void CanIf_lock() {
  pthread_mutex_lock(&gCanIfMutex);
}

void CanIf_unlock() {
  pthread_mutex_unlock(&gCanIfMutex);
}

void CanIf_signal() {
  pthread_cond_signal(&gCanIfCond);
}

void CanIf_ExitThread(void)
{
	gCanThreadExit = 1;
	// 一応、スレッドが完全に抜けるまで delay
	Dem_Sleep(500); // 0.5msecのsleep
}

