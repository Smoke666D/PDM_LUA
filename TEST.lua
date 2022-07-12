
Out1 = true 
Out2 = false 
Out3 = false 
Out4 = false 
Out5 = false 
Out6 = false 
Out7 = false 
Out8 = false 
Out9 = false 
Out10 = false
Out11 = false 
Out12 = false 
Out13 = true 
Out14 = false 
Out15 = false 
Out16 = false 
Out17 = false 
Out18 = false 
Out19 = false 
Out20 = false 
timer = 0

KEYPAD8 = {}
KEYPAD8.__index = KEYPAD8

function KEYPAD8:NEW( addr)
      local obj = {key = 0x00, ADDR = addr, new = false,  tog= 0x00, old =0x00, led_red=0x00,led_green=0x00, led_blue =0x00, temp={0}}
      setmetatable (obj, self) 
      SetCanFilter(0x180 +addr)
      return obj
end
function KEYPAD8:PROCESS()
	if (GetCanToTable(0x180 + self.ADDR,self.temp) ==1 ) then  
		self.tog = (~ self.key & self.temp[1]) ~ self.tog	
		self.key =self.temp[1]	 
	end
	if self.new == true then
		self.new = false		
		CanSend(0x215,self.led_red,self.led_green,self.led_blue,0,0,0,0,0)
	end
end
function KEYPAD8:KEY( ind )
	  return  (self.key & ( 0x01 << (ind-1)) ) ~= 0 
end
function KEYPAD8:TOG( ind ) 
         return  (self.tog & ( 0x01 << (ind-1)) ) ~= 0 
end
function KEYPAD8:RES( ind )
	 self.tog =  (~(0x01<< (ind-1))) & self.tog
end
function KEYPAD8:LED_RED( ind, data)
	 if (data == false) then 
		self.led_red = self.led_red & (~(0x01<<(ind-1))) 
	 else 
		self.led_red = self.led_red | (0x01<<(ind-1)) 
	 end
	 self.new = true
end
function KEYPAD8:LED_GREEN( ind, data)
 	 if (data == false) then 
		self.led_green = self.led_green & (~(0x01<<(ind-1))) 
	 else 
		self.led_green = self.led_green | (0x01<<(ind-1)) 
	 end    
         self.new = true
end
function KEYPAD8:LED_BLUE( ind, data)
	 if (data == false) then 
		self.led_blue = self.led_blue & (~(0x01<<(ind-1))) 
	 else 
		self.led_blue = self.led_blue | (0x01<<(ind-1)) 
	 end
	 self.new = true
end

TURNS_LIGTH = {}
TURNS_LIGTH.__index = TURNS_LIGTH
function TURNS_LIGTH:NEW()
		       local obj = { in_left = false, in_rigth = false, in_alarm = false,  out_left = false, out_rigth, out_alarm =false, timer =0, clock = false}
		       	       setmetatable (obj, self)
						return obj
						end
				function TURNS_LIGTH:PROCESS( tim ,ign )
					self.timer = self.timer + tim 
					if (ign == true) then
						if (self.timer > 500) then 
							 self.clock = not self.clock 
							 self.timer=0 
						end
						self.out_alarm = false 
						self.out_left  = false 
						self.out_rigth = false				
						if (self.in_alarm == true) then  
							self.out_alarm = self.clock  
						else 
							self.out_rigth = self.clock and  self.in_rigth
							self.in_left   = self.clock and  self.in_left														

						end  
					end
				end
function stop() timer = coroutine.yield(Out20,Out19,Out18,Out17,Out16,Out15,Out14,Out13,Out12,Out11,Out10,Out9,Out8,Out7,Out6,Out5,Out4,Out3,Out2,Out1) end
main = function () 
	OutConfig(10,10,1000,60) 
	KeyPad =  KEYPAD8:NEW(0x15) 
	al     = TURNS_LIGTH:NEW()  
	k=3  
	i=0 
	t=false
	while true do 
		OutSetPWM(13,k) 
		KeyPad:PROCESS()
		al:PROCESS(timer,true)  		
		if  KeyPad:TOG(2) == true then
		    KeyPad:RES(1)
		    KeyPad:RES(3)
		end
		al.in_alarm = KeyPad:TOG(2) 
		al.in_left  = KeyPad:TOG(1) 
		al.in_rigth = KeyPad:TOG(3) 		 
		KeyPad:LED_RED(1,al.out_left) 
		KeyPad:LED_RED(2,al.out_alarm) 
		KeyPad:LED_RED(3,al.out_rigth)		
	        i = i + timer  
		if (i > 500) then 
			i = 0 
		        k = k+1
		        if k >8 then 
				k = 3 
				t = not t
			else 
				KeyPad:LED_GREEN(k,t) 
			end  
		end  
		stop()
 	end 
end