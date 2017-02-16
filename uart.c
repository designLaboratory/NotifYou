#include "uart.h"

#define UART2_IRQ_NBR (IRQn_Type) 14
volatile uint8_t receiveBuff;

volatile uint8_t notifCnt[5];
volatile uint8_t notifPtr= 0;

void uartInit() {
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	PORTE->PCR[16] |= PORT_PCR_MUX(3); //UART0_TX
	PORTE->PCR[17] |= PORT_PCR_MUX(3); //UART0_RX
		
	UART2->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);
	
	UART2->BDH |= UART_BDH_SBR(0);			//baudrate 9600
	UART2->BDL |= UART_BDL_SBR(156);		//baudrate 9600
	UART2->BDH &= ~UART_BDH_SBNS_MASK;
	UART2->C1 &= ~(UART_C1_M_MASK | UART0_C1_PE_MASK);

	NVIC_ClearPendingIRQ(UART2_IRQ_NBR);
	NVIC_SetPriority(UART2_IRQ_NBR, 1);
	NVIC_EnableIRQ(UART2_IRQ_NBR);
	
	UART2->C2 |= UART_C2_RIE_MASK;
	UART2->C2 |= UART_C2_RE_MASK; //| UART_C2_TE_MASK;
}

void UART2_IRQHandler() {
	UART2->S1;
	notifCnt[notifPtr++]= UART2->D;
	if(notifPtr == 5)
		notifPtr= 0;

}