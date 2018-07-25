#include "ble_set.h"

ble_settings_t ble_settings;

uint8_t showadc = 0;
uint32_t uart_work = 0;
uint8_t adcBitCut = 0;
uint8_t but_ble = 0;
void ble_set_init(void);
ble_type_sett_t sett_type = SHOWADC;
void remote_but_update(uint8_t but_num);
int findIdexOfArray(uint8_t *buf, int startIndex, char character);


void ble_set_init(){
	ble_settings.adcBitForCut = 5;
	ble_settings.showADC = 0;
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
				
			}
										
}
}


void set_send_cor_mode(uint8_t set_value){
		correct_mode = set_value;
		if(set_value == 1) ble_comm_send_handler("n3/1");
		else if (set_value == 2) ble_comm_send_handler("n3/2");
}

void ble_set(uint8_t *ble_set_buffer){
	
			
			uint8_t set_number = atoi((char*) ble_set_buffer+1);
			
			uint8_t slashIndex = findIdexOfArray(ble_set_buffer, 1, '/')+1;  // index = 1
			
			uint8_t set_value = atoi((char*) ble_set_buffer + slashIndex);
	
			switch (set_number){
				
				case SHOWADC:
						ble_settings.showADC = set_value;
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
				
				case ADCBIT:
						ble_settings.adcBitForCut = set_value;
				break;
				
				case AUTOCOR:
						set_send_cor_mode(set_value);
				break;
				
				case SAVEBUT:
					//but_ble = set_value;
					remote_but_update(set_value);
					SEGGER_RTT_printf(0, "but_ble = %d\n", but_ble);
				break;
				
				case CALIBRATION:
					if(set_value == 1) cal_unload();
					else if (set_value == 3) define_corr_on();
					else if (set_value == 2) cal_load();
					ble_comm_send_handler("s5..");
					break;
				case NUM_COR_BUT9:
					if(set_value == 9) change_num_cor_but(9);
					else change_num_cor_but(3);
				break;
				
				case RESET_COUNTERS_BLE:
					reset_counters(set_value);
				break;
				
			}
			
		
}

