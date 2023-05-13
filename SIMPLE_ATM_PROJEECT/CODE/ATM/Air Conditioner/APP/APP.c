#include "APP.h"
#include <stdlib.h>
#include <string.h>
/***********************************************************************************************************************
                                    GLOBAL VARIABLES 
************************************************************************************************************************/
uint8_t enter_zeroFlag ,state ;
uint8_t Wrong_Counter = 1;
uint8_t Stage_flag = 1;
uint8_t PIN[5] ={};
uint8_t pin_index =0;
uint8_t check = 0 ;
uint8_t pin[5]= {};
uint8_t Balance[]={};
uint8_t pan [17] = {};
uint8_t spi_rec [21] = {};

float dailyMax = 5000.0;
uint8_t bell[8] = {0x04, 0x0E ,0x0E ,0x0E ,0x1f ,0x00 ,0x04 ,0x00};  //HEX Array for the Bell Shape//

/***********************************************************************************************************************
                                   FUNCTION IMPLMENTATION
************************************************************************************************************************/


void app_Init(void){
	SPI_SlaveInit(PRESCALER_16);	
	DIO_INITPIN(pind2,INPLUP);
	EXI_Enable(EXT_INT_0);
	EXI_Trigger(EXT_INT_0,ANY_LOGIC_CHANGE);
	EXI_SetCallBack(EXT_INT_0,set_sage);
	KEYPAD_init();
	LCD_INIT();
	buzzer_init();
	TIMER_0_init(NORMAL_MODE);
	DIO_INITPIN(BUTTON_PIN,INFREE);
	DIO_INITPIN(pinb3,OUTPUT);
	TIMER_2_INT();
	
}


	
void app_Start(void){
	
	

	if (Stage_flag == WELCOME_STAGE)
	{
		welcome();
	}
	else if (Stage_flag == INSERTION_STAGE)
	{
		insertion();
	}
	
	else if (Stage_flag == RECIEVE_STAGE)
	{
		RECIEVE_data();
	}
	else if (Stage_flag == PIN_ENTRY_STAGE)
	{
		PIN_Entery();
	}
	else if (Stage_flag == PIN_CHECK_STAGE){
		
		PIN_Check();
		
	}else if (Stage_flag == TRANSACTION_ENTRY_STAGE){
		
		transaction_Entry();

	}else if (Stage_flag == ACCOUNT_CHECK_STAGE){
	
		account_checks();

		}
	
		
}


/************************************************************************************************************/
/*DESCRIPTION :-
	- used for the call back function of interrupt 0
	- used to set stage flag to the PIN_Entery stage
*/
void set_sage(){
	Stage_flag = RECIEVE_STAGE ;
	
}

/************************************************************************************************************/

/*DESCRIPTION :-
	- used to display welcome message	
*/
void welcome(void){
	LCD_Write_String(" WELCOME TO ATM ");
	TIMER_0_DELAY_MS(1000);
	Stage_flag = INSERTION_STAGE ;
	
}	

/************************************************************************************************************/

/*DESCRIPTION :-
	- used to display a message to the user to insert the card 
*/
void insertion(void){
	
	LCD_GoTo(1,0);
	LCD_Write_String("INSERT YOUR CARD");
	
}

void RECIEVE_data(void){
	// start receiving data
	LCD_Clear();
	uint8_t index = 0;
	uint8_t index2 = 0;
	while (1)
	{
		if (SPI_RecievePeriodicChecking(&spi_rec[index]))
		{
			LCD_GoTo(1,0);
			LCD_Write_String("LOADING ...");
			if (spi_rec[index] == '\0')
			{
				spi_rec[index] = '\0';
				break;
				
			}
			index++;
			//TIMER_0_DELAY_MS(100);
		}
		
	}
	
	//TIMER_0_DELAY_MS(3000);

	for(uint8_t i =0 ; i<16 ; i++){
		pan[i]= spi_rec[i];
	}
	
	for(uint8_t i =0 ; i<5 ; i++){
		pin[i]= spi_rec[i+16];
	}
		LCD_Clear();
	
	/*
	while (1)
	{
		if (SPI_RecievePeriodicChecking(&pin[index2]))
		{
			if (pin[index2] == '\0')
			{
				pin[index2] = '\0';
				break;
				
			}
			index2++;
		//	TIMER_0_DELAY_MS(100);
		}
		
	}
	
		LCD_Clear();
		LCD_Write_String(pan);
		TIMER_0_DELAY_MS(3000);
		LCD_Clear();
		LCD_Write_String(pin);
		TIMER_0_DELAY_MS(3000);
		*/
		Stage_flag = PIN_ENTRY_STAGE;
}


/************************************************************************************************************/

/*DESCRIPTION :-
	- used to enter pin 
	- if user entered more than 4 numbers it will go to the next stage (PIN_Check) with status of incorrect pin
	- if user entered less than or equal to 4 numbers and then pressed enter it will go to the next stage to check if it is correct or not
*/
void PIN_Entery (void)
{
	

	
		uint8_t key ;
		DIO_READPIN(BUTTON_PIN,&state);
	
				LCD_Clear();
				LCD_GoTo(1,1);
				LCD_Write_String("Enter Your PIN");
				TIMER_0_DELAY_MS(200);
				pin_index = 0 ;
				while (1)
				{
					DIO_READPIN(BUTTON_PIN,&state);
					 key = KEYPAD_getKey();
			
			 
					 if(pin_index > 4) // check if the user entered 5 numbers not 4
					 {
							 Stage_flag = PIN_CHECK_STAGE ; 
							 check = 1 ;  // wrong entry 
							 pin_index = 0 ;
							 break;
						 
					 }else 
					 {
							 
							 if (enter_zeroFlag ==1) // check if the enter button pressed
							 {
								 check = 2 ;
								 Stage_flag = PIN_CHECK_STAGE ;
								 pin_index =0;
								 break ;
								 
							 }
							 
						 }
			 
					  if (enter_zeroFlag == 2 && pin_index <4)  // check if the zero button pressed
					  {
				 
						  while(state==1 && enter_zeroFlag == 2) 
						  {
							  DIO_READPIN(BUTTON_PIN,&state);
							  PIN[pin_index] = '0';
							  LCD_GoTo(2,4+pin_index);
						  }
						  
						  LCD_Write_String("*");
						  pin_index++;
				  
					  }
					  
					  if(key!=0) // if keypad pressed
					  {
							PIN[pin_index] = key;
							LCD_GoTo(2,4+pin_index);
							LCD_Write_String("*");
							pin_index++;
					  }
						   
			   
	}
				
}
	
/************************************************************************************************************/

/*DESCRIPTION :-

	- if status coming from pin entry is pin incorrect 
				- the function will display a message to make user try again and the number of trials
				- if trials equals to 3 all machine will be blocked 
				- if nit equal to 3 the function will go the pin entry again 
				
	- if the status is not pin incorrect 
				- it will check if the entered pin is the same of the card pin
				- if it not the same it will do the same steps of status of pin incorrect
				- if it the same it will go the next stage (transaction_Entry)
*/


void PIN_Check (void)
{
	
	while (check == 2)  // if the pin entered was 4 numbers
	{
		
	for (uint8_t i=0; i <4; i++)
		{
			if (PIN[i] !=  pin[i] /*EEPROM PIN)*/ ) // check if the pin was not matched
			{
				
				check = 1 ; 
			}
		}
		
		if (check != 1) // check if the pin was correct
		{
			check = 0 ;
		}
	}
	
	if (check == 1)  // if the pin was incorrect
	{
	
		if(Wrong_Counter <3){
			
				Stage_flag = PIN_ENTRY_STAGE ;
				pin_index = 0;
				LCD_Clear();
				LCD_GoTo(1,3);
				LCD_Write_String("TRY AGAIN ");
				LCD_GoTo(2,7);
				LCD_Write_String("TRIALS :");
				LCD_Write_Number(Wrong_Counter);
				Wrong_Counter++;
				TIMER_0_DELAY_MS(2000);
				LCD_Clear();
		}
		else // if the user entered the pin wrong 3 times
		 {
				 LCD_Create_Charecter(bell,0);
				buzzer_On();
				LCD_Clear();
				LCD_GoTo(1,3);
				LCD_Write_String(" BLOCKED ");
				LCD_Write_Charecter(0);
				EXI_Disable(EXT_INT_0);
				TIMER_0_DELAY_MS(200000);
				
				//Stage_flag = 0 ;
					
				
			
		}
	
		
		
	}
	
	if (check == 0) // if the pin was correct
	{
		Stage_flag = TRANSACTION_ENTRY_STAGE ;
		Wrong_Counter = 1;
		pin_index = 0;
		LCD_Clear();
	}
	
	
}

/************************************************************************************************************/

/**DESCRIPTION:-
	
*/
void transaction_Entry(void){
	uint8_t key = 0,flag=0,temp=0,i=0;

	LCD_Clear();
	LCD_GoTo(1,0);
	LCD_Write_String("Enter Amount");
	LCD_GoTo(2,0);
	LCD_Write_String("0000.00");
	
	while (i<=4) 
	
	
	{
		key=KEYPAD_getKey();
		DIO_READPIN(BUTTON_PIN,&state);
		
		
		while (enter_zeroFlag == 2 && state ==1) // check the zero button is pressed
		{
				DIO_READPIN(BUTTON_PIN,&state);
				Balance[i]='0'; // save zero to the balance array
					flag = 1 ;
		}
			
		
		
		
		if (key!=0 )  // check if keypad is pressed
		{
			
			Balance[i]=key;	// save entered value to the balance array
				
			flag = 1;
		}
			
			
		while(flag == 1  ) // when zero button or keypad is pressed
		{
		
		
			
			if (enter_zeroFlag == 1) // if enter button is pressed
				{
				Balance[i]='\0';   
				Stage_flag = ACCOUNT_CHECK_STAGE ;
				i=5 ;
				flag = 0;
				
			}else if(enter_zeroFlag == 2){
					
			}
			else // if enter or zero button not pressed
			{
				
				DIO_READPIN(BUTTON_PIN,&state);
				for (uint8_t c =0 ; c<=i ; c++) // display numbers and shift them to left
				{
							
					LCD_GoTo(2,3-(i-c));
					LCD_Write_Charecter(Balance[(c)]);  
						
				}
				flag =0 ;
				i++;
			}
			
		}
		
				
		
	}
	
	
	
}


/************************************************************************************************************/
/**DESCRIPTION:-
	
*/
void account_checks(void){
	
		
	LCD_Create_Charecter(bell,0);
	float transAmount = atof(Balance);
	uint8_t approved_flag = 0 ;
	
	LCD_Clear();
	LCD_Write_String(Balance);
	LCD_GoTo(2,0);
	
	
	LCD_Clear();
	
	if(isValidAccount(pan) == SERVER_OK){
		
		if(isBlockedAccount()==SERVER_OK){
		
			if (transAmount<= dailyMax)
			{
				if(isAmountAvailable(&transAmount) == SERVER_OK){
					
					approved_flag = 1 ;
					
				}else{
					
					LCD_GoTo(1,3);
					LCD_Write_String("INSUFFICIENT ");
					LCD_GoTo(2,5);
					LCD_Write_String(" FUND !!");
					LCD_GoTo(2,15);
					LCD_Write_Charecter(0);
					TIMER_0_DELAY_MS(DELAY_VALUE);
					Stage_flag = TRANSACTION_ENTRY_STAGE ;
				}// end of balance check
				
			}else{
				
					LCD_GoTo(1,4);
					LCD_Write_String("MAX LIMIT");
					LCD_GoTo(2,1);
					LCD_Write_String(" IS EXCEEDED ");
					LCD_GoTo(2,15);
					LCD_Write_Charecter(0);
					TIMER_0_DELAY_MS(DELAY_VALUE);
					Stage_flag = TRANSACTION_ENTRY_STAGE ;
			}// end of amount check
		
		}else{
			
			buzzer_On();
			LCD_GoTo(1,4);
			LCD_Write_String("THIS IS");
			LCD_GoTo(2,1);
			LCD_Write_String("A STOLEN CARD !!");	
			LCD_GoTo(2,15);
			LCD_Write_Charecter(0);		
			TIMER_0_DELAY_MS(500000);
		} //  end of blocked check
		
	}
	
	else{
		buzzer_On();
		LCD_GoTo(1,4);
		LCD_Write_String("THIS IS");
		LCD_GoTo(2,1);
		LCD_Write_String("A FRAUD CARD !!");
		LCD_GoTo(2,15);
		LCD_Write_Charecter(0);
		TIMER_0_DELAY_MS(DELAY_VALUE);
		
	}
	
	
	if ( approved_flag)
	{
		buzzer_Off();
		p_account->balance -= transAmount ;
		LCD_Clear();
		LCD_GoTo(1,4);
		LCD_Write_String("APPROVED ");
		LCD_GoTo(2,3);
		LCD_Write_String("TRANSACTION ");
		TIMER_0_DELAY_MS(DELAY_VALUE);
		
		LCD_Clear();
		LCD_GoTo(1,4);
		LCD_Write_String("REMAINING");
		LCD_GoTo(2,0);
		LCD_Write_String("BALANCE :");
		LCD_Write_Number(p_account->balance);
		LCD_Write_String(".00");
		TIMER_0_DELAY_MS(DELAY_VALUE);
		
		LCD_Clear();
		LCD_GoTo(1,4);
		LCD_Write_String("EJECTING ");
		LCD_GoTo(2,6);
		LCD_Write_String("CARD ");
		TIMER_0_DELAY_MS(DELAY_VALUE);
		LCD_Clear();
		Stage_flag = WELCOME_STAGE ;

	}
	

}

