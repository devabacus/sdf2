#ifndef BLE_SET_H__
#define BLE_SET_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_nus.h"
#include "scale_setup.h"
#include "flash_storage.h"
#include "device_name.h"
#include "ble_comm.h"
#include "sd_util.h"

typedef struct 
{
	uint8_t showADC;
	uint8_t workADC;
	uint8_t adcBitForCut;
	uint8_t autocor;
} ble_settings_t; 

typedef enum
{
	SHOWADC = 1,
	ADCBIT,
	AUTOCOR,
	SAVEBUT,
	CALIBRATION, //5
	NUM_COR_BUT9,
	RESET_COUNTERS_BLE,
	UART,
	UART_ST_INDEX,
	UART_END_INDEX, //10
	UART_BLE, //11
	CHANGE_MAX_COUNTERS, //12
	OPTION_COUNTER,//13
	CAL_MAX_WEIGHT,//14
	CAL_DISCRET,//15
	CAL_LOAD_WEIGHT//16
} ble_type_sett_t;


void ble_set_init(void);
void set_send_cor_mode(uint8_t set_value);


extern uint8_t but_ble;
extern uint8_t showadc;
extern uint32_t uart_work;
extern uint8_t adcBitCut;
extern ble_settings_t ble_settings;
extern void ble_settings_handler(ble_settings_t ble_settings);
#endif
