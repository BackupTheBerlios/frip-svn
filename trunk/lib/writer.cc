// $Id$

#include "writer.h"

writer::writer(const reader *r)
{
	mReader = r;
	mSampleSize = r->GetSampleSize();
}

writer::~writer()
{
}

bool writer::open(const char *fname)
{
	log("Writing a %s file '%s'.", name(), fname);
	return true;
}
