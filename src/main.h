/**
  ******************************************************************************
  * @file    P008_main_V00.h
  * @author  A.Rodriguez
  * @version V00
  * @date    May-2015
  * @brief   Header for P008_main_V00.c module
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
	unsigned char b0:1;
	unsigned char b1:1;
	unsigned char b2:1;
	unsigned char b3:1;
	unsigned char b4:1;
	unsigned char b5:1;
	unsigned char b6:1;
	unsigned char b7:1;
}BIT8_STRUCT_TypeDef;

typedef union
{
	unsigned char Byte;
	BIT8_STRUCT_TypeDef Bit;
} TYPEBYTE_Typedef;

typedef struct
{
	uint16_t time;
	uint8_t on:1;
	uint8_t  end:1;
}TIMEWINDOW_Typedef;

typedef struct
{
	ITStatus	status;
	ITStatus	status_ant;
	ITStatus	pushed;
	ITStatus	released;
	ITStatus	ledStatus;
	ITStatus	sent;
}SWITCH_Typedef;
/*
typedef struct
{
	ITStatus	enabled;
	ITStatus	valid;
	ITStatus	format24H;	
	unsigned char 	hour;
	unsigned char 	minute;
	unsigned char 	seconds;
	unsigned char 	dayNumber;
	unsigned char 	weekday;
	unsigned char 	month;
}CLOCK_Typedef;
*/
/* Private define ------------------------------------------------------------*/
/***** PORT A *****/
#define CTS_M0				GPIO_Pin_0
#define RTS_M0	 			GPIO_Pin_1
#define TX_M0		 			GPIO_Pin_2
#define RX_M0		 			GPIO_Pin_3
#define BLE_WHW 			GPIO_Pin_4
#define BLE_WSW  			GPIO_Pin_5
#define BLE_CMD_MLDP 	GPIO_Pin_6
#define SWITCH2PIN 			GPIO_Pin_7
#define USB_LP 				GPIO_Pin_8
#define USART1_TX			GPIO_Pin_9
#define USART1_RX			GPIO_Pin_10
#define CHARGER_EN1 	GPIO_Pin_11
#define CHARGER_EN2 	GPIO_Pin_12
#define SWDAT					GPIO_Pin_13
#define SWCLK				 	GPIO_Pin_14
#define BLE_CONNECT		GPIO_Pin_15

/***** PORT B *****/
#define LCD_B0					GPIO_Pin_0
#define LCD_B1					GPIO_Pin_1
#define LCD_B2					GPIO_Pin_2
#define LCD_B3					GPIO_Pin_3
#define LCD_B4					GPIO_Pin_4
#define LCD_B5					GPIO_Pin_5
#define LCD_B6					GPIO_Pin_6
#define LCD_B7					GPIO_Pin_7

#define I2C1_SCL 				GPIO_Pin_8
#define I2C1_SDA				GPIO_Pin_9
#define BLE_WS 					GPIO_Pin_10
#define BLE_MLDP_EV			GPIO_Pin_11
#define CHARGER_CE 			GPIO_Pin_12
#define CHARGER_PG 			GPIO_Pin_13
#define CHARGER_CHG 		GPIO_Pin_14
#define CHARGER_SYSOFF 	GPIO_Pin_15

/***** PORT C *****/
#define SWITCH3PIN 	GPIO_Pin_4
#define SWITCH4PIN 	GPIO_Pin_5
#define LED1PIN 			GPIO_Pin_6
#define LED2PIN 			GPIO_Pin_7
#define LED3PIN 			GPIO_Pin_8
#define LED4PIN 			GPIO_Pin_9
#define LCD_E 		GPIO_Pin_10
#define LCD_RS 		GPIO_Pin_11
#define LCD_RW 		GPIO_Pin_12

/***** PORT F *****/
#define SWITCHONPIN 	GPIO_Pin_4
#define ON_LOCK_PIN		GPIO_Pin_5
#define AUX1 					GPIO_Pin_6
#define AUX2					GPIO_Pin_7

#define f1ms 	baseTimeFlags.Bit.b0 
#define f5ms baseTimeFlags.Bit.b1 
#define f10ms baseTimeFlags.Bit.b2 
#define f50ms baseTimeFlags.Bit.b3 
#define f100ms baseTimeFlags.Bit.b4
#define f500ms baseTimeFlags.Bit.b5
#define f1s 	baseTimeFlags.Bit.b6

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static void clockEnables(void);	//enables the clocks of peripherals
static void portsConfig(void);		//port configurations
static void timersConfig(void);		//timer configurations
static void interruptsConfig(void);	//NVIC controller configuration
static void watchdogConfig(void);	//whatchodg configuration
static void SetRDP(void);			//read protection configuration
static void keyboardCheck(void);	//checks if some key is pressed
static void	SysOP(void);		//State machine
void startWindow(uint8_t Wnumber, uint16_t Wtime);	//starts time windows
static void RTC_Config(void);
static void RTC_Enable(void);
static void show_clock(void);

