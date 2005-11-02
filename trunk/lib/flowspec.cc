// $Id$

#include "fripint.h"

flowspec::flowspec()
{
	mChannels = 0;
	mSampleSize = 0;
	mSampleRate = 0;
}

flowspec& flowspec::operator = (const flowspec &src)
{
	mChannels = src.mChannels;
	mSampleSize = src.mSampleSize;
	mSampleRate = src.mSampleRate;
	return *this;
}
