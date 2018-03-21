	/**
  ******************************************************************************
  * @file    BluetoothDriver.c
  * @author  A.Rodriguez
  * @version V00
  * @date    October-2015
  * @brief   Routines to initiate and control the bluetooth device RN4020.
  *
  ==============================================================================
                    ##### Main changes #####
  ==============================================================================

*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "BluetoothDriver.h"
#include "SerialConsole.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_gpio.h"
#include <string.h>
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define USART2_SPEED 115200
#define RXBUFFERSIZE2   0x40	//64 bytes 


/* Private variables -------------------------------------------------------------*/
unsigned char RxBuffer2[RXBUFFERSIZE2];
uint8_t NbrOfDataToTransfer2;
uint8_t NbrOfDataToRead2 = RXBUFFERSIZE2;
__IO uint8_t TxCount2 = 0; 
__IO uint8_t RxCount2 = 0; 
uint8_t commandReceived2=0;
unsigned char* TxAdress2;
BLE_Typedef	bluetooth;
unsigned char MB_VERSION[]="V";
unsigned char MB_ADVERTISING[]="A"; //Start advertising
unsigned char MB_DUMP[]="D"; //dump information

extern unsigned char consoleStatus;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function inits the UART1 peripheral.
  * @param  None
  * @retval None
  */
void USART2_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
    
/* USARTx configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - Two Stop Bit
  - Odd parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = USART2_SPEED;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  //USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 
	USART_Init(USART2,&USART_InitStructure);
  USART_Cmd(USART2,ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	bluetooth.status_prev=0xff;
	bluetooth.status=SB_BOOT;
}

/**
  * @brief  This function inits the UART1 peripheral.
  * @param  unsigned char* stringAddress	//address of the string array to send
  * @param  unsigned char size	//size of the string to send
  * @retval None
  */
void USART2_SendString(unsigned char* stringAddress, unsigned char size)
{
	
	//NbrOfDataToTransfer=(sizeof(TxBuffer) - 1);
	NbrOfDataToTransfer2=size;
	TxAdress2=stringAddress;
	TxCount2=0;
	RxCount2=0;
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}
/*void BLE_SendCommand(char command[],unsigned char size){
	command[0]='A';
}*/

void BluetoothManager(void){
	switch(bluetooth.status){
		case SB_BOOT: 
			if(bluetooth.status_prev!=SB_BOOT){
				GPIO_SetBits(GPIOA,BLE_WSW);	//wakeup software

				/*GPIO_SetBits(GPIOA,BLE_WHW);	//wakeup hardware
				GPIO_ResetBits(GPIOA,BLE_WHW);	//wakeup hardware
				GPIO_SetBits(GPIOA,BLE_WHW);	//wakeup hardware
				GPIO_ResetBits(GPIOA,BLE_WHW);	//wakeup hardware
				GPIO_SetBits(GPIOA,BLE_WHW);	//wakeup hardware
				GPIO_ResetBits(GPIOA,BLE_WHW);	//wakeup hardware*/
				GPIO_SetBits(GPIOA,BLE_WHW);	//wakeup hardware
				
				//GPIO_SetBits(GPIOA,BLE_CMD_MLDP);	//Command mode
				GPIO_ResetBits(GPIOA,BLE_CMD_MLDP);	//Command mode
				bluetooth.initialized=ENABLE;
				bluetooth.command_pending=RESET;
			}
			if(GPIO_ReadInputDataBit(GPIOB,BLE_WS) && bluetooth.command_received && bluetooth.AOK){	//is awake?
				bluetooth.status=SB_STANDBY;
			}
			bluetooth.status_prev=SB_BOOT;
			break;
		case SB_STANDBY: 
			if(bluetooth.status_prev!=SB_STANDBY){
				//USART2_SendString(MB_VERSION,sizeof(MB_VERSION));
				//USART2_SendString(MB_ADVERTISING,sizeof(MB_ADVERTISING));
				//USART2_SendString(MB_DUMP,sizeof(MB_DUMP));
			}
			if (consoleStatus==SC_BLE_MODE)
				bluetooth.status=SB_BLEMODE;
			bluetooth.status_prev=SB_STANDBY;
			break;
			case SB_BLEMODE: 
			if(bluetooth.status_prev!=SB_BLEMODE)
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				
				bluetooth.status_prev=SB_BLEMODE;
			break;
		default: 
			bluetooth.status=SB_BOOT;
			bluetooth.status_prev=0xff;
			break;
	}
}

