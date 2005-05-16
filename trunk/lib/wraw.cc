// $Id$

#include "writer.h"

wraw::wraw(const reader *r) :
	writer(r)
{
}

wraw::~wraw()
{
}

bool wraw::write(samples &smp)
{
	if (smp.size() & 1)
		return true;

	for (samples::const_iterator it = smp.begin(); it != smp.end(); ++it) {
		if (mSampleSize == 16) {
			out.write_short_be(*it);
		}
	}

	smp.clear();
	return true;
}

bool wraw::open(const char *fname)
{
	if (!out.open(fname, true))
		return false;
	return writer::open(fname);
}
