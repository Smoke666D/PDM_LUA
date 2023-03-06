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
extern TIM_HandleTypeDef htim6;

volatile int16_t            ADC1_IN_Buffer[ADC_FRAME_SIZE*ADC1_CHANNELS] = { 0U };   //ADC1 input data buffer
volatile int16_t            ADC2_IN_Buffer[ADC_FRAME_SIZE*ADC2_CHANNELS] = { 0U };   //ADC2 input data buffer
volatile int16_t            ADC3_IN_Buffer[ADC_FRAME_SIZE*ADC3_CHANNELS] = { 0U };   //ADC3 input data buffer


#define TEMP_DATA    5


PDM_OUTPUT_TYPE out[OUT_COUNT]  					__SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawCurData[OUT_COUNT]				__SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawVData[AIN_COUNT + 2]   		__SECTION(RAM_SECTION_CCMRAM);
static   EventGroupHandle_t pADCEvent 				__SECTION(RAM_SECTION_CCMRAM);
static   StaticEventGroup_t xADCCreatedEventGroup   __SECTION(RAM_SECTION_CCMRAM);
static EventGroupHandle_t  * pxPDMstatusEvent		__SECTION(RAM_SECTION_CCMRAM);


#ifdef REV_2



static KAL_DATA CurSensData[OUT_COUNT][KOOF_COUNT] ={   {{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
										{{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
										{{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
										{{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
										{{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
										{{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
										{{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
										{{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										{{K05O08,V05O08},{K10O08,V10O08},{K15O08,V15O08},{K30O08,V30O08}},
										};
#endif

static void vHWOutSet( OUT_NAME_TYPE out_name, uint8_t power);
static void vHWOutInit(OUT_NAME_TYPE out_name, TIM_HandleTypeDef * ptim, uint32_t  uiChannel, GPIO_TypeDef* EnablePort, uint16_t EnablePin, GPIO_TypeDef* OutPort, uint16_t OutPin );
static void vHWOutOFF( uint8_t ucChannel );
static void ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
static void ADC_Stop_DMA(ADC_HandleTypeDef* hadc);
static void ADC_STOP();
void vOutEnable(OUT_NAME_TYPE out_name);
void vOutDisable(OUT_NAME_TYPE out_name);


static uint32_t ulRestartTimer()
{
	uint32_t data = htim6.Instance->CNT;
	htim6.Instance->CNT = 0U;
	return ( data );
}
/*
 *
 */
void vOutInit( void )
{
	//Инициализация портов упраления ключами
	HAL_GPIO_WritePin(GPIOG, Cs_Dis20_5_Pin|Cs_Dis20_2_Pin|Cs_Dis20_1_Pin|Cs_Dis8_13_14_Pin
	                          |Cs_Dis8_17_18_Pin|Cs_Dis8_15_16_Pin|Cs_Dis20_3_Pin|Cs_Dis20_4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, Cs_Dis8_11_12_Pin|Cs_Dis20_7_Pin|Cs_Dis8_19_20_Pin|Cs_Dis20_8_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, Cs_Dis20_6_Pin|Cs_Dis8_9_10_Pin, GPIO_PIN_RESET);
	vHWOutInit(OUT_1, &htim4, TIM_CHANNEL_3, GPIOG,Cs_Dis20_1_Pin, GPIOD  ,InCH20_1_Pin);
	vHWOutInit(OUT_2, &htim4, TIM_CHANNEL_4, GPIOG,Cs_Dis20_2_Pin, GPIOD , InCH20_2_Pin );
	vHWOutInit(OUT_3, &htim2, TIM_CHANNEL_1, GPIOG,Cs_Dis20_3_Pin , InCH20_3_GPIO_Port, InCH20_3_Pin);
	vHWOutInit(OUT_4, &htim3, TIM_CHANNEL_2, GPIOG,Cs_Dis20_4_Pin , GPIOB, InCH20_4_Pin);
	vHWOutInit(OUT_5, &htim1, TIM_CHANNEL_3, GPIOG, Cs_Dis20_5_Pin ,GPIOE, InCH20_5_Pin);
	vHWOutInit(OUT_6, &htim1, TIM_CHANNEL_4, GPIOE, Cs_Dis20_6_Pin ,GPIOE, InCH20_6_Pin);
	vHWOutInit(OUT_7, &htim12, TIM_CHANNEL_1, GPIOD,Cs_Dis20_7_Pin , GPIOB, InCH20_7_Pin );
	vHWOutInit(OUT_8, &htim4, TIM_CHANNEL_2, GPIOD,Cs_Dis20_8_Pin ,GPIOD ,InCH20_8_Pin );
	vHWOutInit(OUT_9, &htim1, TIM_CHANNEL_1,  GPIOE, Cs_Dis8_9_10_Pin,  GPIOE ,InCH8_9_Pin );
	vHWOutInit(OUT_10, &htim1, TIM_CHANNEL_2, GPIOE, Cs_Dis8_9_10_Pin , GPIOE, InCH8_10_Pin);
	vHWOutInit(OUT_11, &htim2, TIM_CHANNEL_4, GPIOD, Cs_Dis8_11_12_Pin , GPIOB , InCH8_11_Pin);
	vHWOutInit(OUT_12, &htim2, TIM_CHANNEL_3, GPIOD, Cs_Dis8_11_12_Pin ,GPIOB , InCH8_12_Pin );
	vHWOutInit(OUT_13, &htim8, TIM_CHANNEL_1,GPIOG,Cs_Dis8_13_14_Pin , GPIOC , InCH8_13_Pin  );
	vHWOutInit(OUT_14, &htim8, TIM_CHANNEL_2,GPIOG, Cs_Dis8_13_14_Pin , GPIOC , InCH8_14_Pin);
	vHWOutInit(OUT_15, &htim3, TIM_CHANNEL_1,GPIOG, Cs_Dis8_15_16_Pin , GPIOB , InCH8_15_Pin );
	vHWOutInit(OUT_16, &htim2, TIM_CHANNEL_2, GPIOG, Cs_Dis8_15_16_Pin ,GPIOB , InCH8_16_Pin);
	vHWOutInit(OUT_17, &htim8, TIM_CHANNEL_3, GPIOG,  Cs_Dis8_17_18_Pin ,  GPIOC , InCH8_17_Pin	);
	vHWOutInit(OUT_18,  &htim8, TIM_CHANNEL_4, GPIOG, Cs_Dis8_17_18_Pin , GPIOC , InCH8_18_Pin);
	vHWOutInit(OUT_19, &htim12, TIM_CHANNEL_2,GPIOD,Cs_Dis8_19_20_Pin , GPIOB , InCH8_19_Pin);
	vHWOutInit(OUT_20, &htim4, TIM_CHANNEL_1, GPIOD,Cs_Dis8_19_20_Pin, GPIOD, InCH8_20_Pin );
	HAL_TIM_Base_Start_IT(&htim2);
	return;
}
/*
 *
 */

/*
 * Функция конфигурация номинальной мощности и режима перегрузки канала, с проверкой коректности парамертов
 */
ERROR_CODE vHWOutOverloadConfig(OUT_NAME_TYPE out_name,  float power, uint16_t overload_timer, float overload_power, OFF_STATE_TYPE off_state)
{
	ERROR_CODE res = INVALID_ARG;
	if (out_name < OUT_COUNT)     //Проверяем корекность номера канала
	{
		RESET_FLAG(out_name,ENABLE_FLAG);
	//	if ( (overload_timer <= MAX_OVERLOAD_TIMER) && (
	//				((power <= MAX_LPOWER) &&  (overload_power<= MAX_OVERLOAD_LPOWER)) ||
	//				((power <= MAX_HPOWER) &&  (overload_power<= MAX_OVERLOAD_HPOWER) && ( out_name < OUT_HPOWER_COUNT) ) )
	//		   )
			{
				out[out_name].power = power;
				out[out_name].overload_config_timer = overload_timer;
				out[out_name].overload_power = overload_power;
				if  ( off_state == RESETTEBLE_STATE_AFTER_ERROR)
				{
					SET_FLAG(out_name,RESETTEBLE_FLAG);
				}
				else
				{
					RESET_FLAG(out_name,RESETTEBLE_FLAG);
				}
				res = ERROR_OK;
			}
			SET_FLAG(out_name,ENABLE_FLAG);
	}
	return ( res );
}
/*
 * Функция конфигурации режима рестарта
 */
ERROR_CODE vHWOutResetConfig(OUT_NAME_TYPE out_name, uint8_t restart_count, uint16_t timer)
{
	ERROR_CODE res = INVALID_ARG;
	if ( out_name < OUT_COUNT )      //Проверяем корекность номера канала
	{
		RESET_FLAG(out_name,ENABLE_FLAG);;
		out[out_name].error_counter = restart_count;
		out[out_name].restart_timer = 0U;
		out[out_name].restart_config_timer = timer;
		res = ERROR_OK;
		SET_FLAG(out_name,ENABLE_FLAG);
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
		if ( out[out_name].PWM != PWM )
		{
			out[out_name].PWM = PWM;
			if (out[out_name].out_state != STATE_OUT_ERROR) //Если выход вклчюен и не находится в каком-то переходном процессе
			{
				vHWOutSet( out_name, MAX_PWM );
			}
		}
		res = ERROR_OK;
	}
	return ( res );
}
/*
 *
 */
ERROR_CODE vOutSetSoftStart(OUT_NAME_TYPE out_name, uint16_t timer, uint8_t power)
{
	ERROR_CODE res = INVALID_ARG;
	if (out_name <  OUT_COUNT ) //Проверяем коректность агрументов
	{
		out[out_name].soft_start_timer = timer;
		out[out_name].soft_start_power = power;
		res = ERROR_OK;
	}
	return ( res );
}
/*
 *
 */
void vOutSetState(OUT_NAME_TYPE out_name, uint8_t state)
{
	switch (state)
	{
		case 0:
			SET_FLAG(out_name, CONTROL_OFF_STATE);
			//out[out_name].out_state = STATE_OUT_OFF;
			//vHWOutOFF(out_name);
			/*if ( out_name > 7 )
			{
				HAL_GPIO_WritePin(out[out_name].GPIOx, out[out_name].GPIO_Pin , CS_DISABLE);
			}*/
			break;
		case 1:
			SET_FLAG(out_name, CONTROL_ON_STATE);
			//if (out[out_name].out_state == STATE_OUT_OFF)
			//{
			//		out[out_name].out_state = STATE_OUT_ON_PROCESS;
			//}
			break;
		default:
			break;
	}
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
void vGetDoutStatus(uint32_t * Dout1_10Status, uint32_t * Dout11_20Status)
{
	uint32_t status1 		= 0;
	uint32_t status2 		= 0;
	uint32_t channel_state 	= 0;
	for (uint8_t i = 0; i < OUT_COUNT ;i++)
	{
		switch (out[i].error_flag)
		{
			case ERROR_CIRCUT_BREAK:
				channel_state = 0x02;
				break;
			case ERROR_OVERLOAD:
			case  ERROR_OVER_LIMIT:
				channel_state = 0x03;
				break;
			default:
				switch( out[i].out_state)
				{
					case STATE_OUT_ON_PROCESS:
					case STATE_OUT_ON:
						channel_state = 1;
						break;
					default:
						channel_state = 0;
						break;
				}
		}
		if (i<10)
		{
			status1 |= channel_state<<(2*i);
		}
		else
		{
			status2 |= channel_state<<(2*(i-10));
		}
	}
	*Dout1_10Status = status1;
	*Dout11_20Status = status2;
	return;
}

/*
 *
 */
ERROR_FLAGS_TYPE eOutGetError(OUT_NAME_TYPE eChNum )
{
	return ( (eChNum < OUT_COUNT) ? out[eChNum ].error_flag : 0U );
}
/*
 *
 */
float fOutGetMaxCurrent(OUT_NAME_TYPE eChNum)
{
	return ( (eChNum < OUT_COUNT) ? out[eChNum ].power : 0U );
}
/*
 * Напряжение на аналогвом входе
 */
float fAinGetState ( AIN_NAME_TYPE channel )
{
 return  ( (channel < AIN_COUNT) ? (float) muRawVData[channel] *  AINCOOF1 : 0U ) ;
}
/*
 *
 */
float fBatteryGet ( void )
{
	 return (float)muRawVData[3] * AINCOOF1 + INDIOD;
}
/*
 *
 */

float fTemperatureGet ( uint8_t chanel )
{
	return ((float)muRawVData[4] * K - 0.76) /0.0025 + 25;
}
/*
 *
 */

/*
 *
 */
static void vHWOutInit(OUT_NAME_TYPE out_name, TIM_HandleTypeDef * ptim, uint32_t  uiChannel, GPIO_TypeDef* EnablePort, uint16_t EnablePin, GPIO_TypeDef* OutPort, uint16_t OutPin )
{
	volatile uint8_t j;
	if ( out_name < OUT_COUNT )
	{
		out[out_name].ptim 			   = ptim;
		out[out_name].channel 		   = uiChannel;
		out[out_name].GPIOx 		   = EnablePort;
		out[out_name].GPIO_Pin		   = EnablePin;
		out[out_name].OutGPIOx 		   = OutPort;
		out[out_name].OutGPIO_Pin	   = OutPin;
		out[out_name].error_counter    = 0U;
		out[out_name].soft_start_timer = 0;
		out[out_name].out_state		   = STATE_OUT_OFF;
		out[out_name].current 		   = 0.0;
		out[out_name].PWM_err_counter  = 0;
		out[out_name].POWER_SOFT 	   = 0;
		RESET_FLAG(out_name,CONTROL_FLAGS );
		if (out_name < OUT_HPOWER_COUNT)
		{
			vHWOutOverloadConfig(out_name, DEFAULT_HPOWER,DEFAULT_OVERLOAD_TIMER_HPOWER, DEFAULT_HPOWER_MAX, RESETTEBLE_STATE_AFTER_ERROR);
		}
		else
		{
			vHWOutOverloadConfig(out_name, DEFAULT_LPOWER,DEFAULT_OVERLOAD_TIMER_LPOWER, DEFAULT_LPOWER_MAX , RESETTEBLE_STATE_AFTER_ERROR);
		}
		vHWOutResetConfig(out_name,DEFAULT_RESET_COUNTER, DEFAULT_RESET_TIMER);
		vOutSetPWM(out_name, DEFAULT_PWM);
		RESET_FLAG(out_name,ENABLE_FLAG);

		out[out_name].error_flag = ERROR_OFF;
		for (j=0; j< KOOF_COUNT - 1 ; j++)
		{
			//Проверяем что хоты одно значение АЦП не равно нулю,что-то не словить делением на ноль.
			if ((CurSensData[out_name][j].Data != 0.0) || (CurSensData[out_name][j+1].Data != 0.0 ))
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
		HAL_GPIO_WritePin(out[out_name].GPIOx, out[out_name].GPIO_Pin , CS_ENABLE);
	}
	return;
}

/*
 *
 */
static void vHWOutSet( OUT_NAME_TYPE out_name, uint8_t power)
{
   TIM_OC_InitTypeDef sConfigOC = {0};

	   out[out_name].POWER_SOFT = power;
	   sConfigOC.OCMode = TIM_OCMODE_PWM1;
	   sConfigOC.Pulse = (uint32_t )( (float)power/ MAX_POWER * (out[out_name].ptim->Init.Period *(float)out[out_name].PWM/ MAX_PWM ) )+1U;
	   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	   HAL_TIM_PWM_Stop(out[out_name].ptim,out[out_name].channel);
	   HAL_TIM_PWM_ConfigChannel(out[out_name].ptim, &sConfigOC, out[out_name].channel);
	   HAL_TIM_PWM_Start(out[out_name].ptim,out[out_name].channel);
	 /*  if ( out_name < 8 )
	   {
		   HAL_GPIO_WritePin(out[out_name].GPIOx, out[out_name].GPIO_Pin , CS_ENABLE);
	   }*/

   return;
}
/*
 * Функция вытаскивает из входного буфера Indata  (размером FrameSize*BufferSize) со смещением InIndex FrameSize отсчетов,
 * счетает среднее арефмитическое и записывает в буффер OutData со смещением OutIndex
 */
static void vGetAverDataFromRAW(uint16_t * InData, uint16_t *OutData, uint8_t InIndex, uint8_t OutIndex, uint8_t Size, uint16_t BufferSize)
{
	uint32_t temp;
	for (uint8_t i=0; i<Size; i++ )
	{
		temp = 0;
		for (uint8_t j=0;j < ADC_FRAME_SIZE; j++ )
		{
		  temp += InData[ InIndex + i + j * BufferSize ];
		}
		OutData[ OutIndex + i ] = temp / ADC_FRAME_SIZE;
	}
	return;
}
/*
*/
static void vHWOutOFF( uint8_t ucChannel )
{
	HAL_TIM_PWM_Stop(out[ucChannel].ptim,  out[ucChannel].channel);
	out[ucChannel].POWER_SOFT = 0;

	return;
}
/*
 *
 */
static float fGetDataFromRaw( float fraw,PDM_OUTPUT_TYPE xOut)
{
	float fRes;
	for (uint8_t r = 0; r < ( KOOF_COUNT -1 ); r++)
    {
		if (( fraw < xOut.CSC[r].data ) || (r == (KOOF_COUNT -2)) )
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
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawCurData, 0U, 6U, 2U , ADC1_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 19-20
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawCurData, 2U, 18U, 2U , ADC1_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов AIN
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawVData, 4U, 0U, 5U , ADC1_CHANNELS);
	 // Полчени из буфера ADC 2 данныех каналов каналов тока 4-6
	 vGetAverDataFromRAW((uint16_t *)&ADC2_IN_Buffer, (uint16_t *)&muRawCurData,0U, 3U, 3U , ADC2_CHANNELS);
	 // Полчени из буфера ADC 2 данныех каналов каналов тока 9-12
	 vGetAverDataFromRAW((uint16_t *)&ADC2_IN_Buffer, (uint16_t *)&muRawCurData, 3U, 8U, 4U , ADC2_CHANNELS);
	 // Полчени из буфера ADC 3 данныех каналов каналов тока 1-3
	 vGetAverDataFromRAW((uint16_t *)&ADC3_IN_Buffer, (uint16_t *)&muRawCurData, 0U, 0U, 3U , ADC3_CHANNELS);
	 // Полчени из буфера ADC 3 данныех каналов каналов тока 13-18
	 vGetAverDataFromRAW((uint16_t *)&ADC3_IN_Buffer, (uint16_t *)&muRawCurData, 3U, 12U, 6U , ADC3_CHANNELS);
	return;
}
/*
 *
 */
 void vADC_Ready ( uint8_t adc_number )
 {
   static portBASE_TYPE xHigherPriorityTaskWoken;
   xHigherPriorityTaskWoken = pdFALSE;
   xEventGroupSetBitsFromISR( pADCEvent, adc_number, &xHigherPriorityTaskWoken );
   portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
   return;
 }
/*
 *
 */
 static void vGotoRestartState( uint8_t ucChannel, float fCurr )
 {
	 out[ ucChannel ].out_state =  (out[ ucChannel ].error_counter == 1) ? STATE_OUT_ERROR : STATE_OUT_RESTART_PROCESS;
	 if  (( out[ ucChannel ].out_state == STATE_OUT_ERROR ) &&  IS_FLAG_RESET( ucChannel, RESETTEBLE_FLAG ) )
	 {
		 RESET_FLAG(ucChannel,ENABLE_FLAG);
     }
	 out[ ucChannel ].error_flag = ERROR_OVER_LIMIT;
	 out[ ucChannel ].restart_timer = 0U;
	 out[ ucChannel ].soft_start_timer = 0;
	 vHWOutOFF(ucChannel);
	 if ( fCurr < ( ucChannel < OUT_HPOWER_COUNT ? MAX_HOVERLOAD_POWER : MAX_LOVERLOAD_POWER ) )
	 {
		 out[ucChannel].current = fCurr;
	 }
 }
 /*
  *
  */
 static void vOutControlFSM(void)
 {
    for (uint8_t i = 0U; i < OUT_COUNT; i++ )
 	{
    	if (IS_FLAG_SET(i,ENABLE_FLAG) )		/*Если канал не выключен или не в режиме конфигурации*/
    	{


    	    float fCurrent  = fGetDataFromRaw( ((float) muRawCurData [ i ] *K ) , out[i] );
    	    if ( ( fCurrent > out[ i ].power) && (out[i].PWM != 100) )
			{
    	      if  (HAL_GPIO_ReadPin(out[i].OutGPIOx, out[i].OutGPIO_Pin) == GPIO_PIN_SET)
    	      {
    	    	 out[i].PWM_err_counter ++;
    	      }
    	      if  (out[i].PWM_err_counter < 3 )
    	      {
    	    		 fCurrent = out[i].current;
    	      }
			}
    	    else
    	    {
    	    	 out[i].PWM_err_counter = 0;
    	    }
 			switch (out[i].out_state)
 			{
 				case STATE_OUT_OFF: //Состония входа - выключен
 					out[i].current 	   		 = 0U;
 					out[i].restart_timer   	 = 0U;
 					out[i].error_flag 		 = ERROR_OFF;
 					out[i].soft_start_timer  = 0U;
 					break;
 				case STATE_OUT_ON_PROCESS: //Состояния влючения

 					out[i].restart_timer++;
 					out[i].soft_start_timer++;
 					/*if (out[i].soft_start_timer !=0)
 					{
 						if  ( fCurrent  > out[i].power )
 						{
 							vGotoRestartState(i,fCurrent);
 							break;
 						}
 						if  ( out[i].restart_timer >= out[i].soft_start_timer ) //Если прошло время полонго пуска
 						{
 						 		vHWOutSet(i,MAX_POWER);
 						 		out[i].out_state = STATE_OUT_ON; //переходим в стосония влючено и запускаем выход на 100% мощности
 						}
 						else
 						 {   //время пуска не прошоло, вычисляем текущую мощность, котору надо пдать на выход.
 						 		uint8_t ucCurrentPower = out[i].soft_start_power + (uint8_t) (((float)out[i].restart_timer/(float)out[i].soft_start_timer)*(MAX_POWER - out[i].soft_start_power));
 						 		if (ucCurrentPower  > MAX_POWER)
 						 		{
 						 			ucCurrentPower = MAX_POWER;
 						 		}
 						 		vHWOutSet( i, ucCurrentPower );
 						 }
 					}
 					else*/
 					{

 						 if ( out[i].restart_timer  < 2 )
 						 {
 							 break;
 						 }
 						 if  ( fCurrent  > out[ i ].overload_power )
 						 {
 						 	vGotoRestartState(i,fCurrent);
 						 	break;
 						 }
 						 if ( out[ i ].restart_timer >= out[ i ].overload_config_timer )
 						 {
 							out[ i ].out_state = STATE_OUT_ON;
 						 }
 					}
 					out[i].current = fCurrent;
 					break;
 				case STATE_OUT_ON:  // Состояние входа - включен
 					if  (fCurrent  > out[ i ].power  )
 					{
 						vGotoRestartState( i, fCurrent );
 						break;
 					}
 					out[i].error_flag = (fCurrent  < CIRCUT_BREAK_CURRENT ? ERROR_CIRCUT_BREAK: ERROR_OFF );
 					out[i].current = fCurrent;
 					break;
 				case STATE_OUT_RESTART_PROCESS:
 					out[ i ].restart_timer++;
 					if  ( out[ i ].restart_timer >= out[ i ].restart_config_timer )
 					{
 						out[ i ].out_state = STATE_OUT_ON_PROCESS;
 						out[ i ].restart_timer =0;
 						out[ i ].error_flag  = ERROR_OFF;
 						if ( out[i].error_counter !=0 )
 						{
 							out[i].error_counter--;
 						}
 					}
 					break;
 				case STATE_OUT_ERROR:
 				default:
 					break;
 			}
 			// Проверям управляющие сигналы. Если они изменилсь, то выключем или включаем каналы. Это нужно сделать именно тот,
 			// чтобы на следующем циклые конечного автомата были актуальные данные о состонии каналов

 				if ( IS_FLAG_SET( i, CONTROL_OFF_STATE ) && (out[i].out_state != STATE_OUT_OFF ) )
 				{
 				 	out[i].out_state = STATE_OUT_OFF;
 				 	vHWOutOFF(i);
 				}
 				if ( IS_FLAG_SET( i, CONTROL_ON_STATE ) &&  (out[i].out_state == STATE_OUT_OFF))
 				{
 					out[i].out_state = STATE_OUT_ON_PROCESS;
 				 	vHWOutSet( i , MAX_POWER );
 				}
 				RESET_FLAG(i,CONTROL_FLAGS );


   		 }
 	}
 }

 static void vADCEnable(ADC_HandleTypeDef* hadc1,ADC_HandleTypeDef* hadc2,ADC_HandleTypeDef* hadc3)
 {
	     __HAL_ADC_ENABLE(hadc1);
	     __HAL_ADC_ENABLE(hadc2);
	     __HAL_ADC_ENABLE(hadc3);
 }
 /*
  *
  */
 void vADCTask(void * argument)
 {
   /* USER CODE BEGIN vADCTask */

   pADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );
   pxPDMstatusEvent = osLUAetPDMstatusHandle();
   TickType_t xLastWakeTime;
   const TickType_t xPeriod = pdMS_TO_TICKS( 1 );
   xLastWakeTime = xTaskGetTickCount();

   HAL_TIM_Base_Start(&htim6);
   for(;;)
   {
	   vTaskDelayUntil( &xLastWakeTime, xPeriod );
	   xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
	   ulRestartTimer();
	   ADC_Start_DMA( &hadc1,( uint32_t* )&ADC1_IN_Buffer, ( ADC_FRAME_SIZE * ADC1_CHANNELS ));
	   ADC_Start_DMA( &hadc2,( uint32_t* )&ADC2_IN_Buffer, ( ADC_FRAME_SIZE * ADC2_CHANNELS ));
	   ADC_Start_DMA( &hadc3,( uint32_t* )&ADC3_IN_Buffer, ( ADC_FRAME_SIZE * ADC3_CHANNELS ));
	   xEventGroupWaitBits( pADCEvent, ( ADC3_READY  | ADC2_READY | ADC1_READY   ), pdTRUE, pdTRUE, 100 );
	   ADC_STOP();
	   vDataConvertToFloat();
	   vOutControlFSM();
	   vADCEnable(&hadc1,&hadc2,&hadc3); /* Влючаем АЦП, исходя из времени выполнения следующей функции,
	   к моменту ее завершения, АЦП уже включаться*/
   }
   /* USER CODE END vADCTask */
 }


/*
 *
 */
 static void ADC_DMAError(DMA_HandleTypeDef *hdma)
 {
   ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
   hadc->State= HAL_ADC_STATE_ERROR_DMA;
   hadc->ErrorCode |= HAL_ADC_ERROR_DMA;
 }

 static void ADC_DMAConvCplt(DMA_HandleTypeDef *hdma)
 {
   /* Retrieve ADC handle corresponding to current DMA handle */
   ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

   /* Update state machine on conversion status if not in error state */
   if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL | HAL_ADC_STATE_ERROR_DMA))
   {
     /* Update ADC state machine */
     SET_BIT(hadc->State, HAL_ADC_STATE_REG_EOC);

     /* Determine whether any further conversion upcoming on group regular   */
     /* by external trigger, continuous mode or scan sequence on going.      */
     /* Note: On STM32F4, there is no independent flag of end of sequence.   */
     /*       The test of scan sequence on going is done either with scan    */
     /*       sequence disabled or with end of conversion flag set to        */
     /*       of end of sequence.                                            */
     if(ADC_IS_SOFTWARE_START_REGULAR(hadc)                   &&
        (HAL_IS_BIT_CLR(hadc->Instance->SQR1, ADC_SQR1_L) ||
         HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)  )   )
     {
       /* Disable ADC end of single conversion interrupt on group regular */
       /* Note: Overrun interrupt was enabled with EOC interrupt in          */
       /* HAL_ADC_Start_IT(), but is not disabled here because can be used   */
       /* by overrun IRQ process below.                                      */
       __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);

       /* Set ADC state */
       CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

       if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_INJ_BUSY))
       {
         SET_BIT(hadc->State, HAL_ADC_STATE_READY);
       }
     }
   }
   else /* DMA and-or internal error occurred */
   {
     if ((hadc->State & HAL_ADC_STATE_ERROR_INTERNAL) != 0UL)
     {
       /* Call HAL ADC Error Callback function */
     	HAL_ADC_ErrorCallback(hadc);
     }
 	else
 	{
       /* Call DMA error callback */
 		ADC_DMAError(hdma);
     }
   }
 }


#define HAL_TIMEOUT_DMA_ABORT    5U
 /* Private types -------------------------------------------------------------*/

 /*
  *
  */
 HAL_StatusTypeDef DMA_Abort(DMA_HandleTypeDef *hdma)
 {
   /* calculate DMA base and stream number */
   DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;

   uint32_t tickstart = HAL_GetTick();

   if(hdma->State != HAL_DMA_STATE_BUSY)
   {
     hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
     return HAL_ERROR;
   }
   else
   {
     /* Disable all the transfer interrupts */
     hdma->Instance->CR  &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME);
     hdma->Instance->FCR &= ~(DMA_IT_FE);


     /* Disable the stream */
     __HAL_DMA_DISABLE(hdma);

     /* Check if the DMA Stream is effectively disabled */
     while((hdma->Instance->CR & DMA_SxCR_EN) != RESET)
     {
       /* Check for the Timeout */
       if((HAL_GetTick() - tickstart ) > HAL_TIMEOUT_DMA_ABORT)
       {
         /* Update error code */
         hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

         /* Change the DMA state */
         hdma->State = HAL_DMA_STATE_TIMEOUT;

         return HAL_TIMEOUT;
       }
     }

     /* Clear all interrupt flags at correct offset within the register */
     regs->IFCR = 0x3FU << hdma->StreamIndex;

     /* Change the DMA state*/
     hdma->State = HAL_DMA_STATE_READY;

   }
   return HAL_OK;
 }


 static void ADC_STOP()
 {
	   __HAL_ADC_DISABLE(&hadc1);
	   __HAL_ADC_DISABLE(&hadc2);
	   __HAL_ADC_DISABLE(&hadc3);
	   hadc1.Instance->CR2 &= ~ADC_CR2_DMA;
	   hadc2.Instance->CR2 &= ~ADC_CR2_DMA;
	   hadc3.Instance->CR2 &= ~ADC_CR2_DMA;
	   if (hadc1.DMA_Handle->State == HAL_DMA_STATE_BUSY)
	   {
	        if (DMA_Abort(hadc1.DMA_Handle) != HAL_OK)
	        {
	          SET_BIT(hadc1.State, HAL_ADC_STATE_ERROR_DMA);
	        }
	   }
	   if (hadc2.DMA_Handle->State == HAL_DMA_STATE_BUSY)
		   {
		        if (DMA_Abort(hadc2.DMA_Handle) != HAL_OK)
		        {
		          SET_BIT(hadc2.State, HAL_ADC_STATE_ERROR_DMA);
		        }
		   }
	   if (hadc3.DMA_Handle->State == HAL_DMA_STATE_BUSY)
		   {
		        if (DMA_Abort(hadc3.DMA_Handle) != HAL_OK)
		        {
		          SET_BIT(hadc3.State, HAL_ADC_STATE_ERROR_DMA);
		        }
		   }
	   __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_OVR);
	   __HAL_ADC_DISABLE_IT(&hadc2, ADC_IT_OVR);
	   __HAL_ADC_DISABLE_IT(&hadc3, ADC_IT_OVR);
	      ADC_STATE_CLR_SET(hadc1.State,
	                        HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
	                        HAL_ADC_STATE_READY);
	      ADC_STATE_CLR_SET(hadc2.State,
	      	                        HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
	      	                        HAL_ADC_STATE_READY);
	      ADC_STATE_CLR_SET(hadc3.State,
	      	                        HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
	      	                        HAL_ADC_STATE_READY);

 }


/*
 *
 */
 static void ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
 {

   CLEAR_BIT(hadc->Instance->CR2, ADC_CR2_DMA);
   if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
   {
     ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR, HAL_ADC_STATE_REG_BUSY);
     __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC | ADC_FLAG_OVR);
     __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
     hadc->Instance->CR2 |= ADC_CR2_DMA;

     DMA_Base_Registers *regs = (DMA_Base_Registers *)hadc->DMA_Handle->StreamBaseAddress;


      if(HAL_DMA_STATE_READY == hadc->DMA_Handle->State)
      {
        	hadc->DMA_Handle->State = HAL_DMA_STATE_BUSY;
        	hadc->DMA_Handle->ErrorCode = HAL_DMA_ERROR_NONE;
            /* Clear DBM bit */
        	hadc->DMA_Handle->Instance->CR &= (uint32_t)(~DMA_SxCR_DBM);
        	hadc->DMA_Handle->Instance->NDTR = Length;
        	hadc->DMA_Handle->Instance->PAR = &hadc->Instance->DR;
        	hadc->DMA_Handle->Instance->M0AR = pData;
        	regs->IFCR = 0x3FU << hadc->DMA_Handle->StreamIndex;
        	hadc->DMA_Handle->Instance->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
        	__HAL_DMA_ENABLE(hadc->DMA_Handle);
        }
     hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
   }
   else
   {
     SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);
     SET_BIT(hadc->ErrorCode, HAL_ADC_ERROR_INTERNAL);
   }

 }


 void DMA_IRQHandler(DMA_HandleTypeDef *hdma)
 {
   uint32_t tmpisr;
   __IO uint32_t count = 0U;
   uint32_t timeout = SystemCoreClock / 9600U;

   /* calculate DMA base and stream number */
   DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;

   tmpisr = regs->ISR;

   /* Transfer Error Interrupt management ***************************************/
   if ((tmpisr & (DMA_FLAG_TEIF0_4 << hdma->StreamIndex)) != RESET)
   {
     if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TE) != RESET)
     {
       /* Disable the transfer error interrupt */
       hdma->Instance->CR  &= ~(DMA_IT_TE);

       /* Clear the transfer error flag */
       regs->IFCR = DMA_FLAG_TEIF0_4 << hdma->StreamIndex;

       /* Update error code */
       hdma->ErrorCode |= HAL_DMA_ERROR_TE;
     }
   }
   /* FIFO Error Interrupt management ******************************************/
   if ((tmpisr & (DMA_FLAG_FEIF0_4 << hdma->StreamIndex)) != RESET)
   {
     if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_FE) != RESET)
     {
       /* Clear the FIFO error flag */
       regs->IFCR = DMA_FLAG_FEIF0_4 << hdma->StreamIndex;

       /* Update error code */
       hdma->ErrorCode |= HAL_DMA_ERROR_FE;
     }
   }
   /* Direct Mode Error Interrupt management ***********************************/
   if ((tmpisr & (DMA_FLAG_DMEIF0_4 << hdma->StreamIndex)) != RESET)
   {
     if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_DME) != RESET)
     {
       /* Clear the direct mode error flag */
       regs->IFCR = DMA_FLAG_DMEIF0_4 << hdma->StreamIndex;

       /* Update error code */
       hdma->ErrorCode |= HAL_DMA_ERROR_DME;
     }
   }

   /* Transfer Complete Interrupt management ***********************************/
   if ((tmpisr & (DMA_FLAG_TCIF0_4 << hdma->StreamIndex)) != RESET)
   {
     if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TC) != RESET)
     {
       /* Clear the transfer complete flag */
       regs->IFCR = DMA_FLAG_TCIF0_4 << hdma->StreamIndex;

       if(HAL_DMA_STATE_ABORT == hdma->State)
       {
         /* Disable all the transfer interrupts */
         hdma->Instance->CR  &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME);
         hdma->Instance->FCR &= ~(DMA_IT_FE);

         /* Clear all interrupt flags at correct offset within the register */
         regs->IFCR = 0x3FU << hdma->StreamIndex;
         /* Change the DMA state */
         hdma->State = HAL_DMA_STATE_READY;
         return;
       }

       if(((hdma->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET)
       {
    	   ADC_DMAConvCplt(hdma);

       }
       /* Disable the transfer complete interrupt if the DMA mode is not CIRCULAR */
       else
       {
         if((hdma->Instance->CR & DMA_SxCR_CIRC) == RESET)
         {
           /* Disable the transfer complete interrupt */
           hdma->Instance->CR  &= ~(DMA_IT_TC);

           /* Change the DMA state */
           hdma->State = HAL_DMA_STATE_READY;
         }
         ADC_DMAConvCplt(hdma);

       }
     }
   }

   /* manage error case */
   if(hdma->ErrorCode != HAL_DMA_ERROR_NONE)
   {
     if((hdma->ErrorCode & HAL_DMA_ERROR_TE) != RESET)
     {
       hdma->State = HAL_DMA_STATE_ABORT;

       /* Disable the stream */
       __HAL_DMA_DISABLE(hdma);

       do
       {
         if (++count > timeout)
         {
           break;
         }
       }
       while((hdma->Instance->CR & DMA_SxCR_EN) != RESET);

       /* Change the DMA state */
       hdma->State = HAL_DMA_STATE_READY;

     }
     ADC_DMAConvCplt(hdma);
   }
 }
