#include "weight.h"


uint32_t cal_zero_value = 0;
uint32_t cal_load_value = 0;
uint32_t cal_turn_on = 0;
uint8_t  start_average_adc = 0;
uint8_t  count_average_adc = 0;
uint32_t average_adc = 0;
float average_adc_float = 0;
uint32_t adc_array[AVERAGE_ADC_TIMES];
uint32_t adc_array_filtered[AVERAGE_ADC_TIMES-NUM_EXCEED_MEMBERS];
float weight_stable_arr[10];
uint32_t cal_coef = 0;
float cal_coef_float = 0;
float weight = 0;
uint32_t num_of_discrete_for_cal = NUM_OF_DISCRETE_FOR_CAL;
uint8_t str[20];
char weight_char[10];
uint32_t cal_weight = 0;
uint32_t maxWeight = 0;
double discrete = 0;
float last_weight = 0;
float last_weight1 = 0;
int weight_int = 0;
char discrete_char1[20];
uint32_t weight_rem = 0;
uint8_t stable = 0;
uint8_t repeat_weight = 0;
uint8_t stable_time_aw = 0;
uint8_t stable_freeze_discretes = 1;
uint8_t weight_stable_count = 0;
uint8_t weight_count_for_stable = 10;
uint8_t start_stable_find = 0;
uint32_t adc_value_weight = 0;

void adc_cut(void) {
	adc_value = adc_value_r >> ble_settings.adcBitForCut;
	if(ble_settings.showADC >= 2){
	SEGGER_RTT_printf(0, "%d\n", adc_value);
	}
}


void set_weight(uint16_t weight_value)
{
	adc_need = weight_value*((cal_load_value - cal_zero_value)/20)+cal_zero_value; 
	start_timer_adc();
	SEGGER_RTT_printf(0, "adc_need = %d\r\n", adc_need);
}

void sort_array(uint32_t* array, uint8_t size)
{
	for(int i = size-1; i>=0; i--)
		{
			for (int j=0; j<i; j++)
				{
					if(array[j] > array[j+1])
						{
							uint32_t tmp = array[j];
							array[j] = array[j+1];
							array[j+1] = tmp;
						}
				}
		}
}

void print_array(uint32_t* array, uint8_t size)
{
		for(int i = 0; i<size; i++)
			{
					SEGGER_RTT_printf(0, "%d\n\r", array[i]);
					nrf_delay_ms(50);
			}
}

void print_array_float(float* array, uint8_t size)
{
		for(int i = 0; i<size; i++)
			{
					sprintf((char*)str, "%.2f\n", array[i]);
					SEGGER_RTT_printf(0, (char*)str);
					nrf_delay_ms(50);
			}
}





void find_average_in_array(uint32_t* array, uint8_t size)
{
				uint8_t start_id = NUM_EXCEED_MEMBERS/2;
				uint8_t end_id = size - start_id;
			  uint8_t j = 0;
	
				for(uint8_t i = start_id; i < end_id; i++)
					{
						average_adc = average_adc + adc_array[i];
						adc_array_filtered[j] = adc_array[i];
						j++;
						//SEGGER_RTT_printf(0, "arr[%d] %d\n\r",i, adc_array[i]);
					}
					//segtext("adc_array_filtered\n");
					//print_array(adc_array_filtered, AVERAGE_ADC_TIMES-NUM_EXCEED_MEMBERS);
					//SEGGER_RTT_printf(0, "average_adc = %d; size-num = %d\n", average_adc, (size-NUM_EXCEED_MEMBERS));
					average_adc_float = (float)average_adc/(size-NUM_EXCEED_MEMBERS);
					average_adc = (int)(average_adc_float + (float)0.5);
					
					//SEGGER_RTT_printf(0, "rounded average = %d; ", average_adc);
					//sprintf(str, "average_adc_float = %.4f\n", average_adc_float);
					//segtext(str);
					//SEGGER_RTT_printf(0, "result float = %2.2f; ", average_adc_float);
					
}

void find_average_adc(void)
{
			  rgb_set(50,0,0,0,1000);
				// записываем в массив новое значение АЦП (каждые 100мс)
				adc_array[count_average_adc] = adc_value;
				count_average_adc++;

	
	// если мы набрали нужное количество значений ацп в массив
				if(count_average_adc == AVERAGE_ADC_TIMES)
						{
						//	print_array(adc_array, AVERAGE_ADC_TIMES);
							// сортируем их
							sort_array(adc_array, AVERAGE_ADC_TIMES);
							find_average_in_array(adc_array, AVERAGE_ADC_TIMES);
							uint8_t cal_adc_pref[] = "s5/1/";
							uint8_t cal_adc_pref2[] = "s5/2/";
							uint8_t cal_adc_pref3[] = "s5/3/";
							uint16_t length;
							uint16_t length2;
							uint16_t length3;
							switch(start_average_adc)
							{
								case 1:
									//ble_settings.showADC = 0;
									cal_zero_value = average_adc;
									fds_update_value(&cal_zero_value, file_id, fds_rk_cal_zero);
								
									sprintf((char*)ble_string_put1, "%d", cal_zero_value);
									length = strlen((char*)ble_string_put1);
									memcpy(cal_adc_pref+5, ble_string_put1, length);
									ble_comm_send_handler(cal_adc_pref);
									segtext(cal_adc_pref);
								  segtext("\nstart_average_adc == 1\n");
								// почему то записывается значение ацп в discrete_char1, поэтому повторно пишем туда дискрет
									sprintf(discrete_char1, "%.2f", discrete);
									SEGGER_RTT_printf(0, "maxWeight = %d\n", maxWeight);
									SEGGER_RTT_printf(0, "discrete = %s\n", discrete_char1);
									SEGGER_RTT_printf(0, "cal_weight = %d\n", cal_weight);
									num_of_discrete_for_cal = cal_weight/(float)discrete;
									SEGGER_RTT_printf(0, "num_of_discrete_for_cal = %d\n", num_of_discrete_for_cal);
									//we have stopped it by phone before zero calibrating, now start again	
									ble_settings.showADC = 1;
								 //	ble_comm_send_handler(ble_string_put);
								//	SEGGER_RTT_printf(0, "zero - %d\n\r", cal_zero_value);
								break;
								
								case 2:
									ble_settings.showADC = 0;
									cal_load_value = average_adc;
									if((scale_type == SCALE_600) && cal_zero_value)
										{
											 // adc value for one discrete //10 
											
										cal_coef_float = (cal_load_value - cal_zero_value)/(float)num_of_discrete_for_cal; 
										sprintf((char*)str, "cal_coef = %.4f\n", cal_coef_float);
										segtext(str);
											
											SEGGER_RTT_printf(0, "cal_coef - %d\n\r", cal_coef);
										}
										fds_update_value(&cal_coef, file_id, fds_rk_cal_zero+1);
										ble_comm_send_handler("s5/2/");
										SEGGER_RTT_printf(0, "load - %d\n\r", cal_load_value);
										//discrete+=1000;
										sprintf(discrete_char1, "%.2f", discrete);
										SEGGER_RTT_printf(0, "maxWeight = %d\n", maxWeight);
										
										sprintf((char*)ble_string_put1, "%d", cal_coef);
										length2 = strlen((char*)ble_string_put1);
										memcpy(cal_adc_pref2+5, ble_string_put1, length2);
										ble_comm_send_handler(cal_adc_pref2);
										segtext(cal_adc_pref2);
										ble_settings.showADC = 1;
								break;
								
								case 3:
									cal_turn_on = average_adc;
									fds_update_value(&cal_turn_on, file_id, fds_rk_cal_zero+2);
									SEGGER_RTT_printf(0, "turn_on - %d\n\r", cal_turn_on);
									
									sprintf((char*)ble_string_put1, "%d", cal_turn_on);
									length3 = strlen((char*)ble_string_put1);
									memcpy(cal_adc_pref3+5, ble_string_put1, length3);
									ble_comm_send_handler(cal_adc_pref3);
									segtext(cal_adc_pref3);
								break;
								
								case 4:
								count_average_adc = 0;
								start_average_adc = 0;
								average_adc = 0;
								stop_timer_02s();
								rgb_set(0,0,0,0,0);
							}
						}
}

void weight_define(void){
	
	
	if(cal_coef_float){
			weight = ((adc_value-cal_zero_value)/cal_coef_float)*discrete;
			if(weight > 80000){
				weight = 0;
			}
			int weight_10 = (int)(weight*10);
			int weight_100 = (int)(weight*100);
			weight_int = (int)weight;
			//выделяем целую часть
			//	sprintf(str, "weight = %.4f\n", weight);
			//segtext(str);
			
			
			
			
			if(discrete == 0.01){
				sprintf(weight_char, "weight = %.2f\n", weight);
			} 
			else if (discrete == 0.02)
			{
				if((weight_100)%2 == 0){
						weight = weight_100/100.0;
				} else {
					weight = (weight_100 + 1)/100.0;
				}
				
				sprintf(weight_char, "weight = %.2f\n", weight);
				
				
			} 
			
			else if (discrete == 0.05)
			{
					weight_rem = weight_100 % 5;
								
									if(weight_rem < 3)
										{
											weight = (weight_100 - weight_rem)/100.0;
										}
									else 
									{
										  weight = (weight_100 + (5 - weight_rem))/100.0;
									}
								sprintf(weight_char, "weight = %.2f\n", weight);
			}



			else if (discrete == 0.10)
			{
				weight = (((double)weight*10 + 0.5))/10.00;
		  	sprintf(weight_char, "weight = %.1f\n", weight);
			} else if (discrete == 0.20)
					{
				
					//weight = ((int)(weight*10 + 0.5))/10.0;
						
						if(weight_10 % 2 == 0)
								{					
									// weight is even
									weight = weight_10/10.0;
								}
							else 
								{
									// weight is odd
									weight = (weight_10 + 1)/10.0;
								}
						
						sprintf(weight_char, "weight = %.1f\n", weight);
					} 
					
					else if (discrete == 0.5)
							{
								//segtext("discrete == 0.5");
								weight_rem = weight_10 % 5;
								
									if(weight_rem < 3)
										{
											weight = (weight_10 - weight_rem)/10.0;
										}
									else 
									{
										  weight = (weight_10 + (5 - weight_rem))/10.0;
									}
								sprintf(weight_char, "weight = %.1f\n", weight);
							}
					
			 else if (discrete == 1)
			{
				weight = ((int)((double)weight + 0.5))/1.0;
			//	SEGGER_RTT_printf(0, "(%d + (5 - %d))/10.0 = %d;\n", weight_10, weight_10, (weight_10 + (5 - weight_10))/10.0);
		   	sprintf(weight_char, "weight = %.1f\n", weight);
				
			}
			
			else if (discrete == 2)
			{
				if((weight_int % 2) != 0){
					weight_int += 1;
					//segtext("odd\n");
				}
				else{
					//segtext("even\n");
				}
				sprintf(weight_char, "weight_int = %d\n", weight_int);
			} 
				else if (discrete == 5)
							{
								
								weight_rem = weight_int % 5;
								
									if(weight_rem < 3)
										{
											weight_int -= weight_rem;
										}
									else 
									{
										  weight_int = (weight_int + (5 - weight_rem));
									}
								sprintf(weight_char, "weight_int = %d\n", weight_int);
							}
			else if (discrete == 10)
			{
				weight_int = ((weight_int + 5)/10)*10;
			//	SEGGER_RTT_printf(0, "(%d + (5 - %d))/10.0 = %d;\n", weight_10, weight_10, (weight_10 + (5 - weight_10))/10.0);
		   	sprintf(weight_char, "weight_int = %d\n", weight_int);
				
			}
			
			else if (discrete == 20)
			{
				weight_rem = weight_int % 20;
				if(weight_rem < 10){
					weight_int = (weight_int/20)*20;
				}
				else{
					weight_int = ((weight_int/20)*20)+20;
				}
				sprintf(weight_char, "weight_int = %d\n", weight_int);
			} 
			
			else if (discrete == 50)
			{
				weight_rem = weight_int % 50;
				//SEGGER_RTT_printf(0, "weight_rem = %d\n", weight_rem);
				//SEGGER_RTT_printf(0, "weight_int = %d\n", weight_int);
				if(weight_rem < 25){
					weight_int = (weight_int/50)*50;
				}
				else{
				
					weight_int = ((weight_int/50)*50) + 50;// + (50-weight_rem);
				}
				sprintf(weight_char, "weight_int_end = %d\n", weight_int);
			} 
//			switch (discrete){
//				case: 0.2
//			}
			
			if(discrete < 2){
			
					if(last_weight != weight)
						{
							  float dif_weight = last_weight - weight;
								if(dif_weight < 0) dif_weight = -dif_weight;
							  //sprintf(str, "weight = %.2f\n", dif_weight);
						  	if(dif_weight > (discrete*stable_freeze_discretes)){
									start_stable_find = 1;
									}
									segtext(str);
									segtext("\n");
									segtext((uint8_t*)weight_char);
									// handler of this timer in remote.c and ther we call find_average_adc that above
										if(start_stable_find){
											weight_stable_arr[weight_stable_count] = weight;
											weight_stable_count++;
										  if(weight_stable_count >= weight_count_for_stable){
												//print_array_float(weight_stable_arr, 10);
												start_stable_find = 0;
												weight_stable_count = 0;
												segtext("---------------------\n");
										}
									}
								}
  							// if(last_weight1 == weight){
								//SEGGER_RTT_printf(0, "weight_10 = %d\n", weight_10);
								//сохраняем пред пред последний вес, для стабилизации
								last_weight1 = last_weight;	
								last_weight = weight;
							}
						}
			}

void cal_unload(void)
{
		start_average_adc = 1;
		// handler of this timer in remote.c and ther we call find_average_adc that above
	  start_timer_02s();
		segtext("cal_unload\n");
	}

void cal_load(void) // define adc_value for 10 discretes 
{
	start_average_adc = 2;
	start_timer_02s();
	segtext("cal_load\n");
}

void define_corr_on(void)
{	
	start_average_adc = 3;
	start_timer_02s();
	segtext("define_corr_on\n");
	
}

void define_corr_on_uart(void)
{
	if(uart_weight > 0)
	{
		cal_turn_on = uart_weight;
		fds_uart_automode = 1;
		fds_update_value(&cal_turn_on, file_id, fds_rk_cal_zero+2);
		fds_update_value(&fds_uart_automode, file_id_c, fds_rk_uart_automode);
		SEGGER_RTT_printf(0, "uart turn_on = %d\n\r", cal_turn_on);
		rgb_set(0, 50, 0, 5, 500);
	}
	
}


void init_cal_values(void)
{
		fds_get_data(&cal_zero_value, file_id, fds_rk_cal_zero);
		fds_get_data(&cal_coef, file_id, fds_rk_cal_zero+1);
		fds_get_data(&cal_turn_on, file_id, fds_rk_cal_zero+2);
}
