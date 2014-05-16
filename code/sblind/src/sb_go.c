#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "sb.h"

#define HANDLE_INIT 1
#define HANDLE_CLEANUP 0
#define LOOP_LIMIT 32
#define __30MIN_RANGE__ 1800

static pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;

static void *_go_shot_each_url(void *t)
{
    pthread_detach(pthread_self());
    pthread_mutex_lock(&mutex_a);

    CURL *handle;
    char *_url = NULL;
    int len;
    userdata_t *data = (userdata_t*)t;

    if(!data)
    {
        DEBUG("Error:%d\n", __LINE__);
        pthread_mutex_unlock(&mutex_a);
        pthread_exit(NULL);
    }

    len = strlen(data->url);
    _url = calloc(1, len+2);
    strncpy(_url, data->url, len);
    strncat(_url, "'", 1);

    handle = curl_easy_init();

#ifdef __DODEBUG__
    curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(handle, CURLOPT_HEADER, 1L);
    //curl_easy_setopt(handle, CURLOPT_PROXYUSERPWD, PAUTH);

    curl_easy_setopt(handle, CURLOPT_PROXYTYPE, SOCKSTYPE); 
    curl_easy_setopt(handle, CURLOPT_PROXY, SOCKSURL);

    curl_easy_setopt(handle, CURLOPT_WRITEDATA, data);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, sbIsUrlVuln);

    curl_easy_setopt(handle, CURLOPT_URL, _url);

    int ugent = random_num(65535)%9;
    curl_easy_setopt(handle, CURLOPT_USERAGENT, UserAgentList[ugent]);

    curl_easy_perform(handle);

    curl_easy_cleanup(handle);

    SAFE_FREE(_url);
    SAFE_FREE(data->url);
    SAFE_FREE(data);

    pthread_mutex_unlock(&mutex_a);
    pthread_exit(NULL);
}

/*
 * Test each given URL 
 */
int go_shot_each_url(char *url, size_t len)
{
#ifdef __DISABLE_VULN_TEST__
    return 0;
#endif

    pthread_t thread;
    userdata_t *data = NULL;
    int ret;

    if(!url || len <= 0)
        return -1;


    data = calloc(1, sizeof(userdata_t));
    data->url = calloc(1, len+1);

    strncpy(data->url, url, len);

    ret = pthread_create(&thread, NULL, _go_shot_each_url, (void*)data);
    if(ret != 0)
    {
        DEBUG("Error: pthread_create = %d\n", ret);
        SAFE_FREE(data->url);
        SAFE_FREE(data);
        return -1;
    }

    return 0;
}

/*
 * This is our main thread.
 */
static void *_go_shot(void *t)
{
    CURL *handle;
    userdata_t *args = (userdata_t*)t;
    if(!args)
    {
        DEBUG("Error: Invalid data\n");
        pthread_exit(NULL);
    }

    int w8 = 10, slp = 0;
    engineId_e engine = args->engine;
    char *url = NULL;
    int sum = args->pagesum ? args->pagesum : engineHelper[engine].page;


    userdata_t *data = calloc(1, sizeof(userdata_t));
    data->wcard = engineHelper[engine].wcard;
    data->engine = engine;
    data->pagesum = sum;
    *(&data->retcode) = 0;
    *(&data->tryurl) = go_shot_each_url;
    
    handle = curl_easy_init();

#ifdef __DODEBUG__
    curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(handle, CURLOPT_HEADER, 0L);

#define PAUTH "user:pass"
    /*
     * Uncomment next line and set above info if
     * you are behind a proxy authentication
     *
     */
    //curl_easy_setopt(handle, CURLOPT_PROXYUSERPWD, PAUTH);

    curl_easy_setopt(handle, CURLOPT_PROXYTYPE, SOCKSTYPE); 
    curl_easy_setopt(handle, CURLOPT_PROXY, SOCKSURL);

    curl_easy_setopt(handle, CURLOPT_WRITEDATA, data);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, sbHtpParser);

    //data->logfp = sb_fopen(engineHelper[data->engine].name);
    //if(data->logfp == NULL)
    //    goto die;

    while(1)
    {
        static int lmt = 0;
        url = e_fptr[engine](engineHelper[engine].wcard, sum);
        curl_easy_setopt(handle, CURLOPT_URL, url);

        DEBUG("Next URL: %s\n", url);

        int ugent = random_num(65535)%9;
        curl_easy_setopt(handle, CURLOPT_USERAGENT, UserAgentList[ugent]);
        DEBUG("User Agent #%d: %s\n", ugent, UserAgentList[ugent]);

        pthread_mutex_lock(&mutex_b);
        data->page = sum;
        curl_easy_perform(handle);
        pthread_mutex_unlock(&mutex_b);
        
        switch((unsigned long)data->retcode)
        {
            case 1:
                DEBUG("[%s] Works!\n", engineHelper[data->engine].name);
                *(&data->retcode) = 0;
                sum += engineHelper[data->engine].range;
                lmt = 1;
                w8 = 10;
                slp = random_num_range(w8);
                break;
            default:
                ++lmt;
                w8 <= __30MIN_RANGE__ ?  w8 += 10 : 0;
                slp = random_num_range(w8);
                break;
        }

        if(w8 >= __30MIN_RANGE__ && engineHelper[data->engine].engine != e_GOOGLE)
            goto die;

        DEBUG("Sleep=%ds, w8=%d\n", slp, w8);
        sleep(slp);
    }

die:
    curl_easy_cleanup(handle);
    SAFE_FREE(data);
    pthread_exit(NULL);
}

int go_shot(engineId_e engine, int pagen)
{
    if(engine == e_ALL)
    {
        pthread_t thread[ENGINES_NUM];
        userdata_t *data[ENGINES_NUM]; 
        int i;

        for(i = 1; i <= ENGINES_NUM; ++i)
        {
            if(!e_fptr[i])
                continue;

            int pos = i - 1;
            data[pos] = calloc(1, sizeof(userdata_t));
            data[pos]->engine = engineHelper[i].engine;
            data[pos]->wcard = engineHelper[i].wcard;
            data[pos]->pagesum = pagen;

            if( pthread_create(&thread[pos], NULL, _go_shot, data[pos]) != 0)
            {
                DEBUG("Error: pthread error\n");
                SAFE_FREE(data[pos]);
                continue;
            }

            PRINTF("-= %s running =-\n", engineHelper[i].name);
        }
    
        
        for(i = 1; i <= ENGINES_NUM; ++i, pthread_join(thread[i-1], NULL)){}
        for(i = 1; i <= ENGINES_NUM; ++i)
            SAFE_FREE(data[i-1]);

    } else if(e_fptr[engine])
    {
        pthread_t thread;
        userdata_t *data = calloc(1, sizeof(userdata_t)); 
        data->engine = engineHelper[engine].engine;
        data->wcard = engineHelper[engine].wcard;
        data->pagesum = pagen;

        if(pthread_create(&thread, NULL, _go_shot, data) != 0)
        {
            DEBUG("pthread error\n");
            SAFE_FREE(data);
            return -1;
        }
            
        PRINTF("-= %s running =-\n", engineHelper[engine].name);
        pthread_join(thread, NULL);
    }

    return 0;
}

