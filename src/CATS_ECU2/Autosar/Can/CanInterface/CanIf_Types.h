/* CanIf_types.h


*/

#ifndef CANIF_TYPES_H
#define CANIF_TYPES_H

#include "Std_Types.h"
#include "ComStack_Types.h"


typedef uint32 Can_IdType;

/*
  Name: Can_PduType 
  Type: Structure 
  Description: This type is used to provide ID, DLC and SDU from CAN interface to CAN driver. 
*/

typedef struct Can_PduType_ {
  uint8*        sdu;
  Can_IdType    id;
  PduIdType     swPduHandle;
  PduLengthType length;
} Can_PduType;


/* EcuM specific types */
/* The following type of the EcuM shall be used : */

/* EcuM_WakeupSourceType */
typedef	uint8	EcuM_WakeupSourceType;


/* CAN specific types */
/* The following type of the CAN Driver shall be used : */



typedef void (*CanIf_BusOffCallbackType)(uint8 controller);
typedef void (*CanIf_WakeupCallbackType)(uint32 wakeupEvents);
typedef void (*CanIf_TxCallbackType)(PduIdType pduId);
typedef void (*CanIf_RxCallbackType)(PduIdType pduId, const void*);

typedef void CanGeneral;
typedef void CanHardwareObject;


#endif /* ifndef CANIF_TYPES_H */
