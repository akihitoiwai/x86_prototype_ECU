/**********************************************************************************
 * �t�@�C����      : flexray_api.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : FLEXRAY API
 * �v���O������    : 
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : flexray_api.h			API Header
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N09��23�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "Fr.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_flexray.h"

/**********************************************************************************
 * �ϐ���`
 **********************************************************************************/
const char *CATSDRV_FLEXRAY_PATH[FR_CTRL_IDX_MAX_SUPPORTED] = {
	"/dev/catsdrv1",
	"/dev/catsdrv2"
};

Fr_Controller *FrControllerPtr[FR_CTRL_IDX_MAX_SUPPORTED] = {(Fr_Controller*)0, (Fr_Controller*)0};

int catsdrv_flexray_fd[FR_CTRL_IDX_MAX_SUPPORTED] = {-1, -1};

int disable_int(uint8 Fr_CtrlIdx);
int enable_int(uint8 Fr_CtrlIdx);
int set_cmd(uint8 Fr_CtrlIdx, cFr_CHICmdVectType cFr_CHICmdVect);
int set_cc_config_chi(uint8 Fr_CtrlIdx, char *chi_file_path);
int compare_write(char *str, unsigned long *addr, unsigned long *data);
int compare_wait(char *str, unsigned long *addr, unsigned long *data);
int get_poc_status(uint8 Fr_CtrlIdx, Fr_POCStatusType *cFr_POCStatusPtr);

void Fr_Close(void)
{
	int i;

	for (i = 0; i < FR_CTRL_IDX_MAX_SUPPORTED; i++)
	{
		if (catsdrv_flexray_fd[i] != -1)
		{
			close(catsdrv_flexray_fd[i]);

			catsdrv_flexray_fd[i] = -1;
		}
	}
}
/* 
   Service name: Fr_GetVersionInfo 
   Service ID[hex]: 0x1b 
   Sync/Async: Synchronous 
   Reentrancy: Non Reentrant 
   Parameters (in): None 
   Parameters (inout): None
   Parameters (out): 
     VersioninfoPtr Pointer to where to store the version information of this module. 
   Return value: None 
   Description: 
     Returns the version information of this module. 
*/
void Fr_GetVersionInfo(Std_VersionInfoType* VersioninfoPtr)
{
	VersioninfoPtr->moduleID = 0;
	VersioninfoPtr->vendorID = 0;
	VersioninfoPtr->sw_major_version = 1;
	VersioninfoPtr->sw_minor_version = 1;
	VersioninfoPtr->sw_patch_version = 0;
}
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
void Fr_Init(const Fr_ConfigType* Fr_ConfigPtr)
{
	uint8 Fr_CtrlIdx;
	int oflags;

	// Fr_ConfigPtr��NULL�Ȃ�I������
	if (Fr_ConfigPtr == NULL)
	{
		return;
	}

	Fr_CtrlIdx = Fr_ConfigPtr->FrController->FrCtrlIdx;

	// �h���C�o���I�[�v������
	catsdrv_flexray_fd[Fr_CtrlIdx] = open(CATSDRV_FLEXRAY_PATH[Fr_CtrlIdx], O_RDWR);

	if(catsdrv_flexray_fd[Fr_CtrlIdx] < 0)
	{
		return;
	}

	// ���荞�݋֎~�ɂ���
	if (disable_int(Fr_CtrlIdx) != E_OK)
	{
		return;
	}

	// HALT��ԂɑJ�ڂ�����
	if (set_cmd(Fr_CtrlIdx, CFR_HALT) != E_OK)
	{
		return;
	}

	// ����M�o�b�t�@���N���A����
	if (set_cmd(Fr_CtrlIdx, CFR_CLEAR_RAMS) != E_OK)
	{
		return;
	}

	fcntl(catsdrv_flexray_fd[Fr_CtrlIdx], F_SETOWN, getpid());
	oflags = fcntl(catsdrv_flexray_fd[Fr_CtrlIdx], F_GETFL);
	fcntl(catsdrv_flexray_fd[Fr_CtrlIdx], F_SETFL, oflags | FASYNC);

	// �R���t�B�O���[�V�����f�[�^�̃A�h���X��ێ�����
	FrControllerPtr[Fr_CtrlIdx] = Fr_ConfigPtr->FrController;
}
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
Std_ReturnType Fr_ControllerInit(uint8 Fr_CtrlIdx, uint8 Fr_LowLevelConfSetIdx, uint8 Fr_BufConfSetIdx)
{
	Fr_POCStatusType poc_status;
	int wait;

	// Fr_CtrlIdx�������ł���ꍇE_NOT_OK��Ԃ��ďI������
	if (Fr_CtrlIdx >= FR_CTRL_IDX_MAX_SUPPORTED)
	{
		// FR_E_INV_CTRL_IDX
		return E_NOT_OK;
	}

	// Fr_Init������I������O�ɂ��̊֐����Ă΂ꂽ�ꍇE_NOT_OK��Ԃ��ďI������
	if (FrControllerPtr[Fr_CtrlIdx] == NULL)
	{
		// FR_E_NOT_INITIALIZED
		return E_NOT_OK;
	}

	// Fr_LowLevelConfSetIdx�܂���Fr_BufConfSetIdx�������ł���ꍇE_NOT_OK��Ԃ��ďI������
	if ((Fr_LowLevelConfSetIdx != 0) || (Fr_BufConfSetIdx != 0))
	{
		// FR_E_INV_CONFIG
		return E_NOT_OK;
	}

	// CONFIG��ԂɑJ�ڂ�����
	if (set_cmd(Fr_CtrlIdx, CFR_CONFIG) != E_OK)
	{
		return E_NOT_OK;
	}

	// �R���t�B�O���[�V����������������
	if (set_cc_config_chi(Fr_CtrlIdx, FrControllerPtr[Fr_CtrlIdx]->ChiFilePath) != E_OK)
	{
		return E_NOT_OK;
	}

	// READY��ԂɑJ�ڂ�����
printf("ControllerInit: WAIT READY\n");
	for (wait = 1000; wait > 0; wait--)
	{
printf("ControllerInit: READ POC\n");
		if (get_poc_status(Fr_CtrlIdx, &poc_status) != E_OK)
		{
			return E_NOT_OK;
		}
		
		if (poc_status.State == FR_POCSTATE_READY)
		{
printf("ControllerInit: OK\n");
			return E_OK;
		}

		if (set_cmd(Fr_CtrlIdx, CFR_READY) != E_OK)
		{
			return E_NOT_OK;
		}

		usleep(1);
	}

	return E_NOT_OK;
}
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
Std_ReturnType Fr_SetWakeupChannel(uint8 Fr_CtrlIdx, Fr_ChannelType Fr_ChnlIdx)
{
	CFR_SETWAKEUPCH_REG reg;
	int ret;

	switch (Fr_ChnlIdx)
	{
		case FR_CHANNEL_A:
			reg.wakeup_ch = 0;
			break;
		case FR_CHANNEL_B:
			reg.wakeup_ch = 1;
			break;
		default:
			return E_NOT_OK;
	}

	// �E�F�C�N�A�b�v�`�����l�����Z�b�g����
	ret = ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_SETWAKEUPCH, &reg);

	if (ret != 0)
	{
		return E_NOT_OK;
	}

	return E_OK;
}
/*
  Service name: Fr_SendWUP 
  Service ID[hex]: 0x06 
  Sync/Async: Asynchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC withiln the context of the FlexRay Driver. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: API call finished successfully. 
                   E_NOT_OK: API call aborted due to errors. 
  Description: 
    Invokes the CC CHI command WAKEUP. 
*/
Std_ReturnType Fr_SendWUP(uint8 Fr_CtrlIdx)
{
	Fr_POCStatusType poc_status;
	int wait;

	// Fr_CtrlIdx�������ł���ꍇE_NOT_OK��Ԃ��ďI������
	if (Fr_CtrlIdx >= FR_CTRL_IDX_MAX_SUPPORTED)
	{
		// FR_E_INV_CTRL_IDX
		return E_NOT_OK;
	}

	// Fr_Init������I������O�ɂ��̊֐����Ă΂ꂽ�ꍇE_NOT_OK��Ԃ��ďI������
	if (FrControllerPtr[Fr_CtrlIdx] == NULL)
	{
		// FR_E_NOT_INITIALIZED
		return E_NOT_OK;
	}

	// ���݂̏�Ԃ��擾����
	if (get_poc_status(Fr_CtrlIdx, &poc_status) != E_OK)
	{
		return E_NOT_OK;
	}

	// READY��ԂłȂ����E_NOT_OK��Ԃ��ďI������
	if (poc_status.State != FR_POCSTATE_READY)
	{
		// FR_E_INV_POCSTATE
		return E_NOT_OK;
	}

	// WAKEUP��ԂɑJ�ڂ�����
	if (set_cmd(Fr_CtrlIdx, CFR_WAKEUP) != E_OK)
	{
		return E_NOT_OK;
	}

printf("SendWUP: WAIT READY\n");
	for (wait = 1000; wait > 0; wait--)
	{
printf("SendWUP: READ POC\n");
		if (get_poc_status(Fr_CtrlIdx, &poc_status) != E_OK)
		{
			return E_NOT_OK;
		}
		
		if (poc_status.State == FR_POCSTATE_READY)
		{
printf("SendWUP: OK\n");
			return E_OK;
		}
	}

	return E_NOT_OK;
}
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
  Invokes the CC CHI command ALLOW_COLDSTART. 
*/
Std_ReturnType Fr_AllowColdstart(uint8 Fr_CtrlIdx)
{
	Fr_POCStatusType poc_status;

	// Fr_CtrlIdx�������ł���ꍇE_NOT_OK��Ԃ��ďI������
	if (Fr_CtrlIdx >= FR_CTRL_IDX_MAX_SUPPORTED)
	{
		// FR_E_INV_CTRL_IDX
		return E_NOT_OK;
	}

	// Fr_Init������I������O�ɂ��̊֐����Ă΂ꂽ�ꍇE_NOT_OK��Ԃ��ďI������
	if (FrControllerPtr[Fr_CtrlIdx] == NULL)
	{
		// FR_E_NOT_INITIALIZED
		return E_NOT_OK;
	}

	// ���݂̏�Ԃ��擾����
	if (get_poc_status(Fr_CtrlIdx, &poc_status) != E_OK)
	{
		return E_NOT_OK;
	}

	// DEFAULT_CONFIG�ł�CONFIG�ł�HALT�ł��Ȃ��Ȃ�E_NOT_OK��Ԃ��ďI������
	if ((poc_status.State == FR_POCSTATE_DEFAULT_CONFIG) ||
		(poc_status.State == FR_POCSTATE_CONFIG) ||
		(poc_status.State == FR_POCSTATE_HALT))
	{
		return E_NOT_OK;
	}

	// ALLOW_COLDSTART�����s����
	if (set_cmd(Fr_CtrlIdx, CFR_ALLOW_COLDSTART) != E_OK)
	{
		return E_NOT_OK;
	}

printf("Allow Coldstart: OK\n");	
	return E_OK;
}
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
Std_ReturnType Fr_StartCommunication(uint8 Fr_CtrlIdx)
{
	Fr_POCStatusType poc_status;

	// Fr_CtrlIdx�������ł���ꍇE_NOT_OK��Ԃ��ďI������
	if (Fr_CtrlIdx >= FR_CTRL_IDX_MAX_SUPPORTED)
	{
		// FR_E_INV_CTRL_IDX
		return E_NOT_OK;
	}

	// Fr_Init������I������O�ɂ��̊֐����Ă΂ꂽ�ꍇE_NOT_OK��Ԃ��ďI������
	if (FrControllerPtr[Fr_CtrlIdx] == NULL)
	{
		// FR_E_NOT_INITIALIZED
		return E_NOT_OK;
	}

	// ���݂̏�Ԃ��擾����
	if (get_poc_status(Fr_CtrlIdx, &poc_status) != E_OK)
	{
		return E_NOT_OK;
	}

	// READY��ԂłȂ����E_NOT_OK��Ԃ��ďI������
	if (poc_status.State != FR_POCSTATE_READY)
	{
		// FR_E_INV_POCSTATE
		return E_NOT_OK;
	}

	// RUN��ԂɑJ�ڂ�����
	if (set_cmd(Fr_CtrlIdx, CFR_RUN) != E_OK)
	{
		return E_NOT_OK;
	}

	// ���荞�݂�������
	if (enable_int(Fr_CtrlIdx) != E_OK)
	{
		return;
	}

printf("Start Communication: OK\n");	
	return E_OK;
}
/*
  Service name: Fr_HaltCommunication 
  Service ID[hex]: 0x04 
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
   Invokes the CC CHI command HALT. 
*/
Std_ReturnType Fr_HaltCommunication(uint8 Fr_CtrlIdx)
{
	// Fr_CtrlIdx�������ł���ꍇE_NOT_OK��Ԃ��ďI������
	if (Fr_CtrlIdx >= FR_CTRL_IDX_MAX_SUPPORTED)
	{
		// FR_E_INV_CTRL_IDX
		return E_NOT_OK;
	}

	// Fr_Init������I������O�ɂ��̊֐����Ă΂ꂽ�ꍇE_NOT_OK��Ԃ��ďI������
	if (FrControllerPtr[Fr_CtrlIdx] == NULL)
	{
		// FR_E_NOT_INITIALIZED
		return E_NOT_OK;
	}

	// HALT�����s����
	if (set_cmd(Fr_CtrlIdx, CFR_HALT) != E_OK)
	{
		return E_NOT_OK;
	}

	return E_OK;
}
/*
  Service name: Fr_AbortCommunication 
  Service ID[hex]: 0x05 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: 
    Std_ReturnType E_OK: API call finished successfully. 
                   E_NOT_OK: API call aborted due to errors. 
  Description: 
    Invokes the CC CHI command FREEZE. 
*/
Std_ReturnType Fr_AbortCommunication(uint8 Fr_CtrlIdx)
{
	// Fr_CtrlIdx�������ł���ꍇE_NOT_OK��Ԃ��ďI������
	if (Fr_CtrlIdx >= FR_CTRL_IDX_MAX_SUPPORTED)
	{
		// FR_E_INV_CTRL_IDX
		return E_NOT_OK;
	}

	// Fr_Init������I������O�ɂ��̊֐����Ă΂ꂽ�ꍇE_NOT_OK��Ԃ��ďI������
	if (FrControllerPtr[Fr_CtrlIdx] == NULL)
	{
		// FR_E_NOT_INITIALIZED
		return E_NOT_OK;
	}

	// FREEZE�����s����
	if (set_cmd(Fr_CtrlIdx, CFR_FREEZE) != E_OK)
	{
		return E_NOT_OK;
	}

	return E_OK;
}
/* 
  Service name:  Fr_GetPOCStatus 
  Service ID[hex]: 0x0a 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
  Parameters (inout): None 
  Parameters (out): 
    Fr_POCStatusPtr Address the output value is stored to. 
  Return value: 
    Std_ReturnType  E_OK: API call finished successfully. 
                    E_NOT_OK: API call aborted due to errors. 
  Description: 
    Gets the POC status. 
*/
Std_ReturnType Fr_GetPOCStatus(uint8 Fr_CtrlIdx, Fr_POCStatusType* Fr_POCStatusPtr)
{
	if (get_poc_status(Fr_CtrlIdx, Fr_POCStatusPtr) != E_OK)
	{
		return E_NOT_OK;
	}

	return E_OK;
}
/*
  Service name: Fr_GetNmVector 
  Service ID[hex]: 0x22 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
  Parameters (inout): None 
  Parameters (out): 
    Fr_NmVectorPtr Address where the NmVector of the last communication cycle shall be stored. 
  Return value: 
    Std_ReturnType E_OK: API call finished successfully. 
                   E_NOT_OK: API call aborted due to errors. 
  Description: 
    Gets the network management vector of the last communication cycle.
*/
Std_ReturnType Fr_GetNmVector(uint8 Fr_CtrlIdx, uint8* Fr_NmVectorPtr)
{
	CFR_GETNMVECTOR_REG reg;
	int ret, i;

	// NM�x�N�^���擾
	ret = ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_GETNMVECTOR, &reg);

	if (ret != 0)
	{
		return E_NOT_OK;
	}

	for (i = 0; i < 12; i++)
	{
		Fr_NmVectorPtr[i] = reg.nm_vector[i];
	}

	return E_OK;
}
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
Std_ReturnType Fr_TransmitTxLPdu(uint8 Fr_CtrlIdx, uint16 Fr_LPduIdx, const uint8* Fr_LSduPtr, uint8 Fr_LSduLength)
{
	CFR_TRANSMIT_REG reg;
	int ret;

	reg.index  = Fr_LPduIdx;
	reg.data   = (unsigned char*)Fr_LSduPtr;
	reg.length = Fr_LSduLength;
	reg.int_enable = True;

	// ���M����
	ret = ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_TRANSMIT, &reg);

	if (ret != 0)
	{
		return E_NOT_OK;
	}

	return E_OK;
}
/*
  Service name: Fr_CheckTxLPduStatus 
  Service ID[hex]: 0x0d 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver.
    Fr_LPduIdx  This index is used to uniquely identify a FlexRay frame 
  Parameters (inout): None 
  Parameters (out): 
    Fr_TxLSduStatusPtrThis reference is used to store the transmit status of the LSdu
  Return value: 
    Std_ReturnType  E_OK: API call finished successfully. 
                    E_NOT_OK: API call aborted due to errors. 
  Description: 
    Checks the transmit status of the LSdu. 
*/
Std_ReturnType Fr_CheckTxLPduStatus(uint8 Fr_CtrlIdx, uint16 Fr_LPduIdx, Fr_TxLPduStatusType* Fr_TxLPduStatusPtr)
{
	CFR_GETINTSTS_REG getintsts_reg;
	CFR_RESETINT_REG resetint_reg;
	int txrq_index;

	*Fr_TxLPduStatusPtr = FR_NOT_TRANSMITTED;

	// ���荞�݂̗L�����`�F�b�N
	if (ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_GETINTSTS, &getintsts_reg) != 0)
	{
		printf("FlexRay%d Error: Get Int Status\n", Fr_CtrlIdx);
		return;
	}

	// ���M�������荞�݂���
	if (getintsts_reg.txi)
	{
		if (Fr_LPduIdx == getintsts_reg.txbufno)
		{
			*Fr_TxLPduStatusPtr = FR_TRANSMITTED;

			resetint_reg.rxi = False;
			resetint_reg.txi = True;
			resetint_reg.txbufno = Fr_LPduIdx;

			// ���荞�݃N���A
			if (ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_RESETINT, &resetint_reg) != 0)
			{
				printf("FlexRay%d Error: Reset Int\n", Fr_CtrlIdx);
				return;
			}
		}
	}

	return E_OK;
}
/*
  Service name: Fr_ReceiveRxLPdu 
  Service ID[hex]:  0x0c 
  Sync/Async: Synchronous 
  Reentrancy: Non Reentrant for the same device 
  Parameters (in): 
    Fr_CtrlIdx  Index of FlexRay CC within the context of the FlexRay Driver. 
    Fr_LPduIdx  This index is used to uniquely identify a FlexRay frame. 
  Parameters (inout): None 
  Parameters (out): 
    Fr_LSduPtr  This reference points to the buffer where the LSdu to be received shall be stored. 
    Fr_LPduStatusPtr This reference points to the memory location where the status of the LPdu shall be stored 
    Fr_LSduLengthPtrThis reference points to the memory location where the length of the LSdu (in bytes) shall be stored. This length represents the number of bytes copied to Fr_LSduPtr. 
  Return value: 
    Std_ReturnType  E_OK: API call finished successfully. 
                    E_NOT_OK: API call aborted due to errors. 
  Description: 
    Receives data from the FlexRay network. 
*/
Std_ReturnType Fr_ReceiveRxLPdu(uint8 Fr_CtrlIdx, uint16 Fr_LPduIdx, uint8* Fr_LSduPtr, Fr_RxLPduStatusType* Fr_LPduStatusPtr, uint8* Fr_LSduLengthPtr)
{
	CFR_GETINTSTS_REG getintsts_reg;
	CFR_RECEIVE_REG receive_reg;
	CFR_RESETINT_REG resetint_reg;
	int i;

	*Fr_LPduStatusPtr = FR_NOT_RECEIVED;

	// ���荞�݂̗L�����`�F�b�N
	if (ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_GETINTSTS, &getintsts_reg) != 0)
	{
		printf("FlexRay%d Error: Get Int Status\n", Fr_CtrlIdx);
		return;
	}

	// ��M���荞�݂���
	if (getintsts_reg.rxi)
	{
		// ��M�o�b�t�@�̃C���f�b�N�X���Z�b�g
		receive_reg.index = Fr_LPduIdx;

		// ��M�f�[�^�i�[��̃|�C���^���Z�b�g
		receive_reg.data = Fr_LSduPtr;

		// ��M�f�[�^�擾
		if (ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_RECEIVE, &receive_reg) != 0)
		{
			printf("FlexRay%d Error: Receive\n", Fr_CtrlIdx);
			return;
		}

		// ��M�f�[�^����
		if (receive_reg.received)
		{
			*Fr_LSduLengthPtr = receive_reg.length;
			*Fr_LPduStatusPtr = FR_RECEIVED;

			resetint_reg.rxi = True;
			resetint_reg.txi = False;

			// ���荞�݃N���A
			if (ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_RESETINT, &resetint_reg) != 0)
			{
				printf("FlexRay%d Error: Reset Int\n", Fr_CtrlIdx);
				return;
			}
		}
	}
}

int disable_int(uint8 Fr_CtrlIdx)
{
	int ret;

	// FlexRay�R���g���[���̊��荞�݂��֎~����
	ret = ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_DISABLEINT);

	if (ret != 0)
	{
		return E_NOT_OK;
	}

	return E_OK;
}

int enable_int(uint8 Fr_CtrlIdx)
{
	int ret;

	// FlexRay�R���g���[���̊��荞�݂��֎~����
	ret = ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_ENABLEINT);

	if (ret != 0)
	{
		return E_NOT_OK;
	}

	return E_OK;
}

int set_cmd(uint8 Fr_CtrlIdx, cFr_CHICmdVectType cFr_CHICmdVect)
{
	CFR_SETCMD_REG reg;
	int wait;

	// �R�}���h���Z�b�g
	reg.cmd = cFr_CHICmdVect;

	for (wait = 1000; wait > 0; wait--)
	{
		ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_SETCMD, &reg);

		// �r�W�[�łȂ���΃R�}���h�������ݐ���
		if (!reg.busy)
		{
			break;
		}

		usleep(1);
	}

	// �r�W�[�̂܂܂Ȃ�G���[
	if (wait == 0)
	{
		return E_NOT_OK;
	}

	return E_OK;
}

int set_cc_config_chi(uint8 Fr_CtrlIdx, char *chi_file_path)
{
	FILE *fd;
	char str[256];
	unsigned long addr, data;
	CFR_WRITE32_REG wreg;
	CFR_READ32_REG rreg;
	int wait, ret;

	// CHI�t�@�C�����J��
	fd = fopen(chi_file_path, "r");
	if (fd < 0)
	{
		return E_NOT_OK;
	}

	ret = E_OK;
	while (NULL != fgets(str, 255, fd))
	{
		// "WRITE32"�Ɣ�r
		if (compare_write(str, &addr, &data))
		{
			// "WRITE32"�ƈ�v�����烉�C�g�����s
			wreg.addr = addr;
			wreg.data = data;
			ret = ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_WRITE32, &wreg);
		}
		// "WAIT_TILL_CLEARED32"�Ɣ�r
		else if (compare_wait(str, &addr, &data))
		{
			// "WAIT_TILL_CLEARED32"�ƈ�v������w��̃r�b�g���N���A�����܂ŃE�F�C�g�����s
			rreg.addr = addr;
			for (wait = 1000; wait > 0; wait--)
			{
				ret = ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_READ32, &rreg);

				// �w��̃r�b�g���N���A����Ă���?
				if ((rreg.data & data) == 0)
				{
					break;
				}

				usleep(1);
			}

			// "WAIT_TILL_CLEARED32"�Ŏw��̃r�b�g���N���A����Ȃ�������G���[�Ŕ�����
			if (wait == 0)
			{
				ret = E_NOT_OK;
				break;
			}
		}
	}

	fclose(fd);

	return ret;
}

int compare_write(char *str, unsigned long *addr, unsigned long *data)
{
	char work[255];
	char write_str[] = "WRITE32";
	int i;

	// "WRITE32(0x00000000, 0x00000000); /* XXXX */"��'('��')'��'x'��','���󔒂ɒu��������
	// "WRITE32 0x00000000  0x00000000 ; /* XXXX */"�ɂ���
	for (i = 0; i < 255 && str[i] != '\0'; i++)
	{
		if ((str[i] == '(') || (str[i] == ')') || (str[i] == ','))
			str[i] = ' ';
	}

	// "WRITE32 0x00000000  0x00000000 ; /* XXXX */"�𕪉�����
	work[0] = '\0';
	sscanf(str, "%s %x %x", work, data, addr);

	// �ŏ��̕������"WRITE32"?
	if (strcmp(write_str, work))
	{
		// �s��v�Ń��^�[��
		return False;
	}

	return True;
}

int compare_wait(char *str, unsigned long *addr, unsigned long *data)
{
	char work[255];
	char wait_str[]  = "WAIT_TILL_CLEARED32";
	int i;

	// "WAIT_TILL_CLEARED32(0x00000000, 0x00000000); /* XXXX */"��'('��')'��'x'��','���󔒂ɒu��������
	// "WAIT_TILL_CLEARED32 0x00000000  0x00000000 ; /* XXXX */"�ɂ���
	for (i = 0; i < 255 && str[i] != '\0'; i++)
	{
		if ((str[i] == '(') || (str[i] == ')') || (str[i] == ','))
			str[i] = ' ';
	}

	// "WAIT_TILL_CLEARED32 0x00000000  0x00000000 ; /* XXXX */"�𕪉�����
	work[0] = '\0';
	sscanf(str, "%s %x %x", work, data, addr);

	// �ŏ��̕������"WAIT_TILL_CLEARED32"?
	if (strcmp(wait_str, work))
	{
		// �s��v�Ń��^�[��
		return False;
	}

	return True;
}

int get_poc_status(uint8 Fr_CtrlIdx, Fr_POCStatusType *cFr_POCStatusPtr)
{
	CFR_GETPOCSTATE_REG reg;
	int ret;

	// POC�X�e�[�^�X���擾
	ret = ioctl(catsdrv_flexray_fd[Fr_CtrlIdx], IOCTL_CATS_FLEXRAY_GETPOCSTATE, &reg);

	if (ret != 0)
	{
		return E_NOT_OK;
	}

	cFr_POCStatusPtr->ColdstartNoise = reg.coldstart_noise;
	cFr_POCStatusPtr->CHIHaltRequest = reg.halt_request;
	cFr_POCStatusPtr->Freeze         = reg.freeze;

	switch (reg.slot_mode)
	{
		case 0x00:
			cFr_POCStatusPtr->SlotMode = FR_SLOTMODE_SINGLE;
			break;
		case 0x02:
			cFr_POCStatusPtr->SlotMode = FR_SLOTMODE_ALL_PENDING;
			break;
		case 0x03:
			cFr_POCStatusPtr->SlotMode = FR_SLOTMODE_ALL;
			break;
	}

	switch (reg.wakeup_status)
	{
		case 0x00:
			cFr_POCStatusPtr->WakeupStatus = FR_WAKEUP_UNDEFINED;
			break;
		case 0x02:
			cFr_POCStatusPtr->WakeupStatus = FR_WAKEUP_RECEIVED_WUP;
			break;
		case 0x03:
			cFr_POCStatusPtr->WakeupStatus = FR_WAKEUP_COLLISION_HEADER;
			break;
		case 0x04:
			cFr_POCStatusPtr->WakeupStatus = FR_WAKEUP_COLLISION_WUP;
			break;
		case 0x05:
			cFr_POCStatusPtr->WakeupStatus = FR_WAKEUP_COLLISION_UNKNOWN;
			break;
		case 0x06:
			cFr_POCStatusPtr->WakeupStatus = FR_WAKEUP_TRANSMITTED;
			break;
	}

	switch (reg.error_mode)
	{
		case 0x00:
			cFr_POCStatusPtr->ErrorMode = FR_ERRORMODE_ACTIVE;
			break;
		case 0x01:
			cFr_POCStatusPtr->ErrorMode = FR_ERRORMODE_PASSIVE;
			break;
		case 0x02:
			cFr_POCStatusPtr->ErrorMode = FR_ERRORMODE_COMM_HALT;
			break;
	}

	switch (reg.poc_status)
	{
		case 0x20:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_UNDEFINED;
			break;
		case 0x21:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_COLDSTART_LISTEN;
			break;
		case 0x22:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_COLDSTART_COLLISION_RESOLUTION;
			break;
		case 0x23:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_COLDSTART_CONSISTENCY_CHECK;
			break;
		case 0x24:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_COLDSTART_GAP;
			break;
		case 0x25:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_COLDSTART_JOIN;
			break;
		case 0x26:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_INTEGRATION_COLDSTART_CHECK;
			break;
		case 0x27:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_INTEGRATION_LISTEN;
			break;
		case 0x28:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_INTEGRATION_CONSISTENCY_CHECK;
			break;
		case 0x29:
			cFr_POCStatusPtr->StartupState = FR_STARTUP_INITIALIZE_SCHEDULE;
			break;
	}

	switch (reg.poc_status & 0xf0)
	{
		case 0x00:
			switch (reg.poc_status)
			{
				case 0x00:
					cFr_POCStatusPtr->State = FR_POCSTATE_DEFAULT_CONFIG;
					break;
				case 0x01:
					cFr_POCStatusPtr->State = FR_POCSTATE_READY;
					break;
				case 0x02:
					cFr_POCStatusPtr->State = FR_POCSTATE_NORMAL_ACTIVE;
					break;
				case 0x03:
					cFr_POCStatusPtr->State = FR_POCSTATE_NORMAL_PASSIVE;
					break;
				case 0x04:
					cFr_POCStatusPtr->State = FR_POCSTATE_HALT;
					break;
				case 0x0f:
					cFr_POCStatusPtr->State = FR_POCSTATE_CONFIG;
					break;
			}
			break;
		case 0x10:
			cFr_POCStatusPtr->State = FR_POCSTATE_WAKEUP;
			break;
		case 0x20:
			cFr_POCStatusPtr->State = FR_POCSTATE_STARTUP;
			break;
	}

	return E_OK;
}

/* ----- End Of File ----- */
