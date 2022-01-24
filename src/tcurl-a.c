#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>

#include "buffer.h"
#include "tcurl.h"

// extern field
extern pthread_mutex_t is_download;
extern Buffer * _rsp_header;
extern char * tmpDir;

pthread_t _tcurl_perform_thread;


char * _tcurl_afn = NULL;
char _tcurl_auto_generation = 0;
FILE * _tcurl_afp = NULL;
int _tcurl_get_size = 0;

typedef struct Tcurl_AData {
	CURL * curl;
	FILE * fp;
	int head_ok;
} Tcurl_AData;

void tc_itoa(int x, char * str) {
	char s[20] = {0};
	int i = 0;
	while (x != 0) {
		s[i]  = x % 10;
		x /= 10;
		i++;
	}
	for (int j = 0;j < i; j++) {
		str[j] = s[i - j - 1];
	}
}

unsigned int hash(const char * _str) {
	char * str = (char *)_str;
	unsigned int hash = 5381;
	while(*str){
		hash += (hash << 5 ) + (*str++);
	}
	return (hash & 0x7FFFFFFF);
}

void tcurl_acleanup(int type) {
	tcurl_cleanup(type);
	switch (type) {
		case TCURL_ALL:
		case TCURL_CALL:
			if (_tcurl_afn != NULL) {
				free(_tcurl_afn);
				_tcurl_afn = NULL;
			}
			if (_tcurl_auto_generation != 0) {
				fclose(_tcurl_afp);
				_tcurl_auto_generation = 0;
			}
			_tcurl_afp = NULL;
			_tcurl_get_size = 0;
			break;
		case TCURL_RESULT:
			break;
	}
}

size_t abodyDataProcess(void *buffer, size_t chunkSize, size_t chunkNum, void * userData) {
	Tcurl_AData * adata = (Tcurl_AData *)userData;
	if (adata->head_ok == 0) {
		adata->head_ok = 1;
	}
	int l = fwrite(buffer,  chunkSize, chunkNum, adata->fp);
	_tcurl_get_size += l;
	return l;
}

void * _tcurl_aperform(void * data) {
	Tcurl_AData * adata = (Tcurl_AData *)data;

	// Configure curl download options
	CURL * curl = adata->curl;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, abodyDataProcess);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

	// True performing
	CURLcode res;
	res = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	tcurl_acleanup(TCURL_CALL);

	pthread_mutex_unlock(&is_download);
	free(data);

	if (res != CURLE_OK) {
		tcurl_set_err(curl_easy_strerror(res), TCURL_ERR_CURL);
	}
}

void tcurl_setfp(FILE *fp) {
	_tcurl_afp = fp;
}

char * tcurl_getfn() {
	return _tcurl_afn;
}

int tcurl_setfn(const char * fn) {
	_tcurl_afn = (char *)malloc(sizeof(char) * (strlen(fn) + 1));
	strcpy(_tcurl_afn, fn);

	if ((_tcurl_afp = fopen(_tcurl_afn, "wb")) == NULL) {
		tcurl_set_err("Could not open the file", TCURL_ERR_FILEOPEN);
		return TCURL_ERROR;
	}

	return TCURL_OK;
}

// TCurl aget can get the source with another thread
// But data structure is not the same as simple tcurl_get
// simple one would store data in RAM, but this would store it by files
// Use tcurl_set_file to set a specific file, or it would just store it at tmp dir
Buffer * tcurl_aget(const char * url) {
	if (pthread_mutex_trylock(&is_download) != 0) {
		// That means there's another process is downloading
		tcurl_set_err("another process is downloading", TCURL_ERR_MULTIPROCESS);
		return NULL;
	}

	CURL * curl = tcurl_request_init(url);
	// Get the write-in file pointer
	if (_tcurl_afp == NULL) {
		if (_tcurl_afn == NULL) {
			int l = strlen(tmpDir) + 16;
			_tcurl_afn = (char *)malloc(sizeof(char) * l);
			int h = hash(url);
			sprintf(_tcurl_afn, "%sTC%d", tmpDir, h);
			_tcurl_auto_generation = 1;
		}

		_tcurl_afp = fopen(_tcurl_afn, "wb");
		if (_tcurl_afp == NULL) {
			tcurl_set_err("Could not open the file", TCURL_ERR_FILEOPEN);
			return NULL;
		}
	}

	Tcurl_AData * adata = (Tcurl_AData *)malloc(sizeof(Tcurl_AData));
	adata->fp = _tcurl_afp;
	adata->curl = curl;
	adata->head_ok = 0;

	int r = pthread_create(&_tcurl_perform_thread, NULL, _tcurl_aperform, adata);
	if (r != 0) {
		tcurl_set_err("Could not create thread to get source", TCURL_ERR_THREAD);
	}

	// pthread_mutex_lock(&head_ok_lock);
	while (adata->head_ok == 0) { 
		sleep(0.1);
	}

	return _rsp_header;
}

int tcurl_get_size() {
	return _tcurl_get_size;
}

int tcurl_afinish() {
	if (pthread_mutex_trylock(&is_download) != 0) {
		return TCURL_NOTFINISH;
	}
	// ok now
	pthread_mutex_unlock(&is_download);
	if (tcurl_err_code() != 0) {
		return TCURL_ERROR;
	}

	return TCURL_OK;
}

int tcurl_afinish_wait() {
	tcurl_wait();
	return tcurl_afinish();
}
