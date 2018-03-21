/**
  ******************************************************************************
  * @file    SerialConsole.h
  * @author  A.Rodriguez
  * @version V00
  * @date    May-2015
  * @brief   Header for SerialConsole.c module
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

/* Private define ------------------------------------------------------------*/
#define SC_BOOT 0
#define SC_PROMT 1
#define SC_COMMAND 2
#define SC_HELP		3
#define SC_VERSIONCMD 4
#define SC_SYNTAXERR 5
#define SC_REBOOT	6
#define SC_SWITCHMSG 7
#define SC_HALT 8
#define SC_BLE_MODE 9

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void USART1_Config(void);
void USART1_SendString(unsigned char* stringAddress, unsigned char size);
void ConsoleManager(void);	
/* Private functions ---------------------------------------------------------*/
