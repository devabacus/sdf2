#ifndef BLE_CORRECT_H__
#define BLE_CORRECT_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_nus.h"
#include "scale_setup.h"
#include "flash_storage.h"
#include "device_name.h"
#include "correct.h"
#include "ble_comm.h"


#define PHONE_COR_COUNTER_MAX_DEMO  200 
#define PHONE_COR_COUNTER_MAX 			3000

void ble_correct(uint8_t * buffer);
extern uint8_t ble_string_put1[10];
extern uint8_t ble_correct_active;
void remote_but_update(uint8_t but_num);
extern uint32_t phone_cor_counter;

#endif
