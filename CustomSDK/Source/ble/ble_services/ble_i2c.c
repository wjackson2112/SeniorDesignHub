
#include "ble_i2c.h"

#include <stdlib.h>
#include <string.h>
#include "app_error.h"
#include "ble_gatts.h"
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "nrf_gpio.h"


#define BLE_UUID_I2C_SERVICE	 	 	 	 	    0x4740 
#define BLE_UUID_I2C_RECEIVE_CHAR         0x4741  
#define BLE_UUID_I2C_TRANSMIT_CHAR					0x4742  
#define BLE_UUID_I2C_CONFIG_CHAR						0x4743
#define I2C_UUID_INDEX              				0

/**@brief 128-bit I2C UUID base List. */
static const ble_uuid128_t m_base_uuid128 =
{
	 {
			 0x66, 0x9A, 0x0C, 0x20, 0x00, 0x08, 0x21, 0x8C,
			 0xE4, 0x11, 0xC8, 0x1D, 0x00, 0x00, 0x1E, 0x7B
	 }
};

bool send_stop_bit = I2C_STOP_BIT;
bool use_operation_chaining = I2C_OPERATION_CHAINING;
bool use_transaction_id = I2C_TRANSACTION_ID;

ble_i2c_t m_i2c;

/**@brief Function for handling the Connect event.
 *
 * @param   p_i2c       I2C Service structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_i2c_t * p_i2c, ble_evt_t * p_ble_evt)
{
	p_i2c->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param   p_i2c       I2C Service structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_i2c_t * p_i2c, ble_evt_t * p_ble_evt)
{
	UNUSED_PARAMETER(p_ble_evt);
	p_i2c->conn_handle = BLE_CONN_HANDLE_INVALID;
}



/**@brief Function for handling the Write event.
 *
 * @param   p_i2c       I2C structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_i2c_t * p_i2c, ble_evt_t * p_ble_evt)
{
	
		ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	  const uint16_t char_handler = p_evt_write->handle;
	
		static uint16_t len_i2c_transmit_packet = 20;
		static uint16_t len_i2c_receive_packet = 20;
		static uint16_t len_i2c_config_packet = 1;

		if(p_evt_write->handle == p_i2c->transmit_handles.value_handle)
		{
			sd_ble_gatts_value_get(char_handler, 0, &len_i2c_transmit_packet, p_i2c->transmit_packet);
		}
		
		if(p_evt_write->handle == p_i2c->receive_handles.value_handle)
		{
			sd_ble_gatts_value_get(char_handler, 0, &len_i2c_receive_packet, p_i2c->receive_packet);
		}
		
		if(p_evt_write->handle == p_i2c->config_handles.value_handle)
		{
			sd_ble_gatts_value_get(char_handler, 0, &len_i2c_config_packet, p_i2c->config_packet);
		}
		
}

void ble_i2c_on_ble_evt(ble_i2c_t * p_i2c, ble_evt_t * p_ble_evt)
{
	switch(p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			on_connect(p_i2c, p_ble_evt);
			break;
			
		case BLE_GAP_EVT_DISCONNECTED:
			on_disconnect(p_i2c, p_ble_evt);
		    break;
					
		case BLE_GATTS_EVT_WRITE:
			on_write(p_i2c, p_ble_evt);
			break;
			
		default:	
			break;
	}
}	

/**@brief Function for adding the transmit characteristic.
 *
 * @param   p_i2c        I2C Service structure.
 * @param   p_i2c_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t i2c_transmit_char_add(ble_i2c_t * p_i2c, const ble_i2c_init_t * p_i2c_init)
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

		char_md.char_props.write_wo_resp = 1;
		char_md.char_props.write  = 1;
    char_md.char_props.read   = 0;
    char_md.char_props.notify = 0;
    char_md.p_char_user_desc       = NULL;
    char_md.p_char_pf              = NULL;
    char_md.p_user_desc_md         = NULL;
    char_md.p_cccd_md              = &cccd_md;
    char_md.p_sccd_md 	 	 	       = NULL;

	BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_I2C_TRANSMIT_CHAR);
	
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
	  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
		
		initial_trans_state[0] = p_i2c_init->initial_transmit_state[0];
		initial_trans_state[1] = p_i2c_init->initial_transmit_state[1];
		initial_trans_state[2] = p_i2c_init->initial_transmit_state[2];
		initial_trans_state[3] = p_i2c_init->initial_transmit_state[3];
		initial_trans_state[4] = p_i2c_init->initial_transmit_state[4];
		initial_trans_state[5] = p_i2c_init->initial_transmit_state[5];
		initial_trans_state[6] = p_i2c_init->initial_transmit_state[6];
		initial_trans_state[7] = p_i2c_init->initial_transmit_state[7];
		initial_trans_state[8] = p_i2c_init->initial_transmit_state[8];
		initial_trans_state[9] = p_i2c_init->initial_transmit_state[9];
		initial_trans_state[10] = p_i2c_init->initial_transmit_state[10];
		initial_trans_state[11] = p_i2c_init->initial_transmit_state[11];
		initial_trans_state[12] = p_i2c_init->initial_transmit_state[12];
		initial_trans_state[13] = p_i2c_init->initial_transmit_state[13];
		initial_trans_state[14] = p_i2c_init->initial_transmit_state[14];
		initial_trans_state[15] = p_i2c_init->initial_transmit_state[15];
		initial_trans_state[16] = p_i2c_init->initial_transmit_state[15];
		initial_trans_state[17] = p_i2c_init->initial_transmit_state[17];
		initial_trans_state[18] = p_i2c_init->initial_transmit_state[18];
		initial_trans_state[19] = p_i2c_init->initial_transmit_state[19];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 20;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;
    attr_char_value.p_value      = initial_trans_state;

    return sd_ble_gatts_characteristic_add(p_i2c->service_handle,
											&char_md,
											&attr_char_value,
											&p_i2c->transmit_handles);

}


/**@brief Function for adding the receive characteristic.
 *
 * @param  p_i2c        I2C Service structure.
 * @param  p_i2c_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t i2c_receive_char_add(ble_i2c_t * p_i2c, const ble_i2c_init_t * p_i2c_init)
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

	BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_I2C_RECEIVE_CHAR);
	
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
	  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
		
		initial_rec_state[0] = p_i2c_init->initial_receive_state[0];
		initial_rec_state[1] = p_i2c_init->initial_receive_state[1];
		initial_rec_state[2] = p_i2c_init->initial_receive_state[2];
		initial_rec_state[3] = p_i2c_init->initial_receive_state[3];
		initial_rec_state[4] = p_i2c_init->initial_receive_state[4];
		initial_rec_state[5] = p_i2c_init->initial_receive_state[5];
		initial_rec_state[6] = p_i2c_init->initial_receive_state[6];
		initial_rec_state[7] = p_i2c_init->initial_receive_state[7];
		initial_rec_state[8] = p_i2c_init->initial_receive_state[8];
		initial_rec_state[9] = p_i2c_init->initial_receive_state[9];
		initial_rec_state[10] = p_i2c_init->initial_receive_state[10];
		initial_rec_state[11] = p_i2c_init->initial_receive_state[11];
		initial_rec_state[12] = p_i2c_init->initial_receive_state[12];
		initial_rec_state[13] = p_i2c_init->initial_receive_state[13];
		initial_rec_state[14] = p_i2c_init->initial_receive_state[14];
		initial_rec_state[15] = p_i2c_init->initial_receive_state[15];
		initial_rec_state[16] = p_i2c_init->initial_receive_state[16];
		initial_rec_state[17] = p_i2c_init->initial_receive_state[17];
		initial_rec_state[18] = p_i2c_init->initial_receive_state[18];
		initial_rec_state[19] = p_i2c_init->initial_receive_state[19];

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 20;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;
    attr_char_value.p_value      = initial_rec_state;

    return sd_ble_gatts_characteristic_add(p_i2c->service_handle,
											&char_md,
											&attr_char_value,
											&p_i2c->receive_handles);

}

/**@brief Function for adding the configuration characteristic.
 *
 * @param  p_i2c        I2C Service structure.
 * @param  p_i2c_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t i2c_config_char_add(ble_i2c_t * p_i2c, const ble_i2c_init_t * p_i2c_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t	 	 	 	initial_conf_state[1];

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	
    memset(&char_md, 0, sizeof(char_md));

		char_md.char_props.write  = 1;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 0;
    char_md.p_char_user_desc       = NULL;
    char_md.p_char_pf              = NULL;
    char_md.p_user_desc_md         = NULL;
    char_md.p_cccd_md              = &cccd_md;
    char_md.p_sccd_md 	 	 	       = NULL;

		BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_I2C_CONFIG_CHAR);
	
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
	  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
		
		initial_conf_state[0] = (use_transaction_id << 2) && ((use_operation_chaining) << 1) && send_stop_bit;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 1;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 1;
    attr_char_value.p_value      = initial_conf_state;

    return sd_ble_gatts_characteristic_add(p_i2c->service_handle,
											&char_md,
											&attr_char_value,
											&p_i2c->config_handles);

}

uint32_t ble_i2c_init(ble_i2c_t * p_i2c, const ble_i2c_init_t * p_i2c_init)
{
	uint32_t  err_code;
	ble_uuid_t ble_uuid;
	
	p_i2c->evt_handler 	 	  	 	      = p_i2c_init->evt_handler;
	p_i2c->conn_handle 	  	 		      = BLE_CONN_HANDLE_INVALID;
	p_i2c->is_notification_supported  = p_i2c_init->support_notification;
	
    ble_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN + I2C_UUID_INDEX;
	ble_uuid.uuid = BLE_UUID_I2C_SERVICE;
	
	err_code = sd_ble_uuid_vs_add(&m_base_uuid128, &ble_uuid.type);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_i2c->service_handle);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	 
	err_code = i2c_receive_char_add(p_i2c, p_i2c_init);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	err_code = i2c_transmit_char_add(p_i2c, p_i2c_init);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	err_code = i2c_config_char_add(p_i2c, p_i2c_init);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	return NRF_SUCCESS;
}

uint32_t ble_i2c_transmit_send(ble_i2c_t * p_i2c, uint8_t data[], uint16_t length)
{
	uint32_t err_code = NRF_SUCCESS;
		
		//update database
		err_code = sd_ble_gatts_value_set(p_i2c->transmit_handles.value_handle,
																			0,
																			&length,
																		  data);
		if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		//send value if connected and notifying
		if((p_i2c->conn_handle != BLE_CONN_HANDLE_INVALID) && p_i2c->is_notification_supported)
		{
			ble_gatts_hvx_params_t hvx_params;
			
			memset(&hvx_params, 0, sizeof(hvx_params));
			
			hvx_params.handle      = p_i2c->transmit_handles.value_handle;
			hvx_params.type        = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset 	   = 0;
			hvx_params.p_len 	   = &length;
			hvx_params.p_data 	   = data;
			
			err_code = sd_ble_gatts_hvx(p_i2c->conn_handle, &hvx_params);
		}
		else
		{
			err_code = NRF_ERROR_INVALID_STATE;
		}
	return err_code;
}

uint32_t ble_i2c_receive_send(ble_i2c_t * p_i2c, uint8_t * data, uint16_t length)
{
	uint32_t err_code = NRF_SUCCESS;
		
		//update database
		err_code = sd_ble_gatts_value_set(p_i2c->receive_handles.value_handle,
																			0,
																			&length,
																		  data);
		if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		//send value if connected and notifying
		if((p_i2c->conn_handle != BLE_CONN_HANDLE_INVALID) && p_i2c->is_notification_supported)
		{
			ble_gatts_hvx_params_t hvx_params;
			
			memset(&hvx_params, 0, sizeof(hvx_params));
			
			hvx_params.handle      = p_i2c->receive_handles.value_handle;
			hvx_params.type        = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset 	   = 0;
			hvx_params.p_len 	     = &length;
			hvx_params.p_data 	   = data;
			
			err_code = sd_ble_gatts_hvx(p_i2c->conn_handle, &hvx_params);
		}
		else
		{
			err_code = NRF_ERROR_INVALID_STATE;
		}
	return err_code;
}

uint32_t ble_i2c_config_send(ble_i2c_t * p_i2c, uint8_t * data, uint16_t length)
{
	uint32_t err_code = NRF_SUCCESS;
		
		//update database
		err_code = sd_ble_gatts_value_set(p_i2c->config_handles.value_handle,
																			0,
																			&length,
																		  data);
		if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		//send value if connected and notifying
		if((p_i2c->conn_handle != BLE_CONN_HANDLE_INVALID) && p_i2c->is_notification_supported)
		{
			ble_gatts_hvx_params_t hvx_params;
			
			memset(&hvx_params, 0, sizeof(hvx_params));
			
			hvx_params.handle      = p_i2c->config_handles.value_handle;
			hvx_params.type        = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset 	   = 0;
			hvx_params.p_len 	     = &length;
			hvx_params.p_data 	   = data;
			
			err_code = sd_ble_gatts_hvx(p_i2c->conn_handle, &hvx_params);
		}
		else
		{
			err_code = NRF_ERROR_INVALID_STATE;
		}
	return err_code;
}

/** @} */
