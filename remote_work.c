#include "remote_work.h"


uint32_t cor_value_auto = 0;
uint32_t cur_comp_cor = 0;
uint32_t last_cor_value_auto = 0;
uint32_t correct_mode = COR_MANUAL;
uint8_t cor_set = 0;
uint32_t current_correct = 0;
uint32_t current_correct_counter = 0;
uint32_t just_reset = 0;
uint32_t current_adc_value = 0;
uint8_t unload_weight = 0;
uint32_t cal_turn_off = 0;		
uint32_t adc_value_max = 0;
uint32_t adc_value_last = 0;
uint32_t adc_cor = 0;
uint8_t adc_change_counter = 0;
uint8_t adc_direct = 0;
uint8_t adc_over = 0;
uint32_t adc_value_lowest = 0;
uint8_t adc_monoton_change = 0;
uint8_t cor_activate_time = 0;
uint32_t freeze_auto_cor = 0;
uint8_t uart_unload_detect = 0;
uint32_t uart_change_counter = 0;
uint8_t uart_direct = 0;
int uart_last = 0;

current_but_t current_but;

void init_corr_values(void)
{
	fds_get_data(&corr_1_1, file_id, fds_rk_cor1);
		fds_get_data(&corr_1_2, file_id, fds_rk_cor1+1);
		fds_get_data(&corr_1_3, file_id, fds_rk_cor1+2);
		fds_get_data(&corr_2_1, file_id, fds_rk_cor1+3);
		fds_get_data(&corr_2_2, file_id, fds_rk_cor1+4);
		fds_get_data(&corr_2_3, file_id, fds_rk_cor1+5);
		fds_get_data(&corr_3_1, file_id, fds_rk_cor1+6);
		fds_get_data(&corr_3_2, file_id, fds_rk_cor1+7);
		fds_get_data(&corr_3_3, file_id, fds_rk_cor1+8);
}

// вызываем постоянно в цикле в main, если активен авторежим COR_AUTO = COR_AUTO
void cor_auto_handle(void)
{
	//если кнопки находятся в рабочем режиме, установлен порог и нажата кнопка корректировки
	if((remote_mode == WORK_MODE)&&(cor_value_auto))
	{
		//SEGGER_RTT_printf(0, "adc_value = %d\n", adc_value);
		//если ацп изменяется в одну сторону то считаем сколько раз
		//и пишем в adc_change_counter. При смене направления счетчик сбрасывается
				
		if(adc_value != adc_value_last)
			{			
			// если ацп увеличивается
				if(adc_value > adc_value_last)
					{
						//а до этого уменьшалось или это первое изменение				
						if(adc_direct == 2 || !adc_direct){
							adc_change_counter = 0;
							adc_direct = 1;	
						}
					}
			// если ацп уменьшается
			else 
					{
						//а до этого увеличивалось или это первое изменение
						if(adc_direct == 1 || !adc_direct){
							adc_change_counter = 0;
							adc_direct = 2;	
						}
							if(cor_set)
								{
									if(!adc_value_lowest){
									adc_value_lowest = adc_value;
										}
									if(adc_value_lowest > adc_value)
										{
											adc_value_lowest = adc_value;
											//SEGGER_RTT_printf(0, "adc_value_lowest = %d\n", adc_value_lowest);
										}
								}
//						if((adc_change_counter >= 3) && (adc_value_last*10 <= adc_value)){
//							adc_over = 1;
//							SEGGER_RTT_printf(0, "adc_over = %d\n", adc_over);
//						}
								
					}
					adc_change_counter++;
					if(adc_change_counter > 3){
						adc_monoton_change = 1;
						//SEGGER_RTT_printf(0, "adc_change_counter = %d\n", adc_change_counter);
					}
//					if(adc_value_last)
//					{
//							if(adc_value_last > (adc_value*100) || adc_value_last < adc_value*100)
//								{
//								adc_value = adc_value_last;
//								}
//							else 
//								{
//								
//								}
//				  }
					adc_value_last = adc_value;
			  }
				if(adc_value_lowest)
				{
						if((adc_value > (adc_value_lowest*10)) && cor_set)
						{
							adc_over++;
						}
				}
		// ацп меняется в одном направлении
		if(1)//adc_change_counter > 0)
			
			{
				cal_turn_off = cal_turn_on;
				//каждый раз при нарастании ацп сохраняем максимальное значение
				//если корректировка установлена и значение ацп больше чем порог + 20%
				//то пороги включение и отключения равны
//				if(adc_value > adc_value_max && adc_value < 16000000)
//					{
//				
//							adc_value_max = adc_value;
//							//SEGGER_RTT_printf(0, "adc_value_max = %d\n", adc_value_max);
//							// if weight with correct value more than cal_turn_on * 20% we can safety equal cal_turn_on and cal_turn_off
//							
////							if((adc_value_max > (cal_turn_on + (cal_turn_on*20/100))) && cor_set)
////								{
////									cal_turn_off = cal_turn_on;
////								//	SEGGER_RTT_printf(0, "calc_turn_off changed = %d\n", cal_turn_off);
////								}
//					
//					}
				//если авторежим работает с порта
				if(fds_uart_automode && uart_active)
				{
					if(uart_weight > uart_last){
						uart_last = uart_weight;
						if(uart_direct == 2){
							uart_change_counter = 0;
							uart_direct = 0;
						}
						uart_change_counter++;
						if(uart_change_counter >= 5){
							uart_direct = 1;
							
							
						}
					} else if (uart_weight < uart_last){
						uart_last = uart_weight;
						uart_change_counter++;
						if(uart_direct == 1){
							uart_change_counter = 0;
							uart_direct = 0;
						}
						if(uart_change_counter >= 5){
							uart_direct = 2;
							uart_change_counter = 0;
						}
				}
					//SEGGER_RTT_printf(0, "uart_direct = %d\n", uart_direct);							
					if((uart_weight > (int)cal_turn_on) && (!cor_set || (last_cor_value_auto != cor_value_auto)))
					//if((uart_weight > cal_turn_on) && !cor_set && (uart_weight > 0))
					{
//						uint8_t uart_unload_detect = 0;
//						uint32_t uart_change_counter = 0;
//						uint8_t uart_direct = 0;
						//SEGGER_RTT_printf(0, "freeze_auto_cor= %d\n", freeze_auto_cor);							
						//if(abs(freeze_auto_cor - clock_counter) > 2){
								cor_set = 1;
								last_cor_value_auto = cor_value_auto;
								SEGGER_RTT_printf(0, "clock_counter = %d, freeze_auto_cor = %d\n", clock_counter, freeze_auto_cor);							
								correct_value(cor_value_auto);
								if(cur_comp_cor > 0 && cor_value_auto > 2000)
										{
											correct_value(cur_comp_cor);
										}
								segtext("activate\n");
								SEGGER_RTT_printf(0, "uart_weight = %d, corr %d, turn_on = %d set\n\r", uart_weight, cor_value_auto, cal_turn_on);
						//}							
					}
					else if ((uart_weight < (int)cal_turn_on) &&  cor_set)
					//else if (((uart_weight < cal_turn_on) || (uart_direct == 2)) && uart_weight && cor_set)
					{
							//SEGGER_RTT_printf(0, "uart_weight = %d, corr %d, turn_on = %d reset\n\r", cal_tu);
							correct(0,0,0);
							//if(uart_direct == 2) rgb_set(50,50,50, 2, 500);
							cor_set = 0;
							SEGGER_RTT_printf(0, "uart_weight = %d, corr %d, turn_on = %d reset\n\r", uart_weight, cor_value_auto, cal_turn_on);
							freeze_auto_cor = clock_counter;
							if(ble_active){
							ble_comm_send_handler("n2/0");
								ble_comm_send_handler("n2/0");
								ble_comm_send_handler("n2/0");
							} 
					}
				}
				
				// если авторежим работает через ацп
				else 
				{
								if((adc_monoton_change) && (adc_value > cal_turn_on) && ((!cor_set)|| (last_cor_value_auto != cor_value_auto)))
								{
									// получаем значение ацп настроенной кнопки
									adc_over = 0;
									adc_value_lowest = 0;
									adc_cor = adc_vals_ar[current_but-1];
									SEGGER_RTT_printf(0, "adc_cor = %d\n", adc_cor);
									if(cal_turn_on > adc_cor)
										{
											//определяем выключение корректировки
											//cal_turn_off = (cal_turn_on - adc_cor-adc_cor*20/100);
											
										} 
										
										SEGGER_RTT_printf(0, "-----------------ON-------------------\n");
										
										SEGGER_RTT_printf(0, "adc_value = %d, cal_turn_on = %d, adc_change_counter = %d, adc_direct = %d\n", adc_value, cal_turn_on, adc_change_counter, adc_direct);
										//SEGGER_RTT_printf(0, "adc_over = %d, adc_value_lowest = %d, adc_value = %d, direct = %d, counter = %d, adc_value_max = %d\n", adc_over, adc_value_lowest, adc_value, adc_direct, adc_change_counter, adc_value_max);
									//SEGGER_RTT_printf(0, "cal_turn_on = %d\n", cal_turn_on);
									//SEGGER_RTT_printf(0, "calc_turn_off = %d\n", cal_turn_off);
									
									last_cor_value_auto = cor_value_auto;
									correct_value(cor_value_auto);
									if(cur_comp_cor > 0 && cor_value_auto > 2000)
										{
											correct_value(cur_comp_cor);
										}
									cor_set = 1;
									
									SEGGER_RTT_printf(0, "adc_value = %d, corr %d set\n\r", adc_value, cor_value_auto);
									//
								}
								
									else if (((adc_value < cal_turn_off) || (adc_over>3)) && cor_set)  // если возникне ошибка тоже нужно сбросить adc_value > 10000000
									{
										SEGGER_RTT_printf(0, "-----------------OFF-------------------\n");
										SEGGER_RTT_printf(0, "adc_value = %d, cal_turn_off = %d, adc_over = %d, adc_change_counter = %d, direct = %d\n", adc_value, cal_turn_on, adc_over, adc_change_counter, adc_direct);		
										if (adc_value > 0 && ble_active) {
										 ble_comm_send_handler("n2/0");
											 ble_comm_send_handler("n2/0");
											 ble_comm_send_handler("n2/0");
										}
									//SEGGER_RTT_printf(0, "adc_over = %d, adc_value_lowest = %d, adc_value = %d, direct = %d, counter = %d\n", adc_over, adc_value_lowest, adc_value, adc_direct, adc_change_counter);
						//			if(current_adc_value >= adc_value)
						//			{
						//				unload_weight = 1;
						//				current_adc_value = adc_value;
						//			}
			//						if(adc_value < cal_zero_value)
			//							{
			//								SEGGER_RTT_printf(0, "adc_value < cal_zero_value\n");
			//							}
			//						SEGGER_RTT_printf(0, "cal_turn_on = %d\n", cal_turn_on);
			//						SEGGER_RTT_printf(0, "calc_turn_off = %d\n", cal_turn_off);
						//			else 
						//			{
						//				unload_weight = 0;
						//			}
									SEGGER_RTT_printf(0, "reset\n");
										
									if(num_cor_buts == 9){
										corr_counter_inc();
									}
									correct(0,0,0);
									adc_monoton_change = 0;
									adc_value_last = 0;
									cor_set = 0;
									adc_over = 0;
									adc_value_max = 0;
									SEGGER_RTT_printf(0, "adc_value = %d, corr %d reset\n\r", adc_value, cor_value_auto);
									ble_comm_send_handler("n2/0");
									ble_comm_send_handler("n2/0");
									//ble_comm_send_handler("n2/0");
									nrf_delay_ms(1000);
									}
				
						}
				
			
			}
}
		
}

void buttons_handle(void)
{
	if(remote_mode == WORK_MODE)
	{
		
		if (pin_in1_is_release)
		{
			if(num_cor_buts > 3)
			{
				switch(pin_in1_is_release)
				{
				case 1:
						rgb_set(50, 0, 0, 1, 500);
						current_correct = corr_1_1;
						current_but = BUT_1_1;
						//SEGGER_RTT_printf(0, "corr_1_1 = %d\n", corr_1_1);
						SEGGER_RTT_printf(0, "cur_but = %d\n\r", current_but);
				break;

				case 2:
						rgb_set(50, 0, 0, 2, 500);
						current_correct = corr_1_2;
				current_but = BUT_1_2;
				break;
				
				case 3:
					rgb_set(50, 0, 0, 3, 500);
					current_correct = corr_1_3;
					pin_in1_is_release = 0;
					current_but = BUT_1_3;
				break;
				}
			}
			
			else
			{
					rgb_set(50, 0, 0, 1, 500);
					current_correct = corr_1_1;
					current_but = BUT_1_1;
					SEGGER_RTT_printf(0, "cur_but = %d\n\r", current_but);
					SEGGER_RTT_printf(0, "cur_correct = %d\n\r", current_correct);
					SEGGER_RTT_printf(0, "adc_value_but = %d\n\r", adc_vals_ar[current_but-1]);
			}
			
		}
		
		else if (pin_in2_is_release)
		{
			if(num_cor_buts > 3)
			{
			switch(pin_in2_is_release) // how many times was pushed 2 button
			{
				case 1:
					rgb_set(0, 50, 0, 1, 500);
					current_correct = corr_2_1;
					current_but = BUT_2_1;
				break;

				case 2:
					rgb_set(0, 50, 0, 2, 500);
					current_correct = corr_2_2;
					current_but = BUT_2_2;
				break;
				
				case 3:
					rgb_set(0, 50, 0, 3, 500);
					current_correct = corr_2_3;
					pin_in2_is_release = 0;
					current_but = BUT_2_3;
				break;
			}
		}
		else
		{
					rgb_set(0, 50, 0, 1, 500);
					current_correct = corr_2_1;
					current_but = BUT_2_1;
					SEGGER_RTT_printf(0, "cur_but = %d\n\r", current_but);
					//SEGGER_RTT_printf(0, "adc_value_but = %d\n\r", adc_vals_ar[current_but-1]);
		}			
		
		}
	  else if (pin_in3_is_release)
		{
			
			if(num_cor_buts > 3)
			{
				switch(pin_in3_is_release)
			{
				case 1:
					rgb_set(0, 0, 50, 1, 500);
					current_correct = corr_3_1;
					current_but = BUT_3_1;
				break;

				case 2:
					rgb_set(0, 0, 50, 2, 500);
					current_correct = corr_3_2;
					current_but = BUT_3_2;
				break;
				
				case 3:
					rgb_set(0, 0, 50, 3, 500);
					current_correct = corr_3_3;
					pin_in3_is_release = 0;
					current_but = BUT_3_3;
				break;
			}
			}
			
			else
			{
					rgb_set(0, 0, 50, 1, 500);
					current_correct = corr_3_1;
					pin_in3_is_release = 0;
					current_but = BUT_3_1;
					SEGGER_RTT_printf(0, "cur_but = %d\n\r", current_but);
				SEGGER_RTT_printf(0, "adc_value_but = %d\n\r", adc_vals_ar[current_but-1]);
			}
		}
		else if (pin_in4_is_release )
		{
			if(current_correct && num_cor_buts == 9){
				corr_counter_inc();
			}
			rgb_set(50, 50, 50, 1, 500);
			if(ble_active) {
				ble_comm_send_handler("n1/0");
				ble_comm_send_handler("n1/0");
				ble_comm_send_handler("n1/0");
			} 
			correct(0,0,0);
			freeze_auto_cor = 0;
			cor_value_auto = 0;
			current_correct = 0;
			adc_value_max = 0;
			adc_over = 0;
			adc_value_last = 0;
			adc_monoton_change = 0;
			
		}
		
		//	#ifndef DEBUG_MODE
				test_expired();
		//	#endif
				
		//correct_activate(current_correct);
		if((fds_remote_type != PHONE_ONLY) || admin)
	//	if((true) || admin)
		{										
			if(correct_mode == COR_MANUAL)
			{
				//if((fds_pcb_config == NEWBIE_CONFIG)&&())
					correct_value(current_correct);
					
				//	SEGGER_RTT_printf(0, "manual %d\n\r", current_correct);
			}
			
		  else if (correct_mode == COR_AUTO)	
			{
						cor_value_auto = current_correct;
						
				//notife phone that we are waiting of min weight value for activate correct
//						
//						if(feedback == 1)
//						{
//								correct_value(cor_feedback);
//								stop_timer();
//								start_timer(time_feedback);
//						}
//						
//						else if (feedback == 2)
//						{
//							correct_value(current_correct);
//							stop_timer();
//							start_timer(time_feedback);
//						}
					//SEGGER_RTT_printf(0, "auto %d\n\r", cor_value_auto);	
			}
		
		}
		
	}
}
