/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2013
 *
 *    File name   : main.c
 *    Description : Main module
 *
 *    History :
 *    1. Date        : October, 2013
 *       Author      : Atanas Uzunov
 *       Description : Create
 *
 * DESCRIPTION
 * ===========
 *  This example project shows how to use the IAR Embedded Workbench
 * for ARM to develop code for the IAR-STM32F429II-SK board. It shows basic
 * use of the parallel I/O, the timer, the interrupt controller, the LCD
 * controller and the I2S(SPI) module.
 *  The I2S module is connected to external Wolfson WM8731 audio codec, and is
 * configured for 44.1kHz / Stereo mode/playback. User can choose from 4
 * possible inputs (tone generator, microphone, line-in, and wav file playback)
 * Also it is possible to set up/down/mute the volume.
 *
 *  The background file and the audio file are combined into the
 * "res.bin" file with the command:
 *
 *  copy /b iar_globe_logo.png+iar_disco_2b_mixdown_stereo.wav res.bin
 *
 * The PNG decoding is based on LodePNG decoder - http://lodev.org/lodepng/
 *
 *  The music piece used in this example is composed and recorded by some
 * developers at IAR Systems.
 *
 * COMPATIBILITY
 * =============
 *  The audio demo example project is compatible with IAR-STM32F429II-SK
 * evaluation board. By default, the project is configured to use the
 * J-Link JTAG/SWD interface.
 *
 * CONFIGURATION
 * =============
 * Flash Debug - The Progam is loaded to internal Flash.
 *
 *  Make sure that the following jumpers are correctly configured on the
 * IAR-STM32F429II-SK evaluation board:
 *
 *  Jumpers:
 *   PWR_SEL - depending of power source
 *   B0_1/B0_0 - B0_0
 *   B1_1/B1_0 - B1_0
 *
 *
 *    $Revision: 3190 $
 **************************************************************************/
#include "includes.h"
#include "can.h"
#include "display.h"
#include "audio.h"

#define TICK_PER_SEC          30

#define TICK_DLY              5000 /* tick at every 10ms */

#define TICK_DLY_10MS         100

static volatile uint32_t TimingDelay;

/* variable for critical section entry control */
uint32_t CriticalSecCntr;

uint32_t intrTickTimer;
volatile Boolean flTick;
volatile Boolean flTick10ms;
volatile uint32_t TickCounter = 1;

uint16_t MyID;
extern uint32_t SetTxID;

/*************************************************************************
 * Function Name: DelayResolution100us
 * Parameters: uint32_t Dly
 *
 * Return: none
 *
 * Description: Delay ~ (arg * 100us)
 *
 *************************************************************************/
void DelayResolution100us(uint32_t Dly)
{
  TimingDelay = Dly;

  while(TimingDelay != 0);
}

/*************************************************************************
 * Function Name: SysTickHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: SysTick Handler function @ every 100us
 *
 *************************************************************************/
void SysTickHandler(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
  if (++intrTickTimer >= TICK_DLY_10MS * TickCounter)
  {
    flTick10ms = TRUE;
    TickCounter++;
  }
  if (++intrTickTimer >= TICK_DLY)
  {
    flTick = TRUE;
    intrTickTimer = 0;
    TickCounter = 1;
  }
}

Int32U Cnt_buf = 0;


/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/
int main(void)
{
uint32_t CurID = 0x7FF;
uint16_t Curstat = 0;
uint16_t xr, yr;
uint16_t sxr, syr;
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */

  ENTR_CRT_SECTION();

  /* Enable GPIOs clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                         RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC |
                         RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |
                         RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG |
                         RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);

  /* SysTick Config */
  if(SysTick_Config(SystemCoreClock/10000))
  {
    /* Capture error */
    while (1);
  }

  EXT_CRT_SECTION();
  
  
  InitDisplay();

  CanInit();
  PrintMessage(100, 400, "                                        ");
  
  AudioInit();
  PrintMessage(100, 400, "                                                ");
  
  __enable_interrupt();

  ChangeImg(RES_BACK_IMG2);
  
  while(1)
  {
    CanRxMsg RxMsg;
    
    
    if((CAN_MessagePending(CANx, CAN_FIFO0) > 0))
      CAN_Receive(CANx ,CAN_FIFO0, &RxMsg);
    if(RxMsg.StdId == CurID && CurID != 0x7FF)
    {
      Save_CAN_Rx(&RxMsg);
      Cnt_buf = Cnt_buf + 1;
    }
    
    //display and touch control
    if(flTick10ms == FALSE)
      continue;
    flTick10ms = FALSE;
    GetTouchLocation(&xr, &yr);
    if(flTick == FALSE)
      continue;
    flTick = FALSE;
    if(Curstat == 0)
    {
      if(xr == sxr && yr == syr)
        continue;
      if(yr >= 232 && yr <= 301)
      {
        if(xr >= 577 && xr <= 740)
        {
          CurID = 3;
          sxr = xr;
          syr = yr;
          ChangeImg(RES_BACK_IMG3);
          Curstat = 1;
        }
        else if(xr >= 336 && xr <= 505)
        {
          CurID = 2;
          sxr = xr;
          syr = yr;
          ChangeImg(RES_BACK_IMG3);
          Curstat = 1;
        }
        else if(xr >= 87 && xr <= 245)
        {
          CurID = 1;
          sxr = xr;
          syr = yr;
          ChangeImg(RES_BACK_IMG3);
          Curstat = 1;
        }
      }
    }
    if(Curstat == 1)
    {
      if(xr == sxr && yr == syr)
        continue;
      if(yr >= 232 && yr <= 301)
      {
        if(xr >= 577 && xr <= 740)
        {
          SetTxID = 3;
          sxr = xr;
          syr = yr;
          ChangeImg(RES_BACK_IMG4);
          Curstat = 2;
        }
        else if(xr >= 336 && xr <= 505)
        {
          SetTxID = 2;
          sxr = xr;
          syr = yr;
          ChangeImg(RES_BACK_IMG4);
          Curstat = 2;
        }
        else if(xr >= 87 && xr <= 245)
        {
          SetTxID = 1;
          sxr = xr;
          syr = yr;
          ChangeImg(RES_BACK_IMG4);
          Curstat = 2;
        }
      }
      if(yr >= 395 && yr <= 470 && xr >= 669 && xr <= 794)
      {
        sxr = xr;
        syr = yr;
        ChangeImg(RES_BACK_IMG2);
        Curstat = 0;
      }
        
    }
    if(Curstat == 2)
    {
      PrintMessage(30, 300, "FROM : %d", CurID);
      PrintMessage(30, 350, "TO : %d", SetTxID);
      if(xr == sxr || yr == syr)
        continue;
      if(yr >= 395 && yr <= 470 && xr >= 669 && xr <= 794)
      {
        sxr = xr;
        syr = yr;
        ChangeImg(RES_BACK_IMG3);
        Curstat = 1;
      }
    }
  }
}

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

