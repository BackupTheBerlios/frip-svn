// $Id$

#include "reader.h"

reader::reader(frip_callback cb)
{
	mReady = false;
	mCallBack = cb;
	mSamplesTotal = 0;
	mSamplesLeft = 0;
	mLastReport = 0;
}

reader::~reader()
{
}

bool reader::open(const char *fname)
{
	mLastReport = 0;
	return in.open(fname);
}

void reader::UpdateStatus()
{
	if (mCallBack != NULL) {
		unsigned one = mSamplesTotal / 100;
		unsigned now = (mSamplesTotal - mSamplesLeft) / one;

		if (now != mLastReport) {
			mLastReport = now;
			mCallBack(now);
		}
	}
}
