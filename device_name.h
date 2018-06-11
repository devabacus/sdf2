#ifndef DEVICE_NAME_H__
#define DEVICE_NAME_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_nus.h"
#include "scale_setup.h"
#include "flash_storage.h"

// Для прошивки рабочей версии строка ниже должна быть зеленой
//#define DEBUG_MODE 1

////////////////////////////////////////////////////////

#define DEVICE_NAME                     "scale-driver2"                         /**< Name of device. Will be included in the advertising data. */


//728123108
//721113101


extern uint8_t device_name[13];
extern uint8_t activation_code[9];
extern uint8_t activation_code_mac[9];
extern ble_gap_addr_t mac_address;

extern uint32_t demo1;
extern uint32_t demo2;
extern uint32_t demo3;
extern uint32_t demo4;
extern uint32_t demo5;
extern uint32_t demo6;



void demo_define (void);

#endif
