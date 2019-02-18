#ifndef REMOTE_WORK_H
#define REMOTE_WORK_H

#define COR_AUTO	 1
#define COR_MANUAL 2
#define COR_AUTO_PORT 3
#define COR_MODE



#include "remote.h"
#include "ble_comm.h"

#define NUM_ADC_CHANGE_FOR_DIRECT 2

extern uint32_t cor_value_auto;
extern uint32_t current_correct;
extern uint32_t cur_comp_cor;
extern uint32_t correct_mode;
extern uint8_t cor_set;

extern uint32_t adc_value_start;

void buttons_handle(void);

void init_corr_values(void);

void cor_auto_handle(void);

typedef enum 
{
	BUT_1_1 = 1,
	BUT_1_2,
	BUT_1_3,
	BUT_2_1,
	BUT_2_2,
	BUT_2_3,
	BUT_3_1,
	BUT_3_2,
	BUT_3_3,
} current_but_t;

extern current_but_t current_but;

#endif //
