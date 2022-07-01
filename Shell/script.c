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


