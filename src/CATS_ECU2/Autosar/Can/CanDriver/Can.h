/* CanDriver.h



*/

#ifndef CANDRIVER_H
#define CANDRIVER_H

#include "CanInterface/CanIf_Types.h"
#include "CanInterface/CanIf_Cfg.h"

/*
  enums or defines
*/

/*
  @brief Error classification
*/
enum CanDriverErrors {
  CAN_E_PARAM_POINTER = 0x01,
  CAN_E_PARAM_HANDLE  = 0x02,
  CAN_E_PARAM_DLC     = 0x03,
  CAN_E_CONTROLLER    = 0x04,
  CAN_E_UNINIT        = 0x05,
  CAN_E_TRANSITION    = 0x06,
  CAN_E_TIMEOUT       = 0xf0, /* Assigned by DEM */
};


#define CANDRV_STRUCTURENAMELEN 8 /* ç\ë¢ëÃÇÃéØï ñºÇÃí∑Ç≥ */


/*
  structures
*/





/*
  Name: Can_PduType 
  Type: Structure 
  Description: This type is used to provide ID, DLC and SDU from CAN interface to CAN driver. 
  # CanIf_types.hÇ≈íËã`
*/

/*
  Name: Can_IdType
  Type: uint32,uint16 
  Range: 0...0xFFFFFFFF for Extended IDs 
         0...0x7FF for Standard IDs 
  Description:  Represents the Identifier of an L-PDU. For extended IDs the most significant bit is 
                set. 
  typedef uint32 Can_IdType;
  # CanIf_types.hÇ≈íËã`
*/


/*
  Name: Can_StateTransitionType 
  Type: Enumeration 
  Description:State transitions that are used by the function CAN_SetControllerMode 
*/
enum Can_StateTransitionType_t { 
  CAN_T_START, 
  CAN_T_STOP, 
  CAN_T_SLEEP, 
  CAN_T_WAKEUP 
};
typedef uint32 Can_StateTransitionType;

/*
  Name: Can_ReturnType 
  Type: Enumeration 
  Range:Description: Return values of CAN driver API . 
*/
enum Can_ReturnType_t {
  CAN_OK,     /* success */ 
  CAN_NOT_OK, /* error occured or wakeup event occurred during sleep transition */
  CAN_BUSY    /* transmit request could not be processed because no transmit object was available */
};
typedef uint32 Can_ReturnType;



/*
  functions
*/

/*  
  Service name:       Can_Init 
  Service ID[hex]:    0x00 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant 
  Parameters (in):    config  Pointer to driver configuration. 
  Return value:       None 
  Description:        This function initializes the module. 
  void Can_Init(const Can_ConfigType* config);
*/
typedef void (*Can_Init_proc)(const Can_ConfigType* config);

/*
  Service name:       Can_GetVersionInfo 
  Service ID[hex]:    0x07 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant 
  Parameters (out):   versioninfo  Pointer to where to store the version information of this module. 
  Return value:       None 
  Description:        This function returns the version information of this module. 
  void Can_GetVersionInfo(Std_VersionInfoType* versioninfo);  
*/ 
typedef void (*Can_GetVersionInfo_proc)(Std_VersionInfoType* versioninfo);

/*
  Service name:       Can_InitController 
  Service ID[hex]:    0x02 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant Controller  CAN controller to be initialized 
  Parameters (in):    config  Pointer to controller configuration. 
  Return value:       None 
  Description:        This function initializes only CAN controller specific settings. 
  void Can_InitController(uint8 controller, const Can_ControllerConfigType* config);  
*/ 
typedef void (*Can_InitController_proc)(uint8 controller, const Can_ControllerConfigType* config);

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
  Can_ReturnType Can_SetControllerMode(uint8 controller, Can_StateTransitionType transition);  
*/ 
typedef Can_ReturnType (*Can_SetControllerMode_proc)(uint8 controller, Can_StateTransitionType transition);

/*
  Service name:       Can_DisableControllerInterrupts 
  Service ID[hex]:    0x04 
  Sync/Async:         Synchronous 
  Reentrancy:         Reentrant 
  Parameters (in):    Controller  CAN controller for which interrupts shall be disabled. 
  Return value:       None 
  Description:        This function disables all interrupts for this CAN controller. 
  void Can_DisableControllerInterrupts(uint8 controller);
*/
typedef void (*Can_DisableControllerInterrupts_proc)(uint8 controller);

/*
  Service name:       Can_EnableControllerInterrupts 
  Service ID[hex]:    0x05 
  Sync/Async:         Synchronous 
  Reentrancy:         Reentrant 
  Parameters (in):    Controller  CAN controller for which interrupts shall be re-enabled 
  Return value:       None 
  Description:        This function enables all allowed interrupts. 
  void Can_EnableControllerInterrupts(uint8 controller);
*/
typedef void (*Can_EnableControllerInterrupts_proc)(uint8 controller);

/*
  Service name:       Can_Cbk_CheckWakeup 
  Service ID[hex]:    0x00 
  Sync/Async:         Synchronous 
  Reentrancy:         Non Reentrant 
  Parameters (in):    controller  -- 
  Return value:       None 
  Description: 
  void Can_Cbk_CheckWakeup(uint8 controller);
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
  Std_ReturnType Can_Cbk_CheckWakeup(uint8 controller);  
*/
typedef Std_ReturnType (*Can_Cbk_CheckWakeup_proc)(uint8 controller);

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
                      CAN_BUSY: No TX hardware buffer available or preemptive call of Can_Write that can¡t be implemented reentrant 
  Description: 
  Can_ReturnType Can_Write(uint8 hth, const Can_PduType* pduInfo);  
*/
typedef Can_ReturnType (*Can_Write_proc)(uint8 hth, const Can_PduType* pduInfo);

/*
  Service name:       Can_MainFunction_Write 
  Service ID[hex]:    0x01 
  Timing:             FIXED_CYCLIC 
  Description:        This function performs the polling of TX confirmation and TX cancellation 
                      confirmation when CAN_TX_PROCESSING is set to POLLING. 
  void Can_MainFunction_Write();  
*/
typedef void (*Can_MainFunction_Write_proc)();

/*
  Service name:       Can_MainFunction_Read 
  Service ID[hex]:    0x08 
  Timing:             FIXED_CYCLIC 
  Description:        This function performs the polling of RX indications when 
                      CAN_RX_PROCESSING is set to POLLING. 
  void Can_MainFunction_Read();  
*/
typedef void (*Can_MainFunction_Read_proc)();

/*
  Service name:       Can_MainFunction_BusOff 
  Service ID[hex]:    0x09 
  Timing:             FIXED_CYCLIC 
  Description:        This function performs the polling of bus-off events that are configured statically as 
                      'to be polled'. 
  void Can_MainFunction_BusOff();  
*/
typedef void (*Can_MainFunction_BusOff_proc)();  
 
/*
  Service name:       Can_MainFunction_Wakeup 
  Service ID[hex]:    0x0a 
  Timing:             FIXED_CYCLIC 
  Description:        This function performs the polling of wake-up events that are configured statically 
                      as 'to be polled'. 
  void Can_MainFunction_Wakeup();
*/
typedef void (*Can_MainFunction_Wakeup_proc)();

typedef void (*Can_Deinit_proc)();

typedef struct Can_FunctionTable_ {
  Can_Init_proc Can_Init; /* 0x00 */
  Can_MainFunction_Write_proc Can_MainFunction_Write; /* 0x01 */
  Can_InitController_proc Can_InitController; /* 0x02 */
  Can_SetControllerMode_proc Can_SetControllerMode; /* 0x03 */
  Can_DisableControllerInterrupts_proc Can_DisableControllerInterrupts; /* 0x04 */
  Can_EnableControllerInterrupts_proc Can_EnableControllerInterrupts; /* 0x05 */
  Can_Write_proc Can_Write; /* 0x06 */
  Can_GetVersionInfo_proc Can_GetVersionInfo; /* 0x07 */
  Can_MainFunction_Read_proc Can_MainFunction_Read; /* 0x08 */
  Can_MainFunction_BusOff_proc Can_MainFunction_BusOff; /* 0x09 */
  Can_MainFunction_Wakeup_proc Can_MainFunction_Wakeup; /* 0x0a */
  Can_Cbk_CheckWakeup_proc Can_Cbk_CheckWakeup; /* 0x0b */
  Can_Deinit_proc Can_Deinit;
} Can_FunctionTable;



#endif


