/** @file
 *
 * @defgroup ble_kst_srv_ana_header ble_ana.h
 * @{
 * @ingroup  ble_kst_srv_ana
 * @brief ANA Service Header
 *
 * @details Includes all of the externally accessible values and functions for the ANA Service.
 */
 
#ifndef BLE_ANA_H__
#define BLE_ANA_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "global_config.h"

/**@brief Connectionles Configuration Service Event Type. */
typedef enum
{
	BLE_ANA_COPY_EVT_NOTIFICATION_ENABLED,
	BLE_ANA_COPY_EVT_NOTIFICATION_DISABLED
} ble_ana_evt_type_t;

/**@brief ANA Service event. */
typedef struct
{
	ble_ana_evt_type_t evt_type;
} ble_ana_evt_t;

//Forward declaration of the ble_ana_t type.
typedef struct ble_ana_s ble_ana_t;

/**@brief Connectionles Configuration Service event handler type. */
typedef void (*ble_ana_evt_handler_t) (ble_ana_t * p_ana, ble_ana_evt_t * p_evt);

/**@brief ANA Service init structure.  This contains all options and data needed
  * for initialization.  */
typedef struct
{
	  ble_ana_evt_handler_t         	evt_handler;
    bool 	 	 	 	 	 	 	 	 	 	 	 	 		support_notification;
	  uint8_t 	 	 	 	 	 	 	 	 	 	 		initial_transmit_state[1];
		uint8_t										 			initial_receive_state[1];
}ble_ana_init_t;

/**@brief ANA Service structure. This contains various status information for the service. */
typedef struct ble_ana_s
{
	ble_ana_evt_handler_t     			evt_handler;
	uint16_t 	 	 	 	          			service_handle;
	ble_gatts_char_handles_t  			transmit_handles;
	ble_gatts_char_handles_t 				receive_handles;
	ble_gatts_char_handles_t				config_handles;
	uint16_t 	 	 	 	          			conn_handle;	
	uint8_t 	 	 	 	 	 	 						transmit_packet[1];
	uint8_t													receive_packet[1];
	uint8_t													config_packet[1];
	bool 	 	 	 	 	 	 	  						is_notification_supported;
}ble_ana_t;

extern ble_ana_t m_ana;

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the ANA Service.
 *
 * @param   p_ana      Battery Service structure.
 * @param   p_ble_evt  Event received from the BLE stack.
 */
void ble_ana_on_ble_evt(ble_ana_t * p_ana, ble_evt_t * p_ble_evt);

/**@brief Function for initializing the ANA Service.
 *
 * @param  p_ana       ANA Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param   p_ana_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_ana_init(ble_ana_t * p_ana, const ble_ana_init_t * p_ana_init);

/**@brief Function for updating the passcode value.
 *
 * @details The application calls this function when the user attempts to input a passcode.
 *
 * @param   p_ana          ANA Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_ana_transmit_send(ble_ana_t * p_ana, uint8_t data[], uint16_t length);

/**@brief Function for updating the response value.
 *
 * @details The application calls this function when an input has been receieved to confirm the input to the user.
 *
 * @param   p_ana          ANA Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_ana_receive_send(ble_ana_t * p_ana, uint8_t data[], uint16_t length);

uint32_t ble_ana_config_send(ble_ana_t * p_ana, uint8_t data[], uint16_t length);

#endif

/** @} */
