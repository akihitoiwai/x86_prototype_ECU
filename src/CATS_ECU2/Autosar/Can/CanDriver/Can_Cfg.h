/* Can_Cfg.h


*/

#ifndef CAN_CFG_H
#define CAN_CFG_H


/**
  @brief        Can_ConfigType�\����
�@@note         Can_ConfigType�͎����ˑ��̂��߁A���ʕ����`����B
  Type:         Structure 
  Range:        Implementation specific. 
  Description:  This is the type of the external data structure containing the overall initialization 
                data for the CAN driver and SFR settings affecting all controllers. Furthermore it 
                contains pointers to controller configuration structures. The contents of the 
                initialization data structure are CAN hardware specific. 

 */
typedef struct {
  uint32 deviceId;
} Can_ConfigType;

/*
  Name:         Can_ControllerConfigType 
  Type:         Structure 
  Range:        Implementation specific.   
  Description:  This is the type of the external data structure containing the overall initialization 
                data for one CAN controller. The contents of the initialization data structure are 
                CAN hardware specific. 
*/
typedef struct {
  uint32 deviceId;
  uint32 baudRate; /* �{�[���[�g */
  uint8  btr0;     /* btr���W�X�^0 */
  uint8  btr1;     /* btr���W�X�^1 */
  uint32 acceptCode; /* Accept Code */
  uint32 acceptMask; /* Accept Mask */
} Can_ControllerConfigType;



#endif /* #ifndef CAN_CFG_H */

