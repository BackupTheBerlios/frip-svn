// $Id$

#include "writer.h"

wraw::wraw(const reader *r) :
	writer(r)
{
}

wraw::~wraw()
{
}

void wraw::write(samples &smp)
{
	if (smp.size() & 1)
		return;

	for (samples::const_iterator it = smp.begin(); it != smp.end(); ++it) {
		if (mSampleSize == 16) {
			out.write_short_be(*it);
		}
	}

	smp.clear();
}

bool wraw::open(const char *fname)
{
	if (!out.open(fname, true))
		return false;
	return writer::open(fname);
}
