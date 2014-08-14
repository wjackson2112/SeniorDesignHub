
#include "ble_uart.h"

#include <stdlib.h>
#include <string.h>
#include "app_error.h"
#include "ble_gatts.h"
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "nrf_gpio.h"

#define BLE_UUID_UART_SERVICE	 	 	 	 	    0x3740 
#define BLE_UUID_UART_RECEIVE_CHAR         0x3741  
#define BLE_UUID_UART_TRANSMIT_CHAR					0x3742  
#define UART_UUID_INDEX              				0

/**@brief 128-bit UART UUID base List. */
static const ble_uuid128_t m_base_uuid128 =
{
	 {
			 0x66, 0x9A, 0x0C, 0x20, 0x00, 0x08, 0x21, 0x8C,
			 0xE4, 0x11, 0xC8, 0x1D, 0x00, 0x00, 0x1E, 0x7B
	 }
};

/**@brief Function for handling the Connect event.
 *
 * @param   p_uart       UART Service structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_uart_t * p_uart, ble_evt_t * p_ble_evt)
{
	p_uart->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param   p_uart       UART Service structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_uart_t * p_uart, ble_evt_t * p_ble_evt)
{
	UNUSED_PARAMETER(p_ble_evt);
	p_uart->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the Write event.
 *
 * @param   p_uart       UART structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_uart_t * p_uart, ble_evt_t * p_ble_evt)
{
	
		ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	  const uint16_t char_handler = p_evt_write->handle;
	
		static uint16_t len_transmit_packet = 20;
		static uint16_t len_receive_packet = 20;

		if(p_evt_write->handle == p_uart->transmit_handles.value_handle)
		{
			sd_ble_gatts_value_get(char_handler, 0, &len_transmit_packet, p_uart->transmit_packet);
		}
		
		if(p_evt_write->handle == p_uart->receive_handles.value_handle)
		{
			sd_ble_gatts_value_get(char_handler, 0, &len_receive_packet, p_uart->receive_packet);
		}
		
}

void ble_uart_on_ble_evt(ble_uart_t * p_uart, ble_evt_t * p_ble_evt)
{
	switch(p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			on_connect(p_uart, p_ble_evt);
			break;
			
		case BLE_GAP_EVT_DISCONNECTED:
			on_disconnect(p_uart, p_ble_evt);
		    break;
					
		case BLE_GATTS_EVT_WRITE:
			on_write(p_uart, p_ble_evt);
			break;
			
		default:	
			break;
	}
}	

/**@brief Function for adding the transmit characteristic.
 *
 * @param   p_uart        UART Service structure.
 * @param   p_uart_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t uart_transmit_char_add(ble_uart_t * p_uart, const ble_uart_init_t * p_uart_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t	 	 	 	initial_trans_state[20];

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	
    memset(&char_md, 0, sizeof(char_md));

	char_md.char_props.write  = 1;
    char_md.char_props.read   = 0;
    char_md.char_props.notify = 0;
    char_md.p_char_user_desc       = NULL;
    char_md.p_char_pf              = NULL;
    char_md.p_user_desc_md         = NULL;
    char_md.p_cccd_md              = &cccd_md;
    char_md.p_sccd_md 	 	 	       = NULL;

	BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_UART_TRANSMIT_CHAR);
	
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
	  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
		
		initial_trans_state[0] = p_uart_init->initial_transmit_state[0];
		initial_trans_state[1] = p_uart_init->initial_transmit_state[1];
		initial_trans_state[2] = p_uart_init->initial_transmit_state[2];
		initial_trans_state[3] = p_uart_init->initial_transmit_state[3];
		initial_trans_state[4] = p_uart_init->initial_transmit_state[4];
		initial_trans_state[5] = p_uart_init->initial_transmit_state[5];
		initial_trans_state[6] = p_uart_init->initial_transmit_state[6];
		initial_trans_state[7] = p_uart_init->initial_transmit_state[7];
		initial_trans_state[8] = p_uart_init->initial_transmit_state[8];
		initial_trans_state[9] = p_uart_init->initial_transmit_state[9];
		initial_trans_state[10] = p_uart_init->initial_transmit_state[10];
		initial_trans_state[11] = p_uart_init->initial_transmit_state[11];
		initial_trans_state[12] = p_uart_init->initial_transmit_state[12];
		initial_trans_state[13] = p_uart_init->initial_transmit_state[13];
		initial_trans_state[14] = p_uart_init->initial_transmit_state[14];
		initial_trans_state[15] = p_uart_init->initial_transmit_state[15];
		initial_trans_state[16] = p_uart_init->initial_transmit_state[15];
		initial_trans_state[17] = p_uart_init->initial_transmit_state[17];
		initial_trans_state[18] = p_uart_init->initial_transmit_state[18];
		initial_trans_state[19] = p_uart_init->initial_transmit_state[19];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 20;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;
    attr_char_value.p_value      = initial_trans_state;

    return sd_ble_gatts_characteristic_add(p_uart->service_handle,
											&char_md,
											&attr_char_value,
											&p_uart->transmit_handles);

}


/**@brief Function for adding the transmit characteristic.
 *
 * @param  p_uart        UART Service structure.
 * @param  p_uart_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t uart_receive_char_add(ble_uart_t * p_uart, const ble_uart_init_t * p_uart_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t	 	 	 	initial_rec_state[20];

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	
    memset(&char_md, 0, sizeof(char_md));

	char_md.char_props.write  = 0;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc       = NULL;
    char_md.p_char_pf              = NULL;
    char_md.p_user_desc_md         = NULL;
    char_md.p_cccd_md              = &cccd_md;
    char_md.p_sccd_md 	 	 	       = NULL;

	BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_UART_RECEIVE_CHAR);
	
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
	  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
		
		initial_rec_state[0] = p_uart_init->initial_receive_state[0];
		initial_rec_state[1] = p_uart_init->initial_receive_state[1];
		initial_rec_state[2] = p_uart_init->initial_receive_state[2];
		initial_rec_state[3] = p_uart_init->initial_receive_state[3];
		initial_rec_state[4] = p_uart_init->initial_receive_state[4];
		initial_rec_state[5] = p_uart_init->initial_receive_state[5];
		initial_rec_state[6] = p_uart_init->initial_receive_state[6];
		initial_rec_state[7] = p_uart_init->initial_receive_state[7];
		initial_rec_state[8] = p_uart_init->initial_receive_state[8];
		initial_rec_state[9] = p_uart_init->initial_receive_state[9];
		initial_rec_state[10] = p_uart_init->initial_receive_state[10];
		initial_rec_state[11] = p_uart_init->initial_receive_state[11];
		initial_rec_state[12] = p_uart_init->initial_receive_state[12];
		initial_rec_state[13] = p_uart_init->initial_receive_state[13];
		initial_rec_state[14] = p_uart_init->initial_receive_state[14];
		initial_rec_state[15] = p_uart_init->initial_receive_state[15];
		initial_rec_state[16] = p_uart_init->initial_receive_state[16];
		initial_rec_state[17] = p_uart_init->initial_receive_state[17];
		initial_rec_state[18] = p_uart_init->initial_receive_state[18];
		initial_rec_state[19] = p_uart_init->initial_receive_state[19];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 20;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;
    attr_char_value.p_value      = initial_rec_state;

    return sd_ble_gatts_characteristic_add(p_uart->service_handle,
											&char_md,
											&attr_char_value,
											&p_uart->receive_handles);

}

uint32_t ble_uart_init(ble_uart_t * p_uart, const ble_uart_init_t * p_uart_init)
{
	uint32_t  err_code;
	ble_uuid_t ble_uuid;
	
	p_uart->evt_handler 	 	  	 	      = p_uart_init->evt_handler;
	p_uart->conn_handle 	  	 		      = BLE_CONN_HANDLE_INVALID;
	p_uart->is_notification_supported  = p_uart_init->support_notification;
	
    ble_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN + UART_UUID_INDEX;
	ble_uuid.uuid = BLE_UUID_UART_SERVICE;
	
	err_code = sd_ble_uuid_vs_add(&m_base_uuid128, &ble_uuid.type);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_uart->service_handle);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	err_code = uart_transmit_char_add(p_uart, p_uart_init);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	err_code = uart_receive_char_add(p_uart, p_uart_init);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	return NRF_SUCCESS;
}

uint32_t ble_uart_transmit_send(ble_uart_t * p_uart, uint8_t data[], uint16_t length)
{
	uint32_t err_code = NRF_SUCCESS;
		
		//update database
		err_code = sd_ble_gatts_value_set(p_uart->transmit_handles.value_handle,
																			0,
																			&length,
																		  data);
		if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		//send value if connected and notifying
		if((p_uart->conn_handle != BLE_CONN_HANDLE_INVALID) && p_uart->is_notification_supported)
		{
			ble_gatts_hvx_params_t hvx_params;
			
			memset(&hvx_params, 0, sizeof(hvx_params));
			
			hvx_params.handle      = p_uart->transmit_handles.value_handle;
			hvx_params.type        = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset 	   = 0;
			hvx_params.p_len 	   = &length;
			hvx_params.p_data 	   = data;
			
			err_code = sd_ble_gatts_hvx(p_uart->conn_handle, &hvx_params);
		}
		else
		{
			err_code = NRF_ERROR_INVALID_STATE;
		}
	return err_code;
}

uint32_t ble_uart_receive_send(ble_uart_t * p_uart, uint8_t * data, uint16_t length)
{
	uint32_t err_code = NRF_SUCCESS;
		
		//update database
		err_code = sd_ble_gatts_value_set(p_uart->receive_handles.value_handle,
																			0,
																			&length,
																		  data);
		if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		//send value if connected and notifying
		if((p_uart->conn_handle != BLE_CONN_HANDLE_INVALID) && p_uart->is_notification_supported)
		{
			ble_gatts_hvx_params_t hvx_params;
			
			memset(&hvx_params, 0, sizeof(hvx_params));
			
			hvx_params.handle      = p_uart->receive_handles.value_handle;
			hvx_params.type        = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset 	   = 0;
			hvx_params.p_len 	     = &length;
			hvx_params.p_data 	   = data;
			
			err_code = sd_ble_gatts_hvx(p_uart->conn_handle, &hvx_params);
		}
		else
		{
			err_code = NRF_ERROR_INVALID_STATE;
		}
	return err_code;
}

/** @} */
