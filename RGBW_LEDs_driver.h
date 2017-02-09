#ifndef RGBW_LEDs_driver_H
#define RGBW_LEDs_driver_H

/* INCLUDES */
#include "MKL46Z4.h"


/*----------------------------------------------------------------------------
			FUNCTIONS DECLARATION
 *----------------------------------------------------------------------------*/
//Initialization of modules
void init_modules(void);
void init_tpm(void);
void init_dma(void);
void init_dma_tables(void);
void init_led_matrix(void);
void init_Device(void);
//IRQ Handlers
void DMA0_IRQHandler(void);
void TPM0_IRQHandler(void);
void dma_done(void);
//Main functions
void start_DMA(void);
void turn_pixel_on(uint16_t num_pixel, uint8_t green, uint8_t red, uint8_t blue);
void turn_pixel_off(uint16_t num_pixel);
void set_matrix_off(void);
void delay_mc(uint32_t value);
void turn_symbol(uint8_t matrix[], uint8_t green, uint8_t red, uint8_t blue);
void return_sym(uint8_t notif_tab[60], uint8_t pos);
void display_symbol(uint8_t sym_matrix[60], uint8_t green, uint8_t red, uint8_t blue);
void scroll_syms(void);
#endif
