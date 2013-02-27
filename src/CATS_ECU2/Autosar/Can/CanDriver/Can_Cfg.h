/* Can_Cfg.h


*/

#ifndef CAN_CFG_H
#define CAN_CFG_H


/**
  @brief        Can_ConfigType構造体
　@note         Can_ConfigTypeは実装依存のため、共通部を定義する。
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
  uint32 baudRate; /* ボーレート */
  uint8  btr0;     /* btrレジスタ0 */
  uint8  btr1;     /* btrレジスタ1 */
  uint32 acceptCode; /* Accept Code */
  uint32 acceptMask; /* Accept Mask */
} Can_ControllerConfigType;



#endif /* #ifndef CAN_CFG_H */

