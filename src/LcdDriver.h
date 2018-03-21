/**
  ******************************************************************************
  * @file    LcdDriver.h
  * @author  A.Rodriguez
  * @version V00
  * @date    May-2015
  * @brief   Header for LcdDriver.c module
  *
  ==============================================================================
                    ##### Main changes #####
  ==============================================================================

*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	ITStatus initialized;	
	ITStatus powered;	
	ITStatus writed;
	ITStatus cleared;
}LCD_Typedef;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LcdConfig(void);	//starts and configs the LCD, returns SET if it's succesful
static FlagStatus CheckBusyFlag(void);	//checks if the busy flag is activated
static void WriteLCDCommand(uint8_t command);
void WriteLCDString(unsigned char* stringAddress, unsigned char size);
void WriteLCDString2(unsigned char* stringAddress, unsigned char size);
void WriteLCDLineCenter(unsigned char* stringAddress, unsigned char size, unsigned char line);
void WriteLCDStringTimed(unsigned char* stringAddress, unsigned char size, unsigned int time);
void ClearLCD(void);
void WriteLCDShiftChar(void);
