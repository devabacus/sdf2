#ifndef BLE_DEBUG_H__
#define BLE_DEBUG_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_nus.h"
#include "scale_setup.h"
#include "flash_storage.h"
#include "device_name.h"
#include "ble_comm.h"


extern void ble_debug_handler(uint8_t * buf);

//typedef struct{
//	ble_debug_handler_t ble_debug_handler;
//} ble_debug_handler1_t;



extern uint8_t ble_string_put2[20];




#endif
