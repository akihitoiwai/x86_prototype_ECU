/* CanIf.c


*/
#include <pthread.h>
#include "Platform_Types.h"
#include "Dem.h"
#include "CanIf.h"
#include "CanDriver/Can.h"
#include "CanIf_sub.h"
#ifdef TEST
#include "Test/util/TimeRecorder.h"
#endif

/* 本来なら動的に読み込むので不要になる */
#include "CanDriver/SJA1000/Can_SJA1000.h"


#ifdef TEST

#include "CanDriver/test/Can_test.h"
//const char* CanDriverNames[] = { CAN_DRIVERNAME_TEST, NULL };
//#define NUMBER_OF_CANHWUNITS 2
const char* CanDriverNames[] = { CAN_DRIVERNAME_SJA1000, CAN_DRIVERNAME_TEST, NULL };
#define NUMBER_OF_CANHWUNITS 4

#else

const char* CanDriverNames[] = { CAN_DRIVERNAME_SJA1000, NULL };
#define NUMBER_OF_CANHWUNITS 4

#endif


/* 本来は静的に確定される設定データ */
CanIfConfigurationsType CanIfConfigurations;

Can_FunctionTable* CanDriverList[NUMBER_OF_CANDRIVERS]; /* CanDriverのリスト */
Can_FunctionTable* CanDriver[NUMBER_OF_CANCONTROLLER]; /* controllerからCanDriverを得る */
CanIf_ControllerModeType CanControllerMode[NUMBER_OF_CANCONTROLLER];


/* 名称からドライバを検索する */
Can_FunctionTable* CanIf_GetCanDriverByName(const char* name) {
  if (strcmp(name, CAN_DRIVERNAME_SJA1000) == 0) {
    /* SJA1000 */
    return Can_GetFunctionTable_SJA1000();
  }
#ifdef TEST
  else if (strcmp(name, CAN_DRIVERNAME_TEST) == 0) {
    /* test */
    return Can_GetFunctionTable_TEST();
  }
#endif
  return 0; /* not found */
}

static int gCanInitialized = 0;

/* 初期化 */
void CanIf_Init(const CanIf_ConfigType * ConfigPtr) {
  int i,j;
  int failed = 0;
  Can_FunctionTable* candrv = 0;


  /* Configurationの初期化 */
  memset(&CanIfConfigurations, 0, sizeof(CanIfConfigurations));
  for (i = 0; i < NUMBER_OF_CANDRIVERS; ++i) {
    CanIfConfigurations.driverConfig[i].CanIfDriverNameRef = CanDriverNames[i];
    CanIfConfigurations.driverConfig[i].CanIfReceiveIndication = True;
  }
  for (i = 0; i < NUMBER_OF_CANCONTROLLER; ++i) {
    CanIfConfigurations.controllerConfig[i].CanIfControllerIdRef = i;
    CanIfConfigurations.controllerConfig[i].CanIfDriverNameRef = 0;
    CanIfConfigurations.controllerConfig[i].CanIfInitControllerRef = 0;
  }
  for (i = 0; i < NUMBER_OF_CANCONFIGURATIONS; ++i) {
    CanIfConfigurations.initConfiguration[i].ConfigSet.driverName = CanDriverNames[i];
    CanIfConfigurations.initConfiguration[i].ConfigSet.baudRate = 1000;
    CanIfConfigurations.initConfiguration[i].ConfigSet.acceptCode = 0x00000000;
    CanIfConfigurations.initConfiguration[i].ConfigSet.acceptMask = 0xffffffff;
    CanIfConfigurations.initConfiguration[i].CanIfNumberOfCanRxPduIds = 0;
    CanIfConfigurations.initConfiguration[i].CanIfNumberOfCanTxPduIds = 0;
    CanIfConfigurations.initConfiguration[i].CanIfNumberOfDynamicCanTxPduIds = 0;
    for (j = 0; j < NUMBER_OF_PDU_MAX; ++j) {
      /* PeriCAN mode , DLC=8 固定 */
      CanIfConfigurations.initConfiguration[i].CanIfRxPduConfigList[j].CanIfRxPduIdCanIdType = CANIF_EXTENDED_CAN;
      CanIfConfigurations.initConfiguration[i].CanIfRxPduConfigList[j].CanIfCanRxPduIdDlc = 8;
      CanIfConfigurations.initConfiguration[i].CanIfTxPduConfigList[j].CanIfTxPduIdCanIdType = CANIF_EXTENDED_CAN;
      CanIfConfigurations.initConfiguration[i].CanIfTxPduConfigList[j].CanIfCanTxPduType = CANIF_DYNAMIC;
      CanIfConfigurations.initConfiguration[i].CanIfTxPduConfigList[j].CanIfCanTxPduIdDlc = 8;
    }
  };
  CanIfConfigurations.publicConfiguration.CanIfDevErrorDetect = False;
  CanIfConfigurations.publicConfiguration.CanIfMultipleDriverSupport = True;
  CanIfConfigurations.publicConfiguration.CanIfNumberOfCanHwUnits = NUMBER_OF_CANHWUNITS;
  CanIfConfigurations.publicConfiguration.CanIfReadRxPduDataApi = True;
  CanIfConfigurations.publicConfiguration.CanIfReadRxPduNotifyStatusApi = False;  
  CanIfConfigurations.publicConfiguration.CanIfReadTxPduNotifyStatusApi = False;
  CanIfConfigurations.publicConfiguration.CanIfSetDynamicTxIdApi = False;
  CanIfConfigurations.publicConfiguration.CanIfVersionInfoApi = True;
  CanIfConfigurations.publicConfiguration.CanIfWakeupEventApi = False;
  CanIfConfigurations.privateConfiguration.CanIfDlcCheck = False;
  CanIfConfigurations.privateConfiguration.CanIfNumberOfTxBuffers = 128;
  CanIfConfigurations.privateConfiguration.CanIfSoftwareFilterType = CANIF_BINARY;


  CanIf_subInit();

  memset(CanControllerMode, 0, sizeof(CanControllerMode));
  memset(CanDriver, 0, sizeof(CanDriver));
  memset(CanDriverList, 0, sizeof(CanDriverList));


  /* ドライバの初期化 */
  for (i = 0; CanDriverNames[i] != NULL; ++i) {
    Can_ConfigType canconfig;
    candrv = CanIf_GetCanDriverByName(CanDriverNames[i]);
    if (candrv == 0) {
      DebugOut1("CanIf_Init invalid driver name %s\n", CanDriverNames[i]);
    }
    else {
      candrv->Can_Init(&canconfig);
      if (Dem_GetLastError() == DEM_E_NOERROR) {
	canconfig.deviceId = CanIf_addCanDriver(candrv);
      }
      else {
	/* Can_Init failed */
	++failed;
      }
    }
  }
  /* ポーリング用スレッドの生成 */
  CanIf_CreateThread();

  if (failed) {
    Dem_ReportErrorStatus(CAN_E_TRANSITION, DEM_EVENT_STATUS_FAILED);
  } else {
	gCanInitialized = 1;
  }
}

/* コントローラ単位での初期化 */
void CanIf_InitController(uint8 controller, uint8 configurationIndex) {
  uint16 id;
  Can_FunctionTable* candrv = 0;
  CanIfControllerConfig* controllerConfig = 0;
  CanIfInitConfiguration* initConfiguration = 0;

  if (controller >= NUMBER_OF_CANCONTROLLER) {
    DebugOut("CanIf_InitController: out of controller count\n");
	Dem_ReportErrorStatus(CANIF_E_PARAM_CONTROLLER, DEM_EVENT_STATUS_FAILED);
    return;
  }
  if (configurationIndex >= NUMBER_OF_CANCONFIGURATIONS) {
    DebugOut("CanIf_InitController: out of configurationIndex count\n");
	Dem_ReportErrorStatus(CANIF_E_PARAM_POINTER, DEM_EVENT_STATUS_FAILED);
    return;
  }
  /* コントローラ毎の設定 */
  controllerConfig = &CanIfConfigurations.controllerConfig[controller];

  /* 初期化設定 */
  initConfiguration = &CanIfConfigurations.initConfiguration[configurationIndex];

  controllerConfig->CanIfControllerIdRef = controller;
  controllerConfig->CanIfInitControllerRef = configurationIndex;
  controllerConfig->CanIfDriverNameRef = initConfiguration->ConfigSet.driverName;

  candrv = CanIf_GetCanDriverByName(initConfiguration->ConfigSet.driverName);

  if (candrv == 0) {
    DebugOut1("CanIf_InitController: invalid CanDriver name:%d\n", controller);
	Dem_ReportErrorStatus(CANIF_E_UNINIT, DEM_EVENT_STATUS_FAILED);
    return;
  }
  CanDriver[controller] = candrv;
  {
    Can_ControllerConfigType canConfig;
    canConfig.deviceId = controller;
    canConfig.baudRate = initConfiguration->ConfigSet.baudRate;
    canConfig.btr0 = initConfiguration->ConfigSet.btr0;
    canConfig.btr1 = initConfiguration->ConfigSet.btr1;
    canConfig.acceptCode = initConfiguration->ConfigSet.acceptCode;
    canConfig.acceptMask = initConfiguration->ConfigSet.acceptMask; 
    candrv->Can_InitController(controller, &canConfig);
  }
  CanControllerMode[controller] = CANIF_CS_STOPPED; /* 初期状態はCANIF_CS_STOPPED */

  /* PduIDの登録 */
  for (id = 0; id < initConfiguration->CanIfNumberOfCanRxPduIds; ++id) {
    AddRxPduTable(initConfiguration->CanIfRxPduConfigList[id].CanIfCanRxPduId,
                controller,
                initConfiguration->CanIfRxPduConfigList[id].CanIfCanRxPduCanId);
  }
  for (id = 0; id < initConfiguration->CanIfNumberOfCanTxPduIds; ++id) {
    AddTxPduTable(initConfiguration->CanIfTxPduConfigList[id].CanIfCanTxPduId,
                controller,
                initConfiguration->CanIfTxPduConfigList[id].CanIfTxPduIdCanId);
  }

}

/* コントローラモードの設定 */
Std_ReturnType CanIf_SetControllerMode(uint8 controller, CanIf_ControllerModeType controllerMode) {
  Can_ReturnType result = CAN_NOT_OK;

  if (controller >= NUMBER_OF_CANCONTROLLER) {
    DebugOut("CanIf_SetControllerMode: out of Controller num\n");
    return E_NOT_OK;
  }
  if (controllerMode == CANIF_CS_UNINIT || controllerMode > CANIF_CS_SLEEP) {
    DebugOut("CanIf_SetControllerMode: invalid ControllerMode\n");
    return E_NOT_OK;
  }
 
  if (CanDriver[controller] == 0) {
    DebugOut("CanIf_SetControllerMode: invalid Controller num\n");
    return E_NOT_OK;
  }

  switch (controllerMode) {
  case CANIF_CS_STOPPED:
    result = CanDriver[controller]->Can_SetControllerMode(controller, CAN_T_STOP);
    break;
  case CANIF_CS_STARTED:
    /* 起動中はCAN_T_STARTを、停止中はCAN_T_WAKEUPを送る */
    if (CanControllerMode[controller] != CANIF_CS_STARTED) {
      result = CanDriver[controller]->Can_SetControllerMode(controller, CAN_T_START);
    }
    else {
      result = CanDriver[controller]->Can_SetControllerMode(controller, CAN_T_WAKEUP);
    }
    break;
  case CANIF_CS_SLEEP:
    result = CanDriver[controller]->Can_SetControllerMode(controller, CAN_T_SLEEP);
    break;
  default:
    break;
  }
  if (result == CAN_OK) {
    CanControllerMode[controller] = controllerMode;
    return E_OK;
  }
  else {
    return E_NOT_OK;
  }
}


/* コントローラモードの取得 */
Std_ReturnType CanIf_GetControllerMode(uint8 Controller, CanIf_ControllerModeType * ControllerModePtr)
{

  if (Controller >= NUMBER_OF_CANCONTROLLER) {
    DebugOut("CanIf_GetControllerMode: out of Controller num\n");
    return E_NOT_OK;
  }

  if (ControllerModePtr == NULL) {
    DebugOut("CanIf_GetControllerMode: invalid ControllerModePtr\n");
    return E_NOT_OK;
  }
 
  if (CanDriver[Controller] == NULL) {
    DebugOut("CanIf_GetControllerMode: invalid Controller num\n");
    return E_NOT_OK;
  }

  *ControllerModePtr = CanControllerMode[Controller];

  return E_OK;
}


/* 送信処理 */
Std_ReturnType CanIf_Transmit(PduIdType CanTxPduId, const PduInfoType * pduInfo) {
  CanPduIdTableType* pduIdTable;
  Can_PduType canpdu;
  if(gCanInitialized == 0) {
	  DebugOut("CanIf_Transmit: not initialization path\n");
	  return E_NOT_OK;
  }

  pduIdTable = CanIf_FindTxPduIdTable(CanTxPduId);
  if (pduIdTable == NULL) {
    DebugOut1("CanIf_Transmit: Invalid PduId: %d\n", CanTxPduId);
    return E_NOT_OK;
  }

  if (pduIdTable->controller >= NUMBER_OF_CANCONTROLLER) {
    DebugOut("CanIf_Transmit: out of Controller num\n");
    return E_NOT_OK;
  }
  if (CanDriver[pduIdTable->controller] == 0) {
    DebugOut("CanIf_Transmit: invalid Controller num\n");
    return E_NOT_OK;
  }

  canpdu.id = pduIdTable->canPduId;
  canpdu.length = pduInfo->SduLength;
  canpdu.sdu = pduInfo->SduDataPtr;
  canpdu.swPduHandle = CanTxPduId;
  switch(CanDriver[pduIdTable->controller]->Can_Write(pduIdTable->controller, &canpdu)) {
  case CAN_OK:
    return E_OK;
  case CAN_BUSY:
    return E_NOT_OK;
  default:
    return E_NOT_OK;
  }
}

/* 受信通知コールバック */
void CanIf_RxIndication(uint8 Hrh, Can_IdType CanId, uint8 CanDlc, const uint8 * CanSduPtr) {
  uint8 controller = Hrh; /* トランシーバーハンドルとコントローラ番号は等しいとする */
  CanIfReceiveBuffer* rb;
  CanIfReceiveBufferData* data;
  CanPduIdTableType* pduIdTable = 0;

  if (controller >= NUMBER_OF_CANCONTROLLER) {
    DebugOut("CanIf_RxIndication: out of Controller num\n");
    return ;
  }
#ifdef TEST
  TimeRecorderStart(TR_CANIF_RXINDICATION);
#endif

  rb = CanIf_GetReceiveBuffer(controller);
  CanIfReceiveBuffer_lock(rb);
  data = CanIfReceiveBuffer_write(rb);
  if (data != 0) {
    memcpy(data->sdu, CanSduPtr, 8);
    data->dlc = CanDlc;
    data->CanId = CanId;
  }
  CanIfReceiveBuffer_unlock(rb);
  pduIdTable = CanIf_FindCanIdToRxPduIdTable(controller, CanId);

  /* ユーザーコールバックルーチンの呼び出し */
  if (pduIdTable != NULL) {
    uint32 conf = CanIfConfigurations.controllerConfig[controller].CanIfInitControllerRef;
    uint32 n;
    for (n = 0; n < CanIfConfigurations.initConfiguration[conf].CanIfNumberOfCanRxPduIds; ++n) {
      if (CanIfConfigurations.initConfiguration[conf].CanIfRxPduConfigList[n].CanIfCanRxPduId == pduIdTable->pduId) {
        CanIf_RxCallbackType cb = CanIfConfigurations.initConfiguration[conf].CanIfRxPduConfigList[n].CanIfUserRxIndication;
        if (cb != NULL) {
          void* cbdata = 0;
          PduInfoType pduinfo;
          switch(CanIfConfigurations.initConfiguration[conf].CanIfRxPduConfigList[n].CanIfRxUserType) {
          default:
          case CANIF_CAN_NM:
            cbdata = (void*)CanSduPtr;
            break;
          case CANIF_CAN_TP:
            {
              pduinfo.SduLength = CanDlc;
              pduinfo.SduDataPtr = (uint8*)CanSduPtr;
              cbdata = (void*)&pduinfo;
            }
            break;
          case CANIF_PDUR:
            cbdata = (void*)CanSduPtr;
            break;
          }
          cb(pduIdTable->pduId, cbdata);
        }
      }
    }
  }
#ifdef TEST
  TimeRecorderStop(TR_CANIF_RXINDICATION);
#endif
}

/* 送信完了コールバック */
void CanIf_TxConfirmation(PduIdType canTxPduId) {
  uint32 n;
  uint8 controller = GetTxController(canTxPduId);
  uint32 conf;
  if (controller > NUMBER_OF_CANCONTROLLER) {
    DebugOut1("CanIf_TxConfirmation: Invalid PduId:0x%x\n", canTxPduId);
    return;
  }
#ifdef TEST
  TimeRecorderStart(TR_CANIF_TXCONFIRMATION);
#endif
  /* ユーザーコールバックルーチンの呼び出し */
  conf = CanIfConfigurations.controllerConfig[controller].CanIfInitControllerRef;
  for (n = 0; n < CanIfConfigurations.initConfiguration[conf].CanIfNumberOfCanTxPduIds; ++n) {
    if (CanIfConfigurations.initConfiguration[conf].CanIfTxPduConfigList[n].CanIfCanTxPduId == canTxPduId) {
      CanIf_TxCallbackType cb = CanIfConfigurations.initConfiguration[conf].CanIfTxPduConfigList[n].CanIfUserTxConfirmtion;
      if (cb != NULL) {
        cb(canTxPduId);
      }
    }
  }
  //  DebugOut2("CanIf_TxConfirmation: controller:%d PduId=0x%x\n", controller, canTxPduId);
#ifdef TEST
  TimeRecorderStop(TR_CANIF_TXCONFIRMATION);
#endif
}

/* BusOffコールバック */
void CanIf_ControllerBusOff(uint8 controller) {
  DebugOut1("CanIf_ControllerBusOff:%d\n", controller);
  if (CanIfConfigurations.dispatchConfig.CanIfBusOffNotification != 0) {
    CanIfConfigurations.dispatchConfig.CanIfBusOffNotification(controller);
  }
}

Std_ReturnType CanIf_CheckWakeup(EcuM_WakeupSourceType WakeupSource) {
  return E_OK;
}

Std_ReturnType CanIf_CheckValidation(EcuM_WakeupSourceType WakeupSource) {
  return E_OK;
}



Std_ReturnType CanIf_ReadRxPduData(PduIdType canRxPduId, PduInfoType * pduInfoPtr) {
  uint8 controller = GetRxController(canRxPduId);
  CanIfReceiveBuffer* rb;
  const CanIfReceiveBufferData* data = 0;
  if (controller >= NUMBER_OF_CANCONTROLLER) {
    DebugOut("CanIf_ReadRxPduData: out of Controller num\n");
    return E_NOT_OK;
  }
  rb = CanIf_GetReceiveBuffer(controller);
  if (CanIfReceiveBuffer_CheckReceivedData(rb) > 0) {
    /* 受信データあり */
    CanIfReceiveBuffer_lock(rb);
    data = CanIfReceiveBuffer_read(rb);
    if (data != 0) {
      pduInfoPtr->SduLength = data->dlc;
      pduInfoPtr->SduDataPtr = (uint8*)data->sdu;
    }
    CanIfReceiveBuffer_unlock(rb);
  }
  if (data == 0) return E_NOT_OK;
  return E_OK;
}

void CanIf_Deinit() {
  int i;
  int failed = 0;

  CanIf_ExitThread();

  for (i = 0; i < NUMBER_OF_CANDRIVERS; ++i) {
    Can_FunctionTable* candrv = CanDriverList[i]; /* CanDriverのリスト */
    if (candrv != 0) {
      candrv->Can_Deinit();
    }
  }

  if(failed) {
	Dem_ReportErrorStatus(CAN_E_TRANSITION, DEM_EVENT_STATUS_FAILED);
  }
  gCanInitialized = 0;
}



/* ポーリング用のメインルーチン */
void CanIf_MainRoutine() {
  int i;
#ifdef TEST
  TimeRecorderStart(TR_CANIF_MAINROUTINE);
#endif
  for (i = 0; i < NUMBER_OF_CANDRIVERS; ++i) {
    Can_FunctionTable* candrv = CanDriverList[i]; /* CanDriverのリスト */
    if (candrv != 0) {
      candrv->Can_MainFunction_BusOff();
      candrv->Can_MainFunction_Wakeup();
      candrv->Can_MainFunction_Read();
      candrv->Can_MainFunction_Write();
    }
  }
#ifdef TEST
  TimeRecorderStop(TR_CANIF_MAINROUTINE);
#endif
}




#if 0
int CanIf_wait(int msec) {
  int result;
  CanIf_lock():
  if (msec == -1) {
    result = pthread_cond_wait(&gCanIfCond, &gCanIfMutex);
  }
  else {
    struct timeval now;
    struct timespec t;
    if (!gettimeofday(&now, NULL)) {
      now.tv_usec += (msec % 1000) * 1000;
      while(now.tv_usec > 1000000) {
        now.tv_sec += 1;
        now.tv_usec -= 1000000;
      }
      t.tv_sec = now.tv_sec + (msec / 1000);
      t.tv_nsec = now.tv_usec * 1000;
    }
    else {
      // gettimeofdayに失敗した場合は秒単位
      t.tv_sec = time(NULL) + (msec / 1000);
      t.tv_nsec = (msec % 1000) * 1000000;
    }
    result = pthread_cond_timedwait(&gCanIfCond, &gCanIfMutex, &t); 
  }
  CanIf_unlock();
  return result; /* ETIMEOUT or EINTR or EBUSY */
}
#endif

