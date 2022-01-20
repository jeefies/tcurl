#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "buffer.h"
#include "tcurl.h"


scurl_slist *_headers = NULL;
static Buffer *_rsp_header = NULL;

static int _scurl_inited = 0;
char * _scurl_errMsg = NULL;

char * scurl_err() {
	return _scurl_errMsg;
}

int scurl_err_l() {
	return strlen(_scurl_errMsg);
}

void scurl_set_err(const char * errmsg) {
	static char * pre = "Get Error: ";
	static int prelength = 11;
	if (_scurl_errMsg != NULL) {
		free(_scurl_errMsg);
	}

	int length = strlen(errmsg);
	_scurl_errMsg = (char *)malloc(sizeof(char) * (length + 1 + prelength));
	memcpy(_scurl_errMsg, pre, prelength);
	memcpy(_scurl_errMsg + prelength, errmsg, length + 1);
}

void scurl_init() {
	curl_global_init(CURL_GLOBAL_ALL);
	_rsp_header = buffer_create(100);
}

void scurl_quit() {
	curl_global_cleanup();
}

size_t dataProcess(void *buffer, size_t chunkSize, size_t chunkNum, void * userData) {
	// if we don't use curl_easy_cleanup(.., CURLOPT_WRITEDATA, ..)
	// userData should be a null ptr
	Buffer *buf = (Buffer *)userData;
	int data_size = buffer_write(buf, (char *)buffer, chunkSize * chunkNum);
	return data_size;
}

void scurl_cleanup(int type) {
	switch (type) {
		case SCURL_CALL:
			curl_slist_free_all(_headers);
			break;
		case SCURL_ALL:
			curl_slist_free_all(_headers);
			buffer_reset(_rsp_header, 0);
			break;
	}
}

void scurl_add_header(const char * title, const char * content) {
	static char * mid = ": ";
	static int midlength = 2;
	int l_ti = strlen(title), l_ct = strlen(content);
	int length = l_ti + midlength + l_ct;

	char * head = (char *)malloc(sizeof(char) * (length + 1));
	memcpy(head, title, l_ti);
	memcpy(head + l_ti, mid, midlength);
	memcpy(head + l_ti + midlength, content, l_ct);
	head[length - 1] = '\0';
	printf("Header add: %s\n", head);

	_headers = curl_slist_append(_headers, head);
}

scurl_slist * scurl_get_headers() {
	return _headers;
}

char * scurl_get_rsp_header() {
	return _rsp_header->buf;
}

Buffer * get(const char * url) {
	CURL *curl = curl_easy_init();

	if (_scurl_inited == 0) {
		scurl_init();
		_scurl_inited = 1;
	}
	Buffer *buf = buffer_create(500);

	if (_headers != NULL) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers);
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dataProcess);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &dataProcess);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, _rsp_header);

	CURLcode res;
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		scurl_set_err(curl_easy_strerror(res));
		return NULL;
	}

	curl_easy_cleanup(curl);
	scurl_cleanup(SCURL_CALL);
	return buf;
}
