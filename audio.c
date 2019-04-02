/*************************************************************************
 *
 *
 *    File name   : audio.c
 *    Description : audio function
 *                  this modules are wraped whith st's library
 *                  need to test!!!!!
 *    $Revision: 39 $
 **************************************************************************/
#include "includes.h"
#include "can.h"
#include "audio.h"
   
   
volatile AudioChannelSample16b_t * SineData;
volatile AudioSample16b_t * RecData;
volatile PlaybackState_t playState;
volatile uint32_t recPtr, playPtr;
volatile Boolean allowPlay;

/*************************************************************************
 * Function Name: GenerateSineWave
 * Parameters: none
 *
 * Return: none
 *
 * Description: Generates 1 kHz sine wave array
 *
 *************************************************************************/
void GenerateSineWave(void)
{
  for (uint32_t i = 0; i<SINE_BUFFER_SIZE; i++) *(SineData+i) = (AudioChannelSample16b_t)(sin(10 * (2 * PI) * i / SINE_BUFFER_SIZE) * 28000);
}

/*************************************************************************
 * Function Name: GetSineSample
 * Parameters: none
 *
 * Return: AudioChannelSample16b_t
 *
 * Description: Return the next audio sample from an array containing
 *              1 kHz sine wave
 *
 *************************************************************************/
AudioChannelSample16b_t GetSineSample(void){
  static uint32_t sinePtr=0;
  static AudioChannelSample16b_t sample;
  sample  = *(SineData + sinePtr);
  if (!SPI_I2S_GetFlagStatus(SPI2, I2S_FLAG_CHSIDE))
  {
    /* If both channels sended */
    if (++sinePtr >= SINE_BUFFER_SIZE) sinePtr = 0;
  }
  return sample;
}

/*************************************************************************
 * Function Name: GetRecSample
 * Parameters: none
 *
 * Return: AudioChannelSample16b_t
 *
 * Description: Return the next audio sample from the recording array
 *
 **************************************************************************/

AudioChannelSample16b_t GetRecSample(void){
  AudioChannelSample16b_t sample;
  if (SPI_I2S_GetFlagStatus(SPI2, I2S_FLAG_CHSIDE))
  {
    /* Right Channel */
    sample = (RecData + playPtr)->rightChannel;
  }
  else
  {
    /* Left Channel */
    sample = (RecData + playPtr)->leftChannel;
    /* Both channels fetched, increment pointer */
    if (++playPtr >= REC_BUFFER_SIZE) playPtr = 0;
  }
  return sample;
}

/*************************************************************************
 * Function Name: SaveRecSample
 * Parameters: AudioChannelSample16b_t sample
 *
 * Return: uint32_t - pointer in the used circular buffer
 *
 * Description: Save an audio sample to the recording array
 *
 *************************************************************************/
uint32_t SaveRecSample(AudioChannelSample16b_t sample){
  if (SPI_I2S_GetFlagStatus(I2S2ext, I2S_FLAG_CHSIDE))
  {
    /* Save Right Channel */
    (RecData + recPtr)->rightChannel = sample;
  }
  else
  {
    /* Save Left Channel */
    (RecData + recPtr)->leftChannel = sample;
    if (++recPtr >= REC_BUFFER_SIZE) recPtr = 0;
  }
  return recPtr;
}

static int pat = 0; 

void Save_Audio_Rx( Int16U *  Audio_Rx_Buf ,  Int8U *I2S_int_cnt)	
{
	if (SPI_I2S_GetFlagStatus(I2S2ext, I2S_FLAG_CHSIDE))
		SPI_I2S_ReceiveData(I2S2ext);
  	else
  	{
    /* Save Left Channel */
          Audio_Rx_Buf[*I2S_int_cnt] = SPI_I2S_ReceiveData(I2S2ext);    //need to check!!!!!!!!!!!
#if 0
          if(pat % 3 == 0)
          {
            Audio_Rx_Buf[*I2S_int_cnt] = 0x123;
          }
          else if(pat %3 == 1)
          {
            Audio_Rx_Buf[*I2S_int_cnt] = 0x4567;
          }
          else
          {
            Audio_Rx_Buf[*I2S_int_cnt] = 0x89AB;
          }
           pat = pat + 1; 
#endif
          *I2S_int_cnt = *I2S_int_cnt + 1; 
  	}
		
}


static Int16U Audio_Rx_Buf[4];
static Int8U I2S_int_cnt = 0; 
CanTxMsg 	TxMessage; 
static Int16U Audio_Tx_buf[30];
static Int16U Audio_buf_cnt = 0;
static Int32U A_Rx_Cnt = 0;
static Int32U A_Tx_Cnt = 0;
uint32_t SetTxID = 0x7FF;

/*************************************************************************
 * Function Name: I2S2Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: I2S2 IRQ Handler
 *
 *************************************************************************/
void I2S2Handler(void)
{
  /* Handle TX Buffer empty interrupt */
  if (SET == SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
  {
    if(stGenerateSine == playState)
    {
      SPI_I2S_SendData(SPI2,GetSineSample());
    }
    else if((stLoopbackMic == playState) || (stLoopbackLineIn == playState))
    {
       A_Tx_Cnt = A_Tx_Cnt + 1;
      /* If playback is allowed, fetch the next audio sample, otherwise send 0 */
      //SPI_I2S_SendData(SPI2,(allowPlay)?GetRecSample():0);
      if(allowPlay)
      {
        Int16U x; 
        x = GetRecSample();
        SPI_I2S_SendData(SPI2,x);
		Audio_Tx_buf[Audio_buf_cnt] = x; 
		Audio_buf_cnt = Audio_buf_cnt + 1; 
		if(Audio_buf_cnt == 30)
			Audio_buf_cnt = 0; 
      }
      else
      	{
          Int16U x; 
      		x = 0;
        	SPI_I2S_SendData(SPI2, x);
			Audio_Tx_buf[Audio_buf_cnt] = x; 
			Audio_buf_cnt = Audio_buf_cnt + 1; 
			if(Audio_buf_cnt == 30)
				Audio_buf_cnt = 0; 
      	}	
      
        
    }
  }
  /* Handle RX Buffer not empty interrupt */
  if (SET == SPI_I2S_GetFlagStatus(I2S2ext, SPI_I2S_FLAG_RXNE))
  {
    if((stLoopbackMic == playState) || (stLoopbackLineIn == playState))
    {
        A_Rx_Cnt = A_Rx_Cnt + 1;        
	      Save_Audio_Rx(Audio_Rx_Buf, &I2S_int_cnt);		
        if (I2S_int_cnt >= 4) 
        {
				  char i;
          uint8_t *pAudio_Rx_Buf;
			  	//Int16U *pTxMessage;
			  	I2S_int_cnt = 0;
          pAudio_Rx_Buf = (uint8_t *)Audio_Rx_Buf;
				  //pTxMessage = (Int16U *)&TxMessage.Data[0];
				  for(i=0; i<8; ++i)
          {
            //TxMessage.Data[i] = Audio_Rx_Buf[i];
            TxMessage.Data[i] = pAudio_Rx_Buf[i];
            //TxMessage.Data[i] = i;
          }
				  TxMessage.StdId = SetTxID;
  			  TxMessage.RTR = CAN_RTR_DATA;
  			  TxMessage.IDE = CAN_ID_STD;
  			  TxMessage.DLC = 8;		
          if(SetTxID != 0x7FF)
            CAN_Transmit(CANx, &TxMessage);
       }			
      /* Save the received sample. If enough samples are saved, allow the playback */
      if (recPtr > REC_BUFFER_ALLOW_PLAY) allowPlay = TRUE;
    }
    else
    {
      /* Dummy Read of input buffer to clear the interrupt flag */
      SPI_I2S_ReceiveData(I2S2ext);
    }
  }
}

/*************************************************************************
 * Function Name: InitPlaybackState
 * Parameters: PlaybackState_t state
 *
 * Return: none
 *
 * Description: Initialize the playback state
 *
 *************************************************************************/
void InitPlaybackState(PlaybackState_t state)
{
  switch(state)
  {
    case stGenerateSine:
      WM8731_Mic_Mute(TRUE);
      WM8731_LineIn_Mute(TRUE);
      break;

    case stLoopbackMic:
      WM8731_Mic_Mute(FALSE);
      WM8731_LineIn_Mute(TRUE);
      WM8731_InputSelectMic();
      allowPlay = 0;
      playPtr = 0;
      recPtr = 0;
      break;

    case stLoopbackLineIn:
      WM8731_Mic_Mute(TRUE);
      WM8731_LineIn_Mute(FALSE);
      WM8731_LineIn_Volume(WM8731_RLIN_RINVOL_Max);
      WM8731_InputSelectLineIn();
      allowPlay = 0;
      playPtr = 0;
      recPtr = 0;
      break;


    default:break;
  }
}

void AudioInit(void)
{
	uint8_t volume;

	/*PLLI2S configure*/
	RCC_PLLI2SConfig(256, 4, 5);
	/*Enable PLLI2S*/
	RCC_PLLI2SCmd(ENABLE);
	/*Wait PLLI2S Lock*/
	while (RESET == RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY));
	/*PLLI2S is I2S clock source*/
	RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
	/* Init I2S2 peripheral */
	STM_I2S2Init();

	/* Reserve space for playback buffers */
	SineData = malloc(SINE_BUFFER_SIZE * sizeof(AudioChannelSample16b_t));
	RecData = malloc(REC_BUFFER_SIZE * sizeof(AudioSample16b_t));
	if ((SineData == NULL) || (RecData == NULL))
	{
		while (1);
	}

	/* Disable I2S interrupts until everything is configured */
	NVIC_DisableIRQ(SPI2_IRQn);
	NVIC_SetPriority(SPI2_IRQn, 15);


	/* Audio Codec init */
	I2C2_Init();
	if (WM8731_RESULT_OK != WM8731_Init())
	{
		/* Codec error */
		while (1);
	}

	/* Generate Sine Wave Array */
	GenerateSineWave();

	/* Set default volume */
	volume = 5;
	WM8731_Headphone_Volume(WM8731_VOL_CONV(volume));

	/* Set default state*/
	playState = stLoopbackMic;
	allowPlay = 0;
	playPtr = 0;
	recPtr = 0;

	/* Init default state*/
	InitPlaybackState(playState);

	/* Enable the I2S2 Interrupts */
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
	SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(SPI2_IRQn);
}