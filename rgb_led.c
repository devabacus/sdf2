#include "rgb_led.h"

nrf_drv_pwm_t m_pwm_rgb = NRF_DRV_PWM_INSTANCE(1);
nrf_pwm_values_individual_t seq_value_rgb[2];
nrf_pwm_sequence_t seq_rgb;


void detect_rgv_ver(){

}


void saadc_callback(nrf_drv_saadc_evt_t const * p_event){

	NRF_LOG_INFO("saadc_callback");
}


void pwm_init_rgb()

{
	
	nrf_saadc_value_t value;
	ret_code_t err_code;
		
	nrf_drv_pwm_config_t config0 =
	{
			.irq_priority = APP_IRQ_PRIORITY_LOW,
			.base_clock   = NRF_PWM_CLK_1MHz,
			.count_mode   = NRF_PWM_MODE_UP,
			.top_value    = RGB_TOP,
			.load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
			.step_mode    = NRF_PWM_STEP_AUTO
	};
	
	nrf_gpio_cfg_output(RGB_GND);
	nrf_gpio_pin_set(RGB_GND);
			
	nrf_saadc_channel_config_t channel_config =
			NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN3);
	channel_config.gain 			= NRF_SAADC_GAIN1_2;
	channel_config.reference 	= NRF_SAADC_REFERENCE_INTERNAL;
	channel_config.acq_time 	= NRF_SAADC_ACQTIME_20US;
	channel_config.burst 			= NRF_SAADC_BURST_ENABLED;
	channel_config.resistor_p = NRF_SAADC_RESISTOR_PULLDOWN;
	
	nrf_drv_saadc_config_t saadc_config = NRF_DRV_SAADC_DEFAULT_CONFIG;
	saadc_config.resolution 		= NRF_SAADC_RESOLUTION_8BIT;
	saadc_config.oversample 		= NRF_SAADC_OVERSAMPLE_32X;
	saadc_config.low_power_mode = true;

	err_code = nrf_drv_saadc_init(&saadc_config, saadc_callback);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_saadc_channel_init(0, &channel_config);
	APP_ERROR_CHECK(err_code);	
	
	err_code = nrf_drv_saadc_sample_convert(0, &value);
	APP_ERROR_CHECK(err_code);
	
	nrf_drv_saadc_uninit();
	
	if(value > 90){
		nrf_gpio_cfg_default(BLUE_PIN);
		
		config0.output_pins[0] = RED_PIN | NRF_DRV_PWM_PIN_INVERTED;
		config0.output_pins[1] = GREEN_PIN | NRF_DRV_PWM_PIN_INVERTED;
		config0.output_pins[2] = BLUE_PIN | NRF_DRV_PWM_PIN_INVERTED;
		config0.output_pins[3] = NRF_DRV_PWM_PIN_NOT_USED;
		
	}else{
		nrf_gpio_cfg_default(BLUE_PIN);
		nrf_gpio_cfg_default(RGB_GND);
	
		
		nrf_gpio_cfg_output(GREEN_PIN);
		nrf_gpio_pin_set(GREEN_PIN);
		
		config0.output_pins[0] = BLUE_PIN | NRF_DRV_PWM_PIN_INVERTED;
		config0.output_pins[1] = RGB_GND | NRF_DRV_PWM_PIN_INVERTED;
		config0.output_pins[2] = RED_PIN | NRF_DRV_PWM_PIN_INVERTED;
		config0.output_pins[3] = NRF_DRV_PWM_PIN_NOT_USED;
		
	}
	
	
	nrf_drv_pwm_init(&m_pwm_rgb, &config0, NULL);
	
	seq_value_rgb[0].channel_0 = RGB_TOP - 200;
	seq_value_rgb[1].channel_0 = RGB_TOP - 0;
	seq_value_rgb[0].channel_1 = RGB_TOP - 200;
	seq_value_rgb[1].channel_1 = RGB_TOP - 0;
	seq_value_rgb[0].channel_2 = RGB_TOP - 200;
	seq_value_rgb[1].channel_2 = RGB_TOP - 0;
	
	
	

	seq_rgb.values.p_individual = seq_value_rgb;
	seq_rgb.length = NRF_PWM_VALUES_LENGTH(seq_value_rgb);
	seq_rgb.repeats             = 0;
  seq_rgb.end_delay           = 0;
//	nrf_drv_pwm_simple_playback(&m_pwm_rgb, &seq_rgb, 0, NRF_DRV_PWM_FLAG_STOP);
	
	
}

void rgb_set(uint16_t red_value, uint16_t green_value, uint16_t blue_value, uint8_t times, uint16_t delay_time)
	{
	
		seq_value_rgb[0].channel_0 = red_value;
	seq_value_rgb[1].channel_0 = 0;
	seq_value_rgb[0].channel_1 = green_value;
	seq_value_rgb[1].channel_1 = 0;
	seq_value_rgb[0].channel_2 = blue_value;
	seq_value_rgb[1].channel_2 = 0;
	seq_rgb.values.p_individual = seq_value_rgb;
	seq_rgb.length = NRF_PWM_VALUES_LENGTH(seq_value_rgb);
	
	if(times > 0)
	{
		seq_rgb.repeats = delay_time;
		nrf_drv_pwm_simple_playback(&m_pwm_rgb, &seq_rgb, (uint16_t)times, NRF_DRV_PWM_FLAG_STOP);	
		
	}
	else
	{
		seq_rgb.repeats = 0;
		nrf_drv_pwm_simple_playback(&m_pwm_rgb, &seq_rgb, 2, NRF_DRV_PWM_FLAG_LOOP);
	}
	
		
}
