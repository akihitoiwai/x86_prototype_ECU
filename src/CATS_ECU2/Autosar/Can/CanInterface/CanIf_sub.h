#ifndef CANIF_SUB_H
#define CANIF_SUB_H

/* PduID, CanPduId, controller��Ή��t�����邽�߂̃e�[�u�� */
typedef struct {
  PduIdType pduId;
  uint8 controller;
  uint32 canPduId;
} CanPduIdTableType;

/* ��MFIFO�o�b�t�@�N���X */
#define CanIfRecvBufferSize 2560
typedef struct { 
  Can_IdType CanId;
  uint8 hrh;
  uint8 dlc;
  uint8 sdu[8];
} CanIfReceiveBufferData;

typedef struct {
  uint32 wrotePosition;
  uint32 readPosition;
  pthread_mutex_t critialSection; /* mutex */
  CanIfReceiveBufferData buffer[CanIfRecvBufferSize];
} CanIfReceiveBuffer;

#ifdef __cplusplus
extern "C" {
#endif
void CanIf_subInit();


int CanIf_addCanDriver(Can_FunctionTable* candrv);
/* ��M�f�[�^�����邩���ׂ� */
uint32 CanIfReceiveBuffer_CheckReceivedData(CanIfReceiveBuffer* rb);
/* mutex lock */
void CanIfReceiveBuffer_lock(CanIfReceiveBuffer* rb);
/* mutex unlock */
void CanIfReceiveBuffer_unlock(CanIfReceiveBuffer* rb);
/* read �K��lock���Ă���ĂԂ��� */
const CanIfReceiveBufferData* CanIfReceiveBuffer_read(CanIfReceiveBuffer* rb);
/* write �K��lock���Ă���ĂԂ��� */
CanIfReceiveBufferData* CanIfReceiveBuffer_write(CanIfReceiveBuffer* rb);
/* ReceiveBuffer�̎擾 */
CanIfReceiveBuffer* CanIf_GetReceiveBuffer(uint8 controller);

Std_ReturnType CanIf_CreateThread(); /* �X���b�h���� */
void CanIf_lock();
void CanIf_unlock();
int CanIf_wait(int msec);
void CanIf_signal();
void CanIf_ExitThread(void);	/* �X���b�h�I�� */

void AddRxPduTable(PduIdType pduId, uint8 controller, uint32 canID);
void AddTxPduTable(PduIdType pduId, uint8 controller, uint32 canID);
/* PduIdType����controller�𓾂� */
uint8 GetRxController(PduIdType id);
uint8 GetTxController(PduIdType id);
#define CANIF_INVALID_CONTROLLER 0xff
CanPduIdTableType* CanIf_FindTxPduIdTable(PduIdType pduId);
CanPduIdTableType* CanIf_FindCanIdToRxPduIdTable(uint8 controller, Can_IdType canId);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
