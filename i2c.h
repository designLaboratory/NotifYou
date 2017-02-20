#ifndef I2C_H
#define I2C_H
#include "MKL46Z4.h"

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
void Init_I2C(void);

/**
 *****************************************************************************************
 *  @brief      Stop I2C module
 *
 ****************************************************************************************/
void Stop_I2C(void);

/**
 *****************************************************************************************
 *  @brief      Start I2C module
 *
 ****************************************************************************************/
void Start_I2C(void);

/**
 *****************************************************************************************
 *  @brief      Enable wait in I2C module
 *
 ****************************************************************************************/
void Wait_I2C(void);

/**
 *****************************************************************************************
 *  @brief      Reset I2C module
 *
 ****************************************************************************************/
void RST_I2C(void);

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
void IIC_RegWrite(uint8_t slave_adr, uint8_t register_adr, uint8_t data);

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
uint8_t IIC_ReadReg(uint8_t slave_adr, uint8_t register_adr);

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
void Wait_ns(uint16_t val_ns);
#endif
