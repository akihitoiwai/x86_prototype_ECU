/* ComStack_Types.h

  Referenced by AUTOSAR Specification of Communication Stack Type V2.2.1 R3.0 Rev 0001
*/


#ifndef COMSTACK_TYPES_H
#define COMSTACK_TYPES_H

#include "Std_Types.h"


/* COM specific types */
/* In this chapter all used types included from the ComStackTypes.h are listed : */

/* PduIdType */
typedef uint16 PduIdType;

/* PduLengthType */
typedef uint8 PduLengthType;

/* PduInfoType */
typedef struct {
  uint8* SduDataPtr;
  PduLengthType SduLength;
} PduInfoType;


/* BusTrcvErrorType */
typedef uint8 BusTrcvErrorType;
#define BUSTRCV_OK 0x00
#define BUSTRCV_E_ERROR 0x01





#endif /* #ifndef COMSTACKTYPES_H */


