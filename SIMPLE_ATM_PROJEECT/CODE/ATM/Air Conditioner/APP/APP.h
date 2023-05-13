
#ifndef app_H_
#define app_H_



/*****************************************************************************************************************************************************************
*									Includes																				 *
******************************************************************************************************************************************************************/
#include "../MCAL/DIO/DIO.h"
#include "../MCAL/exi_interrupt/EXI_INTERRUPT.h"
#include "../HAL/lcd/LCD.h"
#include "../HAL/keypad/keypad.h"
#include "../MCAL/timers/timer.h"
#include "../HAL/buzzer/buzzer.h"
#include "Server.h"
#include "../MCAL/spi/spi.h"

/*****************************************************************************************************************************************************************
*									defines																			 *
******************************************************************************************************************************************************************/

#define  DELAY_VALUE 1000
#define  BUTTON_PIN  pinb2

typedef enum {
	
	WELCOME_STAGE = 1 ,
	INSERTION_STAGE ,
	RECIEVE_STAGE,
	PIN_ENTRY_STAGE ,
	PIN_CHECK_STAGE,
	TRANSACTION_ENTRY_STAGE ,
	ACCOUNT_CHECK_STAGE ,	
	
	}STAGES;
/*****************************************************************************************************************************************************************
*									Functions prototypes																			 *
******************************************************************************************************************************************************************/
extern uint8_t enter_zeroFlag ,state ;

void set_sage(void);
void welcome(void);
void insertion(void);
void PIN_Entery (void);
void PIN_Check (void);
void transaction_Entry(void);
void account_checks(void);
void app_Start(void);
void app_Init(void);
void RECIEVE_data(void);

#endif








