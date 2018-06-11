#ifndef OFFSET_PERC_H__
#define OFFSET_PERC_H__


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "nrf_delay.h"
#include "app_timer.h"



APP_TIMER_DEF(m_timer_offset_perc);

#define TIME_FIRE_ADC1 						200
#define TIME_FIRE_ADC2 						1000

#define TIME_FIRE_PULL						100

#define OFFSET_PLUS_KG_START 	1250
#define OFFSET_MINUS_KG_START  250

typedef enum
{
	TIMER_ADC,
	TIMER_PULL_MANUAL,
	
} timer_num_state_t;


typedef enum 
{
	STANDBY,
  PERC_INCREMENT_MANUAL,
	PERC_INCREMENT_AUTO,
	PERC_OFFSET_INIT,
	
} offset_perc_state_t;


typedef enum
{
		ADC_IDLE,
		ADC_INC_KG1,
	  ADC_INC_PERC,
//	ADC_INC_KG2,
//	ADC_DEC_KG1,
//	ADC_DEC_KG2,
		ADC_FOUND_KG_PERC,
} adc_offset_kg_t;


void perc_offset_init_auto(void);

void offset_perc_define(void);

void offset_fds_kg_get_data(void);

void offset_fds_kg_init_data(void);

//void offset_fds_update(uint8_t button_num);

void offset_kg_define_manual(void);

void offset_kg_init(void);

void offset_kg_cor_work(void);
#endif
