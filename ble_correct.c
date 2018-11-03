#include "ble_correct.h"

uint8_t ble_string_put1[10] = "";
uint32_t cor_value = 0;
int comp_value = 0;
uint8_t ble_correct_active = 0;
uint32_t cor_value_save_ble = 0;
uint32_t phone_cor_counter = 0;
uint8_t first_time_open_set = 0;


uint8_t isdigit(char c);

void remote_but_update(uint8_t but_num){
		
	corr_mode_button = but_num;
	switch(corr_mode_button){
		case 1:
			corr_1_1 = cor_value;
		break;
		case 2:
			corr_1_2 = cor_value;
		break;
		case 3:
			corr_1_3 = cor_value;
		break;
		case 4:
			corr_2_1 = cor_value;
		break;
		case 5:
			corr_2_2 = cor_value;
		break;
		case 6:
			corr_2_3 = cor_value;
		break;
		case 7:
			corr_3_1 = cor_value;
		break;
		case 8:
			corr_3_2 = cor_value;
		break;
		case 9:
			corr_3_3 = cor_value;
		break;
		
		//cor_value_save_ble = 0;
		//cor_value = 0;
		
	}
	save_corr_values();
	cor_value = 0;
	remote_mode = WORK_MODE;
	SEGGER_RTT_printf(0, "corr_1_1 = %d\n", corr_1_1);
	SEGGER_RTT_printf(0, "save button = %d, value = %d\n", but_num, cor_value);
}


void defineCorDir(uint8_t * buf, uint8_t offset)
{
		
							if(buf[offset-1] == '-')
								{
									cor_value = 0;
								}

							else if ((buf[offset-1] == '+') && buf[offset] != '0')
								{
									cor_value = 1000;
								}
							cor_value += atoi((char*)(buf+offset));
}

void ble_correct(uint8_t * ble_buffer)
	{
		//when setup correct we send p, +, - in the beginning
				uint8_t isButton = isdigit(ble_buffer[1]);
					
						if(ble_buffer[1] == 'r')
						{
							correct(0,0,0);
							cor_value_auto = 0;
							cor_value = 0;
							comp_value = 0;
							test_expired();
							
							// send info that correct reset manually
							ble_comm_send_handler("n1/0");
							ble_correct_active = 0;
							first_time_open_set = 0;
							//return;
						}
		
					
					// work_mode
					if (isButton){
						if((fds_remote_type != REMOTE_ONLY) || admin || (phone_cor_counter < PHONE_COR_COUNTER_MAX_DEMO)){
										
										phone_cor_counter++;
										fds_update_value(&phone_cor_counter, file_id_c, fds_rk_phone_cor_counter);
										SEGGER_RTT_printf(0, "phone_cor_counter = %d\n", phone_cor_counter);
										uint8_t cor_button_ble = atoi((char*)ble_buffer+1);
										remote_mode = WORK_MODE;	
										current_but = cor_button_ble;
										correct(0,0,0);
										
										if(cor_button_ble < 10) defineCorDir(ble_buffer, 4);
										else defineCorDir(ble_buffer, 5);
						
						} else {
									phone_cor_counter++;
									rgb_set(50, 0, 0, 2, 500);
									rgb_set(0, 0, 50, 2, 500);
									segtext("phone not allowed\n");
									ble_comm_send_handler("n7/0");
						}
						
						
					}
					//corr_mode_button
					else if (ble_buffer[1] != 'r'){
						 // correct while setup values
						//SEGGER_RTT_printf(0, "first_time_open_set = %d\n", first_time_open_set);
							if(!first_time_open_set)
							{
								first_time_open_set = 1;
						  	if(adc_value) adc_value_start = adc_value;
							}
							
							
						//SEGGER_RTT_printf(0, "adc_value_start_ble = %d\n", adc_value_start);
						remote_mode = CORR_BUT_MODE;
						defineCorDir(ble_buffer, 2);
						}
					 
					 //if there's comp value
					if(findIdexOfArray(ble_buffer, 1, 'p'))
					{
							uint8_t percIndex = findIdexOfArray(ble_buffer, 1, 'p')+1;  // index = 1
							uint8_t compIndex = findIdexOfArray(ble_buffer, 3, 'c')+1;  // index = 
							cor_value = atoi((char*)(ble_buffer+percIndex));;
							comp_value = atoi((char*)(ble_buffer+compIndex));
						
							if(comp_value > 0){
								comp_value += 1000;
							} else {
								comp_value = -comp_value;
							}
							cur_comp_cor = comp_value;	
							
							if(correct_mode == COR_MANUAL){
								correct_value(cor_value + 2000);
								//if(isButton) zero(15, 100);
								correct_value(comp_value);
								
							} else if(correct_mode == COR_AUTO){
								cor_value_auto = cor_value;
								
								cur_comp_cor = comp_value;
							}
							ble_correct_active = 1;
							
					}
					// else if kg+, kg-, % main value
					else {
							//defineCorDir(ble_buffer, 2);
							// just in case reset last value of comp_value if last correct was with comp
							cur_comp_cor = 0;
							if(correct_mode == COR_MANUAL){
								correct_value(cor_value);
								rgb_set(0, 50, 0, 1, 500);
								//if(isButton) zero(15, 100);
								
								
							} else if(correct_mode == COR_AUTO){
									if(cor_value > 0){
											cor_value_auto = cor_value;
											ble_correct_active = 1;
											SEGGER_RTT_printf(0, "adc_value_but = %d\n\r", adc_vals_ar[current_but-1]);
											ble_comm_send_handler("n2/1");
											
									}
									
							}
							
							ble_correct_active = 1;
					}
					
					isButton = 0;
					segtext(ble_buffer);
					segtext("\n");
											
		}
		
		





