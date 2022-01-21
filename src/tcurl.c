#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "buffer.h"
#include "tcurl.h"


tcurl_slist *_headers = NULL;
Buffer *_rsp_header = NULL;

static int _tcurl_inited = 0;
char * _tcurl_errMsg = NULL;

char * tcurl_err() {
	return _tcurl_errMsg;
}

int tcurl_err_l() {
	return strlen(_tcurl_errMsg);
}

void tcurl_set_err(const char * errmsg) {
	static char * pre = "Get Error: ";
	static int prelength = 11;
	if (_tcurl_errMsg != NULL) {
		free(_tcurl_errMsg);
	}

	int length = strlen(errmsg);
	_tcurl_errMsg = (char *)malloc(sizeof(char) * (length + 1 + prelength));
	memcpy(_tcurl_errMsg, pre, prelength);
	memcpy(_tcurl_errMsg + prelength, errmsg, length + 1);
}

void tcurl_init() {
	curl_global_init(CURL_GLOBAL_ALL);
	_rsp_header = buffer_create(100);
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
	switch (type) {
		case TCURL_CALL:
			curl_slist_free_all(_headers);
			_headers = NULL;
			break;
		case TCURL_ALL:
			curl_slist_free_all(_headers);
			_headers = NULL;
			buffer_reset(_rsp_header, 0);
			break;
		case TCURL_RESULT:
			buffer_reset(_rsp_header, 0);
			break;
	}
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

CURL * tcurl_requets_init(const char * url) {
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

int tcurl_requets_perform(CURL * curl) {
	CURLcode res;
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		tcurl_set_err(curl_easy_strerror(res));
		return 1;
	}

	curl_easy_cleanup(curl);
	tcurl_cleanup(TCURL_CALL);

	return 0;
}

Buffer * tcurl_head(const char * url) {
	CURL * curl = tcurl_requets_init(url);

	curl_easy_setopt(curl, CURLOPT_NOBODY,1L);

	int err = tcurl_requets_perform(curl);
	if (err != 0) {
		return NULL;
	}
	return _rsp_header;
}

Buffer * tcurl_get(const char * url) {
	CURL * curl = tcurl_requets_init(url);

	Buffer * buf = buffer_create(500);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dataProcess);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);

	int err = tcurl_requets_perform(curl);
	if (err != 0) {
		return NULL;
	}
	return buf;
}
