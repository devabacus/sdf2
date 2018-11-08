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


#define NUM_OF_DISCRETE_FOR_CAL	500

#define AVERAGE_ADC_TIMES 		30

#define NUM_EXCEED_MEMBERS 		16




extern uint32_t 	cal_zero_value;
extern uint32_t 	cal_load_value;
extern uint32_t		cal_coef;
extern uint8_t  	scale_type;
extern uint32_t 	cal_turn_on;
extern uint8_t  	start_average_adc;
extern uint32_t 	adc_need;
extern float 	weight;
void adc_cut(void);
extern uint32_t adc_value_weight;

extern uint32_t maxWeight;
extern float 		discrete;
extern uint32_t cal_weight;

void weight_define(void);
void cal_unload(void);
void cal_load(void);
void find_average_adc(void);
void init_cal_values(void);
void set_weight(uint16_t weight_value);
void define_corr_on(void);
#endif
