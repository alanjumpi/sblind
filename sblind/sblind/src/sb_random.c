/*
 * Author: Carlos Carvalho
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#if defined(__GNUC__) && defined(__i386) && defined(__i686) 
#include <sys/prctl.h>
#endif

/*
 * INTEL x86 only 
 */
#include <errno.h>
static inline volatile unsigned int _time_counter(void)
{
#if defined(__GNUC__) && defined(__i386) && defined(__i686) 
    unsigned int ts = 0;
    // try to force processor time count
    if(prctl(PR_SET_TSC, PR_TSC_ENABLE, 0, 0) < 0)
        return time(NULL);

    __asm__("rdtsc":"=A"(ts));
    return ts;
#endif

    return time(NULL);
}

/*
 * Generate a pseudo random number 
 * based on floor value.
 */
int32_t random_num(int val)
{
    srand(_time_counter());

    if(val<0)
        return -1;
        
    return rand() & val;
}

/*
 * Generate a pseudo random number 
 * based on floor value.
 *
 */
int32_t random_num_range(int minval)
{
    srand(_time_counter());

    if(minval<0)
        return (rand() % 30)+30;
        
    // Force range between:
    // >= minval && <= (minval * 2)
    return (rand() % minval)+minval;
}

/*
 * Alphanumeric random string generator.
 *
 * Givem 10 as size:
 *
 * x8q2k06021e
 * p2a547qn4mu
 * 55og0cu8tnq
 * 24n0r58m9s4
 * hg3s97r56et
 * ...
 * and so on.
 */
void random_str(char *str, unsigned len)
{
    if(!str || len <= 0)
        return;

    srand(_time_counter());


    register int i;
    for (i = len; i >= 0; --i)
    {
        if(rand() % 2)
            // a- z
            str[i] = (rand() % 25) + 97;
        else
            // 0 - 9
            str[i] = (rand() % 10) + 48;
    }

    str[i] = '\0';
}
