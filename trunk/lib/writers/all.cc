// $Id$

#include "all.h"

writer::writer()
{
	mQuality = 5;
}

writer::~writer()
{
}

void writer::set_quality(int q)
{
	if (q > 9)
		q = 9;
	if (q < 0)
		q = 0;
	mQuality = q;
}

werr::werr(const std::string &msg)
{
	mText = "write error: " + msg;
}
