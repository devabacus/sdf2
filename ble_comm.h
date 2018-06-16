#ifndef BLE_COMM_H__
#define BLE_COMM_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_nus.h"
#include "scale_setup.h"
#include "flash_storage.h"
#include "device_name.h"
#include "hx711.h"
#include "ble_debug.h"
#include "ble_correct.h"
#include "ble_set.h"
#include "muart.h"


extern uint8_t ble_active;
void ble_comm(uint8_t * buffer);
void zero(uint8_t times, uint16_t delay);




extern uint8_t ble_string_put[20];
int findIdexOfArray(uint8_t *buf, int startIndex, char character);


extern void ble_comm_send_handler(uint8_t * buf);
void ble_comm_send_num_handler(uint32_t num);

#endif
