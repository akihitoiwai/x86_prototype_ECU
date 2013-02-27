#ifndef FR_GENERALTYPES_H
#define FR_GENERALTYPES_H

#include "/usr/src/CATS_ECU2/Autosar/Std_Types.h"

#define FR_CTRL_IDX_MAX_SUPPORTED		2

/*
  Container Name: FrController
  Description:
*/
typedef struct {
	int				FrCtrlIdx;
	char			*ChiFilePath;
} Fr_Controller;

/*
  Container Name: FrMultipleConfiguration
  Description:
*/
typedef struct {
	Fr_Controller	FrController[FR_CTRL_IDX_MAX_SUPPORTED];
} Fr_MultipleConfiguration;

/*
  Container Name: FrGeneral
  Description:
*/
typedef struct {
	boolean			FrVersionInfoApi;
} Fr_General;

/*
  Container Name: Fr
  Description:
*/
typedef struct {
	Fr_General					FrGeneral;
	Fr_MultipleConfiguration	FrMultipleConfiguration;
} Fr;

#endif /* FR_GENERALTYPES_H */
