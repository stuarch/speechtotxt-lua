#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <pocketsphinx.h>

static int l_version(lua_State *L){
    lua_pushstring(L, "0.0.2");
    return 1;
}

typedef struct {
    char *hmm;
    char *lm;
    char *dict;
    cmd_ln_t *config;
    ps_decoder_t *ps;
} ps_meta;

static int l_init(lua_State *L){
    ps_meta *ctx;
    ctx = (ps_meta *)lua_newuserdata(L, sizeof(ps_meta));
    luaL_getmetatable(L, "vc_meta");
    lua_setmetatable(L, -2);
    char *hmm, *lm, *dict;

    if(lua_isnil(L, 1)){
        hmm = "/usr/share/pocketsphinx/model/en-us/en-us";
    }else{
        hmm = malloc(sizeof(luaL_checkstring(L, 1)));
        strcpy(hmm, luaL_checkstring(L, 1));
    }
    
    if(lua_isnil(L, 2)){
        lm = "/usr/share/pocketsphinx/model/en-us/en-us.lm.bin";
    }else{
        lm = malloc(sizeof(luaL_checkstring(L, 2)));
        strcpy(ctx->lm, luaL_checkstring(L, 2));
    }
    
    if(lua_isnil(L, 3)){
        dict = "/usr/share/pocketsphinx/model/en-us/cmudict-en-us.dict";
    }else{
        dict = malloc(sizeof(luaL_checkstring(L, 3)));
        strcpy(ctx->dict, luaL_checkstring(L, 3));
    }

    ctx->config = cmd_ln_init(NULL, ps_args(), TRUE,
            "-hmm", hmm,
            "-lm", lm,
            "-dict", dict,
            NULL);

    ctx->ps = ps_init(ctx->config);
    if(ctx->ps!=NULL){
        printf("Init Successed!\n");
    }
    return 1;
}

static int l_close(lua_State *L){
    ps_meta *ctx=(ps_meta *)luaL_checkudata(L, 1, "vc_meta");
    luaL_argcheck(L, ctx != NULL, 1,"Context Error");

    ps_free(ctx->ps);
    cmd_ln_free_r(ctx->config);
    return 1;
}

static int l_open(lua_State *L){
    ps_meta *ctx=(ps_meta *)luaL_checkudata(L, 1, "vc_meta");
    luaL_argcheck(L, ctx != NULL, 1,"Context Error");

    const char *snd = luaL_checkstring(L, 2);
    FILE *fh;
    char const *hyp;
    int16 buf[512];
    int32 score;

    fh = fopen(snd, "rb");
    if (fh == NULL) {
        fprintf(stderr, "Unable to open input file goforward.raw\n");
        return -1;
    }

    ps_start_utt(ctx->ps);
    
    while (!feof(fh)) {
        size_t nsamp;
        nsamp = fread(buf, 2, 512, fh);
        ps_process_raw(ctx->ps, buf, nsamp, TRUE, FALSE);
    }
    
    ps_end_utt(ctx->ps);
    hyp = ps_get_hyp(ctx->ps, &score);
    lua_pushstring(L, hyp);

    fclose(fh);
    
    return 1;
}

static const struct luaL_Reg voice_obj[] = {
    {"open", l_open},
    {"close", l_close},
    {NULL, NULL},
};

static const struct luaL_Reg voice_lib[] = {
    {"version", l_version},
    {"init", l_init},
    {NULL, NULL},
};


int luaopen_speechtotxt(lua_State *L){
    luaL_newmetatable(L, "vc_meta");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, voice_obj);
    luaL_register(L, "vc_meta",  voice_lib);
    return 1;
}
