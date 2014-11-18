
#include "ble_conf.h"

#include <stdlib.h>
#include <string.h>
#include "app_error.h"
#include "ble_gatts.h"
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "nrf_gpio.h"


#define BLE_UUID_CONF_SERVICE	 	 	 	 	      0x2740 
#define BLE_UUID_CONF_ENTER_PASSCODE_CHAR   0x2741
#define BLE_UUID_CONF_SET_PASSCODE_CHAR	    0x2742
#define BLE_UUID_CONF_DEVICE_NAME_CHAR			0x2743
#define CONF_UUID_INDEX              				0

/**@brief 128-bit CONF UUID base List. */
static const ble_uuid128_t m_base_uuid128 =
{
	 {
			 0x66, 0x9A, 0x0C, 0x20, 0x00, 0x08, 0x21, 0x8C,
			 0xE4, 0x11, 0xC8, 0x1D, 0x00, 0x00, 0x1E, 0x7B
	 }
};

ble_conf_t m_conf;
uint8_t passcode[20] = PASSCODE;
bool passcode_valid = false;
uint8_t device_name[20] = DEVICE_NAME;

/**@brief Function for handling the Connect event.
 *
 * @param   p_conf       CONF Service structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_conf_t * p_conf, ble_evt_t * p_ble_evt)
{
	p_conf->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param   p_conf       CONF Service structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_conf_t * p_conf, ble_evt_t * p_ble_evt)
{
	UNUSED_PARAMETER(p_ble_evt);
	p_conf->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the Write event.
 *
 * @param   p_conf       CONF structure.
 * @param   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_conf_t * p_conf, ble_evt_t * p_ble_evt)
{
	
		ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	  const uint16_t char_handler = p_evt_write->handle;
	
		static uint16_t len_enter_passcode_packet = 20;
		static uint16_t len_set_passcode_packet = 20;
		static uint16_t len_device_name_packet = 20;

		if(p_evt_write->handle == p_conf->enter_passcode_handles.value_handle)
		{
			sd_ble_gatts_value_get(char_handler, 0, &len_enter_passcode_packet, p_conf->enter_passcode_packet);
		}
		
		if(p_evt_write->handle == p_conf->set_passcode_handles.value_handle)
		{
			sd_ble_gatts_value_get(char_handler, 0, &len_set_passcode_packet, p_conf->set_passcode_packet);
		}
		
		if(p_evt_write->handle == p_conf->device_name_handles.value_handle)
		{
			sd_ble_gatts_value_get(char_handler, 0, &len_device_name_packet, p_conf->device_name_packet);
		}
		
}

void ble_conf_on_ble_evt(ble_conf_t * p_conf, ble_evt_t * p_ble_evt)
{
	switch(p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			on_connect(p_conf, p_ble_evt);
			break;
			
		case BLE_GAP_EVT_DISCONNECTED:
			on_disconnect(p_conf, p_ble_evt);
		    break;
					
		case BLE_GATTS_EVT_WRITE:
			on_write(p_conf, p_ble_evt);
			break;
			
		default:	
			break;
	}
}	

/**@brief Function for adding the transmit characteristic.
 *
 * @param   p_conf        CONF Service structure.
 * @param   p_conf_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t conf_enter_passcode_char_add(ble_conf_t * p_conf, const ble_conf_init_t * p_conf_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t	 	 	 	initial_ent_pass_state[20];

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

		BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CONF_ENTER_PASSCODE_CHAR);
	
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
	  attr_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 20;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;
    attr_char_value.p_value      = initial_ent_pass_state;

    return sd_ble_gatts_characteristic_add(p_conf->service_handle,
											&char_md,
											&attr_char_value,
											&p_conf->enter_passcode_handles);

}


/**@brief Function for adding the receive characteristic.
 *
 * @param  p_conf        CONF Service structure.
 * @param  p_conf_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t conf_set_passcode_char_add(ble_conf_t * p_conf, const ble_conf_init_t * p_conf_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t	 	 	 	initial_set_pass_state[20];

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

	BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CONF_SET_PASSCODE_CHAR);
	
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
	  attr_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 20;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;
    attr_char_value.p_value      = initial_set_pass_state;

    return sd_ble_gatts_characteristic_add(p_conf->service_handle,
											&char_md,
											&attr_char_value,
											&p_conf->set_passcode_handles);

}

/**@brief Function for adding the configuration characteristic.
 *
 * @param  p_conf        CONF Service structure.
 * @param  p_conf_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t conf_device_name_char_add(ble_conf_t * p_conf, const ble_conf_init_t * p_conf_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    static uint8_t	 	 	 	initial_dev_name_state[20];

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

		BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CONF_DEVICE_NAME_CHAR);
	
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
	  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
		
		for(int i = 0; i < 20; i++){
			initial_dev_name_state[i] = p_conf_init->initial_device_name_state[i];
		}

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 20;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;
    attr_char_value.p_value      = initial_dev_name_state;

    return sd_ble_gatts_characteristic_add(p_conf->service_handle,
											&char_md,
											&attr_char_value,
											&p_conf->device_name_handles);

}

uint32_t ble_conf_init(ble_conf_t * p_conf, const ble_conf_init_t * p_conf_init)
{
	uint32_t  err_code;
	ble_uuid_t ble_uuid;
	
	p_conf->evt_handler 	 	  	 	      = p_conf_init->evt_handler;
	p_conf->conn_handle 	  	 		      = BLE_CONN_HANDLE_INVALID;
	p_conf->is_notification_supported  = p_conf_init->support_notification;
	
    ble_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN + CONF_UUID_INDEX;
	ble_uuid.uuid = BLE_UUID_CONF_SERVICE;
	
	err_code = sd_ble_uuid_vs_add(&m_base_uuid128, &ble_uuid.type);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_conf->service_handle);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	 
	err_code = conf_enter_passcode_char_add(p_conf, p_conf_init);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	err_code = conf_set_passcode_char_add(p_conf, p_conf_init);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	err_code = conf_device_name_char_add(p_conf, p_conf_init);
	if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	return NRF_SUCCESS;
}

uint32_t ble_conf_enter_passcode_send(ble_conf_t * p_conf, uint8_t data[], uint16_t length)
{
	uint32_t err_code = NRF_SUCCESS;
		
		//update database
		err_code = sd_ble_gatts_value_set(p_conf->enter_passcode_handles.value_handle,
																			0,
																			&length,
																		  data);
		if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		//send value if connected and notifying
		if((p_conf->conn_handle != BLE_CONN_HANDLE_INVALID) && p_conf->is_notification_supported)
		{
			ble_gatts_hvx_params_t hvx_params;
			
			memset(&hvx_params, 0, sizeof(hvx_params));
			
			hvx_params.handle      = p_conf->enter_passcode_handles.value_handle;
			hvx_params.type        = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset 	   = 0;
			hvx_params.p_len 	   = &length;
			hvx_params.p_data 	   = data;
			
			err_code = sd_ble_gatts_hvx(p_conf->conn_handle, &hvx_params);
		}
		else
		{
			err_code = NRF_ERROR_INVALID_STATE;
		}
	return err_code;
}

uint32_t ble_conf_set_passcode_send(ble_conf_t * p_conf, uint8_t * data, uint16_t length)
{
	uint32_t err_code = NRF_SUCCESS;
		
		//update database
		err_code = sd_ble_gatts_value_set(p_conf->set_passcode_handles.value_handle,
																			0,
																			&length,
																		  data);
		if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		//send value if connected and notifying
		if((p_conf->conn_handle != BLE_CONN_HANDLE_INVALID) && p_conf->is_notification_supported)
		{
			ble_gatts_hvx_params_t hvx_params;
			
			memset(&hvx_params, 0, sizeof(hvx_params));
			
			hvx_params.handle      = p_conf->set_passcode_handles.value_handle;
			hvx_params.type        = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset 	   = 0;
			hvx_params.p_len 	     = &length;
			hvx_params.p_data 	   = data;
			
			err_code = sd_ble_gatts_hvx(p_conf->conn_handle, &hvx_params);
		}
		else
		{
			err_code = NRF_ERROR_INVALID_STATE;
		}
	return err_code;
}

uint32_t ble_conf_device_name_send(ble_conf_t * p_conf, uint8_t * data, uint16_t length)
{
	uint32_t err_code = NRF_SUCCESS;
		
		//update database
		err_code = sd_ble_gatts_value_set(p_conf->device_name_handles.value_handle,
																			0,
																			&length,
																		  data);
		if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		//send value if connected and notifying
		if((p_conf->conn_handle != BLE_CONN_HANDLE_INVALID) && p_conf->is_notification_supported)
		{
			ble_gatts_hvx_params_t hvx_params;
			
			memset(&hvx_params, 0, sizeof(hvx_params));
			
			hvx_params.handle      = p_conf->device_name_handles.value_handle;
			hvx_params.type        = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset 	   = 0;
			hvx_params.p_len 	     = &length;
			hvx_params.p_data 	   = data;
			
			err_code = sd_ble_gatts_hvx(p_conf->conn_handle, &hvx_params);
		}
		else
		{
			err_code = NRF_ERROR_INVALID_STATE;
		}
	return err_code;
}

/** @} */
