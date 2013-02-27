#ifndef FRIF_GENERALTYPES_H
#define FRIF_GENERALTYPES_H

#define FRIF_CLST_IDX_MAX_SUPPORTED		1
#define FRIF_CTRL_IDX_MAX_SUPPORTED		2
#define FRIF_LPDU_IDX_MAX_SUPPORTED		4	// PDUの数(このパラメータはAUTOSAR仕様書に定義されていない)

/*
  Container Name: FrIfLPdu
  Description:
    PDU IDテーブル(このテーブルはAUTOSAR仕様書に定義されていない)
*/
typedef struct {
	PduIdType			PduId;
} FrIf_LPdu;

/*
  Container Name: FrIfController
  Description:
*/
typedef struct {
	int					FrIfCtrlIdx;
	Fr_Controller		*FrIfFrCtrlRef;
	FrIf_LPdu			FrIfLPdu[FRIF_LPDU_IDX_MAX_SUPPORTED];
} FrIf_Controller;

/*
  Container Name: FrIfController
  Description:
*/
typedef struct {
	int						FrIfClstIdx;
	FrIf_Controller			FrIfController[FRIF_CTRL_IDX_MAX_SUPPORTED];
} FrIf_Cluster;

/*
  Container Name: FrIfConfig
  Description:
    Configuration of the FlexRay Interface. This container is a
	MultipleConfigurationContainer, i.e. this container and its sub-containers
	exist once per congiguration set.
*/
typedef struct {
	FrIf_Cluster			FrIfCluster[FRIF_CLST_IDX_MAX_SUPPORTED];
} FrIf_Config;

/*
  Container Name: FrIfGeneral
  Description:
    This container contains the general configuration parameters of the
	FlexRay Interface.
*/
typedef struct {
	boolean				FrIfDevErrorDetect;
	boolean				FrIfVersionInfoApi;
} FrIf_General;

/*
  Module Name: FrIf
  Module Description:
    Configuration of the FrIf (FlexRay Interface) module.
*/
typedef struct {
	FrIf_Config				FrIfConfig;
	FrIf_General			FrIfGeneral;
} FrIf;

/*
  Service name: <UL_TxConfirmation> 
  Service ID[hex]: 0x00 
  Sync/Async: Synchronous 
  Reentrancy: Reentrant for different FrIf_TxPduId, Non reentrant for the same FrIf_TxPduId 
  Parameters (in): 
    FrIf_TxPduId  PDU-ID of FlexRay PDU whose transmission is being confirmed 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: None 
  Description: 
    This API service of an upper layer BSW module <UL> (e.g. PduR, FrTp, FrNm) is 
    called by the FlexRay Interface to confirm to this upper layer BSW module that the 
    PDU with index FrIf_TxPduId has been transmitted via the FlexRay 
    Communication System. 
*/
typedef void (*FrIf_Cbk_UL_TxConfirmation)(PduIdType FrIf_TxPduId);  

/*
  Service name: <UL_RxIndication> 
  Service ID[hex]: 0x00 
  Sync/Async: Synchronous 
  Reentrancy: Reentrant for different FrIf_RxPduId, Non reentrant for the same FrIf_RxPduId 
  Parameters (in): 
    FrIf_RxPduId  PDU-ID of FlexRay PDU that has been received 
    FrIf_SduPtr  Pointer to FlexRay SDU (buffer of received payload) 
  Parameters (inout): None 
  Parameters (out): None 
  Return value: None 
  Description: 
    This API service of an upper layer BSW module <UL> (e.g. PduR, FrTp, FrNm) is 
    called by the FlexRay Interface to indicate to this upper layer BSW module that the 
    PDU with index FrIf_RxPduId has been received via the FlexRay Communication 
    System. 
    During the execution of this API service, the upper layer BSW module that is the 
    final recipient of this PDU is expected to retireve (i.e. copy) the SDU (i.e. the 
    payload of the PDU) by means of the pointer FrIf_SduPtr and the SDU length 
    known from this BSW moduleﾁs static configuration. 
*/
typedef void (*FrIf_Cbk_UL_RxIndication)(PduIdType FrIf_RxPduId, const uint8* FrIf_SduPtr);

#endif /* FRIF_GENERALTYPES_H */
