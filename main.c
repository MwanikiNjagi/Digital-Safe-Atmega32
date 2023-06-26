/*
 * Keypad_Code.c
 * Author : Mwanikii

 */ 

/* Typical initialization statements for the Atmega32 with an 8Mhz internal clock*/
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LCD_Code.h" // Since we will use the LCD, we need to include the header file.
#include "C:\Users\Mwaniki\Desktop\Safe\Keypad_Code\Keypad_Code\typedef.h"
#include "C:\Users\Mwaniki\Desktop\Safe\Keypad_Code\Keypad_Code\internaleprom.h"

/* Definition of ports for the keypad */
#define KEY_PORT PORTC	// Register to set output values
#define KEY_DDR DDRC	// Register to set whether input/output
#define KEY_PIN PINC	// Register to read input values
#define NOTPRESSED 0xFF


uint16 set_pass = 0x76; //in this address will write '0' (pass set) or 0xff (pass not set)
uint16 address[4] = {0x7A, 0x7B, 0x7C, 0x7D}; //addresses where password will be stored
uint8 password[4];



/* Function to scan the keypad and the pressed key*/
char check_Keypad();
void safe_set_pass(uint8* pass, uint16* address, uint16 pass_status);
uint8 safe_pass_status(uint16 address);
void LCD_move_cursor(uint8 row, uint8 column);
uint32 safe_read_pass(uint16* address);
uint8 safe_check_pass(uint16* address);

/* Keypad array holding the keys in a grid arrangement*/
unsigned char keypad[4][4] = {	{'7','8','9','/'},
								{'4','5','6','*'},
								{'1','2','3','-'},
								{' ','0','=','+'}};
/* Variables to store the column, row values including input from KEY_PIN*/
unsigned char colNum, rowNum, input;

/* The main function*/
int main(void)
{
	uint8 flag = 0x00;
	uint8 pass_status = 0;
	
	//Making PORTA and PORTB outputs
	DDRA = 0xFF;
	DDRB = 0xFF;
	
	LCD_Init();
	pass_status = safe_pass_status(set_pass);
	
	/*check if it is first time to set password or not*/
	if (0xff == pass_status)
	{
		safe_set_pass(password, address, set_pass);
	}
	/*
	while(1)
	{
		LCD_Char(check_Keypad());	// Display the key pressed.
	}*/
	while( (0x00 == flag))
	{
			/*ask user to enter password, and check if it is correct*/
		flag = safe_check_pass(address);
		if (0xff == flag)
		{
			LCD_Clear();
			LCD_String_xy(0,0,"Correct password");
			LCD_move_cursor(2,1);
			PORTB = PORTB | (1<<PB0);
			_delay_ms(100);
			PORTB = PORTB & (~(1<<PB0));
			_delay_ms(100);
			PORTB = PORTB | (1<<PB0);
			_delay_ms(100);
			PORTB = PORTB & (~(1<<PB0));
			_delay_ms(100);
			PORTB = PORTB | (1<<PB0);
			_delay_ms(100);
			PORTB = PORTB & (~(1<<PB0));
			_delay_ms(100);
			PORTB = PORTB | (1<<PB0);
			_delay_ms(100);
			PORTB = PORTB & (~(1<<PB0));
			_delay_ms(100);
			PORTB = PORTB | (1<<PB0);
			_delay_ms(100);
			PORTB = PORTB & (~(1<<PB0));
			_delay_ms(100);				
			LCD_String("Safe is opened");
			break;
		}
		else
		{
			LCD_Clear();
			LCD_String_xy(0,0,"Incorrect pass");
			LCD_move_cursor(2,1);
			PORTB = PORTB | (1<<PB6);
			PORTA = PORTA | (1<<PA5);
			LCD_String_xy(0,0,"Safe is closed");
						
		}
		_delay_ms(2000);
	}
	return 0;
}

/* Function that checks the key that has been pressed on the keypad*/
char check_Keypad()
{
	while(1)
	{
		KEY_DDR = 0xF0; // 0b1111 0000     // Set the rows as outputs and the columns as inputs 
		KEY_PORT = 0x0F;// 0b0000 1111	   // Set the columns HIGH and the rows LOW. 

		/*	With the columns HIGH and the rows LOW, thus the KEY_PIN == 0x0F(0b0000 1111).
			If a button is pressed, the value of the column pin will be grounded and 
			KEY_PIN will change from 0x0F to some other number 0x0E i.e. 
			0x0E(0b0000 1110) i.e. a button in the 4th column was pressed. 
		*/
		do
		{
			do
			{
				_delay_ms(20);				// 20ms key debounce time (can be adjusted)
				colNum = (KEY_PIN & 0x0F);	// read status of column 
			}while(colNum == 0x0F);		// check for any key press 
				
			_delay_ms (40);		// 20 ms key debounce time (can be adjusted)
			colNum = (KEY_PIN & 0x0F);
		}while(colNum == 0x0F);
		
		/*	Once we have established which column the button is, we need to know the row
			To do this we set the rows HIGH and ground them 1 pin at a time while reading the result
			on the column pins. 
			If the we set the row pins to 0xEF(0b1110 1111), meaning the 4th row is grounded, and we
			check the column pins, If a button in the 4th row was pressed, we will find 
			PIN = 0b1110 1110. The last bit will be ground. This will let us know that the 4th row 
			has been activated. If no button in the 4th row as pressed, the result would be
			PIN = 0b1110 1111. Thus if we find this result, we ground the next row pin and check the
			column pins again. 
			*/
			KEY_PORT = 0xEF;	// 0xEF(0b1110 1111) Check for pressed key in 4th row 
			asm("NOP");			// This is an assembler delay function that does nothing for 1 micro-controller cycle.
			input = (KEY_PIN & 0x0F);
			if(input != 0x0F)
			{
				rowNum = 3;
				break;
			}

			KEY_PORT = 0xDF; // 0xDF(0b1101 1111)	// Check for pressed key in 3rd row
			asm("NOP");
			input = (KEY_PIN & 0x0F);
			if(input != 0x0F)
			{
				rowNum = 2;
				break;
			}
			
			KEY_PORT = 0xBF; // 0xDF(0b1011 1111) // Check for pressed key in 2nd row
			asm("NOP");
			input = (KEY_PIN & 0x0F);
			if(input != 0x0F)
			{
				rowNum = 1;
				break;
			}

			KEY_PORT = 0x7F; // 0xDF(0b0111 1111)// Check for pressed key in 1st row
			asm("NOP");
			input = (KEY_PIN & 0x0F);
			if(input != 0x0F)
			{
				rowNum = 0;
				break;
			}
		}

	/*	After obtaining the column and row values, the final keypad key can be output by cross referencing
		the rows and columns in our keypad character array
	*/	
		_delay_ms(150); // Slight delay to prevent multiple key presses. (can be adjusted) 
		if(colNum == 0x0E)
		return(keypad[rowNum][3]);
		else if(colNum == 0x0D)
		return(keypad[rowNum][2]);
		else if(colNum == 0x0B)
		return(keypad[rowNum][1]);
		else 
		return(keypad[rowNum][0]);
	}


		//LCD_Cmd(0xC0);
		//LCD_Hex(colNum);
		//LCD_Char(':');

void safe_set_pass(uint8* pass, uint16* address, uint16 pass_status)
{
	uint8 i = 0, digit = 0;
	LCD_Clear();
	LCD_String_xy(0,0,"Set 8-b password");
	LCD_move_cursor(2,1);
	for(i=0; i<4; ++i)
	{
		do
		{
			digit = check_Keypad();
			_delay_ms(20);
			LCD_Char(digit);
			_delay_ms(200);
			LCD_move_cursor(2, 1+i);
			LCD_Char('*');
			EEPROM_write(address[i], digit-48);
		} while ( digit == NOTPRESSED );
		
		
	}
	
	EEPROM_write(pass_status, 0x00); //write (0x00) to indicate that password is set
	LCD_move_cursor(3, 1);
	LCD_String("Password saved");
	_delay_ms(2000);
}


uint8 safe_pass_status(uint16 address)
{
	uint8 password_status = 0;
	password_status = EEPROM_read(address);
	return password_status;
}

void LCD_move_cursor(uint8 row, uint8 column)
{
	//uint8 row = 0;
	uint8 c = 0;
	
	if( (row>0) && (row<5) && (column>0) && (column<21) )
	{
		if (1 == row)
		{
			c = (0x80 + column - 1);
		}
		else if (2 == row)
		{
			c = (0xC0 + column - 1);
		}
		else if (3 == row)
		{
			c = (0x90 + column - 1);
		}
		else
		{
			c = (0xD0 + column - 1);
		}
	}
	else
	{
		c = 0x80;
	}
	LCD_Cmd(c);
}

uint32 safe_read_pass(uint16* address)
{
	uint32 pass_value = 0;
	/*read the password from EEPROM*/
	pass_value = (uint8)EEPROM_read(*(address+0));
	pass_value <<= 8;
	pass_value |= (uint8)EEPROM_read(*(address+1));
	pass_value <<= 8;
	pass_value |= (uint8)EEPROM_read(*(address+2));
	pass_value <<= 8;
	pass_value |= (uint8)EEPROM_read(*(address+3));
	/*now return password*/
	return pass_value;
}


uint8 safe_check_pass(uint16* address)
{
	uint8 flag = 0xff, digit = NOTPRESSED;
	uint8 password1[4] = {0}, password2[4] = {0};
	uint8 i=0;

	LCD_Clear();
	LCD_String("Enter password:");
	LCD_move_cursor(2,1);
	/* read password from user and EEPROM */
	for(i=0; i<4; ++i)
	{
		do
		{
			digit = check_Keypad();
		} while ( digit == NOTPRESSED );
		
		LCD_Char(digit);
		_delay_ms(100);
		LCD_move_cursor(2, 1+i);
		LCD_Char('*');
		password1[i] = EEPROM_read(address[i]);
		password2[i] = (digit - 48);
	}
	/*check if the password is correct or not:*/
	for (i=0; i<4; ++i)
	{
		if (password1[i] != password2[i])
		{
			flag = 0x00;
		}
		else
		{
			flag = 0xff;
		}
	}
	
	return flag;
}