#include "I2C.h"

void Init_I2C() {
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; 					/**< Clocking PORT_E */

	PORTE->PCR[24] = PORT_PCR_MUX(5);							/**< PTE24 as I2C0_SCL */
	PORTE->PCR[25] = PORT_PCR_MUX(5);							/**< PTE24 as I2C0_SDA */
	
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;						/**< Clocking I2C0 */
	
	I2C0->F = 0x14;																/**< Set Baud Rate */
	
	I2C0->C1 |= I2C_C1_IICEN_MASK;								/**< Enable I2C0 */
}

void Stop_I2C() {
	I2C0->C1 &= ~I2C_C1_MST_MASK;								/**< Receive Select */	
  I2C0->C1 &= ~I2C_C1_TX_MASK;								/**< Select Slave Mode */
}

void Start_I2C() {
	I2C0->C1 |= I2C_C1_TX_MASK;										/**< Transmit Select */
  I2C0->C1 |= I2C_C1_MST_MASK;									/**< Select Master Mode */
}

void Wait_I2C() {
	while(!(I2C0->S & I2C_S_IICIF_MASK)); 				/**< Wait until ISR flag is set */
	I2C0->S |= I2C_S_IICIF_MASK;
}

void RST_I2C() {
	I2C0->C1 |= I2C_C1_RSTA_MASK;
}

void IIC_RegWrite(uint8_t slave_adr,uint8_t register_adr,uint8_t data) {
	Start_I2C();
	I2C0->D = slave_adr << 1UL;					/**< Send Slave Address */
	Wait_I2C();
	I2C0->D = register_adr;							/**< Send Slave's Register Address */
	Wait_I2C();
	I2C0->D = data;											/**< Send Data to this Register */
	Wait_I2C();
	Stop_I2C();

	Wait_ns(50);
}	

uint8_t IIC_ReadReg(uint8_t slave_adr,uint8_t register_adr) {
	uint8_t data;
	
	Start_I2C();
	I2C0->D = slave_adr << 1UL;					/**< Send Slave Address */
	Wait_I2C();
	I2C0->D = register_adr;							/**< Send Slave's Register Address */
	Wait_I2C();
	RST_I2C();
	I2C0->D = slave_adr << 1UL | 0x01;	/**< Send Slave Address after Reset*/
	Wait_I2C();
	
	I2C0->C1 &= ~I2C_C1_TX_MASK;				/**< Receive Select */	
	I2C0->C1 |= I2C_C1_TXAK_MASK;       /**< Transmit Acknowledge Enable */

	(void)I2C0->D;											/**< Get some data from Slave */
	Wait_I2C();
	data = I2C0->D;											/**< Get next portion of data from slave */
	Wait_I2C();
	Stop_I2C();
	
	Wait_ns(50);
	
	return data;												/**< Return acquired data */
}

void Wait_ns(uint16_t val_ns) {
	uint16_t i;
	for(i=0; i<val_ns; i++);
}
