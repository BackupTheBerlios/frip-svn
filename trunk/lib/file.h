// $Id$

#ifndef __frip_file_h
#define __frip_file_h

#include <stdio.h>

class file
{
	FILE *in;
public:
	file();
	~file();
	bool open(const char *fname, bool writable = false);
	void close();
	void skip(unsigned, unsigned * = NULL);
	// Reading.
	bool read_short_le(short &, unsigned * = NULL);
	bool read_short_le(unsigned short &, unsigned * = NULL);
	bool read_short_be(short &, unsigned * = NULL);
	bool read_short_be(unsigned short &, unsigned * = NULL);
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
