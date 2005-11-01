// $Id$

#include <string.h>
#include "all.h"

downmix::downmix(const char *arg) :
	filter(arg)
{
	if (strcmp(arg, "none") == 0)
		mMixType = mtNone;
	else if (strcmp(arg, "left") == 0)
		mMixType = mtLeft;
	else if (strcmp(arg, "right") == 0)
		mMixType = mtRight;
	else if (strcmp(arg, "both") == 0)
		mMixType = mtBoth;
	else
		throw("Unsupported downmix parameter, can be: none, left, right, both.");
}

downmix::~downmix()
{
}

void downmix::process(samples &smp, const flowspec &fs)
{
}
