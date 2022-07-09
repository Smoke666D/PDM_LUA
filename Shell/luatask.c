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
int CanGetMessage(lua_State *L )
{
	uint8_t n;
	CAN_FRAME_TYPE  RXPacket;
	if (lua_gettop(L) >= 1U )  //Проверяем, что при вызове нам передали нужное число аргументов
	{
				RXPacket.ident = (uint32_t) lua_tointeger(L,-1);
				if ( vCanGetMessage(&RXPacket, 0) != 0)
				{

						n = RXPacket.DLC;
						for (int i = 0; i <n;i++)
						{
							lua_pushnumber(L,RXPacket.data[i]);
						}
					}
	}
	return n;

}

int CanGetResivedData(lua_State *L )
{
	uint32_t id;
	uint8_t n;

	CAN_FRAME_TYPE  RXPacket;
	if (lua_gettop(L) >= 1U )  //Проверяем, что при вызове нам передали нужное число аргументов
	{
		        luaL_checktype(L, -1, LUA_TTABLE);
				RXPacket.ident = (uint32_t) lua_tointeger(L,-2);
				if ( vCanGetMessage(&RXPacket, 0) == 1)
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
		       local obj = {ADDR = addr, new = false, key =0x00, tog= 0x00, old =0x00, led_red=0x00,led_green=0x00, led_blue =0x00}\n\
	       	       setmetatable (obj, self) SetCanFilter(0x180 +addr)\n\
     		       return obj\n\
     		   end\n\
			 	function KEYPAD8:PROCESS()\n\
		if (SheckCanId(0x180 + self.ADDR)==1) then  self.old =self.key self.key = GetCanMessage(0x180 +self.ADDR) self.led_blue =self.key self.tog = (~ self.old & self.key) ~ self.tog		 end\n\
			if self.new == true then\n\
				self.new = false		\n\
				CanSend(0x215,self.led_red,self.led_green,self.led_blue,0,0,0,0,0)\n\
			end\n\
		end \n\
		function KEYPAD8:KEY( ind ) \n\
		  	  return  (self.key & ( 0x01 << (ind-1)) ) ~= 0 \n\
					end \n\
		function KEYPAD8:TOG( ind ) \n\
                 return  (self.tog & ( 0x01 << (ind-1)) ) ~= 0 \n\
	        end \n\
		function KEYPAD8:RES( ind ) \n\
		 self.tog =  (~(0x01<< ind)) & self.tog \n\
		end \n\
	        function KEYPAD8:LED_RED( ind, data)\n\
		 	 if (data == false) then self.led_red = self.led_red & (~(0x01<<(ind-1))) else self.led_red = self.led_red | (0x01<<(ind-1)) end\n\
		    self.new = true\n\
	     	end\n\
		function KEYPAD8:LED_GREEN( ind, data)\n\
				 	 if (data == false) then self.led_green = self.led_green & (~(0x01<<(ind-1))) else self.led_green = self.led_green | (0x01<<(ind-1)) end\n\
				    self.new = true\n\
			     	end\n\
					function KEYPAD8:LED_BLUE( ind, data)\n\
							 	 if (data == false) then self.led_blue = self.led_blue & (~(0x01<<(ind-1))) else self.led_blue = self.led_blue | (0x01<<(ind-1)) end\n\
							    self.new = true\n\
						     	end\n\
		TURNS_LIGTH = {}\n\
		TURNS_LIGTH.__index = TURNS_LIGTH\n\
		               function TURNS_LIGTH:NEW()\n\
				       local obj = { in_left = false, in_rigth = false, in_alarm = false,  out_left = false, out_rigth, out_alarm =false, timer =0, clock = false}\n\
		       	       setmetatable (obj, self)\n\
						return obj\n\
						end\n\
				function TURNS_LIGTH:PROCESS( tim ,ign )\n\
					self.timer = self.timer + tim \n\
					if (ign == true) then\n\
					if (self.timer > 500) then if self.clock == true then self.clock = false else self.clock = true end self.timer=0 end\n\
					self.out_alarm = false self.out_left = false  self.out_rigth = false				\n\
					if (self.in_alarm == true) then  self.out_alarm = self.clock  else \n\
					if (self.in_rigth == true) then   self.out_rigth=self.clock  else\n\
					if (self.in_left  == true) then   self.out_left =self.clock  end\n\
						 end end  end\n\
				end\n\
function stop() timer = coroutine.yield(Out20,Out19,Out18,Out17,Out16,Out15,Out14,Out13,Out12,Out11,Out10,Out9,Out8,Out7,Out6,Out5,Out4,Out3,Out2,Out1) end\n\
main = function ()  OutConfig(10,10,1000,60) KeyPad      = KEYPAD8:NEW(0x15) al=TURNS_LIGTH:NEW()   k=3  i=0 t=1\n\
while true do OutSetPWM(13,k) KeyPad:PROCESS()  al:PROCESS(timer,true) al.in_left = KeyPad:TOG(1) al.in_rigth = KeyPad:TOG(3) al.in_alarm = KeyPad:TOG(2) \n\
KeyPad:LED_RED(1,al.out_left) KeyPad:LED_RED(2,al.out_alarm) KeyPad:LED_RED(3,al.out_rigth)		\n\
		 i = i + timer  if (i > 500) then i = 0    k = k+1\n\
  if k >8 then k = 3 if t==false then t=true else t=false end\n\
		 else KeyPad:LED_GREEN(k,t) end  end  stop() end end";

*/
static const char* mainscript1 = "Out1 = true Out2 = false Out3 = false Out4 = false Out5 = false Out6 = false Out7 = false Out8 = false Out9 = false Out10 = false\n\
Out11 = false Out12 = false Out13 = true Out14 = false Out15 = false Out16 = false Out17 = false Out18 = false Out19 = false Out20 = false timer = 0\n\
KEYPAD8 = {}\n\
KEYPAD8.__index = KEYPAD8\n\
               function KEYPAD8:NEW( addr)\n\
		       local obj = {key = 0x00, ADDR = addr, new = false,  tog= 0x00, old =0x00, led_red=0x00,led_green=0x00, led_blue =0x00, temp={0}}\n\
	       	       setmetatable (obj, self) SetCanFilter(0x180 +addr)\n\
     		       return obj\n\
     		   end\n\
			 	function KEYPAD8:PROCESS()\n\
		if (GetCanToTable(0x180 + self.ADDR,self.temp)==1) then  self.tog = (~ self.key & self.temp[1]) ~ self.tog	self.key =self.temp[1]	 end\n\
			if self.new == true then\n\
				self.new = false		\n\
				CanSend(0x215,self.led_red,self.led_green,self.led_blue,0,0,0,0,0)\n\
			end\n\
		end \n\
		function KEYPAD8:KEY( ind ) \n\
		  	  return  (self.key & ( 0x01 << (ind-1)) ) ~= 0 \n\
					end \n\
		function KEYPAD8:TOG( ind ) \n\
                 return  (self.tog & ( 0x01 << (ind-1)) ) ~= 0 \n\
	        end \n\
		function KEYPAD8:RES( ind ) \n\
		 self.tog =  (~(0x01<< ind)) & self.tog \n\
		end \n\
	        function KEYPAD8:LED_RED( ind, data)\n\
		 	 if (data == false) then self.led_red = self.led_red & (~(0x01<<(ind-1))) else self.led_red = self.led_red | (0x01<<(ind-1)) end\n\
		    self.new = true\n\
	     	end\n\
		function KEYPAD8:LED_GREEN( ind, data)\n\
				 	 if (data == false) then self.led_green = self.led_green & (~(0x01<<(ind-1))) else self.led_green = self.led_green | (0x01<<(ind-1)) end\n\
				    self.new = true\n\
			     	end\n\
					function KEYPAD8:LED_BLUE( ind, data)\n\
							 	 if (data == false) then self.led_blue = self.led_blue & (~(0x01<<(ind-1))) else self.led_blue = self.led_blue | (0x01<<(ind-1)) end\n\
							    self.new = true\n\
						     	end\n\
		TURNS_LIGTH = {}\n\
		TURNS_LIGTH.__index = TURNS_LIGTH\n\
		               function TURNS_LIGTH:NEW()\n\
				       local obj = { in_left = false, in_rigth = false, in_alarm = false,  out_left = false, out_rigth, out_alarm =false, timer =0, clock = false}\n\
		       	       setmetatable (obj, self)\n\
						return obj\n\
						end\n\
				function TURNS_LIGTH:PROCESS( tim ,ign )\n\
					self.timer = self.timer + tim \n\
					if (ign == true) then\n\
					if (self.timer > 500) then if self.clock == true then self.clock = false else self.clock = true end self.timer=0 end\n\
					self.out_alarm = false self.out_left = false  self.out_rigth = false				\n\
					if (self.in_alarm == true) then  self.out_alarm = self.clock  else \n\
					if (self.in_rigth == true) then   self.out_rigth=self.clock  else\n\
					if (self.in_left  == true) then   self.out_left =self.clock  end\n\
						 end end  end\n\
				end\n\
function stop() timer = coroutine.yield(Out20,Out19,Out18,Out17,Out16,Out15,Out14,Out13,Out12,Out11,Out10,Out9,Out8,Out7,Out6,Out5,Out4,Out3,Out2,Out1) end\n\
main = function ()  OutConfig(10,10,1000,60) KeyPad      = KEYPAD8:NEW(0x15) al=TURNS_LIGTH:NEW()   k=3  i=0 t=1\n\
while true do OutSetPWM(13,k) KeyPad:PROCESS()  al:PROCESS(timer,true) al.in_left = KeyPad:TOG(1) al.in_rigth = KeyPad:TOG(3) al.in_alarm = KeyPad:TOG(2) \n\
KeyPad:LED_RED(1,al.out_left) KeyPad:LED_RED(2,al.out_alarm) KeyPad:LED_RED(3,al.out_rigth)		\n\
		 i = i + timer  if (i > 500) then i = 0    k = k+1\n\
  if k >8 then k = 3 if t==false then t=true else t=false end\n\
		 else KeyPad:LED_GREEN(k,t) end  end  stop() end end";



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
    lua_register(L1,"GetCanMessage",CanGetMessage);
    lua_register(L1,"GetCanToTable",CanGetResivedData);

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
