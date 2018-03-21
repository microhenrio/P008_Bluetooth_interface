/**
  ******************************************************************************
  * @file    LcdDriver.c
  * @author  A.Rodriguez
  * @version V00
  * @date    September-2015
  * @brief   Routines to initiate and control the LCD with the WS0010 OLED controller.
  *
  ==============================================================================
                    ##### Main changes #####
  ==============================================================================

*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "LcdDriver.h"
#include "main.h"
#include "stm32f0xx_gpio.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define LIM_CHECK_BUSY 100
/* Private variables -------------------------------------------------------------*/
extern GPIO_InitTypeDef        GPIO_InitStructure;
LCD_Typedef lcd;
//uint8_t auxResult;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function inits the LCD.
  * @param  None
  * @retval None
  */
void LcdConfig(void){
	WriteLCDCommand(0x3B);	//8bits,2 line,5x8 char, font table european II
	WriteLCDCommand(0x0C);	//on,cursor off, no blink
	WriteLCDCommand(0x01);	//clear
	WriteLCDCommand(0x06);	//increment, no shift
	WriteLCDCommand(0x02);	//home
	lcd.initialized=SET;	//I've made the inicialization
	lcd.powered=SET;	//the LCD is powered
	lcd.cleared=SET;
	lcd.cleared=SET;
	lcd.writed=RESET;
}
/**
  * @brief  This function checks the LCD busy flag
  * @param  None
  * @retval FlagStatus SET=busy, RESET=ready
  */
FlagStatus CheckBusyFlag(void){
	uint8_t cntCheckBusy=0,resultCheckBusy=1;
	
	GPIOB->MODER &= 0xffff0000;	//set D7 to D0 to input
	GPIOC->BSRR = LCD_RW;		//select read the data bus
	GPIOC->BRR = LCD_RS;	//command
	do{
	GPIOC->BSRR = LCD_E;
	GPIOC->BRR = LCD_E;	//enable pulse
	resultCheckBusy=GPIO_ReadInputDataBit(GPIOB,LCD_B7); 
	cntCheckBusy++;
	}while(resultCheckBusy==1 && cntCheckBusy<LIM_CHECK_BUSY);
	return RESET;
}

/**
  * @brief  writes a command to the LCD
  * @param  uint8_t command
  * @retval None
  */
void WriteLCDCommand(uint8_t command){
	CheckBusyFlag();
	GPIOC->BRR = LCD_RW;		//select write the data bus
	GPIOC->BRR = LCD_RS;	//command
	GPIOB->ODR	&=0xFF00;	//erases the D0 to D7
	GPIOB->MODER |= 0x00005555;	//set D7 to D0 to output
	GPIOB->ODR	|=command;	//writes the command to the port
	GPIOC->BSRR = LCD_E;
	GPIOC->BRR = LCD_E;	//enable pulse
}

/**
  * @brief  writes a string to the LCD using the two lines
  * @param  unsigned char* stringAddress
  * @param  unsigned char size	
	* @retval None
  */
void WriteLCDString(unsigned char* stringAddress, unsigned char size){
	uint8_t cntString=0;
	WriteLCDCommand(0x80);	//writes DDRAM address to 0
	for(cntString=0;cntString<size-1;cntString++){
		if(cntString==20){	//2on line
			WriteLCDCommand(0xC0);	//writes DDRAM to 40H
		}
		CheckBusyFlag();	
		GPIOB->ODR	&=0xFF00;	//erases the D0 to D7
		GPIOC->BRR = LCD_RW;		//select write the data bus
		GPIOB->MODER |= 0x00005555;	//set D7 to D0 to output
		GPIOC->BSRR = LCD_RS;	//data
		GPIOB->ODR	|=(*(stringAddress+(cntString)));	//writes chars
		GPIOC->BSRR = LCD_E;
		GPIOC->BRR = LCD_E;	//enable pulse
	}
	lcd.writed=SET;
	lcd.cleared=RESET;
}

/**
  * @brief  writes a string to the LCD using the second line only
  * @param  unsigned char* stringAddress
  * @param  unsigned char size	
	* @retval None
  */
void WriteLCDString2(unsigned char* stringAddress, unsigned char size){
	uint8_t cntString=0;
	WriteLCDCommand(0xC0);	//writes DDRAM to 40H
	for(cntString=0;cntString<size-1;cntString++){
		CheckBusyFlag();	
		GPIOB->ODR	&=0xFF00;	//erases the D0 to D7
		GPIOC->BRR = LCD_RW;		//select write the data bus
		GPIOB->MODER |= 0x00005555;	//set D7 to D0 to output
		GPIOC->BSRR = LCD_RS;	//data
		GPIOB->ODR	|=(*(stringAddress+(cntString)));	//writes chars
		GPIOC->BSRR = LCD_E;
		GPIOC->BRR = LCD_E;	//enable pulse
	}
	lcd.writed=SET;
	lcd.cleared=RESET;
}
/**
  * @brief  writes a string to the LCD using the second line only, centered
  * @param  unsigned char* stringAddress
  * @param  unsigned char size	
	* @retval None
  */
void WriteLCDLineCenter(unsigned char* stringAddress, unsigned char size, unsigned char line){
	uint8_t cntString=0;
	if(line==0)
		WriteLCDCommand(0x80);	//writes DDRAM address to 0
	else
		WriteLCDCommand(0xC0);	//writes DDRAM to 40H
	for(cntString=0;cntString<(20-size)/2;cntString++){
		CheckBusyFlag();	
		GPIOB->ODR	&=0xFF00;	//erases the D0 to D7
		GPIOC->BRR = LCD_RW;		//select write the data bus
		GPIOB->MODER |= 0x00005555;	//set D7 to D0 to output
		GPIOC->BSRR = LCD_RS;	//data
		GPIOB->ODR	|=' ';
		GPIOC->BSRR = LCD_E;
		GPIOC->BRR = LCD_E;	//enable pulse
	}
	for(cntString=0;cntString<size;cntString++){
		CheckBusyFlag();	
		GPIOB->ODR	&=0xFF00;	//erases the D0 to D7
		GPIOC->BRR = LCD_RW;		//select write the data bus
		GPIOB->MODER |= 0x00005555;	//set D7 to D0 to output
		GPIOC->BSRR = LCD_RS;	//data
		GPIOB->ODR	|=(*(stringAddress+(cntString)));	//writes chars
		GPIOC->BSRR = LCD_E;
		GPIOC->BRR = LCD_E;	//enable pulse
	}
	
	for(cntString=0;cntString<((20-size)-((20-size)/2));cntString++){
		CheckBusyFlag();	
		GPIOB->ODR	&=0xFF00;	//erases the D0 to D7
		GPIOC->BRR = LCD_RW;		//select write the data bus
		GPIOB->MODER |= 0x00005555;	//set D7 to D0 to output
		GPIOC->BSRR = LCD_RS;	//data
		GPIOB->ODR	|=' ';	//writes chars
		GPIOC->BSRR = LCD_E;
		GPIOC->BRR = LCD_E;	//enable pulse
	}
	lcd.writed=SET;
	lcd.cleared=RESET;
}

/**
  * @brief  writes a string to the LCD using the two lines, after the specified time the lcd is cleared
  * @param  unsigned char* stringAddress
  * @param  unsigned char size	
  * @param  unsigned char time
	* @retval None
  */
void WriteLCDStringTimed(unsigned char* stringAddress, unsigned char size, unsigned int time){
	WriteLCDString(stringAddress,size);
	startWindow(2,time);	//start window 2, will clear the lcd when timeout
}
/**
  * @brief  Clears the and send the cursor to the line 0
  * @param  None
	* @retval None
  */
void ClearLCD(void){
	WriteLCDCommand(0x01);	//clear
	WriteLCDCommand(0x02);	//home
	lcd.writed=RESET;
	lcd.cleared=SET;
}

void WriteLCDShiftChar(void){
	//WriteLCDCommand(0x07);	//increment and shift
	WriteLCDCommand(0x18);	//increment and shift
/*	GPIOB->ODR	&=0xFF00;	//erases the D0 to D7
	GPIOC->BRR = LCD_RW;		//select write the data bus
	GPIOB->MODER |= 0x00005555;	//set D7 to D0 to output
	GPIOC->BSRR = LCD_RS;	//data
	GPIOB->ODR	|=65;
		GPIOC->BSRR = LCD_E;
		GPIOC->BRR = LCD_E;	//enable pulse*/
}

