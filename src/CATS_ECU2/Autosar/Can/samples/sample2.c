/*
  CanInterface sample program 2

  Copyright(c) 2010 CATS inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include "Dem.h"
#include "CanInterface/CanIf.h"
#include "Test/util/TimeRecorder.h"

#define SEND_PDUID 0x11
#define RECV_PDUID 0x11
#define SENDCFG 0
#define RECVCFG 1

// ���Ԍv���̎��ʎq
#define TR_CANTRANSMIT          2
#define TR_CANTRANSMITWAIT      3

extern CanIfConfigurationsType CanIfConfigurations;

void CAN_InterfaceTest(const char* driverName, uint32 sendcountmax, int baud, int config);
void printTime(const char* msg, uint32 id, uint64 total);

uint32 queueSize = 32;    /* �A�����đ��M����p�P�b�g�T�C�Y */
int transferd = 0;        /* �A�����M������ */
uint8 sendController = 0; /* ���M�e�X�g�̃R���g���[��ID */
uint8 recvController = 1; /* ��M�e�X�g�̃R���g���[��ID */


int main(int ac, char* av[]) {
  const char* defaultDriver = "SJA1000";
  int countmax = 1024*100/8; /* 100KB */
  int baud = 500; /* �{�[���[�g�̏����l*/
  int i;

  printf("CATS AUTOSAR CAN Driver Sample2\n");
  
  for (i = 1; i < ac; ++i) {
    if (av[i][0] == '-') {
      switch(av[i][1]) {
      case 's': /* transfer size */
        countmax = (atoi(av[++i])+7)/8;
        break;
      case 'b': /* baud */
        baud = atoi(av[++i]);
        break;
      case 'q': /* transfer queue size */
        queueSize = atoi(av[++i]);
        break;
      case 'T': /* use test driver */
        defaultDriver = "TEST";
        break;
      case '1': /* senddevice 1, receivedevice 2 mode */
        sendController = 1;
        recvController = 2;
        break;
      case '2': /* senddevice 2, receivedevice 3 mode */
        sendController = 2;
        recvController = 3;
        break;
      case '3': /* senddevice 3, receivedevice 0 mode */
        sendController = 3;
        recvController = 0;
        break;
	
      default:
        printf("usage: sample2 [-s <transfer size>] [-b <baud>] [-q <transfer queue size>][-1][-2][-3]\n"
	       "       transfer size: transfer data size (byte) default is 100,000 bytes\n"
           "          baud(kbps): 10, 20, 50, 100, 125, 250, 500(default), 800, 1000\n"
	       " transfer queue size: transfer queue size defult is 1\n"
           "                  -1: send channel 1 receive channel 2 mode\n"
           "                  -2: send channel 2 receive channel 3 mode\n"
           "                  -3: send channel 3 receive channel 0 mode\n"
	       );
        exit(0);
      }
    }
    else {
      defaultDriver = av[i];
    }
  }
  printf("transfer:%dKBytes / baud rate:%dKbps / queue size = %d\n", countmax*8/1024, baud, queueSize);
  CAN_InterfaceTest(defaultDriver, countmax, baud, 0);
  return 0;
}

uint32 sendcount = 0;
uint32 recvcount = 0;
PduInfoType recvData;


pthread_cond_t  sendCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sendMutex = PTHREAD_MUTEX_INITIALIZER;

/* ���M�R�[���o�b�N */
void CanTxCallback(PduIdType pduId) {
  pthread_mutex_lock(&sendMutex);
  --transferd;
  pthread_cond_signal(&sendCond);
  pthread_mutex_unlock(&sendMutex);
}

int sendError = 0;    /* ���M���s�̐� */
int receiveError = 0; /* ��M���s�̐� */
char recvErrorMsg[1024] = "";

/* ��M�R�[���o�b�N */
void CanRxCallback(PduIdType pduId, const void* data) {
  if (pduId == RECV_PDUID) {
    if (CanIf_ReadRxPduData(pduId, &recvData) == E_OK) {
      /* ��M�f�[�^���� */
      //      printf("ReceiveCB  ch:%d ID:%x '%8.8s'\n",  recvController, recvData.pduId, recvData.data);
      if (*(uint32*)(&recvData.SduDataPtr[0]) != recvcount || *(uint32*)(&recvData.SduDataPtr[4]) != ~recvcount) {
        if (receiveError == 0) {
          sprintf(recvErrorMsg, "Receive data error.(must be 0x%8.8x:%8.8x but receive data is 0x%8.8x:%8.8x)\n",
	         recvcount, ~recvcount,
	         *(uint32*)(&recvData.SduDataPtr[0]), *(uint32*)(&recvData.SduDataPtr[4]));
        }
        ++receiveError;
      }
      ++recvcount;
    }
    else {
      sprintf(recvErrorMsg, "ReceiveCB no data.\n");
      ++receiveError;
    }
  }
  else {
    sprintf(recvErrorMsg, "ReceiveCB invalid PduId %d\n", pduId);
    ++receiveError;
  }
}


void CAN_InterfaceTest(const char* driverName, uint32 sendcountmax, int baud, int config) {
  long long startTime;
  long long tickTime;
  int cnt;
  uint32 dispcnt = 0;
  Dem_EventIdType demError;

  TimeRecorderInit();
  sendcount = 0;
  recvcount = 0;

  /* CanIf_ConfigType��AUTOSAR�̎d�l�ɏ����������߁A�{�o�[�W�����ł͖��g�p�BNULL���w�肷�� */
  CanIf_Init(NULL);
  
  if ((demError = Dem_GetLastError()) != DEM_E_NOERROR) {
    printf("CanIf_Init Failed DemEventId:0x%x\n", demError);
    //    return;
  }

  /* AUTOSAR�̎d�l�ɏ������A�e��ݒ�͐ÓI�ȃf�[�^�Ƃ��ăR���p�C�����Ɋm�肳����B
  �@ �{�T���v���v���O�����ł́A�e�X�g�̂��ߓ����ݒ�𒼐ڏ���������B
     CanIfConfigurations�́ACanIf_Init�̎��s��ɏC������K�v������ */

  /* �h���C�o���̐ݒ� */
  CanIfConfigurations.initConfiguration[RECVCFG].ConfigSet.driverName = driverName;
  CanIfConfigurations.initConfiguration[SENDCFG].ConfigSet.driverName = driverName;
  /* �{�[���[�g�̐ݒ� */
  CanIfConfigurations.initConfiguration[RECVCFG].ConfigSet.baudRate = baud;
  CanIfConfigurations.initConfiguration[SENDCFG].ConfigSet.baudRate = baud;
  /* Accept�t�B���^�̐ݒ� */
  CanIfConfigurations.initConfiguration[RECVCFG].ConfigSet.acceptCode = 0x00000000;
  CanIfConfigurations.initConfiguration[RECVCFG].ConfigSet.acceptMask = 0xffffffff;
  CanIfConfigurations.initConfiguration[SENDCFG].ConfigSet.acceptCode = 0x00000000;
  CanIfConfigurations.initConfiguration[SENDCFG].ConfigSet.acceptMask = 0xffffffff;
  /* PDU-ID�̓o�^ */
  CanIfConfigurations.initConfiguration[RECVCFG].CanIfNumberOfCanRxPduIds = 1;
  CanIfConfigurations.initConfiguration[RECVCFG].CanIfRxPduConfigList[0].CanIfCanRxPduCanId = RECV_PDUID;
  CanIfConfigurations.initConfiguration[RECVCFG].CanIfRxPduConfigList[0].CanIfCanRxPduId = RECV_PDUID;
  CanIfConfigurations.initConfiguration[RECVCFG].CanIfNumberOfCanTxPduIds = 0;
  CanIfConfigurations.initConfiguration[RECVCFG].CanIfNumberOfDynamicCanTxPduIds = 0;

  CanIfConfigurations.initConfiguration[SENDCFG].CanIfNumberOfCanTxPduIds = 1;
  CanIfConfigurations.initConfiguration[SENDCFG].CanIfTxPduConfigList[0].CanIfTxPduIdCanId = SEND_PDUID;
  CanIfConfigurations.initConfiguration[SENDCFG].CanIfTxPduConfigList[0].CanIfCanTxPduId = SEND_PDUID;
  CanIfConfigurations.initConfiguration[SENDCFG].CanIfNumberOfCanRxPduIds = 0;
  CanIfConfigurations.initConfiguration[SENDCFG].CanIfNumberOfDynamicCanTxPduIds = 0;

  /* ��M�R�[���o�b�N�̐ݒ� */
  CanIfConfigurations.initConfiguration[RECVCFG].CanIfRxPduConfigList[0].CanIfRxUserType = CANIF_CAN_NM;
  CanIfConfigurations.initConfiguration[RECVCFG].CanIfRxPduConfigList[0].CanIfUserRxIndication = CanRxCallback;

  CanIfConfigurations.initConfiguration[SENDCFG].CanIfTxPduConfigList[0].CanIfUserTxConfirmtion = CanTxCallback;


  /* �R���g���[���̏����� */
  CanIf_InitController(sendController, SENDCFG);
  CanIf_InitController(recvController, RECVCFG);

  if (CanIf_SetControllerMode(sendController, CANIF_CS_STARTED) != E_OK) {
    printf("CanIf_SetControllerMode(%d, CANIF_CS_STARTED) failed\n", sendController);
    return;
  }
  if (CanIf_SetControllerMode(recvController, CANIF_CS_STARTED) != E_OK) {
    printf("CanIf_SetControllerMode(%d, CANIF_CS_STARTED) failed\n", recvController);
    return;
  }
  Dem_Sleep(10000);
  
  startTime = getNow();
  tickTime = startTime;

  /* ���M���[�v */
  for(cnt = 0; recvcount < sendcountmax; ++cnt) {
    if (recvcount > dispcnt + 1024) { // 8KB���ɕ\��
      long long now = getNow();
      dispcnt = recvcount;
      printf("Tx:%d byte sent queue=%d busy=%d  Rx:%d byte received (%.2f KBytes/sec)\n",
	     sendcount * 8, 
	     transferd,
         sendError,
         recvcount * 8, (recvcount*8)/(1024*(now - startTime)/1000000.0)
	     );
      tickTime = now;
      if (receiveError != 0) break; // ��M�G���[���������Ă����璆�~
    }

    /* ���M�e�X�g */
    if (sendcount < sendcountmax) {
      int result;
      int transferFailed = 0;
      uint32 sdu[2];
      PduInfoType pduInfo;
      struct timespec ts;
      struct timeval tv;
      if (sendcount - recvcount <= queueSize) {
        pduInfo.SduLength = 8;
        sdu[0] = sendcount;
        sdu[1] = ~sendcount;
        pduInfo.SduDataPtr = (uint8*)sdu;
        pthread_mutex_lock(&sendMutex);
        TimeRecorderStart(TR_CANTRANSMIT);
        result = CanIf_Transmit(SEND_PDUID, &pduInfo);
        TimeRecorderStop(TR_CANTRANSMIT);
        if (result == E_OK) {
          ++sendcount;
          ++transferd;
        }
        else {
          // ���M���s
          //printf("#0x%x transfer failed retry.(%d:%d)\n", sendcount, transferd, sendcount - recvcount);
          ++transferFailed;
          ++sendError;
        }
      }
      else {
        pthread_mutex_lock(&sendMutex);
      }

      if (sendcount - recvcount >= queueSize || transferFailed != 0) {
        // wait for tansmit complete
        gettimeofday(&tv, NULL);
        ts.tv_nsec = tv.tv_usec * 1000;
        ts.tv_sec = tv.tv_sec + 1;
        TimeRecorderStart(TR_CANTRANSMITWAIT);
        result = pthread_cond_timedwait(&sendCond, &sendMutex, &ts);
        TimeRecorderStop(TR_CANTRANSMITWAIT);
        pthread_mutex_unlock(&sendMutex);
        if (result == ETIMEDOUT) {
          printf("wait for send callback timeout.\ntest failed.\n");
          break;
        }
      }
      else {
        pthread_mutex_unlock(&sendMutex);
      }
    }
  }
  tickTime = getNow() - startTime;
  if (receiveError != 0) {
    printf("%s", recvErrorMsg);
  }


  printf("Total time: %.3f sec %.2f KBytes/sec (%.1fKbps) transfer speed\n",
         tickTime/1000000.0,
         sendcount*8/(1024*tickTime/1000000.0),
         sendcount*(1+11+1+1+18+1+1+1+4+64+15+1+1+1+7)/(1024*tickTime/1000000.0));

  if (recvcount == sendcountmax) {
    /* �e�X�g���� */
    FILE* f;

    printf("Section Time:\n");
    printTime("\tTransmit", TR_CANTRANSMIT, tickTime);
    printTime("\tTransmitWait", TR_CANTRANSMITWAIT, tickTime);
    printTime("\tCanSendDevice", TR_CAN_SJA10000_CAN_SEND_DEVICE, tickTime);
    printTime("\tCanRecvDevice", TR_CAN_SJA10000_CAN_RECV_DEVICE, tickTime);
    printTime("\tCanIf RxIndication", TR_CANIF_RXINDICATION, tickTime);
    printTime("\tCanIf TxConfirmation", TR_CANIF_TXCONFIRMATION, tickTime);
    printTime("\tCanIf MainRoutine", TR_CANIF_MAINROUTINE, tickTime);

    /* ���ʂ̕ۑ� */
    f = fopen("sample2.log", "r");
    if (f == NULL) {
      f = fopen("sample2.log", "w");
      if (f == NULL) {
        printf("could not create sample2.log\n");
        exit(-1);
      }
      fprintf(f, "baud,queue,size,Total,Transmit,Wait,CanSendDevice,CanRecvDevice,CanIfRxIndication,CanIfTxConfirmation,CanIfMainRoutine\r\n");
    }
    else {
      fclose(f);
      f = fopen("sample2.log", "a");
    }
    fprintf(f, "%d,%d,%d,", baud, queueSize, sendcount*8);
    fprintf(f, "%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f\r\n", 
      TR_PERSEC(tickTime),
      TR_PERSEC(TimeRecorderGetData(TR_CANTRANSMIT)->past),
      TR_PERSEC(TimeRecorderGetData(TR_CANTRANSMITWAIT)->past),
      TR_PERSEC(TimeRecorderGetData(TR_CAN_SJA10000_CAN_SEND_DEVICE)->past),
      TR_PERSEC(TimeRecorderGetData(TR_CAN_SJA10000_CAN_RECV_DEVICE)->past),
      TR_PERSEC(TimeRecorderGetData(TR_CANIF_RXINDICATION)->past),
      TR_PERSEC(TimeRecorderGetData(TR_CANIF_TXCONFIRMATION)->past),
      TR_PERSEC(TimeRecorderGetData(TR_CANIF_MAINROUTINE)->past));
    fclose(f);
  }
  else {
    printf("Test failed. Receive error count:%d\n", receiveError);
  }

  if (CanIf_SetControllerMode(sendController, CANIF_CS_STOPPED) != E_OK) {
    printf("CanIf_SetControllerMode(0, CANIF_CS_STOPPED) failed\n");
  }
  if (CanIf_SetControllerMode(recvController, CANIF_CS_STOPPED) != E_OK) {
    printf("CanIf_SetControllerMode(1, CANIF_CS_STOPPED) failed\n");
  }

  /* �I�������̒ǉ� */
  CanIf_Deinit();
}

