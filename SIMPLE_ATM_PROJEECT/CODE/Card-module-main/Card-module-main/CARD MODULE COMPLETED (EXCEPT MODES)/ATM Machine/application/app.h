
#ifndef APP_H_
#define APP_H_
/*****************************************************************
					Includes
*****************************************************************/
#include "../MCAL/dio/DIO.h"
#include "../MCAL/Timers/timer.h"
#include "../MCAL/twi/twi.h"
#include "../HAL/eeprom/eeprom.h"
#include "../HAL/keypad/keypad.h"
#include "../HAL/lcd/LCD.h"
#include "../MCAL/spi/spi.h"
#include <util/delay.h>

/*************************************************************
						PROTOTYPES
**************************************************************/
void CARD_PROGRAMMING_MODE(void);
void CARD_USER_MODE(void);
void app_init();
void app_start() ;
#endif /* APP_H_ */