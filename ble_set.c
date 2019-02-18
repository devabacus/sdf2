#include "ble_set.h"
#include "ble_weigh.h"

ble_settings_t ble_settings;

uint8_t showadc = 0;
uint32_t uart_work = 1;
uint8_t adcBitCut = 0;
uint8_t but_ble = 0;
void ble_set_init(void);
ble_type_sett_t sett_type = SHOWADC;
void remote_but_update(uint8_t but_num);
int findIdexOfArray(uint8_t *buf, int startIndex, char character);




void ble_set_init(){
	ble_settings.adcBitForCut = 7;
	ble_settings.showADC = 0;
	ble_settings.workADC = 1;
}


void change_max_counters(uint8_t param, uint16_t value){
	
	if(admin){
		
		if(param == 1){ //cur_cor_max
			cur_cor_max = value;
			fds_update_value(&cur_cor_max, file_id_c, fds_rk_cur_cor_max);
		} else if (param == 2){ //cur_res_max
			cur_hr_max = value;
			fds_update_value(&cur_hr_max, file_id_c, fds_rk_cur_cor_max+1);
			
		} else if (param == 3){ //cur_hr_max
			cur_res_max = value;
			fds_update_value(&cur_res_max, file_id_c, fds_rk_cur_cor_max+2);	
		}
		ble_comm_send_handler("changed");
	} else {
		ble_comm_send_handler("need admin");
	}
}

void reset_counters(uint8_t ch_num){
		if(admin){

			switch(ch_num){
				case 1:
					corr_counter = 1;
					fds_update_value(&corr_counter, file_id_c, fds_rk_corr_counter);
					ble_comm_send_handler("c res");
				break;
				
				case 2:
					power_down_count = 1;
					fds_update_value(&power_down_count, file_id_c, fds_rk_power_down);
					ble_comm_send_handler("p res");
				break;
				
				case 3:
					life_counter = 60;
					fds_update_value(&life_counter, file_id_c, fds_rk_clock);
					ble_comm_send_handler("l res");
				break;
				
				case 4:
					phone_cor_counter = 1;
					fds_update_value(&phone_cor_counter, file_id_c, fds_rk_phone_cor_counter);
					ble_comm_send_handler("ph res");
				break;
				
				case 5:
					fds_archive_counter = 0;
					fds_option_status |= (0x0UL << OPTION_ARCHIVE_Pos);
					fds_update_value(&fds_option_status, file_id_c, fds_rk_option_status);
					ble_comm_send_handler("arch res");
				break;
				
				case 6:
					fds_volume_counter = 0;
					fds_option_status |= (0x0UL << OPTION_VOLUME_Pos);
					fds_update_value(&fds_option_status, file_id_c, fds_rk_option_status);
				ble_comm_send_handler("vol res");
				break;
				
				case 7:
					activate_status = 1;
					fds_update_value(&activate_status, file_id_c, fds_rk_activate_status);
			   	ble_comm_send_handler("demo res");
				break;

			}
										
}
}


void set_send_cor_mode(uint8_t set_value){
		correct_mode = set_value;
		uint8_t mode_state;
		uint8_t interface_enum = REMOTE_MODE_CHANGE; 
		if(set_value == 1){
			 ble_comm_send_handler("n3/1");
				mode_state = 1;
		}
		else if (set_value == 2) {
			ble_comm_send_handler("n3/2");
			  mode_state = 0;
		}
		else if (set_value == 3){
			 ble_comm_send_handler ("n3/3");
		}
		beginPacket();
		lora_write(&interface_enum, 1);
		lora_write(&mode_state, 1);
		endPacket();
	  
}

void ble_set(uint8_t *ble_set_buffer){
	
			segtext(ble_set_buffer);
			segtext("\n");
			uint8_t set_number = atoi((char*) ble_set_buffer+1);
			
			uint8_t slashIndex = findIdexOfArray(ble_set_buffer, 1, '/')+1;  // index = 1
	
			uint8_t slashIndex2 = findIdexOfArray(ble_set_buffer, 4, '/')+1;
			
			uint8_t set_value = atoi((char*) ble_set_buffer + slashIndex);
	
			uint16_t set_value2 = atoi((char*) ble_set_buffer + slashIndex2);
	
			switch (set_number){
				
				case SHOWADC:
						ble_settings.showADC = set_value;
						SEGGER_RTT_printf(0, "ble_settings.showADC = %d\n", ble_settings.showADC);
						
				break;
								
				case ADCBIT:
						ble_settings.adcBitForCut = set_value;
				break;
				
				case AUTOCOR:
						set_send_cor_mode(set_value);
				break;
				
				case UART:
						if(set_value == 0){ 
							uart_work = 0;
							ble_comm_send_handler("uart off");
						}
						else if (set_value == 1) {
							app_uart_flush();
							uart_work = 1;
							weight_float = 0;
							ble_comm_send_handler("uart on, int");
						}
						else if (set_value == 2){
							app_uart_flush();
							uart_work = 1;
							weight_float = 1;
							ble_comm_send_handler("uart on, float");
						}
						
						fds_update_value(&uart_work, file_id, fds_rk_uart_work);
						fds_update_value(&weight_float, file_id, fds_rk_weight_float);
						
				break;
						
				case UART_ST_INDEX:
						startWeightIndex = set_value;
						ble_comm_send_handler("start index changed");
						fds_update_value(&startWeightIndex, file_id, fds_rk_uart_weight_st);
				break;
				
				case UART_END_INDEX:
						endWeightIndex = set_value;
						ble_comm_send_handler("end index changed");
						fds_update_value(&endWeightIndex, file_id, fds_rk_uart_weight_end);
				break;
				
				case UART_BLE:
						uart_ble_mode = set_value;
						ble_comm_send_handler("uart ble changed");
						SEGGER_RTT_printf(0, "uart_ble_mode = %d\n", set_value);
						fds_update_value(&uart_ble_mode, file_id, fds_rk_uart_ble_mode);
				break;
				
				
				
				case SAVEBUT:
					//but_ble = set_value;
					remote_but_update(set_value);
					SEGGER_RTT_printf(0, "but_ble = %d\n", but_ble);
				break;
				
				case CALIBRATION:
					if(set_value == 1) cal_unload();
					else if (set_value == 3) 
						{
								if(uart_weight > 0)
								{
									define_corr_on_uart();
								}
								else 
								{
									define_corr_on();
								}
   					}
					
					
					else if (set_value == 2) {
						SEGGER_RTT_printf(0, "num_of_discrete = %d\n", set_value2);
						cal_load();
					}
					ble_comm_send_handler("s5..");
					break;
				case NUM_COR_BUT9:
					if(set_value == 9) change_num_cor_but(9);
					else change_num_cor_but(3);
				break;
				
				case RESET_COUNTERS_BLE:
					reset_counters(set_value);
				break;
				
				case CHANGE_MAX_COUNTERS:
					SEGGER_RTT_printf(0, "set_value1 = %d\n", set_value);
				SEGGER_RTT_printf(0, "set_value2 = %d\n", set_value2);
					change_max_counters(set_value, set_value2);
				break;
				
				// увеличение счетчика записи в архив и нажатий кнопок громкости
				case OPTION_COUNTER:
					//archive_counter
					if(set_value == 1){
						fds_volume_counter++;
									if((fds_volume_counter > VOLUME_DEMO_COUNTER_MAX) && (!(fds_option_status & (OPTION_VOLUME_Msk)))){
										//reset volume demo
											fds_option_status |= (0x0UL << OPTION_VOLUME_Pos);
											fds_update_value(&fds_option_status, file_id_c, fds_rk_option_status);
											ble_comm_send_handler("o2/0");	
									} else {
											fds_update_value(&fds_volume_counter, file_id_c, fds_rk_volume_counter);
									}

									//SEGGER_RTT_printf(0, "fds_volume_counter = %d\n", fds_volume_counter);
					}



					else if (set_value == 2){
						fds_archive_counter++;
								if((fds_archive_counter > ARCHIVE_DEMO_COUNTER_MAX) && (!(fds_option_status & (OPTION_ARCHIVE_Msk)))){
										fds_option_status |= (0x0UL << OPTION_ARCHIVE_Pos);
										fds_update_value(&fds_option_status, file_id_c, fds_rk_option_status);
										ble_comm_send_handler("o1/0");	
								} else {
								fds_update_value(&fds_archive_counter, file_id_c, fds_rk_archive_counter);
								SEGGER_RTT_printf(0, "fds_archive_counter = %d\n", fds_archive_counter);
								}
				}
					
				break;
				
				case CAL_MAX_WEIGHT:
					maxWeight = atoi((char*) ble_set_buffer + slashIndex);
				
				break;
					
				case CAL_DISCRET:
					//char discrete_char[10];
//					SEGGER_RTT_printf(0, "ble_set_buffer = %s\n", ble_set_buffer);
//				SEGGER_RTT_printf(0, "ble_set_buffer+slashIndex = %s\n", ble_set_buffer+slashIndex);
					discrete = atof((char*) ble_set_buffer + slashIndex);
					sprintf(discrete_char1, "%.2f", discrete);
					SEGGER_RTT_printf(0, "discrete = %s\n", discrete_char1);
				break;
				
				case CAL_LOAD_WEIGHT:
					cal_weight = atoi((char*) ble_set_buffer + slashIndex);
				break;
				case 17:
					if(set_value == 0){
						nrf_gpio_pin_clear(15);
						nrf_gpio_pin_clear(17);
					} else if (set_value == 1){
						nrf_gpio_pin_set(15);
						nrf_gpio_pin_set(17);
					}
					//nrf_gpio_pin_toggle(31);
				//nrf_gpio_pin_toggle(17);
				break;
}
}
