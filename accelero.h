/**
*@file accelero.h
*@date 19 Feb 2017
*@brief Header file of NXP-KL46z accelerometer module driver
*MMA8451Q is an a accelerometer module mounted on the board, connectced to PTE 24, 25 pins
*/

#ifndef ACCELERO_H
#define ACCELERO_H

#include "I2C.h"
#include "MKL46Z4.h"
#include "slcd.h"

/**
 *
 *  @brief Accelerometer Registers Declaration
 * 
 *	Constants which holds the addresses of all MMA8451Q registers (MMA8451Q.pdf - p.49)
 *
 */
#define STATUS_F_STATUS       0x00		// STATUS/F_STATUS Register 

#define OUT_X_MSB             0x01		// [7:0] 8 MSBs of X-axis sample
#define OUT_X_LSB             0x02		// [7:2] 6 LSB of X-axis sample
#define OUT_Y_MSB             0x03		// [7:0] 8 MSBs of Y-axis sample
#define OUT_Y_LSB             0x04		// [7:2] 6 LSB of Y-axis sample
#define OUT_Z_MSB             0x05		// [7:0] 8 MSBs of 14-bit Z-axis sample
#define OUT_Z_LSB             0x06		// [7:2] 6 LSB of 14-bit Z-axis sample

#define F_SETUP               0x09    	// FIFO Setup Register 
#define TRIG_CFG              0x0A    	// Map of FIFO data capture events 
#define SYSMOD                0x0B    	// System Mode Register 
#define INT_SOURCE            0x0C    	// Interrupt Status Register 
#define WHO_AM_I              0x0D    	// Device ID Register 
#define XYZ_DATA_CFG          0x0E    	// Sensor Data Configuration Register 
#define HP_FILTER_CUTOFF      0x0F    	// High Pass Filter Register 

#define PL_STATUS             0x10    	// Portrait/Landscape Status Register 
#define PL_CFG                0x11    	// Portrait/Landscape Configuration Register 
#define PL_COUNT              0x12    	// Portrait/Landscape Debounce Register 
#define PL_BF_ZCOMP           0x13    	// Back/Front and Z Compensation Register 
#define P_L_THS               0x14    	// Portrait to Landscape Threshold Register 

#define FF_MT_CFG             0x15    	// Freefall and Motion Configuration Register 
#define FF_MT_SRC             0x16    	// Freefall and Motion Source Register 
#define FT_MT_THS             0x17    	// Freefall and Motion Threshold Register 
#define FF_MT_COUNT           0x18    	// Freefall Motion Count Register 

#define TRANSIENT_CFG         0x1D    	// Transient Configuration Register 
#define TRANSIENT_SRC         0x1E    	// Transient Source Register 
#define TRANSIENT_THS         0x1F    	// Transient Threshold Register 
#define TRANSIENT_COUNT       0x20    	// Transient Debounce Counter Register 

#define PULSE_CFG             0x21    	// Pulse Configuration Register 
#define PULSE_SRC             0x22    	// Pulse Source Register 
#define PULSE_THSX            0x23    	// XYZ Pulse Threshold Registers 
#define PULSE_THSY            0x24
#define PULSE_THSZ            0x25
#define PULSE_TMLT            0x26    	// Pulse Time Window Register 
#define PULSE_LTCY            0x27    	// Pulse Latency Timer Register 
#define PULSE_WIND            0x28    	// Second Pulse Time Window Register 

#define ASLP_COUNT            0x29    	// Auto Sleep Inactivity Timer Register 

#define CTRL_REG1             0x2A    	// System Control 1 Register 
#define CTRL_REG2             0x2B    	// System Control 2 Register 
#define CTRL_REG3             0x2C    	// Interrupt Control Register 
#define CTRL_REG4             0x2D    	// Interrupt Enable Register 
#define CTRL_REG5             0x2E    	// Interrupt Configuration Register 

#define OFF_X                 0x2F    	// XYZ Offset Correction Registers 
#define OFF_Y                 0x30
#define OFF_Z                 0x31

#define MMA8451Q_I2C_ADR      0x1D


/**
 *****************************************************************************************
 *  @brief      Set ON Accelerometer
 *
 *  Call to set Accelerometer ACTIVE.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void Accelero_Set_Active(void);

/**
 *****************************************************************************************
 *  @brief      Initialize Single Tap Detection
 *
 *  Call to start a sequence of register writes that sets accelerometer in one-tap detection mode.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void Single_Tap_Init(void);

/**
 *****************************************************************************************
 *  @brief      Initialize Accelerometer
 *
 *  Call above functions and initialize accelerometer as Single_Tap Detector
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void Init_Accelero(void);

/**
 *****************************************************************************************
 *  @brief      Initialize Accelerometer ISR
 *
 *  Call to configure interrupts then accelerometer detects a single tap.
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void Init_Accelero_ISR(void);

/**
 *****************************************************************************************
 *  @brief      Configure PORT_C ISR
 *
 *  Call to configure PORT_C interrupt handler
 * 
 *  @param      No arguments
 *
 *  @return     No return value
 ****************************************************************************************/
void PORTC_PORTD_IRQHandler(void);

#endif
