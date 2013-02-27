/* Std_Types.h


*/


#ifndef STD_TYPES_H
#define STD_TYPES_H

/* Standard types */
/* In this chapter all used types included from the Std_Types.h are listed : */
typedef	unsigned char	uint8;
typedef	unsigned short	uint16;
typedef	unsigned long	uint32;
typedef unsigned long long uint64;
typedef	signed char		sint8;
typedef	signed short	sint16;
typedef	signed long		sint32;
typedef signed long long sint64;
typedef uint8 BooleanParamDef;
typedef uint8 boolean;

/* Std_VersionInfoType */
typedef struct {
  uint16	vendorID; /* Vendor ID */
  uint8     moduleID; /* Module ID */
  uint8     sw_major_version;
  uint8     sw_minor_version;
  uint8     sw_patch_version;
} Std_VersionInfoType;

/* Std_ReturnType */
typedef enum {
  E_OK = 0, /* Network mode request has been accepted */
  E_NOT_OK  /* Network mode request has not been accepted */
} Std_ReturnType;

#define True 1
#define False 0

#endif /* #ifndef STD_TYPES_H */

