/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new 
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "ble_nrf6310_pins.h"
#include "app_scheduler.h"
#include "ble_stack_handler.h"
#include "app_timer.h"
#include "ble_error_log.h"
#include "ble_debug_assert_handler.h"
#include "simple_uart.h"
#include "ble_uart.h"
#include "ble_i2c.h"
#include "ble_dig.h"
#include "ble_ana.h"
#include "ble_conf.h"
#include "ble_spi.h"
#include "twi_master.h"
#include "spi_master.h"
#include "global_config.h"

#define WAKEUP_BUTTON_PIN               NRF6310_BUTTON_0                            /**< Button used to wake up the application. */
// YOUR_JOB: Define any other buttons to be used by the applications:
// #define MY_BUTTON_PIN                   NRF6310_BUTTON_1



#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      0                                         /**< The advertising timeout (in units of seconds). */

// YOUR_JOB: Modify these according to requirements.
#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            2                                           /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(50, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time between each call to sd_ble_gap_conn_param_update after the first (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_GPIOTE_MAX_USERS            1                                           /**< Maximum number of users of the GPIOTE handler. */

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)    /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SEC_PARAM_TIMEOUT               30                                          /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static ble_gap_sec_params_t             m_sec_params;                               /**< Security requirements for this application. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */

// YOUR_JOB: Modify these according to requirements (e.g. if other event types are to pass through
//           the scheduler).
#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(app_timer_event_t)                   /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                10                                          /**< Maximum number of events in the scheduler queue. */

static app_timer_id_t analog_timer;

static void uart_reconfig(uint8_t);
static void i2c_init(void);
static void dig_init(void);
void flash_update(void);

bool needs_reboot = false;
bool needs_update = false;

/**@brief Function for error handling, which is called when an error has occurred. 
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    nrf_gpio_pin_set(ASSERT_LED_PIN_NO);
	
    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the 
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
/*
// YOUR_JOB: Uncomment this function and make it handle error situations sent back to your 
//           application by the services it uses.
static void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
} */


/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by the application.
 */
static void leds_init(void)
{
    GPIO_LED_CONFIG(ADVERTISING_LED_PIN_NO);
    GPIO_LED_CONFIG(CONNECTED_LED_PIN_NO);
    GPIO_LED_CONFIG(ASSERT_LED_PIN_NO);
    
    // YOUR_JOB: Add additional LED initialiazations if needed.
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);
    
    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one.
    err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
    APP_ERROR_CHECK(err_code); */
}


/**@brief Function for the GAP initialization.
 *
 * @details This function shall be used to setup all the necessary GAP (Generic Access Profile) 
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)device_name, 
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);  

    /* YOUR_JOB: Use an appearance value matching the application's use case.
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
    APP_ERROR_CHECK(err_code); */
    
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    
    // YOUR_JOB: Use UUIDs for service(s) used in your application.
    //ble_uuid_t adv_uuids[] = {{0x3740, BLE_UUID_TYPE_BLE}, {0x4740, BLE_UUID_TYPE_BLE}, {0x5740, BLE_UUID_TYPE_BLE}, {0x6740, BLE_UUID_TYPE_BLE}};

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));
    
    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
    //advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    //advdata.uuids_complete.p_uuids  = adv_uuids;
    
    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
	
		// Initialize Configuration Service
	  ble_conf_init_t conf_init;

    memset(&conf_init, 0, sizeof(conf_init));

    conf_init.evt_handler = NULL;
    conf_init.support_notification = true;

    for (int i = 0; i < 20; i++) {
        conf_init.initial_device_name_state[i] = 0x00;
    }
		
		for (int i = 0; i < sizeof(DEVICE_NAME); i++){
			conf_init.initial_device_name_state[i] = device_name[i];
		}

    ble_conf_init(&m_conf, &conf_init);
	
	  // Initialize UART Service
	  ble_uart_init_t uart_init;

    memset(&uart_init, 0, sizeof(uart_init));

    uart_init.evt_handler = NULL;
    uart_init.support_notification = true;

    for (int i = 0; i < 20; i++) {
        uart_init.initial_transmit_state[i] = 0x00;
    }

    ble_uart_init(&m_uart, &uart_init);
		
		// Initialize I2C Service
	  ble_i2c_init_t i2c_init;

    memset(&i2c_init, 0, sizeof(i2c_init));

    i2c_init.evt_handler = NULL;
    i2c_init.support_notification = true;

    for (int i = 0; i < 20; i++) {
        i2c_init.initial_transmit_state[i] = 0x00;
    }

    ble_i2c_init(&m_i2c, &i2c_init);
		
		// Initialize Digital Service
	  ble_dig_init_t dig_init;
    
    memset(&dig_init, 0, sizeof(dig_init));

    dig_init.evt_handler = NULL;
    dig_init.support_notification = true;

    for (int i = 0; i < 20; i++) {
        dig_init.initial_transmit_state[i] = 0x00;
    }

    ble_dig_init(&m_dig, &dig_init);
		
		// Initialize SPI Service
	  ble_spi_init_t spi_init;

    memset(&spi_init, 0, sizeof(spi_init));

    spi_init.evt_handler = NULL;
    spi_init.support_notification = true;

    for (int i = 0; i < 20; i++) {
        spi_init.initial_transmit_state[i] = 0x00;
				spi_init.initial_receive_state[i] = 0x00;
    }

    ble_spi_init(&m_spi, &spi_init);
		
		// Initialize Analog Service
		/*ble_ana_init_t ana_init;
		
    memset(&ana_init, 0, sizeof(ana_init));

    ana_init.evt_handler = NULL;
    ana_init.support_notification = true;

    for (int i = 0; i < 20; i++) {
        ana_init.initial_transmit_state[i] = 0x00;
    }

    ble_ana_init(&m_ana, &ana_init);*/

}


/**@brief Function for initializing security parameters.
 */
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;  
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
*/
static void timers_start(void)
{
    /* YOUR_JOB: Start your timers. below is an example of how to start a timer.
    uint32_t err_code;
    
    err_code = app_timer_start(m_app_timer_id, TIMER_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code); */
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t             err_code;
    ble_gap_adv_params_t adv_params;
    
    // Start advertising
    memset(&adv_params, 0, sizeof(adv_params));
    
    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = sd_ble_gap_adv_start(&adv_params);
    APP_ERROR_CHECK(err_code);
    nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);
}

/** Erases a page in flash
 *
 * @param page_address Adress of first word in page to be erased
 */
static void flash_page_erase(uint32_t *page_address)
{
  // Turn on flash erase enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
  }
  // Erase page:
  NRF_NVMC->ERASEPAGE = (uint32_t)page_address;
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
  }
  // Turn off flash erase enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG &= ~(NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
  }
}

/** Fills a page in flash with a value
 *
 * @param address Adress of first word in page to be filled
 * @param value Value to write to flash
 */
static void flash_word_write(uint32_t *address, uint32_t value)
{
  // Turn on flash write enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
  }
  *address = value;
  // Turn off flash write enable and wait until the NVMC is ready:
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
  }
}

/**
 * @brief Calculate Checksum for Page
 * @details This calculates a 32-bit checksum for a chosen page in memory.  The checksum is created by simply adding all of the values
 * in the page together and allowing the uint32 to overflow.  0xFFFFFFFF is always changed to 0x00000000 to avoid confusing this with
 * an empty word in the page.
 * 
 * @param start_address Address of the page for which to create a checksum. 
 * @return The checksum value.
 */
uint32_t calculate_page_checksum(uint32_t *start_address)
{
    uint32_t checksum = 0x00000000;
    for (int i = 0;  i < 255; i++) {
        checksum += *(start_address + i);

    }

    //All F's signals that the page was erased so
    //don't use this sentinel value
    //0x00000000 is acceptable just as a check
    if (checksum == 0xFFFFFFFF) {
        checksum = 0x00000000;
    }

    return checksum;
}

/**
 * @brief Copy a Page
 * @details This function copies one page to another page
 * 
 * @param from_address Address of the page to copy
 * @param to_address Address of the page to overwrite
 */
void copy_page(uint32_t *from_address, uint32_t *to_address)
{
    for (int i = 0; i < 255; i++) {
        flash_word_write(to_address + i, *(from_address + i));
    }
}

/**
 * @brief Update Settings Values in Flash
 * @details This function is called when a disconnect occurs to avoid interferring with BLE timing.  This is also
 * the point at which settings could have changed since the user must connect to change anything.  Once the settings
 * are written, new checksums are created and recorded.
 */
void flash_update(void)
{

    uint32_t first_checksum = 0x00000000;
    uint32_t second_checksum = 0x00000000;
    uint8_t tries_count = 0; //Give up after three tries

    do {
        tries_count++;
        uint32_t *addr;   //Page 1
        uint32_t *addr2;  //Page 2 to be used as a copy of page 1

        uint32_t transmissions[10];
        for(int i = 0; i < 10; i++){
        	transmissions[i] = 0;
        }

        // Start address:
        addr = (uint32_t *) SETTINGS_ADDRESS;
        addr2 = (uint32_t *) SETTINGS_BACKUP;

        flash_page_erase(addr);
				
				for(int i = 0; i < 20; i++){
					transmissions[0 + i/4] += (device_name[i] << (3-(i%4)) * 8);
				}
				
				for(int i = 0; i < 20; i++){
					transmissions[5 + i/4] += (passcode[i] << (3-(i%4)) * 8);
				}

        for(int i = 0; i < 10; i++){
        	flash_word_write(addr + i, transmissions[i]);
        }

        //Write checksum to last word
        first_checksum = calculate_page_checksum(addr);
        flash_word_write(addr + 255, first_checksum);

        //Copy to backup
        flash_page_erase(addr2);
        copy_page(addr, addr2);

        //Calculate backup checksum
        second_checksum = calculate_page_checksum(addr2);
        flash_word_write(addr2 + 255, second_checksum);

        //THIS IS A TEST
        //Use different combinations to test bad checksum logic

        //flash_page_erase(addr);
        //flash_page_erase(addr2);

        //for(int i = 0; i < 255; i++){
        //flash_word_write((addr + i), i);
        //flash_word_write((addr2 + i), i);
        //}

    } while (first_checksum != second_checksum && tries_count < 3);
}

/**
 * @brief Load all Default Values
 * @details When both checksums fail due to first startup or corruption, the default values are loaded from
 * values in defaults.h
 */
void load_all_defaults(void)
{

    //Load Defaults
	
		uint8_t def_device_name[] = DEVICE_NAME;
		for(int i = 0; i < sizeof(DEVICE_NAME); i++){
			device_name[i] = def_device_name[i];
		}
		
		for(int i = sizeof(DEVICE_NAME); i < 20; i++){
			device_name[i] = 0x00;
		}
		
		uint8_t def_passcode[] = PASSCODE;
		for(int i = 0; i < 20; i++){
			passcode[i] = def_passcode[i];
		}
}

/**
 * @brief Load Values from Flash
 * @details This function runs at startup to get the values that have been stored back out of flash.
 * At this point checksums are compared and the integrity of the settings is determined.  If anything is 
 * corrupted, a recovery is attempted from the backup.  If both are corrupted, the device returns to defaults.
 */
void flash_init(void)
{
    uint32_t *addr;
    uint32_t *addr2;

    addr = (uint32_t *) SETTINGS_ADDRESS;
    addr2 = (uint32_t *) SETTINGS_BACKUP;

    /*
     *  Confirm the checksums and attempt recovery
     */

    uint32_t first_checksum = calculate_page_checksum(addr);
    uint32_t second_checksum = calculate_page_checksum(addr2);

    //Note that if the user uses DFU to upgrade to this build, he must rewrite the
    //Settings again so he should record all of the settings first, then rewrite
    //them after the DFU is complete

    //First settings are good, copy over
    if (first_checksum == *(addr + 255) && second_checksum != *(addr2 + 255)) {
        flash_page_erase(addr2);
        copy_page(addr, addr2);
        flash_word_write(addr2 + 255, second_checksum);
    }

    //Second settings are good, copy over
    else if (first_checksum != *(addr + 255) && second_checksum == *(addr2 + 255)) {
        flash_page_erase(addr);
        copy_page(addr2, addr);
        flash_word_write(addr2 + 255, first_checksum);
    }

    //Both are bad, panic, erase everything, load defaults instead, and return
    else if (first_checksum != *(addr + 255) && second_checksum != *(addr2 + 255)) {
        flash_page_erase(addr);
        flash_page_erase(addr2);
        load_all_defaults();
        flash_update();
        return;
    }

    //OTA causes new values to be updated improperly
    //This allows the device to detect the issue and correct itself
    bool needs_update = false;

		for(int i = 0; i < 20; i++){
			device_name[i] = *(addr + i/4) >> ((3 - (i%4)) * 8) & 0x000000FF; 
		}
		
		for(int i = 0; i < 20; i++){
			passcode[i] = *(addr + 5 + i/4) >> ((3 - (i%4)) * 8) & 0x000000FF;
		}

    if(needs_update){
      flash_update();
    }


}

/**@brief BLE bluetooth on write handler.
 */
void on_write(ble_evt_t *p_ble_evt)
{

    uint16_t *check_handler = &p_ble_evt->evt.gatts_evt.params.hvc.handle;
	
	
		//Check for Configuration Service Input
		if (*check_handler == m_conf.enter_passcode_handles.value_handle){
			bool valid = true;
			
			for(int i = 0; i < 20; i++){
				if(m_conf.enter_passcode_packet[i] != passcode[i]){
					valid = false;
				}
			}
			
			passcode_valid = valid;
		}
		
		if(passcode_valid){
			if (*check_handler == m_conf.set_passcode_handles.value_handle){
				for(int i = 0; i < 20; i++){
					passcode[i] = m_conf.set_passcode_packet[i];
				}
				needs_update = true;
			}
		
			if (*check_handler == m_conf.device_name_handles.value_handle){
				int i = 0;
				while(m_conf.device_name_packet[i] != 0x00){
					device_name[i] = m_conf.device_name_packet[i];
					i++;
				}
				
				for(; i < 20; i++){
					device_name[i] = 0x00;
				}
				
				needs_update = true;
				needs_reboot = true;
			}
		

			//Check for UART Service Input
			if (*check_handler == m_uart.transmit_handles.value_handle) {

					uint8_t * response = m_uart.transmit_packet;
					simple_uart_put(response[0]);
					//ble_uart_receive_send(&m_uart, response, 20);
			}
		
			if (*check_handler == m_uart.config_handles.value_handle){
					uart_reconfig(m_uart.config_packet[0]);
			}
		
			//Check for I2C Service Input
			if (*check_handler == m_i2c.transmit_handles.value_handle) {
					uint8_t addr_and_r_w = m_i2c.transmit_packet[0];
					uint8_t * value = &(m_i2c.transmit_packet[1]);
				
					//Write
					if(addr_and_r_w % 2 == 0){
						twi_master_transfer(addr_and_r_w, value, p_ble_evt->evt.gatts_evt.params.write.len - 1, send_stop_bit); 
					}
					//Read
					if(addr_and_r_w % 2 == 1){
						twi_master_transfer(addr_and_r_w, value, p_ble_evt->evt.gatts_evt.params.write.len - 1, send_stop_bit); 
						if(passcode_valid)
							ble_i2c_receive_send(&m_i2c, value, p_ble_evt->evt.gatts_evt.params.write.len - 1);
					}
				
			}
		
			//0x00 - No Stop Bit
			//0x01 - Include Stop Bit
			if (*check_handler == m_i2c.config_handles.value_handle){
					if(m_i2c.config_packet[0] % 2 == 0){
						send_stop_bit = false;
					} else if(m_i2c.config_packet[0] % 2 == 1){
						send_stop_bit = true;
					}
			}
		
			//Check for Digital Service Input
			if (*check_handler == m_dig.transmit_handles.value_handle){
			
				if(m_dig.transmit_packet[0] > 0)
					nrf_gpio_pin_set(DIG_OUTPUT_PIN);
				else
					nrf_gpio_pin_clear(DIG_OUTPUT_PIN);
			}
		
			if (*check_handler == m_dig.config_handles.value_handle){
			
			}
			
			//Check for SPI Service Input
			if(*check_handler == m_spi.transmit_handles.value_handle){								
				uint8_t length = p_ble_evt->evt.gatts_evt.params.write.len;
				
				uint8_t input[length];
				uint8_t output[length];
				
				for(int i = 0; i < length; i++){
					input[i] = m_spi.transmit_packet[i];
					output[i] = 0x00;
				}
				
				if(spi_master_tx_rx(spi_base_address, length, input, output)){
					ble_spi_receive_send(&m_spi, output, length);
				}
				
			}
			
			if(*check_handler == m_spi.config_handles.value_handle){
				lsb_first = m_spi.config_packet[0] % 2;
				switch(m_spi.config_packet[0] >> 1){
					case 0:
						spi_mode = SPI_MODE0;
						break;
					case 1:
						spi_mode = SPI_MODE1;
						break;
					case 2:
						spi_mode = SPI_MODE2;
						break;
					case 3:
						spi_mode = SPI_MODE3;
						break;
					default:
						break;
				}
				spi_base_address = spi_master_init(SPI0, spi_mode, lsb_first);
			}
		}
}


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code = NRF_SUCCESS;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
						passcode_valid = false;
            nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
            nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            /* YOUR_JOB: Uncomment this part if you are using the app_button module to handle button
                         events (assuming that the button events are only needed in connected
                         state). If this is uncommented out here,
                            1. Make sure that app_button_disable() is called when handling
                               BLE_GAP_EVT_DISCONNECTED below.
                            2. Make sure the app_button module is initialized.
            err_code = app_button_enable();
            */
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
						if(needs_update)
							flash_update();
				
						if(needs_reboot)
							sd_nvic_SystemReset();
						
            nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;

            /* YOUR_JOB: Uncomment this part if you are using the app_button module to handle button
                         events. This should be done to save power when not connected
                         to a peer.
            err_code = app_button_disable();
            */
            if (err_code == NRF_SUCCESS)
            {
                advertising_start();
            }
            break;
            
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
                                                   BLE_GAP_SEC_STATUS_SUCCESS, 
                                                   &m_sec_params);
            break;
            
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0);
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
            break;
            
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            p_enc_info = &m_auth_status.periph_keys.enc_info;
            if (p_enc_info->div == p_ble_evt->evt.gap_evt.params.sec_info_request.div)
            {
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, p_enc_info, NULL);
            }
            else
            {
                // No keys found for this device
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL);
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            { 
                nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);

                // Go to system-off mode (this function will not return; wakeup will cause a reset)
                GPIO_WAKEUP_BUTTON_CONFIG(WAKEUP_BUTTON_PIN);
                err_code = sd_power_system_off();    
            }
            break;
						
				case BLE_GATTS_EVT_WRITE:
					on_write(p_ble_evt);
					break;

        default:
            break;
    }

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conf_on_ble_evt(&m_conf, p_ble_evt);
		ble_uart_on_ble_evt(&m_uart, p_ble_evt);
		ble_i2c_on_ble_evt(&m_i2c, p_ble_evt);
		ble_dig_on_ble_evt(&m_dig, p_ble_evt);
		//ble_ana_on_ble_evt(&m_ana, p_ble_evt);
		ble_spi_on_ble_evt(&m_spi, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
		on_ble_evt(p_ble_evt);
    /* 
    YOUR_JOB: Add service ble_evt handlers calls here, like, for example:
    ble_bas_on_ble_evt(&m_bas, p_ble_evt);
    */
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    // YOUR_JOB: If the MTU size is changed by the application, the MTU_SIZE parameter to
    //           BLE_STACK_HANDLER_INIT() must be changed accordingly.
    BLE_STACK_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM,
                           BLE_L2CAP_MTU_DEF,
                           ble_evt_dispatch,
                           true);
}


/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_event_wait();
    APP_ERROR_CHECK(err_code);
}

void UART0_IRQHandler(void){

    sd_nvic_DisableIRQ(UART0_IRQn);

    bool receiving = true;
    int bytesReceived = 0;
    uint8_t buffer[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    while(receiving){
	   //uint8_t next_char = simple_uart_get();
				
        receiving = simple_uart_get_with_timeout(5, &buffer[bytesReceived]);
			
				if(receiving){
					bytesReceived++;
					if(bytesReceived == 20){
						if(passcode_valid)
							ble_uart_receive_send(&m_uart, buffer, 20);
						bytesReceived = 0;
					}
				}
				
				if(!receiving && bytesReceived == 0){
					sd_nvic_EnableIRQ(UART0_IRQn);
					return;
				}
    }

	//uint8_t response[1] = {next_char};
		
	if(bytesReceived != 1){
		if(passcode_valid)
			ble_uart_receive_send(&m_uart, buffer, bytesReceived);  
	}
		
  sd_nvic_EnableIRQ(UART0_IRQn);
}

static void uart_init(void){
	simple_uart_config( UART_RTS_PIN_NUMBER,
                          UART_TXD_PIN_NUMBER,
                          UART_CTS_PIN_NUMBER,
                          UART_RXD_PIN_NUMBER,
                          uart_hw_flow_control,
													uart_baud_rate,
													uart_parity_included);
	
	NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
  sd_nvic_SetPriority(UART0_IRQn, APP_IRQ_PRIORITY_LOW);
  sd_nvic_EnableIRQ(UART0_IRQn);
}

//Config byte format
//		bit7   0
//		bit6   0
// 		bit5   Include Parity? (Only even parity supported)
//		bit4   Hardware Flow Control?
// 		bit3-0 Baud Rate
//			0000 = 1200
//			0001 = 2400
//			0010 = 4800
//			0011 = 9600
//			0100 = 14400
//			0101 = 19200
//			0110 = 28800
//			0111 = 38400
//			1000 = 57600
//			1001 = 76800
//			1010 = 115200
//			1011 = 230400
//			1100 = 250000
//			1101 = 460800
//			1110 = 921600
//			1111 = 1000000
static void uart_reconfig(uint8_t config_byte){
	//Check parity setting
	if((config_byte & 0x20) == 0){
		uart_parity_included = false;
	} else {
		uart_parity_included = true;
	}
	
	//Check hardware flow control setting
	if((config_byte & 0x10) == 0){
		uart_hw_flow_control = false;
	} else {
		uart_hw_flow_control = true;
	}
	
	//Check baud rate settings
	uart_baud_rate = (config_byte & 0x0F);
	uint32_t baud_rate_decode;
	
	switch(uart_baud_rate){
		case 0:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud1200;
			break;
		case 1:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud2400;
			break;
		case 2:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud4800;
			break;
		case 3:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud9600;
			break;
		case 4:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud14400;
			break;
		case 5:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud19200;
			break;
		case 6:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud28800;
			break;
		case 7:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud38400;
			break;
		case 8:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud57600;
			break;
		case 9:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud76800;
			break;
		case 10:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud115200;
			break;
		case 11:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud230400;
			break;
		case 12:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud250000;
			break;
		case 13:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud460800;
			break;
		case 14:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud921600;
			break;
		case 15:
			baud_rate_decode = UART_BAUDRATE_BAUDRATE_Baud1M;
			break;
		default:
			break;
	}
	
	NRF_UART0->TASKS_STOPTX    = 1;
  NRF_UART0->TASKS_STOPRX    = 1;
	NRF_UART0->ENABLE          = (UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos);
	
	simple_uart_config( UART_RTS_PIN_NUMBER,
                          UART_TXD_PIN_NUMBER,
                          UART_CTS_PIN_NUMBER,
                          UART_RXD_PIN_NUMBER,
                          uart_hw_flow_control,
													baud_rate_decode,
													uart_parity_included);

}

static void i2c_init(void){
	twi_master_init();
}

/**
 * Configures pin 0 for input and pin 8 for output and
 * configures GPIOTE to give interrupt on pin change.
 */
static void gpio_init(void)
{
  NRF_GPIO->PIN_CNF[DIG_INPUT_PIN] = (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos)
                                        | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos); 

  // Enable interrupt:
	NVIC_SetPriority(GPIOTE_IRQn, 3);
  NVIC_EnableIRQ(GPIOTE_IRQn);
	
  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Set << GPIOTE_INTENSET_PORT_Pos;
}

/** GPIOTE interrupt handler.
 * Triggered on pin 0 change
 */
void GPIOTE_IRQHandler(void)
{
  // Event causing the interrupt must be cleared
  if ((NRF_GPIOTE->EVENTS_PORT != 0))
  {
    NRF_GPIOTE->EVENTS_PORT = 0;
  }
  // Active low.
  if (nrf_gpio_pin_read(DIG_INPUT_PIN) == 0)
  {
			uint8_t data[] = {0};
			
			if(passcode_valid)
				ble_dig_receive_send(&m_dig, data, 1);
			NVIC_DisableIRQ(GPIOTE_IRQn);
		  NRF_GPIO->PIN_CNF[DIG_INPUT_PIN] = (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos)
                                        | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos); 
			NVIC_EnableIRQ(GPIOTE_IRQn);
			NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Set << GPIOTE_INTENSET_PORT_Pos;
  } else {
			uint8_t data[] = {1};
			NVIC_DisableIRQ(GPIOTE_IRQn);
			if(passcode_valid)
				ble_dig_receive_send(&m_dig, data, 1);
			
			NRF_GPIO->PIN_CNF[DIG_INPUT_PIN] = (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos)
                                        | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos); 
			NVIC_EnableIRQ(GPIOTE_IRQn);
			NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Set << GPIOTE_INTENSET_PORT_Pos;
	}
}

static void dig_init(void){
	nrf_gpio_cfg_output(DIG_OUTPUT_PIN);
	nrf_gpio_pin_clear(DIG_OUTPUT_PIN);
	gpio_init();
}

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200                                      
#define ADC_PRE_SCALING_COMPENSATION         3                                         
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS       270  
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 1023) * ADC_PRE_SCALING_COMPENSATION)

void ADC_IRQHandler(void)
{

	  NRF_ADC->INTENCLR = ADC_INTENCLR_END_Clear;
    if (NRF_ADC->EVENTS_END != 0)
    {
			
        uint16_t     adc_result;
        uint16_t     lvl_in_milli_volts;

        NRF_ADC->EVENTS_END     = 0;
        adc_result              = NRF_ADC->RESULT;
        NRF_ADC->TASKS_STOP     = 1;
			
				lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result);
			
				uint8_t data[] = {lvl_in_milli_volts >> 8, lvl_in_milli_volts};
			
				if(passcode_valid)
					ble_dig_analog_send(&m_dig, data, 2);
			
    }
}

static void ana_read(void * p_context){
	NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
  NRF_ADC->CONFIG     = (ADC_CONFIG_RES_10bit                       << ADC_CONFIG_RES_Pos)     |
                        (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos)  |
                        (ADC_CONFIG_REFSEL_VBG                      << ADC_CONFIG_REFSEL_Pos)  |
                        (ADC_CONFIG_PSEL_AnalogInput2                   << ADC_CONFIG_PSEL_Pos)    |
                        (ADC_CONFIG_EXTREFSEL_None                  << ADC_CONFIG_EXTREFSEL_Pos);
	
	NRF_ADC->EVENTS_END = 0;
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled << ADC_ENABLE_ENABLE_Pos;
	
	// Enable ADC interrupt
  sd_nvic_ClearPendingIRQ(ADC_IRQn);
  sd_nvic_SetPriority(ADC_IRQn, 3);
  sd_nvic_EnableIRQ(ADC_IRQn);

  NRF_ADC->EVENTS_END  = 0;    // Stop any running conversions.
	NRF_ADC->TASKS_START = 1;
}

static void ana_init(void){
	  
		uint32_t err_code = NRF_SUCCESS;
	
		err_code = app_timer_create(&analog_timer, APP_TIMER_MODE_REPEATED, ana_read);
    APP_ERROR_CHECK(err_code);
	
	  err_code = app_timer_start(analog_timer, APP_TIMER_TICKS(ANA_UPDATE_TIME, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
}

static void configuration_init(void){
		
		//uint8_t def_device_name[] = DEVICE_NAME;
	
		//for(int i = 0; i < sizeof(DEVICE_NAME); i++){
		//	device_name[i] = def_device_name[i];
		//}
	
}

static void spi_init(void){
	spi_mode = SPI_MODE;
	lsb_first = SPI_LSB_FIRST;
	spi_base_address = spi_master_init(SPI0, spi_mode, lsb_first);
}

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize
    leds_init();
    timers_init();
    //gpiote_init();
    //buttons_init();
	
		flash_init();
		configuration_init();

		
		
    ble_stack_init();
    scheduler_init();
    gap_params_init();
    advertising_init();
    services_init();
    conn_params_init();
    sec_params_init();
		uart_init();
		i2c_init();
		dig_init();		
		ana_init();
		spi_init();
    
    // Start execution
    timers_start();
    advertising_start();

    
    // Enter main loop
    for (;;)
    {
        app_sched_execute();
        power_manage();
    }
}

/** 
 * @}
 */
