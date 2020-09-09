#ifndef DEVICE_NAME_H__
#define DEVICE_NAME_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_nus.h"
#include "scale_setup.h"
#include "flash_storage.h"

//Для прошивки рабочей версии строка ниже должна быть зеленой

//#define DEBUG_MODE
#define LORA_USE
//#define CHECK_CONFIG 1
////////////////////////////////////////////////////////
#ifndef DEBUG_MODE
#define DEVICE_NAME                     "sdr_5.5.5_1"                         /**< Name of device. Will be included in the advertising data. */
#endif

#ifdef DEBUG_MODE
#define DEVICE_NAME											"debug_5.5.5_1"
#endif

//728123108
//721113101
//5_4_4 change pin for tx

extern uint8_t device_name[17];
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
