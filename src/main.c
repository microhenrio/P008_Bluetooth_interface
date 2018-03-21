/**
  ******************************************************************************
  * @file    main.c
  * @author  A.Rodriguez
  * @version V12
  * @date    May-2015
  * @brief   Main program of the Oled BLE software.
  *          Control of an Oled alphanumeric LCD and a BLE Bluetooth module RN4020
  *
  ==============================================================================
                    ##### Main changes #####
  ==============================================================================

*/
//--v12
//-corregido error en int, no generaba los 5ms

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_conf.h" //variables, structures, constants and defines used
#include "main.h" //variables, structures, constants and defines used
#include "SerialConsole.h" //variables, structures, constants and defines used
#include "LcdDriver.h" //variables, structures, constants and defines used
#include "BluetoothDriver.h" //variables, structures, constants and defines used

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define VERSION 5
#define NUM_WINDOWS 10
#define BKP_VALUE    0x32F0 

#define S_BOOT			0
#define S_WELCOME		1
#define S_STANDBY		2
#define S_CONFIG		3
#define S_OFF				4

#define MENU_BASE			0
#define MENU_BLE			1
#define MENU_DISPLAY	2
#define MENU_POWER		3
#define MENU_CLOCK		4

#define ON_LOCK				GPIO_SetBits(GPIOF, ON_LOCK_PIN);
#define OFF_LOCK			GPIO_ResetBits(GPIOF, ON_LOCK_PIN);
#define RED_LED_ON		GPIO_SetBits(GPIOC, LED1PIN);	//ON led
#define RED_LED_OFF		GPIO_ResetBits(GPIOC, LED1PIN);	//ON led
#define BLUE_LED_ON		GPIO_SetBits(GPIOC, LED2PIN);	
#define BLUE_LED_OFF	GPIO_ResetBits(GPIOC, LED2PIN);	
#define GREEN1_LED_ON		GPIO_SetBits(GPIOC, LED3PIN);	
#define GREEN1_LED_OFF	GPIO_ResetBits(GPIOC, LED3PIN);	
#define GREEN2_LED_ON		GPIO_SetBits(GPIOC, LED4PIN);	
#define GREEN2_LED_OFF	GPIO_ResetBits(GPIOC, LED4PIN);	

#define WRITELCDSTRING(X)	WriteLCDString(X,sizeof(X));
#define WRITELCDSTRING2LINE(X)	WriteLCDString2(X,sizeof(X));
#define WRITELCDSTRINGTIME(X,Y)	WriteLCDStringTimed(X,sizeof(X),Y);

#define fclock_update 	flags01.Bit.b0
#define fclock_set_time flags01.Bit.b1
#define fblink 					flags01.Bit.b2
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef        GPIO_InitStructure;
EXTI_InitTypeDef   			EXTI_InitStructure;
NVIC_InitTypeDef   			NVIC_InitStructure;
TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
TIM_OCInitTypeDef  			TIM_OCInitStructure;
RTC_TimeTypeDef 				RTC_TimeStructure, RTC_TimeStructure_prev;
RTC_DateTypeDef 				RTC_DateStructure, RTC_DateStructure_prev;

extern uint8_t NbrOfDataToTransfer;
__IO uint16_t CCR1_Val = 8000;	//para 100ms
__IO uint16_t CCR2_Val = 4000;	//para 50ms
__IO uint16_t CCR3_Val = 800;		//para 10ms
__IO uint16_t CCR4_Val = 400;		//para 5ms
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

TYPEBYTE_Typedef		baseTimeFlags,flags01;
TIMEWINDOW_Typedef 	timeWindow[NUM_WINDOWS];
FLASH_Status 				FLASHStatus = FLASH_COMPLETE;
SWITCH_Typedef 			Switch1,Switch2,Switch3,Switch4;
//FlagStatus					statusLCD;

unsigned char WELCOME_MSG[]="      Welcome!       OCTAR OLED DISPLAY ";
unsigned char CONFIG_MSG[]="Configuration Menu:";
unsigned char CONFIG_BLE_MSG[] =		"Bluetooth           ";
unsigned char CONFIG_DISPLAY_MSG[] ="Display             ";
unsigned char CONFIG_POWER_MSG[] =	"Power               ";
unsigned char CONFIG_CLOCK_MSG[] =	"Clock               ";
unsigned char POWER_OFF_MSG[] =	"     Power off      ";
unsigned char CONFIG_SET_CLOCK_MSG[] = "Set Clock            ";
unsigned char CONFIG_BLE_ADV_MSG[] ="Start advertising   ";
unsigned char	CONFIG_DISPLAY_SET_TIME_MSG[] = "Set time ON message ";
unsigned char	CONFIG_POWER_AUTO_OFF_MSG[] = "Set auto off time   ";
//unsigned char MONTHS[12][]={"Jan ","Feb ","Mar ","Apr ","May","June","July","Aug ","Sept","Oct ","Nov ","Dec "};
//unsigned char DAYS[7][4]={"Mon","Tues","Weds","Thur","Fri","Sat","Sun"};
unsigned char 
		MONTHS_JAN[]="Jan",
		MONTHS_FEB[]="Feb", 
		MONTHS_MAR[]="Mar",
		MONTHS_APR[]="Apr",
		MONTHS_MAY[]="May",
		MONTHS_JUNE[]="June",
		MONTHS_JULY[]="July",
		MONTHS_AUG[]="Aug",
		MONTHS_SEPT[]="Sept",
		MONTHS_OCT[]="Oct",
		MONTHS_NOV[]="Nov",
		MONTHS_DEC[]="Dec",
		DAYS_MON[]="Monday",
		DAYS_TUES[]="Tuesday",
		DAYS_WEDS[]="Wednesday",
		DAYS_THUR[]="Thursday",
		DAYS_FRI[]="Friday",
		DAYS_SAT[]="Saturday",
		DAYS_SUN[]="Sunday";

uint16_t PrescalerValue = 0;
uint8_t cntWindows,sysop_status,sysop_status_prev,configmenu_status,configmenu_status_prev,configmenu_level;
	unsigned char time_line[20]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
	unsigned char date_line[20]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
	uint8_t size_date=0;
	uint8_t *addr_string;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{ 
	//SetRDP();		//write the RDP reading protection
	clockEnables();
	portsConfig();
	interruptsConfig();
	USART1_Config();
	USART2_Config();
	timersConfig();
	watchdogConfig();
	RTC_Config();
	sysop_status=0xff;	//default status
	sysop_status_prev=0xff;
	Switch1.sent=SET;
	Switch2.sent=SET;
	Switch3.sent=SET;
	Switch4.sent=SET;
	
	while(1)
    {
			if(f5ms)
			{
				ConsoleManager();
				BluetoothManager();
				f5ms=0;
			}
			if(f10ms)
			{
				SysOP();
				f10ms=0;
			}
 			if(f50ms)
			{
				f50ms=0;
			}
			if(f100ms)
			{
				keyboardCheck();
				
				for(cntWindows=0;cntWindows<NUM_WINDOWS;cntWindows++)
				{
					if(timeWindow[cntWindows].on)
					{
						if(timeWindow[cntWindows].time>0)
						{
							timeWindow[cntWindows].time--;
							
						}else{
							timeWindow[cntWindows].on=0;
							timeWindow[cntWindows].end=1;
							timeWindow[cntWindows].time=0;
						}
					}
				}
				/* Time windows analisis */

				if(timeWindow[2].end){	//clears LCD
					timeWindow[2].end=0;
					ClearLCD();
				}
				f100ms=0;
			}
			if(f500ms){
				if(fclock_set_time == SET) 
					fclock_update=1;
				fblink=!fblink;
				
				f500ms=0;
			}
			if(f1s)
			{
				/*if(GPIO_ReadOutputDataBit(GPIOC, LED4PIN))
					GPIO_ResetBits(GPIOC, LED4PIN);	
				else
					GPIO_SetBits(GPIOC, LED4PIN);	*/
				f1s=0;
			}
			//baseTimeFlagsCopy.Byte=baseTimeFlags.Byte;
			//baseTimeFlags.Byte= 0x00; //time flags erase
    }
}



/**
  * @brief  Operative system.
  * @param  None
  * @retval None
  */
void SysOP(void){
	switch(sysop_status){
		case S_BOOT:	//boot status
			if(sysop_status_prev!=S_BOOT){
				ON_LOCK	//locks the power supply
				RED_LED_ON
				BLUE_LED_ON
				GREEN1_LED_ON
				GREEN2_LED_ON
				startWindow(1,5);	//init LCD
			}
			if(timeWindow[1].end){	//init LCD
				timeWindow[0].end=0;	//stops the off window
				timeWindow[0].on=0;
				sysop_status=S_WELCOME;
			}
			sysop_status_prev=S_BOOT;
			break;
		case S_WELCOME:	//welcome message
			if(sysop_status_prev!=S_WELCOME){
				LcdConfig();
				timeWindow[1].end=0;
				WRITELCDSTRINGTIME(WELCOME_MSG,20)
			}
			if(timeWindow[2].end || !timeWindow[2].on){	
				sysop_status=S_STANDBY;
				BLUE_LED_OFF
				GREEN1_LED_OFF
				GREEN2_LED_OFF
			}			
			sysop_status_prev=S_WELCOME;
			break;
		case S_STANDBY:	//waiting for commands, buttons or messages
			if(sysop_status_prev!=S_STANDBY){
				fclock_update=1;
			}
			if(Switch2.pushed==SET){
				sysop_status=S_CONFIG;	//going to config menu
			}
			show_clock();	//shows the full clock in the lcd
			if(timeWindow[0].end){	//going to off
					sysop_status = S_OFF; //going to off
					timeWindow[0].end=0;
			}			
			sysop_status_prev=S_STANDBY;
			break;
		case S_CONFIG:
			if(sysop_status_prev!=S_CONFIG){
				ClearLCD();
				WRITELCDSTRING(CONFIG_MSG)
				configmenu_status=0;
				configmenu_status_prev=0xff;
				configmenu_level=0;
				Switch2.pushed=RESET;
				fclock_set_time=0;
		}
			if(Switch3.pushed==SET && fclock_set_time==RESET ){
				Switch3.pushed=RESET;
				configmenu_status--;
			}
			if(Switch4.pushed==SET && fclock_set_time==RESET){
				Switch4.pushed=RESET;
				configmenu_status++;
			}
			switch(configmenu_level){
				case MENU_BASE:
					switch(configmenu_status){
						case 0:	
							if(configmenu_status_prev!=0){
								WRITELCDSTRING2LINE(CONFIG_BLE_MSG)
							}
							if(Switch2.pushed==SET){
								configmenu_level=MENU_BLE;
								configmenu_status=0;
								configmenu_status_prev=0xff;
								Switch2.pushed=RESET;
							}else
								configmenu_status_prev=0;
							break;
						case 1:	
							if(configmenu_status_prev!=1){
								WRITELCDSTRING2LINE(CONFIG_DISPLAY_MSG)
							}
							if(Switch2.pushed==SET){
								configmenu_level=MENU_DISPLAY;
								configmenu_status=0;
								configmenu_status_prev=0xff;
								Switch2.pushed=RESET;
							}else
								configmenu_status_prev=1;
							break;
						case 2:	
							if(configmenu_status_prev!=2){
								WRITELCDSTRING2LINE(CONFIG_POWER_MSG)
							}
							if(Switch2.pushed==SET){
								configmenu_level=MENU_POWER;
								configmenu_status=0;
								configmenu_status_prev=0xff;
								Switch2.pushed=RESET;
							}else
								configmenu_status_prev=2;
							break;
						case 3:	
							if(configmenu_status_prev!=3){
								WRITELCDSTRING2LINE(CONFIG_CLOCK_MSG)
							}
							if(Switch2.pushed==SET){
								configmenu_level=MENU_CLOCK;
								fclock_set_time=0;
								configmenu_status=0;
								configmenu_status_prev=0xff;
								Switch2.pushed=RESET;
							}else
								configmenu_status_prev=3;
							break;
							
						default:
							if(configmenu_status_prev==0)
								configmenu_status=3; 
							else
								configmenu_status=0;
							configmenu_status_prev=0xff;
							break;
					}
					break;	//fin BASE
				case MENU_BLE:	
					switch(configmenu_status){					
						default:
						case 0: //start advertising
							if(configmenu_status_prev!=0){
								ClearLCD();
								WRITELCDSTRING(CONFIG_BLE_MSG)
								WRITELCDSTRING2LINE(CONFIG_BLE_ADV_MSG)
							}
							configmenu_status_prev=0;
							break;
						}
					if(Switch1.pushed == SET){
						configmenu_level=MENU_BASE;
						configmenu_status_prev=0xff;				
						configmenu_status=0;
						Switch1.pushed = RESET;
						ClearLCD();
						WRITELCDSTRING(CONFIG_MSG)
						Switch2.pushed = RESET;
					}
					break;//fin BLE
				case MENU_DISPLAY:	
					switch(configmenu_status){					
						default:
						case 0: //start advertising
							if(configmenu_status_prev!=0){
								ClearLCD();
								WRITELCDSTRING(CONFIG_DISPLAY_MSG)
								WRITELCDSTRING2LINE(CONFIG_DISPLAY_SET_TIME_MSG)
							}
							configmenu_status_prev=0;
							break;
						}
					if(Switch1.pushed == SET){
						configmenu_level=MENU_BASE;
						configmenu_status_prev=0xff;				
						configmenu_status=1;
						Switch1.pushed = RESET;
						ClearLCD();
						WRITELCDSTRING(CONFIG_MSG)
						Switch2.pushed = RESET;
					}
					break;//fin DISPLAY
				case MENU_POWER:	
					switch(configmenu_status){					
						default:
						case 0: //start advertising
							if(configmenu_status_prev!=0){
								ClearLCD();
								WRITELCDSTRING(CONFIG_POWER_MSG)
								WRITELCDSTRING2LINE(CONFIG_POWER_AUTO_OFF_MSG)
							}
							configmenu_status_prev=0;
							break;
						}
					if(Switch1.pushed == SET){
						configmenu_level=MENU_BASE;
						configmenu_status_prev=0xff;				
						configmenu_status=2;
						Switch1.pushed = RESET;
						ClearLCD();
						WRITELCDSTRING(CONFIG_MSG)
						Switch2.pushed = RESET;
					}
					break;//fin POWER
				case MENU_CLOCK:	
					switch(configmenu_status){
						case 0: //set clock
							if(configmenu_status_prev!=0){
								ClearLCD();
								WRITELCDSTRING(CONFIG_CLOCK_MSG)
								WRITELCDSTRING2LINE(CONFIG_SET_CLOCK_MSG)
								fclock_update=1;
							} 
							if(Switch2.pushed==SET && fclock_set_time == RESET){
								fclock_set_time=SET;
								configmenu_status=1;
								Switch2.pushed= RESET;
							}
							configmenu_status_prev=0;
							break;
						case 1: //set hour
							if(Switch2.pushed==SET){
								configmenu_status=2;
								Switch2.pushed= RESET;
							}
							if(Switch3.pushed==SET){
								RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
								if(RTC_TimeStructure.RTC_Hours==0)
									RTC_TimeStructure.RTC_Hours=23;
								else
									RTC_TimeStructure.RTC_Hours--;
								RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
								fclock_update=1;								
								fblink=0;
								Switch3.pushed=RESET;
							}
							if(Switch4.pushed==SET){
								RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
								if(RTC_TimeStructure.RTC_Hours==23)
									RTC_TimeStructure.RTC_Hours=0;
								else
									RTC_TimeStructure.RTC_Hours++;
								RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
								fclock_update=1;								
								fblink=0;
								Switch4.pushed=RESET;
							}							
							configmenu_status_prev=1;
							break;
						case 2: //set minute
							if(Switch2.pushed==SET){
								configmenu_status=3;
								Switch2.pushed= RESET;
							}
							if(Switch3.pushed==SET){
								RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
								if(RTC_TimeStructure.RTC_Minutes==0)
									RTC_TimeStructure.RTC_Minutes=59;
								else
									RTC_TimeStructure.RTC_Minutes--;
								RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
								fclock_update=1;								
								fblink=0;
								Switch3.pushed=RESET;
							}
							if(Switch4.pushed==SET){
								RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
								if(RTC_TimeStructure.RTC_Minutes==59)
									RTC_TimeStructure.RTC_Minutes=0;
								else
									RTC_TimeStructure.RTC_Minutes++;
								RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
								fclock_update=1;								
								fblink=0;
								Switch4.pushed=RESET;
							}							
							configmenu_status_prev=2;
							break;
						case 3: //set weekday
							if(Switch2.pushed==SET){
								configmenu_status=4;
								Switch2.pushed= RESET;
							}
							if(Switch3.pushed==SET){
								RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);								
								if(RTC_DateStructure.RTC_WeekDay==RTC_Weekday_Monday)
									RTC_DateStructure.RTC_WeekDay=RTC_Weekday_Sunday;
								else
									RTC_DateStructure.RTC_WeekDay--;
								RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);								
								fclock_update=1;								
								fblink=0;
								Switch3.pushed=RESET;
							}
							if(Switch4.pushed==SET){
								RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);								
								if(RTC_DateStructure.RTC_WeekDay==RTC_Weekday_Sunday)
									RTC_DateStructure.RTC_WeekDay=RTC_Weekday_Monday;
								else
									RTC_DateStructure.RTC_WeekDay++;
								RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);								
								fclock_update=1;								
								fblink=0;
								Switch4.pushed=RESET;
							}							
							configmenu_status_prev=3;
							break;
						case 4: //set date
							if(Switch2.pushed==SET){
								configmenu_status=5;
								Switch2.pushed= RESET;
							}
							if(Switch3.pushed==SET){
								RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);								
								if(RTC_DateStructure.RTC_Date==1)
									RTC_DateStructure.RTC_Date=31;
								else
									RTC_DateStructure.RTC_Date--;
								RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);								
								fclock_update=1;								
								fblink=0;
								Switch3.pushed=RESET;
							}
							if(Switch4.pushed==SET){
								RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);								
								if(RTC_DateStructure.RTC_Date==31)
									RTC_DateStructure.RTC_Date=1;
								else
									RTC_DateStructure.RTC_Date++;
								RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);								
								fclock_update=1;								
								fblink=0;
								Switch4.pushed=RESET;
							}							
							configmenu_status_prev=4;
							break;
						case 5: //set month
							if(Switch2.pushed==SET){
								configmenu_status=1;	//vuelvo a las horas
								Switch2.pushed= RESET;
							}
							if(Switch3.pushed==SET){
								RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);	
								if(RTC_DateStructure.RTC_Month==RTC_Month_October)
									RTC_DateStructure.RTC_Month=RTC_Month_September;
								else if(RTC_DateStructure.RTC_Month==RTC_Month_January)
									RTC_DateStructure.RTC_Month=RTC_Month_December;
								else
									RTC_DateStructure.RTC_Month--;
								RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);								
								fclock_update=1;								
								fblink=0;
								Switch3.pushed=RESET;
							}
							if(Switch4.pushed==SET){
								RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);								
								if(RTC_DateStructure.RTC_Month==RTC_Month_December)
									RTC_DateStructure.RTC_Month=RTC_Month_January;
								else if(RTC_DateStructure.RTC_Month==RTC_Month_September)
									RTC_DateStructure.RTC_Month=RTC_Month_October;
								else
									RTC_DateStructure.RTC_Month++;
								RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);								
								fclock_update=1;								
								fblink=0;
								Switch4.pushed=RESET;
							}							
							configmenu_status_prev=5;
							break;
							//año?
						default:
							configmenu_status=0;
							configmenu_status_prev=0xff;
							Switch2.pushed= RESET;
						
							break;
					}
					if(Switch1.pushed == SET){
						configmenu_level=MENU_BASE;
						configmenu_status_prev=0xff;				
						configmenu_status=3;
						Switch1.pushed = RESET;
						ClearLCD();
						WRITELCDSTRING(CONFIG_MSG)
						Switch2.pushed = RESET;
						fclock_set_time=0;
					}
					if(fclock_set_time == SET){
						show_clock();	//shows the full clock in the lcd									
					}
					break;//fin CLOCK
				default:
					configmenu_level=MENU_BASE;
					configmenu_status_prev=0xff;				
					configmenu_status=0;
					break;
			}
			if((Switch1.pushed == SET && configmenu_level == MENU_BASE)  || timeWindow[0].end){
				Switch2.pushed = RESET;
				ClearLCD();
				sysop_status=S_STANDBY;	//return to standby
				if(timeWindow[0].end) Switch1.pushed = SET;// para poder hacer power off desde config
			}
			
			sysop_status_prev=S_CONFIG;
			break;
		case S_OFF:
			if(sysop_status_prev != S_OFF){
				RED_LED_OFF
				BLUE_LED_OFF
				GREEN1_LED_OFF
				GREEN2_LED_OFF
				ClearLCD();
				WRITELCDSTRING(POWER_OFF_MSG)
			}
			if(Switch1.pushed==RESET){
				ClearLCD();
				OFF_LOCK	//locks the power supply
			}

			sysop_status_prev = S_OFF;
			break;
		default:
			sysop_status=S_BOOT;
			sysop_status_prev=0xff;
		
			break;
	}
	
}
/* Enable clocks of peripherals */
void clockEnables(void)
{
/* Enable GPIO's clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOF, ENABLE);

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* Enable SYSCFG clock */  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Enable USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Enable USART1 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//	RCC->CFGR3 |= (uint32_t)0x00000003;	
	
}
/* configures teh hardware ports*/
void portsConfig(void)
{
	/* PA Port USART1 & USART2 pins alternate function*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);
	
	/* Select pins as alternate function */
	GPIO_InitStructure.GPIO_Pin = CTS_M0 | RTS_M0 | TX_M0 | RX_M0 | USART1_TX | USART1_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//GPIO_OType_PP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA Port GPIO inputs*/
	GPIO_InitStructure.GPIO_Pin = SWITCH2PIN | USB_LP | BLE_CONNECT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA Port GPIO outputs*/
	GPIO_InitStructure.GPIO_Pin = BLE_WHW | BLE_WSW | BLE_CMD_MLDP | CHARGER_EN1 | CHARGER_EN2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PB Port I2C pins*/
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8 | GPIO_PinSource9,GPIO_AF_1);
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PB Port GPIO inputs*/
	GPIO_InitStructure.GPIO_Pin = LCD_B0 | LCD_B1 | LCD_B2 | LCD_B3 | LCD_B4 | LCD_B5 | LCD_B6 | LCD_B7 | BLE_WS | BLE_MLDP_EV | CHARGER_PG | CHARGER_CHG;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PB Port GPIO outputs*/
	GPIO_InitStructure.GPIO_Pin = CHARGER_CE | CHARGER_SYSOFF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PC Port ADC pins*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* PC Port GPIO inputs*/
	GPIO_InitStructure.GPIO_Pin = SWITCH3PIN | SWITCH4PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* PC Port GPIO outputs*/
	GPIO_InitStructure.GPIO_Pin =  LED1PIN | LED2PIN | LED3PIN | LED4PIN | LCD_E | LCD_RS | LCD_RW;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* PF Port GPIO inputs*/
	GPIO_InitStructure.GPIO_Pin = SWITCHONPIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* PF Port GPIO outputs*/
	GPIO_InitStructure.GPIO_Pin =  ON_LOCK_PIN | AUX1 | AUX2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* PF Port GPIO LSE*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);


}
/* configures the interrupts */
void interruptsConfig(void)
{
  /* Enable the TIM3 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/* Enable the USART Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* SWITCH_ON_3V3 Interrupt */	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource4);	
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);

	/* SWITCH2 Interrupt */	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7);	
  EXTI_InitStructure.EXTI_Line = EXTI_Line7;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI4_15 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
}
/* configures and starts the hardware timers */
void timersConfig(void)
{
    /* -----------------------------------------------------------------------
    TIM3 Configuration: Output Compare Timing Mode:
    
    In this example TIM3 input clock (TIM3CLK) is set to APB1 clock (PCLK1),  
      => TIM3CLK = PCLK1 = SystemCoreClock = 8 MHz
          
    To get TIM3 counter clock at 80kHz, the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = (PCLK1 /80KHz) - 1
                                              
    CC1 update rate = TIM3 counter clock / CCR1_Val = 10 Hz
    CC2 update rate = TIM3 counter clock / CCR2_Val =  20 Hz
    CC3 update rate = TIM3 counter clock / CCR3_Val = 50 Hz
    CC4 update rate = TIM3 counter clock / CCR4_Val = 200 Hz

    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
  ----------------------------------------------------------------------- */   


  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock  / 80000) - 1;
  

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

  TIM_OC3Init(TIM3, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
   
  /* TIM Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);	
}

/* Set the level 1 flag to avoid readings of the flash */
void SetRDP(void)
{
 
  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();
 
  FLASH_OB_Unlock();
 
  /* Clear all FLASH flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY);
 
  if (FLASH_OB_GetRDP()==RESET)
  {
    FLASHStatus = FLASH_OB_RDPConfig(OB_RDP_Level_1); // Locks Chip (Lvl 2 Toasts)
 
    if (FLASHStatus == FLASH_COMPLETE)
    {
      /* Generate System Reset to load the new option byte values */
      FLASH_OB_Launch();
    }
  } 
}

/* checks if some keys are pressed, avoid bouncing by calling it at interval */
void keyboardCheck(void){
	if(GPIO_ReadInputDataBit(GPIOF,SWITCHONPIN)==SET)
	{
		Switch1.status=SET;
		if(Switch1.status_ant==RESET){
			Switch1.pushed=SET;
			Switch1.released=RESET;
			Switch1.status_ant=SET;
			Switch1.sent=RESET;
			startWindow(0,30);	//3 seconds to off
		}
	}else{
		Switch1.status=RESET;
		if(Switch1.status_ant==SET){
			Switch1.released=SET;
			Switch1.pushed=RESET;
			Switch1.status_ant=RESET;
			Switch1.sent=RESET;
			timeWindow[0].on=0;
		}
	}
	
	if(GPIO_ReadInputDataBit(GPIOA, SWITCH2PIN)==SET)
	{
		Switch2.status=SET;
		if(Switch2.status_ant==RESET){
			Switch2.pushed=SET;
			Switch2.released=RESET;
			Switch2.status_ant=SET;
			Switch2.sent=RESET;
			BLUE_LED_ON
		}
	}else{
		Switch2.status=RESET;
		if(Switch2.status_ant==SET){
			Switch2.released=SET;
			Switch2.pushed=RESET;
			Switch2.status_ant=RESET;
			Switch2.sent=RESET;
			BLUE_LED_OFF
		}
	}
	
	if(GPIO_ReadInputDataBit(GPIOC, SWITCH3PIN)==SET)
	{
		Switch3.status=SET;
		if(Switch3.status_ant==RESET){
			Switch3.pushed=SET;
			Switch3.released=RESET;
			Switch3.status_ant=SET;
			Switch3.sent=RESET;
			GREEN1_LED_ON
		}
	}else{
		Switch3.status=RESET;
		if(Switch3.status_ant==SET){
			Switch3.released=SET;
			Switch3.pushed=RESET;
			Switch3.status_ant=RESET;
			Switch3.sent=RESET;
			GREEN1_LED_OFF
		}
	}
	
	if(GPIO_ReadInputDataBit(GPIOC, SWITCH4PIN)==SET)
	{
		Switch4.status=SET;
		if(Switch4.status_ant==RESET){
			Switch4.pushed=SET;
			Switch4.released=RESET;
			Switch4.status_ant=SET;
			Switch4.sent=RESET;
			GREEN2_LED_ON
		}
	}else{
		Switch4.status=RESET;
		if(Switch4.status_ant==SET){
			Switch4.released=SET;
			Switch4.pushed=RESET;
			Switch4.status_ant=RESET;
			Switch4.sent=RESET;
			GREEN2_LED_OFF
		}
	}
}
/* configures and starts the time windows */
void startWindow(uint8_t Wnumber, uint16_t Wtime)
{
	timeWindow[Wnumber].end=0;
	timeWindow[Wnumber].time=Wtime;
	timeWindow[Wnumber].on=1;
}

void watchdogConfig(void)
{
}

static void RTC_Config(void){
	RTC_InitTypeDef RTC_InitStructure;

	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != BKP_VALUE)
  {  
    /* RTC configuration  */
    RTC_Enable();

    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   
    /* Check on RTC init */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
     // printf("\n\r        /!\\***** RTC Prescaler Config failed ********/!\\ \n\r");
    }

    /* Configure the time register */
    //RTC_TimeRegulate(); 
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
     // printf("\r\n Power On Reset occurred....\n\r");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      //printf("\r\n External Reset occurred....\n\r");
    }

   // printf("\n\r No need to configure RTC....\n\r");
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

 //#ifdef RTC_CLOCK_SOURCE_LSI
    /* Enable the LSI OSC */ 
 //   RCC_LSICmd(ENABLE);
// #endif /* RTC_CLOCK_SOURCE_LSI */

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Clear the RTC Alarm Flag */
    RTC_ClearFlag(RTC_FLAG_ALRAF);

    /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
    EXTI_ClearITPendingBit(EXTI_Line17);

    /* Display the RTC Time and Alarm */
  //  RTC_TimeShow();
  //  RTC_AlarmShow();
  }
}
/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void RTC_Enable(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
    

  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
  SynchPrediv = 0xFF;
		AsynchPrediv = 0x7F;
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
}
/**
  * @brief  Shows in the LCD the actual time and date.
  * @param  None
  * @retval None
  */
void show_clock(void){
	uint8_t cnt_date=0,size_month=0;
	
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
//  printf("\n\r  The current time is :  %0.2d:%0.2d:%0.2d \n\r", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
	if(RTC_TimeStructure_prev.RTC_Seconds != RTC_TimeStructure.RTC_Seconds ||  fclock_update==1){
		if(fclock_set_time == SET && configmenu_status==1 && fblink ){
			time_line[7]='_';
			time_line[8]='_';
		}else{
			if(RTC_TimeStructure.RTC_Hours >0x0f){
				time_line[7]=(RTC_TimeStructure.RTC_Hours >>4)+48;
			}else{
				time_line[7]=' ';
			}
			time_line[8]=(RTC_TimeStructure.RTC_Hours & 0x0f)+48;
		}
		if(RTC_TimeStructure.RTC_Seconds%2==1){
			time_line[9]=':';
		}else{
			time_line[9]=' ';
		}
		if(fclock_set_time == SET && configmenu_status==2 && fblink ){
			time_line[10]='_';
			time_line[11]='_';
		}else{
			time_line[10]=(RTC_TimeStructure.RTC_Minutes >>4)+48;
			time_line[11]=(RTC_TimeStructure.RTC_Minutes & 0x0f)+48;
		}
		if(RTC_TimeStructure.RTC_H12==RTC_H12_PM){
			time_line[12]='P';
			time_line[13]='M';
		}else{
			time_line[12]=' ';
			time_line[13]=' ';
		}
		WRITELCDSTRING(time_line)
	}
	if(RTC_DateStructure_prev.RTC_Date != RTC_DateStructure.RTC_Date ||  fclock_update==1 ){
			/*	if(sizeof(DAYS[RTC_DateStructure.RTC_WeekDay-1])==4){
					date_line[0]=DAYS[RTC_DateStructure.RTC_WeekDay-1][0];
					date_line[1]=DAYS[RTC_DateStructure.RTC_WeekDay-1][1];
					date_line[2]=DAYS[RTC_DateStructure.RTC_WeekDay-1][2];
					date_line[3]=DAYS[RTC_DateStructure.RTC_WeekDay-1][3];
				}*/
		switch(RTC_DateStructure.RTC_WeekDay){
			case RTC_Weekday_Monday:
				size_date=sizeof(DAYS_MON);
				addr_string=DAYS_MON;
				break;
			case RTC_Weekday_Tuesday:
				size_date=sizeof(DAYS_TUES);
				addr_string=DAYS_TUES;
				break;
			case RTC_Weekday_Wednesday:
				size_date=sizeof(DAYS_WEDS);
				addr_string=DAYS_WEDS;
				break;
			case RTC_Weekday_Thursday:
				size_date=sizeof(DAYS_THUR);
				addr_string=DAYS_THUR;
				break;
			case RTC_Weekday_Friday:
				size_date=sizeof(DAYS_FRI);
				addr_string=DAYS_FRI;
				break;
			case RTC_Weekday_Saturday:
				size_date=sizeof(DAYS_SAT);
				addr_string=DAYS_SAT;
				break;
			case RTC_Weekday_Sunday:
				size_date=sizeof(DAYS_SUN);
				addr_string=DAYS_SUN;
				break;
		}
		//size_date--;	//sizeof devuelve uno más?!
		for(cnt_date=0;cnt_date<(size_date-1);cnt_date++){
			if(fclock_set_time == SET && configmenu_status==3 && fblink)
				date_line[cnt_date]='_';
			else
				date_line[cnt_date]=*(addr_string+cnt_date);
		}
		date_line[size_date-1]=' ';
//		size_date++;
		if(RTC_DateStructure.RTC_Date<0x10){
			if(fclock_set_time == SET && configmenu_status==4 && fblink)
				date_line[size_date]='_';
			else
				date_line[size_date]=RTC_DateStructure.RTC_Date+48;
			size_date++;
		}else{
			if(fclock_set_time == SET && configmenu_status==4 && fblink){
				date_line[size_date]='_';
				size_date++;
				date_line[size_date]='_';
			}else{
				date_line[size_date]=(RTC_DateStructure.RTC_Date>>4)+48;
				size_date++;
				date_line[size_date]=(0x0F & RTC_DateStructure.RTC_Date)+48;
			}
			size_date++;
		}
		date_line[size_date]=' ';
		size_date++;
		switch(RTC_DateStructure.RTC_Month){
			case RTC_Month_January:
				size_month=sizeof(MONTHS_JAN);
				addr_string=MONTHS_JAN;
				break;
			case RTC_Month_February:
				size_month=sizeof(MONTHS_FEB);
				addr_string=MONTHS_FEB;
				break;
			case RTC_Month_March:
				size_month=sizeof(MONTHS_MAR);
				addr_string=MONTHS_MAR;
				break;
			case RTC_Month_April:
				size_month=sizeof(MONTHS_APR);
				addr_string=MONTHS_APR;
				break;
			case RTC_Month_May:
				size_month=sizeof(MONTHS_MAY);
				addr_string=MONTHS_MAY;
				break;
			case RTC_Month_June:
				size_month=sizeof(MONTHS_JUNE);
				addr_string=MONTHS_JUNE;
				break;
			case RTC_Month_July:
				size_month=sizeof(MONTHS_JULY);
				addr_string=MONTHS_JULY;
				break;
			case RTC_Month_August:
				size_month=sizeof(MONTHS_AUG);
				addr_string=MONTHS_AUG;
				break;
			case RTC_Month_September:
				size_month=sizeof(MONTHS_SEPT);
				addr_string=MONTHS_SEPT;
				break;
			case RTC_Month_October:
				size_month=sizeof(MONTHS_OCT);
				addr_string=MONTHS_OCT;
				break;
			case RTC_Month_November:
				size_month=sizeof(MONTHS_NOV);
				addr_string=MONTHS_NOV;
				break;
			case RTC_Month_December:
				size_month=sizeof(MONTHS_DEC);
				addr_string=MONTHS_DEC;
				break;
			default:
				size_month=sizeof(MONTHS_DEC);
				addr_string=MONTHS_DEC;
				break;
		
		}		
		for(cnt_date=0;cnt_date<(size_month-1);cnt_date++){
			if(fclock_set_time == SET && configmenu_status==5 && fblink)
				date_line[size_date+cnt_date]='_';
			else
				date_line[size_date+cnt_date]=*(addr_string+cnt_date);
		}
		size_date+=size_month-1;
/*		
		date_line[size_date]=' ';
		spaces_size=(20-size_date)/2;	//tamaño espacios delante 
		if(size_date%2==1){	//tamaño texto impar, más espacios detrás que delante
			
		}
		for(cnt_date=10;cnt_date>0;cnt_date--){
			
			date_line[size_date+cnt_date]=*(addr_string+cnt_date);
		}
		*/
		WriteLCDLineCenter(date_line,size_date,1);
	}
	RTC_TimeStructure_prev.RTC_Seconds=RTC_TimeStructure.RTC_Seconds;	//update of the less value dates
	RTC_DateStructure_prev.RTC_Date=RTC_DateStructure.RTC_Date;
	fclock_update=0;
}

