#ifndef WEIGHT_H__
#define WEIGHT_H__


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "nrf_delay.h"
#include "app_timer.h"
#include "device_name.h"
#include "muart.h"
#include "app_uart.h"
#include "nrf_uart.h"
#include "SEGGER_RTT.h"
#include "hx711.h"
#include "ble_set.h"

void weight_test(void);
extern uint32_t adc_value_weight;

#endif
