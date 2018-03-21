/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "stm32f0xx_exti.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_usart.h"
#include "SerialConsole.h" //variables, structures, constants and defines used
#include "BluetoothDriver.h" //variables, structures, constants and defines used
#include <string.h>

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint16_t capture = 0;
extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;
extern __IO uint16_t CCR3_Val;
extern __IO uint16_t CCR4_Val;
extern TYPEBYTE_Typedef	baseTimeFlags;

unsigned char cntUnSegundo=0;
unsigned int aux_string;
extern uint8_t NbrOfDataToTransfer,NbrOfDataToTransfer2;
extern uint8_t NbrOfDataToRead,NbrOfDataToRead2;
extern __IO uint8_t TxCount,TxCount2; 
extern __IO uint8_t RxCount,RxCount2; 
extern unsigned char* TxAdress;
extern unsigned char*	TxAdress2;
extern uint8_t RxBuffer[],RxBuffer2[];
extern uint8_t commandReceived;
extern SWITCH_Typedef Switch1,Switch2,Switch3,Switch4;
extern BLE_Typedef	bluetooth;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

    /* LED1 toggling with frequency = 73.24 Hz */
//    STM_EVAL_LEDToggle(LED1);
	capture = TIM_GetCapture1(TIM3);
    TIM_SetCompare1(TIM3, capture + CCR1_Val);
		f100ms=1;
		cntUnSegundo++;
		if(cntUnSegundo==5)
			f500ms=1;
		if(cntUnSegundo>=10)
		{
			cntUnSegundo=0;
			f1s=1;
			f500ms=1;
		}
  }
  else if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);

    /* LED2 toggling with frequency = 109.8 Hz */
//    STM_EVAL_LEDToggle(LED2);
		f50ms=1;
    capture = TIM_GetCapture2(TIM3);
    TIM_SetCompare2(TIM3, capture + CCR2_Val);
  }
  else if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);

    /* LED3 toggling with frequency = 219.7 Hz */
//    STM_EVAL_LEDToggle(LED3);
		f10ms=1;
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare3(TIM3, capture + CCR3_Val);
  }
  else
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);

    /* LED4 toggling with frequency = 439.4 Hz */
//    STM_EVAL_LEDToggle(LED4);
		f5ms=1;
    capture = TIM_GetCapture4(TIM3);
    TIM_SetCompare4(TIM3, capture + CCR4_Val);
  }
}

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */

void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer[RxCount] = (USART_ReceiveData(USART1) & 0x7F);
		if(RxCount == NbrOfDataToRead || RxBuffer[RxCount]==0x0d )
    {
      /* Disable the EVAL_COM1 Receive interrupt */
      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
			commandReceived=1;
    }else
			if(RxBuffer[RxCount]!=0x0a){
				if(RxBuffer[RxCount]==0x7f){ 	//LF and backspace
					if(RxCount>0)
						RxCount--;
				}else
				{
					RxCount++;
				}
			}
  }

  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    //USART_SendData(USART1, TxBuffer[TxCount++]);
		USART_SendData(USART1,*(TxAdress+(TxCount++)));

    if(TxCount == NbrOfDataToTransfer)
    {
      /* Disable the EVAL_COM1 Transmit interrupt */
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
  }
}

/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
		if(bluetooth.status!=SB_BLEMODE)
		{
			/* Read one byte from the receive data register */
			RxBuffer2[RxCount2] = (USART_ReceiveData(USART2) & 0x7F);
			//if(RxCount2 == NbrOfDataToRead2 || RxBuffer2[RxCount2]==0x0d )
			if(RxCount2 == NbrOfDataToRead2)
			{
				/* Disable the EVAL_COM1 Receive interrupt */
				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
				
				bluetooth.ERR=ERROR;
				bluetooth.AOK=RESET;
			}else{
				if(RxBuffer2[RxCount2]==0x0a){	//end of command
					if(strncmp((const char *)RxBuffer2,"CMD",3)==0){
						bluetooth.AOK=SET;
					}
					if(strncmp((const char *)RxBuffer2,"ERR",3)==0){
						bluetooth.AOK=RESET;
						bluetooth.ERR=ERROR;
					}
					bluetooth.command_pending=RESET;
					bluetooth.command_received=SET;
					//RxCount2=0;	//command processed
				}else
					RxCount2++;
			}
		}else{
			USART_SendData(USART1,(USART_ReceiveData(USART2) & 0x7F));	//send received form BLE to console
		}
	}
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
		if(TxCount2 < NbrOfDataToTransfer2)	//buffer full
			USART_SendData(USART2,*(TxAdress2+(TxCount2++)));
		if(TxCount2 == NbrOfDataToTransfer2){
			USART_SendData(USART2,'\r');
			TxCount2++;
		}else{
			if(TxCount2 == NbrOfDataToTransfer2+1){
				USART_SendData(USART2,'\r');
				TxCount2=0;
				USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			}
		}
  }
}

/**
  * @brief  This function handles Externals interrupts 4 to 15 .
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4))	//Switch1
	{
		//Switch1.SwitchStatus=SET;
		while(EXTI_GetITStatus(EXTI_Line4))
		{
			EXTI_ClearITPendingBit(EXTI_Line4);
		}
		__NOP();
	}
	if(EXTI_GetITStatus(EXTI_Line7))	//Switch2
	{
		//Switch2.SwitchStatus=SET;
		
		while(EXTI_GetITStatus(EXTI_Line7))
		{
			EXTI_ClearITPendingBit(EXTI_Line7);
		}
		__NOP();
	}
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
