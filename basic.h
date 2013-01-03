#ifndef _BASIC_H_
#define _BASIC_H_

#include <stdio.h>
#include <stdlib.h>

static inline FILE *fopen_r(const char *path, const char *mode)
{
	FILE *fp;

	if ((fp = fopen(path, mode)) == NULL) {
		printf("Open %s with %s mode failed.\n", path, mode);
		exit(-1);
	} 
	return fp;
}

static inline void *MALLOC(size_t size)
{
	void *addr;

	if ((addr = malloc(size)) == NULL) {
		printf("malloc failed.\n");
		exit(-1);
	}
	return addr;
}

#define FREE(PTR) \
	do \
	{ \
		free(PTR);	\
		PTR = NULL; \
	}while(0)


static inline size_t fread_r(void *ptr, size_t size, 
							 size_t nmemb, FILE *stream)
{
	if (fread(ptr, size, nmemb, stream) != nmemb) {
		printf("fread failed.\n");
		exit(-1);
	}
	return 0;
}
static inline size_t fwrite_r(const void *ptr, size_t size,
							  size_t nmemb, FILE *stream)
{
	if (fwrite(ptr, size, nmemb, stream) != nmemb) {
		printf("fwrite failed.\n");
		exit(-1);
	}
	return 0;
}

static inline void skip_bytes(FILE *fp, int skip_size)
{
	if (fseek(fp, skip_size, SEEK_CUR) == -1) {
		printf("fseek failed.\n");
		exit(-1);
	}
}

#endif	//_BASIC_H_
