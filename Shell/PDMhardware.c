#include "PDMhardware.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "system.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim12;

volatile int16_t            ADC1_IN_Buffer[ADC_FRAME_SIZE*ADC1_CHANNELS] = { 0U };   //ADC1 input data buffer
volatile int16_t            ADC2_IN_Buffer[ADC_FRAME_SIZE*ADC2_CHANNELS] = { 0U };   //ADC2 input data buffer
volatile int16_t            ADC3_IN_Buffer[ADC_FRAME_SIZE*ADC3_CHANNELS] = { 0U };   //ADC3 input data buffer

PDM_OUTPUT_TYPE out[OUT_COUNT]  		__SECTION(RAM_SECTION_CCMRAM);

static uint16_t muRawCurData[OUT_COUNT] __SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawVData[AIN_COUNT]   __SECTION(RAM_SECTION_CCMRAM);
static float mfVData[AIN_COUNT] 		__SECTION(RAM_SECTION_CCMRAM);

static KAL_DATA CurSensData[OUT_COUNT][5] ={   {{0U,0.0},{K0025O20,V0025O20},{K06O20,V06O20},{K10O20,V10O20},{K25O20,V25O20}},
										{{0U,0.0}      ,{K0025O20,V0025O20},{K06O20,V06O20},{K10O20,V10O20},{K25O20,V25O20}},
										{{0U,0.0}      ,{K0025O20,V0025O20},{K06O20,V06O20},{K10O20,V10O20},{K25O20,V25O20}},
										{{0U,0.0}      ,{K0025O20,V0025O20},{K06O20,V06O20},{K10O20,V10O20},{K25O20,V25O20}},
										{{0U,0.0}      ,{K0025O20,V0025O20},{K06O20,V06O20},{K10O20,V10O20},{K25O20,V25O20}},
										{{0U,0.0}      ,{K0025O20,V0025O20},{K06O20,V06O20},{K10O20,V10O20},{K25O20,V25O20}},
										{{0U,0.0}      ,{K0025O20,V0025O20},{K06O20,V06O20},{K10O20,V10O20},{K25O20,V25O20}},
										{{0U,0.0}      ,{K0025O20,V0025O20},{K06O20,V06O20},{K10O20,V10O20},{K25O20,V25O20}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										{{K0005O08,V0005O08},{K005O08,V005O08},{K02O08,V02O08},{K03O08,V03O08},{K10O08,V10O08}},
										};


static   EventGroupHandle_t pADCEvent 				__SECTION(RAM_SECTION_CCMRAM);
static   StaticEventGroup_t xADCCreatedEventGroup   __SECTION(RAM_SECTION_CCMRAM);
static   EventGroupHandle_t xOutEvent 				__SECTION(RAM_SECTION_CCMRAM);
static   StaticEventGroup_t xOutCreatedEventGroup	__SECTION(RAM_SECTION_CCMRAM);

/*
 *
 */
void vHWOutInit(OUT_NAME_TYPE out_name, TIM_HandleTypeDef * ptim, uint32_t  uiChannel, GPIO_TypeDef* EnablePort, uint16_t EnablePin,   uint8_t PWM)
{
	volatile uint8_t j;
	if ( out_name < OUT_COUNT )
	{

		out[out_name].ptim 			  = ptim;
		out[out_name].channel 		  = uiChannel;
		out[out_name].GPIOx 		  = EnablePort;
		out[out_name].GPIO_Pin		  = EnablePin;
		out[out_name].error_count     = 0U;
		out[out_name].out_line_state  = 0;
		out[out_name].out_logic_state = OUT_OFF;
		out[out_name].out_state		  =	STATE_OUT_OFF;
		vOutHWDisabale( out_name);
		out[out_name].error_flag  = ERROR_OFF;
		out[out_name].current = 0.0;
		if (out_name < OUT_HPOWER_COUNT)
		{
			vHWOutOverloadConfig(out_name, DEFAULT_HPOWER,DEFAULT_OVERLOAD_TIMER_HPOWER, DEFAULT_HPOWER_MAX);
		}
		else
		{
			vHWOutOverloadConfig(out_name, DEFAULT_LPOWER,DEFAULT_OVERLOAD_TIMER_LPOWER, DEFAULT_LPOWER_MAX);
		}
		vHWOutResetConfig(out_name,DEFAULT_RESET_COUNTER, DEFAULT_RESET_TIMER);
		vOutSetPWM(out_name, DEFAULT_PWM);
		out[out_name].error_flag = ERROR_OFF;
		for (j=0; j< 4U; j++)
		{
			//Проверяем что хоты одно значение АЦП не равно нулю,что-то не словить делением на ноль.
			if ((CurSensData[out_name][j].Data != 0.0D) || (CurSensData[out_name][j+1].Data != 0.0D ))
			{
				float temp;
				float temp1;
				float temp2;
				out[out_name].CSC[j].data = CurSensData[out_name][j+1].Data;
				out[out_name].CSC[j].k = (float)( CurSensData[out_name][j].KOOF -  CurSensData[out_name][j+1].KOOF) /(float) ( CurSensData[out_name][j].Data- CurSensData[out_name][j+1].Data);
				temp =  CurSensData[out_name][j].Data   * CurSensData[out_name][j+1].KOOF;
				temp1 = CurSensData[out_name][j+1].Data * CurSensData[out_name][j].KOOF;
				temp2 = CurSensData[out_name][j+1].Data - CurSensData[out_name][j].Data;
				out[out_name].CSC[j].b = (temp1 - temp)/temp2;
			}
		}
		TIM_OC_InitTypeDef sConfigOC = {0U};
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = (uint32_t)( out[out_name].ptim ->Init.Period *(float)out[out_name].PWM/ MAX_PWM );
		sConfigOC.OCPolarity 	= TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity 	= TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCFastMode 	= TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState	= TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState 	= TIM_OCNIDLESTATE_RESET;
		HAL_TIM_PWM_ConfigChannel(out[out_name].ptim , &sConfigOC, out[out_name].channel) ;
	}
	return;
}
/*
 *
 */
void vOutHWEnbale(OUT_NAME_TYPE out_name)
{
	if (out_name < OUT_COUNT)
	{
		HAL_GPIO_WritePin(out[out_name].GPIOx, out[out_name].GPIO_Pin , GPIO_PIN_RESET);
		out[out_name].ucNoRestartState =1;
	}

}
/*
 *
 */
void vOutHWDisabale(OUT_NAME_TYPE out_name)
{
	if (out_name < OUT_COUNT)
	{
		HAL_GPIO_WritePin(out[out_name].GPIOx, out[out_name].GPIO_Pin , GPIO_PIN_SET);
		out[out_name].ucNoRestartState =0;
	}

}
/*
 * Функция конфигурация номинальной мощности и режима перегрузки канала, с проверкой коректности парамертов
 */
ERROR_CODE vHWOutOverloadConfig(OUT_NAME_TYPE out_name,  float power, uint16_t overload_timer, float overload_power)
{
	ERROR_CODE res = INVALID_ARG;
	if (out_name < OUT_COUNT)     //Проверяем корекность номера канала
	{
		xEventGroupSetBits ( xOutEvent, ( 0x1 < out_name ) );
		if ( (overload_timer <= MAX_OVERLOAD_TIMER) && (
					((power <= MAX_LPOWER) &&  (overload_power<= MAX_OVERLOAD_LPOWER)) ||
					((power <= MAX_HPOWER) &&  (overload_power<= MAX_OVERLOAD_HPOWER) && ( out_name < OUT_HPOWER_COUNT) ) )
			   )
			{
				out[out_name].power = power;
				out[out_name].overload_config_timer = overload_timer;
				out[out_name].overload_power = overload_power;
				res = ERROR_OK;
			}
		xEventGroupClearBits ( xOutEvent, ( 0x1 <out_name ) );
	}
	return ( res );
}
/*
 * Функция конфигурации режима рестарта
 */
ERROR_CODE vHWOutResetConfig(OUT_NAME_TYPE out_name, uint8_t restart_count, uint16_t timer)
{
	ERROR_CODE res = INVALID_ARG;
	if ((out_name < OUT_COUNT) && (timer<= MAX_RESET_TIMER ))     //Проверяем корекность номера канала
	{
		xEventGroupSetBits (xOutEvent, ( 0x1 < out_name) );
		out[out_name].error_count = restart_count;
		out[out_name].restart_timer = 0U;
		out[out_name].restart_config_timer = timer;
		res = ERROR_OK;
		xEventGroupClearBits (xOutEvent, ( 0x1 < out_name) );
	}
	return ( res );
}
/*
 * Функция конфигурации режима PWM
 */
ERROR_CODE vOutSetPWM(OUT_NAME_TYPE out_name, uint8_t PWM)
{
	ERROR_CODE res = INVALID_ARG;
	if ((out_name <  OUT_COUNT ) && (PWM <=MAX_PWM)) //Проверяем коректность агрументов
	{
		xEventGroupSetBits (xOutEvent, ( 0x1 <out_name) );
		out[out_name].PWM = PWM;
		if (out[out_name].out_state == STATE_OUT_ON) //Если выход вклчюен и не находится в каком-то переходном процессе
		{
			vHWOutSet( out_name, MAX_PWM );
		}
		res = ERROR_OK;
		xEventGroupClearBits (xOutEvent, ( 0x1 < out_name) );
	}
	return ( res );
}
/*
 *
 */
void vHWOutSet( OUT_NAME_TYPE out_name, uint8_t power)
{
   TIM_OC_InitTypeDef sConfigOC = {0};
   sConfigOC.OCMode = TIM_OCMODE_PWM1;
   sConfigOC.Pulse = (uint32_t )( (float)power/ MAX_POWER * (out[out_name].ptim->Init.Period *(float)out[out_name].PWM/ MAX_PWM ) )+1U;
   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
   HAL_TIM_PWM_Stop(out[out_name].ptim,out[out_name].channel);
   HAL_TIM_PWM_ConfigChannel(out[out_name].ptim, &sConfigOC, out[out_name].channel);
   HAL_TIM_PWM_Start(out[out_name].ptim,out[out_name].channel);
   out[out_name].out_line_state = 1;
   return;
}
/*
 *
 */
void vOutSetState(OUT_NAME_TYPE out_name, uint8_t state)
{
	out[out_name].out_logic_state = (state != 0U) ? OUT_ON : OUT_OFF;
	return;
}
/*
 *
 */
void vOutSet(OUT_NAME_TYPE out_name)
{
	out[out_name].out_logic_state = OUT_ON;
	return;
}
/*
 *
 */
void vOutReset(OUT_NAME_TYPE out_name)
{
	out[out_name].out_logic_state = OUT_OFF;
	return;
}
/*
 *
 */
PDM_OUT_STATE_t eOutGetState ( OUT_NAME_TYPE eChNum  )
{
	return ( (eChNum < OUT_COUNT) ? out[eChNum ].out_state : 0U );
}
/*
 *
 */
float fOutGetCurrent ( OUT_NAME_TYPE eChNum)
{
	return ( (eChNum < OUT_COUNT) ?  out[eChNum].current : 0U );
}
/*
 *
 */


/*
 *
 */
ERROR_FLAGS_TYPE eOutGetError(OUT_NAME_TYPE eChNum )
{
	return ( (eChNum < OUT_COUNT) ? out[eChNum ].error_flag : 0U );
}

float fOutGetMaxCurrent(OUT_NAME_TYPE eChNum)
{
	return ( (eChNum < OUT_COUNT) ? out[eChNum ].power : 0U );
}

void vOutEventInit()
{
	xOutEvent = xEventGroupCreateStatic(&xOutCreatedEventGroup );
}
/*
 *
 */
void vOutInit( void )
{

	//Инициализация портов упраления ключами
	HAL_GPIO_WritePin(GPIOG, Cs_Dis20_5_Pin|Cs_Dis20_2_Pin|Cs_Dis20_1_Pin|Cs_Dis8_13_14_Pin
	                          |Cs_Dis8_17_18_Pin|Cs_Dis8_15_16_Pin|Cs_Dis20_3_Pin|Cs_Dis20_4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, Cs_Dis8_11_12_Pin|Cs_Dis20_7_Pin|Cs_Dis8_19_20_Pin|Cs_Dis20_8_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, Cs_Dis20_6_Pin|Cs_Dis8_9_10_Pin, GPIO_PIN_SET);
	vHWOutInit(OUT_1, &htim4, TIM_CHANNEL_3, GPIOG,Cs_Dis20_1_Pin, DEFAULT_PWM	);
	vHWOutInit(OUT_2, &htim4, TIM_CHANNEL_4, GPIOG,Cs_Dis20_2_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_3, &htim2, TIM_CHANNEL_1, GPIOG,Cs_Dis20_3_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_4, &htim3, TIM_CHANNEL_2, GPIOG,Cs_Dis20_4_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_5, &htim1, TIM_CHANNEL_3, GPIOG, Cs_Dis20_5_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_6, &htim1, TIM_CHANNEL_4, GPIOE, Cs_Dis20_6_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_7, &htim12, TIM_CHANNEL_1, GPIOD,Cs_Dis20_7_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_8, &htim4, TIM_CHANNEL_2, GPIOD,Cs_Dis20_8_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_9, &htim1, TIM_CHANNEL_1,  GPIOE, Cs_Dis8_9_10_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_10, &htim1, TIM_CHANNEL_2, GPIOE, Cs_Dis8_9_10_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_11, &htim2, TIM_CHANNEL_4, GPIOD, Cs_Dis8_11_12_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_12, &htim2, TIM_CHANNEL_3, GPIOD, Cs_Dis8_11_12_Pin,DEFAULT_PWM	 );
	vHWOutInit(OUT_13, &htim8, TIM_CHANNEL_1,GPIOG,Cs_Dis8_13_14_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_14, &htim8, TIM_CHANNEL_2,GPIOG, Cs_Dis8_13_14_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_15, &htim3, TIM_CHANNEL_1,GPIOG, Cs_Dis8_15_16_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_16, &htim2, TIM_CHANNEL_2, GPIOG, Cs_Dis8_15_16_Pin, DEFAULT_PWM	 );
	vHWOutInit(OUT_17, &htim8, TIM_CHANNEL_3, GPIOG,  Cs_Dis8_17_18_Pin,DEFAULT_PWM	 );
	vHWOutInit(OUT_18,  &htim8, TIM_CHANNEL_4, GPIOG, Cs_Dis8_17_18_Pin, DEFAULT_PWM  );
	vHWOutInit(OUT_19, &htim12, TIM_CHANNEL_2,GPIOD,Cs_Dis8_19_20_Pin, DEFAULT_PWM  );
	vHWOutInit(OUT_20, &htim4, TIM_CHANNEL_1, GPIOD,Cs_Dis8_19_20_Pin,  DEFAULT_PWM	 );
	HAL_TIM_Base_Start_IT(&htim2);
	return;
}



/*
 * Функция вытаскивает из входного буфера Indata  (размером FrameSize*BufferSize) со смещением InIndex FrameSize отсчетов,
 * счетает среднее арефмитическое и записывает в буффер OutData со смещением OutIndex
 */
static void vGetAverDataFromRAW(uint16_t * InData, uint16_t *OutData, uint16_t InIndex, uint16_t OutIndex, uint8_t Size,uint16_t FrameSize, uint16_t BufferSize)
{
	uint32_t temp;
	for (uint8_t i=0;i<Size;i++)
	{
		temp = 0;
		for (uint8_t j=0;j<FrameSize;j++)
		{
		  temp += InData[InIndex+i+j*BufferSize];
		}
		OutData[OutIndex+i] = temp / FrameSize;
	}
	return;
}


static void vHWOutOFF( uint8_t ucChannel )
{
	HAL_TIM_PWM_Stop(out[ucChannel].ptim,  out[ucChannel].channel);
	out[ucChannel].out_line_state  = 0;
}
/*
 *
 */
static void vTurnOutToError( uint8_t ucChannel,  ERROR_FLAGS_TYPE error_flag)
{
	if ( ucChannel <= OUT_COUNT )
	{
		if (out[ ucChannel ].out_state == STATE_OUT_ON_PROCESS)
		{
			return;
		}
		else
		if (out[ucChannel].out_line_state == 1)
		{
			vHWOutOFF(ucChannel);
			out[ ucChannel ].out_state = STATE_OUT_ERROR_PROCESS; //Переходим в состония бработки ошибок
			out[ucChannel].error_flag  = error_flag;

		}
		else
		if ((out[ucChannel].out_line_state == 0 ) && (out[ ucChannel ].out_state != STATE_OUT_ERROR))
		{
			out[ucChannel].error_flag  = ERROR_CIRCUT_BREAK;
			out[ucChannel].current 	   = 0U;
			out[ ucChannel ].out_state = STATE_OUT_ERROR;
		}
	}
	return;
}
/*
 *
 */
static float fGetDataFromRaw( float fraw,PDM_OUTPUT_TYPE xOut)
{
	float fRes;
	for (uint8_t r = 0; r < 4U; r++)
    {
		if (( fraw < xOut.CSC[r].data ) || (r ==3))
		 {
						 fRes =  fraw * xOut.CSC[r].k;
						 fRes += xOut.CSC[r].b ;
						 fRes *= fraw/RR;
						 break;
		 }
	 }
	return ( fRes );
}
/*
 *  Функция усредняет данные из буфеера АЦП, и пробразует их значения
 */

static void vDataConvertToFloat( void)
{
	uint8_t i;
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 7-8
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawCurData, 0U, 6U, 2U ,ADC_FRAME_SIZE, ADC1_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 19-20
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawCurData, 2U, 18U, 2U ,ADC_FRAME_SIZE, ADC1_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов AIN
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawVData, 4U, 0U, 4U ,ADC_FRAME_SIZE, ADC1_CHANNELS);
	 // Полчени из буфера ADC 2 данныех каналов каналов тока 4-6
	 vGetAverDataFromRAW((uint16_t *)&ADC2_IN_Buffer, (uint16_t *)&muRawCurData,0U, 3U, 3U ,ADC_FRAME_SIZE, ADC2_CHANNELS);
	 // Полчени из буфера ADC 2 данныех каналов каналов тока 9-12
	 vGetAverDataFromRAW((uint16_t *)&ADC2_IN_Buffer, (uint16_t *)&muRawCurData, 3U, 8U, 4U ,ADC_FRAME_SIZE, ADC2_CHANNELS);
	 // Полчени из буфера ADC 3 данныех каналов каналов тока 1-3
	 vGetAverDataFromRAW((uint16_t *)&ADC3_IN_Buffer, (uint16_t *)&muRawCurData, 0U, 0U, 3U ,ADC_FRAME_SIZE, ADC3_CHANNELS);
	 // Полчени из буфера ADC 3 данныех каналов каналов тока 13-18
	 vGetAverDataFromRAW((uint16_t *)&ADC3_IN_Buffer, (uint16_t *)&muRawCurData, 3U, 12U, 6U ,ADC_FRAME_SIZE, ADC3_CHANNELS);
	 //Преобразование во флоат данных AIN
	 for ( i = 0; i < AIN_COUNT; i++ )
	 {
		 mfVData[ i ] = (float) muRawVData[ i ] ;
	 }
	return;
}






/*
 *
 */
 void vADC_Ready ( uint8_t adc_number )
 {
   static portBASE_TYPE xHigherPriorityTaskWoken;
   xHigherPriorityTaskWoken = pdFALSE;
   switch ( adc_number )
   {
     case ADC3_READY:
       xEventGroupSetBitsFromISR( pADCEvent, ADC3_READY, &xHigherPriorityTaskWoken );
       break;
     case ADC2_READY:
       xEventGroupSetBitsFromISR( pADCEvent, ADC2_READY, &xHigherPriorityTaskWoken );
       break;
     case ADC1_READY:
       xEventGroupSetBitsFromISR( pADCEvent, ADC1_READY, &xHigherPriorityTaskWoken );
       break;
     default:
       break;
   }
   portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
   return;
 }


 /*
  *
  */
 static void vOutControlFSM(void)
 {

	EventBits_t config_state  = xEventGroupGetBits (xOutEvent);  //Получаем состоние конфигурационных флагов
    for (uint8_t i=0; i<OUT_COUNT;i++)
 	{
    	if (out[i].ucNoRestartState !=0)
    	{
    		 if ( (muRawCurData[ i ] > ERROR_CURRENT ) && ( out[i].out_line_state == 0 ) )
    		 {
    				out[i].error_flag  = ERROR_CIRCUT_BREAK;
    				out[i].out_state   = STATE_OUT_ERROR;
    				out[i].current 	   = 0U;
    		 }
    		 else
    		 {

    			   out[i].current = fGetDataFromRaw( ((float) muRawCurData [ i ] *K ) , out[i] );
    		 }

 			if ( (config_state & ((uint32_t)0x1< i)) ==0 ) //Если канал не находится в режиме конфигурации, то переходим к обработке
 			{
 						switch (out[i].out_state)
 						{
 						    case STATE_OUT_ERROR:
 						    	if
 						        (  (out[i].out_logic_state == OUT_ON) && (out[i].error_flag  == ERROR_CIRCUT_BREAK ))
 						    	{
 						    		out[i].out_state = STATE_OUT_ON_PROCESS;
 						    	}
 						    	break;
 							case STATE_OUT_OFF: //Состония входа - выключен
 								if (out[i].out_logic_state == OUT_ON)  //Если обнаружено логическое соостония -вкл, то переходим в состония включения
 								{
 									out[i].out_state 	   = STATE_OUT_ON_PROCESS;
 									out[i].restart_timer   = 0U;
 								}
 								break;
 							case STATE_OUT_ON_PROCESS: //Состояния влючения
 								if ( (out[i].out_logic_state == OUT_OFF) || ( out[i].current > out[i].overload_power ) )
 								{
 									vHWOutOFF(i);
 									out[i].out_state = (out[i].out_logic_state == OUT_OFF) ? STATE_OUT_OFF : STATE_OUT_RESTART_PROCESS;
 								 	out[i].error_flag  = ( out[i].current > out[i].power ) ? ERROR_OVER_LIMIT : ERROR_OFF;
 								 	out[i].restart_timer = 0U;
 									break;
 								}
 								out[i].restart_timer++;
 								uint8_t ucCurrentPower = MAX_POWER;
 								if  ( out[i].restart_timer >= out[i].overload_config_timer ) //Если прошло время полонго пуска
 								{
 									out[i].out_state = STATE_OUT_ON; //переходим в стосония влючено и запускаем выход на 100% мощности
 									out[i].error_flag  = ERROR_OFF;
 								}
 								else
 								{   //время пуска не прошоло, вычисляем текущую мощность, котору надо пдать на выход.
 									ucCurrentPower =(uint8_t) (((float)out[i].restart_timer/(float)out[i].overload_config_timer)*MAX_POWER);
 									if (ucCurrentPower < START_POWER)
 									{
 										ucCurrentPower = START_POWER;
 									}
 								}
 								vHWOutSet(i,ucCurrentPower);
 								break;
 							case STATE_OUT_ON:  // Состояние входа - включен
 									if  ( (out[i].out_logic_state == OUT_OFF) ||  ( out[i].current > out[i].power ) )
 									{
 										vHWOutOFF(i);
 										out[i].out_state = (out[i].out_logic_state == OUT_OFF) ? STATE_OUT_OFF : STATE_OUT_RESTART_PROCESS;
 										out[i].error_flag  = ( out[i].current > out[i].power ) ? ERROR_OVER_LIMIT : ERROR_OFF;
 									}
 									break;
 							case STATE_OUT_RESTART_PROCESS:
 								out[i].restart_timer++;
 								if  ( out[i].restart_timer >= out[i].restart_config_timer )
 								{
 									if ( out[i].ucNoRestartState  == 2 )
 									{
 										out[i].out_state = STATE_OUT_ERROR;
 									}
 									else
 									{
 										out[i].out_state = STATE_OUT_ON_PROCESS;
 									    out[i].error_flag  = ERROR_OFF;
 										if (out[i].error_count >= 1U)
 										{
 											out[i].error_count--;
 											if (out[i].error_count == 0)
 											{
 												out[i].ucNoRestartState  = 2;
 											}
 										}
 									}

 								}
 								break;

 							default:
 								break;
 						}
    		 }
    	}
    	else
    	{
    		out[i].out_state = STATE_OUT_OFF;
    	}

 	}
 }
 /*
  *
  */
 void vADCTask(void * argument)
 {
   /* USER CODE BEGIN vADCTask */
   pADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );
   TickType_t xLastWakeTime;
   const TickType_t xPeriod = pdMS_TO_TICKS(1 );
   xLastWakeTime = xTaskGetTickCount();
   //vOutInit();
   /* Infinite loop */
   for(;;)
   {
	   vTaskDelayUntil( &xLastWakeTime, xPeriod );
	   HAL_ADC_Start_DMA( &hadc1,( uint32_t* )&ADC1_IN_Buffer, ( ADC_FRAME_SIZE * ADC1_CHANNELS ));
	   HAL_ADC_Start_DMA( &hadc2,( uint32_t* )&ADC2_IN_Buffer, ( ADC_FRAME_SIZE * ADC2_CHANNELS ));
	   HAL_ADC_Start_DMA( &hadc3,( uint32_t* )&ADC3_IN_Buffer, ( ADC_FRAME_SIZE * ADC3_CHANNELS ));
	   xEventGroupWaitBits( pADCEvent, ( ADC3_READY  | ADC2_READY | ADC1_READY   ), pdTRUE, pdTRUE, portMAX_DELAY );
	   HAL_ADC_Stop_DMA(&hadc1);
	   HAL_ADC_Stop_DMA(&hadc2);
	   HAL_ADC_Stop_DMA(&hadc3);
	   vDataConvertToFloat();
	   vOutControlFSM();
   }
   /* USER CODE END vADCTask */
 }

/*
 * Напряжение на аналогвом входе
 */
 float fAinGetState(AIN_NAME_TYPE channel)
 {
	 return  ( (channel < AIN_COUNT) ? mfVData[channel]* COOF : 0U ) ;
 }
 /*
  * Дискретные входа
  *
  *
  */
 void vDoutConfig()
 {
 }



