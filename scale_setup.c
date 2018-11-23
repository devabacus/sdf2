#include "scale_setup.h"

uint8_t  entry_to_cal = 0;
uint8_t  scale_feedback = 0;

uint32_t activate_code = 313;
//uint32_t hard_reset_code = 131;
uint32_t test_activate_code = 0;

uint32_t reset_attempts_code = 3;
uint32_t test_reset_attempts_code = 0;

uint8_t  scale_type = SCALE_600;

uint32_t adc_need = 0;
uint32_t cor_feedback = COR_PLUS_1;
uint32_t time_feedback = TIME_FEEDBACK;
uint32_t feedback = 0;
uint16_t clock_counter = 0;


uint8_t round_input = 1;
uint8_t test_activate_status = 0;

uint8_t counter_for_set_mode = 0;

uint8_t buf_check_sub_data[11] = "";

uint8_t  admin 							= 0;
uint8_t  root								= 0;
uint32_t admin_pass 				= 0;
uint32_t full_pass   				= 0;
uint32_t archive_pass			  = 0;
uint32_t volume_pass				= 0;
uint32_t config_pass1 = 0;
uint32_t config_pass2 = 0;
uint32_t config_pass3 = 0;
			
uint8_t  thousands						= 0;
uint8_t  hundreds 						= 0;
uint8_t  tens		 				  		= 0;
uint8_t  ones		 							= 0;
uint32_t current_life_counter = 0;

//APP_TIMER_DEF(m_util_timer_id);

void fail_attempt(void)
{
		
		activate_attempts++;
		fds_update_value(&activate_attempts, file_id_c, fds_rk_activate_attempts);
		SEGGER_RTT_printf(0, "activate_attempts = %d\r\n", activate_attempts);
		test_activate_code = 0;
		
		SEGGER_RTT_printf(0, "FAIL\r\n");
		//remote_mode = WORK_MODE;
		rgb_set(50, 0, 0, 1, 3000);
		round_input	= 1;
}

void check_option_pass(void){
					
						if(activate_attempts < ACTIVATE_ATTEMPTS_MAX){
								if (test_activate_code == archive_pass)
							{
								fds_option_status |= (OPTION_ARCHIVE_Msk);
								ble_comm_send_handler("archive activated");
								fds_update_value(&fds_option_status, file_id_c, fds_rk_option_status);
								nrf_delay_ms(50);
								ble_comm_send_handler("o1/2");
							}
							
							else if (test_activate_code == volume_pass)
							{
								fds_option_status |= (OPTION_VOLUME_Msk);
								ble_comm_send_handler("volume activated");
								fds_update_value(&fds_option_status, file_id_c, fds_rk_option_status);
								nrf_delay_ms(50);
								ble_comm_send_handler("o2/2");
							}
							else{
								fail_attempt();
							}
						} else {
								ble_comm_send_handler("attempts is over");
								rgb_set(0,0,50,5,500);
						}
						test_activate_code = 0;
}

void check_config_pass(void){
				if(activate_attempts < ACTIVATE_ATTEMPTS_MAX){
						if (test_activate_code == config_pass2){
							ble_comm_send_handler("profi activated");
							fds_pcb_config = PROFI_CONFIG;
							//ble_comm_send_handler("c1/2");
						}
						else if (test_activate_code == config_pass3){
							ble_comm_send_handler("expert activated");
							fds_pcb_config = EXPERT_CONFIG;
						//	ble_comm_send_handler("c1/3");
						}
						else{
								fail_attempt();
						}
						fds_update_value(&fds_pcb_config, file_id_c, fds_rk_pcb_config);
				}	else {
								ble_comm_send_handler("attempts is over");
								rgb_set(0,0,50,5,500);
						}
						test_activate_code = 0;
}

void check_pass(void){
					  
						if(test_activate_code == demo1 || test_activate_code == demo_passes(demo1,0,3) || test_activate_code == demo_passes(demo1,3,6))
							{
								segtext("demo1\n");
							//	segnum1(test_activate_code);
								test_activate_status = DEMO1; // check activation_status
								cur_cor_max = CORRECT_COUNT_MAX_DEMO1;
								cur_hr_max = WORK_DAYS_MAX_DEMO1*24;
								cur_res_max = RESET_MAX_DEMO1;
								fds_remote_type = REMOTE_ONLY;
								if(test_activate_code == demo_passes(demo1,0,3)){
									fds_remote_type = PHONE_ONLY;
								} else if (test_activate_code == demo_passes(demo1,3,6)){
									fds_remote_type = REMOTE_PHONE;
								}
								
							}
						else if (test_activate_code == demo2 || test_activate_code == demo_passes(demo2,0,3) || test_activate_code == demo_passes(demo2,3,6))
							{
								segtext("demo2\n");
								test_activate_status = DEMO2;
								cur_cor_max = CORRECT_COUNT_MAX_DEMO2;
								cur_hr_max = WORK_DAYS_MAX_DEMO2*24;
								cur_res_max = RESET_MAX_DEMO2;
								fds_remote_type = REMOTE_ONLY;
									if(test_activate_code == demo_passes(demo2,0,3)){
									fds_remote_type = PHONE_ONLY;
										segtext("PHONE_ONLY\n");
										
								} else if (test_activate_code == demo_passes(demo2,3,6)){
									fds_remote_type = REMOTE_PHONE;
										segtext("REM_PHONE\n");
								}
							}
						else if (test_activate_code == demo3 || test_activate_code == demo_passes(demo3,0,3) || test_activate_code == demo_passes(demo3,3,6))
							{
								segtext("demo3\n");
								test_activate_status = DEMO3;
								cur_cor_max = CORRECT_COUNT_MAX_DEMO3;
								cur_hr_max = WORK_DAYS_MAX_DEMO3*24;
								cur_res_max = RESET_MAX_DEMO3;
								fds_remote_type = REMOTE_ONLY;
									if(test_activate_code == demo_passes(demo3,0,3)){
									fds_remote_type = PHONE_ONLY;
										segtext("phone\n");
								} else if (test_activate_code == demo_passes(demo3,3,6)){
									fds_remote_type = REMOTE_PHONE;
									segtext("remphone\n");
								}
							}
						else if (test_activate_code == demo4 || test_activate_code == demo_passes(demo4,0,3) || test_activate_code == demo_passes(demo4,3,6))
							{
								segtext("demo4\n");
								test_activate_status = DEMO4;
								cur_cor_max = CORRECT_COUNT_MAX_DEMO4;
								cur_hr_max = WORK_DAYS_MAX_DEMO4*24;
								cur_res_max = RESET_MAX_DEMO4;
								fds_remote_type = REMOTE_ONLY;
									if(test_activate_code == demo_passes(demo4,0,3)){
									fds_remote_type = PHONE_ONLY;
								} else if (test_activate_code == demo_passes(demo4,3,6)){
									fds_remote_type = REMOTE_PHONE;
								}
							}
						else if (test_activate_code == demo5 || test_activate_code == demo_passes(demo5,0,3) || test_activate_code == demo_passes(demo5,3,6))
							{
								segtext("demo5\n");
								test_activate_status = DEMO5;
								cur_cor_max = CORRECT_COUNT_MAX_DEMO5;
								cur_hr_max = WORK_DAYS_MAX_DEMO5*24;
								cur_res_max = RESET_MAX_DEMO5;
								fds_remote_type = REMOTE_ONLY;
									if(test_activate_code == demo_passes(demo5,0,3)){
									fds_remote_type = PHONE_ONLY;
								} else if (test_activate_code == demo_passes(demo5,3,6)){
									fds_remote_type = REMOTE_PHONE;
								}
							}
							
						else if (test_activate_code == admin_pass)
							{ 
								if(activate_attempts < ACTIVATE_ATTEMPTS_MAX_FOR_ADMIN){
								test_activate_status = ADMIN;
								admin = 1;
								SEGGER_RTT_printf(0, "admin\r\n");
								ble_comm_send_handler("ok");
								} else {
									SEGGER_RTT_printf(0, "you expired whole attempts\r\n");
								}
							}
						
						if(test_activate_code > 0)
							
							{
							
									if(test_activate_status >= activate_status)
											{
										
													rgb_set(0, 50, 0, 1, 3000);	
													
													if(admin)
													{
														nrf_delay_ms(1000);
														rgb_set(0, 0, 50, 1, 1000);	
													}
													
													else if (root)
													{
														nrf_delay_ms(1000);
														rgb_set(0, 0, 50, 2, 1000);	
													}
												
													else if (test_activate_status > 0) 
													{
														if(activate_attempts > ACTIVATE_ATTEMPTS_MAX){
															rgb_set(50,0,0,5,500);
														} else{
															uint32_t ret;
															activate_status = test_activate_status;
															if(fds_remote_type == PHONE_ONLY || fds_remote_type == REMOTE_PHONE){
																ble_comm_send_handler("n7/1");
															}
															SEGGER_RTT_printf(0, "status = %d, code = %d\n\r", activate_status, test_activate_code);
															ble_comm_send_handler("code is valid");
															fds_update_value(&fds_remote_type, file_id_c, fds_rk_remote_type);
															fds_update_value(&activate_status, file_id_c, fds_rk_activate_status);
															fds_update_value(&cur_cor_max, file_id_c, fds_rk_cur_cor_max);
															fds_update_value(&cur_hr_max, file_id_c, fds_rk_cur_cor_max+1);
															start_timer(50);
															
														  
														}
														
													}
												}
																										
										else {
											fail_attempt();
												if(activate_attempts > ACTIVATE_ATTEMPTS_MAX){
															rgb_set(50,0,0,5,500);
											  } 
										   	
											}
							}
							
							else {
									rgb_set(0, 50, 0, 1, 2000);	
									nrf_delay_ms(1000);
									rgb_set(50, 0, 0, 1, 1000);	
							}
							
							  if(test_activate_status){
									remote_mode = CHANGE_SUB_DATA;
									SEGGER_RTT_printf(0, "CHANGE_SUB_DATA\r\n");
								} else {
									segtext("one more attempt\n");
									ble_comm_send_handler("one more attempt");
								}
								test_activate_status = 0;
								test_activate_code = 0;
								round_input	= 1;
}

uint32_t demo_passes(uint32_t demo_test, uint8_t num_st, uint8_t num_end){
	
	uint32_t demo_change = 1;
	for(num_st; num_st < num_end-1 ;num_st++)
	{
		demo_change *= mac_address.addr[num_st];
		//SEGGER_RTT_printf(0, "demo_change = %d\n", demo_change);
	}
			demo_change += mac_address.addr[num_end-1];
	//SEGGER_RTT_printf(0, "mac_address.addr[num_end-1] = %d\n", mac_address.addr[num_end-1]);
			demo_change += demo_test;
	return demo_change;
}


void change_num_cor_but(uint8_t num){
	if(num == 9){
		
		num_cor_buts = 9;
		SEGGER_RTT_printf(0, "but=9\r\n");
		ble_comm_send_handler("n6/9");
		rgb_set(0,50,0,2,1000);
		//nrf_delay_ms(2000);
		//rgb_set(50,0,0,,1000);
		
	} else if (num == 3){
		num_cor_buts = 3;
		SEGGER_RTT_printf(0, "but=3\r\n");
		ble_comm_send_handler("n6/3");
		rgb_set(0,50,0,1,1000);
	}
	
	fds_update_value(&num_cor_buts, file_id, fds_rk_num_cor_but);
	
}
//password option generate (archive and volume)
void generate_option_pass(void){
	archive_pass = demo6+12385;
	volume_pass = demo6+13527;
	//config_pass1 = demo6+14431;
	config_pass2 = demo6+15718;
	config_pass3 = demo6+16097;
	
//	segnum1(archive_pass);
//	segnum1(volume_pass);
}

// this is old admin password
void generate_admin_pass(void)
{
	admin_pass = demo5+power_down_count+life_counter/60; 
	full_pass = (admin_pass + 158)/2;
	
	SEGGER_RTT_printf(0, "admin pass = %d\n\r", admin_pass);
}

// this is new admin password
void generate_admin_pass1(void)
{
	uint32_t activation_num = 0;
	for(uint8_t i = 0; i < 6; i++){
			activation_num += mac_address.addr[i];
		}
	//segtext("activation_num = ");
	segnum1(activation_num);
	demo6 = activation_num * mac_address.addr[5] + mac_address.addr[5];
	admin_pass = demo6+power_down_count+life_counter/60; 
	admin_pass += (power_down_count+(life_counter/60))*2;
	uint32_t add_num = 0;
	
	if(power_down_count%2 == 0){
		add_num += 1389 + (power_down_count + life_counter/60)*4;
	} else {
		add_num += 1238 + (power_down_count + life_counter/60)*3;
	}
	
	if(life_counter/60 % 2 == 0){
		add_num += 2381+life_counter/60*2+power_down_count*3;	
	} else {
		add_num += 3284+power_down_count*3+life_counter/60*2;
	}
	
	admin_pass = admin_pass + add_num;
	
	full_pass = (admin_pass + 158)/2;
	
	
	//SEGGER_RTT_printf(0, "admin pass = %d\n\r", admin_pass);
}

void number_indicate(uint32_t number)
{	
	SEGGER_RTT_printf(0, "number for check = %d\n\r", number);
	
	thousands = number/1000;     															//0
		
	SEGGER_RTT_printf(0, "1000 - %d\n\r", thousands);
	
	hundreds	= (number - thousands*1000)/100;   									//1
	
	SEGGER_RTT_printf(0, "100 - %d\n\r", hundreds);
	
	tens 			= (number - thousands*1000 - hundreds*100)/10;   				
	SEGGER_RTT_printf(0, "10 - %d\n\r", tens);
	
	ones 			= number - thousands*1000 - hundreds*100 - tens*10;   				
	SEGGER_RTT_printf(0, "1 - %d\n\r", ones);	
	
	
		if(thousands>0)
		{
			rgb_set(0,0,50, thousands,1500);
			nrf_delay_ms(thousands*1000);
		}
		
		if(hundreds)
		{
			rgb_set(0,50,0, hundreds,1500);
			nrf_delay_ms(hundreds*1000);
		}
		
		if(tens)
		{
			rgb_set(50,0,0, tens,1500);
			nrf_delay_ms(tens*1000);
		}
		
		if(ones)
		{
			rgb_set(50,50,50, ones,1500);
		}
		
	
}





//void print_array(uint32_t* array){
//	uint8_t i = 0;
//	SEGGER_RTT_printf(0, "array[%d] = %d\n", i, array[i]);
//	i++;
//}

void start_led(void)
	{
		if(activate_status == 1)
			{
				#ifndef DEBUG_MODE
				rgb_set(0, 50, 0, 1, 1500);				
				#endif

				#ifdef DEBUG_MODE
				rgb_set(0, 0, 50, 1, 1500);				
				#endif
				
			}
			
		else if (activate_status > 1)
			{
				#ifndef DEBUG_MODE
				rgb_set(0, 50, 0, 2, 1500);				
				#endif

				#ifdef DEBUG_MODE
				rgb_set(0, 0, 50, 2, 1500);				
				#endif
			}
		else 
			{
				rgb_set(50, 0, 0, 1, 1500);
			}
			
		if(exp_subsriber)
		{
			nrf_delay_ms(1000);
			rgb_set(0, 0, 50, exp_subsriber, 1000);
		}
		
	}


void delete_fds_c(void)
{
	for(uint16_t i = 0x0001; i<=0x000F; i++)
						{
						fds_test_find_and_delete(i, file_id_c);
						}
		rgb_set(50, 0, 0, 1, 3000);
}


void delete_fds(void)
{
	
	for(uint16_t i = 0x0010; i<=0x0021; i++)
						{
						fds_test_find_and_delete(i, file_id);
						}
	rgb_set(0, 50, 0, 1, 3000);
}




void scale_setup(void)
{
  	if(remote_mode != WORK_MODE && pin_in4_is_release && (corr_mode_button == 0))
				{
								remote_mode = WORK_MODE;
							//	offset_perc_state = STANDBY;
								test_activate_code = 0;
								round_input = 1;  // для ввода пароля активации первые три символа из шести
								rgb_set(50, 50, 50, 2, 1000);
						}

	
	  if((remote_mode == CALL_MODE) && !short_delay)
	    	{
				
				if(pin_in1_is_release)
					{
						if(adc_value > 0)
						{
							cal_unload();		
							rgb_set(50, 0, 0, 1, 1000);
						}
					}
					
					if(pin_in1_long_press)
				{
					perc_offset_init_auto();		// in separate file
					rgb_set(0, 0, 50, 3, 300);
				}
					
					
				else if (pin_in2_is_release)	
					{
						if(adc_value>0){
						cal_load();
						rgb_set(0, 50, 0, 1, 1000);
						
						}
						//delete_fds();
						//delete_fds_c();
						//rgb_set(50, 50, 50, 2, 3000);
						
					}
				
				else if (pin_in3_is_release)
					{
						if((adc_value - cal_zero_value)>100)
						{
							define_corr_on();
							rgb_set(0, 0, 50, 1, 1000);
							//remote_mode = WORK_MODE;
						}
					}
					
				else if (pin_in4_long_press)
						{
							{
									remote_mode = CHECK_SUB_DATA;
									rgb_set(50, 0, 0, 1, 3000);
									nrf_delay_ms(500);
									rgb_set(0, 50, 0, 1, 500);
									SEGGER_RTT_printf(0, "CHECK_SUB_DATA\n\r");
								  adc_value = 0;   // for not showing adc besides call mode
							}
							
							// save data to flash
						}
		}
				
		else if (remote_mode == CHECK_SUB_DATA)
				{
					adc_value = 0;
					
					if(pin_in1_long_press)
					{
  					number_indicate(cur_cor_max);				
						//SEGGER_RTT_printf(0, "long1\r\n");						
						
					}
					
					else if(pin_in2_long_press)
					{
						//SEGGER_RTT_printf(0, "long2\r\n");						
						number_indicate(cur_res_max);												
					}
					
					else if(pin_in3_long_press)
					{
						//SEGGER_RTT_printf(0, "long3\r\n");						
						number_indicate(cur_hr_max);
					}
					
					
					else if(pin_in1_is_release)
					{
  					number_indicate(corr_counter);														
					}
					
					else if(pin_in2_is_release)
					{
						number_indicate(power_down_count);				
						current_life_counter = life_counter;	
					}
					
					else if(pin_in3_is_release)
					{
						number_indicate(life_counter/60);
					}
					


					else if (pin_in4_long_press)
					
					{
						remote_mode = STATUS_DEFINE;
						rgb_set(0, 0, 50, 1, 3000);
						current_life_counter = life_counter;
						generate_admin_pass1();
						SEGGER_RTT_printf(0, "current_life_counter = %d\n\r", current_life_counter);
						SEGGER_RTT_printf(0, "STATUS_DEFINE\r\n");
//						SEGGER_RTT_printf(0, "rem1 = %d, ph1 = %d, remph1 = %d\n", demo1, demo_passes(demo1,0,3), demo_passes(demo1,3,6)); 
//						SEGGER_RTT_printf(0, "rem2 = %d, ph2 = %d, remph2 = %d\n", demo2, demo_passes(demo2,0,3), demo_passes(demo2,3,6)); 
//						SEGGER_RTT_printf(0, "rem3 = %d, ph3 = %d, remph3 = %d\n", demo3, demo_passes(demo3,0,3), demo_passes(demo3,3,6)); 
//						SEGGER_RTT_printf(0, "rem4 = %d, ph4 = %d, remph4 = %d\n", demo4, demo_passes(demo4,0,3), demo_passes(demo4,3,6)); 
//						SEGGER_RTT_printf(0, "rem5 = %d, ph5 = %d, remph5 = %d\n", demo5, demo_passes(demo5,0,3), demo_passes(demo5,3,6)); 
						
					}
					
					//return_to_work_mode();
				}

		else if (remote_mode == STATUS_DEFINE)
	  		{
						if(pin_in1_long_press)
							{
								rgb_set(50, 0, 0, activate_status, 1500);
							}
				else if(pin_in1_is_release)
				{
					if(round_input == 1)
					{
							test_activate_code += 100000;
					}
					else 
						{
							test_activate_code += 100;
						}
					rgb_set(0, 0, 50, 1, 500);
					SEGGER_RTT_printf(0, "%d\r\n", test_activate_code);
					nrf_delay_ms(300);
				}
				
				else if(pin_in2_is_release)
				{
					if(round_input == 1)
					{
							test_activate_code += 10000;
					}
					else 
						{
							test_activate_code += 10;
						}
					rgb_set(0, 50, 0, 1, 500);
					SEGGER_RTT_printf(0, "%d\r\n", test_activate_code);
					nrf_delay_ms(300);
				}
				
				else if(pin_in3_is_release)
				{
					if(round_input == 1)
					{
							test_activate_code += 1000;
					}
					else 
						{
							test_activate_code += 1;
						}
					rgb_set(50, 0, 0, 1, 500);
					SEGGER_RTT_printf(0, "%d\r\n", test_activate_code);
					pin_in3_is_release = 0;
					nrf_delay_ms(300);
				}
				
				else if (pin_in4_long_press)
				{
					if(round_input == 1)
						{
							round_input = 2;
							rgb_set(0, 0, 50, 1, 3000);
						
						}
						
					else if (round_input == 2)
					{
						
						if(test_activate_code){
								check_pass();
						}
						else {
							remote_mode = FEEDBACK_SET_MODE;
								rgb_set(0, 0, 50, 1, 1000);
								rgb_set(0, 50, 0, 1, 1000);
								//activate_status = 0;
								SEGGER_RTT_printf(0, "FEEDBACK_SET_MODE\r\n");
						}
						
						
						
					}
			}
		}
				
		else if (remote_mode == CHANGE_SUB_DATA)
				{
					if(admin)
					{
							if(pin_in1_is_release)
									{
										cur_cor_max+=100;
										SEGGER_RTT_printf(0, "cur_cor_max = %d\r\n", cur_cor_max);
										rgb_set(0, 50, 0, 1, 1000);
									}
							
							else if(pin_in2_is_release)
									{
										cur_res_max+=100;
										SEGGER_RTT_printf(0, "cur_res_max = %d\r\n", cur_res_max);
										rgb_set(0, 50, 0, 1, 1000);
									}
							
							else if(pin_in3_is_release)
									{
										cur_hr_max+=100;
										SEGGER_RTT_printf(0, "cur_hr_max = %d\r\n", cur_hr_max);
										rgb_set(0, 50, 0, 1, 1000);
									}
									
							else if(pin_in1_long_press)
									{
										cur_cor_max+=1000;
										SEGGER_RTT_printf(0, "cur_cor_max = %d\r\n", cur_cor_max);
										rgb_set(0, 0, 50, 1, 1000);
									}
							
							else if(pin_in2_long_press)
									{
										cur_res_max+=1000;
										SEGGER_RTT_printf(0, "cur_res_max = %d\r\n", cur_res_max);
										rgb_set(0, 0, 50, 1, 1000);
									}
							
							else if(pin_in3_long_press)
									{
										cur_hr_max+=1000;
										SEGGER_RTT_printf(0, "cur_hr_max = %d\r\n", cur_hr_max);
										rgb_set(0, 0, 50, 1, 1000);
									}
									
							
					}
					
					if (pin_in4_long_press)
					
					{
						if(admin)
						{
							fds_update_value(&cur_cor_max, file_id_c, fds_rk_cur_cor_max);
							fds_update_value(&cur_hr_max, file_id_c, fds_rk_cur_cor_max+1);
							fds_update_value(&cur_res_max, file_id_c, fds_rk_cur_cor_max+2);	
						}
						
						remote_mode = RESET_COUNTERS;
						rgb_set(0, 50, 0, 1, 3000);
						//activate_status = 0;
						SEGGER_RTT_printf(0, "RESET_COUNTERS\r\n");
					}
					
					//return_to_work_mode();
				}	


		else if (remote_mode == RESET_COUNTERS){
					
					
						if(admin)
					{
							if(pin_in1_long_press)
									{
										corr_counter = 1;
										phone_cor_counter = 1;
										SEGGER_RTT_printf(0, "corr_counter = %d\r\n", corr_counter);
										
										rgb_set(0, 50, 0, 1, 2000);
										
									}
							
							else if(pin_in2_long_press)
									{
										power_down_count = 1;
										SEGGER_RTT_printf(0, "power_down_count = %d\r\n", power_down_count);
										rgb_set(0, 50, 0, 1, 2000);
										
									}
							
							else if(pin_in3_long_press)
									{
										life_counter = 60;
										SEGGER_RTT_printf(0, "life_counter = %d\r\n", life_counter);
										
										rgb_set(0, 50, 0, 1, 2000);
										
									}
									
								}
										
					if (pin_in4_long_press)
							
							{
								fds_update_value(&corr_counter, file_id_c, fds_rk_corr_counter);
								fds_update_value(&power_down_count, file_id_c, fds_rk_power_down);	
								nrf_delay_ms(50);
								fds_update_value(&life_counter, file_id_c, fds_rk_clock);
								fds_update_value(&phone_cor_counter, file_id_c, fds_rk_phone_cor_counter);
								nrf_delay_ms(50);
								if(!activate_status){
											activate_status = 1;
											fds_update_value(&activate_status, file_id_c, fds_rk_activate_status);
										}
								remote_mode = FEEDBACK_SET_MODE;
								rgb_set(0, 0, 50, 1, 3000);
								//activate_status = 0;
								SEGGER_RTT_printf(0, "FEEDBACK_SET_MODE\r\n");
							}
					
								
									
					}
				

		else if (remote_mode == FEEDBACK_SET_MODE)
	    	{
					if(pin_in1_is_release)
					{
						cor_feedback++;
						rgb_set(50, 0, 0, 1, 500);
						correct_value(cor_feedback);
						
					}
					
					if(pin_in2_is_release)
							if(percent_cor_mode == COR_SIMPLE)
								
								{
												
								}
								
								else if (percent_cor_mode == CORR_OFFSET_KG)

								{
								
								}
								
								if(pin_in1_long_press)
					{
						time_feedback+=500;
						
						if(time_feedback > 3000)
						{
							time_feedback = 500;
						}
						
						rgb_set(0,0,50,time_feedback/500,1000);
						
					}
					
					else if (pin_in3_is_release)
					{
						cor_feedback--;
						rgb_set(50, 0, 0, 1, 500);
						correct_value(cor_feedback);
					}
					
					else if (pin_in2_long_press)
					{
						switch(feedback)
						{
							case 0:
								feedback = 1; // cor_feedback
								rgb_set(0, 50, 0, 1, 1000);
								break;
							
							case 1:
								feedback = 2; // corrects
								rgb_set(0, 50, 0, 2, 1000);
								break;
							
							case 2:
								feedback = 0;
								rgb_set(50, 50, 50, 1, 1000);
								break;
							
						}
						
						fds_update_value(&feedback, file_id, fds_rk_feedback);
						
						SEGGER_RTT_printf(0, "feedback = %d\r\n", feedback);
					}
					
					else if (pin_in3_long_press)
					{
						if(num_cor_buts == 3)
						{
							change_num_cor_but(9);
							//rgb_set(0,50,0,2,1000);
						}
						else if (num_cor_buts == 9)
						{
							change_num_cor_but(3);
						}
					}
					
					else if (pin_in3_is_release)
					{
						set_weight(11);
					}
					
					else if (pin_in4_long_press)
					{
						fds_update_value(&cor_feedback, file_id, fds_rk_cor_feedback);
						fds_update_value(&time_feedback, file_id, fds_rk_time_feedback);
						remote_mode = FACTORY_MODE;
						rgb_set(50, 0, 0, 1, 1000);
						rgb_set(50, 0, 50, 1, 1000);
						SEGGER_RTT_printf(0, "FACTORY_MODE\r\n");
					}
					
		}
		
		else if (remote_mode == FACTORY_MODE)
				{
					if(pin_in1_is_release)
					{
						//delete_fds();						
						rgb_set(50, 50, 50, 1, 3000);
					}
					
					if(pin_in1_long_press)
						
					{
						if(admin){
							activate_attempts = 1;
							fds_update_value(&activate_attempts, file_id_c, fds_rk_activate_attempts);
							segtext("reset attempts\n");
							rgb_set(0,50,0,3,500);
						}
						
						
					}
					
					if(pin_in2_long_press)
					{
//						delete_fds();
//						delete_fds_c();
						rgb_set(50, 50, 50, 2, 3000);
					}
					
					
					else if (pin_in2_is_release)
					{
					}
					else if (pin_in3_long_press)
					{
					}
					else if (pin_in4_long_press)
					{
						remote_mode = WORK_MODE;
						rgb_set(0, 0, 50, 1, 3000);
						SEGGER_RTT_printf(0, "WORK_MODE\r\n");
					}
					//return_to_work_mode();
				}
		else if (remote_mode == WORK_MODE)
				{
					 //SEGGER_RTT_printf(0, "I entry\r\n");
					if(pin_in1_is_set && pin_in4_long_press)
					
						{
							rgb_set(50,0,0,1,1000);
							nrf_delay_ms(200);
							rgb_set(0,50,0,1,1000);
							nrf_delay_ms(200);
							rgb_set(0,0,50,1,1000);
							remote_mode = CALL_MODE;
							time_to_sleep = TIME_TO_SLEEP;
							SEGGER_RTT_printf(0, "CAL MODE\r\n");
							short_delay = 0;
						}
						
					else if ((pin_in4_long_press) && (counter_for_set_mode < 5))
					{
						//remote_mode = CORR_SETUP_MODE;
						//counter_for_set_mode = 0;
					}
					
						if(pin_in4_is_release)
						{		
								if(!pin_in4_long_press)
								{
									counter_for_set_mode++;
//									SEGGER_RTT_printf(0, "counter_for_set_mode = %d \n\r", counter_for_set_mode);
								}
																
								else if (counter_for_set_mode > 5)
								{
									counter_for_set_mode = 0;
								}
						}
					
			  		if (pin_in4_long_press)
						{
							if(counter_for_set_mode == 3)
							{
									SEGGER_RTT_printf(0, "COR_AUTO_MODE\r\n");
							if(correct_mode == COR_MANUAL)
								{
									
									correct_mode = COR_AUTO;
									time_to_sleep = TIME_TO_SLEEP;
									rgb_set(0,50,0,2,1000);
									correct_value(cor_feedback);
									stop_timer();
									start_timer(time_feedback);
									fds_update_value(&correct_mode, file_id, fds_rk_corr_mode);
								}
								else if (correct_mode == COR_AUTO)
									{
										adc_value = 0;
										rgb_set(50,0,0,2,1000);
										time_to_sleep = 0;
										correct_mode = COR_MANUAL;
										correct_value(cor_feedback);
										stop_timer();
										start_timer(time_feedback);
										fds_update_value(&correct_mode, file_id, fds_rk_corr_mode);
									}
									// send info about correct_mode via ble
									set_send_cor_mode(correct_mode);
							}
							
						  else if(counter_for_set_mode == 5)
								
								{
									set_up_mode_indicate();			
									counter_for_set_mode = 0;
								}
						}
						if(pin_in1_is_release || pin_in2_is_release || pin_in3_is_release)
						{
							counter_for_set_mode = 0;
						}
				}						
				

}
