#ifndef SCALE_SETUP_H__
#define SCALE_SETUP_H__

#include <stdlib.h>
#include "hx711.h"
#include "flash_storage.h"
#include "remote.h"
#include "device_name.h"
#include "offset_perc.h"
#include "ble_comm.h"
#include "weight.h"
#include "m_interface.h"



#define NEWBIE_CONFIG 	1
#define PROFI_CONFIG	2 	
#define EXPERT_CONFIG	3


#define SCALE_600   					1   
#define SCALE_5000  					2			
#define SCALE_AUTOVESI				3

#define REMOTE_ONLY						3
#define PHONE_ONLY						4
#define REMOTE_PHONE					5


#define TIME_FEEDBACK 				500

#define ADMIN_TIME_MINUTES		30





extern uint8_t  	scale_feedback;
extern uint32_t 	activate_code;
extern uint32_t 	cor_feedback;
extern uint32_t 	time_feedback;
extern uint32_t 	feedback;
extern uint16_t 	clock_counter;
extern uint8_t  	admin;
extern uint8_t  	root;
extern uint32_t 	admin_pass;
extern uint32_t 	current_life_counter;

extern uint32_t num_of_discrete_for_cal;

extern uint32_t test_activate_code;

uint32_t demo_passes(uint32_t demo_test, uint8_t num_st, uint8_t num_end);

void check_pass(void);
void check_option_pass(void);
void change_correct_mode(void);
//void change_correct_mode();

void scale_setup(void);

void save_call_data(void);

void start_led(void);
void change_num_cor_but(uint8_t num);
void generate_admin_pass1(void);
void check_config_pass(void);
#endif
