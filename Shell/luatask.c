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
/*
 * С функция для конфигурации входов из LUA
 *
 */
#define CONFIG_ARGUMENT_COUNT 4
#define RESET_ARGUMENT_COUNT 3
#define PWM_ARGUMENT_COUNT 2
#define CANSEND_ARGUMENT_COUNT 2
#define CANREQSEND_ARGUMENT_COUNT 3
#define SEND_REQUEST_ARGUMENT_COUNT 3
static LUA_STATE state = LUA_INIT;
static   EventGroupHandle_t xPDMstatusEvent;
/*---------------------------------------------------------------------------------------------------*/
EventGroupHandle_t* osLUAetPDMstatusHandle ( void )
{
  return &xPDMstatusEvent;
}
void vLUArunPDM()
{
	xEventGroupSetBits(xPDMstatusEvent,RUN_STATE);
	state = LUA_RUN;
}

void vLUAstopPDM()
{
	xEventGroupClearBits(xPDMstatusEvent,RUN_STATE);
	state = LUA_STOP;
}

void vLUArestartPDM()
{
	state = LUA_RESTART;
}


/*
 * Устанавливаем конфигурацию дискрнтого входа
 */
int DinConfig(lua_State *L )
{
		uint32_t lf = DEF_L_FRONT, hf = DEF_H_FRONT;
		uint8_t in_number = 0;
		int arg_number = lua_gettop(L);
		LOGIC_STATE state;
		if (arg_number >= 2)  //Проверяем, что при вызове нам передали нужное число аргументов
		{
			in_number =(uint8_t) (lua_tointeger(L,-arg_number) -1) ; //Первым аргументом дожен передоваться номер канала
			state = lua_tonumber(L,-(arg_number-1));  //Вторым агрументом должена передоваться номинальная мощность
			if ( arg_number > 2)
			{
				lf = lua_tointeger(L,-arg_number-2);
			}
			if ( arg_number > 3)
			{
				lf = lua_tointeger(L,-arg_number-3);
			}
			inputConfig(in_number, (state == 1)?POSITIVE_STATE:NEGATIVE_STATE ,hf,lf);

		}
		return 0;

}

/*
 * Устанавливаем новый фильтр, позволяющий принимать пакеты с нужным ID
 */
int CanSetResiveFilter(lua_State *L )
{
  if (lua_gettop(L) >= 1U )  //Проверяем, что при вызове нам передали нужное число аргументов
  {
	  luaL_checktype(L, 1, LUA_TNUMBER);
	  SetMailboxFilter( lua_tointeger(L,-1));
  }
  return 0U;
}

/*
 * Проверка пакета с нужным id
 */
int CanCheckData(lua_State *L )
{
	uint32_t res;
	switch (lua_gettop(L))
	{
	   case 0:
		   res = CheckAnswer();
		   break;
	   case 1:
		   luaL_checktype(L, 1, LUA_TNUMBER);
		   res = vCanChekMessage(lua_tointeger(L,-1));
		   break;
	}
	lua_pushnumber(L,res );
	return 1;
}

int CanGetMessage(lua_State *L )
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
	return n;
}

int CanGetResivedData(lua_State *L )
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
	}
	return 1;
}



int CanSendPDM( lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint32_t ID;
	uint8_t DATA[8];
	uint8_t size;

		if (arg_number >= CANSEND_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
		{
			ID = (uint32_t) lua_tointeger(L,-arg_number) ; //Первым аргументом дожен передоваться ID пакета

			size  = arg_number -1;
			for (int i=0;i<size;i++)
			{
				DATA[i]= (uint8_t) lua_tointeger(L,-(arg_number-1-i)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
			}
			vCanInsertTXData(ID, &DATA[0], size);
		}
		return 0;
}


int CanSendTable( lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint32_t ID;
	uint8_t DATA[8];
	uint8_t size;

		if (arg_number >= 3)  //Проверяем, что при вызове нам передали нужное число аргументов
		{
			ID = (uint32_t) lua_tointeger(L,1) ; //Первым аргументом дожен передоваться ID пакета
			size  = (uint32_t) lua_tointeger(L,2);
		    luaL_checktype(L, 3, LUA_TTABLE);
			for (int i=0;i<size;i++)
			{
				lua_geti(L, 3, i + 1);
				DATA[i]= lua_tointeger(L,-1);
				lua_pop(L,1);
			}
			vCanInsertTXData(ID, &DATA[0], size);
		}
		return 0;
}







int CanSendRequest( lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint32_t ID;
	uint8_t DATA[8];
	uint8_t size;
	if (arg_number >= SEND_REQUEST_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		ID = (uint32_t) lua_tointeger(L,-arg_number) ; //Первым аргументом дожен передоваться ID пакета
		SetWaitFilter(lua_tointeger(L,-(arg_number-1)) );//Второй агрумент - id ответного пакета
		size  = arg_number -2;
		for (int i=0;i<size;i++)
		{
			DATA[i] = (uint8_t) lua_tointeger(L,-(arg_number-2-i)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
		}
		vCanInsertTXData(ID, &DATA[0], size);
	}
	return 0;
}



int  OutConfig( lua_State *L )
{
	int out_number = 0;
	int arg_number = lua_gettop(L);
	float power, overload_power;
	uint16_t overload_timer;
	if (arg_number >= CONFIG_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		out_number = lua_tointeger(L,-arg_number) -1 ; //Первым аргументом дожен передоваться номер канала
		power = (float) lua_tonumber(L,-(arg_number-1));  //Вторым агрументом должена передоваться номинальная мощность
		overload_timer = (uint16_t) lua_tointeger(L,-(arg_number-2)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
		overload_power= (float) lua_tonumber(L,-(arg_number-3));
		vHWOutOverloadConfig(out_number,power, overload_timer, overload_power);
	}
	return 0;
}
int  OutResetConfig( lua_State *L )
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
	return 0;
}

int  OutSetPWM( lua_State *L )
{
	int out_number = 0;
	int temp_i, pwm = 100;
	int arg_number = lua_gettop(L);
		if (arg_number >= PWM_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
		{
			out_number = lua_tointeger(L,-arg_number) -1 ; //Первым аргументом дожен передоваться номер канала
			if (out_number < OUT_COUNT) //Проверяем, что номер канала задан верно
			{
				pwm = DEFAULT_PWM;
				temp_i = lua_tointeger(L,-(arg_number-1)); //Вторам агрументом должна передоватьс мощность канала в процентах 0-100.
				if (temp_i < pwm) pwm = temp_i; //Если аргумент коректный
				vOutSetPWM(out_number, pwm);
			}
		}
	return 0;
}



const char * err = NULL;
int res = 0;
int pdm_time_process = 0;

/****************
 *
 */
LUA_STATE xLUAgetSTATE()
{
	return state;
}
/*
 *
 */
const char * xLUAgetError()
{
	return err;
}
/*
 *
 */
int xLUAgetTime()
{
	return pdm_time_process;
}

void vLuaTask(void *argument)
{
	 uint8_t default_script = 0;
	 EventBits_t config_state;
	 uint16_t system_timer = 0;
     uint8_t init = 0;
	 int temp;
	 uint8_t i,out[20];

	 lua_State *L;// = luaL_newstate();
	 lua_State *L1;

    // Загружаем библиотеки PDM

   while(1)
	{
	   system_timer = GetSysTimer();
	   vTaskDelay(1 );
	   switch (state)
	   {
	   	   	  case LUA_INIT:
	   	   		  init = 0;
	   	   		  L  = luaL_newstate();
	   	   		  L1 = lua_newthread(L);
	   	   		  luaL_openlibs(L1); // open standard libraries
	   	   		  lua_register(L1,"CanTable",CanSendTable);
	   	   		  lua_register(L1,"setDINConfig",DinConfig);
	   	   		  lua_register(L1,"setOutConfig", OutConfig);
	   	   		  lua_register(L1,"OutResetConfig", OutResetConfig);
	   	   		  lua_register(L1,"OutSetPWM", OutSetPWM);
	   	   		  lua_register(L1,"CanSend", CanSendPDM);
	   	   		  lua_register(L1,"setCanFilter", CanSetResiveFilter);
	   	   		  lua_register(L1,"CheckCanId", CanCheckData);
	   	   		  lua_register(L1,"GetCanMessage",CanGetMessage);
	   	   		  lua_register(L1,"GetCanToTable",CanGetResivedData);
	   	   		  lua_register(L1,"sendCandRequest",CanSendRequest);
	   	   		  lua_register(L1,"CheckAnswer", CanCheckData);
	   	   		  lua_register(L1,"GetRequest",CanGetMessage);
	   	   		  lua_register(L1,"GetRequestToTable",CanGetResivedData);
	   	   		  vLUArunPDM();
	   	   		  if ( default_script == 0)
	   	   		  {
	   	   			  const char*  datastring =  (const char*) FLASH_STORAGE_ADR;
	   	   			  uint32_t t = strlen(datastring);
	   	   			  if (t > 0)
	   	   			  {
	   	   				  res = luaL_dostring(L1,uFLASHgetScript());
	   	   			  }
	   	   			  else
	   	   				  default_script = 1;
	   	   		  }
	   	   		  if (default_script ==1)
	   	   		  {
	   	   	       	   res = luaL_dostring(L1,defaultLuaScript);
	   	   		  }
	   	   		  break;
	   	   	  case LUA_RUN:

	   	   		   if (init == 0)
	   	   		    	 lua_getglobal(L1, "init");
	   	   		    else
	   	   		    	 lua_getglobal(L1, "main");
	   	   		     pdm_time_process =GetTimer();
	   	   		     lua_pushinteger(L1,pdm_time_process);
	   	   			 for (i=0;i< DIN_CHANNEL;i++)
	   	   			 {
	   	   				 lua_pushboolean(L1,uDinGet(i));
	   	   			 }

	   	   			 for (i=0;i< OUT_COUNT ;i++)
	   	   			 {
	   	   				 lua_pushnumber(L1,fOutGetCurrent(i));
	   	   			 }
	   	   			 switch (lua_resume(L1,L,(1+DIN_CHANNEL+OUT_COUNT),&temp) )
	   	   			 {
	   	   			    case  LUA_OK:
	   	   				if (init == 0)
	   	   				{
	   	   				   init = 1;
	   	   				}
	   	   				case LUA_YIELD:
	   	   					for (i=0;i<OUT_COUNT;i++)
	   	   					{
	   	   					 	 out[i] =lua_toboolean(L1,-(i+1));
	   	   					  	 vOutSetState(i,(uint8_t)out[i]);
	   	   					}
	   	   					break;
	   	   				 default:
	   	   				   err =  lua_tostring(L1, -1);
	   	   				   state  = LUA_ERROR;
	   	   			  }
	   	   			  lua_pop(L1, temp);
	   	   			  break;
	   	   		     case LUA_ERROR:
	   	   		    	 default_script = 1;
	   	   		    	 state = LUA_RESTART;
	   	   		    	 break;
	   	   		     case LUA_STOP:
	   	   		    	 break;
	   	   		     case LUA_RESTART:
	   	   		         lua_close(L);
	   	   		    	 state = LUA_INIT;
	   	   		    	 break;
	   	   		  break;


	   }
	       system_timer = GetSysTimer();
	 }

}
