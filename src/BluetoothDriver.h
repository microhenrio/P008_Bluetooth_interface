/**
  ******************************************************************************
  * @file    BluetoothDriver.h
  * @author  A.Rodriguez
  * @version V00
  * @date    Oct-2015
  * @brief   Header for BluetoothDriver.c module
  *
  ==============================================================================
                    ##### Main changes #####
  ==============================================================================

*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	FunctionalState 	initialized;	//configured
	FlagStatus 	command_pending;	//command pending for answer
	FlagStatus 	command_received;	//received meaningful command
	FlagStatus 	AOK;
	ErrorStatus ERR;
	uint8_t			status,status_prev;
}BLE_Typedef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SB_BOOT	0
#define SB_STANDBY	1
#define SB_BLEMODE	2

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void USART2_Config(void);
void USART2_SendString(unsigned char* stringAddress, unsigned char size);
void BluetoothManager(void);
/* Private functions ---------------------------------------------------------*/
