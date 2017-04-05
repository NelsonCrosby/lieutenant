#ifndef LT_LIEUTENANT_H
#define LT_LIEUTENANT_H


/*** CORE ***/
typedef struct {
    lua_Reader reader;
    void *reader_data;
    int top;
    lua_State *threadL;
} lt_template_t;

void lt_template_init(lt_template_t *tpl, lua_State *baseL,
        lua_Reader reader, void *data);

const char *lt_load_reader(lua_State *L, void *data, size_t *size);
int lt_load(lua_State *L);
int lt_parse(lua_State *L);


/*** C API ***/
typedef int (*lt_writer)(void *data, const char *chunk, size_t size);

void lt_template_init_lua(lt_template_t *tpl, lua_Reader reader, void *data);

void lt_template_load(lt_template_t *tpl);
void lt_template_load_stack(lua_State *L, lt_template_t *tpl);

void lt_template_parse(lt_template_t *tpl, lt_writer write, void *data);
void lt_template_parse_stack(lua_State *L, lt_template_t *tpl);


#endif // LT_LIEUTENANT_H

