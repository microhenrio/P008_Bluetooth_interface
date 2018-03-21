	/**
  ******************************************************************************
  * @file    SerialConsole.c
  * @author  A.Rodriguez
  * @version V00
  * @date    July-2015
  * @brief   Routines to initiate and control the serial console.
  *
  ==============================================================================
                    ##### Main changes #####
  ==============================================================================

*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "SerialConsole.h"
#include <string.h>
#include "main.h"
#include "BluetoothDriver.h"
#include "stm32f0xx_usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART1_SPEED 115200
#define RXBUFFERSIZE   0x40	//64 bytes 
#define SENDSTRING1(X)	USART1_SendString(X,sizeof(X));
/* Private variables -------------------------------------------------------------*/
extern SWITCH_Typedef Switch1,Switch2,Switch3,Switch4;
unsigned char RxBuffer[RXBUFFERSIZE];
uint8_t NbrOfDataToTransfer;
uint8_t NbrOfDataToRead = RXBUFFERSIZE;
__IO uint8_t TxCount = 0; 
__IO uint8_t RxCount = 0; 
uint8_t commandReceived=0;
unsigned char consoleStatus=0x00,consoleStatusPrevious=0xff;
unsigned char* TxAdress;
unsigned char welcomeMsg[]="OCTAR ELECTRONICS BLE-OLED Display. Enter 'help' for a command list.";
unsigned char promtMSG[]="\r\n>";
unsigned char promtBMSG[]="\r\n#";
unsigned char helpMSG[]="**** List of commands ****\n\rhalt: stops the system.\n\rhelp: shows this list.\n\rreboot: resets the device.\n\rversion: prints current version.\n\rblemode: enters in BLE mode, type 'exit' to return to normal mode.";
unsigned char versionMSG[]="V0.5";
unsigned char systaxErrMSG[]="Syntax error!!";
unsigned char rebootMSG[]="rebooting....";
unsigned char sw1ONMSG[]="SW1 ON";
unsigned char sw2ONMSG[]="SW2 ON";
unsigned char sw3ONMSG[]="SW3 ON";
unsigned char sw4ONMSG[]="SW4 ON";
unsigned char sw1OFFMSG[]="SW1 OFF";
unsigned char sw2OFFMSG[]="SW2 OFF";
unsigned char sw3OFFMSG[]="SW3 OFF";
unsigned char sw4OFFMSG[]="SW4 OFF";
unsigned char HALTMSG[]="Stopping..";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function inits the UART1 peripheral.
  * @param  None
  * @retval None
  */
void USART1_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
    
/* USARTx configured as follow:
  - BaudRate = 9600 baud  
  - Word Length = 8 Bits
  - Two Stop Bit
  - Odd parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = USART1_SPEED;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  //STM_EVAL_COMInit(COM1, &USART_InitStructure);
	USART_Init(USART1,&USART_InitStructure);
  USART_Cmd(USART1,ENABLE);

	/* Enable the EVAL_COM1 Transmoit interrupt: this interrupt is generated when the 
  EVAL_COM1 transmit data register is empty */  
  
	/* Wait until EVAL_COM1 send the TxBuffer */
//  while(TxCount < NbrOfDataToTransfer)
//  {}
 
  /* The software must wait until TC=1. The TC flag remains cleared during all data
  transfers and it is set by hardware at the last frame’s end of transmission*/
//  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//  {}
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/**
  * @brief  This function inits the UART1 peripheral.
  * @param  unsigned char* stringAddress	//address of the string array to send
  * @param  unsigned char size	//size of the string to send
  * @retval None
  */
void USART1_SendString(unsigned char* stringAddress, unsigned char size)
{
	
	//NbrOfDataToTransfer=(sizeof(TxBuffer) - 1);
	NbrOfDataToTransfer=size-1;
	TxAdress=stringAddress;
	TxCount=0;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
// while(TxCount < NbrOfDataToTransfer)
 // {}
	
  /* The software must wait until TC=1. The TC flag remains cleared during all data
  transfers and it is set by hardware at the last frame’s end of transmission*/
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//  {}	
}

/**
  * @brief  This is the console manager, is the state machine to receive and send commands
  * @param  None
  * @retval None
  */
void ConsoleManager()
{
	switch(consoleStatus){
		case SC_BOOT:
			if(consoleStatusPrevious!=SC_BOOT)
				SENDSTRING1(welcomeMsg)
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				consoleStatus=SC_PROMT;
			consoleStatusPrevious=SC_BOOT;
			break;
		case SC_PROMT:
			if(consoleStatusPrevious!=SC_PROMT)
		  {
				SENDSTRING1(promtMSG)
				commandReceived=0;
				RxCount=0;
			}
  		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
			{
				if(commandReceived && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
					consoleStatus=SC_COMMAND;
				else if(Switch1.sent == RESET || Switch2.sent	== RESET | Switch3.sent == RESET | Switch4.sent== RESET )
					{
						consoleStatus=SC_SWITCHMSG;
					}
			}
			consoleStatusPrevious=SC_PROMT;
			break;
		case SC_COMMAND:
			if(RxCount==4 && strncmp((const char*)RxBuffer,"help",4)==0)	
				consoleStatus=SC_HELP;
			else if (RxCount==7 && strncmp((const char *)RxBuffer,"version",7)==0)
				consoleStatus=SC_VERSIONCMD;
			else if (RxCount==6 && strncmp((const char *)RxBuffer,"reboot",6)==0)
				consoleStatus=SC_REBOOT;
			else if (RxCount==4 && strncmp((const char *)RxBuffer,"halt",4)==0)
				consoleStatus=SC_HALT;
			else if (RxCount==7 && strncmp((const char *)RxBuffer,"blemode",7)==0)
				consoleStatus=SC_BLE_MODE;
			else 
				if(RxCount>0)
					consoleStatus=SC_SYNTAXERR;
			commandReceived=0;
			if(RxCount==0)
				consoleStatus=SC_PROMT;
			consoleStatusPrevious=SC_COMMAND;
			break;
		case SC_HELP:
			if(consoleStatusPrevious!=SC_HELP)
				SENDSTRING1(helpMSG)
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				consoleStatus=SC_PROMT;
			consoleStatusPrevious=SC_HELP;
			break;
		case SC_VERSIONCMD:
			if(consoleStatusPrevious!=SC_VERSIONCMD)
				SENDSTRING1(versionMSG)
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				consoleStatus=SC_PROMT;
			consoleStatusPrevious=SC_VERSIONCMD;
			break;
		case SC_SYNTAXERR:
			if(consoleStatusPrevious!=SC_SYNTAXERR)
				SENDSTRING1(systaxErrMSG)
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				consoleStatus=SC_PROMT;
			consoleStatusPrevious=SC_SYNTAXERR;
			break;
		case SC_REBOOT:
			if(consoleStatusPrevious!=SC_REBOOT)
				SENDSTRING1(rebootMSG)
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET){
				NVIC_SystemReset();
			}
			consoleStatusPrevious=SC_REBOOT;
			break;
		case SC_SWITCHMSG:
			if(consoleStatusPrevious!=SC_SWITCHMSG)
			{
				if(Switch1.pushed==SET && Switch1.sent==RESET)
				{
					SENDSTRING1(sw1ONMSG)
					Switch1.sent=SET;
				}
				if(Switch2.status==SET && Switch2.sent==RESET && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				{
					SENDSTRING1(sw2ONMSG)
					Switch2.sent=SET;
				}
				if(Switch3.status==SET && Switch3.sent==RESET && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				{
					SENDSTRING1(sw3ONMSG)
					Switch3.sent=SET;
				}
				if(Switch4.status==SET && Switch4.sent==RESET && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				{
					SENDSTRING1(sw4ONMSG)
					Switch4.sent=SET;
				}
				if(Switch1.status==RESET && Switch1.sent==RESET && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				{
					SENDSTRING1(sw1OFFMSG)
					Switch1.sent=SET;
				}
				if(Switch2.status==RESET && Switch2.sent==RESET && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				{
					SENDSTRING1(sw2OFFMSG)
					Switch2.sent=SET;
				}
				if(Switch3.status==RESET && Switch3.sent==RESET && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				{
					SENDSTRING1(sw3OFFMSG)
					Switch3.sent=SET;
				}
				if(Switch4.status==RESET && Switch4.sent==RESET && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				{
					SENDSTRING1(sw4OFFMSG)
					Switch4.sent=SET;
				}
			}
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
				consoleStatus=SC_PROMT;
			consoleStatusPrevious=SC_SWITCHMSG;
			break;
		case SC_HALT:
			if(consoleStatusPrevious!=SC_HALT)
				SENDSTRING1(HALTMSG)
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET){
				startWindow(0,30);	//3 seconds to off
				consoleStatus=SC_PROMT;
			}
			consoleStatusPrevious=SC_HALT;
			break;
		case SC_BLE_MODE:	//direct communication with the bluetooth device
			if(consoleStatusPrevious!=SC_BLE_MODE){
				SENDSTRING1(promtBMSG)
				commandReceived=0;
				RxCount=0;
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
			}
			if(commandReceived && USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET){
				if(RxCount==4 && strncmp((const char*)RxBuffer,"exit",4)==0)	
					consoleStatus=SC_PROMT;
				else{
					USART2_SendString(RxBuffer,RxCount);	//send command to usart2
					commandReceived=0;
					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
					if(RxCount==0)
						SENDSTRING1(promtBMSG)
					RxCount=0;
				}
			}
			consoleStatusPrevious=SC_BLE_MODE;
			break;
		default:
			consoleStatus=SC_BOOT;			
			break;
	}
}
