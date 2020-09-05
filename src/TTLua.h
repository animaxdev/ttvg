#pragma once
#include <string>
#include <iostream>

#include <fmt/core.h>

#include <lua/lua.hpp>

namespace tt
{

constexpr auto GAMESCREEN_LUA_IDX = 3;
constexpr auto ITEMFACTORY_LUA_IDX = 4;

template<typename T>
[[maybe_unused]] T* checkObject(lua_State* L)
{
    auto temp = static_cast<T**>(luaL_checkudata(L, 1, T::CLASS_NAME));
    return *temp;
}

template<typename T>
[[maybe_unused]] T* checkSharedObject(lua_State* L)
{
    using SharedT = std::shared_ptr<T>;
    auto temp = static_cast<T**>(luaL_checkudata(L, 1, T::CLASS_NAME));
    return *temp;
}
    
void CallLuaFunction(lua_State* L, 
    const std::string& funcname, 
    const std::string& envname, 
    int objIdx);

template<typename ClassT>
void registerLuaFunctions(lua_State* L)
{
    luaL_newmetatable(L, ClassT::CLASS_NAME);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2); // push the metatable
    lua_settable(L, -3);  // metatable.__index = metatable
    

    // this creates object-like methods by populating the table
    // on the stack with the function names/pointers
    // luaL_openlib(L, nullptr, ClassT::LuaMethods, 0);
    luaL_setfuncs(L, ClassT::LuaMethods, 0);
        
    // clear the stack
    lua_settop(L, 0);
}

inline static void dumpstack(lua_State* L) 
{
    int top=lua_gettop(L);
    for (int i=1; i <= top; i++) 
    {
        const std::string line = fmt::format("{:3}\t{:4}\t{:15}", i, ((i-top) - 1), luaL_typename(L, i));
        switch (lua_type(L, i)) 
        {
            case LUA_TNUMBER:
                std::cout << fmt::format("{}\t{}", line, lua_tonumber(L,i));
            break;
            case LUA_TSTRING:
                std::cout << fmt::format("{}\t'{}'", line, lua_tostring(L,i));
            break;
            case LUA_TBOOLEAN:
                std::cout << fmt::format("{}\t{:boolalpha}", line, lua_toboolean(L,i));
            break;
            case LUA_TNIL:
                std::cout << fmt::format("{}\tnil", line);
            break;
            default:
                std::cout << fmt::format("{}\t{}", line, lua_topointer(L,i));
            break;
        }
        std::cout << '\n';
    }
}

}