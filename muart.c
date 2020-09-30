#include "muart.h"
#include "hx711.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_ppi.h"
#include "LoRa.h"
#include "m_interface.h"

#define UART_TIME_SEND 1

uint32_t weight_float = 0;
uint8_t data_array[20];
uint32_t startWeightIndex = 6;
uint32_t endWeightIndex 	 = 15;
// send rs-232 message
uint32_t uart_ble_mode 		 = 0;
uint16_t clock_counter_last = 0;
uint16_t time_changed = 0;
uint16_t weight_diff = 0;
uint8_t weight_change_dir = 0;
uint8_t weight_monoton_counter = 0;
uint8_t try_stable = 0;
uint8_t sent_stable_weight = 0;
uint8_t uart_active = 0;
//uint8_t discrete = 10;

//uint8_t ble_connection = 0;

float uart_weight_f = 0;
int uart_weight 	 = -1;
char uart_weight_ch[5];
float uart_weight_f_last = 0; 
int uart_weight_last	 = 0;
int uart_weight_max = 0;
//пропуски блинка 17 светодиода для индикации получения веса с uart
uint8_t counter_uart_blink = 0;
float uart_weigth_f_max = 0;
// в эту переменную пишем либо флоат либо инт переменной uart_weight
float uart_weight1 = 0;
//APP_TIMER_DEF(m_timer_muart);

enum {
	INCREASE = 1,
	DECREASE,
};

void time_check(void){
			if(clock_counter != clock_counter_last){
				clock_counter_last = clock_counter;
			  time_changed++;
				//segnum1(time_changed);
			}
}

void flushIndexOfArray(uint8_t *buffer, uint8_t ind){
	for (uint8_t f = ind; f < strlen((char*) buffer); f++){
		buffer[f] = 0;
	}
}

void weight_handle()
	{
			if(uart_weight != uart_weight_last)
					{
						weight_diff = abs(uart_weight - uart_weight_last);
							//Вес увеличивается
							if(uart_weight > uart_weight_last)
								{
									if(weight_change_dir == INCREASE)
										{
											weight_monoton_counter++;
									
										}
									else if (weight_change_dir == DECREASE || !weight_change_dir)
										{
											weight_change_dir = INCREASE;
											weight_monoton_counter = 0;
											time_check();
										}
								} 
							//Вес уменьшается	
							else if (uart_weight < uart_weight_last)
								{
									if(weight_change_dir == DECREASE)
										{
											weight_monoton_counter++;							
										}
									else if (weight_change_dir == INCREASE || !weight_change_dir)
										{
											weight_change_dir = DECREASE;
											weight_monoton_counter = 0;
											time_check();
										}
								}
							else 
								{
									weight_monoton_counter = 0;
								}
							if(weight_monoton_counter >= 3)
								{
										//SEGGER_RTT_printf(0, "weight = %d, dir = %d, diff = %d, mon_counter = %d\n", uart_weight, weight_change_dir, weight_diff, weight_monoton_counter);
										sprintf(uart_weight_ch, "%d", uart_weight);
										lora_write_with_flag(REMOTE_WEIGHT, (uint8_t*)uart_weight_ch, strlen(uart_weight_ch));	
										sent_stable_weight = 0;
										time_changed = 0;
								}
							else 
								{
									if(!sent_stable_weight && time_changed >=2)
										{
											//SEGGER_RTT_printf(0, "weight = %d, time_changed = %d\n", uart_weight, time_changed);
											sprintf(uart_weight_ch, "%d", uart_weight);
											lora_write_with_flag(REMOTE_WEIGHT, (uint8_t*)uart_weight_ch, strlen(uart_weight_ch));	
											sent_stable_weight = 1;
										}
								}
								sprintf(uart_weight_ch, "%d", uart_weight);
								lora_write_with_flag(REMOTE_WEIGHT, (uint8_t*)uart_weight_ch, strlen(uart_weight_ch));	
					}
			uart_weight_last = uart_weight;
	}

void weight_ble_msg(void){
		uint8_t weight_pref[] = "wt";
		uint16_t length = strlen((char*)uart_weight_ch);
		memcpy(weight_pref+2, uart_weight_ch, length);
		ble_comm_send_handler(weight_pref);
}

void send_uart_msg(void){
	
	if(uart_ble_mode == 1){
		//segtext(data_array);
		ble_comm_send_handler(data_array);
		
	} else if (uart_ble_mode == 2){
		//segtext(data_array + startWeightIndex);
		//segtext("\n");
		ble_comm_send_handler(data_array + startWeightIndex);
	}
}

void define_uart_weight(void){
	// если отправили s8/2 через телефон то weight_float = 1
//	weight_handle();
	if(!weight_float){
		flushIndexOfArray(data_array, endWeightIndex);
		uart_weight = atoi((char*)(data_array+startWeightIndex));		
		
		if(ble_active){
		
					if(uart_weight != uart_weight_last){
					time_changed=0;
					sprintf(uart_weight_ch, "%d", uart_weight);
					uart_weight_last = uart_weight;
					//SEGGER_RTT_printf(0, "%d\n", uart_weight);
					if(uart_weight_last > uart_weight_max){
						uart_weight_max = uart_weight;
					}
					if(!uart_weight_last){
		//			SEGGER_RTT_printf(0, "uart_weight_max = %d\n", uart_weight_max);
						uart_weight_max = 0;
					}
				//ble_comm_send_handler((uint8_t*)uart_weight_ch);
					weight_ble_msg();
				} else {
					if(time_changed <= UART_TIME_SEND){
					time_check();
					sprintf(uart_weight_ch, "%d", uart_weight);
					//uart_weight_last = uart_weight;
					weight_ble_msg();						
		}
		}
		if(!uart_weight) uart_weight = uart_weight_last;
		} else {
			// if need to send weight by lora
			//weight_handle();
		}
////		else {
//				#ifdef LORA_USE
////				if(abs(uart_weight - uart_weight_last)>200){
//					if((uart_weight != uart_weight_last)){
//					sprintf(uart_weight_ch, "%d", uart_weight);
//						//segtext("lora send\n");
//						SEGGER_RTT_printf(0, "%s\n", (uint8_t*)uart_weight_ch);
//					lora_write_with_flag(REMOTE_WEIGHT, (uint8_t*)uart_weight_ch, strlen(uart_weight_ch));
//					time_changed = 0;
//				} 
//				else {
//					time_check();
//					if(time_changed == 2){
//				//		lora_write_with_flag(REMOTE_WEIGHT, (uint8_t*)uart_weight_ch, strlen(uart_weight_ch));
//						time_changed = 0;
//					}
//				}
//				if(!uart_weight) uart_weight = uart_weight_last;
//			#endif		
//				
//		}
	}
	else {
		flushIndexOfArray(data_array, endWeightIndex);
		uart_weight_f = atof((char*)data_array+startWeightIndex);	
		if(uart_weight_f != uart_weight_f_last){
			time_changed=0;
			sprintf(uart_weight_ch, "%.2f", uart_weight_f);
			uart_weight_f_last = uart_weight_f;
			//ble_comm_send_handler((uint8_t*)uart_weight_ch);
			
			weight_ble_msg();
		} else {
						time_check();
						if(time_changed <= UART_TIME_SEND){
								sprintf(uart_weight_ch, "%.2f", uart_weight_f);
								uart_weight_f_last = uart_weight_f;
								//segtext(uart_weight_ch);
								//ble_comm_send_handler((uint8_t*)uart_weight_ch);
								weight_ble_msg();
						} 
		}
		if(!uart_weight_f) uart_weight_f = uart_weight_f_last;
		
	}

}

void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t index = 0;
   // uint32_t       err_code;

    switch (p_event->evt_type)
    {
			
        case APP_UART_DATA_READY:
							//blink by led when data is receiving
							//uart_active = 1;
							if(uart_ble_mode) {
									//nrf_gpio_pin_toggle(17);
								
								if(uart_weight > 0){
									
									if(counter_uart_blink == 100){
										nrf_gpio_pin_toggle(17);
										counter_uart_blink = 0;
									}
									counter_uart_blink++;
								}
								else if(uart_weight == 0)
								{
									nrf_gpio_pin_toggle(17);
								}
							 
							}
							//nrf_delay_ms(100);
							app_uart_get((uint8_t*)&data_array[index]);
							//remove spaces for gs7516
							
							if(data_array[index] != ' ') index++;
//							if(index == 10) {
//								SEGGER_RTT_printf(0, "%s\n", data_array);
//							}
							if (data_array[index - 1] == '\n'){
									//SEGGER_RTT_printf(0, "%s\n", data_array);
								
									uart_active = 1;
									define_uart_weight();
									nrf_gpio_pin_toggle(17);
									send_uart_msg();
									for(uint8_t i = 0; i < 20; i++){
												data_array[i] = 0;
									}
									 app_uart_flush();
									 index = 0;
								} 
    }
}

void uart_init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = 26,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
        .baud_rate    = NRF_UART_BAUDRATE_9600
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}
