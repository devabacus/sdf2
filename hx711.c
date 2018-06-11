#include "hx711.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_timer.h"
//#include "nrf_drv_pwm.h"
#include "nrf_drv_ppi.h"



uint32_t adc_value = 0;
uint32_t adc_value_r = 0;

uint8_t calib = 0;

void in_data_handler (nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)

{
	adc_ready = 1;
}

void HX711_init(void)
{
	nrf_drv_gpiote_init();
	
	#ifndef ADC1230
	nrf_gpio_pin_set(HX_DATA); 
	#endif
	
	
	nrf_drv_gpiote_in_config_t in_data_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
	in_data_config.pull = NRF_GPIO_PIN_PULLUP;
	
	nrf_drv_gpiote_in_init (HX_DATA, &in_data_config, in_data_handler);
	nrf_drv_gpiote_in_event_enable(HX_DATA, true);
	
	nrf_gpio_cfg_output(HX_SCK);
		
}

uint32_t Weighing(void)
{
	
	//adc_value = 0;
	uint8_t i = 0;
	
	#ifndef ADC1230
	nrf_gpio_pin_clear(HX_SCK); //for hx
	#endif
	
	#ifdef ADC1230
	
	if (calib < 5)
	{
		if (adc_ready == 1)
		{
			nrf_delay_ms(100);
		 for(i = 0; i<26; i++)  
			
					nrf_gpio_pin_set(HX_SCK);
					nrf_delay_us(5);
					nrf_gpio_pin_clear(HX_SCK);
					nrf_delay_us(5);
		}
		calib++;
	}
	
	#endif
	
	
	if (adc_ready == 1)
	{
		adc_value_r = 0;
		nrf_drv_gpiote_in_event_disable(HX_DATA);
		
		#ifdef ADC1230
				for(i = 0; i<20; i++)
		
				{
					nrf_gpio_pin_set(HX_SCK);
					nrf_delay_us(5);
					adc_value_r <<= 1;
					nrf_gpio_pin_clear(HX_SCK);
					nrf_delay_us(5);
				
					if(nrf_drv_gpiote_in_is_set(HX_DATA))
						{
							adc_value_r++;
						}
				}
			#else

				for(i = 0; i<24; i++)
		
				{
					nrf_gpio_pin_set(HX_SCK);
					nrf_delay_us(1);
					adc_value_r <<= 1;
					nrf_gpio_pin_clear(HX_SCK);
					nrf_delay_us(1);
				
					if(nrf_drv_gpiote_in_is_set(HX_DATA))
						{
							adc_value_r++;
						}
				}
				
			#endif
				
			
				
				
				
				#ifndef ADC1230
				//additional pulses for 64 gain (look at the datasheet's table)
		// 25		
		nrf_gpio_pin_set(HX_SCK);
		nrf_delay_us(1); 
		nrf_gpio_pin_clear(HX_SCK);
		nrf_delay_us(1); 
	
		//26
  	nrf_gpio_pin_set(HX_SCK);
		nrf_delay_us(1); 
		nrf_gpio_pin_clear(HX_SCK);
		nrf_delay_us(1); 
					
		//27
  	nrf_gpio_pin_set(HX_SCK);
		nrf_delay_us(1); 
		nrf_gpio_pin_clear(HX_SCK);
		nrf_delay_us(1); 		
				
				#endif
				
				
		adc_ready = 0;
		nrf_drv_gpiote_in_event_enable(HX_DATA, true);
	}
	
	
//			adc_value >>= 7;
//			nrf_delay_ms(100);
			
	return adc_value_r;
}

void ConvertADC(void)
	{
		if(adc_value_r > 0)
		{
			//ultoa(adc_value, buffer+1, 10);
			
		}
	}
	






