#ifndef GLOBAL_CONFIG_H__
#define GLOBAL_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>

/**********************
 * UART CONFIGURATION *
 **********************/
#define UART_RTS_PIN_NUMBER (10U)
#define UART_TXD_PIN_NUMBER (9U)
#define UART_CTS_PIN_NUMBER (6U)
#define UART_RXD_PIN_NUMBER (11U)
#define UART_HWFC false
#define UART_BAUD_RATE (4U) //UART_BAUDRATE_BAUDRATE_Baud9600
#define UART_PARITY_INCLUDED false

/**********************
 * I2C CONFIGURATION  *
 **********************/
#define I2C_CLK_PIN_NUMBER (21U)
#define I2C_SDA_PIN_NUMBER (22U)
#define I2C_STOP_BIT true;
	//Required for Nordic TWI Master Library
#define TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER I2C_CLK_PIN_NUMBER
#define TWI_MASTER_CONFIG_DATA_PIN_NUMBER  I2C_SDA_PIN_NUMBER

/*************************
 * DIGITAL CONFIGURATION *
 *************************/
#define DIG_INPUT_PIN				(13U)
#define DIG_OUTPUT_PIN			(12U)

#endif
