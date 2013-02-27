/*
  CanInterface sample program  

  Copyright(c) 2010 CATS inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 1
#include "Dem.h"
#include "CanInterface/CanIf.h"
#else
#include "/usr/src/CATS_ECU/Autosar/Dem.h"
#include "/usr/src/CATS_ECU/Autosar/Can/CanInterface/CanIf.h"
#endif

extern CanIfConfigurationsType CanIfConfigurations;

void CAN_InterfaceTest(const char* driverName, int sendcountmax, int baud, int config);

#define SEND_PDUID 0x11
#define RECV_PDUID 0x11
#define SENDCFG 0
#define RECVCFG 1

int main(int ac, char* av[]) {
  const char* defaultDriver = "SJA1000";
  int countmax = 100;
  int baud = 500; /* ボーレートの初期値*/
  int config = 0;
  int i;

  printf("CATS AUTOSAR CAN Driver Sample1\n");
  
  for (i = 1; i < ac; ++i) {
    if (av[i][0] == '-') {
      switch(av[i][1]) {
      case 'c': /* count */
        countmax = atoi(av[++i]);
        break;
      case 'b': /* baud */
        baud = atoi(av[++i]);
        break;
      case '0': /* config 0 */
        config = 0;
        break;
      case '1': /* config 1 */
        config = 1;
        break;
      default:
        printf("usage: sample1 [-c sendcount] [-b baud] [-0] [-1]\n"
               "       baud(kbps): 10, 20, 50, 100, 125, 250, 500, 800, 1000\n"
               "       -0        : use configuration type 0 receive callback enabled.(default)\n"
               "       -1        : use configuration type 1 polling mode.\n");
        exit(0);
      }
    }
    else {
      defaultDriver = av[i];
    }
  }
  CAN_InterfaceTest(defaultDriver, countmax, baud, config);
  return 0;
}


uint8 sendController = 0; /* 送信テストのコントローラID */
uint8 recvController = 1; /* 受信テストのコントローラID */
int sendcount = 0;
int recvcount = 0;
PduInfoType recvData;


// 送信カウントと受信カウントの差が、この値以上になったら
// 送信動作を停止する。
int sub_loop_limit = 500;
int sub_loop_trg = 0;


/* 送信コールバック */
void CanTxCallback(PduIdType pduId) {
  printf("TransmitCB %d\n", pduId);
}

/* 受信コールバック */
void CanRxCallback(PduIdType pduId, const void* data) {
  if (pduId == RECV_PDUID) {
    if (CanIf_ReadRxPduData(pduId, &recvData) == E_OK) {
      /* 受信データあり */
      printf("ReceiveCB  ch:%d ID:%x '%8.8s'\n", 
	     recvController, pduId, recvData.SduDataPtr);
      ++recvcount;
    }
    else {
      printf("ReceiveCB no data.\n");
    }
  }
  else {
    printf("ReceiveCB invalid PduID 0x%x\n", pduId);
  }
}


void CAN_InterfaceTest(const char* driverName, int sendcountmax, int baud, int config) {
  int cnt;
  sendcount = 0;
  recvcount = 0;
  time_t t_start, t_end;
	Std_ReturnType std_ret;

  /* CanIf_ConfigTypeはAUTOSARの仕様に準拠したため、本バージョンでは未使用。NULLを指定する */
  CanIf_Init(NULL);

  /* AUTOSARの仕様に準拠し、各種設定は静的なデータとしてコンパイル時に確定させる。
  　 本サンプルプログラムでは、テストのため内部設定を直接書き換える。
     CanIfConfigurationsは、CanIf_Initの実行後に修正する必要がある */

  /* ドライバ名の設定 */
  CanIfConfigurations.initConfiguration[RECVCFG].ConfigSet.driverName = driverName;
  CanIfConfigurations.initConfiguration[SENDCFG].ConfigSet.driverName = driverName;
  /* ボーレートの設定 */
  CanIfConfigurations.initConfiguration[RECVCFG].ConfigSet.baudRate = baud;
  CanIfConfigurations.initConfiguration[SENDCFG].ConfigSet.baudRate = baud;
  /* Acceptフィルタの設定 */
  CanIfConfigurations.initConfiguration[RECVCFG].ConfigSet.acceptCode = 0x00000000;
  CanIfConfigurations.initConfiguration[RECVCFG].ConfigSet.acceptMask = 0xffffffff;
  CanIfConfigurations.initConfiguration[SENDCFG].ConfigSet.acceptCode = 0x00000000;
  CanIfConfigurations.initConfiguration[SENDCFG].ConfigSet.acceptMask = 0xffffffff;
  /* PDU-IDの登録 */
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

  /* 受信コールバックの設定 */
  CanIfConfigurations.initConfiguration[RECVCFG].CanIfRxPduConfigList[0].CanIfRxUserType = CANIF_CAN_NM;
	if(config == 0) {
		CanIfConfigurations.initConfiguration[RECVCFG].CanIfRxPduConfigList[0].CanIfUserRxIndication = CanRxCallback;
		CanIfConfigurations.initConfiguration[SENDCFG].CanIfTxPduConfigList[0].CanIfUserTxConfirmtion = CanTxCallback;
	} else {
		CanIfConfigurations.initConfiguration[RECVCFG].CanIfRxPduConfigList[0].CanIfUserRxIndication = NULL;
		CanIfConfigurations.initConfiguration[SENDCFG].CanIfTxPduConfigList[0].CanIfUserTxConfirmtion = NULL;
	}

  /* コントローラの初期化 */
  CanIf_InitController(sendController, SENDCFG);
  CanIf_InitController(recvController, RECVCFG);


  if (CanIf_SetControllerMode(sendController, CANIF_CS_STARTED) != E_OK) {
    printf("CanIf_SetControllerMode(0, CANIF_CS_STARTED) failed\n");
    return;
  }
  if (CanIf_SetControllerMode(recvController, CANIF_CS_STARTED) != E_OK) {
    printf("CanIf_SetControllerMode(1, CANIF_CS_STARTED) failed\n");
    return;
  }


  time(&t_start);
  for(cnt = 0; (cnt < (sendcountmax * 5) && recvcount < sendcountmax); ++cnt) {
    Dem_Sleep(1000);

    /* 送信テスト */

    if(sub_loop_trg == 0 && sendcount < sendcountmax) {
		PduInfoType pduInfo;
		uint8 buf[8];
		sprintf((char*)buf, "UU%4.4d*", cnt);
		buf[7] = 0x00;
		pduInfo.SduDataPtr = buf;
		pduInfo.SduLength = 8;

		do{	/* エラーの時はリトライして待つ */
			std_ret = CanIf_Transmit(SEND_PDUID, &pduInfo);
			printf("CanIf_Transmit std_ret = %d, cnt = %d\n",std_ret ,cnt);
		}while(std_ret != E_OK);

		printf("Transmit ch:%d to ch:%d: ID:%x '%8.8s'\n", 
			sendController, recvController, SEND_PDUID, pduInfo.SduDataPtr);
		++sendcount;
	}


	// ヒステリシスを持った、送信の制御
	if(sub_loop_trg == 0) {
		if((sendcount - recvcount) >= sub_loop_limit) sub_loop_trg = 1;
	} else {
	  	if((sendcount - recvcount) < (sub_loop_limit / 4)) sub_loop_trg = 0;
	}


    if (config == 1) {
      /* 受信テスト */
      if (CanIf_ReadRxPduData(RECV_PDUID, &recvData) == E_OK) {
        /* 受信データあり */
        printf("Receive  ch:%d ID:%x '%8.8s'\n", 
	       recvController, RECV_PDUID, recvData.SduDataPtr);
        ++recvcount;
		
      }
    }
  }
  if (recvcount == sendcountmax) {
    printf("All data received. (%d/%d)\n", recvcount, sendcountmax);
  }
  else {
    printf("%d data lost.(%d/%d)\n", sendcountmax - recvcount, recvcount, sendcountmax);
  }
  time(&t_end);
  printf("Total time: %d [Sec]\n", (t_end - t_start));

  /* 終了処理の追加 */
  CanIf_Deinit();
}

