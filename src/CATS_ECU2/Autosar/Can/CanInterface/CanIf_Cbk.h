/* CanIf_Cbk.h


*/

#ifndef CANIF_CBK_H
#define CANIF_CBK_H

#include "CanIf_Types.h"

/*
 * Service name :	CanIf_RxIndication
 * Service ID :		0x14
 * Sync/Async :		Synchronous
 * Reentrancy :		Re-entrant
 * Parameters :		Hrh       : ID of the corresponding hardware object
 *					            Range : 0..(total number of Hardware Receive Handles - 1)
 *					CanId     : Standard/Extended CAN ID of CAN L-PDU that has been successfully received
 *					CanDlc    : Data length code (length of CAN L-PDU payload)
 *					CanSduPtr : Pointer to received L-SDU (payload)
 * Return value :
 * Description :	CANIF006 : This service is implemented in the CAN Interface and called by the CAN Driver after a CAN L-PDU has been received. Within this service, the CAN Interface translates the CanId into the configured target PDU ID and routes this indication to the configured upper layer target service(s).
 *					This call-out service is implemented as many times as underlying CAN Drivers are used. In that case one receive indication call-out is assigned to one underlying CAN Driver.
 *					Then following naming convention has to be considered : CanIf_RxIndication_<CAN_Driver>.
 *					For further details please refer to chapter [7.25 Multiple CAN Driver support].
 *					Development errors :
 *					Invalid values of Hrh, CanId, CanDlc or *CanSduPtr will be reported to the development error tracer (CANIF_E_PARAM_HRH, CANIF_E_PARAM_CANID, CANIF_E_PARAM_DLC or CANIF_E_PARAM_POINTER).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). No Rx indication handling will be executed.
 * Caveats :		The call context is either on interrupt level (interrupt mode) or on task level (polling mode).
 *					This call-out service is re-entrant for multiple CAN controller usage.
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :	CAN L-PDUs have to be assigned to the corresponding receive indication service.
 */
void					CanIf_RxIndication(uint8 Hrh, Can_IdType CanId, uint8 CanDlc, const uint8 * CanSduPtr);

/*
 * Service name :	CanIf_ControllerBusOff
 * Service ID :		0x16
 * Sync/Async :		Synchronous
 * Reentrancy :		Re-entrant
 * Parameters :		Controller : CAN controller, where a BusOff occurred.
 * Return value :
 * Description :	CANIF218 : This service indicates a CAN controller BusOff event referring to the corresponding CAN controller. (CANIF014)
 *					This call-out service is called by the CAN Driver and implemented in the CAN Interface. It is called in case of a mode change notification of the CAN Driver.
 *					This call-out service is implemented as many times as underlying CAN Drivers are used. In that case one BusOff notification is assigned to one underlying CAN Driver.
 *					Then following naming convention has to be considered : CanIf_ControllerBusOff_<CAN_Driver>.
 *					For further details please refer to chapter [7.25 Multiple CAN Driver support].
 *					Development errors :
 *					Invalid values of controller will be reported to the development error tracer (CANIF_E_PARAM_CONTROLLER).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). No BusOff notification will be executed.
 * Caveats :		The call context is either on interrupt level (interrupt mode) or on task level (polling mode).
 *					The CAN Interface must be initialized after Power ON.
 *					This call-out service is re-entrant for multiple CAN controller usage.
 * Configuratinos :	ID of the CAN controller is published inside the configuration description of the CAN Interface.
 */
void					CanIf_ControllerBusOff(uint8 Controller);

/*
 * Service name :	CanIf_CheckWakeup
 * Service ID :		0x11
 * Sync/Async :		Synchronous
 * Reentrancy :		Re-entrant
 * Parameters :		WakeupSource : Source device, who initiated the wakeup event : CAN controller or CAN transceiver
 * Return value :	E_OK     : Will be returned, if the check wakeup request has been accepted.
 *					E_NOT_OK : Will be returned, if the check wakeup request has not been accepted.
 * Description :	CANIF219 : This Service checks, whether an underlying CAN driver or CAN Transceiver driver already signals an wakeup event by the CAN network
 *					This service shall evaluate the WakeupSource parameter to get the information, which dedicate wakeup source needs to be checked, either a CAN transceiver or controller device. Depending on this information the function CanIf_CheckWakeup shall either call the function Can_Cbk_CheckWakeup() or CanTrcv_CB_WakeupByBus() with the parameter addressing the correct hardware device causing the wakeup event.
 *					If one of these called functions has detected a wakeup by CAN (return value E_OK), the service CanIf_CheckWakeup() shall call the API EcuM_SetWakeupEvent() for the respective Wakeup Source.
 *					This service is called by the ECU Firmware. In dependence of the parameter value the CAN Interface notifies the CAN Driver of the CAN Transceiver Driver about the wakeup event. This service is implemented by the CAN Interface. It is called in case of a mode change notification of the CAN controller or the CAN transceiver.
 *					Development errors :
 *					Invalid values of WakeupSource will be reported to the development error tracer (CANIF_E_PARAM_WAKEUPSOURCE).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		The call context is either on interrupt level (interrupt mode) or on task level (polling mode).
 *					The CAN Interface must be initialized after Power ON.
 *					This call-out service is re-entrant for multiple CAN controller usage.
 * Configuratinos :	This wake up service is configurable by CANIF_WAKEUP_SUPPORT, which depends on the used CAN controller type and the used wakeup strategy. This callback may not be supported, if no wakeup shal be used.
 */
Std_ReturnType			CanIf_CheckWakeup(EcuM_WakeupSourceType WakeupSource);

/*
 * Service name :	CanIf_CheckValidation
 * Service ID :		0x12
 * Sync/Async :		Synchronous
 * Reentrancy :		Re-entrant
 * Parameters :		WakeupSource : Source device, who initiated the wakeup event has to be validated: CAN controller or CAN transceiver
 * Return value :	E_OK     : Will be returned, if the check validation request has been accepted.
 *					E_NOT_OK : Will be returned, if the check validation request has not been accepted.
 * Description :	CANIF178 : This service is performed to validate a previous wakeup event. This service is called by the ECU Firmware. The CAN Interface checks inside this service, whether a L-PDU was successful received inn the meantime.
 *					CANIF179 : The validation call return, whether the first CAN L-PDU reception event after a wakeup event has been occurred on the corresponding CAN network. In that case EcuM_ValidateWakeupEvent() is called within the validation result.
 *					For different upper layer users different service names shall be used. This type of indication call-out service is mainly designed for the ECU State Manager module.
 *					Development errors :
 *					Invalid values of WakeupSource will be reported to the development error tracer (CANIF_E_PARAM_WAKEUPSOURDE).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). The function returns with E_NOT_OK.
 * Caveats :		The CAN Driver must be initialized after Power ON.
 *					The call context is either on interrupt level (interrupt mode) or on task level (polling mode).
 *					This call-out service is re-entrant for multiple CAN controller/CAN network usage.
 * Configuratinos :	The responsible layers for the copying of the data are statically configurable. If no validation is needed, this API can be omitted by disable of CANIF_WAKEUP_VALIDATION.
 *					The wakeup validation API name for validated wakeup events belonging to the EcuM module must be configured to EcuM_ValidateWakeupEvent().
 */
Std_ReturnType			CanIf_CheckValidation(EcuM_WakeupSourceType WakeupSource);

/*
 * Service name :	CanIf_TxConfirmation
 * Service ID :		0x13
 * Sync/Async :		Synchronous
 * Reentrancy :		Re-entrant
 * Parameters :		CanTxPduId : L-PDU handle of CAN L-PDU successfully transmitted. This ID specifies the corresponding CAN L-PDU ID and implicitly the CAN Driver instance as well as the corresponding CAN controller device.
 * Return value :
 * Description :	CANIF007 : This service is implemented in the CAN Interface and called by the CAN Driver after the CAN L-PDU has been transmitted on the CAN network. Within this service, the CAN Driver passes back the CanTxPduId to the CAN Interface, which it got from Can_Write(Hrh, * PduInfo).
 *					This call-out service is implemented as many times as underlying CAN Drivers are used. In that case one transmit confirmation call-out is assigned to one underlying CAN Driver.
 *					Then following naming convention has to be considered : CanIf_TxConfirmation_<CAN_Driver>.
 *					For further details please refer to chapter [7.25 Multiple CAN Driver support].
 *					Development errors :
 *					Invalid values of CanTxPduId will be reported to the development error tracer (CANIF_E_PARAM_LPDU).
 *					If the CAN Interface was not initialized before, the call of this function will be reported to the development error tracer (CANIF_E_UNINIT). No Tx confirmation handling will be executed.
 * Caveats :		The call context is either on interrupt level (interrupt mode) or on task level (polling mode).
 *					This call-out service is re-entrant for multiple CAN controller usage.
 *					The CAN Interface must be initialized after Power ON.
 * Configuratinos :	Transmit confirmation can be enabled or disabled by configuration. It is always enabled, if transmit buffers are used.
 */
void					CanIf_TxConfirmation(PduIdType CanTxPduId);

#endif /* #ifndef CANIF_CBK_H */
