#ifndef __STREAM_H
#define __STREAM_H

#include <string.h>
#include <inttypes.h>
#include <sys/types.h>

#define MAX_STREAM_PROTOCOLS 10
#define STREAM_BUFFER_SIZE 10240

struct stream_st;
typedef struct stream_info_st {
	const char *info;
	const char *name;
	const char *author;
	const char *comment;
	/// mode isn't used atm (ie always READ) but it shouldn't be ignored
	/// opts is at least in it's defaults settings and may have been
	/// altered by url parsing if enabled and the options string parsing.
	int (*open) (struct stream_st * st, int mode, void *opts, int *file_format);
	char *protocols[MAX_STREAM_PROTOCOLS];
	void *opts;
	int opts_url;		/* If this is 1 we will parse the url as an option string
				 * too. Otherwise options are only parsed from the
				 * options string given to open_stream_plugin */
} stream_info_t;

typedef struct stream_st {
	// Read
	int (*fill_buffer) (struct stream_st * s, char *buffer, int max_len);
	// Seek
	int (*seek) (struct stream_st * s, off_t pos);
	// Close
	void (*close) (struct stream_st * s);

	int fd;			// file descriptor, see man open(2)
	FILE *fp;
	int type;		// see STREAMTYPE_*
	int flags;
	int sector_size;	// sector size (seek will be aligned on this size if non 0)
	unsigned int buf_pos, buf_len;
	off_t pos, start_pos, end_pos;
	int eof;
	unsigned int cache_pid;
	void *cache_data;
	void *priv;		// used for DVD, TV, RTSP etc
	char *url;		// strdup() of filename/url
	unsigned char buffer[STREAM_BUFFER_SIZE];
} stream_t;

stream_t *open_stream(char *filename, char **options, int *file_format);
stream_t *open_stream_full(char *filename, int mode, char **options, int *file_format);
void stream_reset(stream_t * s);
void free_stream(stream_t * s);
stream_t *new_stream(int fd, int type);
stream_t *new_memory_stream(unsigned char *data, int len);

#endif				// __STREAM_H
