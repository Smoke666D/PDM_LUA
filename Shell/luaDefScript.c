#include "luaDefScript.h"
const char* const defaultLuaScript = " delayms=0 DOut={flase,true,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase}DInput={flase,true,flase,flase,flase,flase,flase,flase,flase,flase,flase,flase}Cur={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}function Yield()delayms,DInput[1],DInput[2],DInput[3],DInput[4],DInput[5],DInput[6],DInput[7],DInput[8],DInput[9],DInput[10],DInput[11],DInput[12],Cur[1],Cur[2],Cur[3],Cur[4],Cur[5],Cur[6],Cur[7],Cur[8],Cur[9],Cur[10],Cur[11],Cur[12],Cur[13],Cur[14],Cur[15],Cur[16],Cur[17],Cur[18],Cur[19],Cur[20]=coroutine.yield(DOut[20],DOut[19],DOut[18],DOut[17],DOut[16],DOut[15],DOut[14],DOut[13],DOut[12],DOut[11],DOut[10],DOut[9],DOut[8],DOut[7],DOut[6],DOut[5],DOut[4],DOut[3],DOut[2],DOut[1])end function getDelay()return delayms end function getDIN(ch)return DInput[ch] end function getCurrent(ch)return Cur[ch] end function getCurrentBase(ch,b)return Cur[ch]//1,(Cur[ch]%1)*10*b//1 end function setOut(ch,data)if ch<=20 then DOut[ch]=data; end end KeyPad8={}KeyPad8.__index=KeyPad8 function KeyPad8:new(addr)local obj={key=0x00,ADDR=addr,new=false,tog=0x00,old=0x00,ledRed=0x00,ledGreen=0x00,ledBlue=0x00,temp={0}}setmetatable(obj,self)SetCanFilter(0x180+addr)return obj end function KeyPad8:process()if(GetCanToTable(0x180+self.ADDR,self.temp)==1)then self.tog=(~self.key&self.temp[1])~self.tog self.key=self.temp[1] end if self.new==true then self.new=false CanSend(0x200+self.ADDR,self.ledRed,self.ledGreen,self.ledBlue,0,0,0,0,0)end end function KeyPad8:getKey(n)return(self.key&(0x01<<(n-1)))~=0 end function KeyPad8:getToggle(n)return(self.tog&(0x01<<(n-1)))~=0 end function KeyPad8:resetToggle(n,state)if state==true then self.tog=(~(0x01<<(n-1)))&self.tog end end function KeyPad8:setLedRed(n,state)self.old=(state)and self.ledRed|(0x01<<(n-1))or self.ledRed&(~(0x01<<(n-1)))if(self.old~=self.ledRed)then self.ledRed=self.old self.new=true end end function KeyPad8:setLedGreen(n,state)self.old=(state)and self.ledGreen|(0x01<<(n-1))or self.ledGreen&(~(0x01<<(n-1)))if(self.old~=self.ledGreen)then self.ledGreen=self.old self.new=true end end function KeyPad8:setLedBlue(n,state)self.old=(state)and self.ledBlue|(0x01<<(n-1))or self.ledBlue&(~(0x01<<(n-1)))if(self.old~=self.ledBlue)then self.ledBlue=self.old self.new=true end end function KeyPad8:setLedWhite(n,state)self:setLedBlue(n,state)self:setLedGreen(n,state)self:setLedRed(n,state)end function KeyPad8:setLedBrigth(brigth)CanSend(0x600+self.ADDR,0x2F,0x03,0x20,0x01,brigth,0,0,0)end function KeyPad8:setBackLigthBrigth(brigth)CanSend(0x600+self.ADDR,0x2F,0x03,0x20,0x02,brigth,0,0,0)end function CanSendTable(addr,p)end function getBit(data,pos)if(data&(0x01<<pos-1))then return true else return false end end CanInput={}CanInput.__index=CanInput function CanInput:new(addr)local obj={ADDR=addr,data={0,0,0,0,0,0,0,0}}setmetatable(obj,self)return obj end function CanInput:process()GetCanToTable(self.ADDR,self.data)end function CanInput:getBit(nb,nbit)return getBit(self.data[nb],nbit)end function CanInput:getByte(nb)return self.data[nb] end function CanInput:getWord(nb)return self.data[nb]<<8|self.data[nb+1] end CanOut={}CanOut.__index=CanOut function CanOut:new(addr,time,size,d1,d2,d3,d4,d5,d6,d7,d8)local obj={ADDR=addr,data={d1,d2,d3,d4,d5,d6,d7,d8},delay=time,timer=0,sz=size}setmetatable(obj,self)return obj end function CanOut:process()self.timer=self.timer+getDelay()if self.timer>=self.delay then CanSendTable(self.ADDR,self.data)self.timer=0 end end function CanOut:setFrame(...)local arg=table.pack(...)if arg.n<=8 then for i=1,arg.n do self.data[i]=arg[i] end self.sz=arg.n end end function CanOut:setBit(nb,nbit,state)if state==true then self.data[nb]=self.data[bn]|(0x01<<(nbit-1))else self.data[nb]=self.data[bn]&~(0x01<<(nbit-1))end end function CanOut:setByte(nb,state)self.data[nb]=state&0xFF end function CanOut:setWord(nb,state)self.data[nb]=(state<<8)&0xFF self.data[nb+1]=state&0xFF end Wipers={}Wipers.__index=Wiper function Wipers:new(pause,count)local obj={p=pause,c=count,out=false,timer=0,p_work=false,p_count=0,fsm=0}setmetatable(obj,self)return obj end function Wipers:process(ign,start,pause,location)if ign==true then if(pause==ture)then if self.fsm==0 then if location then self.p_work=true self.fsm=1 end elseif self.fsm==1 then if(not location)then self.fsm=2 end elseif self.fsm==2 then if location then self.p_count=self.p_count+1 if self.p_count>=self.c then self.fsm=3 self.p_work=flase else self.fsm=2 end end elseif self.fsm==3 then self.timer=self.timer+getDelay()if(self.timer>self.p)then self.timer=0 self.fsm=0 end end else self.fsm=0 self.timer=0 self.p_count=0 p_work=false end self.out=start or p_work or(not location)else self.out=false or(not location)end end function Wipers:getOut()return self.out end Counter={}Counter.__index=Counter function Counter:new(inMin,inMax,inDefault,inReload)local obj={counter=(type(inDefault)==\"number\")and inDefault or 0,min=(type(inMin)==\"number\")and inMin or 0,max=(type(inMax)==\"number\")and inMax or 0xFFFF,reload=(type(inReload)==\"boolean\")and inReload or true,old=false}setmetatable(obj,self)return obj end function Counter:process(inc,dec,rst)if(type(inc)==\"boolean\")and(type(dec)==\"boolean\")and(type(rst)==\"boolean\")then if(inc==true)then if(self.old==false)then if(self.counter<self.max)then self.counter=self.counter+1 elseif(self.reload==true)then self.counter=self.min end end end if(dec==true)then if(self.old==false)then if(self.counter>self.min)then self.counter=self.counter-1 elseif(self.reload==true)then self.counter=self.max end end end if(rst==true)then if(self.old==false)then self.counter=self.min end end self.old=(rst or inc or dec)and true or false end return end function Counter:get()return self.counter end Delay={}Delay.__index=Delay function Delay:new(inDelay)local obj={counter=0,launched=false,output=false,delay=inDelay}setmetatable(obj,self)return obj end function Delay:process(start)if(start==true)then if(self.counter>=0)then self.output=false end self.counter=self.counter+getDelay()if(self.counter>=self.delay)then self.output=true self.counter=0 end end return end function Delay:get()return self.output end TurnSygnals={}TurnSygnals.__index=TurnSygnals function TurnSygnals:new(inDelay)local obj={delay=(type(inDelay)==\"number\")and inDelay or 100,counter=0,state=true,outLeft=false,outRight=false,outAlarm=false}setmetatable(obj,self)return obj end function TurnSygnals:process(enb,left,right,alarm)if(type(enb)==\"boolean\")and(type(left)==\"boolean\")and(type(right)==\"boolean\")and(type(alarm)==\"boolean\")then if left or right or alarm then self.counter=self.counter+getDelay()if(self.counter>self.delay)then self.state=not self.state self.counter=0 end else self.state=true self.counter=0 end self.outLeft=left and self.state and enb and(not alarm)self.outRight=right and self.state and enb and(not alarm)self.outAlarm=alarm and self.state and enb end end function TurnSygnals:getRight()return self.outRight end function TurnSygnals:getLeft()return self.outLeft end function TurnSygnals:getAlarm()return self.outAlarm end main=function()OutConfig(13,8,0,10)SetDINConfig(1,1)SetDINConfig(2,1)SetDINConfig(3,1)SetDINConfig(4,1)SetDINConfig(5,1)SetDINConfig(6,1)SetDINConfig(7,1)SetDINConfig(8,1)SetDINConfig(9,1)SetDINConfig(10,1)SetDINConfig(11,1)SetDINConfig(12,1)KeyPad=KeyPad8:new(0x15)TurnSygnal=TurnSygnals:new(500)Delay500ms=Delay:new(500)LigthCounter=Counter:new(1,4,1,true)WiperCounter=Counter:new(1,3,1,true)CAN_OUT1=CanOut:new(0x505,900,8,0,0,0,0,0,0,0,0)CAN_OUT2=CanOut:new(0x506,900,8,0,0,0,0,0,0,0,0)CAN_OUT3=CanOut:new(0x507,900,8,0,0,0,0,0,0,0,0)CAN_CH1=CanInput:new(0x020)CAN_CH2=CanInput:new(0x509)CAN_ALARM=CanInput:new(0x034)Wiper=Wipers:new(2000,3)wiperstart=false wiperpause=false lowbeam=false highbeam=false t=true temp=false while true do KeyPad:process()Delay500ms:process(true)CAN_CH1:process()CAN_CH2:process()CAN_ALARM:process()CAN_OUT1:process()CAN_OUT2:process()CAN_OUT3:process()CAN_OUT1:setFrame(1,2,3,4)KeyPad:setBackLigthBrigth(0x33)setOut(1,KeyPad:getKey(1))KeyPad:setLedGreen(1,KeyPad:getKey(1))LigthCounter:process(KeyPad:getKey(2),false,false)lowbeam=(LigthCounter:get()==2)highbeam=(LigthCounter:get()==4)setOut(8,lowbeam)setOut(6,highbeam)KeyPad:setLedGreen(2,lowbeam)KeyPad:setLedBlue(2,highbeam)WiperCounter:process(KeyPad:getKey(8),false,false)wiperstart=(WiperCounter:get()==2)wiperpause=(WiperCounter:get()==3)setOut(2,KeyPad:getKey(8)and getDIN(2))KeyPad:setLedGreen(8,wiperstart)KeyPad:setLedBlue(8,wiperpause)TurnSygnal:process(true,KeyPad:getToggle(5),KeyPad:getToggle(6),KeyPad:getToggle(7))KeyPad:resetToggle(5,KeyPad:getToggle(7)or KeyPad:getKey(6))KeyPad:resetToggle(6,KeyPad:getToggle(7)or KeyPad:getKey(5))KeyPad:setLedWhite(5,TurnSygnal:getLeft())KeyPad:setLedWhite(6,TurnSygnal:getRight())KeyPad:setLedRed(7,TurnSygnal:getAlarm())KeyPad:setLedGreen(7,TurnSygnal:getAlarm())setOut(3,TurnSygnal:getLeft()or TurnSygnal:getAlarm())setOut(7,TurnSygnal:getRight()or TurnSygnal:getAlarm())setOut(13,TurnSygnal:getAlarm()or TurnSygnal:getLeft()or TurnSygnal:getRight())KeyPad:setLedGreen(3,KeyPad:getToggle(3))KeyPad:setLedGreen(4,KeyPad:getToggle(4))Yield()end end";
