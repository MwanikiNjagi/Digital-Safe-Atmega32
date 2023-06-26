/*
 * LCD_Function.c
 * Author: Michael Mureithi
 */ 

/*Typical Header file inclusion and CPU frequency definition*/
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

/*Useful pin and port definitions 4bit */
#define LCD_Dir DDRD
#define LCD_Port PORTD

#define RS PD0
#define EN PD1

/*LCD command write function*/
void LCD_Cmd(unsigned char cmd){
	/*Sending the first nibble of data (Higher 4 bits)*/
	LCD_Port = (LCD_Port & 0x0F) | (cmd & 0xF0);/* Sending upper nibble */
	LCD_Port &= ~ (1<<RS);		/* RS=0, command reg. */
	LCD_Port |= (1<<EN);		/* Enable pulse ON */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);		/* Enable pulse OFF */
	_delay_us(200);
	/*Sending the second nibble of data (Lower 4 bits)*/
	LCD_Port = (LCD_Port & 0x0F) | (cmd << 4);/* Sending lower nibble */
	LCD_Port |= (1<<EN);		/* Enable pulse ON */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);		/* Enable pulse OFF */
	_delay_ms(2);
}

/*LCD data write function */
void LCD_Char (unsigned char char_data){
	/*Sending the first nibble of data (Higher 4 bits)*/
	LCD_Port = (LCD_Port & 0x0F) | (char_data & 0xF0);/* Sending upper nibble */
	LCD_Port |= (1<<RS);		/* RS=1, data reg. */
	LCD_Port |= (1<<EN);		/* Enable pulse ON */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);		/* Enable pulse OFF */
	_delay_us(200);
	/*Sending the second nibble of data (Lower 4 bits)*/
	LCD_Port = (LCD_Port & 0x0F) | (char_data << 4);  /* Sending lower nibble */
	LCD_Port |= (1<<EN);		/* Enable pulse ON */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);		/* Enable pulse OFF */
	_delay_ms(2);
}

/*LCD Initialize function */
void LCD_Init (void){
	LCD_Dir = 0xFF;	/* Make LCD command port direction as output pins*/
	_delay_ms(20);	/* LCD Power ON delay always > 15ms */
	
	LCD_Cmd(0x02);	/* Return display to its home position */
	LCD_Cmd(0x28);	/* 2 line 4bit mode */
	LCD_Cmd(0x0C);	/* Display ON Cursor OFF */
	LCD_Cmd(0x06);	/* Auto Increment cursor */
	LCD_Cmd(0x01);	/* Clear display */
}
/*Clear LCD Function*/
void LCD_Clear(void){
	LCD_Cmd(0x01);	/* clear display */
	LCD_Cmd(0x02);	/* Return display to its home position */
}
/*Send string to LCD function */
void LCD_String (char *str){	
	int i;
	/* Send each char of string till the NULL */
	for(i=0;str[i]!=0;i++){
		LCD_Char(str[i]);
	}
}
/*Send string to LCD with xy position */
void LCD_String_xy (char row, char pos, char *str){
	if (row == 0 && pos<16){
		LCD_Cmd((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	}
	else if (row == 1 && pos<16){
		LCD_Cmd((pos & 0x0F)|0xC0);	/* Command of second row and required position<16 */
	}	
	LCD_String(str);			/* Call LCD string function */
}

/*Function to view the contents of the CGRAM*/
void LCD_Scan(short start, short finish){
	LCD_Clear();
	LCD_Cmd(0x80);
	LCD_String("Scanning");
	char counter[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	char loci[] = {'0','0'};
	for(short i = start; i <=finish; i++){
		loci[1] = counter[i%16];
		loci[0] = counter[i/16];
		LCD_String_xy(0, 10, loci);
		LCD_Cmd(0xC0);
		LCD_Char(i);
		_delay_ms(500);
	}
	LCD_Cmd(0xC0);
	LCD_String("Scanning Done");
}

/*Function to view the contents of the CGRAM*/
void LCD_Hex(char hex){
	char buffer = hex;
	char counter[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	char loci[] = {'0','0'};
	loci[1] = counter[buffer&0x0F];
	loci[0] = counter[buffer>>4];
	LCD_Char(loci[0]);
	LCD_Char(loci[1]);
}