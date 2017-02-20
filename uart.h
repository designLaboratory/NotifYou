/**
*@file uart.h
*@author Szymon Sarna, Damian Mucha
*@date 19 Feb 2017
*@brief Header file of NXP-KL46z uart module driver
*UART is a serial transmission interface, in our solution it works as a receiver from bluetooth module HC-05
*after data beeing received UART interrupt occurs and data is saved to notification counter variable
*/

#ifndef UART_H
#define UART_H

#include <MKL46Z4.h>


volatile uint8_t Notif_tab[5]= {0, 0, 0, 0, 0};		/**< array with numper of notifications of each type */
volatile uint8_t notifPtr= 0;		/**< iterator for #Notif_tab */

/**
*@brief This fuction initialize UART2 NXP-Kl46z module
*UART2 will communicate with pheriperials on 9600 baud
*after data received interrupt will raise
**/
void init_Uart(void);

#endif