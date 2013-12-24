/*
 * Author: Carlos Carvalho
 *
 */

#pragma once

#include <stdlib.h>
#include <pthread.h>

#include <string.h>


//static pthread_mutex_t parser_mutex = PTHREAD_MUTEX_INITIALIZER;
//static pthread_mutex_t tester_mutex = PTHREAD_MUTEX_INITIALIZER;

static int _is_valid_url(char *buffer, size_t len);

const char *matchStrings[] = {
    "sql error",
    "supplied argument is not a valid MySQL",
    "Unable to jump to row",
    "You have an error in your SQL",
    NULL
};

const char *invalidStrings[] = {
    "inurl",
    "allinur",
    "allinurl",
    "&amp",
    "visible_link",
    "\\x",
    "\\",
    ";",
    "class\\x",
    "\n",
    "\"",
    "</span>",
    "href=\"",
    "</div>",
    NULL
};

static inline int _is_hex(int x)
{
	return	(x >= '0' && x <= '9') ||
		(x >= 'a' && x <= 'f') ||
		(x >= 'A' && x <= 'F');
}
