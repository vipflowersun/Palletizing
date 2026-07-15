#pragma once
#include "LuaManager.h"

namespace LuaFunction
{
	int sleep(lua_State *L);
	int print(lua_State* L);
	
	int plan(lua_State* L);	//单层规划
	int palletLevel(lua_State* L);	//指定层码垛
	int getTotalLevel(lua_State* L);	//获取单个垛的总层数
}