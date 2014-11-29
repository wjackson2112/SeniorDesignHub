#ifndef GLOBAL_CONFIG_H__
#define GLOBAL_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>

/*************************
 * OVERALL CONFIGURATION *
 *************************/
#define DEVICE_NAME "Hub0"
#define PASSCODE {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define SETTINGS_ADDRESS 0x0001CC00
#define SETTINGS_BACKUP 0x0001C800

/**********************
 * UART CONFIGURATION *
 **********************/
#define UART_RTS_PIN_NUMBER (34U)
#define UART_TXD_PIN_NUMBER (32U)
#define UART_CTS_PIN_NUMBER (35U)
#define UART_RXD_PIN_NUMBER (33U)
#define UART_HWFC false
#define UART_BAUD_RATE UART_BAUDRATE_BAUDRATE_Baud9600
#define UART_PARITY_INCLUDED false

/**********************
 * I2C CONFIGURATION  *
 **********************/
#define I2C_CLK_PIN_NUMBER (9U)
#define I2C_SDA_PIN_NUMBER (8U)
#define I2C_STOP_BIT true;
#define I2C_OPERATION_CHAINING false;
#define I2C_TRANSACTION_ID false;
	//Required for Nordic TWI Master Library
#define TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER I2C_CLK_PIN_NUMBER
#define TWI_MASTER_CONFIG_DATA_PIN_NUMBER  I2C_SDA_PIN_NUMBER

/*************************
 * DIGITAL CONFIGURATION *
 *************************/
#define DIG_INPUT_PIN				(5U)
#define DIG_OUTPUT_PIN			(4U)

/*************************
 * ANALOG CONFIGURATION  *
 *************************/
#define ANA_INPUT_PIN				(2U)
#define ANA_UPDATE_TIME			(1000U)

/*********************
 * SPI CONFIGURATION *
 *********************/
#define SPI_SCK_PIN (12U)
#define SPI_MOSI_PIN (10U)
#define SPI_MISO_PIN (11U)
#define SPI_SS_PIN (13U)

#define SPI_MODE SPI_MODE0
#define SPI_LSB_FIRST true

#define SPI_PSELSCK0 SPI_SCK_PIN
#define SPI_PSELMOSI0 SPI_MOSI_PIN
#define SPI_PSELMISO0 SPI_MISO_PIN
#define SPI_PSELSS0 SPI_SS_PIN

#define SPI_PSELSCK1 SPI_SCK_PIN
#define SPI_PSELMOSI1 SPI_MOSI_PIN
#define SPI_PSELMISO1 SPI_MISO_PIN
#define SPI_PSELSS1 SPI_SS_PIN

#define SPI_OPERATING_FREQUENCY  ( 0x02000000UL << (uint32_t)Freq_1Mbps )  /*!< Slave clock frequency. */
#define TIMEOUT_COUNTER          0x3000UL  /*!< timeout for getting rx bytes from slave */


#endif
