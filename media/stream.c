#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>

#include "config.h"

#include "mp_msg.h"
#include "help_mp.h"

#include "stream_internal.h"
#include "demuxer.h"

extern int verbose;
//void cache_uninit(stream_t * s);	// defined in cache2.c

extern stream_info_t stream_info_file;
static int open_f(stream_t * stream, int mode, char *filename, int *file_format);

stream_t *open_stream_full(char *filename, int mode, char **options, int *file_format)
{
	int i, j, l, r;
	stream_info_t *sinfo;
	stream_t *s;

	*file_format = DEMUXER_TYPE_UNKNOWN;

	s = new_stream(-2, -2);
	s->url = strdup(filename);
	s->flags |= mode;
	r = open_f(s, mode, filename, file_format);

	if (r != STREAM_OK) {
		free(s->url);
		free(s);
		s = NULL;
	}

	if (s->type <= -2)
		mp_msg(MSGT_OPEN, MSGL_WARN, "Warning streams need a type !!!!\n");
	if (s->flags & STREAM_SEEK && !s->seek)
		s->flags &= ~STREAM_SEEK;
	if (s->seek && !(s->flags & STREAM_SEEK))
		s->flags |= STREAM_SEEK;

	mp_msg(MSGT_OPEN, MSGL_V, "STREAM: [file] %s\n", filename);

	if (s)
		return s;

	if (r != STREAM_UNSUPORTED) {
		mp_msg(MSGT_OPEN, MSGL_ERR, "Failed to open %s\n", filename);
		return NULL;
	}

	mp_msg(MSGT_OPEN, MSGL_ERR, "No stream found to handle url %s\n", filename);
	return NULL;
}

stream_t *open_stream(char *filename, char **options, int *file_format)
{
	stream_t *stream = NULL;
	int f = -1;
	off_t len;

	// Check if playlist or unknown
	if (*file_format != DEMUXER_TYPE_PLAYLIST) {
		*file_format = DEMUXER_TYPE_UNKNOWN;
	}

	if (!filename) {
		mp_msg(MSGT_OPEN, MSGL_ERR, "NULL filename, report this bug\n");
		return NULL;
	}

	return open_stream_full(filename, STREAM_READ, options, file_format);
}

//=================== STREAMER =========================

int stream_fill_buffer(stream_t * s)
{
	int len;

	if ( /*s->fp == NULL || */ s->eof) {
		s->buf_pos = s->buf_len = 0;
		return 0;
	}

	switch (s->type) {
	case STREAMTYPE_STREAM:
		printf("read stream type\n");
		len = read(s->fd, s->buffer, STREAM_BUFFER_SIZE);
		break;
	case STREAMTYPE_DS:
		printf("read ds type\n");
		len = demux_read_data((demux_stream_t *) s->priv, s->buffer, STREAM_BUFFER_SIZE);
		break;
	default:
		len = s->fill_buffer ? s->fill_buffer(s, s->buffer, STREAM_BUFFER_SIZE) : 0;
	}

	if (len <= 0) {
		s->eof = 1;
		s->buf_pos = s->buf_len = 0;
		return 0;
	}

	s->buf_pos = 0;
	s->buf_len = len;
	s->pos += len;
	// printf("[%d]",len);fflush(stdout);
	return len;
}

int stream_seek_long(stream_t * s, off_t pos)
{
	off_t newpos = 0;
	// if(verbose>=3) printf("seek_long to 0x%X\n",(unsigned int)pos);

	s->buf_pos = s->buf_len = 0;

	switch (s->type) {
	case STREAMTYPE_STREAM:
		newpos = pos & (~(STREAM_BUFFER_SIZE - 1));
		break;
	default:
		// Round on sector size
		if (s->sector_size)
			newpos = (pos / s->sector_size) * s->sector_size;
		else {		// Otherwise on the buffer size
			newpos = pos & (~(STREAM_BUFFER_SIZE - 1));
			break;
		}
		break;
	}

	if (verbose >= 3) {
		printf("s->pos=%X  newpos=%X  new_bufpos=%X  buflen=%X  \n", (unsigned int) s->pos, newpos, pos, s->buf_len);
	}

	pos -= newpos;

	if (newpos == 0 || newpos != s->pos) {
		switch (s->type) {
		case STREAMTYPE_STREAM:
			//s->pos=newpos; // real seek
			// Some streaming protocol allow to seek backward and forward
			// A function call that return -1 can tell that the protocol
			// doesn't support seeking.
			if (newpos < s->pos) {
				mp_msg(MSGT_STREAM, MSGL_INFO, "Cannot seek backward in linear streams!\n");
				return 1;
			}
			while (s->pos < newpos) {
				if (stream_fill_buffer(s) <= 0)
					break;	// EOF
			}
			break;
		default:
			// This should at the beginning as soon as all streams are converted
			if (!s->seek)
				return 0;
			// Now seek
			if (!s->seek(s, newpos)) {
				mp_msg(MSGT_STREAM, MSGL_ERR, "Seek failed\n");
				return 0;
			}
		}
		//   putchar('.');fflush(stdout);
		//} else {
		//   putchar('%');fflush(stdout);
	}

	stream_fill_buffer(s);
	if (pos >= 0 && pos <= s->buf_len) {
		s->buf_pos = pos;	// byte position in sector
		return 1;
	}
	//  if(pos==s->buf_len) printf("XXX Seek to last byte of file -> EOF\n");

	mp_msg(MSGT_STREAM, MSGL_V, "stream_seek: WARNING! Can't seek to 0x%X !\n", (pos + newpos));
	return 0;
}

void stream_reset(stream_t * s)
{
	if (s->eof) {
		s->pos = 0;	//ftell(f);
		// s->buf_pos=s->buf_len=0;
		s->eof = 0;
	}
	//stream_seek(s,0);
}

stream_t *new_memory_stream(unsigned char *data, int len)
{
	stream_t *s = malloc(sizeof(stream_t) + len);
	memset(s, 0, sizeof(stream_t));
	s->fd = -1;
	s->fp = NULL;
	s->type = STREAMTYPE_MEMORY;
	s->buf_pos = 0;
	s->buf_len = len;
	s->start_pos = 0;
	s->end_pos = len;
	stream_reset(s);
	s->pos = len;
	memcpy(s->buffer, data, len);
	return s;
}

stream_t *new_stream(int fd, int type)
{
	stream_t *s = malloc(sizeof(stream_t));
	if (s == NULL)
		return NULL;
	memset(s, 0, sizeof(stream_t));

	s->fd = fd;
	s->fp = NULL;
	s->type = type;
	s->buf_pos = s->buf_len = 0;
	s->start_pos = s->end_pos = 0;
	s->priv = NULL;
	s->url = NULL;
	s->cache_pid = 0;
	stream_reset(s);
	return s;
}

void free_stream(stream_t * s)
{
	if (s->close)
		s->close(s);
	if (s->fp)
		fclose(s->fp);
	if (s->url)
		free(s->url);
	free(s);
}

stream_t *new_ds_stream(demux_stream_t * ds)
{
	stream_t *s = new_stream(-1, STREAMTYPE_DS);
	s->priv = ds;
	return s;
}

static int fill_buffer(stream_t * s, char *buffer, int max_len)
{
	int r;
	r = fread(buffer, 1, max_len, s->fp);
	return (r <= 0) ? -1 : r;
}

static int seek(stream_t * s, off_t newpos)
{
	s->pos = newpos;
	if (fseek(s->fp, s->pos, SEEK_SET) < 0) {
		s->eof = 1;
		return 0;
	}
	return 1;
}

static int seek_forward(stream_t * s, off_t newpos)
{
	if (newpos < s->pos) {
		mp_msg(MSGT_STREAM, MSGL_INFO, "Cannot seek backward in linear streams!\n");
		return 0;
	}
	while (s->pos < newpos) {
		int len = s->fill_buffer(s, s->buffer, STREAM_BUFFER_SIZE);
		if (len <= 0) {
			s->eof = 1;
			s->buf_pos = s->buf_len = 0;
			break;
		}		// EOF
		s->buf_pos = 0;
		s->buf_len = len;
		s->pos += len;
	}
	return 1;
}

static int open_f(stream_t * stream, int mode, char *filename, int *file_format)
{
	FILE *f;
	unsigned int len;

	if (mode != STREAM_READ) {
		mp_msg(MSGT_OPEN, MSGL_ERR, "[file] Unknown open mode %d\n", mode);
		return STREAM_UNSUPORTED;
	}

	if (!filename) {
		mp_msg(MSGT_OPEN, MSGL_ERR, "[file] No filename\n");
		return STREAM_ERROR;
	}

#ifdef WIN32
	f = fopen(filename, "rb");
#else
	f = fopen(filename, "r");
#endif
	if (!f) {
		mp_msg(MSGT_OPEN, MSGL_ERR, MSGTR_FileNotFound, filename);
		return STREAM_ERROR;
	}

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (len == -1) {
		stream->seek = seek_forward;
		stream->type = STREAMTYPE_STREAM;	// Must be move to STREAMTYPE_FILE
		stream->flags |= STREAM_SEEK_FW;
	} else if (len >= 0) {
		stream->seek = seek;
		stream->end_pos = len;
		stream->type = STREAMTYPE_FILE;
		stream->flags |= STREAM_SEEK;
	}

	mp_msg(MSGT_OPEN, MSGL_V, "[file] File size is %u bytes\n", (unsigned int) len);

	stream->fp = f;
	stream->fill_buffer = fill_buffer;

	return STREAM_OK;
}
