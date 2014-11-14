/** @file
 *
 * @defgroup ble_kst_srv_spi_header ble_spi.h
 * @{
 * @ingroup  ble_kst_srv_spi
 * @brief SPI Service Header
 *
 * @details Includes all of the externally accessible values and functions for the SPI Service.
 */
 
#ifndef BLE_SPI_H__
#define BLE_SPI_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "global_config.h"
#include "spi_master.h"

/**@brief Connectionles Configuration Service Event Type. */
typedef enum
{
	BLE_SPI_COPY_EVT_NOTIFICATION_ENABLED,
	BLE_SPI_COPY_EVT_NOTIFICATION_DISABLED
} ble_spi_evt_type_t;

/**@brief SPI Service event. */
typedef struct
{
	ble_spi_evt_type_t evt_type;
} ble_spi_evt_t;

//Forward declaration of the ble_spi_t type.
typedef struct ble_spi_s ble_spi_t;

/**@brief Connectionles Configuration Service event handler type. */
typedef void (*ble_spi_evt_handler_t) (ble_spi_t * p_spi, ble_spi_evt_t * p_evt);

/**@brief SPI Service init structure.  This contains all options and data needed
  * for initialization.  */
typedef struct
{
	  ble_spi_evt_handler_t         	evt_handler;
    bool 	 	 	 	 	 	 	 	 	 	 	 	 		support_notification;
	  uint8_t 	 	 	 	 	 	 	 	 	 	 		initial_transmit_state[20];
		uint8_t										 			initial_receive_state[20];
}ble_spi_init_t;

/**@brief SPI Service structure. This contains various status information for the service. */
typedef struct ble_spi_s
{
	ble_spi_evt_handler_t     			evt_handler;
	uint16_t 	 	 	 	          			service_handle;
	ble_gatts_char_handles_t  			transmit_handles;
	ble_gatts_char_handles_t 				receive_handles;
	ble_gatts_char_handles_t				config_handles;
	uint16_t 	 	 	 	          			conn_handle;	
	uint8_t 	 	 	 	 	 	 						transmit_packet[20];
	uint8_t													receive_packet[20];
	uint8_t													config_packet[1];
	bool 	 	 	 	 	 	 	  						is_notification_supported;
}ble_spi_t;

extern uint32_t *spi_base_address;
extern SPIMode spi_mode;
extern bool lsb_first;

extern ble_spi_t m_spi;

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the SPI Service.
 *
 * @param   p_spi      Battery Service structure.
 * @param   p_ble_evt  Event received from the BLE stack.
 */
void ble_spi_on_ble_evt(ble_spi_t * p_spi, ble_evt_t * p_ble_evt);

/**@brief Function for initializing the SPI Service.
 *
 * @param  p_spi       SPI Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param   p_spi_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_spi_init(ble_spi_t * p_spi, const ble_spi_init_t * p_spi_init);

/**@brief Function for updating the passcode value.
 *
 * @details The application calls this function when the user attempts to input a passcode.
 *
 * @param   p_spi          SPI Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_spi_transmit_send(ble_spi_t * p_spi, uint8_t data[], uint16_t length);

/**@brief Function for updating the response value.
 *
 * @details The application calls this function when an input has been receieved to confirm the input to the user.
 *
 * @param   p_spi          SPI Service structure.
 * @param   data[]  	   The new data input.
 * @param   length		   The integer length of the new data input.
 * 
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_spi_receive_send(ble_spi_t * p_spi, uint8_t data[], uint16_t length);

uint32_t ble_spi_config_send(ble_spi_t * p_spi, uint8_t data[], uint16_t length);

#endif

/** @} */
