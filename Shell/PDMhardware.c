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
static float Power[20] ={100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};
static CUR_SENS_COOF CurSensCoof[20]={{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},
									  {3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},
									  {3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850},{3350,3100,3000,2850}};

static   EventGroupHandle_t xADCEvent;
static   StaticEventGroup_t xADCCreatedEventGroup;

void vHWGetTimer(OUT_NAME_TYPE out_name, uint32_t * channel,  TIM_HandleTypeDef *ptim);
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



  }
  /* USER CODE END vADCTask */
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

