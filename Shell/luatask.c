/*
 * luatask.c
 *
 *  Created on: Apr 6, 2022
 *      Author: igor.dymov
 */
#include "lprefix.h"
#include "luatask.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "PDMhardware.h"
#include "FreeRTOS.h"
#include "script.c"

/*
 * С функция для конфигурации входов из LUA
 *
 */
#define CONFIG_ARGUMENT_COUNT 4
#define RESET_ARGUMENT_COUNT 3
#define PWM_ARGUMENT_COUNT 2
int  OutConfig( lua_State *L )
{
	int out_number = 0;
	int arg_number = lua_gettop(L);
	float power, overload_power,temp_d;
	int overload_timer, temp_i;

	if (arg_number >= CONFIG_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		out_number = lua_tointeger(L,-arg_number) -1 ; //Первым аргументом дожен передоваться номер канала
		if (out_number < OUT_COUNT) //Проверяем, что номер канала задан верно
		{
			if (out_number < OUT_HPOWER_COUNT) //В зависимости от номера канала, устанваливаем значения парамертов по умолчанию, на случай если они не верны
			{
			  power = DEFAULT_HPOWER;
			  overload_power = DEFAULT_HPOWER_MAX;
			}
			else
			{
			  power = DEFAULT_LPOWER;
			  overload_power = DEFAULT_LPOWER_MAX;
			}
			temp_d = (float) lua_tonumber(L,-(arg_number-1));  //Вторым агрументом должена передоваться номинальная мощность
			if ((temp_d < power) && (temp_d > 0))  power =temp_d; //Если аргумент коректный,
			temp_i = lua_tointeger(L,-(arg_number-2)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
			overload_timer = temp_i; //Если аргумент коректный
			temp_d = (float) lua_tonumber(L,-(arg_number-3));
			if (temp_d < overload_power) overload_power = temp_d; //Если аргумент коректный
			vHWOutOverloadConfig(out_number,power, overload_timer, overload_power);
		}
	}
	return 0;
}
int  OutResetConfig( lua_State *L )
{
	int out_number = 0;
	int arg_number = lua_gettop(L);
	int reset_count,timer, temp_i;

		if (arg_number >= RESET_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
		{
			out_number = lua_tointeger(L,-arg_number) -1 ; //Первым аргументом дожен передоваться номер канала
			if (out_number < OUT_COUNT) //Проверяем, что номер канала задан верно
			{
			    reset_count = 0;
				timer = 1000;
				temp_i = lua_tointeger(L,-(arg_number-1)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
			    reset_count = temp_i; //Если аргумент коректный
				temp_i = lua_tointeger(L,-(arg_number-2));
				if (temp_i < timer) timer = temp_i; //Если аргумент коректный
				vHWOutResetConfig(out_number, reset_count, timer);
			}
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


static const char* mainscript1 = "Out1 = false  main = function () function stop()  In1 = coroutine.yield(Out1,i)  end OutConfig(10,10,1000,60) i = 0 \n\
while true do  i = i+ 1 if (i == 1000) then i = 0   Out1 = not Out1 end  stop() end end";

//static const char* mainscript1 = " return 10";


void vLuaTask(void *argument)
{

	 uint32_t temp, temp1;
	 const luaL_Reg *lib;
	 const char * err;
	 int res;
	 lua_State *L = luaL_newstate();
	 lua_State *L1;
     if (L == NULL)
     {
    	 while(1)
    		{
    			  vTaskDelay(1 );
    		}
    }
	L1 = lua_newthread(L);
    luaL_openlibs(L1); // open standard libraries
    // Загружаем библиотеки PDM

    lua_register(L1,"OutConfig", OutConfig);
    lua_register(L1,"OutResetConfig", OutResetConfig);
    lua_register(L1,"OutSetPWM", OutSetPWM);
    res = luaL_dostring(L1,mainscript1);

   while(1)
	{
	     lua_getglobal(L1, "main");
		 lua_pushinteger(L1,1);
		 res = lua_resume(L1,L,1,&temp);
		 if ((res == LUA_OK) || (res == LUA_YIELD))
		 {
			 temp1 =lua_tointeger(L1,-1);
			 temp1 =lua_toboolean(L1,-2);
				vOutState(OUT_9, temp1);
				vOutState(OUT_10, temp1);
				vOutState(OUT_11, temp1);
				vOutState(OUT_12, temp1);
				vOutState(OUT_13, temp1);
		 }
		 else
			 err =  lua_tostring(L1, -1);
		 lua_pop(L1, temp);
		 vTaskDelay(1 );
	 }



}
