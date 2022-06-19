#include "PDMhardware.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim12;

volatile int16_t            ADC1_IN_Buffer[ADC_FRAME_SIZE*ADC1_CHANNELS] = { 0U };   //ADC1 input data buffer
volatile int16_t            ADC2_IN_Buffer[ADC_FRAME_SIZE*ADC2_CHANNELS] = { 0U };   //ADC2 input data buffer
volatile int16_t            ADC3_IN_Buffer[ADC_FRAME_SIZE*ADC3_CHANNELS] = { 0U };   //ADC3 input data buffer
static float CurLimits[20] ={};
static uint16_t muRawCurData[20];
static uint16_t muRawVData[4];
static float mfCurData[20] ={};
static float mfVData[4] ={};
static float Power[20] ={20,20,20,20,20,20,20,20,8,8,8,8,8,8,8,8,8,8,8,8};
static KAL_DATA CurSensData[20][5]={
										{{0U,0U},{7410U,41U},{6749U,1104U},{6570U,1888U},{6420U,3865U}},
										{{0U,0U},{7410U,41U},{6749U,1104U},{6570U,1888U},{6420U,3865U}},
										{{0U,0U},{7410U,41U},{6749U,1104U},{6570U,1888U},{6420U,3865U}},
										{{0U,0U},{7410U,41U},{6749U,1104U},{6570U,1888U},{6420U,3865U}},
										{{0U,0U},{7410U,41U},{6749U,1104U},{6570U,1888U},{6420U,3865U}},
										{{0U,0U},{7410U,41U},{6749U,1104U},{6570U,1888U},{6420U,3865U}},
										{{0U,0U},{7410U,41U},{6749U,1104U},{6570U,1888U},{6420U,3865U}},
										{{0U,0U},{7410U,41U},{6749U,1104U},{6570U,1888U},{6420U,3865U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
										{{3350U,18U},{3150U,196U},{3100U,800U},{3000U,1240U},{2850U,4344U}},
};
static LIN_COOF CSC[20][4] = {
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
		{{41U,0U,0U},{1104U,0U,0U},{1888U,0U,0U},{3865U,0U}},
};




static   EventGroupHandle_t xADCEvent;
static   StaticEventGroup_t xADCCreatedEventGroup;
void vDataConvertToFloat(void);
void vHWGetTimer(OUT_NAME_TYPE out_name, uint32_t * channel,  TIM_HandleTypeDef *ptim);
void vGetAverDataFromRAW(uint16_t * Indata, uint16_t *OutData, uint16_t InIndex, uint16_t OutIndex, uint8_t Size,uint16_t FrameSize, uint16_t BufferSize);
void vHWOutInit();


void vHWOutConfig(OUT_NAME_TYPE out_name, uint16_t power, float max_cur)
{
	uint32_t channel = 0;
	TIM_HandleTypeDef *ptim = NULL;
    TIM_OC_InitTypeDef sConfigOC = {0};
	CurLimits[out_name] = max_cur;
	Power[out_name] =  power;
	vHWGetTimer(out_name, &channel,ptim);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = (uint32_t)((float)(power/100) * ptim->Init.Period);
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(ptim, &sConfigOC, channel) != HAL_OK)
	{
	    Error_Handler();
	}
}

void vHWOutSetState( OUT_NAME_TYPE out_name,  OUT_STATE_TYPE out_state)
{
   uint32_t channel = 0;
   TIM_HandleTypeDef *ptim = NULL;
   //В зависимости от номера выхода выибраетм таймер и канал таймера PWM
   vHWGetTimer(out_name, &channel,ptim);
   switch (out_state)
   {
   	   case OUT_ON:
   		  HAL_TIM_PWM_Start(ptim,channel);
	   	  break;
   	   case OUT_OFF:
   		  HAL_TIM_PWM_Stop(ptim,channel);
   	   	  break;
   }
}




void vHWOutInit()
{
	//Инициализация портов упраления ключами
	HAL_GPIO_WritePin(GPIOG, Cs_Dis20_5_Pin|Cs_Dis20_2_Pin|Cs_Dis20_1_Pin|Cs_Dis8_13_14_Pin
	                          |Cs_Dis8_17_18_Pin|Cs_Dis8_15_16_Pin|Cs_Dis20_3_Pin|Cs_Dis20_4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, Cs_Dis8_11_12_Pin|Cs_Dis20_7_Pin|Cs_Dis8_19_20_Pin|Cs_Dis20_8_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Cs_Dis20_6_GPIO_Port, Cs_Dis20_6_Pin, GPIO_PIN_SET);
	vHWOutConfig(OUT_1, 100, 20.0);
	vHWOutConfig(OUT_2, 100, 20.0);
	vHWOutConfig(OUT_3, 100, 20.0);
	vHWOutConfig(OUT_4, 100, 20.0);
	vHWOutConfig(OUT_5, 100, 20.0);
	vHWOutConfig(OUT_6, 100, 20.0);
	vHWOutConfig(OUT_7, 100, 20.0);
	vHWOutConfig(OUT_8, 100, 20.0);
	vHWOutConfig(OUT_9, 100, 8.0);
	vHWOutConfig(OUT_10, 100, 8.0);
	vHWOutConfig(OUT_11, 100, 8.0);
	vHWOutConfig(OUT_12, 100, 8.0);
	vHWOutConfig(OUT_13, 100, 8.0);
	vHWOutConfig(OUT_14, 100, 8.0);
	vHWOutConfig(OUT_15, 100, 8.0);
	vHWOutConfig(OUT_16, 100, 8.0);
	vHWOutConfig(OUT_17, 100, 8.0);
	vHWOutConfig(OUT_18, 100, 8.0);
	vHWOutConfig(OUT_19, 100, 8.0);
	vHWOutConfig(OUT_20, 100, 8.0);
	vHWOutSetState(OUT_1,OUT_ON);
	vHWOutSetState(OUT_2,OUT_ON);
	vHWOutSetState(OUT_3,OUT_ON);
	vHWOutSetState(OUT_4,OUT_ON);
	vHWOutSetState(OUT_5,OUT_ON);
	vHWOutSetState(OUT_6,OUT_ON);
	vHWOutSetState(OUT_7,OUT_ON);
	vHWOutSetState(OUT_8,OUT_ON);
	vHWOutSetState(OUT_9,OUT_ON);
	vHWOutSetState(OUT_10,OUT_ON);
	vHWOutSetState(OUT_11,OUT_ON);
	vHWOutSetState(OUT_12,OUT_ON);
	vHWOutSetState(OUT_13,OUT_ON);
	vHWOutSetState(OUT_14,OUT_ON);
	vHWOutSetState(OUT_15,OUT_ON);
	vHWOutSetState(OUT_16,OUT_ON);
	vHWOutSetState(OUT_17,OUT_ON);
	vHWOutSetState(OUT_18,OUT_ON);
	vHWOutSetState(OUT_19,OUT_ON);
	vHWOutSetState(OUT_20,OUT_ON);

}


void vADCInit ( void )
{
//  HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port, ANALOG_SWITCH_Pin, GPIO_PIN_RESET );
//  HAL_GPIO_WritePin( DIN_OFFSET_GPIO_Port,    DIN_OFFSET_Pin,    GPIO_PIN_SET   );
//  vADC3DCInit( DC );
//  vADC3FrInit( ADC3Freq );
//  vADC12FrInit( ADC2Freq );
//  hadc3.DMA_Handle->XferCpltCallback     = ADC_DMAConv;
//  hadc2.DMA_Handle->XferCpltCallback     = ADC_DMAConv;
 // hadc1.DMA_Handle->XferCpltCallback     = ADC_DMAConv;
//  hadc3.DMA_Handle->XferHalfCpltCallback = NULL;
//  hadc2.DMA_Handle->XferHalfCpltCallback = NULL;
 // hadc1.DMA_Handle->XferHalfCpltCallback = NULL;
//  hadc3.DMA_Handle->XferErrorCallback    = ADC_DMAErro;
 // hadc2.DMA_Handle->XferErrorCallback    = ADC_DMAErro;
//  hadc1.DMA_Handle->XferErrorCallback    = ADC_DMAErro;
 // ADC_VALID_DATA = 0U;
 // vADCConfigInit();
  return;
}

void vADCTask(void * argument)
{
  /* USER CODE BEGIN vADCTask */
  xADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );
  vSetCalibrateData();
  vHWOutInit();
  /* Infinite loop */
  for(;;)
  {
	HAL_ADC_Start_DMA( &hadc1,( uint32_t* )&ADC1_IN_Buffer, ( ADC_FRAME_SIZE * ADC1_CHANNELS ));
	HAL_ADC_Start_DMA( &hadc2,( uint32_t* )&ADC2_IN_Buffer, ( ADC_FRAME_SIZE * ADC2_CHANNELS ));
	HAL_ADC_Start_DMA( &hadc3,( uint32_t* )&ADC3_IN_Buffer, ( ADC_FRAME_SIZE * ADC3_CHANNELS ));
	xEventGroupWaitBits( xADCEvent, ( ADC3_READY | ADC2_READY | ADC1_READY ), pdTRUE, pdTRUE, portMAX_DELAY );
	HAL_ADC_Stop_DMA(&hadc1);
	HAL_ADC_Stop_DMA(&hadc2);
	HAL_ADC_Stop_DMA(&hadc3);
	vDataConvertToFloat();
  }
  /* USER CODE END vADCTask */
}

/*
 * Функция вытаскивает из входного буфера Indata  (размером FrameSize*BufferSize) со смещением InIndex FrameSize отсчетов,
 * счетает среднее арефмитическое и записывает в буффер OutData со смещением OutIndex
 */
void vGetAverDataFromRAW(uint16_t * InData, uint16_t *OutData, uint16_t InIndex, uint16_t OutIndex, uint8_t Size,uint16_t FrameSize, uint16_t BufferSize)
{
	uint32_t temp;
	for (uint8_t i=0;i<Size;i++)
	{
		for (uint8_t j=0;j<FrameSize;j++)
		{
		  temp = temp + InData[InIndex+i+j*BufferSize];
		}
		OutData[OutIndex+i] = temp / FrameSize;
	}
}
/*
 * Функция расчитывет коофиценты функции исходя их калибровочных данных
 */
void vSetCalibrateData(void )
{
	uint8_t i,j;
	for (i=0;i < 20U; i++)
	{
		for (j=0; j< 4U; j++)
		{
			//Проверяем что хоты одно значение АЦП не равно нулю,что-то не словить делением на ноль.
			if ((CurSensData[i][j].Data !=0) || (CurSensData[i][j+1].Data !=0 ))
			{
				CSC[i][j].k = ( CurSensData[i][j].KOOF -  CurSensData[i][j+1].KOOF) / ( CurSensData[i][j].Data- CurSensData[i][j+1].Data);
				CSC[i][j].b = ( CurSensData[i][j].Data * CurSensData[i][j+1].KOOF - CurSensData[i][j+1].Data * CurSensData[i][j].KOOF)/(CurSensData[i][j].Data + CurSensData[i][j+1].Data);
			}
		}
	}

}


/*
 *  Функция усредняет данные из буфеера АЦП, и пробразует их значения
 */
void vDataConvertToFloat( void)
{
	uint8_t i;
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 7-8
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawCurData, 0U, 6U, 2U ,ADC_FRAME_SIZE,ADC1_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 19-20
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawCurData, 2U, 18U, 2U ,ADC_FRAME_SIZE,ADC1_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов AIN
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawVData, 4U, 0U, 4U ,ADC_FRAME_SIZE,ADC1_CHANNELS);
	 // Полчени из буфера ADC 2 данныех каналов каналов тока 4-6
	 vGetAverDataFromRAW((uint16_t *)&ADC2_IN_Buffer, (uint16_t *)&muRawCurData,0U, 3U, 3U ,ADC_FRAME_SIZE,ADC2_CHANNELS);
	 // Полчени из буфера ADC 2 данныех каналов каналов тока 9-12
	 vGetAverDataFromRAW((uint16_t *)&ADC2_IN_Buffer, (uint16_t *)&muRawCurData, 3U, 8U, 4U ,ADC_FRAME_SIZE,ADC2_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 1-3
	 vGetAverDataFromRAW((uint16_t *)&ADC3_IN_Buffer, (uint16_t *)&muRawCurData, 0U, 0U, 3U ,ADC_FRAME_SIZE,ADC3_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 13-18
	 vGetAverDataFromRAW((uint16_t *)&ADC3_IN_Buffer, (uint16_t *)&muRawCurData, 3U, 12U, 5U ,ADC_FRAME_SIZE,ADC1_CHANNELS);
	 //Преобразование во флоат данных AIN
	 for ( i = 0; i < 4U; i++ )
	 {
		 mfCurData[ i ] = muRawVData[ i ] * COOF;
	 }
	 //Преобразвоание во float данных тока каналов 1-20
	 for ( i =0; i < 20U ; i++)
	{

		 for (uint8_t r = 0; r < 5U; r++)
		 {
			 if ( muRawCurData[ i ] < CSC[i][r].data )
			 {
				 mfCurData[ i ] = ( (float) muRawCurData [ i ] * CSC[i][r].k  + CSC[i][r].b ) *K /RR;
				 break;
			 }
		 }
	}
}
/*
 * Функция анализирует преобразованные значекни
 *
 */
void vDataAnalize( void )
{
	uint8_t i;
	for ( i = 0; i < 20U ; i ++)
	{
		if ()

		mfCurData[ i ]

	}

}


void vADC_Init()
{


}

 void vADC_Ready ( uint8_t adc_number )
 {
   static portBASE_TYPE xHigherPriorityTaskWoken;

   xHigherPriorityTaskWoken = pdFALSE;
   switch ( adc_number )
   {
     case ADC3_READY:
     //  __HAL_TIM_DISABLE(&htim3);
       xEventGroupSetBitsFromISR( xADCEvent, ADC3_READY, &xHigherPriorityTaskWoken );
       break;
     case ADC2_READY:
       xEventGroupSetBitsFromISR( xADCEvent, ADC2_READY, &xHigherPriorityTaskWoken );
       break;
     case ADC1_READY:
       xEventGroupSetBitsFromISR( xADCEvent, ADC1_READY, &xHigherPriorityTaskWoken );
       break;
     default:
       break;
   }
   portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
   return;
 }
/***************** Service function ***********************************************/

 /*
  * Функция возращает номер канала и управлюящий таймер для выбранного выхода
  */

 void vHWGetTimer(OUT_NAME_TYPE out_name, uint32_t * channel,  TIM_HandleTypeDef *ptim)
 {
 	switch (out_name)
 	   {
 	       case OUT_1:
 	    	    ptim = &htim4;
 	    	    *channel = TIM_CHANNEL_3;
 	    	    break;
 	       case OUT_2:
 	    	    ptim = &htim4;
 	    	    *channel = TIM_CHANNEL_4;
 	            break;
 	       case OUT_3:
 	    	    ptim = &htim2;
 	    	    *channel = TIM_CHANNEL_1;
 	            break;
 	       case OUT_4:
 	    	    ptim = &htim3;
 	    	    *channel = TIM_CHANNEL_2;
 	            break;
 	       case OUT_5:
 	    	    ptim = &htim1;
 	    	    *channel = TIM_CHANNEL_3;
 	           	break;
 	       case OUT_6:
 	    	    ptim = &htim1;
 	    	    *channel = TIM_CHANNEL_4;
 	            break;
 	       case OUT_7:
 	    	    ptim = &htim12;
 	    	    *channel = TIM_CHANNEL_1;
 	            break;
 	       case OUT_8:
 	    	    ptim = &htim4;
 	    	    *channel = TIM_CHANNEL_2;
 	            break;
 	       case OUT_9:
 	     	    ptim = &htim1;
 	     	    *channel = TIM_CHANNEL_1;
 	     	    break;
 	       case OUT_10:
 	    	    ptim = &htim1;
 	    	    *channel = TIM_CHANNEL_1;
 	       	    break;
 	       case OUT_11:
 		        ptim = &htim1;
 		       	*channel = TIM_CHANNEL_2;
 	            break;
 	       case OUT_12:
 	    	    ptim = &htim2;
 	    	   	*channel = TIM_CHANNEL_3;
 	            break;
 	       case OUT_13:
 	    	    ptim = &htim8;
 	    	    *channel = TIM_CHANNEL_1;
 	            break;
 	       case OUT_14:
 	    	     ptim = &htim8;
 	    	     *channel = TIM_CHANNEL_2;
 	    	     break;
 	       case OUT_15:
 	    	     ptim = &htim3;
 	    	     *channel = TIM_CHANNEL_1;
 	    	     break;
 	       case OUT_16:
 	    	     ptim = &htim2;
 	    	     *channel = TIM_CHANNEL_2;
 	    	     break;
 	       case OUT_17:
 	    	     ptim = &htim8;
 	    	     *channel = TIM_CHANNEL_4;
 	             break;
 	       case OUT_18:
 	    	     ptim = &htim8;
 	    	     *channel = TIM_CHANNEL_4;
 	             break;
 	       case OUT_19:
 	    	     ptim = &htim12;
 	    	     *channel = TIM_CHANNEL_2;
 	             break;
 	       case OUT_20:
 	    	     ptim = &htim4;
 	    	     *channel = TIM_CHANNEL_1;
 	             break;
 	   }


 }

