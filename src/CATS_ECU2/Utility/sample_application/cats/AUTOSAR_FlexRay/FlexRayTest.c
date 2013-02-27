/**********************************************************************************
 * �t�@�C����      : main.cpp
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : FlexRay �T���v��
 * �o�[�W����      : Ver1.00
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N09��30�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#include <stdio.h>
#include "/usr/src/CATS_ECU2/Autosar/FlexRay/FlexRayInterface/FrIf.h"

void UL_TxConfirmation(PduIdType FrIf_TxPduId);
void UL_RxIndication(PduIdType FrIf_RxPduId, const uint8* FrIf_SduPtr);

char ChiFilePath_0[] = "Ecu1_Controller1.chi";
char ChiFilePath_1[] = "Ecu1_Controller2.chi";

char *POCStateString[]= {
	"CONFIG",
	"DEFAULT_CONFIG",
	"HALT",
	"NORMAL_ACTIVE",
	"NORMAL_PASSIVE",
	"READY",
	"STARTUP",
	"WAKEUP",
};

Fr_MultipleConfiguration FrMultipleConfiguration =
{
	// FrController
	{
		// FrController[0];
		{
			// FrCtrlIdx;
			0,
			// ChiFilePath;
			ChiFilePath_0
		},
		// FrController[1];
		{
			// Fr_CtrlIdx;
			1,
			// ChiFilePath;
			ChiFilePath_1
		}
	}
};

FrIf_Config FrIfConfig =
{
	// FrIfCluster
	{
		// FrIfCluster[0]
		{
			0,									// FrIfClstIdx;
			// FrIfController
			{
				// FrIfController[0]
				{
					0,							// FrIfCtrlIdx;
					&FrMultipleConfiguration.FrController[0],	// FrIfFrCtrlRef;
					// FrIfLPdu
					{
						{0},					// FrIfLPdu[0]
						{1},						// FrIfLPdu[1]
						{2},					// FrIfLPdu[2]
						{3}						// FrIfLPdu[3]
					}
				},
				// FrIfController[1]
				{
					1,							// FrIfCtrlIdx;
					&FrMultipleConfiguration.FrController[1],	// FrIfFrCtrlRef;
					// FrIfLPdu
					{
						{4},					// FrIfLPdu[0]
						{5},						// FrIfLPdu[1]
						{6},					// FrIfLPdu[0]
						{7}						// FrIfLPdu[1]
					}
				}
			}
		}
	}
};

const FrIf_ConfigType Config =
{
	&FrIfConfig,
	UL_TxConfirmation,
	UL_RxIndication
};

int main(int argc,char **argv)
{
	uint8 FrIf_CtrlIdx;
	Std_ReturnType ret;
	PduInfoType pdu_info;
	unsigned long tx_data;
	int wait;
	Std_VersionInfoType FrIf_VersionInfo;
	uint8 FrIf_Cycle;
	Fr_POCStatusType FrIf_POCStatus;

	FrIf_GetVersionInfo(&FrIf_VersionInfo);

	printf("vendorID=%04x\nmoduleID=%02x\n",
		FrIf_VersionInfo.vendorID,
		FrIf_VersionInfo.moduleID);

	printf("sw_major_version=%d\nsw_minor_version=%d\nsw_patch_version=%d\n",
		FrIf_VersionInfo.sw_major_version,
		FrIf_VersionInfo.sw_minor_version,
		FrIf_VersionInfo.sw_patch_version);

	// FlexRay�h���C�o������������
	FrIf_Init(&Config);

	// FlexRay�R���g���[��������������
	for (FrIf_CtrlIdx = 0; FrIf_CtrlIdx < FRIF_CTRL_IDX_MAX_SUPPORTED; FrIf_CtrlIdx++)
	{
		ret = FrIf_ControllerInit(FrIf_CtrlIdx);

		// �E�F�C�N�A�b�v�`�����l�����`�����l��A�ɐݒ肷��
		ret = FrIf_SetWakeupChannel(FrIf_CtrlIdx, FR_CHANNEL_A);

		// �E�F�C�N�A�b�v�M���𑗐M����
		ret = FrIf_SendWUP(FrIf_CtrlIdx);

		// ALLOW_COLDSTART�R�}���h�����s
		ret = FrIf_AllowColdstart(FrIf_CtrlIdx);

		// RUN�R�}���h�����s
		ret = FrIf_StartCommunication(FrIf_CtrlIdx);
	}

	// ���݂̏�Ԃ��m�F
	ret = FrIf_GetPOCStatus(0, &FrIf_POCStatus);
	printf("POCStatus=%s\n", POCStateString[FrIf_POCStatus.State]);

	sleep(1);

	// ���݂̏�Ԃ��m�F
	ret = FrIf_GetPOCStatus(0, &FrIf_POCStatus);
	printf("POCStatus=%s\n", POCStateString[FrIf_POCStatus.State]);

	// ����M�����J�n
	for (tx_data = 0; tx_data < 0xffffffff; tx_data++)
	{
		for (wait = 0; wait < 1000; wait++)
		{
			usleep(1000);
		}

		pdu_info.SduDataPtr = (unsigned char*)&tx_data;
		pdu_info.SduLength = 4;

		// �e�X�g�f�[�^���M
		printf("Transmit: PDUID=%d LEN=%d DATA=%08x\n", 0, pdu_info.SduLength, tx_data);

		ret = FrIf_Transmit(0, &pdu_info);
	}

	// HALT�R�}���h�����s
	ret = FrIf_HaltCommunication(0);

	sleep(1);

	// ���݂̏�Ԃ��m�F
	ret = FrIf_GetPOCStatus(0, &FrIf_POCStatus);
	printf("POCStatus=%s\n", POCStateString[FrIf_POCStatus.State]);

	FrIf_Close();

	return 0;
}

void UL_TxConfirmation(PduIdType FrIf_TxPduId)
{
	printf("Transmitted: PDUID=%d\n", FrIf_TxPduId);
}
void UL_RxIndication(PduIdType FrIf_RxPduId, const uint8* FrIf_SduPtr)
{
	printf("Received: PDUID=%d DATA=%08x\n", FrIf_RxPduId, *(unsigned long*)FrIf_SduPtr);
}
