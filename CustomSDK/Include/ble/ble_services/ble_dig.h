/** @file
 *
 * @defgroup ble_kst_srv_dig_header ble_dig.h
 * @{
 * @ingroup  ble_kst_srv_dig
 * @brief DIG Service Header
 *
 * @details Includes all of the externally accessible values and functions for the DIG Service.
 */
 
#ifndef BLE_DIG_H__
#define BLE_DIG_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "global_config.h"

/**@brief Connectionles Configuration Service Event Type. */
typedef enum
{
	BLE_DIG_COPY_EVT_NOTIFICATION_ENABLED,
	BLE_DIG_COPY_EVT_NOTIFICATION_DISABLED
} ble_dig_evt_type_t;

/**@brief DIG Service event. */
typedef struct
{
	ble_dig_evt_type_t evt_type;
} ble_dig_evt_t;

//Forward declaration of the ble_dig_t type.
typedef struct ble_dig_s ble_dig_t;

/**@brief Connectionles Configuration Service event handler type. */
typedef void (*ble_dig_evt_handler_t) (ble_dig_t * p_dig, ble_dig_evt_t * p_evt);

/**@brief DIG Service init structure.  This contains all options and data needed
  * for initialization.  */
typedef struct
{
	  ble_dig_evt_handler_t         	evt_handler;
    bool 	 	 	 	 	 	 	 	 	 	 	 	 		support_notification;
	  uint8_t 	 	 	 	 	 	 	 	 	 	 		initial_transmit_state[1];
		uint8_t										 			initial_receive_state[1];
}ble_dig_init_t;

/**@brief DIG Service structure. This contains various status information for the service. */
typedef struct ble_dig_s
{
	ble_dig_evt_handler_t     			evt_handler;
	uint16_t 	 	 	 	          			service_handle;
	ble_gatts_char_handles_t  			transmit_handles;
	ble_gatts_char_handles_t 				receive_handles;
	ble_gatts_char_handles_t				config_handles;
	uint16_t 	 	 	 	          			conn_handle;	
	uint8_t 	 	 	 	 	 	 						transmit_packet[1];
	uint8_t													receive_packet[1];
	uint8_t													config_packet[1];
	bool 	 	 	 	 	 	 	  						is_notification_supported;
}ble_dig_t;

extern ble_dig_t m_dig;

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the DIG Service.
 *
 * @param   p_dig      Battery Service structure.
 * @param   p_ble_evt  Event received from the BLE stack.
 */
void ble_dig_on_ble_evt(ble_dig_t * p_dig, ble_evt_t * p_ble_evt);

/**@brief Function for initializing the DIG Service.
 *
 * @param  p_dig       DIG Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param   p_dig_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_dig_init(ble_dig_t * p_dig, const ble_dig_init_t * p_dig_init);

/**@brief Function for updating the passcode value.
 *
 * @details The application calls this function when the user attempts to input a passcode.
 *
 * @param   p_dig          DIG Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_dig_transmit_send(ble_dig_t * p_dig, uint8_t data[], uint16_t length);

void ble_dig_receive(uint8_t *data, uint16_t length);

/**@brief Function for updating the response value.
 *
 * @details The application calls this function when an input has been receieved to confirm the input to the user.
 *
 * @param   p_dig          DIG Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_dig_receive_send(ble_dig_t * p_dig, uint8_t data[], uint16_t length);

uint32_t ble_dig_config_send(ble_dig_t * p_dig, uint8_t data[], uint16_t length);

#endif

/** @} */
