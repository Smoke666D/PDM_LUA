/*
 * luatask.c
 *
 *  Created on: Apr 6, 2022
 *      Author: igor.dymov
 */

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"


static const char* script1="function script1Function ()\n\
	print('\\nscript1: ' .. tostring(uc.counter()) ..'\\n')\n\
	includedScript()\n\
	local example = 'an example string'\n\
	for el in string.gmatch(example, '%S+') do\n\
		print(el .. ' ')\n\
	end\n\
	print('\\n')\n\
	local result1, result2 = uc.func1('Testmessage')\n\
	print('result1: '.. tostring(result1) .. '  result2: '.. tostring(result2) .. '\\n')\n\
	local array = {0,1,2,3,4}\n\
	local resultTable = uc.func2(array)\n\
	print('test print:', 'string', 3, '\\n')\n\
	print('resultTable1: '.. tostring(resultTable[0]) .. '  resultTable2: '.. tostring(resultTable[1]) .. '\\n')\n\
	print('heap usage before collect: ' .. tostring(collectgarbage('count')*1024) .. '\\n')\n\
	collectgarbage('collect')\n\
	print('heap usage after collect: ' .. tostring(collectgarbage('count')*1024) .. '\\n')\n\
end\n\
for loopCounter=0,2 do\n\
	uc.scriptSemaphore(1)\n\
	print(pcall(script1Function), '\\n')\n\
	uc.scriptSemaphore(0)\n\
	uc.taskDelay(1)\n\
end";


void vLuaTask(void *argument)
{

	 lua_State *L = luaL_newstate();

	 //luaL_dostring
	 while(1)
	 {

	 }
	 (void) pvParameters;

	   lua_State *L = luaL_newstate(); /* create state */
	   if (L == NULL)
	   {
	     lua_writestringWithoutsize("cannot create state: not enough memory");
	     while(1){vTaskDelay(1000);}
	   }
	   else
	   {

	     luaL_openlibs(L); /* open standard libraries */

	     if (dostring(L, helperScript, "helperScript") != LUA_OK)
	     {while(1){vTaskDelay(1000);};}

	     lua_gc(L, LUA_GCSTOP, 0); //stop the automatic garbage collection

	     while (1)
	     {
	       xSemaphoreTakeRecursive(g_scriptSemaphore, portMAX_DELAY);

	       if (dostring(L, script2, "script2") != LUA_OK)
	       {while(1){vTaskDelay(1000);};}

	       lua_gc(L, LUA_GCCOLLECT, 0); //collect garbage

	       xSemaphoreGive(g_scriptSemaphore);
	     }
}
