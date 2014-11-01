/** @file
 *
 * @defgroup ble_kst_srv_conf_header ble_conf.h
 * @{
 * @ingroup  ble_kst_srv_conf
 * @brief CONF Service Header
 *
 * @details Includes all of the externally accessible values and functions for the CONF Service.
 */
 
#ifndef BLE_CONF_H__
#define BLE_CONF_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "global_config.h"

extern uint8_t passcode[20];
extern bool passcode_valid;
extern uint8_t device_name[20];

/**@brief Connectionles Configuration Service Event Type. */
typedef enum
{
	BLE_CONF_COPY_EVT_NOTIFICATION_ENABLED,
	BLE_CONF_COPY_EVT_NOTIFICATION_DISABLED
} ble_conf_evt_type_t;

/**@brief CONF Service event. */
typedef struct
{
	ble_conf_evt_type_t evt_type;
} ble_conf_evt_t;

//Forward declaration of the ble_conf_t type.
typedef struct ble_conf_s ble_conf_t;

/**@brief Connectionles Configuration Service event handler type. */
typedef void (*ble_conf_evt_handler_t) (ble_conf_t * p_conf, ble_conf_evt_t * p_evt);

/**@brief CONF Service init structure.  This contains all options and data needed
  * for initialization.  */
typedef struct
{
	  ble_conf_evt_handler_t         	evt_handler;
    bool 	 	 	 	 	 	 	 	 	 	 	 	 		support_notification;
		uint8_t										 			initial_device_name_state[20];
}ble_conf_init_t;

/**@brief CONF Service structure. This contains various status information for the service. */
typedef struct ble_conf_s
{
	ble_conf_evt_handler_t     			evt_handler;
	uint16_t 	 	 	 	          			service_handle;
	ble_gatts_char_handles_t  			enter_passcode_handles;
	ble_gatts_char_handles_t 				set_passcode_handles;
	ble_gatts_char_handles_t				device_name_handles;
	uint16_t 	 	 	 	          			conn_handle;	
	uint8_t 	 	 	 	 	 	 						enter_passcode_packet[20];
	uint8_t													set_passcode_packet[20];
	uint8_t													device_name_packet[20];
	bool 	 	 	 	 	 	 	  						is_notification_supported;
}ble_conf_t;

extern ble_conf_t m_conf;

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the CONF Service.
 *
 * @param   p_conf      Battery Service structure.
 * @param   p_ble_evt  Event received from the BLE stack.
 */
void ble_conf_on_ble_evt(ble_conf_t * p_conf, ble_evt_t * p_ble_evt);

/**@brief Function for initializing the CONF Service.
 *
 * @param  p_conf       CONF Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param   p_conf_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_conf_init(ble_conf_t * p_conf, const ble_conf_init_t * p_conf_init);

/**@brief Function for updating the passcode value.
 *
 * @details The application calls this function when the user attempts to input a passcode.
 *
 * @param   p_conf          CONF Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_conf_enter_passcode_send(ble_conf_t * p_conf, uint8_t data[], uint16_t length);

/**@brief Function for updating the response value.
 *
 * @details The application calls this function when an input has been receieved to confirm the input to the user.
 *
 * @param   p_conf          CONF Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_conf_set_passcode_send(ble_conf_t * p_conf, uint8_t data[], uint16_t length);

uint32_t ble_conf_device_name_send(ble_conf_t * p_conf, uint8_t data[], uint16_t length);

#endif

/** @} */
