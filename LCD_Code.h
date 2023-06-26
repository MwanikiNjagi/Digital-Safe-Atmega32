/*
 * LCD_Code.h
 *  Author: Michael Mureithi
 */ 


#ifndef LCD_CODE_H_
#define LCD_CODE_H_

/*Function definitions that can be accessed by the main.c file*/
void LCD_Cmd(unsigned char cmd);
void LCD_Char(unsigned char char_data);
void LCD_Init(void);
void LCD_Clear(void);
void LCD_String(char *str);
void LCD_String_xy(char row, char pos, char *str);
void LCD_Scan(short, short);
void LCD_Hex(unsigned char hex);

#endif /* LCD_CODE_H_ */