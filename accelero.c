#include "accelero.h"
//uint8_t i;
volatile uint8_t Notif_tab[5];

void Accelero_Set_Active() {
	uint8_t CTRL_REG1_DATA;
	CTRL_REG1_DATA = IIC_ReadReg(MMA8451Q_I2C_ADR, CTRL_REG1); 				/**< Read CTRL1 register from MMA8451Q */
	CTRL_REG1_DATA |= 0x01; 																					/**< Set first bit (Set Accelero Active) */
	IIC_RegWrite(MMA8451Q_I2C_ADR, CTRL_REG1, CTRL_REG1_DATA); 				/**< Write back modified register to the same address */
}

void Single_Tap_Init() {
	IIC_RegWrite(MMA8451Q_I2C_ADR, CTRL_REG1, 0x08); 						/**< Standby mode */
	IIC_RegWrite(MMA8451Q_I2C_ADR, PULSE_CFG, 0x15); 						/**< Enable X and Y and Z Single Pulse */
	
	IIC_RegWrite(MMA8451Q_I2C_ADR, PULSE_THSX, 0x19); 					/**< X Threshold to 1.575g */
	IIC_RegWrite(MMA8451Q_I2C_ADR, PULSE_THSY, 0x19); 					/**< Y Threshold to 1.575g */
	IIC_RegWrite(MMA8451Q_I2C_ADR, PULSE_THSZ, 0x2A); 					/**< Z Threshold to 2.65g */
	
	IIC_RegWrite(MMA8451Q_I2C_ADR, PULSE_LTCY, 0xF0); 					/**< Latency Time to 300 ms */

	IIC_RegWrite(MMA8451Q_I2C_ADR, CTRL_REG4, 0x08); 						/**< Enable Pulse Interrupt Block */
	IIC_RegWrite(MMA8451Q_I2C_ADR, CTRL_REG5, 0x08); 						/**< Route Pulse Interrupt Block to INT1 */
}

void Init_Accelero_ISR() {
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; 							/**< Clocking Port C */
	PORTC->PCR[5] |= PORT_PCR_MUX(1); 								/**< PTC5 as GPIO */
	PORTC->PCR[5] |= PORT_PCR_PE_MASK;								/**< Pull-Down Resistor */
	PORTC->PCR[5] |= PORT_PCR_IRQC(9);							  /**< ISR on rising Edge */
	
	NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);						/**< Clear and enable IRQs from PORT_C */
	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
}

void Init_Accelero() {
	Single_Tap_Init();
	Accelero_Set_Active();
}

void PORTC_PORTD_IRQHandler() {
	NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);
	/**< Determine the source of the interrupt */
	if ( (FPTC->PDIR & (1 << 5)) ) {						
		/**
		* ISR HERE
		*/
		Notif_tab[0] = 0;
		Notif_tab[1] = 0;
		Notif_tab[2] = 0;
		Notif_tab[3] = 0;
		Notif_tab[4] = 0;
		//slcdDisplay(i++, 10);					//Jakis tescik
		PORTC->PCR[5] |= PORT_PCR_ISF_MASK;	/**< Clear ISR flag */
		IIC_ReadReg(MMA8451Q_I2C_ADR, PULSE_SRC);		/**< Read the Pulse Status Register */
		IIC_ReadReg(MMA8451Q_I2C_ADR, INT_SOURCE);	/**< Read the Pulse Status Register */
	}
}
