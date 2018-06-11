#include "device_name.h"


uint8_t device_name[13] = DEVICE_NAME;
uint8_t activation_code[9] = "";
uint8_t activation_code_mac[9] = "";

uint32_t activation_num = 0;

ble_gap_addr_t mac_address = {0};

uint32_t demo;
uint32_t demo1 = 0;
uint32_t demo2 = 0;
uint32_t demo3 = 0;
uint32_t demo4 = 0;
uint32_t demo5 = 0;
uint32_t demo6 = 0;

//test1234
//721113101
//121011101

void demo_define(void){
		
	memcpy(activation_code_mac, mac_address.addr, sizeof(mac_address.addr));
//		for(uint8_t i = 0; i < 6; i++){
//			SEGGER_RTT_printf(0, "%x:", activation_code_mac[i]);
//		}
//			segtext("\n");
		for(uint8_t i = 0; i < 6; i++){
			activation_num += activation_code_mac[i];
			
		}
			demo1 = activation_num * activation_code_mac[0] + activation_code_mac[4];
			demo2 = activation_num * activation_code_mac[1] + activation_code_mac[3];
			demo3 = activation_num * activation_code_mac[2] + activation_code_mac[2];
			demo4 = activation_num * activation_code_mac[3] + activation_code_mac[1];
			demo5 = activation_num * activation_code_mac[4] + activation_code_mac[0];
			demo6 = activation_num * activation_code_mac[5] + activation_code_mac[5];
			

		
		
				fds_mac_init = 1;
//generate_admin_pass1();
		//segnum1(admin);
		
}




void name_convert(void) {
	
	
	memcpy(activation_code, device_name, sizeof(device_name));
//	//activation_code[2] = '9';
	activation_code[0] = activation_code[8];
	activation_code[3] = activation_code[7];
	activation_code[6] = activation_code[2];
	activation_code[5] = activation_code[4];
	
	
	
	
	demo = atoi((char*)activation_code);
	demo1 = demo/4218 - 6219;  //22470
	demo2 = demo/7123 - 3113;  //13875
	demo3 = demo/5512 - 1231;  //20723
	demo4 = demo/3138 - 7131;	 //31432
	demo5 = demo/1921 - 1214;  //61779
	

		
	
	
	//ABD -С- ABD
	//473 -С- 210   Число 473210
	// 22     384

}


