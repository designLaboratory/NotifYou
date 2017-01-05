#include "RGBW_LEDs_driver.h"

/*----------------------------------------------------------------------------
			VARIABLES AND CONSTANTS DECLARATIONS
 *----------------------------------------------------------------------------*/
/* DMA TRANSFER */
#define DMA_LEAD_ZEROS  	 2																					//zeros for start period
#define DMA_TRAIL_ZEROS 	 1																					//zeros for start period
#define RGB_DIODE_BITS     24																					//Bits to configure one diode
#define NUM_OF_LEDS 			 40																					//leds number in strip
#define RGB_MATRIX_BITS		 RGB_DIODE_BITS*NUM_OF_LEDS									//bits needed to program all the leds
/* OUTPUT PIN */
#define OUT_PIN 				 	 2																					//D2 pin as output
#define NSEC_TO_TICKS(nsec) ((nsec)*42/1000)													//function to change time to clock ticks
#define USEC_TO_TICKS(usec) ((usec)*42)															  //function to change time to clock ticks

/*----------------------------------------------------------------------------
			Needed variables initialization
 *----------------------------------------------------------------------------*/
const uint32_t CLK_NSEC = 1250;																														//how much nanoseconds take one period for configure diode
const uint32_t tpm_period    = NSEC_TO_TICKS(CLK_NSEC); 																	//how much is it tiks, 52.5 tiks 52.5 * 23 ns = about 1210 nsec 
const uint32_t tpm_p0_period = NSEC_TO_TICKS(250);																				//how much tiks take high state in zero
const uint32_t tpm_p1_period = NSEC_TO_TICKS(650);																				//how much tiks take high state in zero
const uint32_t guardtime_period = USEC_TO_TICKS(55);   																		//guardtime minimum 50 usec, time between frames

uint8_t LED_tab[RGB_DIODE_BITS*3];           	 												//RGB table of pixels
uint16_t num_of_LEDs = NUM_OF_LEDS; 																	//Numbers of LEDs in strip as variable
uint32_t DMA_out_mask = 0;																						//DMA mask setting output
volatile uint8_t dma_done_status = 1;																	//DMA status variable (volatile intended)

/*----------------------------------------------------------------------------
			DMA DATA STRUCTURE
 *----------------------------------------------------------------------------*/
struct {
    uint8_t start_t1_low[ DMA_LEAD_ZEROS ];																					//leading zeros if dma_words
    uint8_t dma_words[ RGB_MATRIX_BITS ];																						//table of dma_words, size is 2*8 = 16, each element has 8 bits so 16 cases - 8 bits each						
    uint8_t trailing_zeros_1[ DMA_TRAIL_ZEROS ];																		//trailing zeros for dma)words
    uint8_t start_t0_high[ DMA_LEAD_ZEROS - 1 ];																		//zeros, first zeros before all_ones
    uint8_t all_ones[ RGB_MATRIX_BITS ];																						//ones to set output as 1
		uint8_t trailing_zeros_2[ DMA_TRAIL_ZEROS + 1 ];																//zeros to end of all_ones
} dma_data;

/*----------------------------------------------------------------------------
			DMA DATA STRUCTURE #1
 *----------------------------------------------------------------------------*/

enum DMA_MUX_SRC {																																				//enum for number of dma soures
    DMA_MUX_SRC_TPM0_CH_0  = 24,																													//number of dma source TPM0 CH0
    DMA_MUX_SRC_TPM0_CH_1,																																//number of dma source TPM0 CH1
    DMA_MUX_SRC_TPM0_OVFLW = 54,																												  //number of dma source TPM0 Overflow
};

enum DMA_CHANNEL {																																				//enum for DMA_MUX CHannels	
    DMA_CHANNEL_START = 0,																																//DMA_CHANNEL 0
    DMA_CHANNEL_0_LOW = 1,																																//DMA_CHANNEL 1
    DMA_CHANNEL_1_LOW = 2,																																//DMA_CHANNEL 2
};

/*----------------------------------------------------------------------------
			Clocking the ports and GPIO initialization
 *----------------------------------------------------------------------------*/
 void init_modules() {
	 /* CLOCKING THE PORTS */
	 SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	 SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	 SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	 SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	 
	 SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);																					//42 MHZ MCGFLLCLK as the clock source
	 
	 /* GPIO Initialization*/
	 PORTD->PCR[OUT_PIN] = PORT_PCR_MUX(1UL);																		//Selected pin as GPIO
	 PTD->PDDR |= DMA_out_mask;																									//Selected pin low state init																								
	 PTD->PDOR &= ~DMA_out_mask;
 }

 /*----------------------------------------------------------------------------
			 Timer Module Initialization
 *----------------------------------------------------------------------------*/
 void init_tpm() {
	TPM0->SC |= TPM_SC_PS(0);																										//Prescaler Value - 1
	TPM0->SC |= TPM_SC_DMA_MASK;																								//DMA enable in TPM0
	TPM0->SC |= TPM_SC_TOF_MASK;																								//Reset TPM0 overflow flag
	TPM0->SC |= TPM_SC_CMOD(0);																									//Clock in TPM0 disabled
	 
	TPM0->MOD = tpm_period - 1;																									//Set Mod Counter for 51
	/* TPMO_CH0 - DMA_enable, Edge Alligned PWM, clear output on match, set output on reload */ 
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_MSB_MASK;
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_ELSB_MASK;
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_DMA_MASK;
	/* TPMO_CH1 - DMA_enable, Edge Alligned PWM, clear output on match, set output on reload */ 
	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_MSB_MASK;
	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_ELSB_MASK;
	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_DMA_MASK;
	//Set counting values
  TPM0->CONTROLS[0].CnV = tpm_p0_period;																								//TPM0, CH0 will count to 250 ns
  TPM0->CONTROLS[1].CnV = tpm_p1_period;																								//TPM1, CH0 will count to 650 ns
  /* Clear pending and enable interrupts form TPM0 module */
	NVIC_ClearPendingIRQ(TPM0_IRQn);																											 
	NVIC_EnableIRQ(TPM0_IRQn);																														
 }
 
 /*----------------------------------------------------------------------------
			 DMA Module Initialization
 *----------------------------------------------------------------------------*/
 void init_dma() {
	/* Reset DMA channels */
  DMAMUX0->CHCFG[DMA_CHANNEL_0_LOW] = 0;
	DMAMUX0->CHCFG[DMA_CHANNEL_1_LOW] = 0;
	DMAMUX0->CHCFG[DMA_CHANNEL_START] = 0;
	/* Enable DMA channels */ 
	DMAMUX0->CHCFG[DMA_CHANNEL_0_LOW] = DMAMUX_CHCFG_ENBL_MASK
																		| DMAMUX_CHCFG_SOURCE(DMA_MUX_SRC_TPM0_CH_0); 		//t=tpm_p0_period: all of the 0 bits go low     
  DMAMUX0->CHCFG[DMA_CHANNEL_1_LOW] = DMAMUX_CHCFG_ENBL_MASK 													
																		| DMAMUX_CHCFG_SOURCE(DMA_MUX_SRC_TPM0_CH_1);			//t=tpm_p1_period: all outputs go low 
  DMAMUX0->CHCFG[DMA_CHANNEL_START] = DMAMUX_CHCFG_ENBL_MASK 													   
																		| DMAMUX_CHCFG_SOURCE(DMA_MUX_SRC_TPM0_OVFLW);    //t=0: all enabled outputs go high on TPM0 overflow PWM
	
	/* Clear pending and enable interrupts form TPM0 module */
	NVIC_ClearPendingIRQ(DMA0_IRQn);																											 
	NVIC_EnableIRQ(DMA0_IRQn);
 }
 /*----------------------------------------------------------------------------
			 DMA Transfer Initialization
 *----------------------------------------------------------------------------*/
 void init_dma_tables() {
	 uint16_t i = 0;																															//Helping Variable
		for (i=0; i<(RGB_MATRIX_BITS); i++) {
			dma_data.all_ones[i] = 255;																							//Initialize all_ones table
		}		
		for (i=0; i<DMA_LEAD_ZEROS; i++) {
			dma_data.start_t1_low[i] = 0;																						//Initialize start_t1_low table
		}
		for (i=0; i<DMA_TRAIL_ZEROS; i++) {
			dma_data.trailing_zeros_1[i] = 0;																				//Initialize trailing_zeros_1 table
		}
		for (i=0; i<DMA_LEAD_ZEROS-1; i++) {
			dma_data.start_t0_high[i] = 0;																					//Initialize start_t0_high table
		}
		for (i=0; i<DMA_TRAIL_ZEROS+1-1; i++) {
			dma_data.trailing_zeros_2[i] = 0;																				//Initialize trailing_zeros_2 table
		}
 }
 /*----------------------------------------------------------------------------
			 WS2812 LEDs Matrix Initialization
 *----------------------------------------------------------------------------*/
 void init_led_matrix() {
	uint16_t i = 0;																															//Helping Variable
		for (i = 0; i < (NUM_OF_LEDS*3); i++) {																	 	//clear matrix of pixels
				LED_tab[i] = 0;
		}
		
		for (i = 0; i < (RGB_MATRIX_BITS); i++) {																	//Clear table dma_words for DMA
			dma_data.dma_words[i] = 0;
		}
		
	  DMA_out_mask = 1UL << OUT_PIN; 																						//mask for selected pin
 }
 
/*----------------------------------------------------------------------------
			DMA end of transfer interrupt handler
 *----------------------------------------------------------------------------*/
 void DMA0_IRQHandler() {
	uint32_t val;																																//Helping variable
	val = DMA0->DMA[DMA_CHANNEL_0_LOW].DSR_BCR;
	 if (val & DMA_DSR_BCR_DONE_MASK) {
		DMA0 -> DMA[DMA_CHANNEL_0_LOW].DSR_BCR = DMA_DSR_BCR_DONE_MASK;						//Reset DMA status if transfer is done
	 }
	val = DMA0->DMA[DMA_CHANNEL_1_LOW].DSR_BCR;
	 if (val & DMA_DSR_BCR_DONE_MASK) {
		DMA0 -> DMA[DMA_CHANNEL_1_LOW].DSR_BCR = DMA_DSR_BCR_DONE_MASK;						//Reset DMA status if transfer is done
	 }
	val = DMA0->DMA[DMA_CHANNEL_START].DSR_BCR;
	 if (val & DMA_DSR_BCR_DONE_MASK) {
		DMA0 -> DMA[DMA_CHANNEL_START].DSR_BCR = DMA_DSR_BCR_DONE_MASK;						//Reset DMA status if transfer is done
	 }
	 TPM0->SC |= TPM_SC_TOF_MASK;																								//Reset TOF Flag
	 TPM0->CNT = 0;																															//Reset Counter
	 TPM0->MOD = guardtime_period-1;																						//50 us period to latch the WS2812 diode colour
	 TPM0->SC |= TPM_SC_PS(0)																									//Prescaler value - 1
	          | TPM_SC_TOIE_MASK																							//TPM0 Overflow Interrupts Enable
	          | TPM_SC_CMOD(1);																								//TPM0 Internal Clocking Enable
 }
 
/*----------------------------------------------------------------------------
			TPM0 overflow handler 
 *----------------------------------------------------------------------------*/
void TPM0_IRQHandler() {																
  	TPM0->SC = 0;	
		TPM0->SC |= TPM_SC_TOF_MASK;												//Reset TOF Flag
		dma_done_status = 1;
 }
 
/*----------------------------------------------------------------------------
			Be in sleep mode until DMA is done 
 *----------------------------------------------------------------------------*/
void dma_done(void) {																												
    while (!dma_done_status) __wfi();							
}

/*----------------------------------------------------------------------------
			Start DMA Transfer 
 *----------------------------------------------------------------------------*/
void start_DMA() {
 uint32_t n_bytes = DMA_LEAD_ZEROS + DMA_TRAIL_ZEROS + RGB_MATRIX_BITS ; 					//Number of transfered bytes				 
 dma_done();																																			//Wait for DMA transfer to end																																																																			
 dma_done_status = 0;           																									//Reset DMA done status

 TPM0->SC |= TPM_SC_DMA_MASK;        																							//DMA0 enable
 TPM0->SC |= TPM_SC_PS(0);   																									    //Prescaling - 1	
 TPM0->SC |= TPM_SC_CMOD(0);   																									  //disable clocks
 TPM0->SC |= TPM_SC_TOF_MASK;  																									  //reset TPM0 overflow flag

 TPM0->CNT = tpm_p0_period - 2 ;																								  //Set TPM0 counter value 
 TPM0->MOD = tpm_period - 1;       																					      //Set Modulo to 1250 ns
 TPM0->STATUS = 0xFFFFFFFF;																												//Clear All Channel Event Flags
 
 /* Reset DMA status */
 DMA0->DMA[DMA_CHANNEL_START].DSR_BCR = DMA_DSR_BCR_DONE_MASK; 													
 DMA0->DMA[DMA_CHANNEL_0_LOW].DSR_BCR = DMA_DSR_BCR_DONE_MASK; 														
 DMA0->DMA[DMA_CHANNEL_1_LOW].DSR_BCR = DMA_DSR_BCR_DONE_MASK;

 	/* All bits => High State
     source is one word of 0 then 24 x 0xffffffff, then another 0 word
		 first source go 1, tmp0 overflow is first */
    DMA0->DMA[DMA_CHANNEL_START].SAR     = (uint32_t)(void *)dma_data.start_t0_high;			//source of data to transfer, second all ones go
    DMA0->DMA[DMA_CHANNEL_START].DSR_BCR = DMA_DSR_BCR_BCR_MASK & n_bytes; 								//length of transfer in bytes
  /* Some bits (the 0 bits) => Low state.
     Triggered by TPM0_CH0
     Start 2 words before the actual data to avoid garbage pulses.
		 when overflow transfer set output to low*/
    DMA0->DMA[DMA_CHANNEL_0_LOW].SAR     = (uint32_t)(void *)dma_data.start_t1_low; 					//set source address
    DMA0->DMA[DMA_CHANNEL_0_LOW].DSR_BCR = DMA_DSR_BCR_BCR_MASK & n_bytes; 									//length of transfer in bytes
  /* All bits => Low state
     Triggered by TPM0_CH1
     source is constant 0x00000000 (first word of dmaWords)
		 when overflow transfer set output to low */
    DMA0->DMA[DMA_CHANNEL_1_LOW].SAR     = (uint32_t)(void *)dma_data.start_t1_low; 					//set source address
    DMA0->DMA[DMA_CHANNEL_1_LOW].DSR_BCR = DMA_DSR_BCR_BCR_MASK & n_bytes; 									//length of transfer in bytes
  
	/* Set destination adress register */
    DMA0->DMA[DMA_CHANNEL_0_LOW].DAR = DMA0->DMA[DMA_CHANNEL_1_LOW].DAR = 
		DMA0->DMA[DMA_CHANNEL_START].DAR = (uint32_t)(void *)&PTD->PDOR;
		
	/* Configure DMA Control Reg */
    DMA0->DMA[DMA_CHANNEL_0_LOW].DCR  = DMA_DCR_EINT_MASK 															  //Interrupt Enable                           
                                       | DMA_DCR_D_REQ_MASK 															//Clear ERQ on end of transfer/Request disable
																			 | DMA_DCR_CS_MASK																	//single read/write transfer per request
                                       | DMA_DCR_SINC_MASK 																//increment source each transfer
																			 | DMA_DCR_ERQ_MASK																	//Enable peripheral request to enable transfer                                       
                                       | DMA_DCR_SSIZE(1) 																//16-bit source transfers
                                       | DMA_DCR_DSIZE(1); 																//16-bit destination transfers

    DMA0->DMA[DMA_CHANNEL_1_LOW].DCR  = DMA_DCR_EINT_MASK 																//Interrupt Enable                                       
                                       | DMA_DCR_D_REQ_MASK 															//Clear ERQ on end of transfer/Request disable
																			 | DMA_DCR_CS_MASK																	//single read/write transfer per request
																			 | DMA_DCR_ERQ_MASK																	//Enable peripheral request to enable transfer                                       
                                       | DMA_DCR_SSIZE(1) 																//16-bit source transfers
                                       | DMA_DCR_DSIZE(1); 																//16-bit destination transfers

    DMA0->DMA[DMA_CHANNEL_START].DCR  = DMA_DCR_EINT_MASK 																//Interrupt Enable                                       
                                       | DMA_DCR_D_REQ_MASK 															//Clear ERQ on end of transfer/Request disable
																			 | DMA_DCR_CS_MASK																	//single read/write transfer per request
                                       | DMA_DCR_SINC_MASK 																//increment source each transfer
																			 | DMA_DCR_ERQ_MASK																	//Enable peripheral request to enable transfer                                       
                                       | DMA_DCR_SSIZE(1) 																//16-bit source transfers
                                       | DMA_DCR_DSIZE(1);																//16-bit destination transfers
																			 
    TPM0->SC |= TPM_SC_CMOD(1);         																									//Intern. Clocking Enabled
}

/*----------------------------------------------------------------------------
			Turn on specifed pixel on specified colour
 *----------------------------------------------------------------------------*/
void turn_pixel_on(uint16_t num_pixel, uint8_t green, uint8_t red, uint8_t blue) {
	/* DEC to BIN conversion tables */	
	uint8_t bin_tab[8] = { 0,0,0,0,0,0,0,0 };																								
	uint8_t bin_tab_correct[8] = { 0,0,0,0,0,0,0,0 };																			//good sequence	
	/* Loop Variables */
	uint8_t i = 0;																																					
	uint8_t j = 0;
	/* Colour Assignments */
	LED_tab[num_pixel] = green;																								
	LED_tab[num_pixel+1] = red;																								
	LED_tab[num_pixel+2] = blue;
	
	/* GREEN */
	while (green) {																																					//DEC to BIN -- green
		bin_tab[i++] = green % 2;
		green /= 2;
	}		
	for (i = 0; i < 8; i++) {																																//change order to LSB---MSB
		bin_tab_correct[i] = bin_tab[7 - i];		
		bin_tab_correct[i] *= 4;																															//EVERY element multipied by 255 (for DMA conversion purpose)
	}
	for (i = j; i < (8+j); i++) {																														//Add new elements to dma_words
		dma_data.dma_words[num_pixel*24+i] = bin_tab_correct[i];
	}
	
	for (i = 0; i < 8; i++) {																																//Clearing Tables
		bin_tab[i] = 0;
		bin_tab_correct[i] = 0;
	}	
	j = 8; i = 0;																																						//Clear 'i' and change value of 'j'
	
	/* RED */
	while (red) {																																						//DEC to BIN -- red
		bin_tab[i++] = red % 2;
		red /= 2;
	}
	
	for (i = 0; i < 8; i++) {																																//change order to LSB---MSB
		bin_tab_correct[i] = bin_tab[7 - i];		
		bin_tab_correct[i] *= 255;																														//EVERY element multipied by 255 (for DMA conversion purpose)
	}
	
	for (i = j; i < (8 + j); i++) {																													//Add new elements to dma_words
		dma_data.dma_words[num_pixel*24+i] = bin_tab_correct[i-8];
	}																													
	for (i = 0; i < 8; i++) {																																//Clearing tables
		bin_tab[i] = 0;
		bin_tab_correct[i] = 255;
	}	
	j = 16; i = 0; 																	  																			//Clear 'i' and change value of 'j'
	
  /* BLUE */	
	while (blue) {																																					//DEC to BIN -- blue
		bin_tab[i++] = blue % 2;
		blue /= 2;
	}
	for (i = 0; i < 8; i++) {																																//change order to LSB---MSB									
		bin_tab_correct[i] = bin_tab[7 - i];																									//EVERY element multipied by 255 (for DMA conversion purpose)
		bin_tab_correct[i] *= 255;
	}
	for (i = j; i < (8 + j); i++) {																													//Add new elements to dma_words																														
		dma_data.dma_words[num_pixel*24+i] = bin_tab_correct[i - 16];
	}
}

/*----------------------------------------------------------------------------
			Turning pixel off
 *----------------------------------------------------------------------------*/
void turn_pixel_off(uint16_t num_pixel) {
	/* DEC to BIN conversion tables */																							
	uint8_t bin_tab_correct[8] = { 0,0,0,0,0,0,0,0 };																			//good sequence	
	/* Loop Variables */
	uint8_t i = 0;																																					
	uint8_t j = 0;
	
	for (i = j; i < (8+j); i++) {																														//Add new elements to dma_words
		dma_data.dma_words[num_pixel*24+i] = bin_tab_correct[i];
	}
	
	j = 8; i = 0;																																						//Clear 'i' and change value of 'j'
	
	for (i = j; i < (8 + j); i++) {																													//Add new elements to dma_words
		dma_data.dma_words[num_pixel*24+i] = bin_tab_correct[i-8];
	}																													

	j = 16; i = 0; 																	  																			//Clear 'i' and change value of 'j'
	
	for (i = j; i < (8 + j); i++) {																													//Add new elements to dma_words																														
		dma_data.dma_words[num_pixel*24+i] = bin_tab_correct[i - 16];
	}
}

/*----------------------------------------------------------------------------
			Delay function
 *----------------------------------------------------------------------------*/
void delay_mc(uint32_t value){
	uint32_t delay, x;
	
	for(x=0; x < value; x++){
		for(delay=0; delay < 10000; delay++){};
	}
}

/*----------------------------------------------------------------------------
			Initialize all needed modules
 *----------------------------------------------------------------------------*/
void init_Device() {
	init_led_matrix();
	init_dma_tables();
	init_modules();
	init_dma();
	init_tpm();
}

/*----------------------------------------------------------------------------
			Set all LEDs off
 *----------------------------------------------------------------------------*/
void set_matrix_off(void) {
	uint8_t i;
	for (i=0; i<NUM_OF_LEDS; i++)
		turn_pixel_off(i);
}

/*----------------------------------------------------------------------------
			Set all LEDs off
 *----------------------------------------------------------------------------*/
void turn_symbol(uint8_t matrix[], uint8_t green, uint8_t red, uint8_t blue) {
	uint8_t i;
	for (i=0; i<NUM_OF_LEDS; i++) {
		if (matrix[i] == 1)
			turn_pixel_on(i, green, red, blue);
	}
}

