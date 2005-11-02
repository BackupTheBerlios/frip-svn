// $Id$

#ifndef __frip_file_h
#define __frip_file_h

#include <stdio.h>

#define IFF_ID_FORM 0x464F524D /* "FORM" */
#define IFF_ID_AIFF 0x41494646 /* "AIFF" */
#define IFF_ID_AIFC 0x41494643 /* "AIFC" */
#define IFF_ID_COMM 0x434f4d4d /* "COMM" */
#define IFF_ID_SSND 0x53534e44 /* "SSND" */
#define IFF_ID_MPEG 0x4d504547 /* "MPEG" */
#define IFF_ID_FVER 0x46564552 /* "FVER" */

#define IFF_ID_NONE 0x4e4f4e45 /* "NONE" */ /* AIFF-C data format */
#define IFF_ID_2CBE 0x74776f73 /* "twos" */ /* AIFF-C data format */
#define IFF_ID_2CLE 0x736f7774 /* "sowt" */ /* AIFF-C data format */

#define WAV_ID_RIFF 0x52494646
#define WAV_ID_WAVE 0x57415645
#define WAV_ID_FMT  0x666D7420
#define WAV_ID_DATA 0x64617461

class file
{
	FILE *in;
	bool read_short_be_raw(short &, unsigned * = NULL);
	bool read_short_le_raw(short &, unsigned * = NULL);
public:
	file();
	~file();
	bool open(const char *fname, bool writable = false);
	bool isopen() const { return (in != NULL); }
	void close();
	// File pointer navigation.
	void skip(unsigned, unsigned * = NULL);
	size_t tell() const;
	void rewind(size_t);
	// Reading.
	template <class T> bool read_short_be(T &to, unsigned *x = NULL) {
		short tmp;
		bool rc = read_short_be_raw(tmp, x);
		to = static_cast<T>(tmp);
		return rc;
	}
	template <class T> bool read_short_le(T &to, unsigned *x = NULL) {
		short tmp;
		bool rc = read_short_le_raw(tmp, x);
		to = static_cast<T>(tmp);
		return rc;
	}
	bool read_int_le(int &, unsigned * = NULL);
	bool read_int_be(int &, unsigned * = NULL);
	bool read_int_le(unsigned &, unsigned * = NULL);
	bool read_int_be(unsigned &, unsigned * = NULL);
	bool read_float_be(double &, unsigned * = NULL);
	bool read_raw(void *oBuf, unsigned &ioSize);
	// Writing.
	bool write_short_be(short);
	bool write_short_le(short);
	bool write_int_le(int);
	bool write_int_le(unsigned);
	bool write_int_be(int);
	bool write_float_be(double);
	bool write(const void *, unsigned);
	bool write(const char *);
};

#endif
