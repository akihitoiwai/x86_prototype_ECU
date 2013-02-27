/* CanDriverSaj1000.h


  SJA1000用のCANドライバヘッダファイル

*/


#ifndef CANDRIVERSJA1000_H
#define CANDRIVERSJA1000_H

#include "CanDriver/Can.h"

#define CAN_DRIVERNAME_SJA1000 "SJA1000\0"


#ifdef __cplusplus
extern "C" {
#endif
Can_FunctionTable* Can_GetFunctionTable_SJA1000();
  
void Can_Init_SJA1000(const Can_ConfigType* config);
void Can_MainFunction_Write_SJA1000();
void Can_InitController_SJA1000(uint8 controller, const Can_ControllerConfigType* config);
Can_ReturnType Can_SetControllerMode_SJA1000(uint8 controller, Can_StateTransitionType transition);
void Can_DisableControllerInterrupts_SJA1000(uint8 controller);
void Can_EnableControllerInterrupts_SJA1000(uint8 controller);
Can_ReturnType Can_Write_SJA1000(uint8 hth, const Can_PduType* pduInfo);
void Can_GetVersionInfo_SJA1000(Std_VersionInfoType* versioninfo);
Std_ReturnType Can_Cbk_CheckWakeup_SJA1000(uint8 controller);
void Can_MainFunction_Read_SJA1000();
void Can_MainFunction_BusOff_SJA1000();  
void Can_MainFunction_Wakeup_SJA1000();
void Can_Deinit_SJA1000();

#ifdef __cplusplus
} // extern "C"
#endif

#endif

