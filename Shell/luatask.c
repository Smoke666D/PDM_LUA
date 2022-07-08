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
#include "cantask.h"
#include "script.c"

/*
 * С функция для конфигурации входов из LUA
 *
 */
#define CONFIG_ARGUMENT_COUNT 4
#define RESET_ARGUMENT_COUNT 3
#define PWM_ARGUMENT_COUNT 2
#define CANSEND_ARGUMENT_COUNT 2
#define CANREQSEND_ARGUMENT_COUNT 3

/*
 * Устанавливаем новый фильтр, позволяющий принимать пакеты с нужным ID
 */
int CanSetResiveFilter(lua_State *L )
{
  uint32_t id;
  if (lua_gettop(L) >= 1U )  //Проверяем, что при вызове нам передали нужное число аргументов
  {
	     id =  lua_tointeger(L,-1);
		 SetMailboxFilter(id);
  }
  return 0;
}
int CanCheckData(lua_State *L )
{
	uint32_t id,res;
	if (lua_gettop(L) >= 1U )  //Проверяем, что при вызове нам передали нужное число аргументов
	{
			id = (uint32_t) lua_tointeger(L,-1);
			res = vCanChekMessage(id);
			lua_pushnumber(L,res );
	}
	return 1;

}
int CnaGetResivedData(lua_State *L )
{
	uint32_t id;
	uint8_t n;
	CAN_FRAME_TYPE  RXPacket;
	if (lua_gettop(L) >= 1U )  //Проверяем, что при вызове нам передали нужное число аргументов
	{
				RXPacket.ident = (uint32_t) lua_tointeger(L,-1);
				if (vCanCheckMessage(&RXPacket, 0) == 1)
				{
					n = luaL_len(L, 1);
					for (int i = 1; i<(n+1);i++)
					{
						lua_pushnumber(L,RXPacket.data[i]);
						lua_rawseti(L, 1, i);
					}
					lua_pushnumber(L,0U );
				}
				else
				{
					lua_pushnumber(L,1U );
				}
	}
	return 1;



}


int CanSendRequest(lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint32_t ID;
	uint32_t REQ_ID;

	uint8_t DATA[8];
	uint8_t size;
			float power, overload_power;
			uint16_t overload_timer;
			if (arg_number >= CANREQSEND_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
			{
				ID = (uint32_t) lua_tointeger(L,-arg_number) ; //Первым аргументом дожен передоваться ID пакета
				REQ_ID = (uint32_t) lua_tointeger(L,-arg_number-1) ; //Первым аргументом дожен передоваться ID пакета
				SetWaitFilter(REQ_ID);
				size  = arg_number -2;
				for (int i=0;i<size;i++)
				{
					DATA[i]=overload_timer = (uint8_t) lua_tointeger(L,-(arg_number-1-i)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
				}
				vCanInsertTXData(ID, &DATA[0], size);
			}
			return 0;

}

int CanSendPDM( lua_State *L )
{
	int arg_number = lua_gettop(L);
	uint32_t ID;
	uint8_t DATA[8];
	uint8_t size;
		float power, overload_power;
		uint16_t overload_timer;
		if (arg_number >= CANSEND_ARGUMENT_COUNT)  //Проверяем, что при вызове нам передали нужное число аргументов
		{
			ID = (uint32_t) lua_tointeger(L,-arg_number) ; //Первым аргументом дожен передоваться ID пакета
			size  = arg_number -1;
			for (int i=0;i<size;i++)
			{
				DATA[i]=overload_timer = (uint8_t) lua_tointeger(L,-(arg_number-1-i)); //Третьем агрументом должно передоватьс время плавного старта в милисекундах
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


/*
static const char* mainscript1 = "Out1 = true Out2 = false Out3 = false Out4 = false Out5 = false Out6 = false Out7 = false Out8 = false Out9 = false Out10 = false\n\
Out11 = false Out12 = false Out13 = true Out14 = false Out15 = false Out16 = false Out17 = false Out18 = false Out19 = false Out20 = false timer = 0\n\
KEYPAD8 = {}\n\
KEYPAD8.__index = KEYPAD8\n\
               function KEYPAD8:NEW( addr)\n\
		       local obj = {ADDR = addr, NEW = false, led_red=0,led_green=0, led_blue =0}\n\
	       	       setmetatable (obj, self) SetCanFilter(0x180 +addr)\n\
     		       return obj\n\
     		   end\n\
			 	function KEYPAD8:PROCESS()\n\
			if NEW == true then\n\
				NEW = false		\n\
				CanSend(0x215+adr,led_red,led_green,led_blue)\n\
			end\n\
		end \n\
	        function KEYPAD8:LED_RED( ind, data)\n\
	  	  led_red = (data) and (led_red | (0x01<<ind)) or (led_red & (~(0x01<<1)))\n\
		  NEW = true\n\
		end\n\
function stop() timer = coroutine.yield(Out20,Out19,Out18,Out17,Out16,Out15,Out14,Out13,Out12,Out11,Out10,Out9,Out8,Out7,Out6,Out5,Out4,Out3,Out2,Out1) end\n\
main = function ()  OutConfig(10,10,1000,60) KeyPad      = KEYPAD8:NEW(0x15)  k=1  i=0\n\
while true do OutSetPWM(13,k) KeyPad:PROCESS() i = i + timer  if (i > 500) then i = 0  k = k+1 CanSend(0x615,0x2f,0x01,0x20,0x01,k,0,0,0)\n\
  if k ==255 then k =1 end  end  stop() end end";
*/


static const char* mainscript1 = "Out1 = true Out2 = false Out3 = false Out4 = false Out5 = false Out6 = false Out7 = false Out8 = false Out9 = false Out10 = false\n\
Out11 = false Out12 = false Out13 = true Out14 = false Out15 = false Out16 = false Out17 = false Out18 = false Out19 = false Out20 = false timer = 0\n\
KEYPAD8 = {}\n\
KEYPAD8.__index = KEYPAD8\n\
               function KEYPAD8:NEW( addr)\n\
		       local obj = {ADDR = addr, new = false, led_red=0x00,led_green=0x00, led_blue =0x00}\n\
	       	       setmetatable (obj, self) SetCanFilter(0x180 +addr)\n\
     		       return obj\n\
     		   end\n\
			 	function KEYPAD8:PROCESS()\n\
		 if (SheckCanId(0x180 + self.ADDR)==1) then  self.led_green =0xFF end\n\
			if new == true then\n\
				new = false		\n\
				CanSend(0x215,self.led_red,self.led_green,self.led_blue,0,0,0,0,0)\n\
			end\n\
		end \n\
	        function KEYPAD8:LED_RED( ind, data)\n\
		 	 if (data == 0) then self.led_red = self.led_red & (~(0x01<<(ind-1))) else self.led_red = self.led_red | (0x01<<(ind-1)) end\n\
		    new = true\n\
	     	end\n\
		function KEYPAD8:LED_GREEN( ind, data)\n\
				 	 if (data == 0) then self.led_green = self.led_green & (~(0x01<<(ind-1))) else self.led_green = self.led_green | (0x01<<(ind-1)) end\n\
				    new = true\n\
			     	end\n\
					function KEYPAD8:LED_BLUE( ind, data)\n\
							 	 if (data == 0) then self.led_blue = self.led_blue & (~(0x01<<(ind-1))) else self.led_blue = self.led_blue | (0x01<<(ind-1)) end\n\
							    new = true\n\
						     	end\n\
function stop() timer = coroutine.yield(Out20,Out19,Out18,Out17,Out16,Out15,Out14,Out13,Out12,Out11,Out10,Out9,Out8,Out7,Out6,Out5,Out4,Out3,Out2,Out1) end\n\
main = function ()  OutConfig(10,10,1000,60) KeyPad      = KEYPAD8:NEW(0x15)  k=0  i=0 t=1\n\
while true do OutSetPWM(13,k) KeyPad:PROCESS() i = i + timer  if (i > 500) then i = 0    k = k+1\n\
  if k >8 then k = 0 if t==0 then t=1 else t=0 end\n\
		 else KeyPad:LED_RED(k,t) end  end  stop() end end";

//static const char* mainscript1 = " return 10";



void vLuaTask(void *argument)
{

	 uint32_t temp, temp1;
	 uint8_t i,out[20];
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
    lua_register(L1,"CanSend", CanSendPDM);
    lua_register(L1,"SetCanFilter", CanSetResiveFilter);
    lua_register(L1,"SheckCanId", CanCheckData);

    res = luaL_dostring(L1,mainscript1);

   while(1)
	{
	     lua_getglobal(L1, "main");
	     temp =GetTimer();
		 lua_pushinteger(L1,temp);
		 res = lua_resume(L1,L,1,&temp);
		 if ((res == LUA_OK) || (res == LUA_YIELD))
		 {
			 for (i=0;i<OUT_COUNT;i++)
			 {
				 //res = lua_toboolean(L1,-(i+1));
				 out[i] =lua_toboolean(L1,-(i+1));
				 vOutState(i,(uint8_t)out[i]);
			 }
		 }
		 else
			 err =  lua_tostring(L1, -1);
		 lua_pop(L1, temp);
		 vTaskDelay(1 );
	 }



}
