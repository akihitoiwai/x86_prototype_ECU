/* FrIf.c

  AUTOSAR FlexRay Interfaces

  Reference by AUTOSAR Specification of FlexRay Interface V3.0.2

  Copyright(c) 2008 CATS corp.
*/

#include <stdio.h>
#include <signal.h>

#include "FrIf.h"

/**********************************************************************************
 * �ϐ���`
 **********************************************************************************/
const FrIf_ConfigType *FrIfConfigPtr;

void FrIf_Close(void)
{
	Fr_Close();
}

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
void FrIf_GetVersionInfo(Std_VersionInfoType* FrIf_VersionInfoPtr)
{
	Fr_GetVersionInfo(FrIf_VersionInfoPtr);
}
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
void FrIf_Init(const FrIf_ConfigType* FrIf_ConfigPtr)
{
	uint8 clstidx, ctrlidx;
	Fr_ConfigType fr_config;
	struct sigaction act;

	// �N���X�^�C���f�b�N�X�������[�v
	for (clstidx = 0; clstidx < FRIF_CLST_IDX_MAX_SUPPORTED; clstidx++)
	{
		// �R���g���[���C���f�b�N�X�������[�v
		for (ctrlidx = 0; ctrlidx < FRIF_CTRL_IDX_MAX_SUPPORTED; ctrlidx++)
		{
			// �R���g���[���̃R���t�B�O���[�V���������Z�b�g
			fr_config.FrController = FrIf_ConfigPtr->FrIfConfig->FrIfCluster[clstidx].FrIfController[ctrlidx].FrIfFrCtrlRef;
			// �R���g���[����������
			Fr_Init(&fr_config);
		}
	}

	// ���荞�ݒʒm�p�̃V�O�i����ݒ�
	sigaction(SIGIO, NULL, &act);
	act.sa_handler = (__sighandler_t)FrIf_JobListExec_0;	// �R�[���o�b�N�֐��Z�b�g
	sigemptyset(&act.sa_mask);								/* sa_mask �̏����� */
	sigaddset(&act.sa_mask, SIGIO);							/* �V�O�i���n���h�����s���̓u���b�N */
	act.sa_flags = 0;										/* sa_flags �̓f�t�H���g */
	act.sa_restorer = NULL;									/* sa_restorer �͖��g�p */
	sigaction(SIGIO, &act, NULL);							/* �V�O�i���n���h���o�^ */

	FrIfConfigPtr = FrIf_ConfigPtr;
}
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
    Std_ReturnType E_OK: The call of the FlexRay Driver's API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver's API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Initialized a FlexRay CC. 
*/
Std_ReturnType FrIf_ControllerInit(uint8 FrIf_CtrlIdx)
{
	Std_ReturnType ret;

	ret = Fr_ControllerInit(FrIf_CtrlIdx, 0, 0);

	return ret;
}
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
    Std_ReturnType E_OK: The call of the FlexRay Driver's API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver's API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_SetWakeupChannel(). 
*/
Std_ReturnType FrIf_SetWakeupChannel(uint8 FrIf_CtrlIdx, Fr_ChannelType FrIf_ChnlIdx)
{
	return Fr_SetWakeupChannel(FrIf_CtrlIdx, FrIf_ChnlIdx);
}
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
    Std_ReturnType E_OK: The call of the FlexRay Driver's API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver's API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_SendWUP(). 
*/
Std_ReturnType FrIf_SendWUP(uint8 FrIf_CtrlIdx)
{
	return Fr_SendWUP(FrIf_CtrlIdx);
}

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
Std_ReturnType FrIf_AllowColdstart(uint8 FrIf_CtrlIdx)
{
	return Fr_AllowColdstart(FrIf_CtrlIdx);
}
/*
  Service name: FrIf_StartCommunication 
  Service ID[hex]: 0x07 
  Sync/Async: Asynchronous 
  Reentrancy: non reentrant for identical values of FrIf_CtrlIdx, reentrant for different values of 
  Parameters (in): 
    FrIf_CtrlIdx FrIf_CtrlIdx  Index of the FlexRay CC to address.  
  Parameters (inout): None 
  Parameters (out): None (PduIdType FrIf_RxPduId, const uint8* FrIf_SduPtr)
  Return value: 
    Std_ReturnType E_OK: The call of the FlexRay Driver's API service has returned E_OK. 
      E_NOT_OK: The call of the FlexRay Driver's API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_StartCommunication(). 
*/
Std_ReturnType FrIf_StartCommunication(uint8 FrIf_CtrlIdx)
{
	return Fr_StartCommunication(FrIf_CtrlIdx);
}
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
    Std_ReturnType E_OK: The call of the FlexRay Driver�s API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver�s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_HaltCommunication(). 
*/
Std_ReturnType FrIf_HaltCommunication(uint8 FrIf_CtrlIdx)
{
	return Fr_HaltCommunication(FrIf_CtrlIdx);
}
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
    Std_ReturnType E_OK: The call of the FlexRay Driver�s API service has returned E_OK. 
                   E_NOT_OK: The call of the FlexRay Driver�s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Wraps the FlexRay Driver API function Fr_AbortCommunication(). 
*/ 
Std_ReturnType FrIf_AbortCommunication(uint8 FrIf_CtrlIdx)
{
	return Fr_AbortCommunication(FrIf_CtrlIdx);
}
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
Std_ReturnType FrIf_GetState(uint8 FrIf_ClstIdx, FrIf_StateType* FrIf_StatePtr)
{
	Fr_POCStatusType FrIf_POCStatus;
	uint8 ctrlidx;

	*FrIf_StatePtr = FRIF_STATE_OFFLINE;

	// �R���g���[���C���f�b�N�X�������[�v
	for (ctrlidx = 0; ctrlidx < FRIF_CTRL_IDX_MAX_SUPPORTED; ctrlidx++)
	{
		if (Fr_GetPOCStatus(ctrlidx, &FrIf_POCStatus) != E_OK)
		{
			return E_NOT_OK;
		}

		if ((FrIf_POCStatus.State == FR_POCSTATE_NORMAL_ACTIVE) ||
		    (FrIf_POCStatus.State == FR_POCSTATE_NORMAL_PASSIVE))
		{
			*FrIf_StatePtr = FRIF_STATE_ONLINE;
		}
	}

	return E_OK;
}
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
Std_ReturnType FrIf_GetPOCStatus(uint8 FrIf_CtrlIdx, Fr_POCStatusType* FrIf_POCStatusPtr)
{
	return Fr_GetPOCStatus(FrIf_CtrlIdx, FrIf_POCStatusPtr);
}
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
    Std_ReturnType  E_OK: The call of the FlexRay Driver�s API service has returned E_OK. 
                    E_NOT_OK: The call of the FlexRay Driver�s API service has returned E_NOT_OK, or an error has been detected in development mode. 
  Description: 
    Derives the FlexRay NM Vector. 
*/
Std_ReturnType FrIf_GetNmVector(uint8 FrIf_CtrlIdx, uint8* FrIf_CyclePtr, uint8* FrIf_NmVectorPtr)
{
	return Fr_GetNmVector(FrIf_CtrlIdx, FrIf_NmVectorPtr);
}
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
  �E FlexRay Driver reported an error in case of immediate transmission 
  �E An error has been detected in development mode  
  Description: 
    Requests the sending of a PDU. 
*/
Std_ReturnType FrIf_Transmit(PduIdType FrIf_TxPduId, const PduInfoType* FrIf_PduInfoPtr)
{
	uint8 clstidx, ctrlidx, lpduidx;
	FrIf_LPdu *lpdu;

	// �N���X�^�C���f�b�N�X�������[�v
	for (clstidx = 0; clstidx < FRIF_CLST_IDX_MAX_SUPPORTED; clstidx++)
	{
		// �R���g���[���C���f�b�N�X�������[�v
		for (ctrlidx = 0; ctrlidx < FRIF_CTRL_IDX_MAX_SUPPORTED; ctrlidx++)
		{
			lpdu = FrIfConfigPtr->FrIfConfig->FrIfCluster[clstidx].FrIfController[ctrlidx].FrIfLPdu;
			// PDUID�������[�v
			for (lpduidx = 0; lpduidx < FRIF_LPDU_IDX_MAX_SUPPORTED; lpduidx++)
			{
				if (lpdu[lpduidx].PduId == FrIf_TxPduId)
				{
					return Fr_TransmitTxLPdu(ctrlidx,
								     FrIf_TxPduId,
								     FrIf_PduInfoPtr->SduDataPtr,
								     FrIf_PduInfoPtr->SduLength);
				}
			}
		}
	}

	return E_NOT_OK;
}
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
void FrIf_JobListExec_0(void)
{
	uint8 ctrlidx, lpduidx;
	Fr_TxLPduStatusType Fr_TxLPduStatusPtr;
	Fr_RxLPduStatusType Fr_LPduStatusPtr;
	uint8 Fr_LSduLengthPtr;	
	PduIdType lpdu;
	char buf[256];

	// �R���g���[���C���f�b�N�X�������[�v
	for (ctrlidx = 0; ctrlidx < FRIF_CTRL_IDX_MAX_SUPPORTED; ctrlidx++)
	{
		// PDUID�������[�v
		for (lpduidx = 0; lpduidx < FRIF_LPDU_IDX_MAX_SUPPORTED; lpduidx++)
		{
			// ���M�����`�F�b�N
			Fr_CheckTxLPduStatus(ctrlidx, lpduidx, &Fr_TxLPduStatusPtr);

			if (Fr_TxLPduStatusPtr == FR_TRANSMITTED)
			{
				lpdu = FrIfConfigPtr->FrIfConfig->FrIfCluster[0].FrIfController[ctrlidx].FrIfLPdu[lpduidx].PduId;

				FrIfConfigPtr->TxConfirmation(lpdu);
			}

			// ��M�`�F�b�N
			Fr_ReceiveRxLPdu(ctrlidx, lpduidx, buf, &Fr_LPduStatusPtr, &Fr_LSduLengthPtr);

			if (Fr_LPduStatusPtr == FR_RECEIVED)
			{
				lpdu = FrIfConfigPtr->FrIfConfig->FrIfCluster[0].FrIfController[ctrlidx].FrIfLPdu[lpduidx].PduId;

				FrIfConfigPtr->RxIndication(lpdu, buf);
			}
		}
	}
}
