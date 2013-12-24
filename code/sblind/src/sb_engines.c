/*
 * Author: Carlos Carvalho
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sb.h"

#define GOOGLELEN_MIN_LEN /* fixed str*/ 261 /* wildcard(2) each 16 */ + 32 /* char int max 90000*/ + 5 /* '\0' */ + 1
#define BINGLEN_MIN_LEN /* fixed str*/ 97 /* wildcard(2) each 16 */ + 32 /* char int max 90000*/ + 5 /* '\0' */ + 1
#define YAHOOLEN_MIN_LEN /* fixed str*/ 116 /* wildcard */ + 16 /* char int max 90000*/ + 5 /* '\0' */ + 1
#define GIGABLAST_MIN_LEN /* fixed str*/ 55 /* wildcard */ + 16 /* char int max 90000*/ + 5 /* '\0' */ + 1
#define SAPO_MIN_LEN /* fixed str*/ 125 /* wildcard */ + 16 /* char int max 90000*/ + 5 /* '\0' */ + 1
#define EXCITE_MIN_LEN /* fixed str*/ 86 /* wildcard */ + 8 /* char int max 90000*/ + 5 /* '\0' */ + 1

char *sbEnginesGoogleFmt(const char *wildcard, int32_t indice)
{
    static __thread char buf[GOOGLELEN_MIN_LEN];
    memset(buf, 0 , GOOGLELEN_MIN_LEN);
    char number[6] = {[0 ... 5] = '\0'};
    
    if(strlen(wildcard) > 16 || (indice < 0 || indice > 90000))
        return NULL;
        

    static char *tok[] = {
        "http://www.google.com.br/search?sclient=psy-ab&hl=en&site=&source=hp&q=allinurl:", // wildcard
        "&btnK=Pesquisa+Google%23q=allinurl:", // wildcard
        "&hl=en&prmd=imvns&ei=GdGdUJaVFZSs8QSQmYHACw&start=", // page
        "&sa=N&fp=1&bpcl=38093640&biw=1280&bih=711&bav=on.2,or.r_gc.r_pw.r_qf.&cad=b&sei=_9GdUNGGG5Og8QS8rYDgBw",
        NULL
    };

    snprintf(number, 5, "%d", indice);

    strncpy(buf, tok[0], strlen(tok[0]));
    strncat(buf, wildcard, strlen(wildcard));
    strncat(buf, tok[1], strlen(tok[1]));
    strncat(buf, wildcard, strlen(wildcard));
    strncat(buf, tok[2], strlen(tok[2]));
    strncat(buf, number, strlen(number));
    strncat(buf, tok[3], strlen(tok[3]));
    
    return buf;
}

char *sbEnginesBingFmt(const char *wildcard, int32_t indice)
{
    static __thread char buf[BINGLEN_MIN_LEN];
    memset(buf, 0 , BINGLEN_MIN_LEN);
    char number[6] = {[0 ... 5] = '\0'};

    if(strlen(wildcard) > 16 || (indice < 0 || indice > 90000))
        return NULL;

    static char *tok[] = {
        "http://br.bing.com/search?q=allinurl:", // wildcard
        "&go=&qs=n&filt=all&pq=allinurl:", // wildcard
        "&sc=1-13&sp=-1&sk=&first=", // page
        "&FORM=PORE",
        NULL
    };

    snprintf(number, 5, "%d", indice);
    
    strncpy(buf, tok[0], strlen(tok[0]));
    strncat(buf, wildcard, strlen(wildcard));
    strncat(buf, tok[1], strlen(tok[1]));
    strncat(buf, wildcard, strlen(wildcard));
    strncat(buf, tok[2], strlen(tok[2]));
    strncat(buf, number, strlen(number));
    strncat(buf, tok[3], strlen(tok[3]));

    return buf;
}

char *sbEnginesYahooFmt(const char *wildcard, int32_t indice)
{
    static __thread char buf[YAHOOLEN_MIN_LEN];
    memset(buf, 0 , YAHOOLEN_MIN_LEN);
    char number[6] = {[0 ... 5] = '\0'};

    if(strlen(wildcard) > 16 || (indice < 0 || indice > 90000))
        return NULL;

    static char *tok[] = {
        "http://br.search.yahoo.com/search;_ylt=a0ogkmeenavqbayat1nz6qt.?p=allinurl:", // wildcard
        "&fr=yfp-t-707&fr2=sb-top&xargs=0&pstart=1&b=", // page
        NULL
    };

    snprintf(number, 5, "%d", indice);
    
    strncpy(buf, tok[0], strlen(tok[0]));
    strncat(buf, wildcard, strlen(wildcard));
    strncat(buf, tok[1], strlen(tok[1]));
    strncat(buf, number, strlen(number));
    
    return buf;
}

char *sbEnginesGigaBlastFmt(const char *wildcard, int32_t indice)
{
    static __thread char buf[GIGABLAST_MIN_LEN];
    memset(buf, 0 , GIGABLAST_MIN_LEN);
    char number[6] = {[0 ... 5] = '\0'};

    if(strlen(wildcard) > 16 || (indice < 0 || indice > 90000))
        return NULL;

    static char *tok[] = {
        "http://gigablast.com/search?k0e=718815&s=", // page
        "&q=allinurl:", // wildcard
        NULL
    };

    snprintf(number, 5, "%d", indice);
    
    strncpy(buf, tok[0], strlen(tok[0]));
    strncat(buf, number, strlen(number));
    strncat(buf, tok[1], strlen(tok[1]));
    strncat(buf, wildcard, strlen(wildcard));
    
    return buf;
}

char *sbEnginesSapoFmt(const char *wildcard, int32_t indice)
{
    static __thread char buf[SAPO_MIN_LEN];
    memset(buf, 0 , SAPO_MIN_LEN);
    char number[6] = {[0 ... 5] = '\0'};

    if(strlen(wildcard) > 16 || (indice < 0 || indice > 90000))
        return NULL;

    static char *tok[] = {
        "http://pesquisa.sapo.pt/?adultfilter=strict&barra=resumo&cluster=0&format=html&limit=10&location=pt&page=" ,//1
        "&q=inurl%3A",// %2Ephp%3F
        "&st=local",
        NULL
    };

    snprintf(number, 5, "%d", indice);
    
    strncpy(buf, tok[0], strlen(tok[0]));
    strncat(buf, number, strlen(number));
    strncat(buf, tok[1], strlen(tok[1]));
    strncat(buf, wildcard, strlen(wildcard));
    strncat(buf, tok[2], strlen(tok[2]));
    
    return buf;
}

char *sbEnginesExciteFmt(const char *wildcard, int32_t indice)
{
    static __thread char buf[EXCITE_MIN_LEN];
    memset(buf, 0 , EXCITE_MIN_LEN);
    char number[6] = {[0 ... 5] = '\0'};

    if(strlen(wildcard) > 16 || (indice < 0 || indice > 90000))
        return NULL;

    static char *tok[] = {
        "http://msxml.excite.com/search/web?qsi=" ,//1
        "&q=",// %2Ephp%3F
        "&fcoid=4&fcop=results-bottom&fpid=2",
        NULL
    };
    
    snprintf(number, 5, "%d", indice);
    
    strncpy(buf, tok[0], strlen(tok[0]));
    strncat(buf, number, strlen(number));
    strncat(buf, tok[1], strlen(tok[1]));
    strncat(buf, wildcard, strlen(wildcard));
    strncat(buf, tok[2], strlen(tok[2]));
    
    return buf;
}
