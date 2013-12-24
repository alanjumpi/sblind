/*
 * Author: Carlos Carvalho
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

#include "sb.h"


static void availableSearchEngines(void)
{
    int i = 1;
    printf("[-- Available search engines --]\n");
    for(; engineHelper[i].engine != e_UNDEFINED; ++i)
        e_fptr[engineHelper[i].engine] ? printf(" [%d] %s\n", i, engineHelper[i].name) : 0;
}

static int checkHTTPProtocol(void)
{
    curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);

    while(*(data->protocols) != NULL)
    {
        if(!strncmp(*(data->protocols), "http", 4))
            return 0;

        ++(data->protocols);
    }
    return -1;
}

static void init_fptr(void)
{
    e_fptr[e_GOOGLE] = sbEnginesGoogleFmt;
    e_fptr[e_BING] = sbEnginesBingFmt;
    e_fptr[e_YAHOO] = sbEnginesYahooFmt;
    e_fptr[e_GIGABLAST] = sbEnginesGigaBlastFmt;
    e_fptr[e_SAPO] = sbEnginesSapoFmt;
    //e_fptr[e_EXCITE] = sbEnginesExciteFmt;
}


static void h(const char *n, int status)
{
    printf("sBlind v0.1 crawler by xxx\n");
    printf("Usage: %s [-e engine# page#] [-l] [-h]\n", n);
    printf("-e: Engine to use:\n");
    printf("\t0 => All of available engines (recomended) and page# will be considered as starting page\n");
    printf("\t[1 .. N] testing purposes only:\n");
    printf("\t1 => Google\n");
    printf("\t2 => Bing (Google reaper)\n");
    printf("\t3 => Yahoo\n");
    printf("\t4 => GigaBlast (poor)\n");
    printf("\t5 => Sapo\n");
    printf("\t6 => Excite\n");
    printf("-l: List available search engines. Aka: Implemented ones\n");
    printf("-h: Show (sic) this help\n");
    exit(status);
}

extern void showfile(void);
int main(int argc, char **argv)
{
    curl_global_init(CURL_GLOBAL_ALL);

    init_fptr();

    if(checkHTTPProtocol() < 0)
    {
        printf("Oops! HTTP protocol is NOT available dear user.\
                Check your libcurl compilation.\n");
        return -1;
    }

    int c, optind = 0;

    static struct option long_options[] = {
        {"engine", required_argument, 0, 'e'},
        {"list", no_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {NULL, 0, 0, 0},
    };
    
    while(1)
    {
        c = getopt_long(argc, argv, "e:lh", long_options, &optind);
        if(c == -1)
            break;

        switch(c)
        {
            case 'e':
                !argv[optind+2] ? h(argv[0], EXIT_FAILURE) : 0;
                !argv[optind+3] ? h(argv[0], EXIT_FAILURE) : 0;
                go_shot(atoi(argv[optind+2]), atoi(argv[optind+3]));
                break;
            case 'l':
                availableSearchEngines();
                exit(EXIT_SUCCESS);
            case 'h':
                h(argv[0], EXIT_SUCCESS);
            case '?':
                break;
            default:
                break;
        }
    }

    return 0;
}
