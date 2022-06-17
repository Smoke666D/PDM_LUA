

--[[Вызовы С функций]]
function CanSend(addr, count, b1,b2,b3,b4,b5,b6,b7,b8)

end

function CanRead(addr)

return 0,0,0,0,0,0,0,0
end

--[[Глобальные переменные для обмена]]

function BToI( a)
 if a == true then return 0x01 else return 0x00 end
end

KEY = {{KEY = false,TOG = false,RES = flase},{KEY = false,TOG = false,RES = flase},{KEY = false,TOG = false,RES = flase},{KEY = false,TOG = false,RES = flase},{KEY = false,TOG = false,RES = flase},{KEY = false,TOG = false,RES = flase},{KEY = false,TOG = false,RES = flase},{KEY = false,TOG = false,RES = flase}}
KEYPAD_LED = {{RED =0, GREEN=0,BLUE=0},{RED =0, GREEN=0,BLUE=0},{RED =0, GREEN=0,BLUE=0},{RED =0, GREEN=0,BLUE=0},{RED =0, GREEN=0,BLUE=0},{RED =0, GREEN=0,BLUE=0},{RED =0, GREEN=0,BLUE=0},{RED =0, GREEN=0,BLUE=0}}
Out ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
OutState = 0
DI = {0,0,0}
Vbat = 0
Term1 =0
Term2 =0
--[[Ноды]]



function KeyPad(adr)
local keystate = CanRead(180+adr)
if (keystate & 0x80) ~=0 then KEY[1].KEY = true else KEY[1].KEY = false end
if (keystate & 0x40) ~=0 then KEY[2].KEY = true else KEY[2].KEY = false end
if (keystate & 0x20) ~=0 then KEY[3].KEY = true else KEY[3].KEY = false end
if (keystate & 0x10) ~=0 then KEY[4].KEY = true else KEY[4].KEY = false end
if (keystate & 0x08) ~=0 then KEY[5].KEY = true else KEY[5].KEY = false end
if (keystate & 0x04) ~=0 then KEY[6].KEY = true else KEY[6].KEY = false end
if (keystate & 0x02) ~=0 then KEY[7].KEY = true else KEY[7].KEY = false end
if (keystate & 0x01) ~=0 then KEY[8].KEY = true else KEY[8].KEY = false end
if KEY[1].RES  then  KEY[1].RES = false; KEY[1].TOG = false end
if KEY[2].RES  then  KEY[2].RES = false; KEY[2].TOG = false end
if KEY[3].RES  then  KEY[3].RES = false; KEY[3].TOG = false end
if KEY[4].RES  then  KEY[4].RES = false; KEY[4].TOG = false end
if KEY[5].RES  then  KEY[5].RES = false; KEY[5].TOG = false end
if KEY[6].RES  then  KEY[6].RES = false; KEY[6].TOG = false end
if KEY[7].RES  then  KEY[7].RES = false; KEY[7].TOG = false end
if KEY[8].RES  then  KEY[8].RES = false; KEY[8].TOG = false end
if KEY[1].KEY  then  KEY[1].TOG = not KEY[1].TOG end
if KEY[2].KEY  then  KEY[2].TOG = not KEY[2].TOG end
if KEY[3].KEY  then  KEY[3].TOG = not KEY[3].TOG end
if KEY[4].KEY  then  KEY[4].TOG = not KEY[4].TOG end
if KEY[5].KEY  then  KEY[5].TOG = not KEY[5].TOG end
if KEY[6].KEY  then  KEY[6].TOG = not KEY[6].TOG end
if KEY[7].KEY  then  KEY[7].TOG = not KEY[7].TOG end
if KEY[8].KEY  then  KEY[8].TOG = not KEY[8].TOG end
led_red   =  BToI(KEYPAD_LED[1].RED) | BToI(KEYPAD_LED[2].RED) <<1 | BToI(KEYPAD_LED[3].RED) <<2 |  BToI(KEYPAD_LED[4].RED) <<3 | BToI(KEYPAD_LED[5].RED) <<4 | BToI(KEYPAD_LED[6].RED) <<5 | BToI(KEYPAD_LED[7].RED) <<6 | BToI(KEYPAD_LED[8].RED) <<7  
led_green =  BToI(KEYPAD_LED[1].GREEN) | BToI(KEYPAD_LED[2].GREEN) <<1 | BToI(KEYPAD_LED[3].GREEN) <<2 |  BToI(KEYPAD_LED[4].GREEN) <<3 | BToI(KEYPAD_LED[5].GREEN) <<4 | BToI(KEYPAD_LED[6].GREEN) <<5 | BToI(KEYPAD_LED[7].GREEN) <<6 | BToI(KEYPAD_LED[8].GREEN) <<7  
led_blue  =  BToI(KEYPAD_LED[1].BLUE) | BToI(KEYPAD_LED[2].BLUE) <<1 | BToI(KEYPAD_LED[3].BLUE) <<2 |  BToI(KEYPAD_LED[4].BLUE) <<3 | BToI(KEYPAD_LED[5].BLUE) <<4 | BToI(KEYPAD_LED[6].BLUE) <<5 | BToI(KEYPAD_LED[7].BLUE) <<6 | BToI(KEYPAD_LED[8].BLUE) <<7  
CanSend(300+adr,led_red,led_green,led_blue)
end

c ={}
function Counter(name, inc, dec, res, max, min, def, mode)
	local min = min or 0
	local max = max or 1
	local mode =mode or "LIMIT"
	if c[name] == nil then  c[name] = ( def or 0 ) end               
	if  inc  then if c[name] < max then c[name] = c[name] + 1 elseif mode == "OVER" then  c[name] = min end end 
	if  dec  then if c[name] > min then c[name] = c[name] - 1 elseif mode == "OVER" then  c[name] = max end end 
	if  res  then c[name] = min end 
	return c[name]
end

timer = 0
t={}
function Timer(name, high, low, start )
            
      if t[name] == nil then t[name] = 0 end  --Создаем новый объект, если его еще нет
      if t[name] > 0 then  t[name] = t[name] + timer elseif start == true then t[name] = 1 end --Если счет уже идет, обновляем таймер, если нет, то смотрим на вход запуска и запускаем таймер
      if t[name] > high then if (start == true) then t[name] = 1 else t[name] = 0 end end --Если переполение, то смотрим на вход запуска. Если true запускаем заново, если нет то останавливаем таймер
      if ( t[name] > low)  and ( t[name] < high ) then return true  elseif t[name] < low then return false end
end


ct={}
function CanOut(addr,time, b1,b2,b3,b4,b5,b6,b7,b8)
     if ct[addr] == nil then ct[addr] = 0 else ct[addr] = ct[addr] + timer end
     if ct[addr] > time then ct[addr] = 0 end
     if ct[addr] ==0 then  CanSend(addr, 8, b1,b2,b3,b4,b5,b6,b7,b8) end     	
end

function CompareConst(a, b , c)

 if b == "=" then  if a == c then return true else return false end end
 if b == ">" then  if a > c then return true else return false end end
 if b == "<" then  if a < c then return true else return false end end
end

function Inv( a)
 if a == 0 then return false  else return true end
end

Out1 = Out[1]
OutCur1 = Out[2]
Out2 = Out[3]
OutCur2 = Out[4]
Out3 = Out[5]
OutCur3 = Out[6]
Out4 = Out[7]
OutCur4 = Out[8]
Out5 = Out[9]
OutCur5 = Out[10]
Out6 = Out[11]
OutCur6 = Out[12]
Out7 = Out[13]
OutCur7 = Out[14]
Out8 = Out[15]
OutCur8 = Out[16]

--[[Теперь начинается пользовательская часть скрипта ]]


DEV_ID=1	        --Номер блока в сети, 
CAN_SPEED=500		--задаем скорость CAN шины 250, 500 или 1000

OutSetup1 = {10,100,2000}	
OutSetup2 = {10,100,2000}
OutSetup3 = {10,100,2000}
OutSetup4 =  {5,100,2000}
OutSetup5 =  {5,100,2000}
OutSetup6 =  {5,100,2000}
OutSetup7 =  {5,100,2000}
OutSetup8 = {10,100,2000}

TDG_PROTOCOL = true

do


CanOut(0x505,900, OutCur1>>8, OutCur1, OutCur2>>8, OutCur2, OutCur3>>8,OutCur3, OutCur4>>8,OutCur4)
CanOut(0x506,900, OutCur5>>8, OutCur5, OutCur6>>8, OutCur6, OutCur7>>8,OutCur7, OutCur8>>8,OutCur8)
CanOut(0x507,900, DI, OutState, Term1, Term2, Vbat, 0, 0)
KeyPad(21)

REAR =  CanRead(0x020)

RPM1,RPM2 = CanRead(0x509)
RPM = (RPM2<<8) | (RPM1)
if RPM > 0 then RPM_ON  = true else RPM_ON = false end



temp,ALARM = CanRead(0x034) 
ALARM = ((ALARM>>6) & 0x01)
if ALARM == 0 then ALARM = false else ALARM = true end

if DI[2]>0 then IGNITION = true else IGNITION = false  end


Inv5 = Inv(DI[1])
And7= KEY[1].TOG and Inv5


KEYPAD_LED[4].BLUE =  KEY[4].TOG;  --Вентилятор
KEYPAD_LED[4].RED  =  RPM_ON


--Звуковой сигнал. По нажатию клавиши вылючаем выход, и зажигаем зеленый светодиод. Если нет сигнала обортов, то светодиод красный.
Out1 = KEY[1].KEY
KEYPAD_LED[1].GREEN = Out1;  
KEYPAD_LED[1].RED   = RPM_ON

--Омываетль. По нажатию  
Out2 =  IGNITION and KEY[8].KEY;
KEYPAD_LED[8].GREEN = Out2;  
KEYPAD_LED[8].RED   = RPM_ON

--Дворнии
And1 =  Inv5 and DI[2] and Inv(KEY[8].KEY);
And2 =  DI[2] and Timer("Timer4",4000,0,KEY[8].KEY) 
Out5= And1 or And2 or Out2;                 


--Поворотники аварийка
TURN_PWM = Timer("TurnPWN",400,400,true)

KEY[5].RES = KEY[6].KEY or KEY[3].KEY
KEY[6].RES = KEY[5].KEY or KEY[3].KEY

Out3=TURN_PWM and (KEY[5].TOG or KEY[7].TOG)       --левый поворот
KEYPAD_LED[5].GREEN = Out3;  
KEYPAD_LED[5].RED = RPM_ON

Out7= TURN_PWM and (KEY[6].TOG or KEY[7].TOG);     --правый поворот
KEYPAD_LED[6].GREEN = Out7;  --правый поворот
KEYPAD_LED[6].RED = RPM_ON

KEYPAD_LED[7].GREEN = Inv(DI[2])  --аварийка
KEYPAD_LED[7].RED = RPM_ON or ( Out3 and Out7)

--Задний ход
Out4 = CompareConst(REAR,'=',1) or KEY[3].TOG or ALARM;
KEYPAD_LED[3].BLUE =  Out4
KEYPAD_LED[3].RED = RPM_ON

--свет
LigthState = Counter("LigthCounter",KEY[2].KEY,nil,Inv(DI[2]),0,3,"OVER")
KEYPAD_LED[2].RED = RPM_ON

--ближний свет
Out8= CompareConst(LigthState,'=',1) or CompareConst(LigthState,'=',2)  
KEYPAD_LED[2].GREEN = CompareConst(LigthState,'=',3) or CompareConst(LigthState,'=',1)
--дальний свет
Out6= CompareConst(LigthState,'=',2); 
KEYPAD_LED[2].BLUE =  Out6;  

CanOut(0x615,100,0x2F,0x03,0x20,0x02,( 0x00 | BToI(And7) <<5 | BToI(KEY[1].TOG)<<4))
CanOut(0x615,100,0x2F,0x03,0x20,0x01,( 0x04 | BToI(And7) <<5 ),0x00,0x00,0x00)
CanOut(0x52F,100,DI,0x00,0x00,0x00,0x00,0x00,0x00,0x00)

end



















 