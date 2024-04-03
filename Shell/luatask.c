/*
 * luatask.c
 *
 *  Created on: Apr 6, 2022
 *      Author: igor.dymov
 */
#include "luatask.h"
#include "lprefix.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lapi.h"
#include "PDMhardware.h"
#include "cantask.h"
#include "pdm_input.h"
#include "flash.h"
#include "string.h"
#include "CO_driver_ST32F4xx.h"
#include "datastorage.h"
#include "pdm_math.h"
#include "mems.h"
#include "ain.h"


extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim11;
static LUA_STATE_t state 					__SECTION(RAM_SECTION_CCMRAM) = 0U;
uint32_t ulWorkCicleIn10us					__SECTION(RAM_SECTION_CCMRAM) = 0U;
static uint8_t ucErrorCount 				__SECTION(RAM_SECTION_CCMRAM)= 0U;
static ENABLE_t eSafeModeIsEnable 			__SECTION(RAM_SECTION_CCMRAM)= IS_DISABLE;
static ENABLE_t eMainLoopIsEnable 			__SECTION(RAM_SECTION_CCMRAM)= IS_DISABLE;

static EventGroupHandle_t xPDMstatusEvent 	__SECTION(RAM_SECTION_CCMRAM);
static uint8_t RestartFlag  = 0;
char * pcLuaErrorString 				    __SECTION(RAM_SECTION_CCMRAM) = NULL;
int res 									__SECTION(RAM_SECTION_CCMRAM) = 0;
float dd = 0;
/*
 * Переменные, размещенные в секции CCMRAM не инициализиурются при объявлении,
 * для коректности работы необходима явная инициалиазция
 */
static void vCCMRAVarInir()
{
	state 				= LUA_INIT;
    ucErrorCount 	  	= 0U;
    ulWorkCicleIn10us 	= 0U;
    eSafeModeIsEnable 	= IS_DISABLE;
    eMainLoopIsEnable 	= IS_DISABLE;
    pcLuaErrorString  	= NULL;
    res 				= 0;
	return;
}
/*
 *
 */
static uint32_t ulRestartTimer()
{
	uint32_t data = htim11.Instance->CNT;
	htim11.Instance->CNT = 0U;
	return ( data );
}
/*
 *
 */
static void vSafeModeOutState()
{
	 for (uint8_t i = 0; i<OUT_COUNT; i++ )
	 {
		 vHWOutOFF( i );
		 vHWOutDisable( i );
     }
	 return;
}
/*---------------------------------------------------------------------------------------------------*/
EventGroupHandle_t* osLUAetPDMstatusHandle ( void )
{
  return ( &xPDMstatusEvent );
}
/*
 *
 */
char * pcGetLUAError()
{
	return ( pcLuaErrorString );
}
/*
 *
 */
void vLUArunPDM()
{
	xEventGroupSetBits(xPDMstatusEvent,RUN_STATE);
	state = LUA_RUN;
	return;
}
/*
 *
 */
void vLUAstopPDM()
{
	xEventGroupClearBits(xPDMstatusEvent,RUN_STATE);
	state = LUA_STOP;
	return;
}
/*
 *
 */
void vLUArestartPDM()
{
	state = LUA_RESTART;
	return;
}



/*
 *
 */
static int iSysrestart(lua_State *L)
{
	RestartFlag = 1;
	return ( NO_RESULT );
}
/*
 *
 */
static int isetAINCal (lua_State *L)
{
   uint16_t argument = lua_gettop(L);
   int res = 0;
   POINT_t points[2];
   uint8_t ucNumber =(uint8_t) lua_tointeger( L, FIRST_ARGUMENT);
   if ( ( argument >= FIVE_ARGUMENT ) && ( (argument -1)%2 == 0 ))
   {
       if ( eAinCalDataConfig(ucNumber -1 , (argument -1)/2 )== CAL_SUCCESS )
       {
           for (uint16_t i = 0; i < argument ; i = i + 2)
           {
               points[0].X =  lua_tonumber( L, i + 2 );
               points[0].Y =  lua_tonumber( L, i + 3);
               points[1].X =  lua_tonumber( L, i + 4);
               points[1].Y =  lua_tonumber( L, i + 5);
               eSetAinCalPoint(ucNumber-1,&points[0], i/2 );
           }
           res = 1;
       }
   }
   lua_pushboolean( L, res );
   return (ONE_RESULT);


}


static int iSetPID(lua_State *L)
{
	if (lua_gettop(L) == FOUR_ARGUMENTS)
	{
		        uint8_t ucNumber =(uint8_t) lua_tointeger( L, FIRST_ARGUMENT); //First argument it's channel number
		        float Kp = (float) lua_tonumber( L, SECOND_ARGUMENT);
		        float Ki = (float) lua_tonumber( L, THIRD_ARGUMENT);
		        float Kd = (float) lua_tonumber( L, FOURTH_ARGUMENT);
		        vInitPID(ucNumber - 1U , Kp,Ki,Kd);
	}
	return ( NO_RESULT );

}

/*
 * Функция управленя признаком RandomReset
 */
static int iSetRandomRestart(lua_State *L)
{
	if (lua_gettop(L) ==  THREE_ARGUMENTS)
	{
		uint8_t ucNumber =(uint8_t) lua_tointeger( L, FIRST_ARGUMENT); //First argument it's channel number
		uint8_t ucState =(uint8_t) lua_tointeger( L, SECOND_ARGUMENT);
		uint8_t ucCoolDownTime =(uint8_t) lua_tointeger( L,  THIRD_ARGUMENT);
		vSetRendomResetState( ucNumber -1u, ucState, ucCoolDownTime );
	}
	return ( NO_RESULT );

}

static int iResetPID(lua_State *L)
{
	if (lua_gettop(L) == ONE_ARGUMENT)
	{
			uint8_t ucNumber =(uint8_t) lua_tointeger( L, FIRST_ARGUMENT); //First argument it's channel number
			vPidReset(ucNumber - 1U );
	}
	return ( NO_RESULT );

}
/*
 *
 */

static int iProcessPID(lua_State *L)
{
	uint8_t res =  NO_RESULT;
	if (lua_gettop(L) == TWO_ARGUMENTS)
	{
				uint8_t ucNumber =(uint8_t) lua_tointeger( L, FIRST_ARGUMENT); //First argument it's channel number
				dd = vProcessPID(ucNumber- 1U, lua_tonumber( L, SECOND_ARGUMENT));
				float fOutData = dd;
				lua_pushnumber(L, fOutData);
				res = ONE_RESULT;
	}
	return ( res );
}
/*
 *
 */
static int isetPWMFreq(lua_State *L)
{
	if (lua_gettop(L) == TWO_ARGUMENTS)
	{
		uint8_t groupe = (uint8_t) lua_tointeger( L, FIRST_ARGUMENT);
		if  (groupe <= 5)
		{
			vPWMFreqSet( groupe , (uint32_t) lua_tointeger( L, SECOND_ARGUMENT));
		}
	}
	return ( NO_RESULT );
}
/*
 *  Setting Can parameter API function
 */
static int iCanSetConfig(lua_State *L)
{
	if (lua_gettop(L) == TWO_ARGUMENTS)
	{
		uint8_t ucCanNumber =(uint8_t) lua_tointeger( L, FIRST_ARGUMENT); //First argument it's channel number
		vCANBoudInit( (uint16_t)lua_tointeger( L, SECOND_ARGUMENT) );
	}
	return ( NO_RESULT );
}
/*
 * Setting discrete input configuration API function
 */
static int iDinConfig(lua_State *L )
{
	int arg_number = lua_gettop(L);
	if (arg_number >= TWO_ARGUMENTS )  /*Check function argument count*/
	{
		eDinConfig( (uint8_t) lua_tointeger( L, FIRST_ARGUMENT ) -1U ,
				    (uint8_t) lua_tonumber( L, SECOND_ARGUMENT ) ,
					( arg_number == FOUR_ARGUMENTS ) ? ( uint32_t) lua_tointeger( L, FOURTH_ARGUMENT) :DEF_H_FRONT,
					( arg_number == FOUR_ARGUMENTS ) ? ( uint32_t) lua_tointeger( L, THIRD_ARGUMENT) : DEF_L_FRONT );
	}
	return ( NO_RESULT );
}


static int iSetRPMConfig(lua_State *L )
{
	int arg_number = lua_gettop(L);
	if (arg_number >= TWO_ARGUMENTS )
	{
		float coof = ( arg_number >= THIRD_ARGUMENT) ? lua_tonumber( L, THIRD_ARGUMENT ) : 0;
		vSetRPMConfig((uint8_t) lua_tointeger( L, FIRST_ARGUMENT ) -1U, lua_tonumber( L, SECOND_ARGUMENT ), coof);
	}

	return ( NO_RESULT );
}
/*
 * Устанавливаем новый фильтр, позволяющий принимать пакеты с нужным ID
 */
int iCanSetResiveFilter(lua_State *L )
{
  uint8_t ucResNumber = NO_RESULT;
  if (lua_gettop(L) == 1U )  /*Проверяем, что при вызове нам передали нужное число аргументов*/
  {
	  lua_pushnumber(L, eMailboxFilterSet( ( uint32_t ) lua_tointeger(L,-1))== BUFFER_FULL ? 1U : 0U );
	  ucResNumber = ONE_RESULT;
  }
  return ( ucResNumber );
}

/*
 * Проверка пакета с нужным id
 */
int iCanCheckData(lua_State *L )
{
	uint32_t uiRes = 0U;
	switch (lua_gettop(L))
	{
	  case 0U:
		  uiRes = vCheckAnswer();
		  break;
	  case 1U:
		  uiRes = vCanChekMessage( lua_tointeger(L, CAN_ID_POS) );
		  break;
	  default:
		  break;
	}
	lua_pushnumber(L, uiRes );
	return ( 1U );
}

int iCanGetMessage(lua_State *L )
{
	uint8_t n = 0;
	CAN_FRAME_TYPE  RXPacket;
	switch (lua_gettop(L))
	{
		case 0:
			n = vCanGetRequest(&RXPacket);
			break;
		case 1:
			luaL_checktype(L, 1, LUA_TNUMBER);
			RXPacket.ident = (uint32_t) lua_tointeger(L,1);
			n= vCanGetMessage(&RXPacket);
			break;
		default:
			break;
	}
	if (n!=0)
	{
		n = RXPacket.DLC;
		for (int i = 0; i <n;i++)
		{
			lua_pushnumber(L,RXPacket.data[i]);
		}
	}
	return ( n );
}

int iCanGetResivedData(lua_State *L )
{
	uint8_t n;
	CAN_FRAME_TYPE  RXPacket;
	switch (lua_gettop(L))
	{
	  case 1:
	    luaL_checktype(L, 1, LUA_TTABLE);
	    if  (vCanGetRequest(&RXPacket)!=0)
	    {
	      n = luaL_len(L, 1);
	    	for (int i = 1; i<(n+1);i++)
	    	{
	        lua_pushnumber(L,RXPacket.data[i-1]);
	      	lua_seti(L, 1, i);
	      }
	      lua_pushnumber(L,1U );
	    }
	    break;
	  case 2:
	    luaL_checktype(L, -1, LUA_TTABLE);
	    RXPacket.ident = (uint32_t) lua_tointeger(L,-2);
	    if ( vCanGetMessage(&RXPacket) == 1)
	    {
	      n = luaL_len(L, -1);
	    	for (int i = 1; i<(n+1);i++)
	    	{
	    	  lua_pushnumber(L,RXPacket.data[i-1]);
	    		lua_seti(L, -2, i);
	      }
	    	lua_pushnumber(L,1U );
	    }
	    else
	    {
	      lua_pushnumber(L,0U );
	    }
	    break;
	  default:
		  break;
	}
	return ( 1U );
}

/*
 *
 */
int iCanSendPDM( lua_State *L )
{
	uint8_t  DATA[CAN_FRAME_SIZE] = {0,0,0,0,0,0,0,0};
	uint8_t  size = lua_gettop(L);
	if (size >= TWO_ARGUMENTS)
	{
		size--;
		for (int i=0;i< size ;i++)
		{
			DATA[i]= (uint8_t) lua_tointeger(L,-( size-i)); /*Третьем агрументом должно передоватьс время плавного старта в милисекундах*/
		}
		vCanInsertTXData( (uint32_t)lua_tointeger(L, FIRST_ARGUMENT) , &DATA[0], 8);
	}
	return ( NO_RESULT );
}


/*
 *
 */
int iCanSendTable( lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint8_t DATA[ CAN_FRAME_SIZE ];
	if (arg_number == CAN_SEND_TABLE_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		if ( lua_istable(L, CAN_TABLE_POS ) )
		{
			uint8_t size 	= (uint8_t) lua_tointeger(L, FRAME_SIZE_POS);
			if  (size <= CAN_FRAME_SIZE )
			{
				for (uint8_t i = 0; i < size; i++)
				{
					lua_geti(L, CAN_TABLE_POS , i + 1);
					DATA[i]= lua_tointeger(L,-1);
					lua_pop(L,1);
				}
				vCanInsertTXData((uint32_t) lua_tointeger(L, FIRST_ARGUMENT ), &DATA[0], size);
			}
		}
	}
	return (  NO_RESULT );
}
/*
 *
 */
int iCanSendRequest( lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint8_t DATA[CAN_FRAME_SIZE];
	if (arg_number >= SEND_REQUEST_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		vSetWaitFilter( lua_tointeger( L, SECOND_ARGUMENT ) );//Второй агрумент - id ответного пакета
		uint8_t size  = arg_number -2;
		for (int i=0;i<size;i++)
		{
			DATA[i] = (uint8_t) lua_tointeger(L,-(arg_number-2-i)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
		}
		vCanInsertTXData( (uint32_t) lua_tointeger( L, FIRST_ARGUMENT ), &DATA[0], size);
	}
	return ( NO_RESULT );
}
/*
 *
 */
int  iOutConfig( lua_State *L )
{
	if ( lua_gettop(L) >= SECOND_ARGUMENT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		uint8_t out_number 		= ( uint8_t ) lua_tointeger( L, FIRST_ARGUMENT ) - 1U ; //Первым аргументом дожен передоваться номер канала
		float nominal_power     = ( float ) lua_tonumber(L, SECOND_ARGUMENT );										   // Номинальая мощность
		uint8_t reset_satate    = (lua_gettop(L) >= THIRD_ARGUMENT) ? ( uint8_t ) lua_tointeger(L, THIRD_ARGUMENT) : 1; // режим сброс. Можно ли перезапускать канал выключением
		uint16_t overload_time  = (lua_gettop(L) >= FOURTH_ARGUMENT) ? ( uint16_t ) lua_tointeger(L, FOURTH_ARGUMENT) : 0;
		float overlad_power     = (lua_gettop(L) >= FIVE_ARGUMENT) ? ( float ) lua_tonumber(L, FIVE_ARGUMENT ) : nominal_power; //Пусковой ток
		uint8_t filter_enabele  =  (lua_gettop(L) >= SIX_ARGUMENT) ? ( float ) lua_tonumber(L, SIX_ARGUMENT ) : 1;
		vHWOutOverloadConfig( out_number, nominal_power, overload_time  , overlad_power, reset_satate , filter_enabele);
	    vHWOutEnable(out_number);
	}
	return ( NO_RESULT );
}

/*
 *
 */
int iSoftStart( lua_State *L )
{
	if ( lua_gettop(L) >= THREE_ARGUMENTS)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		uint8_t out_number 		= ( uint8_t ) lua_tointeger( L, FIRST_ARGUMENT ) - 1U ; //Первым аргументом дожен передоваться номер канала
		vOutSetSoftStart( out_number,
						( uint16_t ) lua_tonumber(L, SECOND_ARGUMENT ),
						( uint16_t ) lua_tointeger(L, THIRD_ARGUMENT));
	}
	return ( NO_RESULT );

}
/*
 *
 */
int  iOutResetConfig( lua_State *L )
{
	if (lua_gettop(L) >= THREE_ARGUMENTS )  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		vHWOutResetConfig(( uint8_t ) lua_tointeger(L, FIRST_ARGUMENT ) -1U,
						  ( uint8_t ) lua_tointeger(L, SECOND_ARGUMENT ),
						  ( uint16_t) lua_tointeger(L, THIRD_ARGUMENT ));
	}
	return ( NO_RESULT );
}
/*
 *
 */
int  iOutSetPWM( lua_State *L )
{
	if (lua_gettop(L) == TWO_ARGUMENTS )  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		vOutSetPWM( ( uint8_t ) lua_tointeger(L, FIRST_ARGUMENT ) -1U,
					( uint8_t) lua_tointeger( L, SECOND_ARGUMENT )
					);
	}
	return ( NO_RESULT );
}
/*
 * Функция записи в EEPROM
 */
static int iSetEEPROM( lua_State *L )
{
	uint32_t res = ERROR;
	PDM_DATA_TIME temp_time;
	uint16_t adr  = lua_tointeger( L , FIRST_ARGUMENT );
	switch (lua_gettop(L))
	{
		case TWO_ARGUMENTS:
			switch (lua_type(  L, SECOND_ARGUMENT )  )
			{
				case LUA_TNUMBER:
					if ( lua_isinteger( L, SECOND_ARGUMENT ) )
					{
						uint32_t temp_int = lua_tointeger( L, SECOND_ARGUMENT );
						res = (eEEPROMRegTypeWrite( adr, (uint8_t *)&temp_int, INTEGER_DATA ) == EEPROM_OK )? SUCSESS : ERROR;
					}
					else
					{
						float temp_float = lua_tonumber( L, SECOND_ARGUMENT);
						res = (eEEPROMRegTypeWrite( adr,(uint8_t *)&temp_float, NUMBER_DATA ) == EEPROM_OK )? SUCSESS : ERROR;
					}
					break;
				case LUA_TBOOLEAN:
					uint32_t temp_bool = lua_toboolean( L, SECOND_ARGUMENT );
					res = (eEEPROMRegTypeWrite( adr,(uint8_t *)&temp_bool, BOOLEAN_DATA ) == EEPROM_OK )? SUCSESS : ERROR;
					break;
				default:
					break;
			}
			break;
		case ONE_ARGUMENT:
			RTC_TimeTypeDef time_buffer;
			RTC_DateTypeDef date_buffer;
			HAL_RTC_GetTime(&hrtc, &time_buffer,  RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &date_buffer, RTC_FORMAT_BIN);
			temp_time.Day 	 = date_buffer.Date;
			temp_time.Month  = date_buffer.Month;
			temp_time.Year 	 = date_buffer.Year;
			temp_time.Hour 	 = time_buffer.Hours;
			temp_time.Minute = time_buffer.Minutes;
			temp_time.Second = time_buffer.Seconds;
			res = (eEEPROMRegTypeWrite( adr,&temp_time, TIME_STAMP) == EEPROM_OK )? SUCSESS : ERROR;
			break;
		case SEVEN_ARGUMENT:
			temp_time.Day    = lua_tointeger( L, SECOND_ARGUMENT );
			temp_time.Month  = lua_tointeger( L, THIRD_ARGUMENT );
			temp_time.Year   = lua_tointeger( L, FOURTH_ARGUMENT );
			temp_time.Hour 	 = lua_tointeger( L, FIVE_ARGUMENT	);
			temp_time.Minute = lua_tointeger( L, SIX_ARGUMENT );
			temp_time.Second = lua_tointeger( L,  SEVEN_ARGUMENT  );
			res = (eEEPROMRegTypeWrite( adr,&temp_time, TIME_STAMP) == EEPROM_OK )? SUCSESS : ERROR;
			break;
		default: break;
	}
	lua_pushnumber(L, res );
	return ( ONE_RESULT );
}

/*
 *
 */
static int iGetEEPROM( lua_State *L )
{
	uint8_t res = ERROR;
	if ( lua_gettop( L ) == ONE_ARGUMENT )
	{
		uint16_t adr = (uint16_t) lua_tointeger( L, FIRST_ARGUMENT );
		uint8_t d[REGISTER_SIZE];
		int idata;
		res = SUCSESS;
		switch ( eEEPROMReadRegister(adr, d) )
		{
				case INTEGER_DATA:
					    idata = * ((int *)d);
						lua_pushinteger( L, idata );
					break;
					case BOOLEAN_DATA:
						idata = * ((int *)d);
						lua_pushboolean( L, idata);
						break;
					case NUMBER_DATA:
						float fdata = * ((float *)d) ;
						lua_pushnumber( L, fdata);
						break;
					case TIME_STAMP:
						PDM_DATA_TIME data;
						vGetRegToTime( d,  &data);
						lua_pushnumber( L, data.Day);
						lua_pushnumber( L, data.Month);
						lua_pushnumber( L, data.Year);
						lua_pushnumber( L, data.Hour);
						lua_pushnumber( L, data.Minute);
						lua_pushnumber( L, data.Second);
						return (SEVEN_RESULT );
						break;
					default:
						res = ERROR;
						break;
		}
	}
	if (res == ERROR)  lua_pushnumber( L, NO_RESULT );
	lua_pushnumber( L, res);
	return ( TWO_RESULT );
}
/*
 *
 */

/*
 * Функция установки из LUA времени и даты
 */
static int iSetTime( lua_State *L )
{
	RTC_TimeTypeDef time_buffer;
	RTC_DateTypeDef date_buffer;
    if (lua_gettop(L) == SIX_ARGUMENT )
	{
    	time_buffer.Hours   = lua_tointeger( L , FIRST_ARGUMENT );
	    time_buffer.Minutes = lua_tointeger( L , SECOND_ARGUMENT  );
		time_buffer.Seconds = lua_tointeger( L , THIRD_ARGUMENT  );
		date_buffer.Date = lua_tointeger( L ,FOURTH_ARGUMENT  );
		date_buffer.Month = lua_tointeger( L ,FIVE_ARGUMENT);
		date_buffer.Year = lua_tointeger( L ,SIX_ARGUMENT );
		HAL_RTC_SetTime(&hrtc,&time_buffer,  RTC_FORMAT_BIN);
		HAL_RTC_SetDate(&hrtc, &date_buffer, RTC_FORMAT_BIN);
	}
	return ( NO_RESULT );
}
/*
 * Функция чтения из LUA времени и даты
 */
static int iGetTime( lua_State *L )
{
	RTC_TimeTypeDef time_buffer;
	RTC_DateTypeDef date_buffer;
	HAL_RTC_GetTime(&hrtc,&time_buffer,  RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date_buffer, RTC_FORMAT_BIN);
	lua_pushinteger( L, time_buffer.Hours );
	lua_pushinteger( L, time_buffer.Minutes );
	lua_pushinteger( L,	time_buffer.Seconds );
	lua_pushinteger( L,	date_buffer.Date  );
	lua_pushinteger( L,	date_buffer.Month );
	lua_pushinteger( L,	date_buffer.Year );
	return (  SIX_ARGUMENT );
}

static int iSetRecord( lua_State *L )
{
	   int start_index = 0;
       if (eGetReocrdFieldsType( 0 ) == RECORD_TIME_STAMP)
       {
    	   vSetRecordData(0,0);
    	   start_index ++;
       }
	   for (uint8_t i = 0; i < lua_gettop( L ) ; i++ )
	   {
			   switch ( eGetReocrdFieldsType( i + start_index ) )
			   {
			      case RECORD_ERROR:
			      case RECORD_TIME_STAMP:
			    	  break;
		   	   	  case RECORD_BYTE:
		   	      	  uint8_t cd =lua_tointeger( L, i + 1 );
		   	      	  vSetRecordData(i + start_index,&cd);
		   	   		  break;
		   	   	  case RECORD_SHORT:
                      uint16_t sd = (uint16_t)lua_tointeger( L, i +1  );
		   	     	  vSetRecordData(i + start_index,(uint8_t *)&sd);
		   	   		  break;
		   	   	  case RECORD_LUA:
		   	   	    uint8_t record[REGISTER_SIZE];
		   	   	    switch (lua_type(  L, i+1) )
		   	   	    {
		   	   	    		case LUA_TNUMBER:
		   	   	    			if ( lua_isinteger( L, i+1 ) )
		   	   	    			{
		   	   	    				   uint32_t temp_int = lua_tointeger( L, i +1 );
		   	   	    				   vSetRegData(record,(uint8_t *)&temp_int,INTEGER_DATA  );
		   	   	    		    }
		   	   	    			else
		   	   	    			{
		   	   	    					float temp_float = lua_tonumber( L, i+1);
		   	   	    					vSetRegData(record,(uint8_t *)&temp_float,NUMBER_DATA  );
		   	   	    			}
		   	   	    			break;
		   	   	    		case LUA_TBOOLEAN:
		   	   	    		    uint32_t temp_bool = lua_toboolean( L, i+1 );
		   	   	    		    vSetRegData(record,(uint8_t *)&temp_bool,BOOLEAN_DATA );
		   	   	    			break;
		   	   	    		default:
		   	   	    			break;

		   	   	    }
		   	    	vSetRecordData(i + start_index,record);
		   	   		break;
		   	   	  default: break;
			   }
	   }
	   eEEPROMRecordADD();
	  return ( NO_RESULT );
}

static int iSetStorageFormat( lua_State *L )
{

	//RECORD_TIME_STAMP = 0x00,
	//	RECORD_BYTE		  = 0x01,
	//	RECORD_SHORT      = 0x02,
	//	RECORD_LUA		  = 0x03,
	//	RECORD_ERROR	  = 0xFF,

	uint8_t d[16]={RECORD_TIME_STAMP,RECORD_TIME_STAMP,RECORD_TIME_STAMP,RECORD_TIME_STAMP,
			       RECORD_TIME_STAMP,RECORD_TIME_STAMP,RECORD_TIME_STAMP,RECORD_TIME_STAMP,
			       RECORD_TIME_STAMP,RECORD_TIME_STAMP,RECORD_TIME_STAMP,RECORD_TIME_STAMP};
	uint8_t arg_number = lua_gettop(L);

	 if (arg_number!=0)
	 {
		 uint8_t reset_data = lua_tointeger( L , FIRST_ARGUMENT );
		 uint8_t register_count = lua_tointeger( L , SECOND_ARGUMENT );
		 uint8_t record_format_count = 0;
		 for (uint8_t i = 0; i < arg_number -1; i++)
		 {
			 d[i] = lua_tointeger( L, i + 2 );
			 record_format_count++;

		 }
		eCreateDataStorage(register_count , d, record_format_count, reset_data );

	 }

	return ( NO_RESULT );
}


/****************
 *
 */
uint8_t ucLUAgetErrorCount ( void )
{
	return ( ucErrorCount );
}
/*
 *
 */
const char * pcLUAgetErrorString ( void )
{
	return ( pcLuaErrorString );
}
/*
 *
 */
uint32_t ulLUAgetWorkCicle ( void )
{
	return ( ulWorkCicleIn10us );
}
/*
 *
 */
LUA_STATE_t eLUAgetSTATE ( void )
{
  return ( state );
}
/*
 *
 */
static RESULT_t eIsLuaSkriptValid(const char* pcData, uint32_t size)
{
	uint8_t ucRes = RESULT_FALSE;
	uint8_t ucEND = 0x00;
	for (uint32_t ulIndex = 0;ulIndex < size; ulIndex++)
	{
		if ( pcData[ulIndex] == ucEND )
		{
			if (ucEND == 0x00)
			{
				if ( ( pcData[0] == LUA_SIGNATURE[0] ) && ( pcData[1] == LUA_SIGNATURE[1] ) && ( pcData[2] == LUA_SIGNATURE[2] ) && (pcData[3] == LUA_SIGNATURE[3]) )
				{
					ucEND = 0xFF;
				}
				else
				{
					ucRes = RESULT_TRUE;
					break;
				}
			}
			else
			{
				ucRes = RESULT_TRUE;
				break;
			}
		}
	}
	return ( ucRes );
}

extern I2C_HandleTypeDef hi2c2;
/*
 *
 */
void vLuaTask(void *argument)
{
	 uint32_t OutStatus1   = 0;
	 uint32_t OutStatus2   = 0;
	 lua_State *L = NULL;
	 lua_State *L1 = NULL;
	 vCCMRAVarInir();
	 vEEPROMInit( &hi2c2 );
    // Загружаем библиотеки PDM
	 HAL_TIM_Base_Start(&htim11);
     while(1)
	 {
	   vTaskDelay( 1 );
	   WDT_Reset();
	   switch (state)
	   {
       case LUA_INIT:
    	   RestartFlag = 0;
    	   vOutInit();
           vAINInit();
           vDinInit();
    	   eMainLoopIsEnable  = IS_DISABLE;
	   	   eSafeModeIsEnable  = IS_DISABLE;
	   	   L  = luaL_newstate();
	   	   L1 = lua_newthread(L);
	   	   luaL_openlibs(L1); // open standard libraries
	   	   lua_register(L1,"CanTable",iCanSendTable);
	   	   lua_register(L1,"setDINConfig",iDinConfig);
	   	   lua_register(L1,"setOutConfig", iOutConfig);
	   	   lua_register(L1,"OutResetConfig", iOutResetConfig);
	   	   lua_register(L1,"OutSetPWM", iOutSetPWM);
	   	   lua_register(L1,"CanSend", iCanSendPDM);
	   	   lua_register(L1,"setCanFilter", iCanSetResiveFilter);
	   	   lua_register(L1,"CheckCanId", iCanCheckData);
	   	   lua_register(L1,"GetCanMessage",iCanGetMessage);
	   	   lua_register(L1,"GetCanToTable",iCanGetResivedData);
	   	   lua_register(L1,"sendCandRequest",iCanSendRequest);
	   	   lua_register(L1,"CheckAnswer", iCanCheckData);
	   	   lua_register(L1,"GetRequest",iCanGetMessage);
	   	   lua_register(L1,"GetRequestToTable",iCanGetResivedData);
	   	   lua_register(L1,"ConfigCan",iCanSetConfig);
	   	   lua_register(L1,"GetEEPROMReg",iGetEEPROM);
	   	   lua_register(L1,"SetEEPROMReg",iSetEEPROM);
	   	   lua_register(L1,"setPID",iSetPID);
	   	   lua_register(L1,"resetPID",iResetPID);
	   	   lua_register(L1,"processPID",iProcessPID);
	   	   lua_register(L1,"setOutSoftStart",iSoftStart);
	   	   lua_register(L1,"setPWMGroupeFreq",isetPWMFreq);
           lua_register(L1,"setAINCalTable",isetAINCal);
           lua_register(L1,"SYSTEM_RESTASRT",iSysrestart);
           lua_register(L1,"SetTimeDate",iSetTime);
           lua_register(L1,"GetTimeDate",iGetTime);
           lua_register(L1,"AddReccord",iSetRecord);
           lua_register(L1,"ConfigStorage",iSetStorageFormat);
           lua_register(L1,"RPMConfig",iSetRPMConfig);
           lua_register(L1,"RandomRestart",iSetRandomRestart);
	   	   vLUArunPDM();
	   	   if ( eIsLuaSkriptValid(uFLASHgetScript(), uFLASHgetLength()+1) == RESULT_TRUE )
	   	   {
	   	    	res =(luaL_loadbuffer(L1, uFLASHgetScript(), uFLASHgetLength() , uFLASHgetScript())  || lua_pcall(L1, 0, LUA_MULTRET, 0));
	   	   }
	   	   else
	   	   {
	   	   		 state = LUA_ERROR;
	   	   }
	   	   break;
	   	 case LUA_RUN:
	   	   lua_getglobal(L1, "main");
	   	   ulWorkCicleIn10us  = ulRestartTimer();
	   	   lua_pushinteger(L1, ulWorkCicleIn10us );
	   	   vGetDoutStatus(&OutStatus1 , &OutStatus2 );
	   	   lua_pushinteger( L1, OutStatus1 );
	   	   lua_pushinteger( L1, OutStatus2 );
	   	   lua_pushinteger( L1, uiGetDinMask() );
           for ( uint8_t i = 0U; i < OUT_COUNT ; i++ )
	   	   {
        	   lua_pushnumber( L1, fOutGetCurrent( i ) );
	   	   }
           lua_pushnumber( L1, uGetRPM1( ) );
           lua_pushnumber( L1, uGetRPM2( ) );
           lua_pushnumber( L1, fAinGetState(0));
           lua_pushnumber( L1, fAinGetState(1));
           lua_pushnumber( L1, fAinGetState(2));
           lua_pushnumber( L1, fBatteryGet() );
           lua_pushnumber( L1, fAngleGet (ANGLE_TYPE_ROLL) );
           lua_pushnumber( L1, fAngleGet (ANGLE_TYPE_PITCH) );
           lua_pushnumber( L1, fAngleGet (ANGLE_TYPE_YAW) );
           lua_pushnumber( L1, fTemperatureGet () );
           int temp;
           switch ( lua_resume( L1, L, (1+1+2+OUT_COUNT+2+4+3+1), &temp) )
	   	   {
	   	     case  LUA_OK:
	   	   	   if (eMainLoopIsEnable == IS_DISABLE)
	   	   	   {
	   	   		   eMainLoopIsEnable = IS_ENABLE;
	   	   	   }
	   	   	 case LUA_YIELD:
	   	   		 for ( uint8_t i = 0; i < OUT_COUNT; i++)
	   	   		 {
	   	   			 vOutSetState( i, (uint8_t) lua_toboolean( L1,-(i+1)) );
	   	   		 }
	   	   		 break;
	   	   	 default:
	   	   	   pcLuaErrorString =  (char *) lua_tostring( L1, LAST_ARGUMENT );
	   	   	   ucErrorCount++;
	   	   	   state  = LUA_ERROR;
	   	   	   break;
	   	   }
	   	   lua_pop( L1, temp);
	   	   if (RestartFlag != 0)
	   	   {
  	   		  vSafeModeOutState();
	   		  state = LUA_RESTART;
	   	   }
	   	   break;
	   	 case LUA_ERROR:
	   	 case LUA_STOP:
	   		if (eSafeModeIsEnable == IS_DISABLE)
	   		{
	   			vSafeModeOutState();
	   			eSafeModeIsEnable = IS_ENABLE;
	   		}
	   	   break;
	   	 case LUA_RESTART:
	   	   lua_close(L);
	   	   state = LUA_INIT;
	   	   break;
	   	 default:
	   	   break;
	  }
  }
}



