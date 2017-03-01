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
    lua_pushstring(L, "0.0.1");
    return 1;
}

typedef struct {
    char *hmm;
    char *lm;
    char *dict;
} ps_meta;

static int l_init(lua_State *L){
    ps_meta *ctx;
    ctx = (ps_meta *)lua_newuserdata(L, sizeof(ps_meta));
    luaL_getmetatable(L, "vc_meta");
    lua_setmetatable(L, -2);

    if(lua_isnil(L, 1)){
        ctx->hmm = "./model/en-us";
    }else{
        ctx->hmm = malloc(sizeof(luaL_checkstring(L, 1)));
        strcpy(ctx->hmm, luaL_checkstring(L, 1));
    }
    
    if(lua_isnil(L, 2)){
        ctx->lm = "./model/en-us.lm.bin";
    }else{
        ctx->lm = malloc(sizeof(luaL_checkstring(L, 2)));
        strcpy(ctx->lm, luaL_checkstring(L, 2));
    }
    
    if(lua_isnil(L, 3)){
        ctx->dict = "./model/cmudict-en-us.dict";
    }else{
        ctx->dict = malloc(sizeof(luaL_checkstring(L, 3)));
        strcpy(ctx->dict, luaL_checkstring(L, 3));
    }

    return 1;
}

static int l_close(lua_State *L){
    ps_meta *ctx=(ps_meta *)luaL_checkudata(L, 1, "vc_meta");
    luaL_argcheck(L, ctx != NULL, 1,"Context Error");

    free(ctx);
    return 1;
}

static int l_open(lua_State *L){
    ps_meta *ctx=(ps_meta *)luaL_checkudata(L, 1, "vc_meta");
    luaL_argcheck(L, ctx != NULL, 1,"Context Error");

    const char *snd = luaL_checkstring(L, 2);
    ps_decoder_t *ps;
    cmd_ln_t *config;
    FILE *fh;
    char const *hyp;
    int16 buf[512];
    int32 score;

    printf("\ncmd_ln_init\n");

    config = cmd_ln_init(NULL, ps_args(), TRUE,
                 "-hmm", ctx->hmm,
                 "-lm", ctx->lm,
                 "-dict", ctx->dict,
                 NULL);

    printf("\nps_init\n");
    ps = ps_init(config);

    fh = fopen(snd, "rb");
    if (fh == NULL) {
        fprintf(stderr, "Unable to open input file goforward.raw\n");
        return -1;
    }

    printf("\nps_start_utt\n");
    ps_start_utt(ps);
    
    while (!feof(fh)) {
        size_t nsamp;
        nsamp = fread(buf, 2, 512, fh);
        ps_process_raw(ps, buf, nsamp, TRUE, FALSE);
    }
    
    printf("\nps_end_utt\n");
    ps_end_utt(ps);
    printf("\nps_get_hyp\n");
    hyp = ps_get_hyp(ps, &score);
    printf("Recognized: %s\n", hyp);
    lua_pushstring(L, hyp);

    fclose(fh);
    ps_free(ps);
    cmd_ln_free_r(config);
    
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
