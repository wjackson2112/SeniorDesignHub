/** @file
 *
 * @defgroup ble_kst_srv_uart_header ble_uart.h
 * @{
 * @ingroup  ble_kst_srv_uart
 * @brief UART Service Header
 *
 * @details Includes all of the externally accessible values and functions for the UART Service.
 */
 
#ifndef BLE_UART_H__
#define BLE_UART_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "global_config.h"

/**@brief Connectionles Configuration Service Event Type. */
typedef enum
{
	BLE_UART_COPY_EVT_NOTIFICATION_ENABLED,
	BLE_UART_COPY_EVT_NOTIFICATION_DISABLED
} ble_uart_evt_type_t;

/**@brief UART Service event. */
typedef struct
{
	ble_uart_evt_type_t evt_type;
} ble_uart_evt_t;

//Forward declaration of the ble_uart_t type.
typedef struct ble_uart_s ble_uart_t;

/**@brief Connectionles Configuration Service event handler type. */
typedef void (*ble_uart_evt_handler_t) (ble_uart_t * p_uart, ble_uart_evt_t * p_evt);

/**@brief UART Service init structure.  This contains all options and data needed
  * for initialization.  */
typedef struct
{
	  ble_uart_evt_handler_t         	evt_handler;
    bool 	 	 	 	 	 	 	 	 	 	 	 	 		support_notification;
	  uint8_t 	 	 	 	 	 	 	 	 	 	 		initial_transmit_state[20];
		uint8_t										 			initial_receive_state[20];
}ble_uart_init_t;

/**@brief UART Service structure. This contains various status information for the service. */
typedef struct ble_uart_s
{
	ble_uart_evt_handler_t     			evt_handler;
	uint16_t 	 	 	 	          			service_handle;
	ble_gatts_char_handles_t  			transmit_handles;
	ble_gatts_char_handles_t 				receive_handles;
	ble_gatts_char_handles_t				config_handles;
	uint16_t 	 	 	 	          			conn_handle;	
	uint8_t 	 	 	 	 	 	 						transmit_packet[20];
	uint8_t													receive_packet[20];
	uint8_t													config_packet[1];
	bool 	 	 	 	 	 	 	  						is_notification_supported;
}ble_uart_t;

extern bool uart_parity_included;
extern bool uart_hw_flow_control;
extern uint8_t uart_baud_rate;

extern ble_uart_t m_uart;

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the UART Service.
 *
 * @param   p_uart      Battery Service structure.
 * @param   p_ble_evt  Event received from the BLE stack.
 */
void ble_uart_on_ble_evt(ble_uart_t * p_uart, ble_evt_t * p_ble_evt);

/**@brief Function for initializing the UART Service.
 *
 * @param  p_uart       UART Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param   p_uart_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_uart_init(ble_uart_t * p_uart, const ble_uart_init_t * p_uart_init);

/**@brief Function for updating the passcode value.
 *
 * @details The application calls this function when the user attempts to input a passcode.
 *
 * @param   p_uart          UART Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_uart_transmit_send(ble_uart_t * p_uart, uint8_t data[], uint16_t length);

/**@brief Function for updating the response value.
 *
 * @details The application calls this function when an input has been receieved to confirm the input to the user.
 *
 * @param   p_uart          UART Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_uart_receive_send(ble_uart_t * p_uart, uint8_t data[], uint16_t length);

uint32_t ble_uart_config_send(ble_uart_t * p_uart, uint8_t data[], uint16_t length);

#endif

/** @} */
