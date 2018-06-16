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

extern uint8_t weight_float;
extern char uart_weight_ch[10];
extern uint8_t startWeightIndex;
extern uint8_t endWeightIndex;
extern float uart_weight_f;
extern int uart_weight;
void uart_init(void);


#endif
