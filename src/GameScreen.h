#pragma once

#include <lua/lua.hpp>

#include <SFML/Graphics.hpp>

#include "Scenes/Scene.h"

#include "Screen.h"
#include "AnimatedSprite.h"
#include "Player.h"

namespace tt
{

constexpr auto GAMESCREEN_LUA_IDX = 3;

template<typename T>
void initLua(lua_State* L, T& screen)
{
    luaL_openlibs(L);

    // push a reference to `this` into the registry, it should
    // always be the 3rd entry
    lua_pushlightuserdata(L, static_cast<void*>(&screen));
    luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    [[maybe_unused]] int reference = luaL_ref(L, LUA_REGISTRYINDEX);
    assert(GAMESCREEN_LUA_IDX == reference);

    //luaL_newmetatable(_luaState, "GameScreen");
    //lua_pushstring(_luaState, "__index");
    //lua_pushvalue(_luaState, -2); // push the metatable
    //lua_settable(_luaState, -3);  // metatable.__index = metatable

    // create the 'Scene' Lua class
    {
        luaL_newmetatable(L, Scene::CLASS_NAME);
        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2); // push the metatable
        lua_settable(L, -3);  // metatable.__index = metatable

        // this creates object-like methods by populating the table
        // on the stack with the function names/pointers
        luaL_openlib(L, nullptr, Scene::LuaMethods, 0);
            
        // clear the stack
        lua_settop(L, 0);
    }

    // create the 'Scene' Lua class
    {
        luaL_newmetatable(L, Player::CLASS_NAME);
        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2); // push the metatable
        lua_settable(L, -3);  // metatable.__index = metatable

        // this creates object-like methods by populating the table
        // on the stack with the function names/pointers
        luaL_openlib(L, nullptr, Player::LuaMethods, 0);
            
        // clear the stack
        lua_settop(L, 0);
    }
}

class GameScreen final : public Screen
{

public:
    using SceneMap = std::map<std::string, SceneSharedPtr>;

    static GameScreen* l_get(lua_State* L);

    GameScreen(ResourceManager& resmgr, sf::RenderTarget& target);
    ~GameScreen();

    void draw() override;
    PollResult poll(const sf::Event&) override;
    ScreenAction timestep() override;

    lua_State* lua() const { return _luaState; }
    const SceneMap& scenes() const { return _scenes; }

private:
    SceneSharedPtr  _currentScene;
    SceneMap        _scenes;
    PlayerPtr       _player;
    lua_State*      _luaState;
};

} // namespace tt