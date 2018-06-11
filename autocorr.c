#include "autocorr.h"


uint32_t adc_vals_ar[9];


void autocor_adc_values_init(void)
{
					for(uint8_t i = 0; i<9; i++)
						{ 	
							 adc_vals_ar[i] = ADC_VALUE_BUT_INIT;
							 fds_init_flash(adc_vals_ar + i, file_id, fds_rk_adc_val1 + i);
						}
	}	

	
void autocor_adc_values_get(void)
{
				for(uint8_t i = 0; i<9; i++)
						{ 	
							fds_get_data(adc_vals_ar + i, file_id, fds_rk_adc_val1 + i);
							//SEGGER_RTT_printf(0, "adc_vals_ar[%d] = %d \n", i, adc_vals_ar[i]);
						}
}
	
void save_adc_cor_value(void)
{
	if(adc_value > 0 && adc_value < 16000000){
		int adc_value_but_sub = adc_value - adc_value_start;
		if (adc_value_but_sub < 0) adc_value_but_sub = -adc_value_but_sub;
		
		adc_vals_ar[corr_mode_button-1] = adc_value_but_sub;
		SEGGER_RTT_printf(0, "save adc_value = %d\n", adc_vals_ar[corr_mode_button-1]);
		fds_update_value(adc_vals_ar + corr_mode_button - 1, file_id, fds_rk_adc_val1 + corr_mode_button-1);
	}
	
}

uint32_t adc_value_filter(void){
	uint32_t adc_filtered = 0;
	if(adc_value > 0 && adc_value<16000000){
		adc_filtered = adc_value;
	}
		return adc_filtered;
}



