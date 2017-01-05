#include "RGBW_LEDs_driver.h"


uint8_t sym[40];

void return_sym(uint8_t tab1[105], uint8_t pos) {
	uint8_t i, j=0, k, l=0;
	for (i=0; i<105; i++) {
		if(i == pos+l) {
			for(k=0; k<8; k++) {
				sym[k+j]=tab1[k+l+pos];
			}
			j += 8; l += 21;
		}
	}
}	

int main() {
	uint8_t i;
	uint8_t sms[40] = {1, 1, 1,  1,  1,  1, 1, 1, 
										 1, 1, 0,  0,  0,  0, 1, 1,
										 1, 0, 1,  1,  1,  1, 0, 1,
										 1, 1, 0,  0,  0,  0, 1, 1,
										 1, 1, 1,  1,  1,  1, 1, 1};
	
  uint8_t symbol2[40] = {1, 1, 1,  1,  1,  1, 1, 1, 
												 1, 0, 1,  0,  0,  1, 0, 1,
												 1, 0, 1,  1,  1,  1, 0, 1,
												 1, 1, 0,  0,  0,  0, 1, 1,
												 1, 1, 1,  1,  1,  1, 1, 1};	
	
	uint8_t symbol3[105] = {0, 0, 1, 1, 1,  0,  1, 0, 0, 0, 1, 0, 1, 1, 1, 0,  1,  1, 1, 0, 0, 
												  0, 0, 1, 0, 0,  0,  1, 1, 0, 1, 1, 0, 1, 0, 1, 0,  0,  0, 1, 0, 0,
												  0, 0, 1, 1, 1,  0,  1, 0, 1, 0, 1, 0, 1, 1, 1, 0,  1,  1, 1, 0, 0, 
												  0, 0, 0, 0, 1,  0,  1, 0, 0, 0, 1, 0, 1, 0, 0, 0,  1,  0, 0, 0, 0,
												  0, 0, 1, 1, 1,  0,  1, 0, 0, 0, 1, 0, 1, 0, 0, 0,  1,  1, 1, 0, 0};

	init_Device();
	
	for (;;) {
		/* G-R-B */
		/*
		set_matrix_off();									//set elements in pixel tables for second diode
		return_sym(symbol3, 0);
    turn_symbol(sym, 0, 50, 50);
		//turn_pixel_on(39, 100, 0, 0);
		start_DMA();
		delay_mc(400);
		set_matrix_off();
		return_sym(symbol3, 1);
		turn_symbol(sym, 0, 50, 50);
		//turn_pixel_on(38, 0, 0, 100);
	  start_DMA();	
		delay_mc(400);
		set_matrix_off();
		return_sym(symbol3, 2);
		turn_symbol(sym, 0, 0, 50);
		//turn_pixel_on(38, 0, 0, 100);
	  start_DMA();	
		delay_mc(400);
		*/
		for (i=0; i<14; i++) {
			set_matrix_off();
			return_sym(symbol3, i);
			turn_symbol(sym, 0, 0, 50);
			start_DMA();
			delay_mc(200);
		}
	}
		
}
