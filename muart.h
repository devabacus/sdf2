#ifndef MUART_H__
#define MUART_H__


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "nrf_delay.h"
#include "app_timer.h"
#include "nrf_drv_pwm.h"
#include "device_name.h"
#include "ble_conn_state.h"


#include "app_uart.h"
#include "nrf_uart.h"


#define UART_TX_BUF_SIZE                256	                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256																					 /**< UART RX buffer size. */

extern uint32_t weight_float;
extern char uart_weight_ch[5];
extern uint32_t startWeightIndex;
extern uint32_t endWeightIndex;
extern uint32_t uart_ble_mode;
extern uint8_t ble_connection;



extern float uart_weight_f;
extern int uart_weight;
void uart_init(void);
extern uint8_t uart_active;
extern uint32_t protocol;
enum {
	GENERAL_PROTOCOL=1,
	MIDDLE_MI_12_COMMAND_MODE = 2,
	MIDDLE_MI_12_COMMAND_MODE_1 = 3,
};

#endif
