/* FrIf.h

  AUTOSAR FlexRay Interfaces

  Reference by AUTOSAR Specification of FlexRay Interface V3.0.2

  Copyright(c) 2008 CATS corp.
*/
#ifndef FRIF_H
#define FRIF_H

#include "/usr/src/CATS_ECU2/api_library/flexrayapi/Fr.h"
#include "/usr/src/CATS_ECU2/Autosar/FlexRay/FlexRayInterface/FrIf_GeneralTypes.h"
#include "/usr/src/CATS_ECU2/Autosar/ComStack_Types.h"

/*
  Name: FrIf_ConfigType 
  Type: Structure 
  Range: Implementation 
  Description: 
    This type contains the implementation-specific post build time configuration 
    structure. Only pointers of this type are allowed. 
*/
typedef struct {
	FrIf_Config					*FrIfConfig;
	FrIf_Cbk_UL_TxConfirmation	TxConfirmation;
	FrIf_Cbk_UL_RxIndication	RxIndication;
} FrIf_ConfigType;
/*
  Name: FrIf_StateType 
  Type: Enumeration 
  Range: 
    FRIF_STATE_OFFLINE The FlexRay CC is not ready for communication, the FlexRay cluster is not synchronized. 
    FRIF_STATE_ONLINE  The FlexRay CC is ready for communication, the FlexRay cluster is synchronized. 
  Description: 
    Variables of this type are used to represent the FrIf_State of a FlexRay CC. 
*/
typedef enum { 
  FRIF_STATE_OFFLINE, /* No communication services are executed (see chapter 7.6 for details).  */
  FRIF_STATE_ONLINE   /* All communication services (reception, transmission, transmission confirmation) are executed (see chapter 7.6 for details). */
} FrIf_StateType;

void FrIf_Close(void);
/* 
  Service name: FrIf_GetVersionInfo 
  Service ID[hex]: 0x01 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant 
  Parameters (in): None 
  Parameters (inout): None 
  Parameters (out): 
    FrIf_VersionInfoPtr Pointer to a memory location where the FlexRay Interface version information shall be stored. 
  Return value: None 
  Description: 
    Returns the version information of this module. 
*/
void FrIf_GetVersionInfo(Std_VersionInfoType* FrIf_VersionInfoPtr);
/*
  Service name: FrIf_Init 
  Service ID[hex]: 0x02 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant 
  Parameters (in): 
    FrIf_ConfigPtr Base pointer to the configuration structure of the FlexRay Interface. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: None 
  Description: 
    Initializes the FlexRay Interface. 
*/
void FrIf_Init(const FrIf_ConfigType* FrIf_ConfigPtr);
/*
  Service name: FrIf_ControllerInit 
  Service ID[hex]: 0x03 
  Sync/Async: Synchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of FrIf_CtrlIdx 
  Parameters (in): 
    FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: The call of the FlexRay Driver¡s API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver¡s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Initialized a FlexRay CC. 
*/
Std_ReturnType FrIf_ControllerInit(uint8 FrIf_CtrlIdx);
/*
  Service name: FrIf_SetWakeupChannel 
  Service ID[hex]: 0x11 
  Sync/Async: Synchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx FrIf_CtrlIdx  Index of the FlexRay CC to address.  
    FrIf_ChnlIdx  Index of the FlexRay Channel to address in scope of the FlexRay controller FrIf_CtrlIdx.  
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: The call of the FlexRay Driver¡s API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver¡s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_SetWakeupChannel(). 
*/
Std_ReturnType FrIf_SetWakeupChannel(uint8 FrIf_CtrlIdx, Fr_ChannelType FrIf_ChnlIdx);  
/*
  Service name: FrIf_SendWUP 
  Service ID[hex]: 0x12 
  Sync/Async: Synchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: The call of the FlexRay Driver¡s API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver¡s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_SendWUP(). 
*/
Std_ReturnType FrIf_SendWUP(uint8 FrIf_CtrlIdx);  
/*
  Service name: FrIf_AllowColdstart 
  Service ID[hex]: 0x35 
  Sync/Async: Asynchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: The call of the FlexRay Driver's API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver's API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_AllowColdstart(). 
*/
Std_ReturnType FrIf_AllowColdstart(uint8 FrIf_CtrlIdx);
/*
  Service name: FrIf_StartCommunication 
  Service ID[hex]: 0x07 
  Sync/Async: Asynchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: The call of the FlexRay Driver's API service has returned E_OK. 
      E_NOT_OK: The call of the FlexRay Driver's API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_StartCommunication(). 
*/
Std_ReturnType FrIf_StartCommunication(uint8 FrIf_CtrlIdx);
/*
  Service name: FrIf_HaltCommunication 
  Service ID[hex]: 0x09 
  Sync/Async: Asynchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: The call of the FlexRay Driver¡s API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver¡s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_HaltCommunication(). 
*/
Std_ReturnType FrIf_HaltCommunication(uint8 FrIf_CtrlIdx);
/*
  Service name: FrIf_AbortCommunication 
  Service ID[hex]: 0x0b 
  Sync/Async: Synchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: The call of the FlexRay Driver¡s API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver¡s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_AbortCommunication(). 
*/ 
Std_ReturnType FrIf_AbortCommunication(uint8 FrIf_CtrlIdx);
/*
  Service name: FrIf_GetState 
  Service ID[hex]: 0x35 
  Sync/Async: Synchronous 
  Reentrancy: Reentrant 
  Parameters (in): 
    FrIf_ClstIdx  Index of the cluster addressed. 
  Parameters (inout): None 
  Parameters (out): 
    FrIf_StatePtr  Pointer to a memory location where the retrieved FrIf_State will be stored.  
  Return value: 
  Std_ReturnType  E_OK: Function was successfully executed. State transition request was accepted. 
                  E_NOT_OK: Function execution failed due to detected errors. State transition request was not accepted. 
  Description: 
    Get current FrIf state. 
*/ 
Std_ReturnType FrIf_GetState(uint8 FrIf_ClstIdx, FrIf_StateType *FrIf_StatePtr);
/* 
  Service name: FrIf_GetPOCStatus 
  Service ID[hex]: 0x19 
  Sync/Async: Synchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): 
    FrIf_POCStatusPtrPointer to a memory location where output value will be stored.
  Return value: 
    Std_ReturnType  E_OK: The call of the FlexRay Driver's API service has returned E_OK. 
                    E_NOT_OK: The call of the FlexRay Driver's API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_GetPOCStatus(). 
*/
Std_ReturnType FrIf_GetPOCStatus(uint8 FrIf_CtrlIdx, Fr_POCStatusType* FrIf_POCStatusPtr);
/*
  Service name: FrIf_GetNmVector 
  Service ID[hex]: 0x34 
  Sync/Async: Synchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): 
    FrIf_CyclePtr  Pointer to a memory location where output value will be stored. 
    FrIf_NmVectorPtr Pointer to a memory location where output value will be stored. 
  Return value: 
    Std_ReturnType  E_OK: The call of the FlexRay Driver¡s API service has returned E_OK. 
                    E_NOT_OK: The call of the FlexRay Driver¡s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Derives the FlexRay NM Vector. 
*/
Std_ReturnType FrIf_GetNmVector(uint8 FrIf_CtrlIdx, uint8* FrIf_CyclePtr, uint8* FrIf_NmVectorPtr);
/*
  Service name: FrIf_Transmit 
  Service ID[hex]: 0x06 
  Sync/Async: Synchronous 
  Reentrancy: non reentrant for identical values of FrIf_TxPduId, reentrant for different values of FrIf_TxPduId 
  Parameters (in): 
    FrIf_TxPduId  ID of FlexRay PDU to be transmitted. 
    FrIf_PduInfoPtr  Pointer to a structure with FlexRay PDU related data. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType  E_OK: No error has occurred during the execution of this API  service. 
                    E_NOT_OK: An error occurred during execution of this API service: 
  ÅE FlexRay Driver reported an error in case of immediate transmission 
  ÅE An error has been detected in development mode  
  Description: 
    Requests the sending of a PDU. 
*/
Std_ReturnType FrIf_Transmit(PduIdType FrIf_TxPduId, const PduInfoType* FrIf_PduInfoPtr);
/*
  8.5 Interrupt Service Routines 
  Service name: FrIf_JobListExec_<ClstIdx> 
  Service ID[hex]: 0x32 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant 
  Parameters (in): None 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: None 
  Description: 
    Processes the FlexRay Job List of the FlexRay Cluster with index ClstIdx. 
*/
void FrIf_JobListExec_0(void);  

#endif /* FRIF_H */
