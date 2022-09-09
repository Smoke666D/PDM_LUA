/*
 * luatask.c
 *
 *  Created on: Apr 6, 2022
 *      Author: igor.dymov
 */
#include "luatask.h"
#include "lprefix.h"
#include "luaDefScript.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lapi.h"
#include "PDMhardware.h"
#include "cantask.h"
#include "script.c"
#include "pdm_input.h"
#include "flash.h"
#include "string.h"


extern TIM_HandleTypeDef htim11;
static LUA_STATE_t state 					__SECTION(RAM_SECTION_CCMRAM) = 0U;
uint32_t ulWorkCicleIn10us					__SECTION(RAM_SECTION_CCMRAM) = 0U;
static uint8_t ucErrorCount 				__SECTION(RAM_SECTION_CCMRAM)= 0U;
static ENABLE_t eSafeModeIsEnable 			__SECTION(RAM_SECTION_CCMRAM)= IS_DISABLE;
static ENABLE_t eMainLoopIsEnable 			__SECTION(RAM_SECTION_CCMRAM)= IS_DISABLE;

static EventGroupHandle_t xPDMstatusEvent 	__SECTION(RAM_SECTION_CCMRAM);

const char * pcLuaErrorString 				__SECTION(RAM_SECTION_CCMRAM) = NULL;
int res 									__SECTION(RAM_SECTION_CCMRAM) = 0;

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
	uint32_t data;
	data = htim11.Instance->CNT;
	htim11.Instance->CNT= 0;
	return ( data );
}
/*
 *
 */
static void vSafeModeOutState()
{
	 for (uint8_t i=0;i<OUT_COUNT;i++)
	 {
		 vOutSetState( i, 0U);
     }
}
/*---------------------------------------------------------------------------------------------------*/
EventGroupHandle_t* osLUAetPDMstatusHandle ( void )
{
  return ( &xPDMstatusEvent );
}
/*
 *
 */
void vLUArunPDM()
{
	xEventGroupSetBits(xPDMstatusEvent,RUN_STATE);
	state = LUA_RUN;
}
/*
 *
 */
void vLUAstopPDM()
{
	xEventGroupClearBits(xPDMstatusEvent,RUN_STATE);
	state = LUA_STOP;
}
/*
 *
 */
void vLUArestartPDM()
{
	state = LUA_RESTART;
}

/*
 * Устанавливаем конфигурацию дискрнтого входа
 */
#define FULL_DIN_CONFIG_ARG  4U
#define MIN_DIN_CONFIG_ARG   2U

static int iDinConfig(lua_State *L )
{
		uint32_t lf;
		uint32_t hf;
		uint8_t in_number = 0;
		int arg_number = lua_gettop(L);
		LOGIC_STATE state;
		if (arg_number >= MIN_DIN_CONFIG_ARG )  /*Проверяем, что при вызове нам передали нужное число аргументов*/
		{
			in_number =(uint8_t) (lua_tointeger(L,-arg_number) -1) ; /*Первым аргументом дожен передоваться номер канала*/
			state = lua_tonumber(L,-(arg_number-1));  //Вторым агрументом должена передоваться номинальная мощность
			if (arg_number == FULL_DIN_CONFIG_ARG )
			{
					lf = lua_tointeger(L,-arg_number-2);
					hf = lua_tointeger(L,-arg_number-3);
			}
			else
			{
					lf = DEF_L_FRONT;
					hf = DEF_H_FRONT;
			}
			eDinConfig(in_number, (state == 1)?POSITIVE_STATE:NEGATIVE_STATE ,hf,lf);
		}
		return ( 0U );
}

/*
 * Устанавливаем новый фильтр, позволяющий принимать пакеты с нужным ID
 */
int iCanSetResiveFilter(lua_State *L )
{
  uint8_t ucResNumber = 0;
  if (lua_gettop(L) == 1U )  /*Проверяем, что при вызове нам передали нужное число аргументов*/
  {
	  if( lua_isinteger(L, -1) == 1) /*Проверяем что аргумент нужного типа*/
	  {
		  switch (eMailboxFilterSet( lua_tointeger(L,-1)))  /*Пытамеся настроить mailbox.*/
		  {
		  	  case BUFFER_FULL:   /*Если все mailbox заняты*/
		  		  lua_pushnumber(L, 1U );
		  		  ucResNumber = 1;
		  		  break;
		  	  case ERROR_NO:  /*Если id успешно зарегестрирован*/
		  		  lua_pushnumber(L, 0U );
		  		  ucResNumber = 1;
		  		  break;
		  	  default:
		  		  break;
		  }
	  }
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
		  if ( lua_isinteger( L, CAN_ID_POS ) )
		  {
			  uiRes = vCanChekMessage( lua_tointeger(L, CAN_ID_POS) );
		  }
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
	uint32_t ID;
	uint8_t  DATA[CAN_FRAME_SIZE];
	uint8_t  size = lua_gettop(L);
	if ((size >= CANSEND_ARGUMENT_COUNT)&& lua_isinteger(L, CAN_ID_POS ) )    /*Проверяем, что при вызове нам передали нужное число аргументов*/
	{
	    ID = (uint32_t) lua_tointeger(L, CAN_ID_POS) ; /*Первым аргументом дожен передоваться ID пакета*/
		size--;
		for (int i=0;i< size ;i++)
		{
		  DATA[i]= (uint8_t) lua_tointeger(L,-( size-i)); /*Третьем агрументом должно передоватьс время плавного старта в милисекундах*/
		}
		vCanInsertTXData(ID, &DATA[0], size);
	}
	return (0);
}


/*
 *
 */
int iCanSendTable( lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint32_t ID;
	uint8_t DATA[ CAN_FRAME_SIZE ];
	uint8_t size;
	if (arg_number == CAN_SEND_TABLE_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		if ( lua_istable(L, CAN_TABLE_POS ) &&  lua_isinteger(L,FRAME_SIZE_POS) && lua_isinteger(L, CAN_ID_POS) )
		{
			ID 	  = (uint32_t) lua_tointeger(L, CAN_ID_POS ); //Первым аргументом дожен передоваться ID пакета
			size  = (uint8_t) lua_tointeger(L, FRAME_SIZE_POS);
			if  (size <= CAN_FRAME_SIZE )
			{
				for (uint8_t i = 0; i < size; i++)
				{
					lua_geti(L, CAN_TABLE_POS , i + 1);
					DATA[i]= lua_tointeger(L,-1);
					lua_pop(L,1);
				}
				vCanInsertTXData(ID, &DATA[0], size);
			}
		}
	}
	return (0U);
}
/*
 *
 */
int iCanSendRequest( lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint32_t ID;
	uint8_t DATA[CAN_FRAME_SIZE];
	uint8_t size;
	if (arg_number >= SEND_REQUEST_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		ID = (uint32_t) lua_tointeger(L,-arg_number) ; //Первым аргументом дожен передоваться ID пакета
		vSetWaitFilter(lua_tointeger(L,-(arg_number-1)) );//Второй агрумент - id ответного пакета
		size  = arg_number -2;
		for (int i=0;i<size;i++)
		{
			DATA[i] = (uint8_t) lua_tointeger(L,-(arg_number-2-i)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
		}
		vCanInsertTXData(ID, &DATA[0], size);
	}
	return 0;
}


/*
 *
 */
int  iOutConfig( lua_State *L )
{
	int out_number;
	int arg_number = lua_gettop(L);
	float power;
	float overload_power;
	uint16_t overload_timer;
	if (arg_number >= CONFIG_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		out_number = lua_tointeger(L,-arg_number) -1 ; //Первым аргументом дожен передоваться номер канала
		power = (float) lua_tonumber(L,-(arg_number-1));  //Вторым агрументом должена передоваться номинальная мощность
		overload_timer = (uint16_t) lua_tointeger(L,-(arg_number-2)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
		overload_power= (float) lua_tonumber(L,-(arg_number-3));
		vHWOutOverloadConfig(out_number,power, overload_timer, overload_power);
		vOutHWEnbale(out_number);
	}
	return (0U);
}
/*
 *
 */
int  iOutResetConfig( lua_State *L )
{
	int out_number = 0;
	int arg_number = lua_gettop(L);
	uint8_t reset_count;
	uint16_t timer;
	if (arg_number >= RESET_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		out_number = lua_tointeger(L,-arg_number) -1 ; //Первым аргументом дожен передоваться номер канала
		reset_count = (uint8_t) lua_tointeger(L,-(arg_number-1)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
		timer = (uint16_t) lua_tointeger(L,-(arg_number-2));
		vHWOutResetConfig(out_number, reset_count, timer);
	}
	return (0U);
}
/*
 *
 */
int  iOutSetPWM( lua_State *L )
{
	uint16_t out_number = 0;
    uint16_t pwm;
	int arg_number = lua_gettop(L);
	if (arg_number >= PWM_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		out_number = ( uint16_t ) lua_tointeger(L,-arg_number) -1 ; //Первым аргументом дожен передоваться номер канала
		if (out_number < OUT_COUNT) //Проверяем, что номер канала задан верно
		{
			pwm = ( uint16_t) lua_tointeger(L,-(arg_number-1)); //Вторам агрументом должна передоватьс мощность канала в процентах 0-100.
			vOutSetPWM(out_number, ( pwm <= MAX_PWM )? pwm : MAX_PWM);
		}
	}
	return ( 0U );
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
	uint32_t ulIndex;
	for (ulIndex = 0;ulIndex < size; ulIndex++)
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
/*
 *
 */
void vLuaTask(void *argument)
{
	 uint32_t uiScriptSize = 0;
	 uint8_t i;
	 lua_State *L;
	 lua_State *L1;
	 vCCMRAVarInir();
    // Загружаем библиотеки PDM
	 HAL_TIM_Base_Start(&htim11);
     while(1)
	 {
	   vTaskDelay( 1 );
	   switch (state)
	   {
       case LUA_INIT:
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
	   	   if (eMainLoopIsEnable == IS_DISABLE)
	   	     lua_getglobal(L1, "init");
	   	   else
	   	     lua_getglobal(L1, "main");
	   	   ulWorkCicleIn10us  = ulRestartTimer();
	   	   lua_pushinteger(L1, ulWorkCicleIn10us );
	   	   for ( i = 0U; i < DIN_CHANNEL; i++ )
	   	   {
	   		   lua_pushboolean( L1, ucDinGet( i ) );
	   	   }
           for ( i = 0U; i < OUT_COUNT ; i++ )
	   	   {
        	   lua_pushnumber( L1, fOutGetCurrent(i));
	   	   }
           int temp;
	   	   switch (lua_resume(L1,L,(1+DIN_CHANNEL+OUT_COUNT),&temp) )
	   	   {
	   	     case  LUA_OK:
	   	   	   if (eMainLoopIsEnable == IS_DISABLE)
	   	   	   {
	   	   		   eMainLoopIsEnable = IS_ENABLE;
	   	   	   }
	   	   	 case LUA_YIELD:
	   	   		 for ( i = 0; i < OUT_COUNT; i++)
	   	   		 {
	   	   			 vOutSetState( i, (uint8_t) lua_toboolean(L1,-(i+1)) );
	   	   		 }
	   	   		 break;
	   	   	 default:
	   	   	   pcLuaErrorString =  lua_tostring(L1, -1);
	   	   	   ucErrorCount++;
	   	   	   state  = LUA_ERROR;
	   	   	   break;
	   	   }
	   	   lua_pop(L1, temp);
	   	   break;
	   	 case LUA_ERROR:
	   		if (eSafeModeIsEnable == IS_DISABLE)
	   		{
	   			vSafeModeOutState();
	   			eSafeModeIsEnable = IS_ENABLE;
	   		}

	   	   break;
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
