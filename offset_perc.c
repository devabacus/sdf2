#include "offset_perc.h"
#include "scale_setup.h"

uint8_t first_setup_entry = 1; // var for detect first entry into settings
uint32_t kg_offset_ar[9];   // here you store our kg plus values for our 9 buttons
uint32_t cur_kg_value = OFFSET_PLUS_KG_START; // temporary var for store current correct value
int cur_perc_value = COR_PERC_1;

int cur_discrete_counter = 1;

offset_perc_state_t offset_perc_state = STANDBY;
adc_offset_kg_t adc_offset_kg 				= ADC_IDLE;

static timer_num_state_t timer_num_state = TIMER_PULL_MANUAL;
static uint16_t fds_rk_perc_offset1 = 0x0022;
uint16_t perc_arr[100];
uint16_t kg_arr[100];
uint8_t index_for_arrs = 0;

uint8_t timer_setup_flag = 0;

void timer_setup(timer_num_state_t timer_num, uint16_t time_fire)
{
			app_timer_stop(m_timer_offset_perc);
			app_timer_start(m_timer_offset_perc, APP_TIMER_TICKS(time_fire), NULL);
			timer_num_state = timer_num;
	}


void adc_offset_init()
{
//SEGGER_RTT_printf(0, "adc_offset_init()\n\r");
	offset_perc_state = PERC_INCREMENT_AUTO;
	timer_setup(TIMER_ADC, TIME_FIRE_ADC1);
	adc_offset_kg = ADC_INC_KG1;
	
}

void gather_offset_data()
{
	
}


void adc_offset_kg_find()
{ 
	int different_adc = cal_zero_value - adc_value;  // we have minus value some time 
	switch(adc_offset_kg)
	{
		
		case ADC_IDLE:
			break;
			
		case ADC_INC_KG1:
			
		  correct_value(++cur_kg_value);
		 //SEGGER_RTT_printf(0, "%d - %d = %d (%d)\n\r", cal_zero_value, adc_value, cal_zero_value-adc_value, cal_coef/2);
		
			if(different_adc < (int)cal_coef/2)
				{
					 
					if (offset_perc_state == PERC_INCREMENT_AUTO)
					{
		    		//SEGGER_RTT_printf(0, "adc_offset_kg = ADC_FOUND_KG_PERC\n\r");
  					 adc_offset_kg = ADC_FOUND_KG_PERC;
						 rgb_set(0, 50, 0, 1, 2000);	
						 app_timer_stop(m_timer_offset_perc);
						 offset_perc_state = STANDBY;
						 adc_offset_kg 		 = ADC_IDLE;	 
						 timer_setup_flag = 0;
						 rgb_set(0, 50, 0, 1, 200);
					}
					 
					 else if(offset_perc_state == PERC_OFFSET_INIT)
					 {
						 // gather_offset_data();
						 kg_arr[index_for_arrs] = cur_kg_value;
						 adc_offset_kg = ADC_INC_PERC;
						//SEGGER_RTT_printf(0, "kg_arr[%d] = %d\n", index_for_arrs, kg_arr[index_for_arrs]);
						 if(cur_discrete_counter == 3)  
						 {
											// debug --------------------------------------------
										//SEGGER_RTT_printf(0, "perc    				 kg\n");
										  for(uint8_t c = 0; c < 100; c++)
											{
												perc_arr[c] = COR_PERC_1;
												kg_arr[c]	  = OFFSET_PLUS_KG_START;
											//SEGGER_RTT_printf(0, "%d    				%d\n",perc_arr[c], kg_arr[c]);
												nrf_delay_ms(1);
											}
											//-----------------------------------------------------
									
							app_timer_stop(m_timer_offset_perc);
							offset_perc_state = STANDBY;
							adc_offset_kg 		 = ADC_IDLE;	 
						 }
					 }
				}
				
			else if ((different_adc < cal_coef*10)&&(!timer_setup_flag))
			{
			//SEGGER_RTT_printf(0, "change timer fire value\n\r");
				timer_setup_flag = 1;
				timer_setup(TIMER_ADC, TIME_FIRE_ADC2);
			}
		
				break;
		case ADC_INC_PERC:
				{
				//SEGGER_RTT_printf(0, "percent increment\n");
				//SEGGER_RTT_printf(0, "%d - %d = %d ... %d*%d = %d\n", cal_zero_value, adc_value, cal_zero_value-adc_value, cur_discrete_counter, cal_coef, cur_discrete_counter*cal_coef);
					correct_value(++cur_perc_value);
				//SEGGER_RTT_printf(0, "if(different_adc > cur_discrete_counter*(int)cal_coef)\n");
				//SEGGER_RTT_printf(0, "if(%d>%d*(int)%d)\n",different_adc,cur_discrete_counter,cal_coef);
					
					if(different_adc > cur_discrete_counter*(int)cal_coef)
						
					{
						//increase percent due to change scale value for one discrete
					//SEGGER_RTT_printf(0, "perc_arr[cur_discrete_counter-1] = cur_perc_value;\n");
					//SEGGER_RTT_printf(0, "perc_arr[%d] = %d;\n", cur_discrete_counter-1, cur_perc_value);
						perc_arr[cur_discrete_counter-1] = cur_perc_value;
						adc_offset_kg = ADC_INC_KG1;
					}
				}	
				break;
	}
}

void debug_display(uint8_t num)
{
		switch(num)
		{
			case 1:
					//SEGGER_RTT_printf(0, "arr[%d] = %d, cur_kg_value = %d\n\r", corr_mode_button-1, kg_offset_ar[corr_mode_button-1],cur_kg_value);
				break;
		}
}

void offset_fds_kg_init_data(void)
{
					//index = corr_mode_button - 1;
					for(uint8_t my_index = 0; my_index<9; my_index++)
						{ 	
							kg_offset_ar[my_index] = OFFSET_PLUS_KG_START;
							fds_init_flash(kg_offset_ar + my_index, file_id, fds_rk_perc_offset1 + my_index);
						}
	}	

void offset_fds_kg_get_data(void)
{
				for(uint8_t my_index = 0; my_index<9; my_index++)
						{ 	
							fds_get_data(kg_offset_ar + my_index, file_id, fds_rk_perc_offset1 + my_index);
						}
}

void offset_fds_update()
{
	// update fds value for kg offset percent
//SEGGER_RTT_printf(0, "offset_fds_update()\n\r");
	kg_offset_ar[corr_mode_button-1] = cur_kg_value;
	cur_kg_value = OFFSET_PLUS_KG_START;
	debug_display(1);
	fds_update_value(kg_offset_ar+corr_mode_button-1, file_id, fds_rk_perc_offset1 + corr_mode_button-1);
}

void timer_offset_perc_handler(void *p_context)
{
		switch(timer_num_state)
		{
			case TIMER_ADC:
				adc_offset_kg_find();
				break;
			
			case TIMER_PULL_MANUAL:
				if((pin_in1_long_press || pin_in2_long_press || pin_in3_long_press) && PERC_INCREMENT_MANUAL)
				{
					correct_value(++cur_kg_value);
					rgb_set(50, 0, 0, 1, 200);
				//SEGGER_RTT_printf(0, "cur_kg_value = %d\n\r", cur_kg_value);
				}
				break;
		}	
}

void change_but_offset_level()
{
	
//SEGGER_RTT_printf(0, "change_but_offset_level()\n\r");
		if(kg_offset_ar[corr_mode_button-1] < 1000)
					{
							  cur_kg_value = OFFSET_PLUS_KG_START;
								rgb_set(0, 0, 50, 1, 1500);
								nrf_delay_ms(200);
								rgb_set(0, 50, 0, 1, 500);
					}
					
					else
					{
								cur_kg_value = OFFSET_MINUS_KG_START;
								rgb_set(0, 0, 50, 1, 1500);
								nrf_delay_ms(200);
								rgb_set(0, 50, 0, 1, 500);
					}
		 correct_value(cur_kg_value);			
}

void offset_perc_define(void)
{
	//SEGGER_RTT_printf(0, "offset percent mode\n\r");
		rgb_set(0, 0, 50, 3, 500);
}

void perc_offset_init_auto(void)
{
	//SEGGER_RTT_printf(0, "adc_offset_init()\n\r");
	  for(uint8_t c = 0; c < 100; c++)
		{
			perc_arr[c] = COR_PERC_1;
			kg_arr[c]	  = OFFSET_PLUS_KG_START;
		}
		
		app_timer_create(&m_timer_offset_perc, APP_TIMER_MODE_REPEATED, timer_offset_perc_handler);
	  offset_perc_state = PERC_OFFSET_INIT;
		adc_offset_kg = ADC_INC_PERC;
		timer_setup(TIMER_ADC, TIME_FIRE_ADC2);
}


void offset_kg_init()
{
//SEGGER_RTT_printf(0, "offset_kg_init()\n\r");
	app_timer_create(&m_timer_offset_perc, APP_TIMER_MODE_REPEATED, timer_offset_perc_handler);
		if(cur_kg_value == OFFSET_PLUS_KG_START+1)
		{
			cur_kg_value = OFFSET_PLUS_KG_START;
		//SEGGER_RTT_printf(0, "reset kg_offset before\n\r");
		}
		else
		{
			cur_kg_value = kg_offset_ar[corr_mode_button-1];
		}

	
	remote_mode = CORR_OFFSET_KG;
	rgb_set(0, 0, 50, 1, 1500);
	nrf_delay_ms(200);
	rgb_set(0, 50, 0, 1, 500);
	
	debug_display(1);
	
//SEGGER_RTT_printf(0, "offset for button = %d\n\r", corr_mode_button);
	
}

void offset_kg_setting ()
{
	// buttons 7,8,9
	
	if(pin_in1_is_release || pin_in2_is_release || pin_in3_is_release)
	{
		offset_perc_state = STANDBY;
	}
	
	if(corr_mode_button >= 7) 
	{
	  	if(pin_in3_is_release)
			{
				rgb_set(0, 50, 0, 1, 500);
				correct_value(++cur_kg_value);
				offset_perc_state = STANDBY;
			}
			
			else if (pin_in3_long_press)
			{
				timer_setup(TIMER_PULL_MANUAL, TIME_FIRE_PULL);
				offset_perc_state = PERC_INCREMENT_MANUAL;
			}
			
			else if (pin_in2_is_release)
			{
				rgb_set(0, 0, 50, 1, 500);
				correct_value(--cur_kg_value);
			}
			
			else if(pin_in1_is_release)
			{
				change_but_offset_level();
			}
			else if (pin_in1_long_press)
			{
				adc_offset_init();
				rgb_set(0, 50, 0, 3, 500);
			}
			
	}
	// buttons 4,5,6
	else if (corr_mode_button >= 4)
	{
			if(pin_in2_is_release)
			{
				rgb_set(0, 50, 0, 1, 500);
				correct_value(++cur_kg_value);
				offset_perc_state = STANDBY;
				
				
			}
			
			else if (pin_in2_long_press)
			{
				timer_setup(TIMER_PULL_MANUAL, TIME_FIRE_PULL);
				offset_perc_state = PERC_INCREMENT_MANUAL;
			}
			
			
			else if (pin_in1_is_release)
			{
				correct_value(--cur_kg_value);
				rgb_set(0, 0, 50, 1, 500);
			}
			
			else if(pin_in3_is_release)
			{
				change_but_offset_level();
			}
			
				else if (pin_in3_long_press)
			{
				adc_offset_init();
				rgb_set(0, 50, 0, 3, 500);
			}
			
	}
	// buttons 1,2,3
	else if (corr_mode_button >= 1)
	{
			if(pin_in1_is_release)
			{
				rgb_set(0, 50, 0, 1, 500);
				correct_value(++cur_kg_value);
				offset_perc_state = STANDBY;
				
			}
			
			else if (pin_in1_long_press)
			{
				timer_setup(TIMER_PULL_MANUAL, TIME_FIRE_PULL);
				offset_perc_state = PERC_INCREMENT_MANUAL;
			}
			
			
			else if (pin_in3_is_release)
			{
				correct_value(--cur_kg_value);
				rgb_set(0, 0, 50, 1, 500);
			}
			
			else if(pin_in2_is_release)
			{
				change_but_offset_level();
			}
			
			else if (pin_in2_long_press)
			{
				adc_offset_init();
				rgb_set(0, 50, 0, 3, 500);
			}
			
			
			
			else if (pin_in2_long_press)
			{
				offset_perc_define();
				//SEGGER_RTT_printf(0, "start_timer\n\r");
			}
			else if (pin_in3_long_press)  // must to do
			{
//				remote_mode = CORR_BUT_MODE;
//				rgb_set(0, 0, 50, 2, 500);
			}
			
	}
}

void offset_kg_define_manual()
{
//SEGGER_RTT_printf(0, "offset_kg_define_manual()\n\r");
	if(reset_by_change_but_level)
	{
		cur_kg_value = OFFSET_PLUS_KG_START;
		correct_value(cur_kg_value);
		reset_by_change_but_level = 0;
	}
	offset_kg_setting();
	
	if(pin_in4_is_release)
	{
			rgb_set(50, 50, 50, 1, 500);
			correct(0, 0, 0);
			init_corr_values();
			stop_timer_02s();  // stop timer used for fast correct setup
			remote_mode = WORK_MODE;
			corr_mode_button = 0;
			first_setup_entry = 1;
			cur_kg_value = OFFSET_PLUS_KG_START;
			offset_perc_state = STANDBY;
			app_timer_stop(m_timer_offset_perc);
			timer_setup_flag = 0;
	}

	else if (pin_in4_long_press)
		{
		//SEGGER_RTT_printf(0,"pin_in4_long_press, save\r\n");
			
			rgb_set(50, 50, 50, 2, 3000);
			save_corr_values();
			offset_fds_update();
			//while(write_flag == 0);
			stop_timer_02s(); // stop timer used for fast correct setup
			remote_mode = WORK_MODE;
			corr_mode_button = 0;
			correct(0, 0, 0);
			first_setup_entry = 1;
			cur_kg_value = OFFSET_PLUS_KG_START;
			offset_perc_state = STANDBY;
			app_timer_stop(m_timer_offset_perc);
			timer_setup_flag = 0;

		}	
}


void offset_kg_cor_work()
{
			
				if(reset_by_change_but_level)
			{
				//SEGGER_RTT_printf(0, "reset_by_change_but_level = 1\n\r");
				cur_kg_value = OFFSET_PLUS_KG_START+1;  // just for understand in offset init that we reset value
				correct_value(cur_kg_value);
				reset_by_change_but_level = 0;
			}
			
			
			if(remote_mode == WORK_MODE)
			{
				//SEGGER_RTT_printf(0, "offset_kg_cor_work() for work\n\r");
				correct_value(kg_offset_ar[current_but-1]);
				corr_counter--;

				//SEGGER_RTT_printf(0, "arr[%d] = %d, cur_kg_value = %d\n\r", current_but-1, kg_offset_ar[current_but-1],cur_kg_value);
			}
			else if (remote_mode == CORR_BUT_MODE)
			{
				if(first_setup_entry)
				{
					//SEGGER_RTT_printf(0, "fist_setup_entry\n\r");
					cur_kg_value = kg_offset_ar[corr_mode_button-1];
					first_setup_entry = 0;
				}
				//SEGGER_RTT_printf(0, "offset_kg_cor_work() for setting\n\r");
				debug_display(1);
				correct_value(cur_kg_value);
			}
		}
