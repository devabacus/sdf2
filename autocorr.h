#ifndef AUTOCORR_H_
#define AUTOCORR_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_nus.h"
#include "scale_setup.h"
#include "flash_storage.h"
#include "device_name.h"
#include "ble_comm.h"
#include "sd_util.h"


#define ADC_VALUE_BUT_INIT 1000

extern uint32_t adc_vals_ar[9];
void autocor_adc_values_init(void);
void autocor_adc_values_get(void);
uint32_t adc_value_fiter(void);

#endif
