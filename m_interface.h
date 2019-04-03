#ifndef M_INTERFACE_H__
#define M_INTERFACE_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "mem_manager.h"


typedef	void (*interface_evnt_handler_t)(void*);


enum REMOTE_EVENT
{
	REMOTE_INIT,
	REMOTE_WEIGHT,
	REMOTE_CORRECTION_SELECT,
	REMOTE_CORRECTION_ACTIVATE,
	REMOTE_CORRECTION_DEACTIVATE,
	REMOTE_CORRECTION_CANCEL,
	REMOTE_CORRECTION_EDIT,
	REMOTE_CORRECTION_EDIT_SAVE,
	REMOTE_CORRECTION_EDIT_CANCEL,
	REMOTE_MODE_CHANGE,
};




typedef enum COR_VALUE_TYPE
{
	PERCENT,
	MINUS,
	PLUS
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
	v_type_t  v_type;
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
	uint8_t 				 interface_evt;
	
	interface_state_t 	interface_state;
	
}interface_t;


#endif
