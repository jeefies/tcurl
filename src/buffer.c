#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"

Buffer * buffer_create(int init_length) {
	Buffer * bf = (Buffer *)malloc(sizeof(Buffer));

	bf->buf = (char *)malloc(sizeof(char) * init_length);
	bf->length = init_length;
	bf->seek = 0;

	return bf;
}

int buffer_write(Buffer *bf, const char * data, int length) {
	int should_length = bf->seek + length;
	
	if (bf->length < should_length) {
		bf->length = should_length;
		bf->buf = (char *)realloc(bf->buf, sizeof(char) * should_length);
	}

	char * startp = bf->buf + bf->seek;
	memcpy(startp, data, length);

	bf->seek += length;

	return length;
}

int buffer_writestr(Buffer *bf, const char * str) {
	return buffer_write(bf, str, strlen(str));
}

int buffer_close(Buffer *bf) {
	free(bf->buf);
	free(bf);

	return 0;
}

int buffer_tofile(Buffer *bf, const char* fn, const char *method) {
	FILE * fp = NULL;
	if ((fp = fopen(fn, method)) == NULL) {
		perror("Error:");
		return 1;
	}

	fwrite(bf->buf, bf->length, 1, fp);
	fclose(fp);

	return 0;
}

int buffer_reset(Buffer *bf, int init_length) {
	bf->seek = 0;
	bf->length = init_length;
	free(bf->buf);
	bf->buf = (char *)malloc(sizeof(char) * init_length);
}
