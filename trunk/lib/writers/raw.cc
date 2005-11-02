// $Id$

#include "all.h"

wraw::wraw(const char *fname)
{
	mFileName = fname;
}

wraw::~wraw()
{
}

void wraw::write(const flowspec &fs, samples &smp)
{
	size_t todo;

	if (mFlowSpec.isnull())
		mFlowSpec = fs;
	else if (mFlowSpec != fs)
		throw werr("flow specification changed; not yet supported");

	if (!mOut.isopen() && !mOut.open(mFileName.c_str(), true))
		throw werr("could not open output for writing");

	if ((todo = (smp.size() & ~1)) == 0)
		return;

	for (samples::const_iterator it = smp.begin(), lim = smp.begin() + todo; it != lim; ++it) {
		if (mFlowSpec.mSampleSize == 16)
			mOut.write_short_be(*it);
	}

	smp.erase(smp.begin(), smp.begin() + todo);
}
