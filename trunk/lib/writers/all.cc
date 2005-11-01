// $Id$

#include "all.h"

writer::writer(const reader *r)
{
	mReader = r;
	mQuality = 5;
	mMixType = mtNone;
}

writer::~writer()
{
}

bool writer::open(const char *fname)
{
	mSampleSize = mReader->GetSampleSize();

	log("Writing a %s file '%s'.", name(), fname);
	return true;
}

void writer::set_quality(int q)
{
	if (q > 9)
		q = 9;
	if (q < 0)
		q = 0;
	mQuality = q;
}

void writer::set_mix_type(mixtype_t mt)
{
	mMixType = mt;
}
