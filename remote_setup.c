#include "remote_setup.h"

uint8_t corr_mode_button = 0;

//start adc value when we go into button setup mode, later we decrease this value from autocor min value
uint32_t adc_value_start = 0;
void save_adc_cor_value(void);
//test
void change_but_level(uint32_t* value)

{
				if(0 < *value && *value <= 1000) // plus correct
					{
						*value = COR_PLUS_1;  // current value 
						correct(0,0,0);
						correct_value(*value);
						rgb_set(0, 50, 0, 1, 1000);
					}
					else if (1000 < *value && *value <= 2000) // minus correct
					{
						*value = COR_PERC_1;
						reset_by_change_but_level = 1;
						correct(0,0,0);
						correct_value(*value);  
						rgb_set(0, 0, 50, 1, 1000);
					}
					else if (*value > 2000)  // percent correct
					{
						*value = COR_MINUS_1;
						correct(0,0,0);
						correct_value(*value);
						rgb_set(50, 0, 0, 1, 1000);
					}
}

void save_corr_values(void)
{
		if(corr_mode_button <= 3)
		{
				fds_update_value(&corr_1_1, file_id, fds_rk_cor1);
				fds_update_value(&corr_1_2, file_id, fds_rk_cor1+1);
				fds_update_value(&corr_1_3, file_id, fds_rk_cor1+2);
				
		}
		
		else if (corr_mode_button <= 6)
		{
				fds_update_value(&corr_2_1, file_id, fds_rk_cor1+3);
				fds_update_value(&corr_2_2, file_id, fds_rk_cor1+4);
				fds_update_value(&corr_2_3, file_id, fds_rk_cor1+5);	
		}
		
		else if (corr_mode_button <= 9)

		{
				fds_update_value(&corr_3_1, file_id, fds_rk_cor1+6);
				fds_update_value(&corr_3_2, file_id, fds_rk_cor1+7);
				fds_update_value(&corr_3_3, file_id, fds_rk_cor1+8);
		}
		// иначе компенсация не сохраняется
		//save_adc_cor_value();
}


void buttons_handle_setup(void)  
{
	if (remote_mode == CORR_OFFSET_KG)
		{
				offset_kg_define_manual();
		}
	
	else if(remote_mode == CORR_BUT_MODE)
	{
		if((fds_remote_type != PHONE_ONLY) || admin){

			 switch (corr_mode_button)
				{

					 case 0:
						 
							if(pin_in1_long_press)
								{
									corr_mode_button = CORR_MODE_1_1;
									SEGGER_RTT_printf(0, "1_1\n\r");
									rgb_set(50, 0, 0, 1, 3000);
									correct_value(corr_1_1);
								
								}
							if (pin_in2_long_press)
								{
									corr_mode_button = CORR_MODE_2_1;
									SEGGER_RTT_printf(0, "2_1\n\r");
									rgb_set(0, 50, 0, 1, 3000);
									correct_value(corr_2_1);
								}
							if (pin_in3_long_press)
								{
									corr_mode_button = CORR_MODE_3_1;
									SEGGER_RTT_printf(0, "3_1\n\r");
									rgb_set(0, 0, 50, 1, 3000);
									correct_value(corr_3_1);
								}
							if (pin_in4_long_press)
								{
									//SEGGER_RTT_printf(0,"save values\r\n");
									rgb_set(50, 50, 50, 0, 0);
									//save_corr_values();
									remote_mode = WORK_MODE;
									corr_mode_button = 0;
								}
								
								if(adc_value) adc_value_start= adc_value;
								segtext("adc_value_start ");
								segnum1(adc_value_start);
							//	reset_long_press_flags();
							 
					 break;
					
					 case CORR_MODE_1_1:
					
							if(pin_in1_is_release) // change correct value by one
							{
								
								corr_1_1++;
								correct_value(corr_1_1);
								rgb_set(50, 0, 0, 1, 500);
							}
							else if (pin_in1_long_press) // fast correct mode
							{
								start_timer_02s(); // fast correct way
							}
							if(pin_in2_is_release)  //switch correct mode (minus, plus or percent)
							{
									change_but_level(&corr_1_1);
							}
							if(pin_in3_is_release)
							{
								corr_1_1--;
								correct_value(corr_1_1);
								rgb_set(50, 0, 0, 1, 500);
							}
							
							if (pin_in3_long_press && (corr_1_1 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}
							
							if(pin_in2_long_press && (num_cor_buts > 3))  //switch button
							{
								  correct(0,0,0);
									
									corr_mode_button = CORR_MODE_1_2;
									rgb_set(0, 50, 0, 1, 1000);
									correct_value(corr_1_2);
							}
												
							break;

					 case CORR_MODE_1_2:
						 
							
							if(pin_in1_is_release)
							{
								corr_1_2++;
								correct_value(corr_1_2);
								rgb_set(0, 50, 0, 1, 500);
							}
							else if (pin_in1_long_press)
							{
								start_timer_02s(); // fast correct way
							}
							
							if(pin_in2_is_release)
							{
									change_but_level(&corr_1_2);
							}
							
							if(pin_in3_is_release)
							{
								corr_1_2--;
								correct_value(corr_1_2);
								rgb_set(0, 50, 0, 1, 500);
							}
							if(pin_in2_long_press)
							{
									corr_mode_button = CORR_MODE_1_3;
									rgb_set(0, 0, 50, 1, 1000);
									correct_value(corr_1_3);
							}
							if (pin_in3_long_press && (corr_1_2 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}
												
							break;
							
							
						case CORR_MODE_1_3:
					 
						
							if(pin_in1_is_release)
							{
								corr_1_3++;
								correct_value(corr_1_3);
								SEGGER_RTT_printf(0, "correct 1_3 \r\n");
								rgb_set(0, 0, 50, 1, 500);
							}
							else if (pin_in1_long_press)
							{
								start_timer_02s(); // fast correct way
							}
							
							if(pin_in2_is_release)
							{
									change_but_level(&corr_1_3);
							}
							
							if(pin_in3_is_release)
							{
								corr_1_3--;
								correct_value(corr_1_3);
								rgb_set(0, 0, 50, 1, 500);
							}		
							if(pin_in2_long_press)
							{
									corr_mode_button = CORR_MODE_1_1;
									rgb_set(50, 0, 0, 1, 1000);
									correct_value(corr_1_1);
							}
											
							if (pin_in3_long_press && (corr_1_3 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}

							break;
							
					 case CORR_MODE_2_1:
					
							if(pin_in2_is_release) // change correct value by one
							{
								SEGGER_RTT_printf(0, "corr_2_1 - %d \r\n", corr_2_1);
								corr_2_1++;
								correct_value(corr_2_1);
								rgb_set(50, 0, 0, 1, 500);
							}
							else if (pin_in2_long_press) // fast correct mode
							{
								start_timer_02s(); // fast correct way
							}
							if(pin_in3_is_release)  //switch correct mode (minus, plus or percent)
							{
									change_but_level(&corr_2_1);
							}
							
							if(pin_in1_is_release) // change correct value by one
							{
								SEGGER_RTT_printf(0, "corr_2_1 - %d \r\n", corr_2_1);
								corr_2_1--;
								correct_value(corr_2_1);
								rgb_set(50, 0, 0, 1, 500);
							}
							if(pin_in3_long_press && (num_cor_buts > 3))  //switch button
							{
								  correct(0,0,0);
									corr_mode_button = CORR_MODE_2_2;
									rgb_set(0, 50, 0, 1, 1000);
									correct_value(corr_2_2);
							}
												
							if (pin_in1_long_press && (corr_2_1 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}
								
							break;

					 case CORR_MODE_2_2:
						 
							
							if(pin_in2_is_release)
							{
								corr_2_2++;
								correct_value(corr_2_2);
								SEGGER_RTT_printf(0, "corr_2_2 - %d \r\n", corr_2_2);
								rgb_set(0, 50, 0, 1, 500);
							}
							else if (pin_in2_long_press)
							{
								start_timer_02s(); // fast correct way
							}
							
							if(pin_in3_is_release)
							{
									change_but_level(&corr_2_2);
							}
							
								if(pin_in1_is_release)
							{
								corr_2_2--;
								correct_value(corr_2_2);
								rgb_set(0, 50, 0, 1, 500);
							}		

							if(pin_in3_long_press)
							{
									corr_mode_button = CORR_MODE_2_3;
									rgb_set(0, 0, 50, 1, 1000);
									correct_value(corr_2_3);
							}
												
							if (pin_in1_long_press && (corr_2_2 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}
							break;
							
							
						case CORR_MODE_2_3:
					 
						
							if(pin_in2_is_release)
							{
								corr_2_3++;
								correct_value(corr_2_3);
								SEGGER_RTT_printf(0, "corr_2_3 - %d \r\n", corr_2_3);
								rgb_set(0, 0, 50, 1, 500);
							}
							else if (pin_in2_long_press)
							{
								start_timer_02s(); // fast correct way
							}
							
							if(pin_in3_is_release)
							{
									change_but_level(&corr_2_3);
							}
							
							
							if(pin_in1_is_release)
							{
								corr_2_3--;
								correct_value(corr_2_3);
								SEGGER_RTT_printf(0, "corr_2_3 - %d \r\n", corr_2_3);
								rgb_set(0, 0, 50, 1, 500);
							}
							
							if(pin_in3_long_press)
							{
									corr_mode_button = CORR_MODE_2_1;
									rgb_set(50, 0, 0, 1, 1000);
									correct_value(corr_2_1);
							}
											
						  if (pin_in1_long_press && (corr_2_3 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}
							break;
							
							// setup third button in correct mode 
							
							case CORR_MODE_3_1:
					
							if(pin_in3_is_release) // change correct value by one
							{
								SEGGER_RTT_printf(0, "corr_3_1 - %d \r\n", corr_3_1);
								corr_3_1++;
								correct_value(corr_3_1);
								rgb_set(50, 0, 0, 1, 500);
							}
							else if (pin_in3_long_press) // fast correct mode
							{
								start_timer_02s(); // fast correct way
							}
							if(pin_in1_is_release)  //switch correct mode (minus, plus or percent)
							{
									change_but_level(&corr_3_1);
							}
							

							if(pin_in2_is_release) // change correct value by one
							{
								corr_3_1--;
								correct_value(corr_3_1);
								rgb_set(50, 0, 0, 1, 500);
							}							

							if(pin_in1_long_press && (num_cor_buts > 3))//switch button
							{
								  correct(0,0,0);
									corr_mode_button = CORR_MODE_3_2;
									rgb_set(0, 50, 0, 1, 1000);
									correct_value(corr_3_2);
							}
												
							if (pin_in2_long_press && (corr_3_1 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}
							break;

					 case CORR_MODE_3_2:
						 
							if(pin_in3_is_release)
							{
								corr_3_2++;
								correct_value(corr_3_2);
								SEGGER_RTT_printf(0, "corr_3_2 - %d \r\n", corr_3_2);
								rgb_set(0, 50, 0, 1, 500);
							}
							else if (pin_in3_long_press)
							{
								start_timer_02s(); // fast correct way
							}
							
							if(pin_in1_is_release)
							{
									change_but_level(&corr_3_2);
							}
							
							
							if(pin_in2_is_release)
							{
								corr_3_2--;
								correct_value(corr_3_2);
								SEGGER_RTT_printf(0, "corr_3_2 - %d \r\n", corr_3_2);
								rgb_set(0, 50, 0, 1, 500);
							}							

							if(pin_in1_long_press)
							{
									corr_mode_button = CORR_MODE_3_3;
									rgb_set(0, 0, 50, 1, 1000);
									correct_value(corr_3_3);
							}
												
							if (pin_in2_long_press && (corr_3_2 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}
							break;
							
						case CORR_MODE_3_3:
						
							if(pin_in3_is_release)
							{
								corr_3_3++;
								correct_value(corr_3_3);
								SEGGER_RTT_printf(0, "corr_3_3 - %d \r\n", corr_3_3);
								rgb_set(0, 0, 50, 1, 500);
							}
							else if (pin_in3_long_press)
							{
								start_timer_02s(); // fast correct way
							}
							
							if(pin_in1_is_release)
							{
									change_but_level(&corr_3_3);
							}
							
							
								if(pin_in2_is_release)
							{
								corr_3_3--;
								correct_value(corr_3_3);
								SEGGER_RTT_printf(0, "corr_3_3 - %d \r\n", corr_3_3);
								rgb_set(0, 0, 50, 1, 500);
							}
							
							
							if(pin_in1_long_press)
							{
									corr_mode_button = CORR_MODE_3_1;
									rgb_set(50, 0, 0, 1, 1000);
									correct_value(corr_3_1);
							}
											
						  if (pin_in2_long_press && (corr_3_3 > 2000)) // offset kg define for percent
							{
								offset_kg_init();  // offset_perc.c 
							}
							break;
							
				}
			}				
				
				if(pin_in4_is_release)
				{
					rgb_set(50, 50, 50, 1, 500);
					correct(0, 0, 0);
					init_corr_values();
					stop_timer_02s();  // stop timer used for fast correct setup
					remote_mode = WORK_MODE;
					corr_mode_button = 0;
					SEGGER_RTT_printf(0,"reset corr\r\n");
					SEGGER_RTT_printf(0, "%d,%d\n\r", remote_mode, corr_mode_button);
				}
				
				if (pin_in4_long_press)
						{
							SEGGER_RTT_printf(0,"save\r\n");
							SEGGER_RTT_printf(0,"switch to rem_mode %d\r\n", remote_mode);
							rgb_set(50, 50, 50, 2, 3000);
							save_corr_values();
							stop_timer_02s(); // stop timer used for fast correct setup
							remote_mode = WORK_MODE;
							corr_mode_button = 0;
							correct(0, 0, 0);
						}
				}
	

}
	
