#ifndef _SCURL_C_H_
#define _SCURL_C_H_
#include "buffer.h"
#include <curl/curl.h>

typedef struct curl_slist scurl_slist;
/*
 * struct curl_slist {
 * 	char *data;
 * 	struct curl_slist *next;
 * }
 */

#define SCURL_CALL 0
#define SCURL_ALL 1

void scurl_init();
void scurl_quit();
char * scurl_err();
void scurl_set_err(const char * errmsg);
int scurl_err_l();

// The following command would just apply to the following get method
void scurl_add_header(const char * title, const char * content);
scurl_slist * scurl_get_headers();
Buffer * get(const char * url);

// The following command would return the result of the latest get call
char * scurl_get_rsp_header();

void scurl_cleanup(int type);

#endif
