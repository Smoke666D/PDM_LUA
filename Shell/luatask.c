/*
 * luatask.c
 *
 *  Created on: Apr 6, 2022
 *      Author: igor.dymov
 */

#include "luatask.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "FreeRTOS.h"
#include "script.c"


void vLuaTask(void *argument)
{

	// lua_State *L = luaL_newstate(); //Create new LUA state
   //  if (L == NULL)
   //  {

  //   }
  //   vTaskDleay(10);
 //    luaL_openlibs(L); // open standard libraries
 //    luaL_loadstring(L, lib );
 //    luaL_loadstring(L, mainscript);
	 while(1)
	 {
		  vTaskDelay(1 );
	 }
	/* (void) pvParameters;

	  lua_State *L = luaL_newstate();
	   if (L == NULL)
	 {
	   lua_writestringWithoutsize("cannot create state: not enough memory");
	     while(1){vTaskDelay(1000);}
	   }
	   else
	   {



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
	     }*/
}
