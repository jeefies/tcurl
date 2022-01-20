#ifndef _BUFFER_C_H_
#define _BUFFER_C_H_

struct Buffer {
	char * buf;
	int length;
	int seek;
};
typedef struct Buffer Buffer;

Buffer * buffer_create(int);
int buffer_write(Buffer*, const char*, int);
int buffer_writestr(Buffer*, const char*);
int buffer_tofile(Buffer*, const char*, const char*);
int buffer_reset(Buffer*, int);
int buffer_close(Buffer*);

#endif
