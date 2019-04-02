/*************************************************************************
 *
 *
 *    File name   : can.h
 *    Description : can function
 *                  this modules are wraped whith st's library
 *                  so you need to access only this function.
 *    $Revision: 39 $
 **************************************************************************/
#include "includes.h"
#include "can.h"
#include "audio.h"

extern AudioChannelSample16b_t * SineData;
extern AudioSample16b_t * RecData;
extern uint32_t recPtr, playPtr;

void CanInit()
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;


	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
	/* CAN GPIOs configuration **************************************************/

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(CAN_GPIO_TX_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(CAN_GPIO_RX_CLK, ENABLE);

	/* Connect CAN pins to AF9 */
	GPIO_PinAFConfig(CAN_GPIO_RX_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
	GPIO_PinAFConfig(CAN_GPIO_TX_PORT, CAN_TX_SOURCE, CAN_AF_PORT);

	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(CAN_GPIO_TX_PORT, &GPIO_InitStructure);
	/* CAN register init */


   /* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(CAN_GPIO_RX_PORT, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	CAN_DeInit(CANx);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

	/* CAN Baudrate = 175kbps (CAN clocked at 42 MHz) */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
	CAN_InitStructure.CAN_Prescaler = 2;
	CAN_Init(CANx, &CAN_InitStructure);

	/* CAN filter init */
#ifdef  USE_CAN1
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
#else /* USE_CAN2 */
	CAN_FilterInitStructure.CAN_FilterNumber = 14;
#endif  /* USE_CAN1 */
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;

	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
__IO uint32_t ret = 0; /* for return of the interrupt handling */
volatile TestStatus TestRx;

uint32_t Save_CAN_Rx(CanRxMsg * RxMsg )
	{

	  char i;
	  Int16U *pRxMsg;
           pRxMsg = (Int16U *)RxMsg->Data; 
    /* Save Left Channel */
	for(i=0; i<4; ++i) 
      {
        (RecData + recPtr)->leftChannel = pRxMsg[i];
        (RecData + recPtr)->rightChannel = pRxMsg[i]; 
        if (++recPtr >= REC_BUFFER_SIZE) recPtr = 0;
      }
  return recPtr;
}
