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
	else if (strcmp(arg, "blur") == 0)
		mMixType = mtBlur;
	else
		throw("Unsupported downmix parameter, can be: none, left, right, both.");
}

downmix::~downmix()
{
}

const char * downmix::GetName() const
{
	return "downmix";
}

void downmix::process(flowspec &fs, samples &smp)
{
	samples::iterator it;
	samples::const_iterator from, lim;

	if (mMixType == mtNone)
		return;

	if (fs.mChannels == 1)
		return;

	// not sure how to deal with incomplete frames
	if (smp.size() % fs.mChannels != 0)
		return;

	from = it = smp.begin();
	lim = smp.end();

	while (from != lim) {
		switch (mMixType) {
		case mtLeft:
			*it++ = from[0];
			from += fs.mChannels;
			break;
		case mtRight:
			*it++ = from[1];
			from += fs.mChannels;
			break;
		case mtBoth:
			*it++ = (from[0] + from[1]) / 2;
			from += fs.mChannels;
			break;
		case mtNone:
		case mtBlur:
			break;
		}
	}

	smp.erase(it, smp.end());
	fs.mChannels = 1;
}
