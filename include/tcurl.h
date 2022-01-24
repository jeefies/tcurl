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

#define TCURL_ERR_CURL 1
#define TCURL_ERR_MULTIPROCESS 2
#define TCURL_ERR_FILEOPEN 3
#define TCURL_ERR_THREAD 4

#define TCURL_NOTFINISH 2
#define TCURL_ERROR 1
#define TCURL_OK 0

void tcurl_init();
void tcurl_quit();
char * tcurl_err();
void tcurl_set_err(const char * errmsg, int errCode);
int tcurl_err_l();
int tcurl_err_code();
size_t dataProcess(void *buffer, size_t chunkSize, size_t chunkNum, void * userData);


// The following command would just apply to the following get method

void tcurl_add_header(const char * title, const char * content);
tcurl_slist * tcurl_get_headers();
Buffer * tcurl_get(const char * url);
Buffer * tcurl_head(const char * url);

// The following command would return the result of the latest get call
Buffer * tcurl_get_rsp_header();

void tcurl_cleanup(int type);
CURL * tcurl_request_init(const char * url);

// ATCurl
void tcurl_wait();
void tcurl_acleanup(int type);
// The filename to save the content
int tcurl_setfn(const char * fn);
// Or can use this function to set to an opened file
void tcurl_setfp(FILE * fp);
// If you set fp yourself, it should be NULL
// if don't set anything, it would automatically generate a filename by the url
char * tcurl_getfn();

size_t abodyDataProcess(void *buffer, size_t chunkSize, size_t chunkNum, void * userData);

// Buffer is for the response header information
Buffer * tcurl_aget(const char * url);
// Get current downloaded data size (by bytes)
int tcurl_get_size();
// Check whether the download process is finished
// return TCURL_NOTFINISH if not finished
// return TCURL_ERROR if there's some thing wrong
// return TCURL_OK if everything runs well
int tcurl_afinish();
// block the program until get finish
// return as tcurl_afinish do
int tcurl_afinish_wait();

#endif
