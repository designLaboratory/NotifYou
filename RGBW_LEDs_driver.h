/**
*@file RGBW_LEDs_driver.h
*@author Szymon Sarna, Damian Mucha
*@date 19 Feb 2017
*@brief Header file of AdaFruit's WS2812b RGBW LEDs driver
*To drive a matrix of WS2812b LEDs, in our solution we're using 1 DMA channel and 2 TPM channels 
*Data is sent to matrix as DMA tables which are timed using 2 channels of TPM's.
*TPM's are configured to trigger DMA transfer on overflow.
*/
#ifndef RGBW_LEDs_driver_H
#define RGBW_LEDs_driver_H

/**
* @brief Include header of our Kinetis board  
*/
#include "MKL46Z4.h"


/*----------------------------------------------------------------------------
			FUNCTIONS DECLARATION
 *----------------------------------------------------------------------------*/
 
/*----- Initialization of modules -----*/
/**
 *****************************************************************************************
 *  @brief      Modules Initialization
 *
 *  Call to clock used modules and initialize GPIO port.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void init_modules(void);

/**
 *****************************************************************************************
 *  @brief      TPM Initialization
 *
 *  Call to initialize 2 TPM channels as Edge Alligned PWM signals which will steer proper LEDs.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void init_tpm(void);

/**
 *****************************************************************************************
 *  @brief      DMA Initialization
 *
 *  Call to initialize 3 channels of DMA module and specify their sources.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void init_dma(void);

/**
 *****************************************************************************************
 *  @brief      DMA Tables Initialization
 *
 *  Call to initialize 5 DMA tables and specify their starting values.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void init_dma_tables(void);

/**
 *****************************************************************************************
 *  @brief      LED Matrix Initialization
 *
 *  Call to set starting values of pixel matrix and "dma_words" table.
 *	Select Proper PIN as DMA0 transfer output.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void init_led_matrix(void);

/**
 *****************************************************************************************
 *  @brief      Devices Initialization
 *
 *  Call all above initialization functions.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void init_Device(void);

/*----- IRQ Handlers -----*/
/**
 *****************************************************************************************
 *  @brief      DMA0 IRQ handler
 *
 *  Called when DMA is done. Resets DMA_done status and TMP timers.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void DMA0_IRQHandler(void);

/**
 *****************************************************************************************
 *  @brief      TPM0 IRQ handler
 *
 *  Called when TPM0 overflows. Sets DMA_done status.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void TPM0_IRQHandler(void);

/**
 *****************************************************************************************
 *  @brief      DMA done handler
 *
 *  Called when DMA transfer is proceeding. Be in sleep mode until transfer is done.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void dma_done(void);

/*----- Main functions -----*/

/**
 *****************************************************************************************
 *  @brief      Start DMA transfer
 *
 *  Call when DMA tables are ready to transfer. 
 *	Fill Adress and Data fields in DMA channels. Configure DMA Control Register.
 *	Transfer DMA words to selected adress.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void start_DMA(void);

/**
 *****************************************************************************************
 *  @brief      Turn selected pixel on
 *
 *  Call to fill DMA_data table with proper values. 
 * 
 *  @param      num_pixel - Number of pixel in LEDs matrix.
 *  @param      green - How "green" LED needs to be.
 *  @param      red - How "red" LED needs to be.
 *  @param      blue - How "blue" LED needs to be.
 *
 *  @return     No return value
 ****************************************************************************************/
void turn_pixel_on(uint16_t num_pixel, uint8_t green, uint8_t red, uint8_t blue);

/**
 *****************************************************************************************
 *  @brief      Turn selected pixel off
 *
 *  Call to clear DMA_data table on proper LED position. 
 * 
 *  @param      num_pixel - Number of pixel in LEDs matrix.
 *
 *  @return     No return value
 ****************************************************************************************/
void turn_pixel_off(uint16_t num_pixel);

/**
 *****************************************************************************************
 *  @brief      Turn whole LED matrix off
 *
 *  Call to clear whole DMA_data table. 
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void set_matrix_off(void);

/**
 *****************************************************************************************
 *  @brief      Delaying function
 *
 *  Call to delay next process for selected time. 
 * 
 *  @param      value - How much delay you need (in millis).
 *
 *  @return     No return value
 ****************************************************************************************/
void delay_mc(uint32_t value);

/**
 *****************************************************************************************
 *  @brief      Turn selected symbol
 *
 *  Call fill DMA_data tables with proper data to set selected symbol on LED matrix.
 * 
 *  @param      matrix[] - Selected matrix telling function which LEDs to set.
 *  @param      green - How "green" LED needs to be.
 *  @param      red - How "red" LED needs to be.
 *  @param      blue - How "blue" LED needs to be.
 *
 *  @return     No return value
 ****************************************************************************************/
void turn_symbol(uint8_t matrix[], uint8_t green, uint8_t red, uint8_t blue);

/**
 *****************************************************************************************
 *  @brief      Return selected symbol
 *
 *  Call to "cut" part of selected 60-symbol matrix and send it to "Sym_part" 40-symbol matrix.
 * 
 *  @param      notif_tab[60] - Selected bigger matrix .
 *  @param      pos - Parameter telling function which part of.
 *
 *  @return     No return value
 ****************************************************************************************/
void return_sym(uint8_t notif_tab[60], uint8_t pos);

/**
 *****************************************************************************************
 *  @brief      Display selected symbol
 *
 *  Call to display selected 60-symbol matrix and scroll it through a screen.
 * 
 *  @param      matrix[] - Selected matrix telling function which LEDs to set.
 *  @param      green - How "green" symbol needs to be.
 *  @param      red - How "red" symbol needs to be.
 *  @param      blue - How "blue" symbol needs to be.
 *
 *  @return     No return value
 ****************************************************************************************/
void display_symbol(uint8_t sym_matrix[60], uint8_t green, uint8_t red, uint8_t blue);

/**
 *****************************************************************************************
 *  @brief      Scroll symbols
 *
 *  Call to scroll unanswered notifications through a screen.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void scroll_syms(void);

#endif
