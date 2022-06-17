/*
 * script.c
 *
 *  Created on: 21 апр. 2022 г.
 *      Author: igor.dymov
 */
#include "script.h"

static const char* lib = "function CanSend(addr, count, b1,b2,b3,b4,b5,b6,b7,b8) end\n\
function CanRead(addr)\n\
return 0,0,0,0,0,0,0,0\n\
end\n\
function BToI( a)\n\
 if a == true then return 0x01 else return 0x00 end\n\
end\n\
--[[Глобальные переменные для обмена]]\n\
OutState = 0\n\
Vbat = 0\n\
Term1 =0\n\
Term2 =0\n\
--[[Ноды]]\n\
KEYPAD8 = {}\n\
KEYPAD8.__index = KEYPAD8\n\
                function KEYPAD8:NEW( addr)\n\
		       local obj = {ADDR = addr, LKEY = 0, KEY = 0, TOG = 0, led_red=0,led_green=0, led_blue =0}\n\
	       	       setmetatable (obj, self)\n\
     		       return obj\n\
		end\n\
		function KEYPAD8:PROCESS()\n\
			OLD = KEY\n\
                        KEY = CanRead(180+ADDR)\n\
				        TOG = (~ OLD & KEY) ~ TOG\n\
			CanSend(300+adr,led_red,led_green,led_blue)\n\
		end \n\
		function KEYPAD8:KEY( ind )\n\
		  return  (keystate & ( 0x01 << ind) ) ~= 0\n\
		end\n\
		function KEYPAD8:TOG( ind )\n\
                  return  (TOG & ( 0x01 << ind) ) ~= 0\n\
	        end\n\
		function KEYPAD8:RES( ind )\n\
		 TOG =  (~(0x01<< ind)) & TOG\n\
		end\n\
	        function KEYPAD8:LED_RED( ind, data)\n\
	  	  led_red = (data) and (led_red | (0x01<<ind)) or (led_red & (~(0x01<<1)))\n\
		end\n\
	        function KEYPAD8:LED_GREEN( ind, data)\n\
	  	  led_green = (data) and (led_red | (0x01<<ind)) or (led_red & (~(0x01<<1)))\n\
		end\n\
	        function KEYPAD8:LED_BLUE( ind, data)\n\
	  	  led_blue = (data) and (led_red | (0x01<<ind)) or (led_red & (~(0x01<<1)))\n\
		end\n\
COUNTER = { }\n\
COUNTER.__index = COUNTER\n\
	     function COUNTER:NEW (a, b, c)\n\
	       local obj = {counter = 0, min = a, max= b, mode = c}\n\
       	       setmetatable (obj, self)\n\
     	       return obj\n\
       	     end\n\
	     function COUNTER:PROCESS( inc, dec, res)\n\
		if inc  then \n\
		   if self.counter < self.max then self.counter = self.counter + 1 elseif self.mode == \"OVER\" then self.counter = self.min end\n\
		end\n\
		if dec then\n\
		   if self.counter > self.min then self.counter = self.counter - 1 elseif self.mode == \"OVER\" then  self.counter = self.max end\n\
		end \n\
		if res  then self.counter = self.min end\n\
         	return self.counter\n\
             end\n\
TIMER = { }\n\
TIMER.__index = TIMER\n\
	 function TIMER:NEW( a,b )\n\
	    local lobj = { timer = 0,  high = b, low = a+b }\n\
	    setmetatable (lobj, self)	 	\n\
     	    return lobj       	\n\
	 end\n\
	 function TIMER:NEWTIC( tic, start )\n\
	   if (tic ~= nil) then \n\
		if self.timer > 0 then self.timer = self.timer + tic elseif start then self.timer = 1 end\n\
		print (self.timer)\n\
                if self.timer > self.low then if start then self.timer = 1 else self.timer = 0 end end\n\
		if (self.timer > 0 ) and ( self.timer <= self.high )   then return true else return false end\n\
				\n\
	    end	\n\
	   return flase\n\
	 end\n\
	 \n\
CAN_OUT = {}\n\
CAN_OUT.__index = CAN_OUT\n\
	function CAN_OUT:NEW(a,b)\n\
 	  local cobj = { timer = 0, addr = a, time = b }\n\
	  setmetatable (cobj, self)	 	\n\
     	  return cobj       		\n\
	end\n\
	function CAN_OUT:PROCESS(tic,d1,d2,d3,d4,d5,d6,d7,d8)\n\
	 if (tic ~=nil) then\n\
	    self.timer = self.time + tic\n\
	    if (self.timer >= self.time) then\n\
		self.timer =  0\n\
		CanSend(self.addr,8,d1,d2,d3,d4,d5,d6,d7,d8)\n\
    	    end\n\
	 end\n\
	end\n\
Out1 = false\n\
Out2 = flase\n\
Out3 = false\n\
Out4 = false \n\
Out5 = false \n\
Out6 = fasle\n\
Out7 = false\n\
Out8 = fasle\n\
\n\
";

static const char* mainscript = "DEV_ID=1\n\
CAN_SPEED=500\n\
OutSetup1 = {10,100,2000}	\n\
OutSetup2 = {10,100,2000}\n\
OutSetup3 = {10,100,2000}\n\
OutSetup4 = {5,100,2000}\n\
OutSetup5 = {5,100,2000}\n\
OutSetup6 = {5,100,2000}\n\
OutSetup7 = {5,100,2000}\n\
OutSetup8 = {10,100,2000}\n\
TDG_PROTOCOL = true\n\
function tests(ms_tic, DI1, DI2, DI3, OutCur1,OutCur2,OutCur3,OutCur4,OutCur5,OutCur6,OutCur7,OutCur8)\n\
LigthCouter = COUNTER:NEW(0,3,\"OVER\")\n\
TurnPWM     = TIMER:NEW(400,400)\n\
Delay       = TIMER:NEW(4000)\n\
CanOut1     = CAN_OUT:NEW(0x505,900)\n\
CanOut2     = CAN_OUT:NEW(0x506,900)\n\
CanOut3     = CAN_OUT:NEW(0x507,900)\n\
CanOut4     = CAN_OUT:NEW(0x615,100)\n\
CanOut5	    = CAN_OUT:NEW(0x615,100)\n\
CanOut6     = CAN_OUT:NEW(0x52F,100)\n\
KeyPad      = KEYPAD8:NEW(21)\n\
while true do\n\
CanOut1:PROCESS( OutCur1>>8, OutCur1, OutCur2>>8, OutCur2, OutCur3>>8,OutCur3, OutCur4>>8,OutCur4)\n\
CanOut2:PROCESS( OutCur5>>8, OutCur5, OutCur6>>8, OutCur6, OutCur7>>8,OutCur7, OutCur8>>8,OutCur8)\n\
CanOut3:PROCESS(DI, OutState, Term1, Term2, Vbat, 0, 0)\n\
KeyPad:PROCESS()\n\
REAR =  CanRead(0x020)\n\
RPM1,RPM2 = CanRead(0x509)\n\
RPM_ON = ( (RPM2<<8) | (RPM1) ) > 0\n\
__,ALARM = CanRead(0x034) \n\
ALARM = ((ALARM>>6) & 0x01)\n\
ALARM =  not (ALARM == 0)\n\
IGNITION   =  DI2 > 0\n\
And7 = KeyPad:TOG(1) and ( not DI1 )\n\
KeyPad:LED_BLUE ( 4,  KEYPAD_TOG_4 )\n\
KeyPad:LED_RED ( 4,  RPM_ON )\n\
Out1 = KeyPad:KEY(1)\n\
KeyPad:LED_GREEN ( 1, Out1 )\n\
KeyPad:LED_RED ( 1,  RPM_ON )\n\
Out2 =  IGNITION and KeyPad:KEY(8)\n\
KeyPad:LED_GREEN ( 8, Out2 )\n\
KeyPad:LED_RED   ( 8, RPM_ON )\n\
And1 =  not DI1 and DI2 and  (not  KeyPad:KEY(8))\n\
And2 =  DI2 and Delay:NEWTIC(ms_tic,KeyPad:KEY(8)) \n\
Out5 =  And1 or And2 or Out2;\n\
TURN_PWM = TurnPWM:NEWTIC(ms_tic,true)\n\
KeyPad:RES(5, KeyPad:KEY(6) or KeyPad:KEY(3) )\n\
KeyPad:RES(6, KeyPad:KEY(5) or KeyPad:KEY(3) )\n\
Out3                = TURN_PWM and (KeyPad:TOG(5) or KeyPad:TOG(7))\n\
KeyPad:LED_GREEN ( 5, Out3 )\n\
KeyPad:LED_RED ( 5,  RPM_ON )\n\
Out7                = TURN_PWM and (KeyPad:TOG(6) or KeyPad:TOG(7))\n\
KeyPad:LED_GREEN ( 6, Out7 )\n\
KeyPad:LED_RED ( 6,  RPM_ON )\n\
KeyPad:LED_GREEN ( 7, not DI2 )\n\
KeyPad:LED_RED ( 7,  RPM_ON or ( Out3 and Out7) )\n\
Out4 		   = (REAR == 1) or KeyPad:TOG(3) or ALARM;\n\
KeyPad:LED_BLUE (3, Out4 )\n\
KeyPad:LED_RED ( 3,  RPM_ON )\n\
LigthState        = LigthCounter:PROCESS(KeyPad:KEY(2),nil, not DI2)\n\
KeyPad:LED_RED ( 2,  RPM_ON )\n\
Out8		    = ( LigthState == 1 ) or ( LigthState == 2 )\n\
KeyPad:LED_GREEN ( 2,  ( LigthState == 3 ) or ( LigthState == 1 ) )\n\
Out6		    = ( LigthState == 2)\n\
KeyPad:LED_BLUE ( 2, Out6 )\n\
CanOut4:PROCESS(0x2F,0x03,0x20,0x02,( 0x00 | BToI(And7) <<5 | BToI(KeyPad:TOG(1))<<4))\n\
CanOut5:PROCESS(0x2F,0x03,0x20,0x01,( 0x04 | BToI(And7) <<5 ),0x00,0x00,0x00)\n\
CanOut6:PROCESS(DI,0x00,0x00,0x00,0x00,0x00,0x00,0x00)\n\
ms_tic,DI1,DI2,DI3, OutCur1,OutCur2,OutCur3, OutCur4,OutCur5,OutCur6,OutCur7,OutCur8 = coroutine.yield(Out1,Out2,Out3,Out4,Out5,Out6,Out7,Out8)\n\
end end\n\
\n\
";
