#include "sd_util.h"

void segtext(char* text) {
	SEGGER_RTT_printf(0, text);
}

void segnum1(uint32_t num1){
	SEGGER_RTT_printf(0, "%d\n", num1);
}




