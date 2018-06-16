#include "flash_storage.h"
#include "remote.h"
#include "offset_perc.h"



volatile uint8_t init_flag = 0;
volatile uint8_t write_flag = 0;
volatile uint8_t delete_flag = 0;
volatile uint8_t update_flag = 0;



uint32_t fds_update_counter = 0;

uint8_t fds_no_space = 0;
uint16_t file_id 									=  0x0000;
uint16_t file_id_c 								=  0x0001;

fds_stat_t stat = {0};

uint16_t fds_rk_init 							=  0x0001;
uint16_t fds_rk_activate_status   =  0x0002;
uint16_t fds_rk_power_down	      =  0x0003;
uint16_t fds_rk_activate_attempts =  0x0004;
uint16_t fds_rk_corr_counter      =	 0x0005;



uint16_t fds_rk_demo1 						=  0x0006;	
//6,7,8,9,A

uint16_t fds_rk_cur_cor_max 			=  0x000B;  
//B,C,D

uint16_t fds_rk_clock 						=  0x000E;

uint16_t fds_rk_clear_counter			=  0x000F;

uint16_t fds_rk_corr_mode					=  0x0010;
uint16_t fds_rk_num_cor_but       =  0x0011;
uint16_t fds_rk_cor_feedback 			=  0x0012;
uint16_t fds_rk_time_feedback 		=  0x0013;
uint16_t fds_rk_feedback 					=  0x0014;

uint16_t fds_rk_cal_zero 					=  0x0015;	
//cal_coef 		 = 16
//cal_turn_on	 = 17

uint16_t fds_rk_cor1 							=	 0x0018;  
//18,19,1A,1B,1C,1D,1E,1F,20

//uint16_t fds_rk_perc_offset1 			=  0x0022;
//1_1 22,   
//1_2 23,  +1
//1_3 24,  +2	
//2_1 25,	 +3	
//2_2 26,	 +4	
//2_3 27,	 +5	
//3_1 28,	 +6	
//3_2 29,	 +7	
//3_3	3A	 +8

// values for each correct_button for autocormode
uint16_t fds_rk_adc_val1			=	 0x003B;
//3C
//3D
//3E
//3F
//40
//41
//42
//43
//44

uint16_t fds_rk_mac_init 		        = 0x0044;
uint16_t fds_rk_remote_type         = 0x0045;
uint16_t fds_rk_phone_cor_counter   = 0x0046;
uint16_t fds_rk_soft_version        = 0x0047;
uint16_t fds_rk_uart_work 				  = 0x0048;
uint16_t fds_rk_weight_float			  = 0x0049;
uint16_t fds_rk_uart_weight_st 		  = 0x0050;
uint16_t fds_rk_uart_weight_end		  = 0x0051;
uint16_t fds_rk_uart_ble_mode				= 0x0052;



uint32_t fds_is_values_init 			= 0;
uint32_t life_counter 						= 60;
uint32_t power_down_count  				= 1;
uint32_t activate_status   				= 0;
uint32_t activate_attempts 				= 0;
uint32_t num_cor_buts      				= 3;

uint32_t fds_clear_counter 				= 0;
uint32_t fds_mac_init					 		= 0;
uint32_t fds_remote_type			  	= 1;

uint32_t fds_soft_version 				= 0;



void fds_evt_handler(fds_evt_t const * const p_fds_evt)
{
    switch (p_fds_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_fds_evt->result != FDS_SUCCESS)
            {
                // Initialization failed.
            }
						init_flag = 1;
            break;
				case FDS_EVT_WRITE:
						if (p_fds_evt->result == FDS_SUCCESS)
						{
							write_flag=1;
						}
						
					break;
					
				
				case FDS_EVT_UPDATE:
					    
							
							if (p_fds_evt->result == FDS_ERR_NO_SPACE_IN_FLASH)
							{
								segtext("in fds no space event\n");
							}
				
				case FDS_EVT_DEL_RECORD:
						if (p_fds_evt->result == FDS_SUCCESS)
						{
							delete_flag=1;
						//	SEGGER_RTT_printf(0, "del\n\r");
						}
						break;
						
				case FDS_EVT_GC:
						if(p_fds_evt->result == FDS_SUCCESS)
						{
							fds_no_space = 0;
						  fds_clear_counter++;
							SEGGER_RTT_printf(0, "fds_clear_counter = %d\n\r", fds_clear_counter);
							fds_update_value(&fds_clear_counter, file_id_c, fds_rk_clear_counter);											
						}
						
        default:
            break;
    }
}


void fds_init_flash(uint32_t* value, uint16_t file_id, uint16_t rec_key)
{
	uint32_t err_code;
	err_code = fds_write_value(value, file_id, rec_key);
	APP_ERROR_CHECK(err_code);
	while(write_flag == 0);
	//SEGGER_RTT_printf(0, "into fds_init_flash\n\r");
	
	
}

void check_for_old_board(void){
		
					if(!fds_mac_init){
		
						
						SEGGER_RTT_printf(0, "ch mac= %d\n", fds_mac_init);
				  demo_define();
					fds_update_value(&demo1, file_id_c, fds_rk_demo1);
					fds_update_value(&demo2, file_id_c, fds_rk_demo1+1);
					nrf_delay_ms(50);
					fds_update_value(&demo3, file_id_c, fds_rk_demo1+2);
					fds_update_value(&demo4, file_id_c, fds_rk_demo1+3);
					nrf_delay_ms(50);
					fds_update_value(&demo5, file_id_c, fds_rk_demo1+4);
					fds_init_flash(&fds_mac_init, file_id_c, fds_rk_mac_init);
					}


					if(fds_remote_type < 2)
					{
//							SEGGER_RTT_printf(0, "ch phone  = %d\n", phone_cor_counter);
//							SEGGER_RTT_printf(0, "ch type   = %d\n", fds_remote_type);
							phone_cor_counter = 2;
							fds_remote_type   = 2;
							//all of the old board have remote_only activation
							if(activate_status >= 2) fds_remote_type = 3;
							
							fds_soft_version = 1806091;
							fds_init_flash(&phone_cor_counter, file_id_c, fds_rk_phone_cor_counter);
							fds_init_flash(&fds_remote_type, file_id_c, fds_rk_remote_type);
							fds_init_flash(&fds_soft_version, file_id_c, fds_rk_soft_version);
							//fds_update_value(&phone_cor_counter, file_id_c, fds_rk_phone_cor_counter);
					}
		}
		




void fds_init_values(void)
{
	fds_get_data(&fds_is_values_init, file_id, fds_rk_init);
	
	//for those boards that already shipment
	
	
	
	if(fds_is_values_init != 1)
	{
		//name_convert();
		demo_define();
		activate_status = 1;
		fds_init_flash(&corr_1_1, file_id, fds_rk_cor1);
		fds_init_flash(&corr_1_2, file_id, fds_rk_cor1+1);
		fds_init_flash(&corr_1_3, file_id, fds_rk_cor1+2);
		fds_init_flash(&corr_2_1, file_id, fds_rk_cor1+3);
		fds_init_flash(&corr_2_2, file_id, fds_rk_cor1+4);
		fds_init_flash(&corr_2_3, file_id, fds_rk_cor1+5);
		fds_init_flash(&corr_3_1, file_id, fds_rk_cor1+6);
		fds_init_flash(&corr_3_2, file_id, fds_rk_cor1+7);
		fds_init_flash(&corr_3_3, file_id, fds_rk_cor1+8);
		
		fds_init_flash(&cal_zero_value, file_id, fds_rk_cal_zero);
		fds_init_flash(&cal_coef, file_id, fds_rk_cal_zero+1);
		fds_init_flash(&cal_turn_on, file_id, fds_rk_cal_zero+2);
		
		fds_init_flash(&num_cor_buts, file_id, fds_rk_num_cor_but);
		fds_init_flash(&cor_feedback, file_id, fds_rk_cor_feedback);
		fds_init_flash(&time_feedback, file_id, fds_rk_time_feedback);
		fds_init_flash(&feedback, file_id, fds_rk_feedback);
		fds_init_flash(&correct_mode, file_id, fds_rk_corr_mode);
		
		offset_fds_kg_init_data();
		autocor_adc_values_init();
		
		fds_is_values_init = 1;
		
		
		//fds_update_value(&fds_is_values_init, file_id_c, fds_rk_init);
		fds_init_flash(&fds_is_values_init, file_id, fds_rk_init);
		fds_init_flash(&corr_counter, file_id_c, fds_rk_corr_counter);
		fds_init_flash(&activate_status, file_id_c, fds_rk_activate_status);
		fds_init_flash(&activate_attempts, file_id_c, fds_rk_activate_attempts);
		fds_init_flash(&power_down_count, file_id_c, fds_rk_power_down);
	
  	fds_init_flash(&demo1, file_id_c, fds_rk_demo1);
		fds_init_flash(&demo2, file_id_c, fds_rk_demo1+1);
		fds_init_flash(&demo3, file_id_c, fds_rk_demo1+2);
		fds_init_flash(&demo4, file_id_c, fds_rk_demo1+3);
		fds_init_flash(&demo5, file_id_c, fds_rk_demo1+4);
		
		fds_mac_init = 1;
		fds_init_flash(&fds_mac_init, file_id_c, fds_rk_mac_init);
		fds_init_flash(&uart_work, file_id, fds_rk_uart_work);
		fds_init_flash(&weight_float, file_id, fds_rk_weight_float);
		fds_init_flash(&startWeightIndex, file_id, fds_rk_uart_weight_st);
		fds_init_flash(&endWeightIndex, file_id, fds_rk_uart_weight_end);
		fds_init_flash(&uart_ble_mode, file_id, fds_rk_uart_ble_mode);
		
//		fds_init_flash(&fds_remote_type, file_id_c, fds_rk_remote_type);
//		fds_init_flash(&phone_cor_counter, file_id_c, fds_rk_phone_cor_counter);
//		fds_init_flash(&fds_soft_version, file_id_c, fds_rk_soft_version);
		
		fds_init_flash(&cur_cor_max, file_id_c, fds_rk_cur_cor_max);
		fds_init_flash(&cur_hr_max, file_id_c, fds_rk_cur_cor_max+1);		
		fds_init_flash(&cur_res_max, file_id_c, fds_rk_cur_cor_max+2);	
		
		fds_init_flash(&fds_clear_counter, file_id_c, fds_rk_clear_counter);
		fds_init_flash(&life_counter, file_id_c, fds_rk_clock);
	}
	
	else
	
	{
		SEGGER_RTT_printf(0, "init=%d\r\n", fds_is_values_init);
	}
	
}


ret_code_t fds_write_value(uint32_t* value, uint16_t file_id, uint16_t rec_key)
{
		fds_record_desc_t record_desc;
		fds_record_t        record;
		// Set up record.
		record.file_id              = file_id;
		record.key              		= rec_key;
		record.data.p_data = value;
		record.data.length_words   = 1;
		write_flag = 0;
		ret_code_t ret;
		if(!fds_no_space)
		{
			ret = fds_record_write(&record_desc, &record);
		}
	  
		if (ret != FDS_SUCCESS)
		{
				return ret;
		}
		// SEGGER_RTT_printf(0,"Writing Record ID = %d, value = %d\r\n",record_desc.record_id, *value);
		return NRF_SUCCESS;
}


ret_code_t fds_update_value(uint32_t* value, uint16_t file_id, uint16_t rec_key)
{
		fds_record_desc_t record_desc;
		fds_record_t        record;
		ret_code_t ret;

	  //Set up record.
		record.file_id              = file_id;
		record.key              		= rec_key;
		record.data.p_data= value;
		record.data.length_words= 1;
		fds_find_token_t    ftok ={0};//Important, make sure you zero init the ftok token
		
		if(!fds_no_space)
		{
			fds_record_find(file_id, rec_key, &record_desc, &ftok);
			ret = fds_record_update(&record_desc, &record);
//		SEGGER_RTT_printf(0, "valid rec = %d\n", stat.valid_records);
//		SEGGER_RTT_printf(0, "dirty rec = %d\n", stat.dirty_records);
		}
		
		if (ret == FDS_ERR_NO_SPACE_IN_FLASH)
		{
				fds_no_space = 1;
				SEGGER_RTT_printf(0, "no space\n\r");
				
				uint32_t rc = fds_stat(&stat);
				APP_ERROR_CHECK(rc);
			
			
				
				ret_code_t ret = fds_clear();
				nrf_delay_ms(200);
			//	fds_clear();
			//	fds_clear_counter++;
			//	
				return ret;
		} else{
			fds_update_counter++;
			//SEGGER_RTT_printf(0, "f_c=%d\n", fds_update_counter);
		}
			
	//	SEGGER_RTT_printf(0,"REC = %d, value = %d\r\n",record_desc.record_id, *value);
		return NRF_SUCCESS;
}

ret_code_t fds_read_value (uint32_t* data, uint16_t file_id, uint16_t rec_key)
{
	  fds_record_desc_t record_desc;
		fds_flash_record_t  flash_record;
		fds_find_token_t    ftok ={0};//Important, make sure you zero init the ftok token
		uint32_t err_code;
		
		SEGGER_RTT_printf(0,"Start new searching... \r\n");
		fds_record_find(file_id, rec_key, &record_desc, &ftok);
		err_code = fds_record_open(&record_desc, &flash_record);
		data = (uint32_t *) flash_record.p_data;
				
		SEGGER_RTT_printf(0,"data = \r\n", *data);
		err_code = fds_record_close(&record_desc);
		APP_ERROR_CHECK(err_code);
		
		return NRF_SUCCESS;
}

ret_code_t fds_get_data(uint32_t* value, uint16_t file_id, uint16_t rec_key)
{
	  fds_record_desc_t record_desc;
		fds_flash_record_t  flash_record;
		fds_find_token_t    ftok ={0};//Important, make sure you zero init the ftok token
		uint32_t err_code;
		
		//SEGGER_RTT_printf(0,"Start getting data... \r\n");
		fds_record_find(file_id, rec_key, &record_desc, &ftok);
		err_code = fds_record_open(&record_desc, &flash_record);
		if (err_code != FDS_SUCCESS)
		{
				return err_code;
		}
		*value = *((uint32_t *) flash_record.p_data);
				
		//SEGGER_RTT_printf(0,"I got Data_ = %d", *value);
		//SEGGER_RTT_printf(0,"\r\n");
		err_code = fds_record_close(&record_desc);
		if (err_code != FDS_SUCCESS)
		{
				return err_code;
		}
		return NRF_SUCCESS;
	//	APP_ERROR_CHECK(err_code);
}

ret_code_t fds_clear(void)
{
	ret_code_t ret = fds_gc();
	if (ret != FDS_SUCCESS)
		{
				return ret;
		}
		return NRF_SUCCESS;
}

ret_code_t fds_test_init (void)
{
		
		ret_code_t ret = fds_register(fds_evt_handler);
		if (ret != FDS_SUCCESS)
		{
			//SEGGER_RTT_printf(0, "%d\n\r", ret);
					return ret;
		}
		ret = fds_init();
		if (ret != FDS_SUCCESS)
		{
				return ret;
		}
		
		return NRF_SUCCESS;
}

ret_code_t fds_test_find_and_delete (uint16_t rec_key, uint16_t file_id_del)
{
	
		fds_record_desc_t   record_desc;
		fds_find_token_t    ftok ={0};;
		ftok.page=0;
		ftok.p_addr=NULL;
		// Loop and find records with same ID and rec key and mark them as deleted. 
	 
			while(fds_record_find(file_id_del, rec_key, &record_desc, &ftok) == FDS_SUCCESS)
			{
				delete_flag = 0;
				fds_record_delete(&record_desc);
				SEGGER_RTT_printf(0,"Deleted record ID: %d \r\n",record_desc.record_id);
				
			}
				//while(delete_flag == 0);
//			
//			while(fds_record_find(file_id, rec_del+2, &record_desc, &ftok) == FDS_SUCCESS)
//			{
//				fds_record_delete(&record_desc);
//				//SEGGER_RTT_printf(0,"ID: %d \r\n",record_desc.record_id);
//			}
//			
//				while(fds_record_find(file_id, rec_del+1, &record_desc, &ftok) == FDS_SUCCESS)
//			{
//				fds_record_delete(&record_desc);
//				SEGGER_RTT_printf(0,"ID: %d \r\n",record_desc.record_id);
//			}
//			
//			nrf_delay_ms(200);
//			
//		}		
//			for (uint16_t i = 0x0001; i < 0x0019; i++)
//				{
//						fds_record_find(file_id, i, &record_desc, &ftok);
//						fds_record_delete(&record_desc);
//						SEGGER_RTT_printf(0,"ID: %d \r\n",record_desc.record_id);
//						//nrf_delay_ms(100);
//				}
		
		// call the garbage collector to empty them, don't need to do this all the time, this is just for demonstration
			//fds_gc();
			return NRF_SUCCESS;
}
