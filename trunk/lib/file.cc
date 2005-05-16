// $Id$

#include <math.h>
#include <string.h>
#include "file.h"

#define UnsignedToFloat(u) (((double)((long)((u) - 2147483647L - 1))) + 2147483648.0)

file::file()
{
	in = NULL;
}

file::~file()
{
	close();
}

bool file::open(const char *fname, bool writable)
{
	close();
	in = fopen(fname, writable ? "wb" : "rb");
	return in != NULL;
}

void file::close()
{
	if (in != NULL) {
		fclose(in);
		in = NULL;
	}
}

void file::skip(unsigned count, unsigned *sub)
{
	if (sub != NULL)
		*sub -= count;
	fseek(in, count, SEEK_CUR);
}

bool file::read_short_le(short &value, unsigned *sum)
{
	unsigned char tmp[2];
	if (fread(tmp, 1, 2, in) != 2)
		return false;
	if (sum != NULL)
		*sum -= 2;
	value = static_cast<short>(tmp[1] << 8 | tmp[0]);
	return true;
}

bool file::read_short_le(unsigned short &value, unsigned *sum)
{
	unsigned char tmp[2];
	if (fread(tmp, 1, 2, in) != 2)
		return false;
	if (sum != NULL)
		*sum -= 2;
	value = static_cast<unsigned short>(tmp[1] << 8 | tmp[0]);
	return true;
}

bool file::read_short_be(unsigned short &sample, unsigned *sub)
{
	unsigned char tmp[2];
	if (fread(tmp, 1, 2, in) != 2)
		return false;
	if (sub != NULL)
		*sub -= 2;
	sample = static_cast<unsigned short>((tmp[0] << 8) | tmp[1]);
	return true;
}

bool file::read_short_be(short &sample, unsigned *sub)
{
	unsigned char tmp[2];
	if (fread(tmp, 1, 2, in) != 2)
		return false;
	if (sub != NULL)
		*sub -= 2;
	sample = static_cast<short>((tmp[0] << 8) | tmp[1]);
	return true;
}

bool file::read_int_le(unsigned &sample, unsigned *sub)
{
	unsigned char tmp[4];
	if (fread(tmp, 1, 4, in) != 4)
		return false;
	if (sub != NULL)
		*sub -= 4;
	sample = static_cast<unsigned>((tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0]);
	return true;
}

bool file::read_int_be(unsigned &sample, unsigned *sub)
{
	unsigned char tmp[4];
	if (fread(tmp, 1, 4, in) != 4)
		return false;
	if (sub != NULL)
		*sub -= 4;
	sample = static_cast<unsigned>((tmp[0] << 24) | (tmp[1] << 16) | (tmp[2] << 8) | tmp[3]);
	return true;
}

bool file::read_int_le(int &sample, unsigned *sub)
{
	unsigned char tmp[4];
	if (fread(tmp, 1, 4, in) != 4)
		return false;
	if (sub != NULL)
		*sub -= 4;
	sample = static_cast<int>((tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0]);
	return true;
}

bool file::read_int_be(int &sample, unsigned *sub)
{
	unsigned char tmp[4];
	if (fread(tmp, 1, 4, in) != 4)
		return false;
	if (sub != NULL)
		*sub -= 4;
	sample = static_cast<int>((tmp[0] << 24) | (tmp[1] << 16) | (tmp[2] << 8) | tmp[3]);
	return true;
}

bool file::read_float_be(double &value, unsigned *sub)
{
	long expon;
	char bytes[10];
	unsigned long hiMant, loMant;

	if (fread(bytes, 10, 1, in) != 1)
		return false;

	if (sub != NULL)
		*sub -= 10;

	expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
	hiMant	=	((unsigned long)(bytes[2] & 0xFF) << 24)
			|	((unsigned long)(bytes[3] & 0xFF) << 16)
			|	((unsigned long)(bytes[4] & 0xFF) << 8)
			|	((unsigned long)(bytes[5] & 0xFF));
	loMant	=	((unsigned long)(bytes[6] & 0xFF) << 24)
			|	((unsigned long)(bytes[7] & 0xFF) << 16)
			|	((unsigned long)(bytes[8] & 0xFF) << 8)
			|	((unsigned long)(bytes[9] & 0xFF));

        // This case should also be called if the number is below the smallest
	// positive double variable
	if (expon == 0 && hiMant == 0 && loMant == 0) {
		value = 0;
	} else {
	        // This case should also be called if the number is too large to fit into 
		// a double variable
	    
		if (expon == 0x7FFF) { // Infinity or NaN
			value = HUGE_VAL;
		}
		else {
			expon -= 16383;
			value  = ldexp(UnsignedToFloat(hiMant), (int) (expon -= 31));
			value += ldexp(UnsignedToFloat(loMant), (int) (expon -= 32));
		}
	}

	if (bytes[0] & 0x80)
		value = -value;

	return true;
}

bool file::write_short_be(short s)
{
	unsigned char tmp[2];
	tmp[1] = s & 0xFF;
	tmp[0] = (s >> 8) & 0xFF;
	return fwrite(tmp, 1, 2, in) == 2;
}

bool file::write_short_le(short s)
{
	unsigned char tmp[2];
	tmp[0] = s & 0xFF;
	tmp[1] = (s >> 8) & 0xFF;
	return fwrite(tmp, 1, 2, in) == 2;
}

bool file::write_int_le(int value)
{
	unsigned char tmp[4];
	tmp[0] = value & 0xFF;
	tmp[1] = (value >> 8) & 0xFF;
	tmp[2] = (value >> 16) & 0xFF;
	tmp[3] = (value >> 24) & 0xFF;
	return fwrite(tmp, 1, 4, in) == 4;
}

bool file::write_int_le(unsigned value)
{
	unsigned char tmp[4];
	tmp[0] = value & 0xFF;
	tmp[1] = (value >> 8) & 0xFF;
	tmp[2] = (value >> 16) & 0xFF;
	tmp[3] = (value >> 24) & 0xFF;
	return fwrite(tmp, 1, 4, in) == 4;
}

bool file::write_int_be(int value)
{
	unsigned char tmp[4];
	tmp[3] = value & 0xFF;
	tmp[2] = (value >> 8) & 0xFF;
	tmp[1] = (value >> 16) & 0xFF;
	tmp[0] = (value >> 24) & 0xFF;
	return fwrite(tmp, 1, 4, in) == 4;
}

bool file::write_float_be(double value)
{
	unsigned char tmp[10] = { 0x40, 0x0E, 0xAC, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	if (value == 44100)
		return fwrite(tmp, 1, 10, in) == 10;
	else
		return false;
}

bool file::write(const void *at, unsigned size)
{
	return fwrite(at, size, 1, in) == 1;
}

bool file::write(const char *at)
{
	return write(at, strlen(at));
}
