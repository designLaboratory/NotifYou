/**
*@file i2c.h
*@date 19 Feb 2017
*@brief Header file of NXP-KL46z i2c module driver
*I2C is an a serial communication interface between one master and many slaves
*It uses SCL for clock, and SDA for data, open drain lines to exchange informations
*/
#ifndef I2C_H
#define I2C_H
#include "MKL46Z4.h"
#include "uart.h"

/**
 *****************************************************************************************
 *  @brief      Initialize I2C
 *
 *  Call to clock modules, Mux the ports and enable I2C.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void init_I2C(void);

/**
 *****************************************************************************************
 *  @brief      Stop I2C module
 *
 ****************************************************************************************/
void stop_I2C(void);

/**
 *****************************************************************************************
 *  @brief      Start I2C module
 *
 ****************************************************************************************/
void start_I2C(void);

/**
 *****************************************************************************************
 *  @brief      Enable wait in I2C module
 *
 ****************************************************************************************/
void wait_I2C(void);

/**
 *****************************************************************************************
 *  @brief      Reset I2C module
 *
 ****************************************************************************************/
void rst_I2C(void);

//uint8_t I2CWrite(uint8_t data);
//uint8_t I2CRead(uint8_t ack);
/**
 *****************************************************************************************
 *  @brief      Write data to register
 *
 *  Call write
 * 
 *  @param      slave_adr - addres of the slave device(accelerometer)
 *  @param      register_adr - Register to be written with data
 *  @param      data - addres of the slave device
 *
 *  @return     No return value.
 ****************************************************************************************/
void i2c_RegWrite(uint8_t slave_adr, uint8_t register_adr, uint8_t data);

/**
 *****************************************************************************************
 *  @brief      Read data to register
 *
 *  Call write
 * 
 *  @param      slave_adr - addres of the slave device(accelerometer)
 *  @param      register_adr - addres of the slave device
 *
 *  @return     uint8_t data - data read from from register_adr
 ****************************************************************************************/
uint8_t i2c_ReadReg(uint8_t slave_adr, uint8_t register_adr);

/**
 *****************************************************************************************
 *  @brief      Nsec Delay
 *
 *  Perform nanosecond delay
 * 
 *  @param      val_ns - delay value
 *
 *  @return     No return value.
 ****************************************************************************************/
void wait_ns(uint16_t val_ns);
#endif
