/* CAN_SJA1000.c

  AUTOSAR�Ή�SJA1000�pCAN_Driver
*/

#include "Compiler.h"
#include "Platform_Types.h"
#include "Dem.h"
#include "CanInterface/CanIf_Cbk.h"

#include "Can_SJA1000.h"
#include "/usr/src/CATS_ECU2/api_library/canapi/can_api.h"
#ifdef TEST
#include "Test/util/TimeRecorder.h"
#endif

static Can_ConfigType gConfig; /* �ݒ�f�[�^ */
static CAN_CONTEXT* gContext = 0; /* CAN �h���C�o�̃R���e�L�X�g */

#define CAN_SJA1000_SENDBUFFERSIZE 128 /* ���M�o�b�t�@�T�C�Y */
#define CAN_SJA1000_RECVBUFFERSIZE 128 /* ��M�o�b�t�@�T�C�Y */
#define CAN_SJA1000_MODULE_ID 0x11
#define CAN_SJA1000_VENDOR_ID 0x22
#define CAN_SJA1000_VENDORVERSIONNO 0x0001


/* �t�@���N�V�����e�[�u�� */
static Can_FunctionTable functionTable = {
  Can_Init_SJA1000, /* 0x00 */
  Can_MainFunction_Write_SJA1000, /* 0x01 */
  Can_InitController_SJA1000, /* 0x02 */
  Can_SetControllerMode_SJA1000, /* 0x03 */
  Can_DisableControllerInterrupts_SJA1000, /* 0x04 */
  Can_EnableControllerInterrupts_SJA1000, /* 0x05 */
  Can_Write_SJA1000, /* 0x06 */
  Can_GetVersionInfo_SJA1000, /* 0x07 */
  Can_MainFunction_Read_SJA1000, /* 0x08 */
  Can_MainFunction_BusOff_SJA1000, /* 0x09 */
  Can_MainFunction_Wakeup_SJA1000, /* 0x0a */
  Can_Cbk_CheckWakeup_SJA1000, /* 0x0b */
  Can_Deinit_SJA1000
};


#define SJA1000_MAXCONTROLLER 4 /* �R���g���[���[�̐� */

/* �eCAN�R���g���[���̏�� */
static uint32 ControllerStatus[SJA1000_MAXCONTROLLER];
enum {
  SJA1000_UNINITIALIZE = 0,
  SJA1000_INITIALIZED,
  SJA1000_INITIALIZE_FAILED,
  SJA1000_STARTED,
  SJA1000_SLEEPD
};

/* ���M�J�E���^ */
#define SENDPDUIDMAX 1024
static size_t SendCounter[SJA1000_MAXCONTROLLER]; /* ���M�v���̃J�E���^ */
static size_t SentCounter[SJA1000_MAXCONTROLLER]; /* ���M�m�F�ς݂̃J�E���^ */
static uint8 ControllerId[SJA1000_MAXCONTROLLER];
static PduIdType SendPduId[SJA1000_MAXCONTROLLER][SENDPDUIDMAX];

/* �eCAN�R���g���[���[�́uWeakup�v�J�E���^ */
static size_t ControllerWeakupCount[SJA1000_MAXCONTROLLER];

/*  
  Service name:       Can_Init 
  Service ID[hex]:    0x00 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant 
  Parameters (in):    config  Pointer to driver configuration. 
  Return value:       None 
  Description:        This function initializes the module. 
*/
void Can_Init_SJA1000(const Can_ConfigType* config) {
  int i;
  DebugOut("Can_Init_SJA1000 called\n");
  /* ��Ԃ̏����� */
  memset(ControllerStatus, 0, sizeof(ControllerStatus));
  /* �ݒ�f�[�^�̃R�s�[ */
  gConfig = *config;

  /* CAN api_library�̃R���e�L�X�g�擾 */
  if (gContext == 0) {
    gContext = CAN_create();
    if (gContext == NULL) {
      /* ���s�������Ԃ�SJA1000_INITIALIZE_FAILED�ɃZ�b�g */
      for (i = 0; i < SJA1000_MAXCONTROLLER; ++i) {
        ControllerStatus[i] = SJA1000_INITIALIZE_FAILED;
      }
      DebugOut("CAN_create failed\n");
      Dem_ReportErrorStatus(CAN_E_TRANSITION, DEM_EVENT_STATUS_FAILED);
      return;
    }
  }
  
  /* �e�f�o�C�X�̏����� */
  for (i = 0; i < SJA1000_MAXCONTROLLER; ++i) {
    CAN_STATUS st;
    if (CAN_init(gContext, i, CAN_SJA1000_SENDBUFFERSIZE, CAN_SJA1000_RECVBUFFERSIZE) != 0) {
      DebugOut1("FncInitCAN(%d) failed\n", i);
      ControllerStatus[i] = SJA1000_INITIALIZE_FAILED;
      return;
    }
    ControllerStatus[i] = SJA1000_UNINITIALIZE;
    CAN_status(gContext, i, &st);
    SendCounter[i] = st.send_count;
    SentCounter[i] = st.send_count;
  }
}

/*
  Service name:       Can_GetVersionInfo 
  Service ID[hex]:    0x07 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant 
  Parameters (out):   versioninfo  Pointer to where to store the version information of this module. 
  Return value:       None 
  Description:        This function returns the version information of this module. 
*/ 
void Can_GetVersionInfo_SJA1000(Std_VersionInfoType* versioninfo) {
  /* TODO: ID�͎b�� */
  versioninfo->moduleID = CAN_SJA1000_MODULE_ID;
  versioninfo->vendorID = CAN_SJA1000_VENDOR_ID;
  versioninfo->sw_major_version = CAN_SJA1000_VENDORVERSIONNO;
  versioninfo->sw_minor_version = 1;
  versioninfo->sw_patch_version = 0;
}
  

/*
  Service name:       Can_InitController 
  Service ID[hex]:    0x02 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant Controller  CAN controller to be initialized 
  Parameters (in):    config  Pointer to controller configuration. 
  Return value:       None 
  Description:        This function initializes only CAN controller specific settings. 
*/ 
void Can_InitController_SJA1000(uint8 controller, const Can_ControllerConfigType* config) {
  CAN_MODE mode;
  if (controller >= SJA1000_MAXCONTROLLER) return;
  if (gContext == 0) return; // not initialized

  mode.baud_rate = config->baudRate; /* baud rate */
  /* �t�B���^�̏����� */
  mode.accept_code[0] = (unsigned char)(config->acceptCode >> 24);
  mode.accept_code[1] = (unsigned char)((config->acceptCode >> 16) & 0xff);
  mode.accept_code[2] = (unsigned char)((config->acceptCode >> 8) & 0xff);
  mode.accept_code[3] = (unsigned char)(config->acceptCode & 0xff);
  mode.accept_mask[0] = (unsigned char)(config->acceptMask >> 24);
  mode.accept_mask[1] = (unsigned char)((config->acceptMask >> 16) & 0xff);
  mode.accept_mask[2] = (unsigned char)((config->acceptMask >> 8) & 0xff);
  mode.accept_mask[3] = (unsigned char)(config->acceptMask & 0xff);


  if (CAN_reset(gContext, controller, &mode) != 0) {
    DebugOut1("SJA1000_init(%d) failed\n", controller);
    ControllerStatus[controller] = SJA1000_INITIALIZE_FAILED;
  }
  else {
    DebugOut1("SJA1000_init(%d) succeeded\n", controller);
    ControllerStatus[controller] = SJA1000_INITIALIZED;
  }
}
  

/*
  Service name:       Can_SetControllerMode 
  Service ID[hex]:    0x03 
  Sync/Async:         Asynchronous 
  Reentrancy:         Non Reentrant Controller  CAN controller for which the status shall be changed 
  Parameters (in):    Transition  -- 
  Return value:       Can_ReturnType
                      CAN_OK: transition initiated 
                      CAN_NOT_OK: development or production or a wakeup during transition to sleep occured  
  Description:        This function performs software triggered state transitions of the CAN controller State machine. 
*/ 
Can_ReturnType Can_SetControllerMode_SJA1000(uint8 controller, Can_StateTransitionType transition) {
  DebugOut2("Can_SetControllerMode_SJA1000(%u, %d) called\n", controller, (int)transition);
  if (controller >= SJA1000_MAXCONTROLLER) return CAN_NOT_OK;
  if (ControllerStatus[controller] == SJA1000_UNINITIALIZE || gContext == 0) {
    return CAN_NOT_OK;
  }

  switch(transition) {
  case CAN_T_START:
    if (CAN_start(gContext, controller) == 0) {
      CAN_STATUS st;
      ControllerStatus[controller] = SJA1000_STARTED;
      CAN_status(gContext, controller, &st);
      SendCounter[controller] = st.send_count;
      SentCounter[controller] = st.send_count;
      return CAN_OK;
    }
    else {
      DebugOut("CAN_start failed\n");
    }
    break;
  case CAN_T_STOP:
    if (CAN_stop(gContext, controller) == 0) {
      ControllerStatus[controller] = SJA1000_UNINITIALIZE; /* stop���init���K�v */
      return CAN_OK;
    }
    else {
      DebugOut("CAN_stop failed\n");
    }
    break;

  case CAN_T_SLEEP:
    if (CAN_sleep(gContext, controller) == 0) {
      ControllerStatus[controller] = SJA1000_SLEEPD;
      return CAN_OK;
    }
    else {
      DebugOut("CAN_sleep failed\n");
    }
    break;

  case CAN_T_WAKEUP:
    /* TODO: CAN_wakeup������?  */
    break;

  default:
    break;
  }
  /* ���s */
  DebugOut2("Can_SetControllerMode_SJA1000(%d, %d) failed\n", controller, transition);
  return CAN_NOT_OK;
}
  

/*
  Service name:       Can_DisableControllerInterrupts 
  Service ID[hex]:    0x04 
  Sync/Async:         Synchronous 
  Reentrancy:         Reentrant 
  Parameters (in):    Controller  CAN controller for which interrupts shall be disabled. 
  Return value:       None 
  Description:        This function disables all interrupts for this CAN controller. 
*/
void Can_DisableControllerInterrupts_SJA1000(uint8 controller) {
 DebugOut("Can_DisableControllerInterrupts_SJA1000 called\n");
 /* TODO: ���荞�݋֎~�̏��� */
}


/*
  Service name:       Can_EnableControllerInterrupts 
  Service ID[hex]:    0x05 
  Sync/Async:         Synchronous 
  Reentrancy:         Reentrant 
  Parameters (in):    Controller  CAN controller for which interrupts shall be re-enabled 
  Return value:       None 
  Description:        This function enables all allowed interrupts. 
*/
void Can_EnableControllerInterrupts_SJA1000(uint8 controller) {
 DebugOut("Can_EnableControllerInterrupts_SJA1000 called\n");
 /* TODO: ���荞�݋��̏��� */
}


/*
  Service name:       Can_Cbk_CheckWakeup 
  Service ID[hex]:    0x00 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant 
  Parameters (in):    controller  -- 
  Return value:       None 
  Description: 
  void Can_Cbk_CheckWakeup(uint8 controller) {
}

*/ 

/*
  Service name:       Can_Cbk_CheckWakeup 
  Service ID[hex]:    0x0b 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant 
  Parameters (in):    Controller  Controller to be checked for a wakeup. 
  Return value:       Std_ReturnType
                      E_OK: A wakeup was detected for the given controller. 
                      E_NOT_OK: No wakeup was detected for the given controller. 
  Description:        This function checks if a wakeup has occurred for the given controller. 
*/
Std_ReturnType Can_Cbk_CheckWakeup_SJA1000(uint8 controller) {
  /* TODO: CheckWakeup�̏��� */
  return E_OK;
}
  

/*
  Service name:       Can_Write 
  Service ID[hex]:    0x06 
  Sync/Async:         Synchronous 
  Reentrancy:         Reentrant (thread-safe) 
  Parameters (in):    Hth       information which HW-transmit handle shall be used for transmit. 
                                Implicitly this is also the information about the controller to use 
                                because the Hth numbers are unique inside one hardware unit. 
                      PduInfo   Pointer to SDU user memory, DLC and Identifier. 
  Return value:       Can_ReturnType
                      CAN_OK: Write command has been accepted 
                      CAN_NOT_OK: development error occured 
                      CAN_BUSY: No TX hardware buffer available or preemptive call of Can_Write that can't be implemented reentrant 
  Description: 
*/
Can_ReturnType Can_Write_SJA1000(uint8 hth, const Can_PduType* pduInfo) {
  int result;
  CAN_FRAME send_frame;
  /* �f�[�^���M���� */
  DebugOut4("Can_Write_SJA1000(%d) called ID:%x Dlc:%d '0x%x'\n", hth, pduInfo->id, pduInfo->length, *(int*)pduInfo->sdu);

  send_frame.type = 1; /* 0:std, 1:ext */
  send_frame.id = pduInfo->id;
  send_frame.dlc = pduInfo->length;
  send_frame.length = pduInfo->length;
  if (pduInfo->length != 0) {
    memcpy(send_frame.data, pduInfo->sdu, pduInfo->length);
  }

#ifdef TEST
  TimeRecorderStart(TR_CAN_SJA10000_CAN_SEND_DEVICE);
#endif
  /* ���M�J�E���^�̍X�V ������ړ� TxCB�����΍� 20100225*/
  SendPduId[hth][SendCounter[hth] % SENDPDUIDMAX] = pduInfo->swPduHandle;
  SendCounter[hth] += 1;
//  printf("Befor send SendCounter[hth]=%04x\n", SendCounter[hth]);

  result = CAN_send_device(gContext, hth, &send_frame);

#ifdef TEST
  TimeRecorderStop(TR_CAN_SJA10000_CAN_SEND_DEVICE);
#endif

	if( result == CASTCAN_SUCCEEDED)
	{											/* ���� */
		/* ���M�J�E���^�̍X�V */
//		printf("Can_Write_SJA1000:CASTCAN_SUCCEEDED\n");
		DebugOut3("Can_Write(#%d, %d, 0x%x)\n", hth, SendCounter[hth], pduInfo->swPduHandle);
//��Ɉړ� 20100225	SendPduId[hth][SendCounter[hth] % SENDPDUIDMAX] = pduInfo->swPduHandle;
//��Ɉړ� 20100225	SendCounter[hth] += 1;
		return CAN_OK;
	}else
	{
		SendCounter[hth] -= 1;				//�G���[���̓J�E���^��߂� 20100225
//		printf("On Error SendCounter[hth]=%04x\n", SendCounter[hth]);
		if( result == CATSCAN_INVALID_DEVICENUM)
		{											/* �f�o�C�X�w�肪�͈͊O */
	//		printf("Can_Write_SJA1000:CATSCAN_INVALID_DEVICENUM\n");
			DebugOut("CAN_send_device failed.\n");
			return CAN_NOT_OK;
	
		}else if( result == CATSCAN_SEND_BUFFER_OVERFLOW)
		{											/* ���M�o�b�t�@�����t */
	//		printf("Can_Write_SJA1000:CATSCAN_SEND_BUFFER_OVERFLOW\n");
			DebugOut("CAN_send_device Buff_Busy.\n");
			return CAN_BUSY;
		}
	}
	DebugOut("CAN_send_device other error.\n");	
	return CAN_NOT_OK;	//���̑��G���[����NotOk��Ԃ�
}

/*
  Service name:       Can_MainFunction_Write 
  Service ID[hex]:    0x01 
  Timing:             FIXED_CYCLIC 
  Description:        This function performs the polling of TX confirmation and TX cancellation 
                      confirmation when CAN_TX_PROCESSING is set to POLLING. 
*/
void Can_MainFunction_Write_SJA1000() {
  uint8 ch;
  /* ���M�p�|�[�����O���[�`�� */
  for (ch = 0; ch < SJA1000_MAXCONTROLLER; ++ch) {
    if (ControllerStatus[ch] == SJA1000_STARTED) {
      CAN_STATUS st;
      size_t sc = SentCounter[ch];
      CAN_status(gContext, ch, &st);
      while (st.send_count > sc) {
        PduIdType pduId;
        /* ���M���� */
        pduId = SendPduId[ch][sc % SENDPDUIDMAX];
        DebugOut3("TxConfirm #%d %d, 0x%x\n", ch, sc, pduId);
        CanIf_TxConfirmation(pduId);
        ++sc;
      }
      SentCounter[ch] = sc;
    }
  }

}
  

/*
  Service name:       Can_MainFunction_Read 
  Service ID[hex]:    0x08 
  Timing:             FIXED_CYCLIC 
  Description:        This function performs the polling of RX indications when 
                      CAN_RX_PROCESSING is set to POLLING. 
*/
void Can_MainFunction_Read_SJA1000() {
  CAN_FRAME recv_frame[SJA1000_MAXCONTROLLER];
  int ch;
  /* ��M�p�|�[�����O���[�`�� */
  /* �|�[�����O��M�̏ꍇ�A�����Ŏ�M�f�[�^�����邩�`�F�b�N���� */
  
  for (ch = 0; ch < SJA1000_MAXCONTROLLER; ++ch) {
    if (ControllerStatus[ch] == SJA1000_STARTED) {
      /* ��M�o�b�t�@�̏�Ԃ𒲂ׂ� */
      CAN_STATUS st;
      CAN_status(gContext, ch, &st);
      while (st.recv_buff_count > 0) {
#ifdef TEST
	TimeRecorderStart(TR_CAN_SJA10000_CAN_RECV_DEVICE);
#endif
        CAN_recv_device(gContext, ch, &recv_frame[ch]);
#ifdef TEST
	TimeRecorderStop(TR_CAN_SJA10000_CAN_RECV_DEVICE);
#endif
        /* ��M�f�[�^����̃R�[���o�b�N */
        DebugOut1("CanRecevieData has come #%d\n", ch);
        CanIf_RxIndication((uint8)ch, recv_frame[ch].id,  recv_frame[ch].dlc, recv_frame[ch].data);
        CAN_status(gContext, ch, &st);
      }
    }
  }
}
  

/*
  Service name:       Can_MainFunction_BusOff 
  Service ID[hex]:    0x09 
  Timing:             FIXED_CYCLIC 
  Description:        This function performs the polling of bus-off events that are configured statically as 
                      'to be polled'. 
*/
void Can_MainFunction_BusOff_SJA1000() {
  /* BusOff���o�p�|�[�����O���[�`�� */
}
  
 
/*
  Service name:       Can_MainFunction_Wakeup 
  Service ID[hex]:    0x0a 
  Timing:             FIXED_CYCLIC 
  Description:        This function performs the polling of wake-up events that are configured statically 
                      as 'to be polled'. 
*/
void Can_MainFunction_Wakeup_SJA1000() {
  int ch;
  /* Wakeup���o�p�|�[�����O���[�`�� */
  /* Wakeup ���荞�݂���������ƁA�J�E���^����i�� */
  
  for (ch = 0; ch < SJA1000_MAXCONTROLLER; ++ch) {
    CAN_STATUS st;
    CAN_status(gContext, ch, &st);
    if (st.wakeup_count != ControllerWeakupCount[ch]) {
      ControllerWeakupCount[ch] = st.wakeup_count;
      /* Wakeup ���荞�݂����o�I */
    }
  }
}

/* �e�t�@���N�V�����̃A�h���X�e�[�u����Ԃ� */
Can_FunctionTable* Can_GetFunctionTable_SJA1000() {
  return &functionTable;
}

/* �h���C�o�̏I������ */
void Can_Deinit_SJA1000() {
  int ch;
  if (gContext != 0) {
    for (ch = 0; ch < SJA1000_MAXCONTROLLER; ++ch) {
      CAN_exit(gContext, ch);
    }
    CAN_destroy(gContext);
    gContext = 0;
  }
}

#ifdef __cplusplus
} // extern "C"
#endif


