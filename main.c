#include "RGBW_LEDs_driver.h"
#include "i2c.h"

int main() {

	init_Device();
	init_I2C();
	init_Uart();
	
	for (;;) {
		scroll_syms();
	}
		
}

/*! \mainpage notifYou project documentation
 *
 * \section intro_sec Introduction
 *
 *Unlike producers expectations smartwatchs’ market is slowing down. Users can’t replace theirs smartphone’s functionality with them, 
 *what is more their design isn’t that attractive to compete with traditional watches. Those weaknesses cleverly avoid smartbands devices, 
 *band which displays on small screens or just leds basic smartphone’s notifications. They’re targeted for active people who wants connect 
 *technology with their lifestyle. It seems that this group is still growing. Moreover smartbands are much cheaper and easier to customize 
 *their design than smartwatches. 
 *
 * \section desc_sec Project description
 *
 *notifYou project is project based on several elements:
 *- NXP-KL46z board
 *- external Bluetooth module HC-05
 *- led matrix 8x5 ws2812
 *- android application notifYou connected with board by bluetooth
 * \subsection element1 NXP-KL46z board
 *NXP microcontroller is a center of our system, it manages onborad modules which we use like DMA, TPM, UART and GPIO which are resposible for controll
 *led matrix. DMA, TPM and UART events are handled by interrupts it makes system much more responsible and easy to use. 
 * \subsection element2 external Bluetooth module HC-05
 *Slave bluetooth module which converts its rx/tx data to serial UART transmission. Defaul it wokrs at 9600 baud rate, its pin to pair with other devices
 *is 1234. 
 * \subsection element3 led matrix 8x5 ws2812
 *Leds with their own 24 bit data colour registers are cofigured by 1-wire interface which i coded by impulses of set width. Setting pattern on the matrix works
 *like shift register, so if we want to set the last one led we have to program all 40 of them.
 * \subsection element4 android application notifYou connected with board by bluetooth
 *Android app is an element responsible for setting notifications to be show on the matrix. In our prototype needs, app isn't connected to the android core but
 *number of notifications is controlled by user 
 */