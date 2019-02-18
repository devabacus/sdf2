#ifndef M_INTERFACE_H__
#define M_INTERFACE_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "mem_manager.h"


typedef	void (*interface_evnt_handler_t)(void*);


typedef enum
{
	INTERFACE_INIT,
	INTERFACE_WEIGHT,
	INTERFACE_CORRECTION_SELECT,
	INTERFACE_CORRECTION_ACTIVATE,
	INTERFACE_CORRECTION_DEACTIVATE,
	INTERFACE_CORRECTION_CANCEL,
	INTERFACE_CORRECTION_EDIT,
	INTERFACE_CORRECTION_EDIT_SAVE,
	INTERFACE_CORRECTION_EDIT_CANCEL,
	INTERFACE_MODE_CHANGE,
}interface_evt_t;


typedef enum
{
	MINUS,
	PLUS,
	PERCENT,
}v_type_t;


typedef enum
{
	MAIN,
	EDIT,
	MENU,
}interface_state_t;

typedef struct
{
	bool 		active;
	uint8_t corr_n;
	char 	* name;
	uint16_t value;
	v_type_t v_type;
	uint16_t compensation;
}correction_t;

typedef struct
{
	bool			use_grams;
	bool			edit_name;
	bool			mode;
	
	float			weight;
	
	uint8_t   rssi;
	char 		  bat;
	
	uint8_t		menu_counter;
		
	correction_t 					 * current_corr;           
	uint8_t			 					 * p_arr;
	interface_evnt_handler_t interface_evnt_handler;
	interface_evt_t 				 interface_evt;
	
	interface_state_t 	interface_state;
	
}interface_t;


#endif
