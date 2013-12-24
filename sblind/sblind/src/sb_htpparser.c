/*
 * Author: Carlos Carvalho
 *
 */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>

#include "sb_htpparser.h"
#include "sb.h"

/*
 *  sb_htpparser.c
 *
 *  @ WARNING WARNING WARNING WARNING
 *  @ !!! TOO MANY WORKAROUNDS !!!
 *  @ !!! TOO MANY WORKAROUNDS !!!
 *  @ !!! TOO MANY WORKAROUNDS !!!
 *  @ !!! TOO MANY WORKAROUNDS !!!
 *  @ WARNING WARNING WARNING WARNING
 *
 *  I've found that parsing HTTP through C isn't the most
 * simple thing on Earth.
 *
 * libxml didn't prove to be reliable enough, I'd have to parse
 * myself a lot of garbage too, so I moved forward.
 *  
 *  Whatever, you, kind reader, will find some ugly and dirty hacks
 *  and workarounds in this code and you'll blame me about 
 *  that but I don't care, really.
 *
 *  There are some bugs and once in a while a weird URL is displayed
 *  but it is not really a big deal since we can parse most of the links successfully,
 *  providing us a fair success rate.
 *
 */

/*
 * Loop through invalid tokens. 
 */
static int _is_valid_url(char *buffer, size_t len)
{
    int i;
    char *buf = buffer;
    if(!buf)
        return 0;

    for(i = 0; invalidStrings[i] != NULL; ++i)
        if(strcasestr(buf, invalidStrings[i]))
            return 0;

    return 1;
}

/*
 *  Strip invalid '\' characters, blame on Sapo engine. 
 */
static char *_strip_char(char *str, char oldChar, int len, unsigned char c) 
{
    if(len >= 256)
        return NULL;

    static char buf[256];
    memset(buf, 0, 256);

    int i = 0, x = 0;
    char *srcptr = str;


    for(; i < len; ++i)
    {
        if(srcptr[i] != '\\')
            buf[x++] = srcptr[i];
    }

    if(c)
    {
        buf[x] = c;
        buf[x+1] = '\0';
    } else
        buf[x] = '\0';

    return buf;
}
 
/*
 * URL decoder.
 */
static char *_decode_url(char *s, int *total)
{
    int c, slen = strlen(s);
    char *dec = calloc(1, slen + 1);
	char *o, *end = s + slen;
 
	for (o = dec; s <= end; o++) 
    {
		c = *s++;
		if (c == '+') 
            c = ' ';
		else if (c == '%' && (	!_is_hex(*s++)	||
					!_is_hex(*s++)	||
					!sscanf(s - 2, "%2x", &c)))
			return NULL;
 
		if (dec) 
            *o = c;
	}
 
    total ? *total = o - dec : 0;
	return dec;
}

/*
 * Locate specific char on given string.
 */
static int _find_char(char *buf, unsigned char c, int max)
{
    int ret = 0;
    int lmt = max;
    for(; ret < lmt; ++ret)
    {
        if(buf[ret] == c)
            break;
    }
    return ret;
}

/*
 * Virify if URL is vulnerable
 *
 */
size_t sbIsUrlVuln(void *buffer, size_t size, size_t nmemb, void *userp)
{
#ifdef __RAW_OUTPUT__ 
#warning "Raw output mode"
    userdata_t *data = (userdata_t*)userp;
    printf("INN %s\n",data->url); 
    char *buf = calloc(1, size*nmemb);
    memcpy(buf, buffer, size*nmemb);
    printf("%s\n", buf);
    free(buf);

    printf("### %d %d\n", size, nmemb);
    return size*nmemb;
#else
    
    char *url = (char *)userp;
    char *buf = (char*)buffer;
    userdata_t *data;
    static int32_t hash = -1;

    if(!url || !buf)
    {
        DEBUG("Error: null buffer\n"); 
        return 0;
    }

    data = (userdata_t*)userp;
    if(!data)
    {
        DEBUG("Error: null data\n"); 
        return 0;
    }

    if(hash == -1)
    {
        int i;
        hash = hash_get_hash(data->url);
        for(i =0; matchStrings[i] != NULL; ++i)
        {
            if(strcasestr(buf, matchStrings[i]))
            {
                PRINTF_URL("\n@@@@@@@@@@@@@@ VulnURL => %s\n\n", data->url);
                return size*nmemb;
            }
        }
    }

    int32_t hashtmp = hash_get_hash(data->url);

    if(hashtmp != hash)
    {
        int i;
        for(i =0; matchStrings[i] != NULL; ++i)
        {
            if(strcasestr(buf, matchStrings[i]))
            {
                hash = hashtmp;
                PRINTF_URL("\n@@@@@@@@@@@@@@ VulnURL => %s\n\n", data->url);
                return size*nmemb;
            }
        }
    }

    return size*nmemb;
#endif
}

/*
 * Main HTTP parser.
 */
size_t sbHtpParser(void *buffer, size_t size, size_t nmemb, void *userp)
{
#ifdef __RAW_OUTPUT__ 
#warning "Raw output mode"
   
    userdata_t *data = (userdata_t*)userp;
    char *buf = calloc(1, size*nmemb);
    memcpy(buf, buffer, size*nmemb);
    printf("%s\n", buf);
    free(buf);

    return size*nmemb;
#else
    if(!buffer)
    {
        DEBUG("Error: Invalid buffer\n");
        return 0;
    }
    
    static int counter = 0;
    char *buf = (char*)buffer;
    int i = 0;
    int len = nmemb;
    int httpentry = -1;

    userdata_t *data = (userdata_t*)userp;

    for(; i < len; ++i)
    {
        /*
         *  Ok, first explanation about something that should not
         * be explained at all.
         *  Sometimes http tag comes in different formats and also
         *  can be https as well.
         *
         *  Decode it beforehand is possible but I'd have to keep the original
         *  length so I could move the pointer forward correctly so simple using
         *  if/else scheme is easier, though uglier.
         *  
         */
        if(!strncmp(&buf[i], "http:", HTTP_LEN))
        {
            httpentry = i;
            i += HTTP_LEN;
        } else if(!strncmp(&buf[i], "https:", HTTPS_LEN))
        {
            httpentry = i;
            i += HTTPS_LEN;
        } else if(!strncmp(&buf[i], "http%3a", HTTP_FUCK_LEN))
        {
            httpentry = i;
            i += HTTP_FUCK_LEN;
        } else if(!strncmp(&buf[i], "https%3a", HTTPS_FUCK_LEN))
        {
            httpentry = i;
            i += HTTPS_FUCK_LEN;

        } else {
            continue;
        }

        for(; i < len; ++i)
        {
            /*
             * Right, that's shit, what can I do? We can't accept two
             * "http*" occurences in the same string if second "http*" 
             * comes before ".php?* or /?". 
             *
             * We must skip.
             */
            if(!strncmp(&buf[i], "http:", HTTP_LEN))
            {
                i += HTTP_LEN;
                break;
            } else if(!strncmp(&buf[i], "https:", HTTPS_LEN))
            {
                i += HTTPS_LEN;
                break;
            } else if(!strncmp(&buf[i], "http%3a", HTTP_FUCK_LEN))
            {
                i += HTTP_FUCK_LEN;
                break;

            } else if(!strncmp(&buf[i], "https%3a", HTTPS_FUCK_LEN))
            {
                i += HTTPS_FUCK_LEN;
                break;
            }

            /*
             * For each group of possible matching strings the rules are different.
             *
             * Some engines use xhtp (Google..) format and some don't..
             */
            if((!strncasecmp(&buf[i], KEYHEX, KEYHEXSIZ) || !strncasecmp(&buf[i], KEYSHORT, 2) ) && httpentry != -1)
            {
                int urlsize = (i - httpentry)+KEYHEXSIZ; 

                char *_url = _strip_char(&buf[httpentry],'\\', urlsize, '='); 
                if(!_url)
                {
                    i += KEYHEXSIZ+2;
                    break;
                }

                // Extra space to acomodate = '=' + '\0'
                char *url = calloc(1, urlsize+2);
                memcpy(url, _url, urlsize);
                url[urlsize-1] != '=' ? url[urlsize] = '=' : 0;

                if(!_is_valid_url(url, urlsize+1))
                {
                    SAFE_FREE(url);             
                    i += KEYHEXSIZ+2;
                    break;
                }


                if(!hash_add_item(url))
                {
                    SAFE_FREE(url);             
                    i += KEYHEXSIZ;
                    break;
                }

                ++counter;
                PRINTF("[page:%d][%d]%s$ %s\n", data->page, counter, engineHelper[data->engine].name, url);

                // Set success case
                if(data)
                {
                    if(data->retcode == 0)
                        data->retcode = (int*)1;

                    //Dispatch vulnerability checking thread
                    data->tryurl(url, strlen(url));
                }

                SAFE_FREE(url);             
                i += KEYHEXSIZ+2;
                break;

            } else if((!strncasecmp(&buf[i], KEYREG, KEYREGSIZ) || 
                        !strncasecmp(&buf[i], KEYUNI, KEYUNISIZ) || !strncasecmp(&buf[i], KEYSHORT, 2)) && 
                    httpentry != -1 /* && target->id != e_GOOGLE*/) //check this later
            {
                int t = _find_char(&buf[i], '=', 20);

                //TODO: FIXME
                //Huge workaround!
                if(t == 20)
                {
                    t = _find_char(&buf[i], '=', 256);
                    ++t;
                }

                int urlsize = (i - httpentry)+t+1;

                char *_url = _strip_char(&buf[httpentry],'\\', urlsize, 0); 
                if(!_url)
                {
                    i += KEYREGSIZ+t;
                    break;
                }

                char *url = _decode_url(_url, NULL);

                if(url)
                {
                    if(!_is_valid_url(url, urlsize))
                    {
                        SAFE_FREE(url);
                        i += KEYREGSIZ+t;
                        break;
                    }

                    if(!hash_add_item(url))
                    {
                        SAFE_FREE(url);
                        i += KEYREGSIZ;
                        break;
                    }

                    ++counter;
                    PRINTF("[page:%d][%d]%s$ %s\n", data->page, counter, engineHelper[data->engine].name, url);

                    // Set success case
                    if(data)
                    {
                        if(data->retcode == 0)
                            data->retcode = (int*)1;

                        //Dispatch vulnerability checking thread
                        data->tryurl(url, strlen(url));
                    }

                    SAFE_FREE(url);
                    i += KEYREGSIZ+t;
                    break;
                }
                    
                SAFE_FREE(url);
                i += KEYREGSIZ;
            } 
        }
    }

    return size * nmemb;
#endif
}
