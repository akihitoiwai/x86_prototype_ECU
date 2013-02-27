#ifndef FR_H
#define FR_H

#include "Fr_GeneralTypes.h"
#include "/usr/src/CATS_ECU2/Autosar/ComStack_Types.h"

typedef struct {
	Fr_Controller	*FrController;
} Fr_ConfigType;

typedef enum {
	FR_CHANNEL_A = 0,
	FR_CHANNEL_B,
	FR_CHANNEL_AB,
} Fr_ChannelType;

typedef enum {
	FR_SLOTMODE_SINGLE = 0,
	FR_SLOTMODE_ALL_PENDING,
	FR_SLOTMODE_ALL,
} Fr_SlotModeType;

typedef enum {
	FR_WAKEUP_UNDEFINED = 0,
	FR_WAKEUP_RECEIVED_HEADER,
	FR_WAKEUP_RECEIVED_WUP,
	FR_WAKEUP_COLLISION_HEADER,
	FR_WAKEUP_COLLISION_WUP,
	FR_WAKEUP_COLLISION_UNKNOWN,
	FR_WAKEUP_TRANSMITTED,
} Fr_WakeupStatusType;

typedef enum {
	FR_ERRORMODE_ACTIVE = 0,
	FR_ERRORMODE_PASSIVE,
	FR_ERRORMODE_COMM_HALT,
} Fr_ErrorModeType;

typedef enum {
	FR_STARTUP_UNDEFINED = 0,
	FR_STARTUP_COLDSTART_LISTEN,
	FR_STARTUP_INTEGRATION_COLDSTART_CHECK,
	FR_STARTUP_COLDSTART_JOIN,
	FR_STARTUP_COLDSTART_COLLISION_RESOLUTION,
	FR_STARTUP_COLDSTART_CONSISTENCY_CHECK,
	FR_STARTUP_INTEGRATION_LISTEN,
	FR_STARTUP_INITIALIZE_SCHEDULE,
	FR_STARTUP_INTEGRATION_CONSISTENCY_CHECK,
	FR_STARTUP_COLDSTART_GAP,
} Fr_StartupStateType;

typedef enum {
	FR_POCSTATE_CONFIG = 0,
	FR_POCSTATE_DEFAULT_CONFIG,
	FR_POCSTATE_HALT,
	FR_POCSTATE_NORMAL_ACTIVE,
	FR_POCSTATE_NORMAL_PASSIVE,
	FR_POCSTATE_READY,
	FR_POCSTATE_STARTUP,
	FR_POCSTATE_WAKEUP,
} Fr_POCStateType;

typedef enum {
	FR_TRANSMITTED = 0,
	FR_NOT_TRANSMITTED,
} Fr_TxLPduStatusType;

typedef enum {
	FR_RECEIVED = 0,
	FR_NOT_RECEIVED,
} Fr_RxLPduStatusType;

typedef struct {
	boolean			ColdstartNoise;
	boolean			CHIHaltRequest;
	boolean			Freeze;
	Fr_SlotModeType		SlotMode;
	Fr_WakeupStatusType	WakeupStatus;
	Fr_ErrorModeType	ErrorMode;
	Fr_StartupStateType	StartupState;
	Fr_POCStateType		State;
} Fr_POCStatusType;

void Fr_Close(void);
/*
  Service name: Fr_Init 
  Service ID[hex]: 0x1c 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant 
  Parameters (in):
    Fr_ConfigPtr Address to an Fr dependant configuration structure that contains all information for operating the Fr subsequently. 
  Parameters (inout): None 
  Parameters (out):   None 
  Return value:       None 
  Description: 
    Initalizes the Fr. 
  Note:
    FR136: If the function Fr_Init detects errors while accessing any CC, it shall call 
    Dem_ReportErrorStatus(FR_E_ACCESS, DEM_EVENT_STATUS_FAILED) and return. 
*/
void Fr_Init(const Fr_ConfigType* Fr_ConfigPtr);
/* 
  Service name: Fr_ControllerInit 
  Service ID[hex]: 0x00 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant for the same device
  Parameters (in): 
    Fr_CtrlIdx            Index of FlexRay CC within the context of the FlexRay Driver. 
    Fr_LowLevelConfSetIdx This parameter is currently not used. Always value 0 shall be passed. 
    Fr_BufConfSetIdx      This parameter is currently not used. Always value 0 shall be passed. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType  E_OK:     API call finished successfully. 
                    E_NOT_OK: API call aborted due to errors. 
  Description: 
    Initialzes a FlexRay CC. 
*/
Std_ReturnType Fr_ControllerInit(uint8 Fr_CtrlIdx, uint8 Fr_LowLevelConfSetIdx, uint8 Fr_BufConfSetIdx);
/*
  Service name: Fr_SetWakeupChannel 
  Service ID[hex]: 0x07 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
    Fr_ChnlIdx  Index of FlexRay channel within the context of the FlexRay CC Fr_CtrlIdx. Valid values are FR_CHANNEL_A and FR_CHANNEL_B. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: API call finished successfully. 
                   E_NOT_OK: API call aborted due to errors. 
  Description: 
    Sets a wakeup channel. 
*/
Std_ReturnType Fr_SetWakeupChannel(uint8 Fr_CtrlIdx, Fr_ChannelType Fr_ChnlIdx);
/*
  Service name: Fr_SendWUP 
  Service ID[hex]: 0x06 
  Sync/Async: Asynchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: API call finished successfully. 
                   E_NOT_OK: API call aborted due to errors. 
  Description: 
    Invokes the CC CHI command ÁWAKEUPÁ. 
*/
Std_ReturnType Fr_SendWUP(uint8 Fr_CtrlIdx);
/*
  Service name: Fr_AllowColdstart 
  Service ID[hex]: 0x23 
  Sync/Async: Asynchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: API call finished successfully. 
                   E_NOT_OK: API call aborted due to errors. 
  Description: 
  Invokes the CC CHI command ÁALLOW_COLDSTARTÁ. 
*/
Std_ReturnType Fr_AllowColdstart(uint8 Fr_CtrlIdx);
/*
  Service name: Fr_StartCommunication 
  Service ID[hex]: 0x03 
  Sync/Async: Asynchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: API call finished successfully. 
                   E_NOT_OK: API call aborted due to errors. 
  Description: 
    Starts communication. 
*/
Std_ReturnType Fr_StartCommunication(uint8 Fr_CtrlIdx);
/*
  Service name: Fr_TransmitTxLPdu 
  Service ID[hex]: 0x0b 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
    Fr_LPduIdx  This index is used to uniquely identify a FlexRay frame. 
    Fr_LSduPtr  This reference points to a buffer where the assembled LSdu to be transmitted within this LPdu is stored at. 
    Fr_LSduLength Determines the length of the data (in Bytes) to be transmitted. 
  Parameters (inout):   None 
  Parameters (out):   None 
  Return value: 
    Std_ReturnType E_OK: API call finished successfully. 
                   E_NOT_OK: API call aborted due to errors. 
  Description: 
    Transmits data on the FlexRay network. 
*/
Std_ReturnType Fr_TransmitTxLPdu(uint8 Fr_CtrlIdx, uint16 Fr_LPduIdx, const uint8* Fr_LSduPtr, uint8 Fr_LSduLength);

#endif /* FR_H */
