// $Id$

#include "writer.h"

writer::writer(const reader *r)
{
	mReader = r;
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
