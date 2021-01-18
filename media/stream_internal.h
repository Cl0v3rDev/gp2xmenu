#ifndef __STREAM_INTERNAL_H
#define __STREAM_INTERNAL_H

#include "stream.h"
#include "mp_msg.h"

#define STREAMTYPE_DUMMY -1	// for placeholders, when the actual reading is handled in the demuxer
#define STREAMTYPE_FILE 0	// read from seekable file
#define STREAMTYPE_STREAM 2	// same as FILE but no seeking (for net/stdin)
#define STREAMTYPE_MEMORY  4	// read data from memory area
#define STREAMTYPE_DS   8	// read from a demuxer stream

/// atm it will always use mode == STREAM_READ
/// streams that use the new api should check the mode at open
#define STREAM_READ  0
#define STREAM_WRITE 1
/// Seek flags, if not mannualy set and s->seek isn't NULL
/// STREAM_SEEK is automaticly set
#define STREAM_SEEK_BW  2
#define STREAM_SEEK_FW  4
#define STREAM_SEEK  (STREAM_SEEK_BW|STREAM_SEEK_FW)

//////////// Open return code
/// This can't open the requested protocol (used by stream wich have a
/// * protocol when they don't know the requested protocol)
#define STREAM_UNSUPORTED -1
#define STREAM_ERROR 0
#define STREAM_OK    1

#define STREAM_CTRL_RESET 0

int stream_fill_buffer(stream_t * s);
int stream_seek_long(stream_t * s, off_t pos);

#define cache_stream_fill_buffer(x) stream_fill_buffer(x)
#define cache_stream_seek_long(x,y) stream_seek_long(x,y)
#define stream_enable_cache(x,y,z,w) 1

inline static int stream_read_char(stream_t * s)
{
	return (s->buf_pos < s->buf_len) ? s->buffer[s->buf_pos++] : (cache_stream_fill_buffer(s) ? s->buffer[s->buf_pos++] : -256);
	//  if(s->buf_pos<s->buf_len) return s->buffer[s->buf_pos++];
	//  stream_fill_buffer(s);
	//  if(s->buf_pos<s->buf_len) return s->buffer[s->buf_pos++];
	//  return 0; // EOF
}

inline static unsigned int stream_read_word(stream_t * s)
{
	int x, y;
	x = stream_read_char(s);
	y = stream_read_char(s);
	return (x << 8) | y;
}

inline static unsigned int stream_read_dword(stream_t * s)
{
	unsigned int y;
	y = stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	return y;
}

#define stream_read_fourcc stream_read_dword_le

inline static unsigned int stream_read_word_le(stream_t * s)
{
	int x, y;
	x = stream_read_char(s);
	y = stream_read_char(s);
	return (y << 8) | x;
}

inline static unsigned int stream_read_dword_le(stream_t * s)
{
	unsigned int y;
	y = stream_read_char(s);
	y |= stream_read_char(s) << 8;
	y |= stream_read_char(s) << 16;
	y |= stream_read_char(s) << 24;
	return y;
}

inline static uint64_t stream_read_qword(stream_t * s)
{
	uint64_t y;
	y = stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	return y;
}

inline static uint64_t stream_read_qword_le(stream_t * s)
{
	uint64_t y;
	y = stream_read_char(s);
	y |= stream_read_char(s) << 8;
	y |= stream_read_char(s) << 16;
	y |= stream_read_char(s) << 24;
	y |= (uint64_t) stream_read_char(s) << 32;
	y |= (uint64_t) stream_read_char(s) << 40;
	y |= (uint64_t) stream_read_char(s) << 48;
	y |= (uint64_t) stream_read_char(s) << 56;
	return y;
}

inline static unsigned int stream_read_int24(stream_t * s)
{
	unsigned int y;
	y = stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	y = (y << 8) | stream_read_char(s);
	return y;
}

inline static int stream_read(stream_t * s, char *mem, int total)
{
	int len = total;
	while (len > 0) {
		int x;
		x = s->buf_len - s->buf_pos;
		if (x == 0) {
			//*
			if ((STREAM_BUFFER_SIZE < len)
			    && (s->type == STREAMTYPE_FILE)) {
				x = fread(mem, 1, len, s->fp);
				s->pos += x;
				mem += x;
				len -= x;
			}
			//*/
			if (!cache_stream_fill_buffer(s))
				return total - len;	// EOF
			x = s->buf_len - s->buf_pos;
		}
		if (s->buf_pos > s->buf_len)
			mp_msg(MSGT_DEMUX, MSGL_WARN, "stream_read: WARNING! s->buf_pos>s->buf_len\n");
		if (x > len)
			x = len;
		memcpy(mem, &s->buffer[s->buf_pos], x);
		s->buf_pos += x;
		mem += x;
		len -= x;
	}
	return total;
}

inline static int stream_eof(stream_t * s)
{
	return s->eof;
}

inline static off_t stream_tell(stream_t * s)
{
	return s->pos + s->buf_pos - s->buf_len;
}

inline static int stream_seek(stream_t * s, off_t pos)
{
	//mp_dbg(MSGT_DEMUX, MSGL_DBG3, "seek to 0x%qX\n",(long long)pos);

	if (pos < s->pos) {
		off_t x = pos - (s->pos - s->buf_len);
		if (x >= 0) {
			s->buf_pos = x;
			// putchar('*');fflush(stdout);
			return 1;
		}
	}

	return cache_stream_seek_long(s, pos);
}

inline static int stream_skip(stream_t * s, off_t len)
{
	if ((len < 0 && (s->flags & STREAM_SEEK_BW))
	    || (len > (s->buf_len - s->buf_pos)
		&& (s->flags & STREAM_SEEK_FW))) {
		// negative or big skip!
		return stream_seek(s, stream_tell(s) + len);
	}

	while (len > 0) {
		int x = s->buf_len - s->buf_pos;
		if (x == 0) {
			printf("do not seekable stream format...(type: %d, flags: %x)\n", s->type, s->flags);
			if (!cache_stream_fill_buffer(s))
				return 0;	// EOF
			x = s->buf_len - s->buf_pos;
		}
		if (x > len)
			x = len;
		//memcpy(mem,&s->buf[s->buf_pos],x);
		s->buf_pos += x;
		len -= x;
	}

	return 1;
}

extern int dvbin_param_on;

extern char *audio_stream;

#endif				// __STREAM_H
