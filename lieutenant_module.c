#include <lua.h>
#include <lauxlib.h>

#include "lieutenant.h"


static const char *lt_ll_lfuncreader(lua_State *L, void *data, size_t *size);
static int lt_ll_readstring(lua_State *L);
static int lt_ll_readlfile(lua_State *L);


int lt_ll_create(lua_State *L)
{
    lua_createtable(L, 0, 3);
    luaL_getmetatable(L, "lieutenant.template");
    lua_setmetatable(L, -2);

    lua_pushlightuserdata(L, (void *) lt_ll_create);
    lua_createtable(L, 0, 4);

    lua_pushvalue(L, 1);
    if (lua_isstring(L, -1)) {
        lua_pushcclosure(L, lt_ll_readstring, 1);
    } else if (!lua_isfunction(L, -1)) {
        lua_pushcclosure(L, lt_ll_readlfile, 1);
    }
    lua_setfield(L, -2, "reader");

    lt_template_t *tpl = (lt_template_t *)
        lua_newuserdata(L, sizeof(lt_template_t));
    lt_template_init(tpl, L, lt_ll_lfuncreader, (void *) tpl);
    lua_setfield(L, -2, "struct");

    lua_settable(L, -3);

    lua_newtable(L);
    lua_setfield(L, -2, "globals");

    lua_newtable(L);
    lua_setfield(L, -2, "helpers");

    return 1;
}

int lt_ll_template_load(lua_State *L)
{
    lua_pushlightuserdata(L, (void *) lt_ll_create);
    lua_gettable(L, 1);

    lua_getfield(L, -1, "struct");
    lt_template_t *tpl = (lt_template_t *) lua_touserdata(L, -1);
    lua_pop(L, 1);

    lt_template_load_stack(L, tpl);
    lua_setfield(L, -2, "parser");

    lua_pushboolean(L, 1);
    lua_setfield(L, -2, "status");

    lua_pushvalue(L, 1);
    return 1;
}

int lt_ll_template_parse(lua_State *L)
{
    lua_pushlightuserdata(L, (void *) lt_ll_create);
    lua_gettable(L, 1);

    lua_getfield(L, -1, "status");
    if (lua_isnil(L, -1)) {
        lua_getfield(L, 1, "load");
        lua_pushvalue(L, 1);
        lua_call(L, 1, 0);
    }

    lua_getfield(L, -1, "parser");
    lua_getfield(L, 1, "globals");
    lua_getfield(L, 1, "helpers");
    lua_call(L, 3, 1);

    return 1;
}

const luaL_Reg lt_ll_template_mt[] = {
    {"load", lt_ll_template_load},
    {"parse", lt_ll_template_parse},
    {NULL, NULL}
};

int luaopen_lieutenant(lua_State *L)
{
    luaL_newmetatable(L, "lieutenant.template");
    luaL_setfuncs(L, lt_ll_template_mt, 0);
    lua_pushcfunction(L, lt_ll_create);
    return 1;
}

static const char *lt_ll_lfuncreader(lua_State *L, void *vp, size_t *size)
{
    lt_template_t *tpl = (lt_template_t *) vp;
    lua_pushvalue(L, tpl->top);

    lua_pushnil(L);
    lua_setfield(L, -2, "last_chunk");

    lua_getfield(L, -1, "reader");
    lua_call(L, 0, 1);

    if (lua_isnil(L, -1)) {
        lua_pop(L, 3);
        *size = 0;
        return NULL;
    }

    const char *data = lua_tolstring(L, -1, size);
    if (*size == 0) {
        lua_pop(L, 3);
        return NULL;
    }

    lua_setfield(L, -2, "last_chunk");
    lua_pop(L, 2);
    return data;
}

static int lt_ll_readstring(lua_State *L)
{
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushnil(L);
    lua_replace(L, lua_upvalueindex(1));
    return 1;
}

static int lt_ll_readlfile(lua_State *L)
{
    lua_getfield(L, lua_upvalueindex(1), "read");
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushstring(L, "a");
    lua_call(L, 2, 1);
    return 1;
}

