#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <curl/curl.h>

#include "buffer.h"
#include "tcurl.h"


pthread_mutex_t is_download;

tcurl_slist *_headers = NULL;
Buffer *_rsp_header = NULL;

static int _tcurl_inited = 0;

char * tmpDir = "/tmp/";
char * _tcurl_errMsg = NULL;
int _tcurl_errCode = 0;
int _tcurl_breakpoint_upload = 0;
int _tcurl_breakpoint_upload_from = 0;
int _tcurl_breakpoint_upload_to = 0;


void tcurl_err_clean() {
	if (_tcurl_errMsg != NULL) {
		free(_tcurl_errMsg);
		_tcurl_errMsg = NULL;
	}
	_tcurl_errCode = 0;
}

char * tcurl_err() {
	return _tcurl_errMsg;
}

int tcurl_err_l() {
	return strlen(_tcurl_errMsg);
}

int tcurl_err_code() {
	return _tcurl_errCode;
}

void tcurl_set_err(const char * errmsg, int errCode) {
	static char * pre = "Get Error: ";
	static int prelength = 11;
	if (_tcurl_errMsg != NULL) {
		free(_tcurl_errMsg);
	}

	int length = strlen(errmsg);
	_tcurl_errMsg = (char *)malloc(sizeof(char) * (length + 1 + prelength));
	memcpy(_tcurl_errMsg, pre, prelength);
	memcpy(_tcurl_errMsg + prelength, errmsg, length + 1);

	_tcurl_errCode = errCode;
}

void tcurl_init() {
	_tcurl_inited = 1;
	curl_global_init(CURL_GLOBAL_ALL);
	_rsp_header = buffer_create(100);
	char * tmp = getenv("TMP");
	if (tmp != NULL) {
		int lindex = strlen(tmp) - 1;
		if ((tmp[lindex] != '/') && (tmp[lindex] != '\\')) {
			tmpDir = (char *)malloc(sizeof(char) * (lindex + 3));
			memcpy(tmpDir, tmp, lindex+1);
			tmpDir[lindex + 1] = '/';
			tmpDir[lindex + 2] = '\0';
		}  else {
			tmpDir = tmp;
		}
	}
}

void tcurl_quit() {
	curl_global_cleanup();
}

size_t dataProcess(void *buffer, size_t chunkSize, size_t chunkNum, void * userData) {
	// if we don't use curl_easy_cleanup(.., CURLOPT_WRITEDATA, ..)
	// userData should be a null ptr
	Buffer *buf = (Buffer *)userData;
	int data_size = buffer_write(buf, (char *)buffer, chunkSize * chunkNum);
	return data_size;
}

void tcurl_cleanup(int type) {
	tcurl_err_clean();
	switch (type) {
		case TCURL_ALL:
			buffer_reset(_rsp_header, 0);
		case TCURL_CALL:
			curl_slist_free_all(_headers);
			_headers = NULL;
			_tcurl_breakpoint_upload = 0;
			_tcurl_breakpoint_upload_from = 0;
			_tcurl_breakpoint_upload_to = 0;
			break;
		case TCURL_RESULT:
			buffer_reset(_rsp_header, 0);
			break;
	}
}

void tcurl_wait() {
	pthread_mutex_lock(&is_download);
	pthread_mutex_unlock(&is_download);
}


void tcurl_add_header(const char * title, const char * content) {
	static char * mid = ": ";
	static int midlength = 2;
	int l_ti = strlen(title), l_ct = strlen(content);
	int length = l_ti + midlength + l_ct;

	char * head = (char *)malloc(sizeof(char) * (length + 1));
	memcpy(head, title, l_ti);
	memcpy(head + l_ti, mid, midlength);
	memcpy(head + l_ti + midlength, content, l_ct);
	head[length - 1] = '\0';

	_headers = curl_slist_append(_headers, head);
}

tcurl_slist * tcurl_get_headers() {
	return _headers;
}

Buffer * tcurl_get_rsp_header() {
	return _rsp_header;
}

CURL * tcurl_request_init(const char * url) {
	// check for sure every thing prepared
	if (_tcurl_inited == 0) {
		tcurl_init();
		_tcurl_inited = 1;
	}

	CURL * curl = curl_easy_init();

	tcurl_cleanup(TCURL_RESULT);

	if (_headers != NULL) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers);
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &dataProcess);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, _rsp_header);

	// Allows to follow redirect url
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 5);

	return curl;
}

int tcurl_request_perform(CURL * curl) {
	CURLcode res;
	res = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	tcurl_cleanup(TCURL_CALL);

	if (res != CURLE_OK) {
		tcurl_set_err(curl_easy_strerror(res), TCURL_ERR_CURL);
		return 1;
	}
	return 0;
}

Buffer * tcurl_head(const char * url) {
	if (pthread_mutex_trylock(&is_download) != 0) {
		// That means there's another process is downloading
		tcurl_set_err("another process is downloading", TCURL_ERR_MULTIPROCESS);
		return NULL;
	}

	CURL * curl = tcurl_request_init(url);

	curl_easy_setopt(curl, CURLOPT_NOBODY,1L);

	int err = tcurl_request_perform(curl);
	pthread_mutex_unlock(&is_download);
	if (err != 0) {
		return NULL;
	}
	return _rsp_header;
}

Buffer * tcurl_get(const char * url) {
	if (pthread_mutex_trylock(&is_download) != 0) {
		// That means there's another process is downloading
		tcurl_set_err("another process is downloading", TCURL_ERR_MULTIPROCESS);
		return NULL;
	}
	CURL * curl = tcurl_request_init(url);

	Buffer * buf = buffer_create(500);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dataProcess);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);

	int err = tcurl_request_perform(curl);
	pthread_mutex_unlock(&is_download);
	if (err != 0) {
		return NULL;
	}
	return buf;
}
