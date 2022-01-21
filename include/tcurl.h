#ifndef _TCURL_C_H_
#define _TCURL_C_H_
#include "buffer.h"
#include <curl/curl.h>

typedef struct curl_slist tcurl_slist;
/*
 * struct curl_slist {
 * 	char *data;
 * 	struct curl_slist *next;
 * }
 */

#define TCURL_CALL 0
#define TCURL_ALL 1
#define TCURL_RESULT 2

void tcurl_init();
void tcurl_quit();
char * tcurl_err();
void tcurl_set_err(const char * errmsg);
int tcurl_err_l();

// The following command would just apply to the following get method

void tcurl_add_header(const char * title, const char * content);
tcurl_slist * tcurl_get_headers();
Buffer * tcurl_get(const char * url);
Buffer * tcurl_head(const char * url);

// The following command would return the result of the latest get call
Buffer * tcurl_get_rsp_header();

void tcurl_cleanup(int type);

#endif
