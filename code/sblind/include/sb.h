/*
 * Author: Carlos Carvalho
 *
 */

#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define __UNUSED__ __attribute__((unused))

#define SAFE_FREE(x) {\
    if(x != NULL) {\
        free(x);\
        x = NULL;\
    }\
}

#ifdef __DODEBUG__
#include <stdio.h>
#include <errno.h>
#include <string.h>
#define DEBUG(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__);
#else
#define DEBUG(fmt, ...)
#endif

#define PRINTF_URL(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__);
#define PRINTF_ERROR(fmt ...) PRINTF_URL(fmt)
#define PRINTF(fmt ...) PRINTF_URL(fmt)

#define KEYHEX ".php?id\\x"
#define KEYREG ".php?" 
#define KEYSHORT "/?" 
#define KEYUNI ".php%3f" 

#define HTTP_LEN strlen("http:")
#define HTTPS_LEN strlen("https:")

#define HTTP_FUCK_LEN strlen("http%3a")
#define HTTPS_FUCK_LEN strlen("https%3a")

#define KEYHEXSIZ strlen(KEYHEX)-2 // -2? ugly isn't it?
#define KEYREGSIZ strlen(KEYREG)
#define KEYUNISIZ strlen(KEYUNI)



#define SOCKSTYPE CURLPROXY_SOCKS5
#define SOCKSURL "socks5://127.0.0.1:9050"
/*
 * Info about search engines
 *
 */
typedef enum
{
    e_ALL = 0,
    e_GOOGLE,
    e_BING,
    e_YAHOO,
    e_GIGABLAST,
    e_SAPO,
    e_EXCITE,
    e_GO,
    e_ALLTHEWEB,
    e_NAJDI,
    e_NAVER,
    e_RAMBLER,
    e_REDIFF,
    e_WALLA,
    e_UNDEFINED,
}engineId_e;
typedef struct __engine_helper_t
{
    char *name;
    engineId_e engine;
    char *wcard;
    int page;
    int range;
}__engine_helper_t;

#define ENGINES_NUM 5
static __engine_helper_t 
__UNUSED__ engineHelper[] = {
    {"ALL", e_ALL, NULL, 0},
    {"Google", e_GOOGLE, ".php%3Fid%3D", 0, 5},
    {"Bing", e_BING, ".php%3Fid%3D", 0, 5},
    {"Yahoo", e_YAHOO, ".php%3Fid%3D", 1, 5},
    {"GigaBlast", e_GIGABLAST, ".php%3Fid=", 0, 5},
    {"Sapo", e_SAPO, ".php?id=", 1, 1},
    {"Excite", e_EXCITE, ".php?id=", 0, 5},
    {"Go", e_GO, NULL, 0},
    {"AllTheweb", e_ALLTHEWEB, NULL, 0, 0},
    {"Najdi", e_NAJDI, NULL, 0, 0},
    {"Naver", e_NAVER, NULL, 0, 0},
    {"Rambler", e_RAMBLER, NULL, 0, 0},
    {"Rediff", e_REDIFF, NULL, 0, 0},
    {"Walla", e_WALLA, NULL, 0, 0},
    {NULL, e_UNDEFINED, NULL, -1, -1},
};

static char __UNUSED__*UserAgentList[] = {
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.3) Gecko/2008092417 Firefox/3.0.3",
    "Mozilla/5.0 (compatible; MSIE 10.0; Macintosh; Intel Mac OS X 10_7_3; Trident/6.0)",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_2) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1309.0 Safari/537.17",
    "Opera/9.80 (Windows NT 6.1; U; es-ES) Presto/2.9.181 Version/12.00",
    "Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A5355d Safari/8536.25",
    "Galaxy/1.0 [en] (Mac OS X 10.5.6; U; en)",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_2) AppleWebKit/537.13 (KHTML, like Gecko) Chrome/24.0.1290.1 Safari/537.13",
    "Mozilla/5.0 (Windows; U; Windows NT 6.1; x64; fr; rv:1.9.2.13) Gecko/20101203 Firebird/3.6.13",
    "Mozilla/5.0 (Linux; U; Android 4.0.3; ko-kr; LG-L160L Build/IML74K) AppleWebkit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30",
    NULL
};

typedef struct __thread_userdata_t
{
    char *wcard;
    char *url;
    FILE *logfp;
    int (*tryurl)(char *, size_t);
    engineId_e engine;
    int pagesum;
    int page;
    int *retcode;
}userdata_t;

/*
 * sb_engines.c
 */ 
char *sbEnginesGoogleFmt(const char *wildcard, int32_t indice);
char *sbEnginesBingFmt(const char *wildcard, int32_t indice);
char *sbEnginesYahooFmt(const char *wildcard, int32_t indice);
char *sbEnginesGigaBlastFmt(const char *wildcard, int32_t indice);
char *sbEnginesSapoFmt(const char *wildcard, int32_t indice);
char *sbEnginesExciteFmt(const char *wildcard, int32_t indice);

char *(*e_fptr[ENGINES_NUM])(const char*, int32_t);

/*
 * sb_str.c
 */
size_t sbHtpParser(void *buffer, size_t size, size_t nmemb, void *userp);
size_t sbIsUrlVuln(void *buffer, size_t size, size_t nmemb, void *userp);

/*
 * sb_hash.c
 */

#define HASHTSIZ 65535*2
inline int32_t hash_raw(const char *url);
int32_t hash_get_hash(const char *url);
char *hash_add_item(char *url);
int32_t hash_del_item(char *url);
char **hash_get_table(void);

/*
 * sb_random.c
 */
int32_t random_num(int minval);
int32_t random_num_range(int minval);
void random_str(char *str, unsigned len);

/*
 * sb_go.c
 */
int go_shot(engineId_e engine, int pagen);

/*
 * sb_file.c
 */
FILE *sb_fopen(const char *ename);
size_t sb_fwrite(const char *ptr, size_t size, FILE *fp);
int sb_fclose(FILE *fp);
