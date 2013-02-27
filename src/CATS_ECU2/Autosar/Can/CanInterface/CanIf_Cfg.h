/* CanIf_Cfg.h


*/

#ifndef CANIF_CFG_H
#define CANIF_CFG_H

#include "CanDriver/Can_Cfg.h"

#define NUMBER_OF_CANCONTROLLER 4     /* Number of Controller */
#define NUMBER_OF_CANCONFIGURATIONS 4    /* Numver of CanIf Configuration count */

#define NUMBER_OF_PDU_MAX 1024
#define NUMBER_OF_TX_PDUS 1024

#define NUMBER_OF_CANTRANSCEIVERCONFIG 4
#define NUMBER_OF_CANDRIVERS 4 /* Number of CAN Controller device type */


enum { CANIF_CAN_NM, CANIF_CAN_TP, CANIF_PDUR }; 
enum { CANIF_EXTENDED_CAN, CANIF_STANDARD_CAN };
enum { CANIF_DYNAMIC, CANIF_STATIC };
enum { CANIF_BINARY, CANIF_INDEX, CANIF_LINEAR, CANIF_TABLE };

/* 
  Container Name: CanIfPrivateConfiguration
  Description:    This container contains the private configuration (parameters) of the CANInterface.  
  */
typedef struct {
  /* 
    Name: CanIfDlcCheck (not supported)
    Description:  Selects whether the DLC check is supported. True: Enabled False: Disabled
    */  
  BooleanParamDef CanIfDlcCheck; 
  /*
    Name: CanIfNumberOfTxBuffers (not supported)
    Description: Defined the number of L-PDU elements for the transmit buffering. The Tx 
    L-PDU buffers shall be used to store an L-PDU once for each different L-PDU handle.
    Range: 0..max. number of Tx L-PDUs to be used. Default Value: NUMBER_OF_TX_PDUS
    */
  uint32  CanIfNumberOfTxBuffers;
  /*
    Name: CanIfSoftwareFilterType (not supported)
    Description: Selects the desired software filter mechanism for reception only. Each 
    implemented software filtering method is identified by this enumeration 
    number. Range: Types implemented software filtering methods  
      BINARY  Selects Binary Filter method.  
      INDEX   Selects Index Filter method.  
      LINEAR  Selects Linear Filter method.  
      TABLE   Selects Table Filter method
    */
  uint32 CanIfSoftwareFilterType;

} CanIfPrivateConfiguration;

/*
  Container Name: CanIfPublicConfiguration 
  Description:    This container contains the public configuration (parameters) of the CAN Interface.  
*/
typedef struct {
  /*
    Name: CanIfDevErrorDetect
    Description: Enables and disables the development error detection and notification 
    mechanism. True: Enabled False: Disabled
    */
  BooleanParamDef CanIfDevErrorDetect;

  /*
    Name: CanIfMultipleDriverSupport
    Description: Selects support for multiple CAN Drivers. True: Enabled False: Disabled
    */
  BooleanParamDef CanIfMultipleDriverSupport;

  /*
    Name: CanIfNumberOfCanHwUnits
    Description: Number of served CAN hardware units. Range: 1..max. number of 
    underlying supported CAN Hardware units  
    */
  uint32 CanIfNumberOfCanHwUnits;

  /*
    Name: CanIfReadRxPduDataApi
    Description: Enables / Disables the API CanIf_ReadRxPduData() for reading received 
    L-PDU data. True: Enabled False: Disabled
    */
  BooleanParamDef CanIfReadRxPduDataApi;

  /*
    Name: CanIfReadRxPduNotifyStatusApi
    Description: Enables and disables the API for reading the received L-PDU data. True: 
    Enabled False: Disabled  
    */
  BooleanParamDef CanIfReadRxPduNotifyStatusApi;

  /*
    Name: CanIfReadTxPduNotifyStatusApi
    Description: Enables and disables the API for reading the notification status of transmit 
    and receive L-PDUs. True: Enabled False: Disabled  
    */
  BooleanParamDef CanIfReadTxPduNotifyStatusApi;

  /*
    Name: CanIfSetDynamicTxIdApi
    Description: Enables and disables the API for reconfiguration of the CAN Identifier for 
    each Transmit L-PDU. True: Enabled False: Disabled 
    */
  BooleanParamDef CanIfSetDynamicTxIdApi;

  /*
    Name: CanIfVersionInfoApi
    Description: Enables and disables the API for reading the version information about the 
    CAN Interface. True: Enabled False: Disabled
    */
  BooleanParamDef CanIfVersionInfoApi;

  /*
    Name CanIfWakeupEventApi
    Description: Enables and disables the API for wakeup support called by the CAN 
    Driver. True: Enabled False: Disabled
    */
  BooleanParamDef CanIfWakeupEventApi;

} CanIfPublicConfiguration;

/*
  Container Name: CanIfInitControllerConfig{CanInterfaceInitControllerConfiguration} 
  Description: This container contains the references to the configuration setup of each 
  underlying CAN driver.  
*/
typedef struct {
  uint32 CanIfControllerRefConfigSet;
} CanIfInitControllerConfig;


/*
  Container Name: CanIfHrhRangeConfig{CanInterfaceHrhRangeConfiguration } 
  Description: Defines the parameters required for configuraing multiple CANID ranges 
  for a given same HRH.  
*/
typedef struct {
  uint32 CanIfRxPduLowerCanId;
  uint32 CanIfRxPduUpperCanId;
} CanIfHrhRangeConfig;

/*
  Container Name: CanIfHrhConfig{CanInterfaceHrhConfiguration} 
  Description: This container contains configuration parameters for each hardware 
  receive object (HRH).  
*/
typedef struct {
  uint32 CanIfHrhType; /* BASIC_CAN, FULL_CAN */
  BooleanParamDef CanIfSoftwareFilterHrh;
  uint32 CanIfCanControllerHrhIdRef;
  CanHardwareObject* CanIfHrhIdSymRef;
  CanIfHrhRangeConfig* CanIfHrhRangeConfigList;
} CanIfHrhConfig;


/*
  Container Name:  CanIfHthConfig{CanInterfaceHthConfiguration} 
  Description: This container contains parameters related to each HTH.  
*/
typedef struct {
  uint32 CanIfHthType; /* BASIC_CAN, FULL_CAN */
  uint32 CanIfCanControllerIdRef;
  CanHardwareObject* CanIfHthIdSymRef;
} CanIfHthConfig;

/*
  Container Name: CanIfInitHohConfig 
  Description: This container contains the references to the configuration setup of each 
  underlying CAN Driver.  
*/
typedef struct {
  uint32 CanIfRefConfigSet;
  CanIfHrhConfig* CanIfHrhConfigList;
  CanIfHthConfig* CanIfHthConfigList;
} CanIfInitHohConfig;

/*
  Container Name: CanIfDriverConfig{CanInterfaceDriverConfiguration} 
  Description: Configuration parameters for all the underlying CAN drivers are 
  aggregated under this container.  
*/
typedef struct {
  BooleanParamDef CanIfBusoffNotification;
  BooleanParamDef CanIfReceiveIndication;
  BooleanParamDef CanIfTransmitCancellation;
  BooleanParamDef CanIfTxConfirmation;
  BooleanParamDef CanIfWakeupNotification;
  const char* CanIfDriverNameRef; /* Driver Name */
  CanIfInitHohConfig* CanIfInitHohConfigRef; /* not supported */
} CanIfDriverConfig;

/*
  Container Name: CanIfControllerConfig
  Description: This container contains the configuration (parameters) of all addressed 
  CAN controllers by each underlying CAN driver.  
*/
typedef struct {
  uint32 CanIfWakeupSupport; /* CONTROLLER, NO_WAKEUP, TRANSCEVIER (not supported) */
  uint32 CanIfControllerIdRef; 
  const char* CanIfDriverNameRef; /* CanIfDriverConfig */
  uint32 CanIfInitControllerRef; /* CanIfInitControllerConfig */

} CanIfControllerConfig;

/* 
  Container Name: CanIfTxPduConfig
  Description: This container contains the configuration (parameters) of each transmit 
  CAN L-PDU. The SHORT-NAME of "CanIfTxPduConfig" container 
  represents the symolic name of Transmit L-PDU.  
*/
typedef struct {
  PduIdType CanIfCanTxPduId;
  uint32 CanIfTxPduIdCanId;
  uint32 CanIfCanTxPduIdDlc;
  uint32 CanIfCanTxPduType; /* DYNAMIC: CAN ID is defined at runtime, STATIC: CAN ID is defined at compile-time */
  BooleanParamDef CanIfReadTxPduNotifyStatus;
  uint32 CanIfTxPduIdCanIdType; /* EXTENDED_CAN, STANDARD_CAN */
  uint32 CanIfTxUserType; /* CAN_NM, CAN_TP, PDUR */
  CanIf_TxCallbackType CanIfUserTxConfirmtion;
  CanIfHthConfig* CanIfCanTxPduHthRef[NUMBER_OF_PDU_MAX]; /* not supported */
  Can_PduType* PduIdRef; /* not supported */
} CanIfTxPduConfig;

/* 
  Container Name: CanIfRxPduConfig
  Description: This container contains the configuration (parameters) of each receive 
  CAN L-PDU. The SHORT-NAME of "CanIfRxPduConfig" container itselfrepresents the symolic
  name of Receive L-PDU.  
*/
typedef struct {
  uint32 CanIfCanRxPduCanId;
  uint32 CanIfCanRxPduIdDlc;
  PduIdType CanIfCanRxPduId;
  BooleanParamDef CanIfReadRxPduData;
  BooleanParamDef CanIfReadRxPduNotifyStatus;
  uint32 CanIfRxPduIdCanIdType; /* 0:CANIF_EXTENDED_CAN, 1:CANIF_STANDARD_CAN */
  uint32 CanIfRxUserType; /* 0:CANIF_CAN_NM, 1:CANIF_CAN_TP, 2:CANIF_PDUR */
  CanIf_RxCallbackType CanIfUserRxIndication;
  CanIfHthConfig* CanIfCanRxPduHthRef[NUMBER_OF_PDU_MAX]; /* not supported */
  Can_PduType* PduIdRef; /* not supported */

} CanIfRxPduConfig;


typedef struct {
  const char* driverName;
  uint32 deviceId;
  uint32 baudRate; /* ボーレート */
  uint8  btr0;     /* btrレジスタ0 */
  uint8  btr1;     /* btrレジスタ1 */
  uint32  acceptCode; /* Accept Code */
  uint32  acceptMask; /* Accept Mask */
} CanIfConfigSet;

/*
  Container Name: CanIfInitConfiguration [Multi Config Container] 
  Description: This container contains the init parameters of the CAN Interface.  
*/
typedef struct {
  CanIfConfigSet ConfigSet;
  uint32 CanIfNumberOfCanRxPduIds; /* 受信PduIDの数 */
  uint32 CanIfNumberOfCanTxPduIds; /* 送信PduIDの数 */
  uint32 CanIfNumberOfDynamicCanTxPduIds;/* not supported */
  CanIfInitControllerConfig CanIfInitControllerConfigList[NUMBER_OF_CANCONTROLLER]; /* not supported */
  CanIfInitHohConfig    CanIfInitHohConfigList[NUMBER_OF_CANCONTROLLER]; /* not supported */
  CanIfRxPduConfig      CanIfRxPduConfigList[NUMBER_OF_PDU_MAX];
  CanIfTxPduConfig      CanIfTxPduConfigList[NUMBER_OF_PDU_MAX];

} CanIfInitConfiguration;


/*
  Container Name: CanIfDispatchConfig
  Description: Callout functions with respect to the upper layers. This callout functions 
  defined in this container are common to all configured underlying CAN 
  Drivers / CAN Transceiver Drivers.  
  */
typedef struct {
  CanIf_BusOffCallbackType CanIfBusOffNotification;
  CanIf_WakeupCallbackType CanIfWakeupNotification;
  CanIf_WakeupCallbackType CanIfWakeupValidNotification;
} CanIfDispatchConfig;


/*
  Container Name: CanIfTransceiverDrvConfig{CanInterfaceTransceiverDriverConfiguration} 
  Description: This container contains the configuration (parameters) of all addressed 
  CAN transceivers by each underlying CAN Transceiver Driver.  
*/
typedef struct {
  BooleanParamDef CanIfTrcvWakeupNotification;
  uint32 CanIfTrcvIdRef;
} CanIfTransceiverDrvConfig;


/* 
  CanIfのConfiguration定義。
  Configurationのgetter/setterについての定義がAUTOSARにないため、
  グローバル変数として定義する
  */
typedef struct {
  CanIfControllerConfig     controllerConfig[NUMBER_OF_CANCONTROLLER];
  CanIfDispatchConfig       dispatchConfig; /* not supported */
  CanIfDriverConfig         driverConfig[NUMBER_OF_CANDRIVERS];
  CanIfInitConfiguration    initConfiguration[NUMBER_OF_CANCONFIGURATIONS];
  CanIfPrivateConfiguration privateConfiguration; /* not supported */
  CanIfPublicConfiguration  publicConfiguration;  /* not supported */
  CanIfTransceiverDrvConfig transceiverDrvConfig[NUMBER_OF_CANTRANSCEIVERCONFIG]; /* not supported */
} CanIfConfigurationsType;


/*
  Name: CanIf_ConfigType
  Description:  
    This type of the external data structure shall contain the post build initialization 
    data for the CAN Interface for all underlying CAN Drivers. 
    ※現バージンではCanIf_ConfigTypeの内容は無視されますので、CanIf_InitにはNULLを指定してください。
  */
typedef struct {
  /* The definition of CAN Interface public parameters shall contain: */
  uint32 NumberOfTransmitLPDUs; /* Number of  transmit L-PDUs (not supprted) */
  uint32 NumberOfReceiveLPDUs;  /* Number of  receive L-PDUs (not supprted) */
  uint32 NumberOfDynamicTransmitLPDUhandles; /* Number of dynamic transmit L-PDU handles (not supprted) */
  /* The definition for each L-PDU handles shall contain: (not supprted) */
  uint32 HandleForTransmitLPDUs[NUMBER_OF_PDU_MAX]; /* Handle for transmit L-PDUs (not supprted) */
  uint32 HandleForReceiveLPDUs[NUMBER_OF_PDU_MAX];  /* Handle for receive L-PDUs (not supprted) */
  const char* NameForTransmitLPDUs[NUMBER_OF_PDU_MAX]; /* Name for transmit L-PDUs (not supprted) */
  const char* NameForReceiveLPDUS[NUMBER_OF_PDU_MAX];  /* Name for receive L-PDUs (not supprted) */
  uint32 CanIdentifierForStaticAndDynamicTransmitLPDUs[NUMBER_OF_PDU_MAX]; /* CAN Identifier for static and dynamic transmit L-PDUs (not supprted) */
  uint32 CanIdentifierForRceiveLPDUs[NUMBER_OF_PDU_MAX]; /* CAN Identifier for receive L-PDUs (not supprted) */
  uint32 DLCforTransmitLPDUs[NUMBER_OF_PDU_MAX]; /* DLC for transmit L-PDUs (not supprted) */
  uint32 DLCforReceiveLPDUs[NUMBER_OF_PDU_MAX]; /* DLC for receive L-PDUs (not supprted) */
  uint32 DataBufferForReceiveLPDUsInCaceOfPollingMode[NUMBER_OF_PDU_MAX]; /* Data buffer for receive L-PDUs in case of polling mode (not supprted) */
  uint32 NetworkTowardsEachLPDUbelongsTo[NUMBER_OF_PDU_MAX]; /* Network towards each L-PDU belongs to. (not supprted) */
  uint32 TransmitLPDUhandleType[NUMBER_OF_PDU_MAX]; /* Transmit L-PDU handle type (not supprted) */
} CanIf_ConfigType;

/* CanIf_ControllerModeType */
typedef	uint8	CanIf_ControllerModeType;







#endif /* #ifndef CANIF_CFG_H */
