#ifndef MUART_H__
#define MUART_H__


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "nrf_delay.h"
#include "app_timer.h"
#include "nrf_drv_pwm.h"
#include "device_name.h"


#include "app_uart.h"
#include "nrf_uart.h"


#define UART_TX_BUF_SIZE                256	                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256																					 /**< UART RX buffer size. */





void uart_init(void);

#endif
