#include "ble_debug.h"


//void ble_debug_handler_lib (uint8_t* buf);

//uint8_t ble_string_put2[20] = "12345";

void ble_debug(uint8_t * ble_buffer)
	{
		
						
						uint32_t send_num = 0;
						uint16_t debug_number = atoi((char*)(ble_buffer+1));
						
						switch (debug_number)
							
						{
							case 1:
								send_num = clock_counter;
								break;
							
							case 2:
								send_num = life_counter/60;			// passed minutes
								break;
							
							case 3:
								send_num = activate_status;	
								break;
							
							case 31:
								send_num = cur_cor_max;	
								break;
							
							case 32:
								send_num = cur_hr_max;	
							break;
							
							case 33:
								send_num = cur_res_max;	
							break;
							
							case 4:
								send_num = corr_counter;	
							break;
							
							case 5:
								send_num = power_down_count;	
							break;
							
							case 6:
								send_num = cal_zero_value;	
							break;
							
							case 7:
								send_num = cal_turn_on;	
							break;
							
							case 8:
								send_num = fds_clear_counter;	
							break;
							
							case 9:
								//ble_debug_handler(ble_buffer);
								//ble_string_put = device_name;
							//	memcpy(ble_string_put, device_name, sizeof(device_name));

								//can be
								//REMOTE_ONLY   3
								//PHONE_ONLY    4
								//REMOTE_PHONE  5
							send_num = fds_remote_type;	
								SEGGER_RTT_printf(0, "fds_remote_type = %d\n", fds_remote_type);
								break;
							case 10:
								send_num = activate_attempts;
								break;
				
							case 11:
								send_num = phone_cor_counter;
								break;
							case 12:
								send_num = fds_soft_version;
							break;
							
							case 13:
								send_num = fds_volume_counter;
							break;
							
							case 14:
								send_num = fds_archive_counter;
							break;
							
							case 15:
								send_num = fds_option_status;
							break;
							
							case 16:
								send_num = fds_pcb_config;
							break;
							
							case 17:
								send_num = correct_mode;
							break;
						}
						
						if(send_num > 0)
						{
							uint8_t ble_debug_string[20];
							sprintf((char*)ble_debug_string, "%d", send_num);
							ble_comm_send_handler(ble_debug_string);
	
						//	ble_debug_handler(ble_string_put2);
						}
		}
		
		





