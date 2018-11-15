#include "ble_comm.h"

uint8_t ble_active = 0;
uint8_t ble_string_put[20] = "";

void ble_correct(uint8_t * buffer);
void ble_debug(uint8_t * buffer);
void ble_set(uint8_t * buffer);

void ble_comm_send_num_handler(uint32_t num){
	
			
			sprintf((char*)ble_string_put, "%d", num);
			ble_comm_send_handler(ble_string_put);
}



void zero(uint8_t times, uint16_t delay){
			
			for(uint8_t i = 0; i < times; i++){
					nrf_gpio_pin_set(15);
					nrf_delay_ms(delay);
					nrf_gpio_pin_clear(15);
					nrf_delay_ms(delay);
			}
			
}




int findIdexOfArray(uint8_t *buf, int startIndex, char character){
		for(uint8_t i = startIndex; i<strlen((char*)buf); i++ )
		{
				if(buf[i] == character){
				return i;
			}
		}
		return 0;
}

void option_notif(){
	
	
	
		if((fds_archive_counter < ARCHIVE_DEMO_COUNTER_MAX) && (!(fds_option_status & (OPTION_ARCHIVE_Msk)))){
			ble_comm_send_handler("o1/1");
			segtext("o1/1\n");
		} else if((fds_archive_counter >= ARCHIVE_DEMO_COUNTER_MAX) && (!(fds_option_status & (OPTION_ARCHIVE_Msk)))){
			ble_comm_send_handler("o1/0");	
			segtext("o1/0\n");
		}
		else if (fds_option_status & (OPTION_ARCHIVE_Msk)){
			ble_comm_send_handler("o1/2");	
			segtext("o1/2\n");
		}
		
		nrf_delay_ms(50);
		
		if((fds_volume_counter < VOLUME_DEMO_COUNTER_MAX) && (!(fds_option_status & (OPTION_VOLUME_Msk)))){
			ble_comm_send_handler("o2/1");
		} else if((fds_volume_counter >= VOLUME_DEMO_COUNTER_MAX) && (!(fds_option_status & (OPTION_VOLUME_Msk)))){
			ble_comm_send_handler("o2/0");	
		}
		 else if (fds_option_status & (OPTION_VOLUME_Msk)){
			ble_comm_send_handler("o2/2");	
		}
	}


void ble_comm(uint8_t * ble_buffer)
	{
		
					switch(ble_buffer[0]){
							
						case '$':
								
						ble_correct(ble_buffer);
					  break;
						
						case 'd':
								ble_debug(ble_buffer);
								break;
								
						case 's':
								ble_set(ble_buffer);
								//strcpy((char*)ble_string_put, "ivanovich");
								ble_comm_send_handler(ble_string_put);
								break;
						// demand init parameter
						case 'i':
								ble_active = 1;
								set_send_cor_mode(correct_mode);
								nrf_delay_ms(50);
								option_notif();
								
								//ble_comm_send_handler("opt2/1");
								break;
						case 'p':
								remote_mode = STATUS_DEFINE;
								current_life_counter = life_counter;
								generate_admin_pass1();
								generate_option_pass();
								
								ble_comm_send_handler("wait pass");
								segtext("wait pass");
								break;
						
						case 'a':
								if(remote_mode == STATUS_DEFINE){
									test_activate_code = atoi((char*)ble_buffer+1);
									SEGGER_RTT_printf(0, "get number %d\n", test_activate_code);
									check_pass();
									//check_option_pass();
								}
								break;
								
						case 'o':
								if(remote_mode == STATUS_DEFINE){
								  test_activate_code = atoi((char*)ble_buffer+1);
									SEGGER_RTT_printf(0, "get number %d\n", test_activate_code);
									check_option_pass();
								}
								break;
								
						case 'c':
								if(remote_mode == STATUS_DEFINE){
								  test_activate_code = atoi((char*)ble_buffer+1);
									SEGGER_RTT_printf(0, "get number %d\n", test_activate_code);
									check_config_pass();
								}
								break;
								
						case 't':
								app_uart_put(ble_buffer[1]);
								SEGGER_RTT_printf(0, "get %c\n", ble_buffer[1]);
						break;
						
						
						case 'z':
								segtext("zero");
								zero(2, 150);
								
								//ble_comm_send_handler("zero");
							break;
						
		}
	}
		
//	void ble_info(uint8_t* buffer){
//		if(correct_mode == COR_MANUAL)
//		ble_comm_send_handler("i1/2");
//	}
		





