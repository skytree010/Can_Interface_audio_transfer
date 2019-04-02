/*************************************************************************
 *
 *
 *    File name   : can.h
 *    Description : can function header
 *                  this modules are wraped whith st's library
 *                  so you need to access only this function.
 *    $Revision: 39 $
 **************************************************************************/
#ifndef __CAN_H
#define __CAN_H
#include "includes.h"
#include "stm32f4xx.h"
#include "stm32f4xx_can.h"

 /* #define USE_CAN1*/
#define USE_CAN1 

#ifdef  USE_CAN1
#define CANx                       CAN1
#define CAN_CLK                    RCC_APB1Periph_CAN1
#define CAN_RX_PIN                 GPIO_Pin_9
#define CAN_TX_PIN                 GPIO_Pin_13
#define CAN_GPIO_TX_PORT           GPIOH
#define CAN_GPIO_RX_PORT           GPIOI
#define CAN_GPIO_TX_CLK            RCC_AHB1Periph_GPIOH
#define CAN_GPIO_RX_CLK            RCC_AHB1Periph_GPIOI
#define CAN_AF_PORT                GPIO_AF_CAN1
#define CAN_RX_SOURCE              GPIO_PinSource9
#define CAN_TX_SOURCE              GPIO_PinSource13          
#else /*USE_CAN2*/
#define CANx                       CAN2
#define CAN_CLK                    (RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2)
#define CAN_RX_PIN                 GPIO_Pin_5
#define CAN_TX_PIN                 GPIO_Pin_13
#define CAN_GPIO_PORT              GPIOB
#define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define CAN_AF_PORT                GPIO_AF_CAN2
#define CAN_RX_SOURCE              GPIO_PinSource5
#define CAN_TX_SOURCE              GPIO_PinSource13    
#endif  /* USE_CAN1 */


void CanInit();
uint32_t Save_CAN_Rx(CanRxMsg * RxMsg);
#endif