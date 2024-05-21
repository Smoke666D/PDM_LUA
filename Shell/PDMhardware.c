#include "PDMhardware.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "system.h"
#include "dma_pdm.h"
#include "platform_init.h"
#include "ain.h"


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


static PDM_OUTPUT_TYPE out[OUT_COUNT]  			    __SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawCurData[OUT_COUNT]				__SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawOldOutCurData[OUT_COUNT]	    __SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawOldVData[AIN_NUMBER + 2]   		__SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawVData[AIN_NUMBER + 2]   		__SECTION(RAM_SECTION_CCMRAM);
static   EventGroupHandle_t pADCEvent 				__SECTION(RAM_SECTION_CCMRAM);
static   StaticEventGroup_t xADCCreatedEventGroup   __SECTION(RAM_SECTION_CCMRAM);
static EventGroupHandle_t  * pxPDMstatusEvent		__SECTION(RAM_SECTION_CCMRAM);


#ifdef PDM



static const KAL_DATA CurSensData[OUT_COUNT][KOOF_COUNT] ={   {{K002O20,V002O20},{K01O20,V01O20},{K10O20,V10O20},{K15O20,V15O20}},
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

static void vHWOutSet( OUT_NAME_TYPE out_name) ;
static void vHWOutInit(OUT_NAME_TYPE out_name, TIM_HandleTypeDef * ptim, uint32_t  uiChannel, GPIO_TypeDef* EnablePort, uint16_t EnablePin, GPIO_TypeDef* OutPort, uint16_t OutPin );
void vHWOutOFF( uint8_t ucChannel );



/*static uint32_t ulRestartTimer()
{
	uint32_t data = htim6.Instance->CNT;
	htim6.Instance->CNT = 0U;
	return ( data );
}*/
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
	vHWOutInit(OUT_1, &htim4, TIM_CHANNEL_3, Cs_Dis20_1_GPIO_Port,Cs_Dis20_1_Pin,OUT1_PORT ,OUT1_PIN );
	vHWOutInit(OUT_2, &htim4, TIM_CHANNEL_4, GPIOG,Cs_Dis20_2_Pin, OUT2_PORT ,OUT2_PIN  );
	vHWOutInit(OUT_3, &htim2, TIM_CHANNEL_1, GPIOG,Cs_Dis20_3_Pin, OUT3_PORT ,OUT3_PIN );
	vHWOutInit(OUT_4, &htim3, TIM_CHANNEL_2, GPIOG,Cs_Dis20_4_Pin, OUT4_PORT ,OUT4_PIN );
	vHWOutInit(OUT_5, &htim1, TIM_CHANNEL_3, GPIOG, Cs_Dis20_5_Pin,OUT5_PORT ,OUT5_PIN );
	vHWOutInit(OUT_6, &htim1, TIM_CHANNEL_4, GPIOE, Cs_Dis20_6_Pin,OUT6_PORT ,OUT6_PIN );
	vHWOutInit(OUT_7, &htim12, TIM_CHANNEL_1, GPIOD,Cs_Dis20_7_Pin, OUT7_PORT ,OUT7_PIN );
	vHWOutInit(OUT_8, &htim4, TIM_CHANNEL_2, GPIOD,Cs_Dis20_8_Pin ,OUT8_PORT ,OUT8_PIN  );
	vHWOutInit(OUT_9, &htim1, TIM_CHANNEL_1,  GPIOE, Cs_Dis8_9_10_Pin,  OUT9_PORT ,OUT9_PIN  );
	vHWOutInit(OUT_10, &htim1, TIM_CHANNEL_2, GPIOE, Cs_Dis8_9_10_Pin , OUT10_PORT ,OUT10_PIN );
	vHWOutInit(OUT_11, &htim2, TIM_CHANNEL_4, GPIOD, Cs_Dis8_11_12_Pin , OUT11_PORT ,OUT11_PIN );
	vHWOutInit(OUT_12, &htim2, TIM_CHANNEL_3, GPIOD, Cs_Dis8_11_12_Pin ,OUT12_PORT ,OUT12_PIN  );
	vHWOutInit(OUT_13, &htim8, TIM_CHANNEL_1,GPIOG,Cs_Dis8_13_14_Pin , OUT13_PORT ,OUT13_PIN  );
	vHWOutInit(OUT_14, &htim8, TIM_CHANNEL_2,GPIOG, Cs_Dis8_13_14_Pin , OUT14_PORT ,OUT14_PIN );
	vHWOutInit(OUT_15, &htim3, TIM_CHANNEL_1,GPIOG, Cs_Dis8_15_16_Pin , OUT15_PORT ,OUT15_PIN  );
	vHWOutInit(OUT_16, &htim2, TIM_CHANNEL_2, GPIOG, Cs_Dis8_15_16_Pin ,OUT16_PORT ,OUT16_PIN );
	vHWOutInit(OUT_17, &htim8, TIM_CHANNEL_3, GPIOG,  Cs_Dis8_17_18_Pin , OUT17_PORT ,OUT17_PIN );
	vHWOutInit(OUT_18,  &htim8, TIM_CHANNEL_4, GPIOG, Cs_Dis8_17_18_Pin , OUT18_PORT ,OUT18_PIN );
	vHWOutInit(OUT_19, &htim12, TIM_CHANNEL_2,GPIOD,Cs_Dis8_19_20_Pin , OUT19_PORT ,OUT19_PIN );
	vHWOutInit(OUT_20, &htim4, TIM_CHANNEL_1, GPIOD,Cs_Dis8_19_20_Pin,OUT20_PORT ,OUT20_PIN  );
	HAL_TIM_Base_Start_IT(&htim2);
	return;
}
/*
 *
 */

/*
 * Функция конфигурация номинальной мощности и режима перегрузки канала, с проверкой коректности парамертов
 */
ERROR_CODE vHWOutOverloadConfig(OUT_NAME_TYPE out_name,  float power, uint16_t overload_timer, float overload_power, OFF_STATE_TYPE off_state, uint8_t filter_enable)
{
	ERROR_CODE res = INVALID_ARG;
	if (out_name < OUT_COUNT)     //Проверяем корекность номера канала
	{
		RESET_FLAG(out_name,ENABLE_FLAG);
		if ( (overload_timer <= MAX_OVERLOAD_TIMER) && ( power > 0) && (overload_power > 0) &&
					(((power <= MAX_LPOWER) &&  (overload_power<= MAX_OVERLOAD_LPOWER)) ||
					((power <= MAX_HPOWER) &&  (overload_power<= MAX_OVERLOAD_HPOWER) && ( out_name < OUT_HPOWER_COUNT) ))
			   )
			{
				out[out_name].power = power;
				out[out_name].overload_config_timer = overload_timer;
				out[out_name].overload_power = overload_power;
				out[out_name].filter_enable = filter_enable;
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
		out[out_name].max_error_counter = restart_count;
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
	if ((out_name <  OUT_COUNT ) && (PWM <=MAX_PWM) && (out[out_name].PWM_Freg!=0)) //Проверяем коректность агрументов
	{
		if ( out[out_name].PWM != PWM )
		{
			out[out_name].PWM = PWM;
			TIM_OC_InitTypeDef sConfigOC = {0};
			sConfigOC.OCMode = TIM_OCMODE_PWM1;
			if (PWM < 100 )
			{
				sConfigOC.Pulse = (uint32_t )(  (out[out_name].ptim->Init.Period *(float)out[out_name].PWM/ MAX_PWM ) )+1U;
			}
			else
			{
				sConfigOC.Pulse = 1001;
			}
			sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
			sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
			HAL_TIM_PWM_Stop(out[out_name].ptim,out[out_name].channel);
			HAL_TIM_PWM_ConfigChannel(out[out_name].ptim, &sConfigOC, out[out_name].channel);
			if (   IS_FLAG_RESET(out_name, FSM_ERROR_STATE) &&   IS_FLAG_RESET(out_name, FSM_OFF_STATE) ) //Если выход вклчюен и не находится в каком-то переходном процессе
			{
				vHWOutSet( out_name );
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
	if ((out_name <  OUT_COUNT ) && (power < 100)) //Проверяем коректность агрументов
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
			break;
		case 1:
			SET_FLAG(out_name, CONTROL_ON_STATE);
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
	PDM_OUT_STATE_t state = STATE_OUT_OFF;

	if (eChNum < OUT_COUNT)
	{

		switch ( out[eChNum].SysReg & FSM_MASK )
		{
			case  FSM_OFF_STATE:
				state =  STATE_OUT_OFF;
				break;
			case FSM_ON_PROCESS:
				state = STATE_OUT_ON_PROCESS;
				break;
			case FSM_ON_STATE:
				state =   STATE_OUT_ON;
				break;
			case FSM_ERROR_STATE:
				state =   STATE_OUT_ERROR;
				break;
			case FSM_RESTART_STATE:
				state = STATE_OUT_RESTART_PROCESS;
				break;
			default:
				break;
		}
	}
	return (state);
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
		switch (out[i].SysReg & ERROR_MASK)
		{
			case OPEN_LOAD_ERROR:
				channel_state = 0x02;
				break;
			case  OVERLOAD_ERROR :
				channel_state = 0x04;
				break;
			default:
				if ( IS_FLAG_SET(i,  FSM_ON_PROCESS)  ||   IS_FLAG_SET(i,  FSM_ON_STATE ) )
				{
						channel_state = 0x01;
				}
				else
				{
						channel_state = 0x00;
				}
				break;
		  }
		if (i<10)
		{
			status1 |= channel_state<<(3*i);
		}
		else
		{
			status2 |= channel_state<<(3*(i-10));
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
	ERROR_FLAGS_TYPE error = ERROR_OFF;

   if (	eChNum < OUT_COUNT )
   {
	   switch (out[eChNum].SysReg & ERROR_MASK)
	   {
	   	   	   case OPEN_LOAD_ERROR:
	   	   		   error = ERROR_CIRCUT_BREAK;
	   	   		   break;
	   	   	   case OVERLOAD_ERROR:
	   	   		   error = ERROR_OVERLOAD;
	   	   		   break;
	   	   		default:
				   break;
	   }
   }
	return (  error );
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
#ifdef REV_2
  float res = fGetAinCalData( channel , (float) muRawVData[channel] *  AINCOOF2 );
#endif
#ifdef REV_1
  float res = fGetAinCalData( channel , (float) muRawVData[channel] *  AINCOOF1 );
#endif
 return res;// ( (channel < AIN_COUNT) ? (float) muRawVData[channel] *  AINCOOF1 : 0U ) ;
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

float fTemperatureGet (  )
{
	return ((float)muRawVData[4] * K - 0.76) /0.0025 + 25;
}
/*
 *
 */


void vPWMFreqSet( OUT_CH_GROUPE_TYPE groupe, uint32_t Freq)
{
	if ((Freq > 0) && (Freq < 6000))
	{
       switch (groupe)
       {
           case CH5_6_9_10:
               out[ OUT_5 ].ptim->Init.Prescaler =  168000 / Freq;
               HAL_TIM_Base_Init(out[ OUT_5 ].ptim);
               out[ OUT_5 ].PWM_Freg =Freq;
               out[ OUT_6 ].PWM_Freg =Freq;
               out[ OUT_9 ].PWM_Freg =Freq;
               out[ OUT_10 ].PWM_Freg =Freq;
               break;
           case CH11_12_16:
               out[ OUT_11 ].ptim->Init.Prescaler = 84000 / Freq;
               HAL_TIM_Base_Init(out[ OUT_11 ].ptim);
               out[ OUT_11 ].PWM_Freg =Freq;
               out[ OUT_12 ].PWM_Freg =Freq;
               out[ OUT_16 ].PWM_Freg =Freq;
               break;
           case CH4_15:
               out[ 3 ].ptim->Init.Prescaler = 84000 / Freq;
               HAL_TIM_Base_Init(out[ 3 ].ptim);
               out[ 3 ].PWM_Freg =Freq;
               out[ 14 ].PWM_Freg =Freq;
               break;
           case CH1_2_8_20:
               out[ 0 ].ptim->Init.Prescaler = 84000 / Freq;
               HAL_TIM_Base_Init(out[ 0 ].ptim);
               out[ 0 ].PWM_Freg =Freq;
               out[ 1 ].PWM_Freg =Freq;
               out[ 7 ].PWM_Freg =Freq;
               out[ 19 ].PWM_Freg =Freq;
               break;
           case CH13_14_17_18:
               out[ 12 ].ptim->Init.Prescaler = 168000 / Freq;
               HAL_TIM_Base_Init(out[ 12 ].ptim);
               out[ 12 ].PWM_Freg =Freq;
               out[ 13 ].PWM_Freg =Freq;
               out[ 16 ].PWM_Freg =Freq;
               out[ 17 ].PWM_Freg =Freq;
               break;
           case CH7_19:
               out[ 6 ].ptim->Init.Prescaler = 84000 / Freq;
               HAL_TIM_Base_Init(out[ 6 ].ptim);
               out[ 6 ].PWM_Freg =Freq;
               out[ 18 ].PWM_Freg =Freq;
               break;
           default:
               break;
       }
	}
   return;
}
void vHWOutEnable(OUT_NAME_TYPE out_name)
{
	if ( out_name < OUT_COUNT )
	{
		HAL_GPIO_WritePin(out[out_name].GPIOx, out[out_name].GPIO_Pin , CS_ENABLE);
	}
}
void vHWOutDisable(OUT_NAME_TYPE out_name)
{
	if ( out_name < OUT_COUNT )
	{
		HAL_GPIO_WritePin(out[out_name].GPIOx, out[out_name].GPIO_Pin , CS_DISABLE);
	}
}
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
		out[out_name].current 		   = 0.0;
		out[out_name].PWM_err_counter  = 0;
		out[out_name].POWER_SOFT 	   = 0;
		out[out_name].state 		   = 0;
		out[out_name].PWM_Freg         = 0;
		out[out_name].PWM              = 100;
		out[out_name].RanfomOverload   = 0;
		out[out_name].cooldown_coof    = 1;
		out[out_name].cooldown_timer   = 0;
		out[out_name].filter_enable    = 1;
		out[out_name].cool_down_flag   = 0;
		RESET_FLAG(out_name,CONTROL_FLAGS );
		SET_STATE_FLAG(out_name, FSM_OFF_STATE );
		if (out_name < OUT_HPOWER_COUNT)
		{
			vHWOutOverloadConfig(out_name, DEFAULT_HPOWER,DEFAULT_OVERLOAD_TIMER_HPOWER, DEFAULT_HPOWER_MAX, RESETTEBLE_STATE_AFTER_ERROR,1);
		}
		else
		{
			vHWOutOverloadConfig(out_name, DEFAULT_LPOWER,DEFAULT_OVERLOAD_TIMER_LPOWER, DEFAULT_LPOWER_MAX , RESETTEBLE_STATE_AFTER_ERROR,1);
		}
		vHWOutResetConfig(out_name,DEFAULT_RESET_COUNTER, DEFAULT_RESET_TIMER);
		vOutSetPWM(out_name, DEFAULT_PWM);
		RESET_FLAG(out_name,ENABLE_FLAG);
		RESET_FLAG(out_name, ERROR_MASK);

		for (j=0; j< KOOF_COUNT - 1 ; j++)
		{
			//Проверяем что хоты одно значение АЦП не равно нулю,что-то не словить делением на ноль.
			if ((CurSensData[out_name][j].Data != 0.0) || (CurSensData[out_name][j+1].Data != 0.0 ))
			{
			    float temp;
				float temp1;
                vABLineKoofFinde(&temp, &temp1,
                                CurSensData[out_name][j].Data, CurSensData[out_name][j+1].Data,
                                CurSensData[out_name][j].KOOF, CurSensData[out_name][j+1].KOOF);
				out[out_name].CSC[j].data = CurSensData[out_name][j+1].Data;
				out[out_name].CSC[j].k = temp;
				out[out_name].CSC[j].b = temp1;
			}
		}
		TIM_OC_InitTypeDef sConfigOC = {0U};
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = (uint32_t)( out[out_name].ptim ->Init.Period *(float)out[out_name].PWM/ MAX_PWM ) + 1;
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
static void vHWOutSet( OUT_NAME_TYPE out_name )
{
   //TIM_OC_InitTypeDef sConfigOC = {0};

    //sConfigOC.OCMode = TIM_OCMODE_PWM1;
	//sConfigOC.Pulse = (uint32_t )(  (out[out_name].ptim->Init.Period *(float)out[out_name].PWM/ MAX_PWM ) )+1U;
	//sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	//sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	//HAL_TIM_PWM_Stop(out[out_name].ptim,out[out_name].channel);
	//HAL_TIM_PWM_ConfigChannel(out[out_name].ptim, &sConfigOC, out[out_name].channel);
	//if ( out[out_name].PWM !=100)
	//{
		HAL_TIM_PWM_Start(out[out_name].ptim,out[out_name].channel);
//	}
//	else
//	{
//		HAL_GPIO_WritePin(out[out_name].OutGPIOx, out[out_name].OutGPIO_Pin,GPIO_PIN_SET);
//	}
   return;
}
#define A 220


 uint16_t vRCFilter( uint16_t input,uint16_t * old_output)
{

	volatile uint32_t new = input;
	volatile uint32_t old = *old_output;


	volatile uint16_t  output =  ( A * old + (256-A)*new )>>8;
	//*old_input = input;
	*old_output = output;
	return output;
}
/*
 * Функция вытаскивает из входного буфера Indata  (размером FrameSize*BufferSize) со смещением InIndex FrameSize отсчетов,
 * счетает среднее арефмитическое и записывает в буффер OutData со смещением OutIndex
 */
static void vGetAverDataFromRAW(uint16_t * InData, uint16_t *OutData, uint8_t InIndex, uint8_t OutIndex, uint8_t Size, uint16_t BufferSize)
{
	volatile uint32_t temp;
	for (uint8_t i=0; i<Size; i++ )
	{
		temp = 0;
		for (uint8_t j=0;j < ADC_FRAME_SIZE; j++ )
		{
		  temp += (InData[ InIndex + i + j * BufferSize ]);
		}
		OutData[ OutIndex + i ] = temp / ADC_FRAME_SIZE;


	}
	return;
}
/*
*/
void vHWOutOFF( uint8_t ucChannel )
{
	HAL_TIM_PWM_Stop(out[ucChannel].ptim,  out[ucChannel].channel);
//	HAL_GPIO_WritePin(out[ucChannel].OutGPIOx, out[ucChannel].OutGPIO_Pin,GPIO_PIN_RESET);
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


void vSetRendomResetState( uint8_t out_name,  uint8_t state, uint8_t cool_down)
{
	if ( out_name < OUT_COUNT )
	{
		out[out_name].RanfomOverload = (state!=0) ? 1: 0;
		out[out_name].cooldown_coof = (cool_down!=0)? cool_down : 1;
		out[out_name].cooldown_timer   = 0;
	}
}
/*
 *  Функция усредняет данные из буфеера АЦП, и пробразует их значения
 */


static void vDataConvertToFloat( void)
{
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
	// for (int i =0;i<OUT_COUNT;i++)
	// {
	//	 muRawCurData[i] = vRCFilter(muRawCurData[i],&muRawOldOutCurData[i]);

	// }
	 for (int i = 0; i<AIN_NUMBER + 2;i++ )
	 {
		 muRawVData[i] = vRCFilter(  muRawVData[i] ,&muRawOldVData[i]);
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
   xEventGroupSetBitsFromISR( pADCEvent, adc_number, &xHigherPriorityTaskWoken );
   portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
   return;
 }
/*
 *
 */
 static void vGotoRestartState( uint8_t ucChannel, float fCurr )
 {
	 if (out[ ucChannel ].error_counter == 1)
	 {
		 SET_STATE_FLAG( ucChannel, FSM_ERROR_STATE );
		 RESET_FLAG(ucChannel,CONTROL_FLAGS );
	 }
	 else
	 {
		 SET_STATE_FLAG( ucChannel, FSM_RESTART_STATE);
	 }
	 if  ( IS_FLAG_SET(ucChannel, FSM_ERROR_STATE ) &&  IS_FLAG_RESET( ucChannel, RESETTEBLE_FLAG ) )
	 {
		 RESET_FLAG(ucChannel,ENABLE_FLAG);
     }
	 SET_ERROR_FLAG( ucChannel, OVERLOAD_ERROR);
	 out[ ucChannel ].restart_timer = 0U;
	 vHWOutOFF(ucChannel);
	 if ( fCurr < ( ucChannel < OUT_HPOWER_COUNT ? MAX_OVERLOAD_HPOWER : MAX_OVERLOAD_LPOWER ) )
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
    		if (out[i].filter_enable )
    		{
    			muRawCurData[i] = vRCFilter(muRawCurData[i],&muRawOldOutCurData[i]);
    		}
    	    float fCurrent  = fGetDataFromRaw( ((float) muRawCurData [ i ] *K ) , out[i] );
    	 //   fCurrent = vRCFilter(fCurrent);

    	    if ( ( fCurrent > out[ i ].power) && (out[i].PWM != 100) )
    	    {
    	        if  (out[i].state == GPIO_PIN_RESET)
    	     	{
    	        	 fCurrent = out[i].current;
    	        	 out[i].PWM_err_counter = 0;
    	     	}
    	        else
    	        {
    	        	if  (out[i].PWM_err_counter < 2 )
    	        	 {
    	        		 	  fCurrent = out[i].current;
    	        	 }
    	       	     out[i].PWM_err_counter ++;
    	        }
    	    }
    	    switch (out[i].SysReg & FSM_MASK )
 			{
 				case FSM_OFF_STATE : //Состония входа - выключен
 					out[i].current 	   		 = 0U;
 					out[i].restart_timer   	 = 0U;
 					RESET_FLAG(i,ERROR_MASK);
 					out[i].cool_down_flag   = 0;
 					out[ i ].cooldown_timer = 0;
 					break;
 				case FSM_ON_PROCESS: //Состояния влючения

 					out[i].restart_timer++;
 					uint8_t ucCurrentPower;
 					if ((out[i].soft_start_timer !=0) && (out[i].PWM_Freg != 0))
 					{
 						if  ( fCurrent  > out[i].overload_power)
 						{
 							vGotoRestartState(i,fCurrent);
 							break;
 						}
 						if  ( out[i].restart_timer >= out[i].soft_start_timer ) //Если прошло время полонго пуска
 						{
 							  SET_STATE_FLAG(i, FSM_ON_STATE );
 							  ucCurrentPower = MAX_POWER;
 						}
 						else
 						 {   //время пуска не прошоло, вычисляем текущую мощность, котору надо пдать на выход.
 						 	    ucCurrentPower = out[i].soft_start_power + (uint8_t) (((float)out[i].restart_timer/(float)out[i].soft_start_timer)*(MAX_POWER - out[i].soft_start_power));
 						 		if (ucCurrentPower  >= MAX_POWER)
 						 		{
 						 			ucCurrentPower = MAX_POWER;
 						 		}
 						 }
 						vOutSetPWM(i, ucCurrentPower );
 					}
 					else
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
 						if  (fCurrent  > out[ i ].power  )
 						{
 							out[ i ].cooldown_timer = 0;
 							out[ i ].cool_down_flag = 0;
 						}
 						 if ( out[ i ].restart_timer >= out[ i ].overload_config_timer )
 						 {
 							SET_STATE_FLAG(i, FSM_ON_STATE );
 							 out[ i ].restart_timer = 0;
 						 }
 					}
 					out[i].current = fCurrent;
 					break;
 				case FSM_ON_STATE:  // Состояние входа - включен
 					if  (fCurrent  > out[ i ].power  )
 					{
 						if ((out[i].RanfomOverload) &&  (out[i].cool_down_flag   == 1))
 						{
 								out[ i ].cooldown_timer = 0;
 								out[i].restart_timer++;
 								if ( out[i].restart_timer  < 2 )  break;
 								if  (( fCurrent  > out[ i ].overload_power ) ||   ( out[ i ].restart_timer >= out[ i ].overload_config_timer ))
 								{
 									vGotoRestartState(i,fCurrent);
 									break;
 								}
 						}
 						else
 						{
 							vGotoRestartState( i, fCurrent );
 							break;
 						}
 					}
 					else
 					{
 						out[ i ].restart_timer = 0;

 						if ( out[ i ].cooldown_timer <= ( out[ i ].overload_config_timer * out[ i ].cooldown_coof) )
 						{
 							 out[i].cool_down_flag   = 0;
 							 out[ i ].cooldown_timer++;
 						}
 						else
 							out[i].cool_down_flag   = 1;
 					}
 					RESET_FLAG(i,ERROR_MASK);
 					if (fCurrent  < CIRCUT_BREAK_CURRENT)
 					{
 						SET_ERROR_FLAG(i,OPEN_LOAD_ERROR);
 					}

 					out[i].current = fCurrent;
 					break;
 				case FSM_RESTART_STATE:
 					out[ i ].restart_timer++;
 					if  ( out[ i ].restart_timer >= out[ i ].restart_config_timer )
 					{
 						SET_STATE_FLAG(i, FSM_ON_PROCESS );
 						out[i].cool_down_flag   = 0;
 						out[ i ].cooldown_timer  = 0;
 						vHWOutSet( i );
 						out[ i ].restart_timer =0;
 						RESET_FLAG(i,ERROR_MASK);
 						if ( out[i].error_counter !=0 )
 						{
 							out[i].error_counter--;
 						}
 					}
 					break;
 				case FSM_ERROR_STATE:
 					if (IS_FLAG_SET(i,RESETTEBLE_FLAG) && IS_FLAG_SET(i, CONTROL_OFF_STATE))
 					{
 						RESET_FLAG(i, OVERLOAD_ERROR);
 						SET_STATE_FLAG(i, FSM_OFF_STATE);
 					}
 					break;
 				default:
 					break;
 			}
 			// Проверям управляющие сигналы. Если они изменилсь, то выключем или включаем каналы. Это нужно сделать именно тот,
 			// чтобы на следующем циклые конечного автомата были актуальные данные о состонии каналов


   		 }
 	}
    for (uint8_t i = 0U; i < OUT_COUNT; i++ )
    {
       	if (IS_FLAG_SET(i,ENABLE_FLAG) )		/*Если канал не выключен или не в режиме конфигурации*/
       	{
       		if ( IS_FLAG_SET( i, CONTROL_OFF_STATE ) && IS_FLAG_RESET(i, FSM_OFF_STATE)   )
       		{
       				SET_STATE_FLAG(i, FSM_OFF_STATE );
       				out[i].error_counter =  out[i].max_error_counter;
       		 	 	vHWOutOFF(i);
       		 }
       		 if ( IS_FLAG_SET( i, CONTROL_ON_STATE ) &&  IS_FLAG_SET(i, FSM_OFF_STATE) )
       		 {
       		 		SET_STATE_FLAG(i, FSM_ON_PROCESS );
       		 	 	vHWOutSet( i );
       		 }
       		 RESET_FLAG(i,CONTROL_FLAGS );
       		 out[i].state = HAL_GPIO_ReadPin(out[i].OutGPIOx, out[i].OutGPIO_Pin);
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
   pxPDMstatusEvent = osLUAetPDMstatusHandle();
   TickType_t xLastWakeTime;
   const TickType_t xPeriod = pdMS_TO_TICKS( 1 );
   xLastWakeTime = xTaskGetTickCount();

  // HAL_TIM_Base_Start(&htim6);
   for (int i = 0; i< OUT_COUNT;i++)
   {

	   muRawOldOutCurData[i] = 0;
   }
   for (int i=0; i< AIN_NUMBER + 2;i++ )
   {
	   muRawOldVData[i] = 0;
   }
   for(;;)
   {
	   WDT_Reset();
	   vTaskDelayUntil( &xLastWakeTime, xPeriod );
	   xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
	  // ulRestartTimer();
	   ADC_Start_DMA( &hadc1,( uint32_t* )&ADC1_IN_Buffer, ( ADC_FRAME_SIZE * ADC1_CHANNELS ));
	   ADC_Start_DMA( &hadc2,( uint32_t* )&ADC2_IN_Buffer, ( ADC_FRAME_SIZE * ADC2_CHANNELS ));
	   ADC_Start_DMA( &hadc3,( uint32_t* )&ADC3_IN_Buffer, ( ADC_FRAME_SIZE * ADC3_CHANNELS ));
	   xEventGroupWaitBits( pADCEvent, ( ADC3_READY  | ADC2_READY | ADC1_READY   ), pdTRUE, pdTRUE, 500 );
	   ADC_STOP();
	   vDataConvertToFloat();
	   vOutControlFSM();
	   vADCEnable();
	    /* Влючаем АЦП, исходя из времени выполнения следующей функции,
	   к моменту ее завершения, АЦП уже включаться*/

   }
   /* USER CODE END vADCTask */
 }











