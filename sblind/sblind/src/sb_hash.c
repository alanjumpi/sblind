/*
 * Author: Carlos Carvalho
 *
 */

#include "sb.h"


/*
 * Hash table array
 */
static char *htable[HASHTSIZ];

/*
 * Bernstein's hash algorithm.
 *
 * Slightly modified by me so
 * result will be posive sign.
 */
inline int32_t hash_raw(const char *url)
{
    register int32_t h = 0;

    if(!url)
        return -1;

	while(*url) 
        h=33*h + *url++;

    // Make it positive sign
    (h >> 31) & 0x1 ? h = ~h : 0;

	return h & HASHTSIZ;
}

int32_t hash_get_hash(const char *url)
{
    if(!url)
        return 0;

    return hash_raw(url);
}

/*
 * Add new item to hash table.
 *
 * @in url - must be previously malloc'ed
 * @return url 
 *
 * @note: caller must never free memory, hash API does it.
 *
 */
char *hash_add_item(char *url)
{
    if(!url)
        return NULL;

    int32_t hash = hash_raw(url);
    if(htable[hash])
        return NULL;

    htable[hash] = url;
    return htable[hash];
}

/*
 * Remove hash table item from given url
 *
 * @in url string
 * @return 0 - success, -1 - error
 *
 * @note memory is freed
 *
 */
int32_t hash_del_item(char *url)
{
    if(!url)
        return -1;

    int32_t hash = hash_raw(url);
    if(!htable[hash])
        return 0;

    SAFE_FREE(htable[hash]);
    return 1;
}

/*
 * Return hash table entry point
 */
char **hash_get_table(void) { return htable; }

/*
 * Cleanup whole table
 */
void hash_cleanup(void)
{
    char** table = hash_get_table();
    int i = HASHTSIZ;
    for(; i >= 0; --i, ++(*table))
        //if(*table)
            SAFE_FREE(*table);
}
