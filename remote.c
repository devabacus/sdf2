#include "remote.h"

APP_TIMER_DEF(m_timer_remote);
APP_TIMER_DEF(m_timer_remote02);
APP_TIMER_DEF(m_timer_remote05);
APP_TIMER_DEF(m_timer_adc);

uint8_t exp_subsriber = 0;

uint8_t button_event = 0;

uint8_t pin_in1_is_set = 0;
uint8_t pin_in1_is_release = 0;
uint8_t pin_in1_long_press = 0;

uint8_t pin_in2_is_set = 0;
uint8_t pin_in2_is_release = 0;
uint8_t pin_in2_long_press = 0;

uint8_t pin_in3_is_set = 0;
uint8_t pin_in3_is_release = 0;
uint8_t pin_in3_long_press = 0;

uint8_t pin_in4_is_set = 0;
uint8_t pin_in4_is_release = 0;
uint8_t pin_in4_long_press = 0;

uint8_t push_count1 = 0;
uint8_t push_count2 = 0;
uint8_t push_count3 = 0;
uint8_t push_count4 = 0;

uint8_t PIN_IN_1 = 10;
uint8_t PIN_IN_2 =  9;
uint8_t PIN_IN_3 =  7;
uint8_t PIN_IN_4 =  8;


uint8_t short_delay = 0;
uint16_t correct_need = 1250;
uint16_t time_to_sleep = 0;

uint8_t first_entry = 0;

uint32_t cur_cor_max = CORRECT_COUNT_MAX_DEMO;
uint32_t cur_hr_max = WORK_DAYS_MAX_DEMO*24;
uint32_t cur_res_max = RESET_MAX_DEMO;

uint8_t remote_mode = WORK_MODE;

void set_up_mode_indicate()
{
			correct(0,0,0);
			rgb_set(50,0,0,1,1000);
			nrf_delay_ms(200);
			rgb_set(0,50,0,1,1000);
			nrf_delay_ms(200);
			rgb_set(0,0,50,1,1000);
			remote_mode = CALL_MODE;
			time_to_sleep = TIME_TO_SLEEP;
			SEGGER_RTT_printf(0, "CAL MODE\r\n");
}

void reset_activate()
{
		activate_status = 0;
		fds_update_value(&activate_status, file_id_c, fds_rk_activate_status);
}


void test_expired(void)
{
	if(activate_status <= DEMO)
	{
		if(corr_counter >= CORRECT_COUNT_MAX_DEMO)
		{
			reset_activate();
			SEGGER_RTT_printf(0, "correct_demo_exp\r\n");
		}
		else if (life_counter > WORK_HOURS_MAX_DEMO*60)    
		{
			reset_activate();
			SEGGER_RTT_printf(0, "hours_demo_exp\r\n");
		}
		else if (power_down_count >= RESET_MAX_DEMO)
		{
			reset_activate();
			SEGGER_RTT_printf(0, "reset_demo_exp\r\n");
		}
	}
	
	else if(activate_status > DEMO)
	{
		if(corr_counter >= cur_cor_max)
		{
			exp_subsriber = 1;
		}
		else if (power_down_count >= cur_res_max)
		{
			//because of there was fail with activate reset max. (50). So we turn checking off
			exp_subsriber = 2;
		}
		else if (life_counter >= cur_hr_max*60)
		{
			exp_subsriber = 3;
		}
		
		if(exp_subsriber)
		{
			SEGGER_RTT_printf(0, "exp_subsriber = %d, activate_status = %d\r\n", exp_subsriber, activate_status);	
		}
	}
}

void reset_long_press_flags(void)
{
	pin_in1_long_press = 0;
	pin_in2_long_press = 0;
	pin_in3_long_press = 0;
	pin_in4_long_press = 0;
}

void reset_press_flags()
{
	pin_in1_is_set = 0;
	pin_in2_is_set = 0;
	pin_in3_is_set = 0;
	pin_in4_is_set = 0;
}

void reset_release_flags1()
{
	pin_in2_is_release= 0;
	pin_in3_is_release = 0;
	pin_in4_is_release= 0;
}

void reset_release_flags2()
{
	pin_in1_is_release= 0;
	pin_in3_is_release = 0;
	pin_in4_is_release= 0;
}

void reset_release_flags3()
{
	pin_in1_is_release= 0;
	pin_in2_is_release= 0;
	pin_in4_is_release= 0;
}

void reset_release_flags4()
{
	pin_in1_is_release= 0;
	pin_in2_is_release= 0;
	pin_in3_is_release = 0;
}

void reset_release_flags()
{
	pin_in1_is_release= 0;
	pin_in2_is_release= 0;
	pin_in3_is_release = 0;
	pin_in4_is_release = 0;
}

void flag_analize(void)
{
	SEGGER_RTT_printf(0, "%d,%d,%d,\r\n", pin_in1_is_set, pin_in1_is_release, pin_in1_long_press);
	SEGGER_RTT_printf(0, "%d,%d,%d,\r\n", pin_in2_is_set, pin_in2_is_release, pin_in2_long_press);
	SEGGER_RTT_printf(0, "%d,%d,%d,\r\n", pin_in3_is_set, pin_in3_is_release, pin_in3_long_press);
	SEGGER_RTT_printf(0, "%d,%d,%d,\r\n", pin_in4_is_set, pin_in4_is_release, pin_in4_long_press);
	SEGGER_RTT_printf(0, "--------\r\n");
}


void timer_2s_handler(void *p_context)
{
	button_event = 1;
	
	 			
			if(pin_in1_is_set == 1)
			{
				pin_in1_long_press = 1;
				//SEGGER_RTT_printf(0, "long 1\n\r");
				if(remote_mode == WORK_MODE)
				{
					remote_mode = CORR_BUT_MODE;
					// when we go into buttons setup first time we also neet to increase correct counter.
					// because someone can use it like work mode 
					corr_counter_inc();
				}

				else 
				{
						scale_setup();
				}

				
			}
			else if (pin_in2_is_set == 1)
			{
				pin_in2_long_press = 1;
				//SEGGER_RTT_printf(0, "long 2\n\r");
				
					if(remote_mode == WORK_MODE)
				{
					remote_mode = CORR_BUT_MODE;
					corr_counter_inc();
				}

				else 
				{
						scale_setup();
				}

			}
			else if (pin_in3_is_set == 1)
			{
				pin_in3_long_press = 1;
				//SEGGER_RTT_printf(0, "long 3\n\r");
	  		
				if(remote_mode == WORK_MODE)
				{
					remote_mode = CORR_BUT_MODE;
					corr_counter_inc();
				}

				else 
				{
						scale_setup();
				}

			}
			else if (pin_in4_is_set == 1)
			{
				pin_in4_long_press = 1;
				//SEGGER_RTT_printf(0, "long 4\n\r");
				if(remote_mode != CORR_BUT_MODE)
				{
						scale_setup();
				}
			}
			reset_release_flags();
			buttons_handle_setup();
				
	 if (correct_mode == COR_AUTO && scale_feedback)	
	 {
		 correct(0,0,0);
	 }
	 	 
//	 freeze_auto_cor = 0;
//	 //stop_timer_2s();
//	 segtext("timer fire\n");
//	 SEGGER_RTT_printf(0, "freeze_auto_cor= %d\n", freeze_auto_cor);
	
}

void timer_02s_handler(void *p_context)
{
	if(pin_in1_long_press || pin_in2_long_press || pin_in3_long_press)
	{
		switch(corr_mode_button)
			{
				case CORR_MODE_1_1:
					corr_1_1++;
					rgb_set(50, 0, 0, 1, 200);
					correct_value(corr_1_1);
				break;
					
				case CORR_MODE_1_2:
					corr_1_2++;
					rgb_set(0, 50, 0, 1, 200);
					correct_value(corr_1_2);
				break;
				
				case CORR_MODE_1_3:
					corr_1_3++;
					rgb_set(0, 0, 50, 1, 200);
					correct_value(corr_1_3);
				break;
				
				case CORR_MODE_2_1:
					corr_2_1++;
					rgb_set(50, 0, 0, 1, 200);
					correct_value(corr_2_1);
				break;
					
				case CORR_MODE_2_2:
					corr_2_2++;
					rgb_set(0, 50, 0, 1, 200);
					correct_value(corr_2_2);
				break;
				
				case CORR_MODE_2_3:
					corr_2_3++;
					rgb_set(0, 0, 50, 1, 200);
					correct_value(corr_2_3);
				break;
				
				case CORR_MODE_3_1:
					corr_3_1++;
					rgb_set(50, 0, 0, 1, 200);
					correct_value(corr_3_1);
				break;
					
				case CORR_MODE_3_2:
					corr_3_2++;
					rgb_set(0, 50, 0, 1, 200);
					correct_value(corr_3_2);
				break;
				
				case CORR_MODE_3_3:
					corr_3_3++;
					rgb_set(0, 0, 50, 1, 200);
					correct_value(corr_3_3);
				break;
				
			}
		
	}
		else if (pin_in2_long_press)
		{
			
		}
		
		else if ((start_average_adc)&&(adc_value>0))
		{
			find_average_adc();
		}
}


void timer_05s_handler(void *p_context)
{
		correct(0,0,0); //reset cor feedback
		fds_update_value(&cur_res_max, file_id_c, fds_rk_cur_cor_max+2);	
		//segtext("hello\n");
		
}

void timer_adc_handler(void *p_context)
{
	if(adc_value <= adc_need)
	{
		correct_need++;
		correct_value(correct_need);
		SEGGER_RTT_printf(0, "cor = %d, adc = %d\r\n", correct_need, adc_value);
	}
	else 
	{
		
		SEGGER_RTT_printf(0, "ok, cor = %d, adc = %d\r\n", correct_need, adc_value);
		app_timer_stop(m_timer_adc);
		correct_need = 0;
		adc_need = 0;
	}
}

void timer_remote_butts_init(void)
{
	app_timer_init();
	app_timer_create(&m_timer_remote, APP_TIMER_MODE_SINGLE_SHOT, timer_2s_handler); 
	app_timer_create(&m_timer_remote02, APP_TIMER_MODE_REPEATED, timer_02s_handler); 
	app_timer_create(&m_timer_remote05, APP_TIMER_MODE_SINGLE_SHOT, timer_05s_handler); 
	app_timer_create(&m_timer_adc, APP_TIMER_MODE_REPEATED, timer_adc_handler); 
	
}

void start_timer_adc(void)
{
	app_timer_start(m_timer_adc, APP_TIMER_TICKS(200), NULL);
}

void start_timer_2s(void)
{
	app_timer_start(m_timer_remote, APP_TIMER_TICKS(2000), NULL);
}

void stop_timer_2s(void)
{
	app_timer_stop(m_timer_remote);
}



void start_timer(uint16_t num)
	{
		 app_timer_start(m_timer_remote05, APP_TIMER_TICKS(num), NULL);
	}


void start_timer_02s(void)
{
	app_timer_start(m_timer_remote02, APP_TIMER_TICKS(100), NULL);
}

void stop_timer_02s(void)
{
	app_timer_stop(m_timer_remote02);
}

void stop_timer(void)
{
	app_timer_stop(m_timer_remote05);
}




void start_timer_05s(void)
{
	app_timer_start(m_timer_remote05, APP_TIMER_TICKS(500), NULL);
}



void in_pin_handler1(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	//SEGGER_RTT_printf(0, "button 1\n");
		if(nrf_drv_gpiote_in_is_set(PIN_IN_1))
		{
			 pin_in1_is_set = 1;
			 start_timer_2s();
			 reset_long_press_flags();
			 reset_release_flags1();
		}
		else //if (!nrf_drv_gpiote_in_is_set(PIN_IN_1))
		{
				app_timer_stop(m_timer_remote);
			pin_in1_is_set = 0;
			if(!pin_in1_long_press)
			{
				pin_in1_is_release++;
			}
			reset_long_press_flags();
			button_event = 1;	  
			buttons_handle();
			buttons_handle_setup();
			app_timer_stop(m_timer_remote02);
			scale_setup();
		}
}

void in_pin_handler2(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	
	//SEGGER_RTT_printf(0, "button 2\n");
			if(nrf_drv_gpiote_in_is_set(PIN_IN_2))
		{
			 start_timer_2s();
			 pin_in2_is_set = 1;
			 reset_long_press_flags();
			 reset_release_flags2();
		}
		else
		{
			app_timer_stop(m_timer_remote);
			pin_in2_is_set = 0;
			if(!pin_in2_long_press)
			{
				pin_in2_is_release++;
			}
			reset_long_press_flags();
			app_timer_stop(m_timer_remote02);
			button_event = 1;
			if(fds_pcb_config != NEWBIE_CONFIG){
				buttons_handle();
			}
			else
			{
				segtext("only first button");
				rgb_set(50, 0, 0, 3, 1000);
			}
			buttons_handle_setup();
			scale_setup();
		}
}

void in_pin_handler3(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
//	SEGGER_RTT_printf(0, "button 3\n");
		if(nrf_drv_gpiote_in_is_set(PIN_IN_3))
		{
			 start_timer_2s();
			 pin_in3_is_set = 1;
			 reset_long_press_flags();
				reset_release_flags3();
		}
		else
		{
			app_timer_stop(m_timer_remote);
			pin_in3_is_set = 0;
			if(!pin_in3_long_press)
			{
				pin_in3_is_release++;
			}
			reset_long_press_flags();
			app_timer_stop(m_timer_remote02);
			button_event = 1;
			if(fds_pcb_config != NEWBIE_CONFIG){
				buttons_handle();
			}
			else
			{
				rgb_set(50, 0, 0, 3, 1000);
				segtext("only first button");
			}
			buttons_handle_setup();
			scale_setup();
		}
		
		

}

void in_pin_handler4(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{ 
	
		if(nrf_drv_gpiote_in_is_set(PIN_IN_4))
		{
			 pin_in4_is_set = 1;
			 start_timer_2s();
			 reset_long_press_flags();
			 reset_release_flags4();
		}
		else
		{
				if(pin_in1_is_release)
				{
							set_up_mode_indicate();
				}
			
				else
							{
								app_timer_stop(m_timer_remote);
								pin_in4_is_set = 0;
							if(!pin_in4_long_press)
							{
								pin_in4_is_release++;
								//start_timer_05s();
								scale_setup();
								first_entry = 0;
								stop_timer_02s();
						
							}
			
							button_event = 1;
							buttons_handle();
							buttons_handle_setup();
						//	#ifndef DEBUG_MODE
							test_expired();
						//	#endif
							
							SEGGER_RTT_printf(0, "reset\n\r");
							}
			}
}

void in_pin_handler5(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
	segtext("8 changed\n");
}

void nrf_define_test_pin(void){
		nrf_drv_gpiote_init();		
		nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
		in_config.pull = NRF_GPIO_PIN_PULLUP;
		nrf_drv_gpiote_in_init(8, &in_config, in_pin_handler4);
		nrf_drv_gpiote_in_event_enable(8, true);	
}

void nrf_gpiote(void)
	{
	//	nrf_drv_gpiote_init();
		nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
		in_config.pull = NRF_GPIO_PIN_PULLUP;		
		
		nrf_drv_gpiote_in_init(PIN_IN_1, &in_config, in_pin_handler1);
		nrf_drv_gpiote_in_init(PIN_IN_2, &in_config, in_pin_handler2);
		nrf_drv_gpiote_in_init(PIN_IN_3, &in_config, in_pin_handler3);
		nrf_drv_gpiote_in_init(PIN_IN_4, &in_config, in_pin_handler4);		
		
		nrf_drv_gpiote_in_event_enable(PIN_IN_1, true);	
		nrf_drv_gpiote_in_event_enable(PIN_IN_2, true);	
		nrf_drv_gpiote_in_event_enable(PIN_IN_3, true);	
		nrf_drv_gpiote_in_event_enable(PIN_IN_4, true);	
		
		timer_remote_butts_init();
	}
	
void define_pins(void){
			if(nrf_drv_gpiote_in_is_set(8)){
			
					PIN_IN_1 =	14;
					PIN_IN_2 =	13;
					PIN_IN_3 =	11;
					PIN_IN_4 =	12;
				
					CORR_KG_PLUS = 9; 
          CORR_KG_MINUS = 7;
          CORR_PERCENT = 10;				
		} 
		
}
	
	
