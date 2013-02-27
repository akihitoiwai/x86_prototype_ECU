/* Dem.h

  DEM  Diagnostic Event Manager

  contains the declarations of the API services of the Dem used by the FlexRay Interface

  @note   Demの実装は暫定です。

  Copyright(c) 2008 CATS corp.
*/
#ifndef DEM_H
#define DEM_H

#include "Std_Types.h"
#include "Platform_Types.h"

#define DEM_E_NOERROR 0
/*
  FlexRay Interface errors
*/

/*  Invalid pointer  Development */
#define FRIF_E_INV_POINTER  0x01 
/* Invalid Controller index  Development */
#define FRIF_E_INV_CTRL_IDX  0x02 
/* Invalid Cluster index  Development */
#define FRIF_E_INV_CLST_IDX  0x03 
/* Invalid Channel index  Development */
#define FRIF_E_INV_CHNL_IDX  0x04  
/* Invalid timer index  Development */
#define FRIF_E_INV_TIMER_IDX  0x05 
/* Invalid FrIf_TxPdu */
#define FRIF_E_INV_TXPDUID  0x06 
/* Invalid configuration  */
#define FRIF_E_INV_FRIFCONF_IDX  0x07 
/* FrIf not initialized  Development */
#define FRIF_E_NOT_INITIALIZED  0x08 
/* Job List Execution lost synchronization to the FlexRay Global Time */
#define FRIF_E_JLE_SYNC  0xf0  /* Assignedby DEM  */

/*
  FlexRay Driver Errors
*/

/* parameter timer index exceeds */
#define FR_E_INV_TIMER_IDX 0x01 
/* invalid pointer in parameter list */
#define FR_E_INV_POINTER 0x02 
/* parameter offset exceeds bounds */
#define FR_E_INV_OFFSET 0x03 
/* invalid controller index */
#define FR_E_INV_CTRL_IDX 0x04 
/* invalid channel index */
#define FR_E_INV_CHNL_IDX 0x05 
/* parameter cycle exceeds 63 */
#define FR_E_INV_CYCLE 0x06 
/* Invalid configuration index */
#define FR_E_INV_CONFIG 0x07 
/* Fr module was not initialized */
#define FR_E_NOT_INITIALIZED 0x08 
/* Fr CC is not in the expected POC state. */
#define FR_E_INV_POCSTATE 0x09 
/* Payload length parameter has an invalid value. */
#define FR_E_INV_LENGTH 0x0A 
/* invalid LPdu index */
#define FR_E_INV_LPDU_IDX 0x0B 
/* Access to FlexRay CC event id  Production */
#define FR_E_ACCESS 0xf1 /* Assigned by DEM */



/* Dem_Types.h */
typedef uint8 Dem_EventIdType;
typedef uint8 Dem_EventStatusType;

#define DEM_EVENT_STATUS_FAILED 1

#ifdef __cplusplus
extern "C" {
#endif

void Dem_ReportErrorStatus(Dem_EventIdType id, Dem_EventStatusType status);

/* 以下はCATS暫定仕様 */

/* Developmentエラーコードの設定 */
void Dem_SetDevelopmentError(uint8 id);

/* 最後のエラーコードを取得 */
Dem_EventIdType Dem_GetLastError();

/* 最後のDevelopmentエラーコードの取得 */
uint8 Dem_GetLastDevelopmentError();


/* Sleep μ秒 */
void Dem_Sleep(uint32 usec);

#ifdef __cplusplus
}
#endif

#endif
