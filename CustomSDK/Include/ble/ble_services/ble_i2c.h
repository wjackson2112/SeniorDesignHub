/** @file
 *
 * @defgroup ble_kst_srv_i2c_header ble_i2c.h
 * @{
 * @ingroup  ble_kst_srv_i2c
 * @brief I2C Service Header
 *
 * @details Includes all of the externally accessible values and functions for the I2C Service.
 */
 
#ifndef BLE_I2C_H__
#define BLE_I2C_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "global_config.h"

/**@brief Connectionles Configuration Service Event Type. */
typedef enum
{
	BLE_I2C_COPY_EVT_NOTIFICATION_ENABLED,
	BLE_I2C_COPY_EVT_NOTIFICATION_DISABLED
} ble_i2c_evt_type_t;

/**@brief I2C Service event. */
typedef struct
{
	ble_i2c_evt_type_t evt_type;
} ble_i2c_evt_t;

//Forward declaration of the ble_i2c_t type.
typedef struct ble_i2c_s ble_i2c_t;

/**@brief Connectionles Configuration Service event handler type. */
typedef void (*ble_i2c_evt_handler_t) (ble_i2c_t * p_i2c, ble_i2c_evt_t * p_evt);

/**@brief I2C Service init structure.  This contains all options and data needed
  * for initialization.  */
typedef struct
{
	  ble_i2c_evt_handler_t         	evt_handler;
    bool 	 	 	 	 	 	 	 	 	 	 	 	 		support_notification;
	  uint8_t 	 	 	 	 	 	 	 	 	 	 		initial_transmit_state[20];
		uint8_t										 			initial_receive_state[20];
}ble_i2c_init_t;

/**@brief I2C Service structure. This contains various status information for the service. */
typedef struct ble_i2c_s
{
	ble_i2c_evt_handler_t     			evt_handler;
	uint16_t 	 	 	 	          			service_handle;
	ble_gatts_char_handles_t  			transmit_handles;
	ble_gatts_char_handles_t 				receive_handles;
	ble_gatts_char_handles_t				config_handles;
	uint16_t 	 	 	 	          			conn_handle;	
	uint8_t 	 	 	 	 	 	 						transmit_packet[20];
	uint8_t													receive_packet[20];
	uint8_t													config_packet[1];
	bool 	 	 	 	 	 	 	  						is_notification_supported;
}ble_i2c_t;

extern bool send_stop_bit;
extern bool use_operation_chaining;
extern bool use_transaction_id;

extern ble_i2c_t m_i2c;

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the I2C Service.
 *
 * @param   p_i2c      Battery Service structure.
 * @param   p_ble_evt  Event received from the BLE stack.
 */
void ble_i2c_on_ble_evt(ble_i2c_t * p_i2c, ble_evt_t * p_ble_evt);

/**@brief Function for initializing the I2C Service.
 *
 * @param  p_i2c       I2C Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param   p_i2c_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_i2c_init(ble_i2c_t * p_i2c, const ble_i2c_init_t * p_i2c_init);

/**@brief Function for updating the passcode value.
 *
 * @details The application calls this function when the user attempts to input a passcode.
 *
 * @param   p_i2c          I2C Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_i2c_transmit_send(ble_i2c_t * p_i2c, uint8_t data[], uint16_t length);

/**@brief Function for updating the response value.
 *
 * @details The application calls this function when an input has been receieved to confirm the input to the user.
 *
 * @param   p_i2c          I2C Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_i2c_receive_send(ble_i2c_t * p_i2c, uint8_t data[], uint16_t length);

uint32_t ble_i2c_config_send(ble_i2c_t * p_i2c, uint8_t data[], uint16_t length);

#endif

/** @} */
