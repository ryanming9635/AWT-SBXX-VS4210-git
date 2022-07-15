/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f103xb.h"
#include "VXIS_Extern_Define.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define	MCU_FW_Major			0x00	
#define	MCU_FW_Sub0			0x01		
#define	MCU_FW_Sub1			0x00	
//#define	FWVER				0x0001

#define	_NTSC			1
#define	_PAL			2

#define SET_VX8812_Video  _NTSC


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void main_init (void);
void uComOnChipInitial(void);
void LCDPowerON(BYTE set);
void LCDPowerOFF(void);
void user_pwm_setvalue(uint16_t value);
void main_loop(void);
void BackLightON(void);
void BackLightOFF(void);
void SETTW8812OFF(void);
void SETTW8812ON(void);

extern BYTE GET_PCA9554PW(BYTE reg);
extern void  SET_PCA9554PW(BYTE addr, BYTE val);
void LoadEEPROM (void);


extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

extern void VXOSD_UseFont_init(uint8_t Size,const uint8_t *UserDefineFontPtr); //20200110-01
extern void PowerLED(EnumSystemTimerEventID flag);
extern void MCUTimerDelayXms(WORD usNum);
extern BYTE CheckKeyIn(void);
extern void TriggerHandler(BYTE T_Flag);
extern BYTE CheckTrigger(void);
extern BYTE GetAudioVolEE(void);
extern void LoadVideoData(void);
extern void VS4210_SetCamMirror(tByte ch, tByte win );
extern void AUDIO_SELECT(BYTE sel);
extern void MCUTimerCancelTimerEvent(BYTE ucEventID);
extern void SET_VX8812_SD_Video_Out(tByte tbMode);
extern void SW_I2C3_Init(void);
extern BYTE GT9147_Scan(BYTE mode);  
extern BYTE GT9147_Init(void);
extern void Get_2824cIrq(void);//cocrx
extern void Boot_Trigger_Function(BYTE ch);
extern void AUDIO_SELECT_MUTE(BYTE sel);
extern void ClearBasicEE(void);




//extern uint8_t ReadEEP( uint16_t reg);
//extern void WriteEEP (uint16_t reg, uint8_t val);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

#define TX_RST_Pin 			GPIO_PIN_0
#define TX_RST_GPIO_Port 		GPIOA

#define TP2824_RST_Pin 		GPIO_PIN_1
#define TP2824_RST_GPIO_Port GPIOA
/*
#define IR_IN_Pin 				GPIO_PIN_2
#define IR_IN_GPIO_Port 		GPIOA
#define IR_IN_EXTI_IRQn 		EXTI2_IRQn
*/
#define IR_IN_Pin 				GPIO_PIN_0
#define IR_IN_GPIO_Port 		GPIOC
#define IR_IN_EXTI_IRQn 		EXTI0_IRQn

#define VS_RST_Pin 			GPIO_PIN_4
#define VS_RST_GPIO_Port 	GPIOA

#define RX1_Pin 				GPIO_PIN_0
#define RX1_GPIO_Port 		GPIOB

#define SW1_Pin 				GPIO_PIN_12
#define SW1_GPIO_Port 		GPIOB

#define SW2_Pin 				GPIO_PIN_13
#define SW2_GPIO_Port 		GPIOB

#define SW3_Pin 				GPIO_PIN_14
#define SW3_GPIO_Port 		GPIOB

#define SW4_Pin 				GPIO_PIN_15
#define SW4_GPIO_Port 		GPIOB

#define SW5_Pin 				GPIO_PIN_3
#define SW5_GPIO_Port 		GPIOB

#define SW6_Pin 				GPIO_PIN_4
#define SW6_GPIO_Port 		GPIOB

#define SW7_Pin 				GPIO_PIN_5
#define SW7_GPIO_Port 		GPIOB

//LED
#define GREEN_LED_Pin 		GPIO_PIN_9
#define GREEN_LED_GPIO_Port 	GPIOC

#define RED_LED_Pin			GPIO_PIN_8
#define RED_LED_GPIO_Port	GPIOC


//KEY
#define SOURCE_KEY_Pin			GPIO_PIN_7
#define SOURCE_KEY_GPIO_Port	GPIOC

#define JUMP_KEY_Pin			GPIO_PIN_6
#define JUMP_KEY_GPIO_Port	GPIOC

#define DOWN_KEY_Pin			GPIO_PIN_15
#define DOWN_KEY_GPIO_Port		GPIOB

#define UP_KEY_Pin			GPIO_PIN_14
#define UP_KEY_GPIO_Port		GPIOB

#define MENU_KEY_Pin			GPIO_PIN_13
#define MENU_KEY_GPIO_Port	GPIOB

#define POWER_KEY_Pin			GPIO_PIN_12
#define POWER_KEY_GPIO_Port		GPIOB

//Audio sel
#define AUD_SEL1_Pin			GPIO_PIN_2
#define AUD_SEL1_GPIO_Port		GPIOD

#define AUD_SEL2_Pin			GPIO_PIN_12
#define AUD_SEL2_GPIO_Port		GPIOC


//Panel
#define BL_ENABLE_Pin		GPIO_PIN_8
#define BL_ENABLE_GPIO_Port	GPIOA

#define PA7_Pin			GPIO_PIN_7
#define PA7_GPIO_Port	GPIOA

#define PANEL_EN_Pin			GPIO_PIN_11
#define PANEL_EN_GPIO_Port	GPIOB

#define BL_PWM_Pin			GPIO_PIN_10
#define BL_PWM_GPIO_Port	GPIOB

#define LCD_PWR_Pin			GPIO_PIN_2
#define LCD_PWR_GPIO_Port	GPIOC

#define TTLI1_RES_Pin			GPIO_PIN_13
#define TTLI1_RES_GPIO_Port	GPIOC

//Power 
#define PCON3V3_TP_Pin			GPIO_PIN_6
#define PCON3V3_TP_GPIO_Port	GPIOA

#define PCON1V2_TP_Pin			GPIO_PIN_5
#define PCON1V2_TP_GPIO_Port	GPIOA

//VS4210
#define VS4210_RST_Pin			GPIO_PIN_4
#define VS4210_RST_GPIO_Port	GPIOA

//light sensor
#define DIMMER_Pin			GPIO_PIN_3
#define DIMMER_GPIO_Port		GPIOC

#define EXTERNOSDNUMBER (4+23+1+9/*20*/)

//STM i2c
#define STM_SDA_Pin			GPIO_PIN_10
#define STM_SDA_Port			GPIOC

#define STM_SCL_Pin			GPIO_PIN_11
#define STM_SCL_Port			GPIOC

//touch panel
#define TOUCH_INT			GPIO_PIN_1
#define TOUCH_INT_Port		GPIOB
#define TOUCH_RESET			GPIO_PIN_0
#define TOUCH_RESET_Port		GPIOB

//Camera R and D Switch
#define INPUT_SEL1			GPIO_PIN_0   ///CAM D
#define INPUT_SEL1_Port		GPIOC
#define INPUT_SEL2			GPIO_PIN_1   ///CAM R
#define INPUT_SEL2_Port		GPIOC          


//Panel
#define BL_ENABLE(x)      	HAL_GPIO_WritePin(GPIOA, BL_ENABLE_Pin , (x))
#define PANEL_EN(x)      	HAL_GPIO_WritePin(GPIOB, PANEL_EN_Pin , (x^1))
//#define LCD_PWR(x)      	HAL_GPIO_WritePin(GPIOC, LCD_PWR_Pin , (x^1))
#define LCD_PWR(x)      	HAL_GPIO_WritePin(GPIOC, LCD_PWR_Pin , (x))
#define TTLI1_RES(x)      	HAL_GPIO_WritePin(GPIOC, TTLI1_RES_Pin , (x^1))
#define BL_PWM(x)      		HAL_GPIO_WritePin(BL_PWM_GPIO_Port, BL_PWM_Pin , (x))

//power
#define PCON3V3_TP(x)      	HAL_GPIO_WritePin(GPIOA, PCON3V3_TP_Pin , (x^1))
#define PCON1V2_TP(x)      	HAL_GPIO_WritePin(GPIOA, PCON1V2_TP_Pin , (x^1))

//VS4210
#define VS4210_RST(x)      	HAL_GPIO_WritePin(GPIOA, VS4210_RST_Pin , (x^1))

//Audio sel
#define AUD_SEL1(x)      	HAL_GPIO_WritePin(GPIOD, AUD_SEL1_Pin , (x))
#define AUD_SEL2(x)      	HAL_GPIO_WritePin(GPIOC, AUD_SEL2_Pin , (x))
#define AUDIO_MUTE(x)      	HAL_GPIO_WritePin(GPIOC, AUD_SEL2_Pin , (x))

//#define _ReadKey()  		( ((~P4>>2)& 0x01) | (~P1 & 0xfe) )
#define _ReadKey()  		( ((~GPIOC->IDR)& (GPIO_PIN_7|GPIO_PIN_6)) | (((~GPIOB->IDR) & (GPIO_PIN_13|GPIO_PIN_12))>>8) )

//touch panel configure
#define SET_TOUCH_RESET(x)     	HAL_GPIO_WritePin(GPIOB, TOUCH_RESET , (x))


//pwm max
#define _PWMLevelMax 	4000

//camera R and D switch
#define CAM_D_SW(x)      	HAL_GPIO_WritePin(INPUT_SEL2_Port, INPUT_SEL2 , (x))
#define CAM_R_SW(x)      	HAL_GPIO_WritePin(INPUT_SEL1_Port, INPUT_SEL1 , (x))


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
