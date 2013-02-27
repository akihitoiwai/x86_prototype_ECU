/* CanIf.h */


#ifndef CANIF_H
#define CANIF_H

#include "CanIf_Types.h"
#include "CanIf_Cfg.h"

/* Std_VersionInfoType */

/* Development error tracer */
#define	CANIF_E_PARAM_POINTER				2
#define	CANIF_E_PARAM_CONTROLLER			3
#define	CANIF_E_PARAM_WAKEUPSOURDE			4
#define	CANIF_E_PARAM_LPDU					5
#define	CANIF_E_PARAM_HRH					6
#define	CANIF_E_PARAM_CANID					7
#define	CANIF_E_PARAM_DLC					8

#define	CANIF_E_UNINIT						9

#define	CANIF_E_INVALID_TXPDUID				10
#define	CANIF_E_INVALID_RXPDUID				11

#define	CANIF_READRXPDU_DATA_API			12
#define	CANIF_READTXPDU_NOTIFY_STATUS_API	13
#define	CANIF_VERSION_INFO_API				14
#define	CANIF_SETDYNAMICTXID_API			15

#define	CANIF_CANTXPDUID_CONTROLLER			16
#define	CANIF_CANRXPDUID_CONTROLLER			17

#define	CANIF_TRCV_E_TRANSCEIVER			18
#define	CANIF_TRCV_E_TRCV_NOT_STANDBY		19
#define	CANIF_TRCV_E_TRCV_NOT_NORMAL		20

#define	CANIF_WAKEUP_SUPPORT				21
#define	CANIF_WAKEUP_VALIDATION				22

#define	CANIF_TRANSMIT_CANCELLATION			23

#define	CANTRCV_E_UNINIT					24



/* Description : Operating modes of the CAN network and CAN Driver */
enum {
	CANIF_CS_UNINIT = 0,						/* UNINIT mode. Default mode of hte CAN Driver and all CAN controllers connected to one CAN network after power on. */
	CANIF_CS_STOPPED,							/* STOPPED mode. At least one of all CAN controllers connected to one CAN network is halted and does not operate on the network. */
	CANIF_CS_STARTED,							/* STARTED mode. All CAN controllers connected to one CAN network are started by the CAN Driver and in full-operational mode. */
	CANIF_CS_SLEEP								/* SLEEP mode. At least one of all CAN controllers connected to one CAN network are set into the SLEEP mode and can be woken up by request of the CAN Driver or by a network event (must be supported by CAN hardware) */
};

/* CanIf_ChannelSetModeType */
typedef	uint8	CanIf_ChannelSetModeType;
/* Description : Request for PDU channel group. The request type of the channel defines it's transmit or receive activity. Communication direction (transmission and/or reception) of the channel can be controlled separately or together by upper layers. */
enum {
	CANIF_SET_OFFLINE = 0,						/* Channel shall be set to the offline mode => no transmission and reception */
	CANIF_SET_RX_OFFLINE,						/* Receive path of the corresponding channel shall be disabled */
	CANIF_SET_RX_ONLINE,						/* Receive path of the corresponding channel shall be enabled */
	CANIF_SET_TX_OFFLINE,						/* Transmit path of the corresponding channel shall be disabled */
	CANIF_SET_TX_ONLINE,						/* Transmit path of the corresponding channel shall be enabled */
	CANIF_SET_ONLINE,							/* Channel shall be set to online mode => full operation mode */
	CANIF_SET_TX_OFFLINE_ACTIVE					/* Transmit path of the corresponding channel shall be set to the offline active mode => notifications are processed but transmit requests are blocked. */
};

/* CanIf_PduGetModeType */
typedef	uint8	CanIf_PduGetModeType;
/* Description : Status of the PDU channel group. Current mode of the channel defines its transmit or receive activity. Communication direction (transmission and/or reception) of the channel can be controlled separately or together by upper layers. */
enum {
	CANIF_GET_OFFLINE = 0,						/* Channel is in the offline mode => no transmission and reception */
	CANIF_GET_RX_ONLINE,						/* Receive path of the corresponding channel is enabled and transmit path is disabled. */
	CANIF_GET_TX_ONLINE,						/* Transmit path of the corresponding channel is enabled and receive path is disabled. */
	CANIF_GET_ONLINE,							/* Channel is in the online mode => full operation mode */
	CANIF_GET_OFFLINE_ACTIVE,					/* Transmit path of the corresponding channel is in the offline active mode => transmit notifications are processed but transmit requests are blocked. The receive path is disabled. */
	CANIF_GET_OFFLINE_ACTIVE_RX_ONLINE			/* Transmit path of the corresponding channel is in the offline active mode => transmit notifications are processed but transmit requests are blocked. The receive path is enabled. */
};

/* CanIf_NotifStatusType */
typedef	uint8	CanIf_NotifStatusType;
/* Description : Return value of CAN L-PDU notification status. */
enum {
	CANIF_NO_NOTIFICATION = 0,					/* No transmit or receive event occurred for the requested L-PDU. */
	CANIF_TX_RX_NOTIFICATION					/* The requested Rx/Tx CAN L-PDU was successfully transmitted or received. */
};

/* CanIf_TransceiverModeType */
typedef	uint8	CanIf_TransceiverModeType;
/* Description : Operating modes of the CAN Transceiver Driver. */
enum {
	CANIF_TRCV_MODE_NORMAL = 0,					/* Transceiver mode NORMAL */
	CANIF_TRCV_MODE_STANDBY,					/* Transceiver mode STANDBY */
	CANIF_TRCV_MODE_SLEEP						/* Transceiver mode SLEEP */
};

/* CanIf_TrcvWakeupReasonType */
typedef	uint8	CanIf_TrcvWakeupReasonType;
/* Description : This type shall be used to specify the wake up reason detected by the CAN transceiver in detail. */
enum {
	CANIF_TRCV_WU_ERROR = 0,					/* Due to an error wake up reason was not detected. This value may only be reported when error was reported to DEM before. */
	CANIF_TRCV_WU_NOT_SUPPORTED,				/* The transceiver does not support any information for the wake up reason. */
	CANIF_TRCV_WU_BY_BUS,						/* The transceiver has detected, that the network has caused the wake up of the ECU. */
	CANIF_TRCV_WU_INTERNALLY,					/* The transceiver has detected, that the network has woken up by the ECU via a request to NORMAL mode. */
	CANIF_TRCV_WU_RESET,						/* The transceiver has detected, that the "wake up" is due to an ECU reset. */
	CANIF_TRCV_WU_POWER_ON						/* The transceiver has detected, that the "wake up" is due to an ECU reset after power on. */
};

/* CanIf_TrcvWakeupModeType */
typedef	uint8	CanIf_TrcvWakeupModeType;
/* Description : This type shall be used to specify the wake up reason detected by the CAN transceiver in detail. */
enum {
	CANIF_TRCV_WU_ENABLE = 0,					/* The notification for wakeup events is enabled on the addressed network. */
	CANIF_TRCV_WU_DISABLE,						/* The notification for wakeup events is disabled on the addressed network. */
	CANIF_TRCV_WU_CLEAR							/* A stored wakeup event is cleared on the addressed network. */
};

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Service name :	CanIf_Init
 * Service ID :		0x01
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		ConfigPtr : Pointer to configuration parameter set, used e.g. for post build parameters
 * Return value :
 * Description :	CANIF001 : This service initializes internal and external interfaces of the CAN Interface for the further processing. All underlying CAN controllers and CAN transceivers still remain not operational.
 *					This service is called only ECU State Manager (EcuM).
 *					If a NULL pointer is passed for *ConfigPtr to this function the default configuration shall be used.
 *					In case only one configuration setup is used, a NULL pointer is sufficient to choose the one static existing configuration setup.
 *					Development errors :
 *					Invalid values of *ConfigPtr will be reported to the development error tracer (CANIF_E_PARAM_POINTER) only for post built use cases.
 * Caveats :
 * Configuratinos :
 */
void					CanIf_Init(const CanIf_ConfigType * ConfigPtr);
/*
 * Service name :	CanIf_InitController
 * Service ID :		0x02
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Controller        : CAN controller requested for initialization
 *					ConfigrationIndex : Index to controller related configuration setup
 * Return value :
 * Description :	CANIF002 : This service initializes in the CAN Interface the configured buffers of all Tx/Rx L-PDUs of the corresponding CAN controller. Different sets of static configuration may have been configured. A logical number is assigned to each set statically.
 *					The parameter ConfigurationIndex selects the configuration set that is used for initialization. The CAN controller still remains not operational and neither sends nor receives CAN L-PDUs.
 *					CANIF022 : Development errors :
 *					Invalid values of Controller or ConfigurationIndex will be reported to the development error tracer (CANIF_E_PARAM_CONTROLLER or CANIF_E_PARAM_POINTER) only for post built use cases.
 *					If the CAN Interface was not initialized before invoking of CanIf_Init(), the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). No initialization will be executed.
 * Caveats :		CAN identifiers of dynamic transmit L-PDUs are not initialized by invoking this API.
 * Configuratinos :	ID of the CAN controller is published inside the configuration description of the CAN Interface. At configuration time the relation has to be set up between the CAN Interface configuration set and the available corresponding CAN conrtoller configuration sets by the CAN Driver configuration.
 */
void					CanIf_InitController(uint8 Controller, uint8 ConfigurationIndex);
/*
 * Service name :	CanIf_SetControllerMode
 * Service ID :		0x03
 * Sync/Async :		Asynchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Controller     : CAN controller requested for mode transition
 *					ControllerMode : Requested mode transition
 * Return value :	E_OK     : Network mode request has been accepted
 *					E_NOT_OK : Network mode request has not been accepted
 * Description :	CANIF003 : This service calls the corresponding CAN Driver service for changing of the CAN controller mode. It initiates a transition to the requested CAN controller mode of one or multiple CAN controllers.
 *					This service calls Can_SetControllerMode(Controller, Transition) for the requested CAN controller.
 *					Development errors :
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 *					Invalid values of Controller will be reported to the development error tracer (CANIF_E_PARAM_CONTROLLER) only for post built use cases.
 * Caveats :		Re-entrant calls of this API are allowed only for different controller Identifiers.
 *					The CAN Driver must be initialized after Power ON.
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :	ID of the CAN controller is published inside the configuration description of the CAN Interface.
 */
Std_ReturnType			CanIf_SetControllerMode(uint8 Controller, CanIf_ControllerModeType ControllerMode);
/*
 * Service name :	CanIf_GetControllerMode
 * Service ID :		0x04
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Controller              : CAN controller requested for current operation mode
 *					CanIf_ControllerModePtr : Pointer to a memory location, where the current mode of the CAN network will be stored.
 * Return value :	E_OK     : Controller mode request has been accepted
 *					E_NOT_OK : Controller mode request has not been accepted
 * Description :	CANIF229 : Service reports about the current status of the requested CAN controller.
 *					Development errors :
 *					Invalid values of Controller will be reported to the development error tracer (CANIF_E_PARAM_CONTROLLER).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		The CAN Driver must be initialized after Power ON.
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :	ID of CAN controller is published inside the configuration description of the CAN Interface Layer.
 */
Std_ReturnType			CanIf_GetControllerMode(uint8 Controller, CanIf_ControllerModeType * ControllerModePtr);
/*
 * Service name :	CanIf_Transmit
 * Service ID :		0x05
 * Sync/Async :		Synchronous
 * Reentrancy :		Re-entrant
 * Parameters :		CanTxPduId : L-PDU handle of CAN L-PDU to be transmitted.
 *					             This handle specifies the corresponding CAN L-PDU ID and implicitly the CAN Driver instance as well as the corresponding CAN controller device.
 *					PduInfoPtr : Pointer to a structure with CAN L-PDU related data : DLC and pointer to CAN L-SDU buffer
 * Return value :	E_OK     : Transmit request has been accepted
 *					E_NOT_OK : Transmit request has not been accepted
 * Description :	CANIF005 : This service initiates a request for transmission of the CAN L-PDU specified by the CanTxPduId and CAN related data in the L-PDU structure. The corresponding CAN controller and HTH have to be resolved by the CanTxPduId.
 *					A transmit request has not been accepted, if the controller mode is not STARTED and/or the channel mode at least for the transmit path is not online or offline active.
 *					One call of this function results in one call of Can_Write(Hth, *PduInfo).
 *					Development errors :
 *					Invalid values of CanTxPduId or PduInforPtr will be reported to the development error tracer (CANIF_E_INVALID_TXPDUID or CANIF_E_PARAM_POINTER).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		During the call of this API the buffer of PduInfoPtr is controlled by the CAN Interface may not be accessed for read/write from another call context. After return of this call the ownership changes to the upper layer.
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :
 */
Std_ReturnType			CanIf_Transmit(PduIdType CanTxPduId, const PduInfoType * PduInfoPtr);
/*
 * Service name :	CanIf_ReadRxPduData
 * Service ID :		0x06
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		CanRxPduId : Receive L-PDU handle of CAN L-PDU.
 *					             This handle specifies the corresponding CAN L-PDU ID and implicitly the CAN Driver instance as well as the corresponding CAN controller device.
 *					PduInfoPtr : Pointer to a structure with CAN L-PDU related data : DLC and pointer to CAN L-SDU buffer
 * Return value :	E_OK     : Request for L-PDU data has been accepted
 *					E_NOT_OK : No valid data has been received.
 * Description :	CANIF194 : This service provides the CAN DLC and the received data of the requested CanRxPduId to the calling upper layer.
 *					A request has not been accepted, if the network mode is not STARTED and/or the channel mode at least for the receive path online or offline active.
 *					Development errors :
 *					Invalid values of CanRxPduId or PduInfoPtr will be reported to the development error tracer (CANIF_E_INVALID_RXPDUID or CANIF_E_PARAM_POINTER).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		During the call of this API the buffer of PduInfoPtr is controlled by the CAN Interface may not be accessed for read/write from another call context. After return of this call the ownership changes to the upper layer.
 *					This API must not be used for CanRxPduIds, which are defined to receive multiple CAN-Ids (range reception).
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :	This API can be enabled or disabled at pre-compile time configuration by the configuration parameter CANIF_READRXPDU_DATA_API.
 */
Std_ReturnType			CanIf_ReadRxPduData(PduIdType CanRxPduId, PduInfoType * PduInfoPtr);
/*
 * Service name :	CanIf_ReadTxNotifStatus
 * Service ID :		0x07
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		CanTxPduId : L-PDU handle of CAN L-PDU to be transmitted.
 *					             This handle specifies the corresponding CAN L-PDU ID and implicitly the CAN Driver instance as well as the corresponding CAN controller device.
 * Return value :	CanIf_NotifStatusType : Current notification status of the corresponding CAN L-PDU.
 * Description :	CANIF202 : This service provides the status of the static or dynamic CAN Tx L-PDU requested by CanTxPduId. This API service notifies the upper layer about any transmit confirmation event to the corresponding requested CAN L-PDU.
 *					During this call the CAN Tx L-PDU notification status is reset inside the CAN Interface.
 *					Development errors :
 *					Invalid values of CanTxPduId will be reported to the development error tracer (CANIF_E_INVALID_TXPDUID). Error cases :
 *					=> CanTxPduId is out of range or
 *					=> no status information was configured for this CAN Tx L-PDU.
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		The CAN Interface must be initialized after Power ON.
 * Configuratinos :	This API can be enabled or disabled at pre-compile time configuration globally by the parameter CANIF_READTXPDU_NOTIFY_STATUS_API.
 */
CanIf_NotifStatusType	CanIf_ReadTxNotifStatus(PduIdType CanTxPduId);
/*
 * Service name :	CanIf_ReadRxNotifStatus
 * Service ID :		0x08
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		CanRxPduId : L-PDU handle of CAN L-PDU to be received.
 *					             This handle specifies the corresponding CAN L-PDU ID and implicitly the CAN Driver instance as well as the corresponding CAN controller device.
 * Return value :	CanIf_NotifStatusType : Current notification status of the corresponding CAN Rx L-PDU.
 * Description :	CANIF230 : This service provides the status of the CAN Rx L-PDU requested by CanRxPduId. This API service notifies the upper layer about any receive indication event to the corresponding requested CAN L-PDU.
 *					During this call the CAN Rx L-PDU notification status is reset inside the CAN Interface.
 *					Development errors :
 *					Invalid values of CanRxPduId will be reported to the development error tracer (CANIF_E_INVALID_RXPDUID). Error cases :
 *					=> CanRxPduId is out of range or
 *					=> Status for CanRxPduId was requested whereas CANIF_READRXPDU_DATA_API is disabled
 *					=> no status information was configured for this CAN Rx L-PDU.
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		This API must not be used for CanRxPduIds, which are defined to receive multiple CAN-Ids (range reception).
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :	This API can be enabled or disabled at pre-compile time configuration globally by time parameter CANIF_READRXPDU_NOTIFY_STATUS_API.
 */
CanIf_NotifStatusType	CanIf_ReadRxNotifStatus(PduIdType CanRxPduId);
/*
 * Service name :	CanIf_SetPduMode
 * Service ID :		0x09
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Controller     : All PDUs of the own ECU connected to the corresponding physical CAN controller are addressed.
 *					PduModeRequest : Requested PDU mode change (see CanIf_ChannelSetModeType)
 * Return value :	E_OK     : Request for mode transition has been accepted
 *					E_NOT_OK : Request for mode transition has not been accepted
 * Description :	CANIF008 : This service sets the requested mode at all L-PDUs of the predefined logical PDU channel. This channel parameter can be derived from Controller.
 *					Development errors :
 *					Invalid values of Controller will be reported to the development error tracer (CANIF_E_PARAM_CONTROLLER).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		Re-entrant calls of this API are allowed only for different channel Identifiers.
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :	The channel mode is configurable by CANIF_CANTXPDUID_CONTROLLER/CANIF_CANRXPDUID_CONTROLLER.
 */
Std_ReturnType			CanIf_SetPduMode(uint8 Controller, CanIf_ChannelSetModeType PduModeRequest);
/*
 * Service name :	CanIf_GetPduMode
 * Service ID :		0x0A
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Controller : All PDUs of the own ECU connected to the corresponding physical CAN controller are addressed.
 *					PduModePtr : Pointer to a memory location, where the current mode of the logical PDU channel will be stored.
 * Return value :	E_OK     : Pdu mode request has been accepted
 *					E_NOT_OK : Pdu request has not been accepted
 * Description :	CANIF009 : This service reports the current mode of the requested Pdu channel
 *					Development errors :
 *					Invalid values of Controller will be reported to the development error tracer (CANIF_E_PARAM_CONTROLLER).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINT). The function returns with E_NOT_OK.
 * Caveats :		The CAN Interface must be initialized after Power ON.
 * Configuratinos :
 */
Std_ReturnType			CanIf_GetPduMode(uint8 Controller, CanIf_PduGetModeType * PduModePtr);
/*
 * Service name :	CanIf_GetVersionInfo
 * Service ID :		0x0B
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		VersionInfo : Pointer to where to store the version information of this module.
 * Return value :
 * Description :	CANIF158 : This service returns the version information of this module. The version information includes :
 *						Module Id
 *						Vendor Id
 *						Vendor specific version numbers (BSW00407).
 *					If source code for caller and callee of this function is available this function should be realized as a macro. The macro should be defined in the modules header file.
 * Caveats :
 * Configuratinos :	This function shall be pre compile time configurable On/Off by the configuration parameter CANIF_VERSION_INFO_API.
 */
void					CanIf_GetVersionInfo(Std_VersionInfoType * VersionInfo);
/*
 * Service name :	CanIf_SetDynamicTxId
 * Service ID :		0x0C
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		CanTxPduId : L-PDU handle of CAN L-PDU for transmission.
 *					             This ID specifies the corresponding CAN L-PDU ID and implicitly the CAN Driver instance as well as the corresponding CAN controller device.
 *					CanId      : Standard/Extended CAN ID of CAN L-PDU that shall be transmitted
 * Return value :
 * Description :	CANIF189 : This service reconfigures the corresponding CAN identifier of the requested CAN L-PDU.
 *					Development errors :
 *						Invalid values of CanTxPduId and CanId well be reported to the development error tracer (CANIF_E_INVALID_TXPDUID or CANIF_E_PARAM_CANID)
 *						If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). No reconfiguration of TxCanId will be executed.
 * Caveats :		The CAN Interface must be initialized after Power ON.
 *					This function may not be interrupted by CanIf_Transmit(), if the same L-PDU ID is handled.
 * Configuratinos :	This function shall be pre compile time configurable On/Off by the configuration parameter CANIF_SETDYNAMICTXID_API.
 */
void					CanIf_SetDynamicTxId(PduIdType CanTxPduId, Can_IdType CanId);
/*
 * Service name :	CanIf_SetTransceiverMode
 * Service ID :		0x0D
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Transceiver     : CAN transceivers requested for mode transition
 *					TransceiverMode : Requested mode transition
 * Return value :	E_OK     : Will be returned, if the transceiver state has been changed to the requested mode.
 *					E_NOT_OK : Will be returned, if the transceiver state change has failed or the parameter is out of the allowed range. The previous state has not been changed.
 * Description :	CANIF287 : This API requests actual state of CAN Transceiver Driver. For more details, please refer to the [9] Specification of CAN Transceiver Driver.
 *					This service calls CanTrcv_SetOpMode(Transceiver, * OpMode) for the corresponding requested CAN transceiver.
 *					Development errors :
 *					Invalid values of transceiver or transceivermode will be reported to the development error tracer (CANIF_TRCV_E_TRANSCEIVER, CANIF_TRCV_E_TRCV_NOT_STANDBY or CANIF_TRCV_E_TRCV_NOT_NORMAL)
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		This API shall be applicable to all CAN transceivers with all values independent, if the transceiver hardware supports these modes or not. This is to ease up the view of the Can Interface to the assigned physical CAN chennel. If the mode is not supported, the return value shall be E_OK.
 * Configuratinos :	The number of supported transceiver types for each network is set up in the configuration phase. If no transceiver is used, this API shall not be provided.
 */
Std_ReturnType			CanIf_SetTransceiverMode(uint8 Transceiver, CanIf_TransceiverModeType TransceiverMode);
/*
 * Service name :	CanIf_GetTransceiverMode
 * Service ID :		0x0E
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Transceiver        : CAN transceivers requested for mode transition
 *					TransceiverModePtr : Requested mode transition
 * Return value :	E_OK     : Transceiver mode request has been accepted
 *					E_NOT_OK : Transceiver mode request has not been accepted
 * Description :	CANIF288 : This API returns actual state of CAN Transceiver Driver. For more details, please refer to the [9] Specification of CAN Transceiver Driver.
 *					This service calls CanTrcv_GetOpMode(Transceiver, * OpMode) for the corresponding requested CAN transceiver.
 *					Development errors :
 *					Invalid values of transceiver will be reported to the development error tracer (CANIF_TRCV_E_TRANSCEIVER)
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error trace (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		See CanIf_Init() for the provided state after the CAN Transceiver Driver initialization till the first operation mode change request.
 * Configuratinos :	The number of supported transceiver types for each network is set up in the configuration phase. If no transceiver if used, this API shall not be provided.
 */
Std_ReturnType			CanIf_GetTransceiverMode(uint8 Transceiver, CanIf_TransceiverModeType * TransceiverModePtr);
/*
 * Service name :	CanIf_GetTrcvWakeupReason
 * Service ID :		0x0F
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Transceiver     : The handle identifies the CAN transceiver to which the API call has to be applied.
 *					TrcvWuReasonPtr : Requested transceiver wakeup reason
 * Return value :	E_OK     : Transceiver mode request has been accepted
 *					E_NOT_OK : Transceiver mode request has not been accepted
 * Description :	CANIF289 : This API returns the reason for the wake up that the CAN transceiver has detected. The ability to detect and differentiate the possible wakeup reasons depends strongly on the CAN transceiver hardware. For more details, please refer to the [9] Specification of CAN Transceiver Driver.
 *					This service calls CanTrcv_GetBusWuReason(Transceiver, Reason) for the corresponding requested CAN transceiver.
 *					Development errors :
 *						CANIF_TRCV_E_TRCV_NOT_STANDBY
 *						If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		Please be aware, that if more than one network is available, each network may report a different wake upreason. E.g. if an ECU has CAN, a wake up by CAN may occur and the incoming data may cause an internal wake up for another CAN network.
 *						This API has a "per network" view and does not vote the more important reason of sequence internally. The same may be true if e.g. one transceiver controls the power supply and the other is just powered or un-powered. Then one may be able to return CANIF_TRCV_WU_POWER_ON, whereas the other may state e.g. CANIF_TRCV_WU_RESET.
 *					It is up to the EcuM to decide, how to handle that wake up information.
 * Configuratinos :	The number of supported transceiver types for each network is set up in the configuration phase. If no transceiver is used, this API shal not be provided.
 */
Std_ReturnType			CanIf_GetTrcvWakeupReason(uint8 Transceiver, CanIf_TrcvWakeupReasonType * TrcvWuReasonPtr);
/*
 * Service name :	CanIf_SetTransceiverWakeupMode
 * Service ID :		0x10
 * Sync/Async :		Synchronous
 * Reentrancy :		Non re-entrant
 * Parameters :		Transceiver    : The handle identifies the CAN transceiver to which the API call has to be applied.
 *					TrcvWakeupMode : Requested transceiver wakeup reason
 * Return value :	E_OK     : Will be returned, if the wakeup state has been changed to the requested mode.
 *					E_NOT_OK : Will be returned, if the wakeup state change has failed or the parameter is out of the allowed range. The previous state has not been changed.
 * Description :	CANIF290 : This API enables, disables and clears the notification for wakeup events on the addressed network. For more details, please refer to the [9] Specification of CAN Transceiver Driver.
 *					This service calls CanTrcv_SetWakeupMode(Transceiver, TrcvWakeupMode) for the corresponding requested CAN transceiver.
 *					Enabled : if the CAN Transceiver Driver has a stored wakeup event pending for the addressed network, the notification is executed within the API call or immediately after (depending on the implementation).
 *					Disabled : li it is required by the transceiver device and the underlying communication, the driver has to detect the wakeup events nevertheless and stores it internally to raise the event, when the wakeup notification is enabled again.
 *					Clear : Clearing of wakeup events have to be used, when the wake up notification is disabled to clear all stored wake up events under control of the higher layer.
 *					Development errors :
 *					CANTRCV_E_UNINIT : not yet initialized
 * Caveats :		The implementation may be e.g. disabling the interrupt source for the wake up. If the interrupt is level triggered a pending interrupt is automatically stored and raised after enabling the notification again.
 *					If is very important not to lose wake up events during the disabled period.
 * Configuratinos :	The number of supported transceiver types for each network is set up in the configuration phase. If no transceiver is used, this API shall not be provided.
 */
Std_ReturnType			CanIf_SetTransceiverWakeupMode(uint8 Transceiver, CanIf_TrcvWakeupModeType TrcvWakeupMode);

/*
 * Service name :	CanIf_CancelTxConfirmation
 * Service ID :		0x15
 * Sync/Async :		Synchronous
 * Reentrancy :		Re-entrant
 * Parameters :		PduInfoPtr : Pointer to a structure with CAN L-PDU related data : L-PDU handle of the successfully aborted CAN L-PDU, CAN identifier, DLC and pointer to CAN L-SDU buffer.
 * Return value :
 * Description :	CANIF101 : This service is implemented in the CAN Interface and called by the CAN Driver after a previous request for cancellation of a pending L-PDU transmit request was successfully performed.
 *					This callback service is implemented as many times as underlying CAN Drivers are used. In that case one cancel transmit confirmation callback is assigned to one underlying CAN Driver.
 *					Then following naming convention has to be considered : CanIf_CancelTxConfirmation_<CAN_Driver>.
 *					For further details please refer to chapter [7.25 Multiple CAN Driver support].
 *					Development errors :
 *					Invalid values of CanTxPduId will be reported to the development error tracer (CANIF_E_PARAM_LPDU).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). No Tx cancellation handling will be executed.
 * Caveats :		The call context is either on interrupt level (interrupt mode) or on task level (polling mode).
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :	This function shall be pre compile time configurable On/Off by the configuration parameter CANIF_TRANSMIT_CANCELLATION.
 */
void				CanIf_CancelTxConfirmation(const Can_PduType * PduInfoPtr);


/*
 * Service name :   CanIf_Deinit
 */
void                CanIf_Deinit();
void                CanIf_MainRoutine();

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifdef CANIF_H */

